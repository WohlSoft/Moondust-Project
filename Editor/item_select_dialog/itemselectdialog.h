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
    explicit ItemSelectDialog(dataconfigs *configs, int tabs, int npcExtraData = 0,
                              int curSelIDBlock = 0, int curSelIDBGO = 0, int curSelIDNPC = 0, QWidget *parent = 0);
    ~ItemSelectDialog();

    int blockID;
    int bgoID;
    int npcID;

    bool isCoin;

    enum Tabs{
        TAB_BLOCK = 1 << 0,
        TAB_BGO = 1 << 1,
        TAB_NPC = 1 << 2
    };

    enum NpcExtraFlags{
        NPCEXTRA_WITHCOINS = 1 << 0,
        NPCEXTRA_ISCOINSELECTED = 1 << 1
    };

    void removeEmptyEntry(int tabs);


private slots:

    void on_Sel_TabCon_ItemType_currentChanged(int index);
    void npcTypeChange(bool toggled);

    void on_Sel_DiaButtonBox_accepted();
    void on_Sel_Combo_GroupsBlock_currentIndexChanged(int index);
    void on_Sel_Combo_CategoryBlock_currentIndexChanged(int index);
    void on_Sel_Combo_GroupsBGO_currentIndexChanged(int index);
    void on_Sel_Combo_CategoryBGO_currentIndexChanged(int index);
    void on_Sel_Combo_GroupsNPC_currentIndexChanged(int index);
    void on_Sel_Combo_CategoryNPC_currentIndexChanged(int index);
    void on_Sel_Text_FilterBlock_textChanged(const QString &arg1);
    void on_Sel_Text_FilterBGO_textChanged(const QString &arg1);
    void on_Sel_Text_NPC_textChanged(const QString &arg1);
    void on_Sel_Combo_FiltertypeBlock_currentIndexChanged(int index);
    void on_Sel_Combo_FiltertypeBGO_currentIndexChanged(int index);
    void on_Sel_Combo_FiltertypeNPC_currentIndexChanged(int index);

private:
    void selectListItem(QListWidget *w, int array_id);

    QRadioButton* npcFromList;
    QRadioButton* npcCoins;
    QSpinBox* npcCoinsSel;

    int removalFlags;

    void addExtraDataControl(QWidget* control);

    void updateBoxes(bool setGrp = false, bool setCat = false);
    QString cat_blocks;
    QString cat_bgos;
    QString cat_npcs;

    void updateFilters();

    QList<QWidget*> extraBlockWid;
    QList<QWidget*> extraBGOWid;
    QList<QWidget*> extraNPCWid;

    dataconfigs* conf;
    Ui::ItemSelectDialog *ui;
};

inline ItemSelectDialog::Tabs operator|(ItemSelectDialog::Tabs a, ItemSelectDialog::Tabs b)
{return static_cast<ItemSelectDialog::Tabs>(static_cast<int>(a) | static_cast<int>(b));}

inline ItemSelectDialog::NpcExtraFlags operator|(ItemSelectDialog::NpcExtraFlags a, ItemSelectDialog::NpcExtraFlags b)
{return static_cast<ItemSelectDialog::NpcExtraFlags>(static_cast<int>(a) | static_cast<int>(b));}

#endif // ITEMSELECTDIALOG_H
