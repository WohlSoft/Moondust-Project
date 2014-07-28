#include "wld_setpoint.h"
#include "ui_wld_setpoint.h"

WLD_SetPoint::WLD_SetPoint(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WLD_SetPoint)
{
    ui->setupUi(this);
}

WLD_SetPoint::~WLD_SetPoint()
{
    delete ui;
}
