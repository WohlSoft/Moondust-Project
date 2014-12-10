#include "savingnotificationdialog.h"
#include <ui_savingnotificationdialog.h>

SavingNotificationDialog::SavingNotificationDialog(bool showDiscardButton, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SavingNotificationDialog)
{
    ui->setupUi(this);
    ui->button_Discard->setVisible(showDiscardButton);
}

SavingNotificationDialog::~SavingNotificationDialog()
{
    delete ui;
}

void SavingNotificationDialog::setSavingTitle(const QString &title)
{
    ui->label_Title->setText(title);
}

void SavingNotificationDialog::setAdjustSize(const int &width, const int &height)
{
    setMinimumSize(width, height);
    setMaximumSize(width, height);
}

void SavingNotificationDialog::addUserItem(const QString &desc, QWidget *wid)
{
    ui->userItems->addWidget(new QLabel(desc), ui->userItems->count()/2, 1);
    ui->userItems->addWidget(wid, ui->userItems->count()/2, 2);
}

void SavingNotificationDialog::on_button_Save_clicked()
{
    savemode = SAVE_SAVE;
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
    accept();
}
