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




QString allWLabel = "[all]";
QString customWLabel = "[custom]";

bool lock_Wgrp=false;
bool lock_Wcat=false;


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

    util::memclear(ui->WLD_SceneList);

    util::memclear(ui->WLD_LevelList);

    util::memclear(ui->WLD_MusicList);

    //util::memclear(ui->BlockItemsList);
    //util::memclear(ui->NPCItemsList);

    WriteToLog(QtDebugMsg, "WorldTools -> Declare new");
    QListWidgetItem * item;
    QPixmap tmpI;

    QStringList tmpList, tmpGrpList;
    //bool found = false;

    tmpList.clear();
    tmpGrpList.clear();

//    foreach(obj_w_tile tileItem, configs.main_wtiles)
//    {
//            if(tileItem.animated)
//                tmpI = tileItem.image.copy(0,0,
//                            tileItem.image.width(),
//                            (int)round(tileItem.image.height() / tileItem.frames));
//            else
//                tmpI = tileItem.image;

//            item = new QListWidgetItem();
//            item->setIcon( QIcon( tmpI.scaled( QSize(32,32), Qt::KeepAspectRatio ) ) );
//            item->setText( NULL );
//            item->setData(3, QString::number(tileItem.id) );
//            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);

//            ui->WLD_TilesList->addItem( item );
//    }

    foreach(obj_w_scenery sceneItem, configs.main_wscene)
    {
            if(sceneItem.animated)
                tmpI = sceneItem.image.copy(0,0,
                            sceneItem.image.width(),
                            (int)round(sceneItem.image.height() / sceneItem.frames));
            else
                tmpI = sceneItem.image;

            item = new QListWidgetItem();
            item->setIcon( QIcon( tmpI ) );
            item->setText( NULL );
            item->setData(3, QString::number(sceneItem.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

            ui->WLD_SceneList->addItem( item );
    }

    //    foreach(obj_w_tile pathItem, configs.main_wtiles)
    //    {
    //            if(tileItem.animated)
    //                tmpI = tileItem.image.copy(0,0,
    //                            tileItem.image.width(),
    //                            (int)round(tileItem.image.height() / tileItem.frames));
    //            else
    //                tmpI = tileItem.image;

    //            item = new QListWidgetItem();
    //            item->setIcon( QIcon( tmpI.scaled( QSize(32,32), Qt::KeepAspectRatio ) ) );
    //            item->setText( NULL );
    //            item->setData(3, QString::number(tileItem.id) );
    //            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);

    //            ui->WLD_TilesList->addItem( item );
    //    }

    foreach(obj_w_level levelItem, configs.main_wlevels)
    {
            if((configs.marker_wlvl.path==levelItem.id)||
               (configs.marker_wlvl.bigpath==levelItem.id))
                continue;
            if(levelItem.animated)
                tmpI = levelItem.image.copy(0,0,
                            levelItem.image.width(),
                            (int)round(levelItem.image.height() / levelItem.frames));
            else
                tmpI = levelItem.image;

            item = new QListWidgetItem();
            item->setIcon( QIcon( tmpI.scaled( QSize(32,32), Qt::KeepAspectRatio ) ) );
            item->setText( NULL );
            item->setData(3, QString::number(levelItem.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

            ui->WLD_LevelList->addItem( item );
    }

    foreach(obj_music musicItem, configs.main_music_wld)
    {
            item = new QListWidgetItem();
            item->setIcon( QIcon( QPixmap(":/images/playmusic.png").scaled( QSize(32,32), Qt::KeepAspectRatio ) ) );
            item->setText( musicItem.name );
            item->setData(3, QString::number(musicItem.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

            ui->WLD_MusicList->addItem( item );
    }



    tmpList.clear();
    tmpGrpList.clear();

    lock_Wgrp=false;
    lock_Wcat=false;

    updateFilters();

}



void MainWindow::on_WLD_TilesList_itemClicked(QTableWidgetItem *item)
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


void MainWindow::on_WLD_SceneList_itemClicked(QListWidgetItem *item)
{
    resetEditmodeButtons();
   //placeScenery

    if ((activeChildWindow()==3) && (ui->WLD_SceneList->hasFocus()))
    {
       activeWldEditWin()->scene->clearSelection();
       activeWldEditWin()->changeCursor(2);
       activeWldEditWin()->scene->EditingMode = 2;
       activeWldEditWin()->scene->disableMoveItems=false;
       activeWldEditWin()->scene->DrawMode=true;
       activeWldEditWin()->scene->EraserEnabled = false;

       WldPlacingItems::fillingMode = false;

       activeWldEditWin()->scene->setItemPlacer(1, item->data(3).toInt() );

//       LvlItemProps(1,FileFormats::dummyLvlBlock(),
//                                 LvlPlacingItems::bgoSet,
//                                 FileFormats::dummyLvlNpc(), true);

       activeWldEditWin()->setFocus();
    }
}

void MainWindow::on_WLD_PathsList_itemClicked(QTableWidgetItem *item)
{
    resetEditmodeButtons();
   //placePath

    if ((activeChildWindow()==3) && (ui->WLD_PathsList->hasFocus()))
    {
       activeWldEditWin()->scene->clearSelection();
       activeWldEditWin()->changeCursor(2);
       activeWldEditWin()->scene->EditingMode = 2;
       activeWldEditWin()->scene->disableMoveItems=false;
       activeWldEditWin()->scene->DrawMode=true;
       activeWldEditWin()->scene->EraserEnabled = false;

       WldPlacingItems::fillingMode = false;

       activeWldEditWin()->scene->setItemPlacer(2, item->data(3).toInt() );

//       LvlItemProps(1,FileFormats::dummyLvlBlock(),
//                                 LvlPlacingItems::bgoSet,
//                                 FileFormats::dummyLvlNpc(), true);

       activeWldEditWin()->setFocus();
    }
}

void MainWindow::on_WLD_LevelList_itemClicked(QListWidgetItem *item)
{
    resetEditmodeButtons();
   //placeLevel

    if ((activeChildWindow()==3) && (ui->WLD_LevelList->hasFocus()))
    {
       activeWldEditWin()->scene->clearSelection();
       activeWldEditWin()->changeCursor(2);
       activeWldEditWin()->scene->EditingMode = 2;
       activeWldEditWin()->scene->disableMoveItems=false;
       activeWldEditWin()->scene->DrawMode=true;
       activeWldEditWin()->scene->EraserEnabled = false;

       WldPlacingItems::fillingMode = false;

       activeWldEditWin()->scene->setItemPlacer(3, item->data(3).toInt() );

       WldItemProps(0, WldPlacingItems::LevelSet, true);

       activeWldEditWin()->setFocus();
    }
}

void MainWindow::on_WLD_MusicList_itemClicked(QListWidgetItem *item)
{
    resetEditmodeButtons();
   //placeLevel

    if ((activeChildWindow()==3) && (ui->WLD_MusicList->hasFocus()))
    {
       activeWldEditWin()->scene->clearSelection();
       activeWldEditWin()->changeCursor(2);
       activeWldEditWin()->scene->EditingMode = 2;
       activeWldEditWin()->scene->disableMoveItems=false;
       activeWldEditWin()->scene->DrawMode=true;
       activeWldEditWin()->scene->EraserEnabled = false;

       WldPlacingItems::fillingMode = false;

       activeWldEditWin()->scene->setItemPlacer(4, item->data(3).toInt() );

       activeWldEditWin()->currentMusic = item->data(3).toInt();
       setMusic( ui->actionPlayMusic->isChecked() );

//       LvlItemProps(1,FileFormats::dummyLvlBlock(),
//                                 LvlPlacingItems::bgoSet,
//                                 FileFormats::dummyLvlNpc(), true);

       activeWldEditWin()->setFocus();
    }
}
