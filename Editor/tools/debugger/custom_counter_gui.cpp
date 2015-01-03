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

#include <QMenu>

#include <common_features/util.h>
#include <common_features/mainwinconnect.h>
#include <editing/_dialogs/itemselectdialog.h>

#include "custom_counter_gui.h"
#include "ui_custom_counter_gui.h"

CustomCounterGUI::CustomCounterGUI(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CustomCounterGUI)
{
    ui->setupUi(this);
    lockCombobox=false;
    if(MainWinConnect::pMainWin->activeChildWindow()==1)
    {
        ui->ItemType->addItem(tr("Block"), QVariant(int(ItemTypes::LVL_Block)));
        ui->ItemType->addItem(tr("BGO"), QVariant(int(ItemTypes::LVL_BGO)));
        ui->ItemType->addItem(tr("NPC"), QVariant(int(ItemTypes::LVL_NPC)));
        counterData.windowType=1;
    }
    else
    if(MainWinConnect::pMainWin->activeChildWindow()==3)
    {
        ui->ItemType->addItem(tr("Tile"), QVariant(int(ItemTypes::WLD_Tile)));
        ui->ItemType->addItem(tr("Scenery"), QVariant(int(ItemTypes::WLD_Scenery)));
        ui->ItemType->addItem(tr("Path"), QVariant(int(ItemTypes::WLD_Path)));
        ui->ItemType->addItem(tr("Level"), QVariant(int(ItemTypes::WLD_Level)));
        ui->ItemType->addItem(tr("Music"), QVariant(int(ItemTypes::WLD_MusicBox)));
        counterData.windowType=3;
    }
}


CustomCounterGUI::~CustomCounterGUI()
{
    delete ui;
}

void CustomCounterGUI::setCounterData(CustomCounter &data)
{
    counterData=data;
    ui->NameOfCounter->setText(counterData.name);
    lockCombobox=true;
    for(int i=0;i<ui->ItemType->count(); i++)
    {
        if(ui->ItemType->itemData(i, Qt::UserRole).toInt()==counterData.type)
        {
            ui->ItemType->setCurrentIndex(i);
            break;
        }
    }
    lockCombobox=false;

    util::memclear(ui->ItemList);
    foreach(long x, counterData.items)
    {
        addCntItem(makeItemName(x), x);
    }

}


void CustomCounterGUI::addCntItem(QString name, long id)
{
    QListWidgetItem *item;
    item = new QListWidgetItem(ui->ItemList);
    item->setData(Qt::UserRole, QVariant((int)id));
    item->setText(name);
    ui->ItemList->addItem(item);
}

void CustomCounterGUI::replaceItem(long what, long with)
{
    if(counterData.items.contains(what))
    {
        counterData.items[counterData.items.indexOf(what)]=with;
        QList<QListWidgetItem * > items = ui->ItemList->findItems("*", Qt::MatchWildcard);
        foreach(QListWidgetItem* x, items)
        {
            if(x->data(Qt::UserRole).toInt()==what)
            {
                x->setData(Qt::UserRole, (int)with);
                x->setText(makeItemName(with));
                break;
            }
        }
    }
}

void CustomCounterGUI::removeItem(long what)
{
    if(counterData.items.contains(what))
    {
        counterData.items.remove(counterData.items.indexOf(what));
        QList<QListWidgetItem * > items = ui->ItemList->findItems("*", Qt::MatchWildcard);
        foreach(QListWidgetItem* x, items)
        {
            if(x->data(Qt::UserRole).toInt()==what)
            {
                delete x;x=NULL;
            }
        }
    }
}

void CustomCounterGUI::addCItem(long item, QString name)
{
    if(!counterData.items.contains(item))
    {
        counterData.items.push_back(item);
        if(!name.isEmpty())
            addCntItem(name, item);
    }
}

QString CustomCounterGUI::makeItemName(long item)
{
    switch(counterData.type)
    {
    case ItemTypes::LVL_Block:
        {
            int bI = MainWinConnect::configs->getBlockI(item);
            if(bI<0)
                return QString("Block-%1").arg(item);
            else
            {
                if(!MainWinConnect::configs->main_block[bI].name.isEmpty())
                    return MainWinConnect::configs->main_block[bI].name + QString(" (Block-%1)").arg(item);
                else
                    return QString("Block-%1").arg(item);
            }
        }
        break;
    case ItemTypes::LVL_BGO:
        {
            int bI = MainWinConnect::configs->getBgoI(item);
            if(bI<0)
                return QString("BGO-%1").arg(item);
            else
            {
                if(!MainWinConnect::configs->main_bgo[bI].name.isEmpty())
                    return MainWinConnect::configs->main_bgo[bI].name + QString(" (BGO-%1)").arg(item);
                else
                    return QString("BGO-%1").arg(item);
            }
        }
        break;
    case ItemTypes::LVL_NPC:
        {
            int bI = MainWinConnect::configs->getNpcI(item);
            if(bI<0)
                return QString("NPC-%1").arg(item);
            else
            {
                if(!MainWinConnect::configs->main_npc[bI].name.isEmpty())
                    return MainWinConnect::configs->main_npc[bI].name + QString(" (NPC-%1)").arg(item);
                else
                    return QString("NPC-%1").arg(item);
            }
        }
        break;
    case ItemTypes::WLD_Tile:
        {
            return QString("Tile-%1").arg(item);
        }
        break;
    case ItemTypes::WLD_Scenery:
        {
            return QString("Scenery-%1").arg(item);
        }
        break;
    case ItemTypes::WLD_Path:
        {
            return QString("Path-%1").arg(item);
        }
        break;
    case ItemTypes::WLD_Level:
        {
            return QString("Level-%1").arg(item);
        }
        break;
    case ItemTypes::WLD_MusicBox:
        {
            int bI = MainWinConnect::configs->getMusWldI(item);
            if(bI<0)
                return QString("Music-%1").arg(item);
            else
            {
                if(!MainWinConnect::configs->main_music_wld[bI].name.isEmpty())
                    return MainWinConnect::configs->main_music_wld[bI].name + QString(" (Music-%1)").arg(item);
                else
                    return QString("Music-%1").arg(item);
            }
        }
        break;
    default:
        break;
    }
    return QString("Item-%1").arg(item);
}


void CustomCounterGUI::on_ItemType_currentIndexChanged(int)
{
    if(lockCombobox) return;
    counterData.type = (ItemTypes::itemTypes)ui->ItemType->currentData(Qt::UserRole).toInt();
    counterData.items.clear();
    util::memclear(ui->ItemList);
}

void CustomCounterGUI::on_ItemList_customContextMenuRequested(const QPoint &pos)
{
    if(ui->ItemList->selectedItems().isEmpty()) return;

    QListWidgetItem *item = ui->ItemList->selectedItems().first();
    int itemID = item->data(Qt::UserRole).toInt();

    QMenu menu;
    QAction* replace = menu.addAction(tr("Change item..."));
    QAction* remove = menu.addAction(tr("Remove"));
    QAction* selected = menu.exec( ui->ItemList->mapToGlobal(pos) );
    if(selected==NULL) return;
    if(selected==remove)
    {
        removeItem(itemID);
    }
    else
    if(selected==replace)
    {
        bool accepted=false;
        int selected=0;
        switch(counterData.type)
        {
        case ItemTypes::LVL_Block:
            {
                ItemSelectDialog* selBlock = new ItemSelectDialog(MainWinConnect::configs, ItemSelectDialog::TAB_BLOCK,0,itemID);
                if(selBlock->exec()==QDialog::Accepted){selected = selBlock->blockID; accepted=true;}
                delete selBlock;
            }
            break;
        case ItemTypes::LVL_BGO:
            {
                ItemSelectDialog* selBgo = new ItemSelectDialog(MainWinConnect::configs, ItemSelectDialog::TAB_BGO,0,0,itemID);
                if(selBgo->exec()==QDialog::Accepted){selected = selBgo->bgoID; accepted=true;}
                delete selBgo;
            }
            break;
        case ItemTypes::LVL_NPC:
            {
                ItemSelectDialog* selNpc = new ItemSelectDialog(MainWinConnect::configs, ItemSelectDialog::TAB_NPC,0,0,0,itemID);
                if(selNpc->exec()==QDialog::Accepted){selected = selNpc->npcID; accepted=true;}
                delete selNpc;
            }
            break;
        case ItemTypes::WLD_Tile:
            {
                ItemSelectDialog* selTile = new ItemSelectDialog(MainWinConnect::configs, ItemSelectDialog::TAB_TILE,0,0,0,0,itemID);
                if(selTile->exec()==QDialog::Accepted){selected = selTile->tileID; accepted=true;}
                delete selTile;
            }
            break;
        case ItemTypes::WLD_Scenery:
            {
                ItemSelectDialog* selScene = new ItemSelectDialog(MainWinConnect::configs, ItemSelectDialog::TAB_SCENERY,0,0,0,0,0,itemID);
                if(selScene->exec()==QDialog::Accepted){selected = selScene->sceneryID; accepted=true;}
                delete selScene;
            }
            break;
        case ItemTypes::WLD_Path:
            {
                ItemSelectDialog* selPath = new ItemSelectDialog(MainWinConnect::configs, ItemSelectDialog::TAB_PATH,0,0,0,0,0,0,itemID);
                if(selPath->exec()==QDialog::Accepted){selected = selPath->pathID; accepted=true;}
                delete selPath;
            }
            break;
        case ItemTypes::WLD_Level:
            {
                ItemSelectDialog* selLevel = new ItemSelectDialog(MainWinConnect::configs, ItemSelectDialog::TAB_LEVEL,0,0,0,0,0,0,0,itemID);
                if(selLevel->exec()==QDialog::Accepted){selected = selLevel->levelID; accepted=true;}
                delete selLevel;
            }
            break;
        case ItemTypes::WLD_MusicBox:
            {
                ItemSelectDialog* selMusicbox = new ItemSelectDialog(MainWinConnect::configs, ItemSelectDialog::TAB_MUSIC,0,0,0,0,0,0,0,0,itemID);
                if(selMusicbox->exec()==QDialog::Accepted){selected = selMusicbox->musicID; accepted=true;}
                delete selMusicbox;
            }
            break;
        default:
            break;
        }

        if(accepted)
        {
            if(selected>0)
                replaceItem(itemID, selected);
        }

    }

}

void CustomCounterGUI::on_buttonBox_accepted()
{
    counterData.name = ui->NameOfCounter->text();
    counterData.type = (ItemTypes::itemTypes)ui->ItemType->currentData(Qt::UserRole).toInt();
    this->accept();
}

void CustomCounterGUI::on_buttonBox_rejected()
{
    this->reject();
}

void CustomCounterGUI::on_AddItem_clicked()
{
    switch(counterData.type)
    {
    case ItemTypes::LVL_Block:
        {
        ItemSelectDialog* selBlock = new ItemSelectDialog(MainWinConnect::configs, ItemSelectDialog::TAB_BLOCK,0,0);
        if(selBlock->exec()==QDialog::Accepted){
            int selected = selBlock->blockID;
            if(selected>0)
                addCItem(selected, makeItemName(selected));
        }
        delete selBlock;
        }
        break;
    case ItemTypes::LVL_BGO:
        {
        ItemSelectDialog* selBgo = new ItemSelectDialog(MainWinConnect::configs, ItemSelectDialog::TAB_BGO,0,0,0);
        if(selBgo->exec()==QDialog::Accepted){
            int selected = selBgo->bgoID;
            if(selected>0)
                addCItem(selected, makeItemName(selected));
        }
        delete selBgo;
        }
        break;
    case ItemTypes::LVL_NPC:
        {
        ItemSelectDialog* selNpc = new ItemSelectDialog(MainWinConnect::configs, ItemSelectDialog::TAB_NPC,0,0,0,0);
        if(selNpc->exec()==QDialog::Accepted){
            int selected = selNpc->npcID;
            if(selected>0)
                addCItem(selected, makeItemName(selected));
        }
        delete selNpc;
        }
        break;
    case ItemTypes::WLD_Tile:
        {
            ItemSelectDialog* selTile = new ItemSelectDialog(MainWinConnect::configs, ItemSelectDialog::TAB_TILE,0,0,0,0,0);
            if(selTile->exec()==QDialog::Accepted)
            {
                int selected = selTile->tileID; if(selected>0)
                    addCItem(selected, makeItemName(selected));
            }
            delete selTile;
        }
        break;
    case ItemTypes::WLD_Scenery:
        {
            ItemSelectDialog* selScene = new ItemSelectDialog(MainWinConnect::configs, ItemSelectDialog::TAB_SCENERY,0,0,0,0,0,0);
            if(selScene->exec()==QDialog::Accepted){int selected = selScene->sceneryID;
                addCItem(selected, makeItemName(selected));}
            delete selScene;
        }
        break;
    case ItemTypes::WLD_Path:
        {
            ItemSelectDialog* selPath = new ItemSelectDialog(MainWinConnect::configs, ItemSelectDialog::TAB_PATH,0,0,0,0,0,0,0);
            if(selPath->exec()==QDialog::Accepted){int selected = selPath->pathID;
                addCItem(selected, makeItemName(selected));}
            delete selPath;
        }
        break;
    case ItemTypes::WLD_Level:
        {
            ItemSelectDialog* selLevel = new ItemSelectDialog(MainWinConnect::configs, ItemSelectDialog::TAB_LEVEL,0,0,0,0,0,0,0,0);
            if(selLevel->exec()==QDialog::Accepted){int selected = selLevel->levelID;
                addCItem(selected, makeItemName(selected));}
            delete selLevel;
        }
        break;
    case ItemTypes::WLD_MusicBox:
        {
            ItemSelectDialog* selMusicbox = new ItemSelectDialog(MainWinConnect::configs, ItemSelectDialog::TAB_MUSIC,0,0,0,0,0,0,0,0,0);
            if(selMusicbox->exec()==QDialog::Accepted){int selected = selMusicbox->musicID;
                addCItem(selected, makeItemName(selected));}
            delete selMusicbox;
        }
        break;
    default:
        break;
    }

}

void CustomCounterGUI::on_RemoveItem_clicked()
{
    if(ui->ItemList->selectedItems().isEmpty()) return;

    QListWidgetItem *item = ui->ItemList->selectedItems().first();
    int itemID = item->data(Qt::UserRole).toInt();
    removeItem(itemID);
}
