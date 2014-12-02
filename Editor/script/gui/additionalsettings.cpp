#include "additionalsettings.h"
#include "ui_additionalsettings.h"

AdditionalSettings::AdditionalSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AdditionalSettings)
{
    ui->setupUi(this);
}

AdditionalSettings::~AdditionalSettings()
{
    delete ui;
}
