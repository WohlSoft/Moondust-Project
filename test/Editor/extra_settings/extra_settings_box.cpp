#include "extra_settings_box.h"
#include "ui_extra_settings_box.h"

ExtraSettingsBox::ExtraSettingsBox(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ExtraSettingsBox)
{
    ui->setupUi(this);
}

ExtraSettingsBox::~ExtraSettingsBox()
{
    delete ui;
}

