#include "calculator.h"
#include "ui_calculator.h"

Calculator::Calculator(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Calculator)
{
    ui->setupUi(this);
}

Calculator::~Calculator()
{
    delete ui;
}

void Calculator::on_time_direction_currentIndexChanged(int)
{
    on_time_source_editingFinished();
}

void Calculator::on_time_source_editingFinished()
{
    if(ui->time_source->text().isEmpty())
    {
        ui->time_dest->clear();
        return;
    }
    bool ok=false;
    double source = ui->time_source->text().toDouble(&ok);
    if(!ok)
    {
        ui->time_source->clear();
        ui->time_source->setFocus();
        return;
    }
    double res = 0.0;
    if(ui->time_direction->currentIndex()==0)
    {
        res = source*(1000.0/65.0);
    } else {
        res = (source/1000.0)*65.0;
    }
    ui->time_dest->setText(QString::number(res));
}



void Calculator::on_speed_direction_currentIndexChanged(int)
{
    on_speed_source_editingFinished();
}

void Calculator::on_speed_source_editingFinished()
{
    if(ui->speed_source->text().isEmpty())
    {
        ui->speed_dest->clear();
        return;
    }
    bool ok=false;
    double source = ui->speed_source->text().toDouble(&ok);
    if(!ok)
    {
        ui->speed_source->clear();
        ui->speed_source->setFocus();
        return;
    }
    double res = 0.0;
    if(ui->speed_direction->currentIndex()==0)
    {
        res = source*65.0;
    } else {
        res = source/65.0;
    }
    ui->speed_dest->setText(QString::number(res));
}
