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

    ui->menuNew->setEnabled(false);
    ui->actionNew->setEnabled(false);

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

    ui->WLD_TilesList->clear();
    util::memclear(ui->WLD_SceneList);
    util::memclear(ui->WLD_LevelList);
    ui->WLD_PathsList->clear();
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

    unsigned int tableRows=0;
    unsigned int tableCols=0;

    WriteToLog(QtDebugMsg, "WorldTools -> Table size");
    //get Table size
    foreach(obj_w_tile tileItem, configs.main_wtiles )
    {
        if(tableRows<tileItem.row+1) tableRows=tileItem.row+1;
        if(tableCols<tileItem.col+1) tableCols=tileItem.col+1;
    }

    WriteToLog(QtDebugMsg, "WorldTools -> set size");
    ui->WLD_TilesList->setRowCount(tableRows);
    ui->WLD_TilesList->setColumnCount(tableCols);
    ui->WLD_TilesList->setStyleSheet("QTableWidget::item { padding: 0px; margin: 0px; }");

    WriteToLog(QtDebugMsg, "WorldTools -> Table of tiles");
    foreach(obj_w_tile tileItem, configs.main_wtiles )
    {
            if(tileItem.animated)
                tmpI = tileItem.image.copy(0,
                            (int)round(tileItem.image.height() / tileItem.frames) * tileItem.display_frame,
                            tileItem.image.width(),
                            (int)round(tileItem.image.height() / tileItem.frames));
            else
                tmpI = tileItem.image;

        QTableWidgetItem * Titem = ui->WLD_TilesList->item(tileItem.row, tileItem.col);

        if ( (!Titem) || ( (Titem!=NULL)&&(Titem->text().isEmpty())) )
        {
            Titem = new QTableWidgetItem();
            Titem->setIcon( QIcon( tmpI.scaled( QSize(32,32), Qt::KeepAspectRatio ) ) );
            Titem->setText( NULL );
            Titem->setSizeHint(QSize(32,32));
            Titem->setData(3, QString::number(tileItem.id) );
            Titem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);

            ui->WLD_TilesList->setRowHeight(tileItem.row, 34);
            ui->WLD_TilesList->setColumnWidth(tileItem.col, 34);

            ui->WLD_TilesList->setItem(tileItem.row,tileItem.col, Titem);
        }
    }

    WriteToLog(QtDebugMsg, "WorldTools -> List of sceneries");
    foreach(obj_w_scenery sceneItem, configs.main_wscene)
    {
            if(sceneItem.animated)
                tmpI = sceneItem.image.copy(0,
                            (int)round(sceneItem.image.height() / sceneItem.frames) * sceneItem.display_frame,
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

    tableRows=0;
    tableCols=0;

    WriteToLog(QtDebugMsg, "WorldTools -> Table of paths size");
    //get Table size
    foreach(obj_w_path pathItem, configs.main_wpaths )
    {
        if(tableRows<pathItem.row+1) tableRows=pathItem.row+1;
        if(tableCols<pathItem.col+1) tableCols=pathItem.col+1;
    }

    WriteToLog(QtDebugMsg, "WorldTools -> Table of paths size define");
    ui->WLD_PathsList->setRowCount(tableRows);
    ui->WLD_PathsList->setColumnCount(tableCols);
    ui->WLD_PathsList->setStyleSheet("QTableWidget::item { padding: 0px; margin: 0px; }");

    WriteToLog(QtDebugMsg, "WorldTools -> Table of paths");
    foreach(obj_w_path pathItem, configs.main_wpaths )
    {
            if(pathItem.animated)
                tmpI = pathItem.image.copy(0,
                            (int)round(pathItem.image.height() / pathItem.frames)*pathItem.display_frame,
                            pathItem.image.width(),
                            (int)round(pathItem.image.height() / pathItem.frames));
            else
                tmpI = pathItem.image;

        QTableWidgetItem * Titem = ui->WLD_PathsList->item(pathItem.row, pathItem.col);

        if ( (!Titem) || ( (Titem!=NULL)&&(Titem->text().isEmpty())) )
        {
            Titem = new QTableWidgetItem();
            Titem->setIcon( QIcon( tmpI.scaled( QSize(32,32), Qt::KeepAspectRatio ) ) );
            Titem->setText( NULL );
            Titem->setSizeHint(QSize(32,32));
            Titem->setData(3, QString::number(pathItem.id) );
            Titem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);

            ui->WLD_PathsList->setRowHeight(pathItem.row, 34);
            ui->WLD_PathsList->setColumnWidth(pathItem.col, 34);

            ui->WLD_PathsList->setItem(pathItem.row,pathItem.col, Titem);
        }
    }

    WriteToLog(QtDebugMsg, "WorldTools -> List of levels");
    foreach(obj_w_level levelItem, configs.main_wlevels)
    {
            if((configs.marker_wlvl.path==levelItem.id)||
               (configs.marker_wlvl.bigpath==levelItem.id))
                continue;
            if(levelItem.animated)
                tmpI = levelItem.image.copy(0,
                            (int)round(levelItem.image.height() / levelItem.frames)*levelItem.display_frame,
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

    WriteToLog(QtDebugMsg, "WorldTools -> List of musics");
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

    //updateFilters();

    ui->menuNew->setEnabled(true);
    ui->actionNew->setEnabled(true);
    WriteToLog(QtDebugMsg, "WorldTools -> done");
}



void MainWindow::on_WLD_TilesList_itemClicked(QTableWidgetItem *item)
{
    resetEditmodeButtons();
   //placeTile

    if ((activeChildWindow()==3) && (ui->WLD_TilesList->hasFocus()))
    {
       ui->PlacingToolbar->setVisible(true);
       activeWldEditWin()->scene->clearSelection();
       activeWldEditWin()->changeCursor(2);
       activeWldEditWin()->scene->EditingMode = 2;
       activeWldEditWin()->scene->disableMoveItems=false;
       activeWldEditWin()->scene->DrawMode=true;
       activeWldEditWin()->scene->EraserEnabled = false;

       WldPlacingItems::fillingMode = false;
       ui->actionSquareFill->setChecked(false);
       ui->actionSquareFill->setEnabled(true);

       WldPlacingItems::lineMode = false;
       ui->actionLine->setChecked(false);
       ui->actionLine->setEnabled(true);

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
       ui->PlacingToolbar->setVisible(true);
       activeWldEditWin()->scene->clearSelection();
       activeWldEditWin()->changeCursor(2);
       activeWldEditWin()->scene->EditingMode = 2;
       activeWldEditWin()->scene->disableMoveItems=false;
       activeWldEditWin()->scene->DrawMode=true;
       activeWldEditWin()->scene->EraserEnabled = false;

       WldPlacingItems::fillingMode = false;
       ui->actionSquareFill->setChecked(false);
       ui->actionSquareFill->setEnabled(true);

       WldPlacingItems::lineMode = false;
       ui->actionLine->setChecked(false);
       ui->actionLine->setEnabled(true);

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
       ui->PlacingToolbar->setVisible(true);
       activeWldEditWin()->scene->clearSelection();
       activeWldEditWin()->changeCursor(2);
       activeWldEditWin()->scene->EditingMode = 2;
       activeWldEditWin()->scene->disableMoveItems=false;
       activeWldEditWin()->scene->DrawMode=true;
       activeWldEditWin()->scene->EraserEnabled = false;

       WldPlacingItems::fillingMode = false;
       ui->actionSquareFill->setChecked(false);
       ui->actionSquareFill->setEnabled(true);

       WldPlacingItems::lineMode = false;
       ui->actionLine->setChecked(false);
       ui->actionLine->setEnabled(true);

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
       ui->PlacingToolbar->setVisible(true);
       activeWldEditWin()->scene->clearSelection();
       activeWldEditWin()->changeCursor(2);
       activeWldEditWin()->scene->EditingMode = 2;
       activeWldEditWin()->scene->disableMoveItems=false;
       activeWldEditWin()->scene->DrawMode=true;
       activeWldEditWin()->scene->EraserEnabled = false;

       WldPlacingItems::fillingMode = false;
       ui->actionSquareFill->setChecked(false);
       ui->actionSquareFill->setEnabled(true);

       WldPlacingItems::lineMode = false;
       ui->actionLine->setChecked(false);
       ui->actionLine->setEnabled(true);

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
       ui->PlacingToolbar->setVisible(true);
       activeWldEditWin()->scene->clearSelection();
       activeWldEditWin()->changeCursor(2);
       activeWldEditWin()->scene->EditingMode = 2;
       activeWldEditWin()->scene->disableMoveItems=false;
       activeWldEditWin()->scene->DrawMode=true;
       activeWldEditWin()->scene->EraserEnabled = false;

       WldPlacingItems::fillingMode = false;
       ui->actionSquareFill->setChecked(false);
       ui->actionSquareFill->setEnabled(false);

       WldPlacingItems::lineMode = false;
       ui->actionLine->setChecked(false);
       ui->actionLine->setEnabled(false);

       activeWldEditWin()->scene->setItemPlacer(4, item->data(3).toInt() );

       activeWldEditWin()->currentMusic = item->data(3).toInt();
       setMusic( ui->actionPlayMusic->isChecked() );

//       LvlItemProps(1,FileFormats::dummyLvlBlock(),
//                                 LvlPlacingItems::bgoSet,
//                                 FileFormats::dummyLvlNpc(), true);

       activeWldEditWin()->setFocus();
    }
}
