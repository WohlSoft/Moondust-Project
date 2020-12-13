#include "testwidget.h"
#include "ui_testwidget.h"

TestWidget::TestWidget(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TestWidget)
{
    ui->setupUi(this);
    ui->widget->setText(ui->demoText->toPlainText());
    ui->widget->setVanillaMode(ui->vanillaMode->isChecked());
}

TestWidget::~TestWidget()
{
    delete ui;
}


void TestWidget::on_demoText_textChanged()
{
    ui->widget->setText(ui->demoText->toPlainText());
}

void TestWidget::on_vanillaMode_clicked(bool checked)
{
    ui->widget->setVanillaMode(checked);
}
