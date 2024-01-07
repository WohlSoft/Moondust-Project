/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2024 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "tip_of_day.h"
#include "ui_tip_of_day.h"
#include <QFile>
#include <QTextStream>
#include <QList>
#include <QDateTime>
#include <main_window/global_settings.h>


TipOfDay::TipOfDay(QString language, QWidget* parent) :
    QDialog(parent),
    m_mt(m_rd()),
    m_rand(0, RAND_MAX),
    ui(new Ui::TipOfDay)
{
    ui->setupUi(this);

    QFile ftips;
    QString tipsFile = QString(":/tips/tips_%1.html").arg(language);

    if(QFile::exists(tipsFile))
        ftips.setFileName(tipsFile);
    else // Fallback to English
        ftips.setFileName(":/tips/tips_en.html");

    ftips.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream ts(&ftips);
    QString allTipsRaw = ts.readAll();
    ftips.close();

    ui->showEachStart->setChecked(GlobalSettings::ShowTipOfDay);
    curTip = 0;

    tips.clear();
    tips = allTipsRaw.split("<hr>\n");

    ui->prevTip->setEnabled(false);
    on_nextTip_clicked();
}

TipOfDay::~TipOfDay()
{
    GlobalSettings::ShowTipOfDay = ui->showEachStart->isChecked();
    delete ui;
}

void TipOfDay::on_prevTip_clicked()
{
    curTip--;
    ui->tipText->setText(tips_viewed[curTip]);

    ui->prevTip->setEnabled(curTip > 0);
    ui->nextTip->setEnabled((curTip < tips_viewed.size() - 1) || (!tips.isEmpty()));
}

void TipOfDay::on_nextTip_clicked()
{
    QString tip;

    if((curTip >= (tips_viewed.size() - 1)) || tips_viewed.isEmpty())
    {
        int random_selector;
        int n = tips.size();
        random_selector = (m_rand(m_mt) % n);
        tip = tips[random_selector];
        ui->tipText->setText(tip);
        tips_viewed.append(tip);
        curTip = tips_viewed.size() - 1;
        tips.removeAt(random_selector);
    }
    else
    {
        curTip++;
        ui->tipText->setText(tips_viewed[curTip]);
    }

    ui->prevTip->setEnabled(curTip > 0);
    ui->nextTip->setEnabled((curTip < tips_viewed.size() - 1) || (!tips.isEmpty()));
}
