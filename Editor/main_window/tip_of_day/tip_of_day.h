/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2023 Vitaly Novichkov <admin@wohlnet.ru>
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

#pragma once
#ifndef TIP_OF_DAY_H
#define TIP_OF_DAY_H

#include <random>
#include <QDialog>
#include <QStringList>

namespace Ui {
class TipOfDay;
}

class TipOfDay : public QDialog
{
    Q_OBJECT

    std::random_device m_rd;
    std::mt19937 m_mt;
    std::uniform_int_distribution<int> m_rand;

public:
    explicit TipOfDay(QString language, QWidget *parent = nullptr);
    ~TipOfDay();

private slots:
    void on_prevTip_clicked();
    void on_nextTip_clicked();

private:
    Ui::TipOfDay *ui;
    QStringList tips;
    QStringList tips_viewed;
    int curTip;
};

#endif // TIP_OF_DAY_H
