#include "lazyfix.h"
#include "ui_lazyfix.h"
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


LazyFixTool::LazyFixTool(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LazyFixTool)
{
    ui->setupUi(this);
#ifdef _WIN32
    m_lazyfix.setProgram(ApplicationPath_x + "/LazyFixTool.exe");
#else
    m_lazyfix.setProgram(ApplicationPath_x + "/LazyFixTool");
#endif
    m_lazyfix.setProcessChannelMode(QProcess::MergedChannels);
    QObject::connect(&m_lazyfix, &QProcess::readyReadStandardError,
                     this, &LazyFixTool::processOutput);
    QObject::connect(&m_lazyfix, &QProcess::readyReadStandardOutput,
                     this, &LazyFixTool::processOutput);
    QObject::connect(&m_lazyfix, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                     this, &LazyFixTool::processFinished);
}

LazyFixTool::~LazyFixTool()
{
    delete ui;
}

void LazyFixTool::closeEvent(QCloseEvent *e)
{
    if(m_lazyfix.state() == QProcess::Running)
    {
        e->ignore();
        return;
    }
}

void LazyFixTool::dragEnterEvent(QDragEnterEvent *e)
{
    if(ui->whatToConvert->currentWidget() != ui->filesTab)
        return; // Don't accept drop when drop area tab isn't active
    if(e->mimeData()->hasUrls())
        e->acceptProposedAction();
}

void LazyFixTool::dropEvent(QDropEvent *e)
{
    this->raise();
    this->setFocus(Qt::ActiveWindowFocusReason);

    if(m_lazyfix.state() == QProcess::Running)
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
        if(fileName.endsWith(".gif", Qt::CaseSensitive))
            filesToConvert.push_back(fileName);
    }

    if(!filesToConvert.isEmpty())
    {
        m_lazyfix.setArguments(filesToConvert);
        ui->outputLog->clear();
        m_lazyfix.start();
        toggleBusy(true);
    }
}

void LazyFixTool::on_doMadJob_clicked()
{
    if(m_lazyfix.state() == QProcess::Running)
    {
        m_lazyfix.terminate();
        return;
    }

    QString episodePath = ui->inPath->text();

    QStringList args;

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

    if(ui->flagScanSubdirs->isChecked())
        args << "-d";

    if(ui->flagRemoveOld->isChecked())
        args << "-n";

    args << "--" << episodePath;

    m_lazyfix.setArguments(args);
    ui->outputLog->clear();
    m_lazyfix.start();
    toggleBusy(true);
}

void LazyFixTool::on_inPathBrowse_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Select an input directory"),
                                                    ui->inPath->text(), c_dirDialogOptions);
    if(dir.isEmpty())
        return;
    ui->inPath->setText(dir);
}

void LazyFixTool::processOutput()
{
    while(1)
    {
        QByteArray out = m_lazyfix.readLine(0);
        if(out.isEmpty())
            break;
        out.replace("\r\n", "");
        out.replace("\n", "");
        ui->outputLog->append(QString::fromLocal8Bit(out));
    }
}

void LazyFixTool::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
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

void LazyFixTool::toggleBusy(bool busy)
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
