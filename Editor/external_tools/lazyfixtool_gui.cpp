#include "lazyfixtool_gui.h"
#include <ui_lazyfixtool_gui.h>
#include <QMessageBox>
#include <QFileDialog>
#include "../common_features/app_path.h"

#include "../dev_console/devconsole.h"

LazyFixTool_gui::LazyFixTool_gui(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LazyFixTool_gui)
{
    proc = new QProcess;
    ui->setupUi(this);
}

LazyFixTool_gui::~LazyFixTool_gui()
{
    delete proc;
    delete ui;
}

void LazyFixTool_gui::on_BrowseInput_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Source Directory"),
                                                 (ui->inputDir->text().isEmpty() ? ApplicationPath : ui->inputDir->text()),
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    if(dir.isEmpty()) return;

    ui->inputDir->setText(dir);
}

void LazyFixTool_gui::on_BrowseOutput_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Target Directory"),
                                                 (ui->outputDir->text().isEmpty() ? ApplicationPath : ui->outputDir->text()),
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    if(dir.isEmpty()) return;

    ui->outputDir->setText(dir);
}

void LazyFixTool_gui::on_startTool_clicked()
{
    if(ui->inputDir->text().isEmpty())
    {
        QMessageBox::warning(this, tr("Source directory is not set"), tr("Please, set the source directory"), QMessageBox::Ok);
        return;
    }

    QString command;

    #ifdef _WIN32
    command = ApplicationPath+"/LazyFixTool.exe";
    #else
    command = ApplicationPath+"/LazyFixTool";
    #endif

    if(!QFile(command).exists())
    {
        QMessageBox::warning(this, tr("Tool is not found"), tr("Can't run application: \n%1\nPlease, check the application directory.").arg(command), QMessageBox::Ok);
        return;
    }

    QStringList args;
    if(ui->WalkSubDirs->isChecked()) args << "-W";
    if(ui->noBackUp->isChecked()) args << "-N";
    //if(ui->grayMasks->isChecked()) args << "-G";
    args << "--nopause";
    args << ui->inputDir->text();
    if(!ui->outputDir->text().isEmpty()) args << QString("-O%1").arg(ui->outputDir->text());

    DevConsole::show();
    DevConsole::log("Ready>>>", "LazyFix Tool");
    DevConsole::log("----------------------------------", "LazyFix Tool", true);
    proc->waitForFinished(1);
    proc->start(command, args);
    connect(proc, SIGNAL(readyReadStandardOutput()),this, SLOT(consoleMessage()) );
    //connect(proc, SIGNAL(readyReadStandardError()), this, SLOT(consoleMessage()) );
}

void LazyFixTool_gui::consoleMessage()
{
    QByteArray strdata = proc->readAllStandardOutput();
    DevConsole::log(strdata, "LazyFix Tool");
}


void LazyFixTool_gui::on_close_clicked()
{
    this->close();
}
