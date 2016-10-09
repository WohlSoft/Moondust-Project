#include "greeting_dialog.h"
#include "ui_greeting_dialog.h"

GreetingDialog::GreetingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GreetingDialog)
{
    ui->setupUi(this);
}

GreetingDialog::~GreetingDialog()
{
    delete ui;
}
