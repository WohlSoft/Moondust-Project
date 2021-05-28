#include "palette_filter.h"
#include "ui_palette_filter.h"
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMessageBox>
#include <QCloseEvent>
#include <QFileDialog>
#include <QSettings>
#include <signal.h>
#include <common_features/app_path.h>

#include "qfile_dialogs_default_options.hpp"


PaletteFilter::PaletteFilter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PaletteFilter)
{
    ui->setupUi(this);
#ifdef _WIN32
    m_lazyfix.setProgram(ApplicationPath_x + "/PaletteFilter.exe");
#else
    m_paletteFilter.setProgram(ApplicationPath_x + "/PaletteFilter");
#endif
    m_paletteFilter.setProcessChannelMode(QProcess::MergedChannels);
    QObject::connect(&m_paletteFilter, &QProcess::readyReadStandardError,
                     this, &PaletteFilter::processOutput);
    QObject::connect(&m_paletteFilter, &QProcess::readyReadStandardOutput,
                     this, &PaletteFilter::processOutput);
    QObject::connect(&m_paletteFilter, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                     this, &PaletteFilter::processFinished);
}

PaletteFilter::~PaletteFilter()
{
    delete ui;
}

void PaletteFilter::closeEvent(QCloseEvent *e)
{
    if(m_paletteFilter.state() == QProcess::Running)
    {
        e->ignore();
        return;
    }
}

void PaletteFilter::dragEnterEvent(QDragEnterEvent *e)
{
    if(ui->whatToConvert->currentWidget() != ui->filesTab)
        return; // Don't accept drop when drop area tab isn't active
    if(e->mimeData()->hasUrls())
        e->acceptProposedAction();
}

void PaletteFilter::dropEvent(QDropEvent *e)
{
    this->raise();
    this->setFocus(Qt::ActiveWindowFocusReason);

    if(m_paletteFilter.state() == QProcess::Running)
    {
        QMessageBox::information(this,
                                 tr("Process is busy"),
                                 tr("Can't convert these files while already running conversion!"));
        return;
    }

    QStringList filesToConvert;
    for(const QUrl &url : e->mimeData()->urls())
    {
        const QString &fileName = url.toLocalFile();
        if(fileName.endsWith(".png", Qt::CaseSensitive))
            filesToConvert.push_back(fileName);
    }

    if(!filesToConvert.isEmpty())
    {
        QString paletteImg = ui->paletteImg->text();
        QStringList args;

        if(paletteImg.isEmpty())
        {
            QMessageBox::warning(this,
                                 tr("Incorrect input"),
                                 tr("You should give a PNG image with a target palette reference to start the processing."));
            return;
        }

        args << "-P" << paletteImg << "--" << filesToConvert;
        m_paletteFilter.setArguments(args);
        ui->outputLog->clear();
        m_paletteFilter.start();
        toggleBusy(true);
    }
}

void PaletteFilter::on_doMadJob_clicked()
{
    if(m_paletteFilter.state() == QProcess::Running)
    {
        m_paletteFilter.terminate();
        return;
    }

    QString episodePath = ui->inPath->text();
    QString paletteImg = ui->paletteImg->text();

    QStringList args;

    if(paletteImg.isEmpty())
    {
        QMessageBox::warning(this,
                             tr("Incorrect input"),
                             tr("You should give a PNG image with a target palette reference to start the processing."));
        return;
    }

    if(episodePath.isEmpty())
    {
        QMessageBox::warning(this,
                             tr("Incorrect input"),
                             tr("You should give an input directory to start the processing."));
        return;
    }

    QDir epDir(episodePath);
    if(!epDir.exists())
    {
        QMessageBox::warning(this,
                             tr("Incorrect input"),
                             tr("Given input directory path doesn't exist."));
        return;
    }

    args << "-P" << paletteImg;

    if(ui->flagScanSubdirs->isChecked())
        args << "-d";

    if(ui->flagRemoveOld->isChecked())
        args << "-n";

    args << "--" << episodePath;

    m_paletteFilter.setArguments(args);
    ui->outputLog->clear();
    m_paletteFilter.start();
    toggleBusy(true);
}

void PaletteFilter::on_inPathBrowse_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Select an input directory"),
                                                    ui->inPath->text(), c_dirDialogOptions);
    if(dir.isEmpty())
        return;
    ui->inPath->setText(dir);
}

void PaletteFilter::processOutput()
{
    while(1)
    {
        QByteArray out = m_paletteFilter.readLine(0);
        if(out.isEmpty())
            break;
        out.replace("\r\n", "");
        out.replace("\n", "");
        ui->outputLog->append(QString::fromLocal8Bit(out));
    }
}

void PaletteFilter::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    ui->outputLog->append(tr("Process was exited with a code %1.").arg(exitCode));

    this->raise();
    this->setFocus(Qt::ActiveWindowFocusReason);

    if(exitCode != 0 || exitStatus != QProcess::NormalExit)
    {
        if(exitCode == SIGTERM)
        {
            QMessageBox::warning(this,
                                 tr("Process was terminated"),
                                 tr("Processing has been termiated."));
        }
        else
        {
            QMessageBox::warning(this,
                                 tr("Unsuccess exit"),
                                 tr("Processing was been completed with errors. "
                                    "Please look the output log for more detailed information."));
        }
    }
    toggleBusy(false);
}

void PaletteFilter::toggleBusy(bool busy)
{
    ui->closeBox->setEnabled(!busy);
    ui->whatToConvert->setEnabled(!busy);
    if(busy)
    {
        ui->doMadJob->setText(tr("Stop"));
    } else {
        ui->doMadJob->setText(tr("Start"));
    }
}

void PaletteFilter::on_paletteImgBrowse_clicked()
{
    QString img = QFileDialog::getOpenFileName(this,
                                               tr("Select the PNG image as a palette"),
                                               ui->inPath->text(),
                                               "PNG images (*.png);;All files (*.*)",
                                               nullptr,
                                               c_dirDialogOptions);
    if(img.isEmpty())
        return;
    ui->paletteImg->setText(img);
}
