#include "tip_of_day.h"
#include "ui_tip_of_day.h"

TipOfDay::TipOfDay(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TipOfDay)
{
    ui->setupUi(this);
}

TipOfDay::~TipOfDay()
{
    delete ui;
}

void TipOfDay::on_prevTip_clicked()
{

}

void TipOfDay::on_nextTip_clicked()
{

}
