#include "user_data_edit.h"
#include "ui_user_data_edit.h"

UserDataEdit::UserDataEdit(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserDataEdit)
{
    ui->setupUi(this);
}

UserDataEdit::UserDataEdit(const QString &text, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserDataEdit)
{
    ui->setupUi(this);
    setText(text);
}

QString UserDataEdit::getText()
{
    return ui->currentText->toPlainText();
}

void UserDataEdit::setText(const QString &text)
{
    ui->currentText->setPlainText(text);
}

UserDataEdit::~UserDataEdit()
{
    delete ui;
}

void UserDataEdit::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void UserDataEdit::on_buttonBox_accepted()
{
    this->accept();
}

void UserDataEdit::on_buttonBox_rejected()
{
    this->reject();
}
