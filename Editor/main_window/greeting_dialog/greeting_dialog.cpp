#include "greeting_dialog.h"
#include "ui_greeting_dialog.h"
#include <mainwindow.h>
#include <QKeyEvent>

GreetingDialog::GreetingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GreetingDialog)
{
    ui->setupUi(this);
    setWindowFlags((Qt::Dialog|Qt::WindowTitleHint|Qt::CustomizeWindowHint)& ~Qt::WindowCloseButtonHint);
}

GreetingDialog::~GreetingDialog()
{
    delete ui;
}

void GreetingDialog::keyPressEvent(QKeyEvent *e)
{
    if(e->key()!= Qt::Key_Escape)
        QDialog::keyPressEvent(e);
}

void GreetingDialog::on_modern_clicked()
{
    emit switchModern();
    close();
}

void GreetingDialog::on_classic_clicked()
{
    emit switchClassic();
    close();
}
