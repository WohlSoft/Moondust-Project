/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "npcdialog.h"
#include <ui_npcdialog.h>

NpcDialog::NpcDialog(dataconfigs *configs, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NpcDialog)
{
    pConfigs = configs;
    ui->setupUi(this);
    isEmpty = false;
    isCoin = false;
    selectedNPC = 0;
    coins = 0;

    QListWidgetItem * item;
    QPixmap tmpI;

    ui->npcList->clear();

    foreach(obj_npc npcItem, pConfigs->main_npc)
    {
        //Add category
            tmpI = npcItem.image.copy(0,0, npcItem.image.width(), npcItem.gfx_h );

            item = new QListWidgetItem( npcItem.name );
            item->setIcon( QIcon( tmpI ) );
            item->setData(3, QString::number(npcItem.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );
            ui->npcList->addItem( item );
    }

}

NpcDialog::~NpcDialog()
{
    delete ui;
}

void NpcDialog::setState(int npcID, int mode)
{
    isCoin = false;
    isEmpty=false;

    switch(mode)
    {
    case 2: //Disable coin selecting
        ui->NPCfromList->setChecked(true);
        ui->coinsNPC->setEnabled(false);
        ui->coinsNPC->setVisible(false);
        ui->coinsInBlock->setVisible(false);
        selectedNPC = npcID;
        if(selectedNPC==0)
        {
            isEmpty=true;
            ui->emptyNPC->setChecked(true);
        }
        break;
    case 1: //select NPC only
        ui->NPCfromList->setChecked(true);
        ui->ContentType->setVisible(false);
        ui->ContentType->setEnabled(false);
        selectedNPC = npcID;
        break;
    case 0:
        if(npcID<=0)
        {
            isEmpty=true;
            ui->emptyNPC->setChecked(true);
        }
        else
        if(npcID<1000)
        {
            isCoin = true;
            coins = npcID;
            ui->coinsNPC->setChecked(true);
            ui->coinsInBlock->setValue(npcID);
        }
        else
        {
            coins=0;
            selectedNPC = npcID-1000;
            ui->NPCfromList->setChecked(true);

        }
    default: break;
    }

  if((!isEmpty)&&(!isCoin))
  foreach(QListWidgetItem *item, ui->npcList->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard))
  {
      if(item->data(3).toInt() == selectedNPC)
      {
          item->setSelected(true);
          ui->npcList->scrollToItem(item);
      }
  }

}

void NpcDialog::updateSearch(QString searchStr)
{
    QString toSearch;
    if(searchStr.isEmpty()){
        toSearch = ui->searchEdit->text();
    }else{
        toSearch = searchStr;
    }
    for(int i = 0; i < ui->npcList->count(); i++){
        if(toSearch.isEmpty()){
            ui->npcList->setRowHidden(i,false);
            continue;
        }
        if(ui->searchTypeBox->currentIndex()==0){ //search by text
            if(!ui->npcList->item(i)->text().contains(toSearch, Qt::CaseInsensitive)){
                ui->npcList->setRowHidden(i,true);
            }else{
                ui->npcList->setRowHidden(i,false);
            }
        }else if(ui->searchTypeBox->currentIndex()==1){ //search by id
            bool conv = false;
            int toIdSearch = toSearch.toInt(&conv);
            if(!conv){ //cannot convert
                break;
            }
            if(ui->npcList->item(i)->data(3).toInt()==toIdSearch){
                ui->npcList->setRowHidden(i,false);
            }else{
                ui->npcList->setRowHidden(i,true);
            }
        }else{//else do nothing
            break;
        }
    }
}

void NpcDialog::on_npcList_doubleClicked(const QModelIndex &index)
{
    selectedNPC = index.data(3).toInt();
    isEmpty = false;
    isCoin = false;
    this->accept();
}

void NpcDialog::on_npcList_activated(const QModelIndex &index)
{
    on_npcList_doubleClicked(index);
}

void NpcDialog::on_buttonBox_accepted()
{

    if(ui->NPCfromList->isChecked())
    {
        if(ui->npcList->selectedItems().isEmpty()) return;
        isEmpty = false;
        isCoin = false;
        selectedNPC = ui->npcList->selectedItems()[0]->data(3).toInt();
        coins=0;
    }
    else
    if(ui->emptyNPC->isChecked())
    {
        isEmpty = true;
        isCoin = false;
        selectedNPC = 0;
        coins=0;
    }
    else
    if(ui->coinsNPC->isChecked())
    {
        isEmpty = false;
        isCoin = true;
        selectedNPC = 0;
        coins=ui->coinsInBlock->value();
    }

    this->accept();
}

void NpcDialog::on_NPCfromList_toggled(bool checked)
{
    ui->npcList->setEnabled(checked);
    ui->searchEdit->setEnabled(checked);
    ui->searchTypeBox->setEnabled(checked);
}

void NpcDialog::on_coinsNPC_toggled(bool checked)
{
    ui->coinsInBlock->setEnabled(checked);
}

void NpcDialog::on_searchEdit_textChanged(const QString &arg1)
{
    updateSearch(arg1);
}

void NpcDialog::on_searchTypeBox_currentIndexChanged(int /*index*/)
{
    updateSearch(QString(""));
}
