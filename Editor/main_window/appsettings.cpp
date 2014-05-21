#include "appsettings.h"
#include "./ui_appsettings.h"
#include <QFileDialog>

//#include "../common_features/logger_sets.h"

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

    ui->logLevel->setCurrentIndex(loglevel);
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
