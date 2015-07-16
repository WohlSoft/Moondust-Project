#include "tip_of_day.h"
#include "ui_tip_of_day.h"
#include <QFile>
#include <QTextStream>
#include <QList>
#include <QDateTime>
#include <main_window/global_settings.h>

TipOfDay::TipOfDay(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TipOfDay)
{
    qsrand(QDateTime::currentMSecsSinceEpoch());
    ui->setupUi(this);
    QFile ftips(":/tips/tips/tips_en.html");
    ftips.open(QIODevice::ReadOnly|QIODevice::Text);
    QTextStream ts(&ftips);
    QString allTipsRaw = ts.readAll();
    ftips.close();
    ui->showEachStart->setChecked(GlobalSettings::ShowTipOfDay);
    curTip=0;
    tips.clear();
    tips = allTipsRaw.split("<hr>\n");
    ui->prevTip->setEnabled(false);
    on_nextTip_clicked();
}

TipOfDay::~TipOfDay()
{
    GlobalSettings::ShowTipOfDay=ui->showEachStart->isChecked();
    delete ui;
}

void TipOfDay::on_prevTip_clicked()
{
    curTip--;
    ui->tipText->setText(tips_viewed[curTip]);

    ui->prevTip->setEnabled(curTip>0);
    ui->nextTip->setEnabled(  (curTip<tips_viewed.size()-1) || (!tips.isEmpty()) );
}

void TipOfDay::on_nextTip_clicked()
{
    QString tip;
    if((curTip>=(tips_viewed.size()-1))||tips_viewed.isEmpty())
    {
        int random_selector;
        int n = tips.size();
        random_selector = (qrand()%n);
        tip=tips[random_selector];
        ui->tipText->setText(tip);
        tips_viewed.append(tip);
        curTip=tips_viewed.size()-1;
        tips.removeAt(random_selector);
    } else {
        curTip++;
        ui->tipText->setText(tips_viewed[curTip]);
    }

    ui->prevTip->setEnabled(curTip>0);
    ui->nextTip->setEnabled(  (curTip<tips_viewed.size()-1) || (!tips.isEmpty()) );
}
