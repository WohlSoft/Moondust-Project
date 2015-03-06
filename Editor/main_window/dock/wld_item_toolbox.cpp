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

#include <common_features/util.h>
#include <common_features/items.h>
#include <common_features/graphics_funcs.h>
#include <editing/_scenes/world/wld_item_placing.h>
#include <data_configs/custom_data.h>
#include <PGE_File_Formats/file_formats.h>
#include <audio/music_player.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>

#include "wld_item_toolbox.h"
#include "ui_wld_item_toolbox.h"

WorldItemBox::WorldItemBox(QWidget *parent) :
    QDockWidget(parent),
    MWDock_Base(parent),
    ui(new Ui::WorldItemBox)
{
    setVisible(false);
    ui->setupUi(this);

    allWLabel = "[all]";
    customWLabel = "[custom]";

    lock_Wgrp=false;
    lock_Wcat=false;

    grp_tiles = "";
    grp_paths = "";
    grp_scenes = "";

    mw()->addDockWidget(Qt::LeftDockWidgetArea, this);
    connect(mw(), SIGNAL(languageSwitched()), this, SLOT(re_translate()));

    mw()->docks_world.
          addState(this, &GlobalSettings::WorldItemBoxVis);
}

WorldItemBox::~WorldItemBox()
{
    delete ui;
}

QTabWidget *WorldItemBox::tabWidget()
{
    return ui->WorldToolBoxTabs;
}

void WorldItemBox::re_translate()
{
    ui->retranslateUi(this);
}

// World tool box show/hide
void WorldItemBox::on_WorldItemBox_visibilityChanged(bool visible)
{
    mw()->ui->actionWLDToolBox->setChecked(visible);
}

void MainWindow::on_actionWLDToolBox_triggered(bool checked)
{
    dock_WldItemBox->setVisible(checked);
    if(checked) dock_WldItemBox->raise();
}



void WorldItemBox::setWldItemBoxes(bool setGrp, bool setCat)
{
    if((setGrp)&&(mw()->activeChildWindow()!=3)) return;
    WorldEdit *edit = mw()->activeWldEditWin();
    if(!edit) return;

    allWLabel    = MainWindow::tr("[all]");
    customWLabel = MainWindow::tr("[custom]");

    mw()->ui->menuNew->setEnabled(false);
    mw()->ui->actionNew->setEnabled(false);

    if(!setCat)
    {
        lock_Wcat=true;
        mw()->cat_blocks = allWLabel;
        mw()->cat_bgos = allWLabel;
        mw()->cat_npcs = allWLabel;
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
    foreach(obj_w_tile tileItem, mw()->configs.main_wtiles )
    {
        if(tableRows<tileItem.row+1) tableRows=tileItem.row+1;
        if(tableCols<tileItem.col+1) tableCols=tileItem.col+1;
    }

    WriteToLog(QtDebugMsg, "WorldTools -> set size");
    ui->WLD_TilesList->setRowCount(tableRows);
    ui->WLD_TilesList->setColumnCount(tableCols);
    ui->WLD_TilesList->setStyleSheet("QTableWidget::item { padding: 0px; margin: 0px; }");

    WriteToLog(QtDebugMsg, "WorldTools -> Table of tiles");
    foreach(obj_w_tile tileItem, mw()->configs.main_wtiles )
    {
        tmpI = GraphicsHelps::squareImage(
                    Items::getItemGFX(ItemTypes::WLD_Tile, tileItem.id),
                    QSize(32,32));

        QTableWidgetItem * Titem = ui->WLD_TilesList->item(tileItem.row, tileItem.col);

        if ( (!Titem) || ( (Titem!=NULL)&&(Titem->text().isEmpty())) )
        {
            Titem = new QTableWidgetItem();
            Titem->setIcon( QIcon( tmpI ) );
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
    foreach(obj_w_scenery sceneItem, mw()->configs.main_wscene)
    {
            tmpI = GraphicsHelps::squareImage(
                        Items::getItemGFX(ItemTypes::WLD_Scenery, sceneItem.id),
                        QSize(32,32));

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
    foreach(obj_w_path pathItem, mw()->configs.main_wpaths )
    {
        if(tableRows<pathItem.row+1) tableRows=pathItem.row+1;
        if(tableCols<pathItem.col+1) tableCols=pathItem.col+1;
    }

    WriteToLog(QtDebugMsg, "WorldTools -> Table of paths size define");
    ui->WLD_PathsList->setRowCount(tableRows);
    ui->WLD_PathsList->setColumnCount(tableCols);
    ui->WLD_PathsList->setStyleSheet("QTableWidget::item { padding: 0px; margin: 0px; }");

    WriteToLog(QtDebugMsg, "WorldTools -> Table of paths");
    foreach(obj_w_path pathItem, mw()->configs.main_wpaths )
    {
        tmpI = GraphicsHelps::squareImage(
                    Items::getItemGFX(ItemTypes::WLD_Path, pathItem.id),
                    QSize(32,32));

        QTableWidgetItem * Titem = ui->WLD_PathsList->item(pathItem.row, pathItem.col);

        if ( (!Titem) || ( (Titem!=NULL)&&(Titem->text().isEmpty())) )
        {
            Titem = new QTableWidgetItem();
            Titem->setIcon( QIcon( tmpI ) );
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
    foreach(obj_w_level levelItem, mw()->configs.main_wlevels)
    {
            if((mw()->configs.marker_wlvl.path==levelItem.id)||
               (mw()->configs.marker_wlvl.bigpath==levelItem.id))
                continue;

            tmpI = GraphicsHelps::squareImage(
                        Items::getItemGFX(ItemTypes::WLD_Level, levelItem.id),
                        QSize(32,32));

            item = new QListWidgetItem();
            item->setIcon( QIcon( tmpI.scaled( QSize(32,32), Qt::KeepAspectRatio ) ) );
            item->setText( NULL );
            item->setData(3, QString::number(levelItem.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

            ui->WLD_LevelList->addItem( item );
    }

    WriteToLog(QtDebugMsg, "WorldTools -> List of musics");
    {//Place zero music item <Silence>
        item = new QListWidgetItem();
        item->setIcon( QIcon( QPixmap(":/images/playmusic.png").scaled( QSize(32,32), Qt::KeepAspectRatio ) ) );
        item->setText( tr("[Silence]") );
        item->setData(3, QString::number(0) );
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );
        ui->WLD_MusicList->addItem( item );
    };

    foreach(obj_music musicItem, mw()->configs.main_music_wld)
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

    mw()->ui->menuNew->setEnabled(true);
    mw()->ui->actionNew->setEnabled(true);
    WriteToLog(QtDebugMsg, "WorldTools -> done");
}



void WorldItemBox::on_WLD_TilesList_itemClicked(QTableWidgetItem *item)
{
    //placeTile
    if ((mw()->activeChildWindow()==3) && (ui->WLD_TilesList->hasFocus()))
    {
        mw()->SwitchPlacingItem(ItemTypes::WLD_Tile, item->data(3).toInt());
    }
}


void WorldItemBox::on_WLD_SceneList_itemClicked(QListWidgetItem *item)
{
    //placeScenery
    if ((mw()->activeChildWindow()==3) && (ui->WLD_SceneList->hasFocus()))
    {
        mw()->SwitchPlacingItem(ItemTypes::WLD_Scenery, item->data(3).toInt());
    }
}

void WorldItemBox::on_WLD_PathsList_itemClicked(QTableWidgetItem *item)
{
    //placePath
    if ((mw()->activeChildWindow()==3) && (ui->WLD_PathsList->hasFocus()))
    {
        mw()->SwitchPlacingItem(ItemTypes::WLD_Path, item->data(3).toInt());
    }
}

void WorldItemBox::on_WLD_LevelList_itemClicked(QListWidgetItem *item)
{
    //placeLevel
    if ((mw()->activeChildWindow()==3) && (ui->WLD_LevelList->hasFocus()))
    {
        mw()->SwitchPlacingItem(ItemTypes::WLD_Level, item->data(3).toInt());
    }
}

void WorldItemBox::on_WLD_MusicList_itemClicked(QListWidgetItem *item)
{
    //placeLevel
    if ((mw()->activeChildWindow()==3) && (ui->WLD_MusicList->hasFocus()))
    {
        mw()->SwitchPlacingItem(ItemTypes::WLD_MusicBox, item->data(3).toInt());

        //Play selected music
        mw()->activeWldEditWin()->currentMusic = item->data(3).toInt();
        LvlMusPlay::setMusic(LvlMusPlay::WorldMusic, mw()->activeWldEditWin()->currentMusic, "");
        mw()->setMusic();
    }
}

