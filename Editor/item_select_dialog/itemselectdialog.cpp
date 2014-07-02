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

    ui->Sel_List_Block->item(0)->setData(3, QVariant(0));
    ui->Sel_List_BGO->item(0)->setData(3, QVariant(0));
    ui->Sel_List_NPC->item(0)->setData(3, QVariant(0));

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
            npcFromList = new QRadioButton(tr("NPC from List"));
            npcCoins = new QRadioButton(tr("Coins"));
            npcCoinsSel = new QSpinBox();
            npcCoinsSel->setMinimum(1);
            npcCoinsSel->setEnabled(false);
            extraNPCWid << npcFromList << npcCoins << npcCoinsSel;
            addExtraDataControl(npcFromList);
            addExtraDataControl(npcCoins);
            addExtraDataControl(npcCoinsSel);
            npcFromList->setChecked(true);
            connect(npcFromList, SIGNAL(toggled(bool)), this, SLOT(npcTypeChange(bool)));
            break;
        }
        default:
            break;
        }
    }

    if(blockTab){
        foreach(obj_block blockItem, conf->main_block)
        {
            //Add category
            QPixmap tmpI = blockItem.image;

            QListWidgetItem* item = new QListWidgetItem( blockItem.name );
            item->setIcon( QIcon( tmpI ) );
            item->setData(3, QString::number(blockItem.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );
            ui->Sel_List_Block->addItem(item);
        }
    }

    if(bgoTab){
        foreach(obj_bgo bgoItem, conf->main_bgo)
        {
            //Add category
            QPixmap tmpI = bgoItem.image;

            QListWidgetItem* item = new QListWidgetItem( bgoItem.name );
            item->setIcon( QIcon( tmpI ) );
            item->setData(3, QString::number(bgoItem.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );
            ui->Sel_List_BGO->addItem(item);
        }
    }

    if(npcTab){
        foreach(obj_npc npcItem, conf->main_npc)
        {
            //Add category
            QPixmap tmpI = npcItem.image.copy(0,0, npcItem.image.width(), npcItem.gfx_h );

            QListWidgetItem* item = new QListWidgetItem( npcItem.name );
            item->setIcon( QIcon( tmpI ) );
            item->setData(3, QString::number(npcItem.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );
            ui->Sel_List_NPC->addItem(item);
        }
    }



    on_Sel_TabCon_ItemType_currentChanged(ui->Sel_TabCon_ItemType->currentIndex());
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

void ItemSelectDialog::npcTypeChange(bool /*toggled*/)
{
    ui->Sel_List_NPC->setEnabled(npcFromList->isChecked());
    ui->Sel_Text_NPC->setEnabled(npcFromList->isChecked());
    ui->Sel_Combo_CategoryNPC->setEnabled(npcFromList->isChecked());
    ui->Sel_Combo_FiltertypeNPC->setEnabled(npcFromList->isChecked());
    ui->Sel_Combo_GroupsNPC->setEnabled(npcFromList->isChecked());
    npcCoinsSel->setEnabled(npcCoins->isChecked());
}

void ItemSelectDialog::on_Sel_DiaButtonBox_accepted()
{
    blockID = 0;
    bgoID = 0;
    npcID = 0;

    if(ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_Block)!=-1){
        if(!ui->Sel_List_Block->selectedItems().isEmpty()){
            blockID = ui->Sel_List_Block->selectedItems()[0]->data(3).toInt();
        }
    }

    if(ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_BGO)!=-1){
        if(!ui->Sel_List_BGO->selectedItems().isEmpty()){
            bgoID = ui->Sel_List_BGO->selectedItems()[0]->data(3).toInt();
        }
    }

    if(ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_NPC)!=-1){
        if(npcCoins!=0){
            if(npcCoins->isChecked()){
                npcID = npcCoinsSel->value();
            }else{
                if(!ui->Sel_List_NPC->selectedItems().isEmpty()){
                    npcID = ui->Sel_List_NPC->selectedItems()[0]->data(3).toInt()+1000;
                }
            }
        }else{
            if(!ui->Sel_List_NPC->selectedItems().isEmpty()){
                npcID = ui->Sel_List_NPC->selectedItems()[0]->data(3).toInt();
            }
        }
    }

    this->accept();
}
