#include "savingnotificationdialog.h"
#include "ui_savingnotificationdialog.h"

SavingNotificationDialog::SavingNotificationDialog(bool showDiscardButton, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SavingNotificationDialog)
{
    ui->setupUi(this);
    if(!showDiscardButton)
        ui->horizontalLayout->removeWidget(ui->button_Discard);
}

SavingNotificationDialog::~SavingNotificationDialog()
{
    delete ui;
}

void SavingNotificationDialog::on_button_OK_clicked()
{
    savemode = SAVE_OK;
    accept();
}

void SavingNotificationDialog::on_button_Discard_clicked()
{
    savemode = SAVE_DISCARD;
    accept();
}

void SavingNotificationDialog::on_button_Cancle_clicked()
{
    savemode = SAVE_CANCLE;
    reject();
}
