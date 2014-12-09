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

#ifndef NPCDIALOG_H
#define NPCDIALOG_H

#include <QDialog>
#include "../../data_configs/data_configs.h"

namespace Ui {
class NpcDialog;
}

class NpcDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NpcDialog(dataconfigs *configs, QWidget *parent = 0);
    ~NpcDialog();
    int selectedNPC;
    bool isEmpty;
    bool isCoin;
    int coins;
    int mode;

    void setState(int npcID, int mode=0);
    void updateSearch(QString searchStr);

private slots:
    void on_npcList_doubleClicked(const QModelIndex &index);

    void on_npcList_activated(const QModelIndex &index);

    void on_buttonBox_accepted();

    void on_NPCfromList_toggled(bool checked);

    void on_coinsNPC_toggled(bool checked);

    void on_searchEdit_textChanged(const QString &arg1);

    void on_searchTypeBox_currentIndexChanged(int index);

private:
    dataconfigs * pConfigs;
    Ui::NpcDialog *ui;
};

#endif // NPCDIALOG_H
