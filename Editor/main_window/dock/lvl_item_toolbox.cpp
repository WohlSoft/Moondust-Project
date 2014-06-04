/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */


#include "../../ui_mainwindow.h"
#include "../../mainwindow.h"

#include "../../level_scene/lvl_item_placing.h"


void MainWindow::setItemBoxes(bool setCat)
{
        WriteToLog(QtDebugMsg, "LevelTools -> Clear current");
    ui->BGOItemsList->clear();
    ui->BlockItemsList->clear();
    ui->NPCItemsList->clear();

        WriteToLog(QtDebugMsg, "LevelTools -> Declare new");
    QListWidgetItem * item;
    QPixmap tmpI;

    QStringList tmpList;
    bool found = false;

    tmpList.clear();
    tmpList.push_back("[all]");

    //set Block item box
    foreach(obj_block blockItem, configs.main_block)
    {
        //Add category
        found = false;
        if(tmpList.size()!=0)
            foreach(QString cat, tmpList)
            {   if(blockItem.type==cat)
                {found =true; break;}  }
        if(!found) tmpList.push_back(blockItem.type);

        if((blockItem.type==cat_blocks)||(cat_blocks=="[all]"))
        {
            if(blockItem.animated)
                tmpI = blockItem.image.copy(0,0,
                            blockItem.image.width(),
                            (int)round(blockItem.image.height() / blockItem.frames));
            else
                tmpI = blockItem.image;

            item = new QListWidgetItem( blockItem.name );
            item->setIcon( QIcon( tmpI ) );
            item->setData(3, QString::number(blockItem.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

            ui->BlockItemsList->addItem( item );
        }

    }

    //apply category list
    if(!setCat)
    {
        ui->BlockCatList->clear();
        ui->BlockCatList->addItems(tmpList);
    }

    tmpList.clear();
    tmpList.push_back("[all]");

    //set BGO item box
    foreach(obj_bgo bgoItem, configs.main_bgo)
    {
        //Add category
        found = false;
        if(tmpList.size()!=0)
            foreach(QString cat, tmpList)
            {   if(bgoItem.type==cat)
                {found =true; break;}  }
        if(!found) tmpList.push_back(bgoItem.type);

        if((bgoItem.type==cat_bgos)||(cat_bgos=="[all]"))
        {
            if(bgoItem.animated)
                tmpI = bgoItem.image.copy(0,0,
                            bgoItem.image.width(),
                            (int)round(bgoItem.image.height() / bgoItem.frames) );
            else
                tmpI = bgoItem.image;

            item = new QListWidgetItem( bgoItem.name );
            item->setIcon( QIcon( tmpI ) );
            item->setData(3, QString::number(bgoItem.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

            ui->BGOItemsList->addItem( item );
        }
    }
    //apply category list
    if(!setCat)
    {
        ui->BGOCatList->clear();
        ui->BGOCatList->addItems(tmpList);
    }

    tmpList.clear();
    tmpList.push_back("[all]");

    //set NPC item box
    foreach(obj_npc npcItem, configs.main_npc)
    {
        //Add category
        found = false;
        if(tmpList.size()!=0)
            foreach(QString cat, tmpList)
            {   if(npcItem.category==cat)
                {found = true; break;}  }
        if(!found) tmpList.push_back(npcItem.category);

        if((npcItem.category==cat_npcs)||(cat_npcs=="[all]"))
        {
            tmpI = npcItem.image.copy(0,0, npcItem.image.width(), npcItem.gfx_h );

            item = new QListWidgetItem( npcItem.name );
            item->setIcon( QIcon( tmpI ) );
            item->setData(3, QString::number(npcItem.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

            ui->NPCItemsList->addItem( item );
        }
    }
    //apply category list
    if(!setCat)
    {
        ui->NPCCatList->clear();
        ui->NPCCatList->addItems(tmpList);
    }

}

// ///////////////////////////////////
void MainWindow::on_BlockCatList_currentIndexChanged(const QString &arg1)
{
    cat_blocks=arg1;
    setItemBoxes(true);
}


void MainWindow::on_BGOCatList_currentIndexChanged(const QString &arg1)
{
    cat_bgos=arg1;
    setItemBoxes(true);
}


void MainWindow::on_NPCCatList_currentIndexChanged(const QString &arg1)
{
    cat_npcs=arg1;
    setItemBoxes(true);
}

// ///////////////////////////////////

void MainWindow::on_BGOUniform_clicked(bool checked)
{
    ui->BGOItemsList->setUniformItemSizes(checked);
    setItemBoxes(true);
}

void MainWindow::on_BlockUniform_clicked(bool checked)
{
    ui->BlockItemsList->setUniformItemSizes(checked);
    setItemBoxes(true);
}


void MainWindow::on_NPCUniform_clicked(bool checked)
{
    ui->NPCItemsList->setUniformItemSizes(checked);
    setItemBoxes(true);
}


// ///////////////////////////////////

void MainWindow::on_BlockItemsList_itemClicked(QListWidgetItem *item)
{
    resetEditmodeButtons();
   //placeBlock

    if ((activeChildWindow()==1) && (ui->BlockItemsList->hasFocus()))
    {
       activeLvlEditWin()->scene->clearSelection();
       activeLvlEditWin()->changeCursor(2);
       activeLvlEditWin()->scene->EditingMode = 2;
       activeLvlEditWin()->scene->disableMoveItems=false;
       activeLvlEditWin()->scene->DrawMode=true;
       activeLvlEditWin()->scene->EraserEnabled = false;
       LvlPlacingItems::fillingMode = false;

       activeLvlEditWin()->scene->setItemPlacer(0, item->data(3).toInt() );

       LevelBlock dummyBlock;
       dummyBlock.array_id=0;
       LevelBGO dummyBgo;
       dummyBgo.array_id=0;
       LevelNPC dummyNPC;
       dummyNPC.array_id=0;

       LvlItemProps(0,LvlPlacingItems::blockSet,
                                 dummyBgo,
                                 dummyNPC, true);

       activeLvlEditWin()->setFocus();
    }
}

void MainWindow::on_BGOItemsList_itemClicked(QListWidgetItem *item)
{
    resetEditmodeButtons();
   //placeBGO

    if ((activeChildWindow()==1) && (ui->BGOItemsList->hasFocus()))
    {
       activeLvlEditWin()->scene->clearSelection();
       activeLvlEditWin()->changeCursor(2);
       activeLvlEditWin()->scene->EditingMode = 2;
       activeLvlEditWin()->scene->disableMoveItems=false;
       activeLvlEditWin()->scene->DrawMode=true;
       activeLvlEditWin()->scene->EraserEnabled = false;

       LvlPlacingItems::fillingMode = false;

       activeLvlEditWin()->scene->setItemPlacer(1, item->data(3).toInt() );

       LevelBlock dummyBlock;
       dummyBlock.array_id=0;
       LevelBGO dummyBgo;
       dummyBgo.array_id=0;
       LevelNPC dummyNPC;
       dummyNPC.array_id=0;

       LvlItemProps(1,dummyBlock,
                                 LvlPlacingItems::bgoSet,
                                 dummyNPC, true);

       activeLvlEditWin()->setFocus();
    }

}

void MainWindow::on_NPCItemsList_itemClicked(QListWidgetItem *item)
{
    resetEditmodeButtons();

    //placeNPC
    if ((activeChildWindow()==1) && (ui->NPCItemsList->hasFocus()))
    {
       activeLvlEditWin()->scene->clearSelection();
       activeLvlEditWin()->changeCursor(2);
       activeLvlEditWin()->scene->EditingMode = 2;
       activeLvlEditWin()->scene->disableMoveItems=false;
       activeLvlEditWin()->scene->DrawMode=true;
       activeLvlEditWin()->scene->EraserEnabled = false;
       activeLvlEditWin()->scene->setItemPlacer(2, item->data(3).toInt() );

       LevelBlock dummyBlock;
       dummyBlock.array_id=0;
       LevelBGO dummyBgo;
       dummyBgo.array_id=0;
       LevelNPC dummyNPC;
       dummyNPC.array_id=0;

       LvlItemProps(2,dummyBlock,
                                 dummyBgo,
                                 LvlPlacingItems::npcSet, true);

       activeLvlEditWin()->setFocus();
    }

}

