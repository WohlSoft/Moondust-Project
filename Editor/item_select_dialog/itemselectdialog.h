/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef ITEMSELECTDIALOG_H
#define ITEMSELECTDIALOG_H

#include <QDialog>
#include "data_configs/data_configs.h"

namespace Ui {
class ItemSelectDialog;
}

class ItemSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ItemSelectDialog(dataconfigs *configs, bool blockTab = true, bool bgoTab = true, bool npcTab = true,
                              QVariant npcExtraData = 0, QWidget *parent = 0);
    ~ItemSelectDialog();

    int blockID;
    int bgoID;
    int npcID;

private slots:

    void on_Sel_TabCon_ItemType_currentChanged(int index);
    void npcTypeChange(bool toggled);

    void on_Sel_DiaButtonBox_accepted();

private:

    QRadioButton* npcFromList;
    QRadioButton* npcCoins;
    QSpinBox* npcCoinsSel;

    void addExtraDataControl(QWidget* control);
    QList<QWidget*> extraBlockWid;
    QList<QWidget*> extraBGOWid;
    QList<QWidget*> extraNPCWid;

    dataconfigs* conf;
    Ui::ItemSelectDialog *ui;
};

#endif // ITEMSELECTDIALOG_H
