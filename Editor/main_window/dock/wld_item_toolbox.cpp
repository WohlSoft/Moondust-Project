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

#include "../../ui_mainwindow.h"
#include "../../mainwindow.h"

#include "../../world_scene/wld_item_placing.h"
#include "../../file_formats/file_formats.h"
#include "../../common_features/util.h"

#include "../../data_configs/custom_data.h"




static QString allWLabel = "[all]";
static QString customWLabel = "[custom]";

static bool lock_Wgrp=false;
static bool lock_Wcat=false;


//void MainWindow::UpdateWldCustomItems()
//{
//    if((ui->BlockCatList->currentText()==customLabel)||
//       (ui->BGOCatList->currentText()==customLabel)||
//       (ui->NPCCatList->currentText()==customLabel))
//    {
//        setItemBoxes(true);
//    }

//}

static QString grp_tiles = "";
static QString grp_paths = "";
static QString grp_scenes = "";

void MainWindow::setWldItemBoxes(bool setGrp, bool setCat)
{
    allWLabel    = MainWindow::tr("[all]");
    customWLabel = MainWindow::tr("[custom]");

    if(!setCat)
    {
        lock_Wcat=true;
        cat_blocks = allWLabel;
        cat_bgos = allWLabel;
        cat_npcs = allWLabel;
        if(!setGrp)
        {
            lock_Wgrp=true;
            grp_tiles = allWLabel;
            grp_paths = allWLabel;
            grp_scenes = allWLabel;
        }
    }

    WriteToLog(QtDebugMsg, "WorldTools -> Clear current");

    util::memclear(ui->WLD_TilesList);
    //util::memclear(ui->BlockItemsList);
    //util::memclear(ui->NPCItemsList);

    WriteToLog(QtDebugMsg, "WorldTools -> Declare new");
    QListWidgetItem * item;
    QPixmap tmpI;

    QStringList tmpList, tmpGrpList;
    //bool found = false;

    tmpList.clear();
    tmpGrpList.clear();

    //set custom Block items from loaded level
//    if((ui->BlockCatList->currentText()==customWLabel)&&(setCat)&&(setGrp))
//    {
//        if(activeChildWindow()==1)
//        {
//            long j=0;
//            bool isIndex=false;
//            leveledit * edit = activeLvlEditWin();
//            foreach(UserBlocks block, edit->scene->uBlocks)
//            {

//                //Check for index
//                if(block.id < (unsigned long)configs.index_blocks.size())
//                {
//                    if(block.id == configs.main_block[configs.index_blocks[block.id].i].id)
//                    {
//                        j = configs.index_blocks[block.id].i;
//                        isIndex=true;
//                    }
//                }
//                //In index is false, fetch array
//                if(!isIndex)
//                {
//                    for(int i=0; i < configs.main_block.size(); i++)
//                    {
//                        if(configs.main_block[i].id == block.id)
//                        {
//                            j = 0;
//                            isIndex=true;
//                            break;
//                        }
//                    }
//                    if(!isIndex) j=0;
//                }


//                if(configs.main_block[j].animated)
//                    tmpI = block.image.copy(0,0,
//                                block.image.width(),
//                                (int)round(block.image.height() / configs.main_block[j].frames));
//                else
//                    tmpI = block.image;

//                item = new QListWidgetItem( QString("block-%1").arg(block.id) );
//                item->setIcon( QIcon( tmpI ) );
//                item->setData(3, QString::number(block.id) );
//                item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

//                ui->BlockItemsList->addItem( item );
//            }

//        }

//    }
//    else
    //set Block item box from global configs
    foreach(obj_w_tile tileItem, configs.main_wtiles)
    {
        //Add Group
//        found = false;
//        if(tmpList.size()!=0)
//            foreach(QString grp, tmpGrpList)
//            {
//                if(tileItem.group.isEmpty())
//                {found=true; break;}//Skip empty values
//                if(tileItem.group==grp)
//                {found=true; break;}
//            }
//        if(!found) tmpGrpList.push_back(tileItem.group);

//        //Add category
//        found = false;
//        if(tmpList.size()!=0)
//            foreach(QString cat, tmpList)
//            {
//                if(tileItem.category==cat)
//                {found=true; break;}
//                if((tileItem.group!=grp_blocks)&&(grp_blocks!=allWLabel))
//                {found=true; break;}
//            }
//        if(!found) tmpList.push_back(tileItem.category);

//        if(
//                ((tileItem.group==grp_blocks)||(grp_blocks==allWLabel)||(grp_blocks==""))&&
//                ((tileItem.category==cat_blocks)||(cat_blocks==allWLabel)))
//        {
            if(tileItem.animated)
                tmpI = tileItem.image.copy(0,0,
                            tileItem.image.width(),
                            (int)round(tileItem.image.height() / tileItem.frames));
            else
                tmpI = tileItem.image;

            item = new QListWidgetItem();
            item->setIcon( QIcon( tmpI.scaled( QSize(32,32), Qt::KeepAspectRatio ) ) );
            item->setText( NULL );
            item->setData(3, QString::number(tileItem.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);

            ui->WLD_TilesList->addItem( item );

        //}

    }
//    tmpList.sort();
//    tmpList.push_front(customWLabel);
//    tmpList.push_front(allWLabel);
//    tmpGrpList.sort();
//    tmpGrpList.push_front(allWLabel);

//    //apply group list
//    if(!setGrp)
//    {
//        ui->BlockGroupList->clear();
//        ui->BlockGroupList->addItems(tmpGrpList);
//    }

//    //apply category list
//    if(!setCat)
//    {
//        ui->BlockCatList->clear();
//        ui->BlockCatList->addItems(tmpList);
//    }

    tmpList.clear();
    tmpGrpList.clear();

    lock_Wgrp=false;
    lock_Wcat=false;

    updateFilters();

}









void MainWindow::on_WLD_TilesList_itemClicked(QListWidgetItem *item)
{
    resetEditmodeButtons();
   //placeTile

    if ((activeChildWindow()==3) && (ui->WLD_TilesList->hasFocus()))
    {
       activeWldEditWin()->scene->clearSelection();
       activeWldEditWin()->changeCursor(2);
       activeWldEditWin()->scene->EditingMode = 2;
       activeWldEditWin()->scene->disableMoveItems=false;
       activeWldEditWin()->scene->DrawMode=true;
       activeWldEditWin()->scene->EraserEnabled = false;

       WldPlacingItems::fillingMode = false;

       activeWldEditWin()->scene->setItemPlacer(0, item->data(3).toInt() );

//       LvlItemProps(1,FileFormats::dummyLvlBlock(),
//                                 LvlPlacingItems::bgoSet,
//                                 FileFormats::dummyLvlNpc(), true);

       activeWldEditWin()->setFocus();
    }

}

