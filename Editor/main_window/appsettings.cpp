#include "appsettings.h"
#include "./ui_appsettings.h"
#include <QFileDialog>
#include <math.h>

#include "../common_features/logger_sets.h"

AppSettings::AppSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AppSettings)
{
    ui->setupUi(this);
}

AppSettings::~AppSettings()
{
    delete ui;
}

void AppSettings::applySettings()
{
    //set settings into GUI fields
    ui->autoPlayMusic->setChecked(autoPlayMusic);
    ui->Animations->setChecked(Animation);
    ui->itemsLisit->setValue(AnimationItemLimit);

    ui->Collisions->setChecked(Collisions);

    loglevel = LogWriter::logLevel;
    logfile = LogWriter::DebugLogFile;
    logEnabled = LogWriter::enabled;

    if(logEnabled)
        switch(loglevel)
        {
        case QtDebugMsg:
            ui->logLevel->setCurrentIndex(4); break;
        case QtWarningMsg:
            ui->logLevel->setCurrentIndex(3); break;
        case QtCriticalMsg:
            ui->logLevel->setCurrentIndex(2); break;
        case QtFatalMsg:
            ui->logLevel->setCurrentIndex(1); break;
        }
    else
        ui->logLevel->setCurrentIndex(0);

    ui->logFileName->setText(logfile);
}

void AppSettings::on_setLogFile_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Set log file"),
        logfile, tr("Text files (*.txt *.log)"));
    if (fileName.isEmpty())
        return;

    logfile = fileName;
    ui->logFileName->setText(fileName);
}

void AppSettings::on_buttonBox_accepted()
{
    autoPlayMusic = ui->autoPlayMusic->isChecked();
    Animation = ui->Animations->isChecked();
    AnimationItemLimit = ui->itemsLisit->value();

    Collisions = ui->Collisions->isChecked();

    logfile = ui->logFileName->text();

    logEnabled=true;
    switch(ui->logLevel->currentIndex())
    {
    case 4:
        LogWriter::logLevel = QtDebugMsg; break;
    case 3:
        LogWriter::logLevel = QtWarningMsg; break;
    case 2:
        LogWriter::logLevel = QtCriticalMsg; break;
    case 1:
        LogWriter::logLevel = QtFatalMsg; break;
    case 0:
    default:
        logEnabled=false;
        LogWriter::logLevel = QtFatalMsg; break;
    }

    LogWriter::DebugLogFile = logfile;
    LogWriter::enabled = logEnabled;

    accept();
}
