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

#include "itemselectdialog.h"
#include "ui_itemselectdialog.h"

ItemSelectDialog::ItemSelectDialog(dataconfigs *configs,
                                   bool blockTab, bool bgoTab, bool npcTab,
                                   QVariant npcExtraData, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ItemSelectDialog)
{
    conf = configs;
    ui->setupUi(this);

    if(!blockTab)
        ui->Sel_TabCon_ItemType->removeTab(ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_Block));

    if(!bgoTab)
        ui->Sel_TabCon_ItemType->removeTab(ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_BGO));

    if(!npcTab)
        ui->Sel_TabCon_ItemType->removeTab(ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_NPC));

    if(!npcExtraData.isNull()){
        switch (npcExtraData.toInt()) {
        case 1:
        {
            QRadioButton* npcFromList = new QRadioButton(tr("NPC from List"));
            QRadioButton* npcCoins = new QRadioButton(tr("Coins"));
            QSpinBox* npcCoinsSel = new QSpinBox();
            npcCoinsSel->setMinimum(1);
            extraNPCWid << npcFromList << npcCoins << npcCoinsSel;
            addExtraDataControl(npcFromList);
            addExtraDataControl(npcCoins);
            addExtraDataControl(npcCoinsSel);
            break;
        }
        default:
            break;
        }
    }

    on_Sel_TabCon_ItemType_currentChanged(0);
}

ItemSelectDialog::~ItemSelectDialog()
{
    delete ui;
}

void ItemSelectDialog::addExtraDataControl(QWidget *control)
{
    ui->verticalLayout->insertWidget(ui->verticalLayout->count()-1, control);
}

void ItemSelectDialog::on_Sel_TabCon_ItemType_currentChanged(int index)
{
    if(!extraBlockWid.isEmpty()){
        for(QList<QWidget*>::iterator it = extraBlockWid.begin(); it != extraBlockWid.end(); ++it){
            ((QWidget*)(*it))->setVisible((ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_Block)!=-1 ? ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_Block)==index : false));
        }
    }
    if(!extraBGOWid.isEmpty()){
        for(QList<QWidget*>::iterator it = extraBGOWid.begin(); it != extraBGOWid.end(); ++it){
            ((QWidget*)(*it))->setVisible((ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_BGO)!=-1 ? ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_BGO)==index : false));
        }
    }
    if(!extraNPCWid.isEmpty()){
        for(QList<QWidget*>::iterator it = extraNPCWid.begin(); it != extraNPCWid.end(); ++it){
            ((QWidget*)(*it))->setVisible((ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_NPC)!=-1 ? ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_NPC)==index : false));
        }
    }
}
