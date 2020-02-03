#include "png2gifs.h"
#include "ui_png2gifs.h"
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMessageBox>
#include <QCloseEvent>
#include <QFileDialog>
#include <QSettings>
#include <signal.h>
#include <common_features/app_path.h>

PNG2GIFs::PNG2GIFs(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PNG2GIFs)
{
    ui->setupUi(this);
#ifdef _WIN32
    m_png2gifs.setProgram(ApplicationPath_x + "/PNG2GIFs.exe");
#else
    m_png2gifs.setProgram(ApplicationPath_x + "/PNG2GIFs");
#endif
    m_png2gifs.setProcessChannelMode(QProcess::MergedChannels);
    QObject::connect(&m_png2gifs, &QProcess::readyReadStandardError,
                     this, &PNG2GIFs::processOutput);
    QObject::connect(&m_png2gifs, &QProcess::readyReadStandardOutput,
                     this, &PNG2GIFs::processOutput);
    QObject::connect(&m_png2gifs, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                     this, &PNG2GIFs::processFinished);
}

PNG2GIFs::~PNG2GIFs()
{
    delete ui;
}

void PNG2GIFs::closeEvent(QCloseEvent *e)
{
    if(m_png2gifs.state() == QProcess::Running)
    {
        e->ignore();
        return;
    }
}

void PNG2GIFs::dragEnterEvent(QDragEnterEvent *e)
{
    if(ui->whatToConvert->currentWidget() != ui->filesTab)
        return; // Don't accept drop when drop area tab isn't active
    if(e->mimeData()->hasUrls())
        e->acceptProposedAction();
}

void PNG2GIFs::dropEvent(QDropEvent *e)
{
    this->raise();
    this->setFocus(Qt::ActiveWindowFocusReason);

    if(m_png2gifs.state() == QProcess::Running)
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
        m_png2gifs.setArguments(filesToConvert);
        ui->outputLog->clear();
        m_png2gifs.start();
        toggleBusy(true);
    }
}

void PNG2GIFs::on_doMadJob_clicked()
{
    if(m_png2gifs.state() == QProcess::Running)
    {
        m_png2gifs.terminate();
        return;
    }

    QString episodePath = ui->inPath->text();

    QStringList args;

    if(episodePath.isEmpty())
    {
        QMessageBox::warning(this,
                             tr("Incorrect input"),
                             tr("You should give an input directory to start the conversion."));
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

    if(ui->flagScanSubdirs->isChecked())
        args << "-d";

    if(ui->flagRemoveOld->isChecked())
        args << "-r";

    args << "--" << episodePath;

    m_png2gifs.setArguments(args);
    ui->outputLog->clear();
    m_png2gifs.start();
    toggleBusy(true);
}

void PNG2GIFs::on_inPathBrowse_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Select an input directory"),
                                                    ui->inPath->text());
    if(dir.isEmpty())
        return;
    ui->inPath->setText(dir);
}

void PNG2GIFs::processOutput()
{
    while(1)
    {
        QByteArray out = m_png2gifs.readLine(0);
        if(out.isEmpty())
            break;
        out.replace("\r\n", "");
        out.replace("\n", "");
        ui->outputLog->append(QString::fromLocal8Bit(out));
    }
}

void PNG2GIFs::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
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
                                 tr("Conversion has been termiated."));
        }
        else
        {
            QMessageBox::warning(this,
                                 tr("Unsuccess exit"),
                                 tr("Conversion was been completed with errors. "
                                    "Please look the output log for more detailed information."));
        }
    }
    toggleBusy(false);
}

void PNG2GIFs::toggleBusy(bool busy)
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
