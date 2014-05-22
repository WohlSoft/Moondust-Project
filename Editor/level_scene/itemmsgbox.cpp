#include "itemmsgbox.h"
#include "ui_itemmsgbox.h"

ItemMsgBox::ItemMsgBox(QString text, QString label, QString title, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ItemMsgBox)
{
    currentText = text;
    ui->setupUi(this);

    if(!label.isEmpty())
        ui->NotesLabel->setText(label);

    if(!title.isEmpty())
        this->setWindowTitle(title);

    ui->msgTextBox->clear();
    ui->msgTextBox->appendPlainText(currentText);
}

ItemMsgBox::~ItemMsgBox()
{
    delete ui;
}

void ItemMsgBox::on_buttonBox_accepted()
{
    currentText = ui->msgTextBox->toPlainText().simplified().remove('\"');
    accept();
}
