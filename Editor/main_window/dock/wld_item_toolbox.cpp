/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <editing/_dialogs/musicfilelist.h>

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
    setAttribute(Qt::WA_ShowWithoutActivating);
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
    if( (setGrp) && (mw()->activeChildWindow() !=3) )
        return;
    WorldEdit *edit = mw()->activeWldEditWin();
    if( (edit==NULL) || (!edit->sceneCreated) )
        return;
    WldScene* scene = edit->scene;
    if(!scene)
        return;

    allWLabel    = MainWindow::tr("[all]");
    customWLabel = MainWindow::tr("[custom]");

    mw()->ui->menuNew->setEnabled(false);
    mw()->ui->actionNew->setEnabled(false);

    if(!setCat)
    {
        lock_Wcat=true;
        if(!setGrp)
        {
            lock_Wgrp=true;
            grp_tiles = allWLabel;
            grp_paths = allWLabel;
            grp_scenes = allWLabel;
        }
    }

    LogDebugQD("WorldTools -> Clear current");

    ui->WLD_TilesList->clear();
    util::memclear(ui->WLD_SceneList);
    util::memclear(ui->WLD_LevelList);
    ui->WLD_PathsList->clear();
    util::memclear(ui->WLD_MusicList);

    //util::memclear(ui->BlockItemsList);
    //util::memclear(ui->NPCItemsList);

    LogDebugQD("WorldTools -> Declare new");
    QListWidgetItem * item;
    QPixmap tmpI;

    QStringList tmpList, tmpGrpList;
    //bool found = false;

    tmpList.clear();
    tmpGrpList.clear();

    unsigned int tableRows=0;
    unsigned int tableCols=0;

    LogDebugQD("WorldTools -> Table size");
    //get Table size
    for(int i=1; i < scene->m_localConfigTerrain.size(); i++)
    {
        obj_w_tile &tileItem = scene->m_localConfigTerrain[i];
        if( tableRows < (tileItem.setup.row+1) ) tableRows = tileItem.setup.row + 1;
        if( tableCols < (tileItem.setup.col+1) ) tableCols = tileItem.setup.col + 1;
    }

    LogDebugQD("WorldTools -> set size");
    ui->WLD_TilesList->setRowCount(tableRows);
    ui->WLD_TilesList->setColumnCount(tableCols);
    ui->WLD_TilesList->setStyleSheet("QTableWidget::item { padding: 0px; margin: 0px; }");

    LogDebugQD("WorldTools -> Table of tiles");
    for(int i=1; i < scene->m_localConfigTerrain.size(); i++)
    {
        obj_w_tile &tileItem = scene->m_localConfigTerrain[i];
        Items::getItemGFX(&tileItem, tmpI, false, QSize(32,32));

        QTableWidgetItem * Titem = ui->WLD_TilesList->item(tileItem.setup.row, tileItem.setup.col);

        if ( (!Titem) || ( (Titem!=NULL)&&(Titem->text().isEmpty())) )
        {
            Titem = new QTableWidgetItem();
            Titem->setIcon( QIcon( tmpI ) );
            Titem->setText( NULL );
            Titem->setSizeHint(QSize(32,32));
            Titem->setData(3, QString::number(tileItem.setup.id) );
            Titem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);

            ui->WLD_TilesList->setRowHeight(tileItem.setup.row, 34);
            ui->WLD_TilesList->setColumnWidth(tileItem.setup.col, 34);

            ui->WLD_TilesList->setItem(tileItem.setup.row,tileItem.setup.col, Titem);
        }
    }

    LogDebugQD("WorldTools -> List of sceneries");
    for(int i=1; i<scene->m_localConfigScenery.size(); i++)
    {
            obj_w_scenery &sceneItem = scene->m_localConfigScenery[i];
            Items::getItemGFX(&sceneItem, tmpI, false, QSize(32,32));

            item = new QListWidgetItem();
            item->setIcon( QIcon( tmpI ) );
            item->setText( NULL );
            item->setData(3, QString::number(sceneItem.setup.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

            ui->WLD_SceneList->addItem( item );
    }

    tableRows=0;
    tableCols=0;

    LogDebugQD("WorldTools -> Table of paths size");
    //get Table size
    for(int i=1; i < scene->m_localConfigPaths.size(); i++ )
    {
        obj_w_path &pathItem = scene->m_localConfigPaths[i];
        if( tableRows < (pathItem.setup.row+1) ) tableRows=pathItem.setup.row + 1;
        if( tableCols < (pathItem.setup.col+1) ) tableCols=pathItem.setup.col + 1;
    }

    LogDebugQD("WorldTools -> Table of paths size define");
    ui->WLD_PathsList->setRowCount(tableRows);
    ui->WLD_PathsList->setColumnCount(tableCols);
    ui->WLD_PathsList->setStyleSheet("QTableWidget::item { padding: 0px; margin: 0px; }");

    LogDebugQD("WorldTools -> Table of paths");
    for(int i=1; i < scene->m_localConfigPaths.size(); i++ )
    {
        obj_w_path &pathItem = scene->m_localConfigPaths[i];
        Items::getItemGFX(&pathItem, tmpI, false, QSize(32,32));

        QTableWidgetItem * Titem = ui->WLD_PathsList->item(pathItem.setup.row, pathItem.setup.col);

        if ( (!Titem) || ( (Titem!=NULL)&&(Titem->text().isEmpty())) )
        {
            Titem = new QTableWidgetItem();
            Titem->setIcon( QIcon( tmpI ) );
            Titem->setText( NULL );
            Titem->setSizeHint(QSize(32,32));
            Titem->setData(3, QString::number(pathItem.setup.id) );
            Titem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);

            ui->WLD_PathsList->setRowHeight(pathItem.setup.row, 34);
            ui->WLD_PathsList->setColumnWidth(pathItem.setup.col, 34);

            ui->WLD_PathsList->setItem(pathItem.setup.row,pathItem.setup.col, Titem);
        }
    }

    LogDebugQD("WorldTools -> List of levels");
    for(int i=0; i < scene->m_localConfigLevels.size(); i++)
    {
        obj_w_level& levelItem = scene->m_localConfigLevels[i];

        if((mw()->configs.marker_wlvl.path==levelItem.setup.id)||
           (mw()->configs.marker_wlvl.bigpath==levelItem.setup.id))
            continue;

        Items::getItemGFX(&levelItem, tmpI, false, QSize(32,32));

        item = new QListWidgetItem();
        item->setIcon( QIcon( tmpI.scaled( QSize(32,32), Qt::KeepAspectRatio ) ) );
        item->setText( NULL );
        item->setData(3, QString::number(levelItem.setup.id) );
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

        ui->WLD_LevelList->addItem( item );
    }

    LogDebugQD("WorldTools -> List of musics");
    {//Place zero music item <Silence>
        item = new QListWidgetItem();
        item->setIcon( QIcon( QPixmap(":/images/playmusic.png").scaled( QSize(32,32), Qt::KeepAspectRatio ) ) );
        item->setText( tr("[Silence]") );
        item->setData(3, QString::number(0) );
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );
        ui->WLD_MusicList->addItem( item );
    };

    for(int i=1;i<mw()->configs.main_music_wld.size(); i++)
    {
        obj_music &musicItem = mw()->configs.main_music_wld[i];
        item = new QListWidgetItem();
        item->setIcon( QIcon( QPixmap(":/images/playmusic.png").scaled( QSize(32,32), Qt::KeepAspectRatio ) ) );
        item->setText( (musicItem.id==mw()->configs.music_w_custom_id)? customWLabel : musicItem.name );
        item->setData(3, QString::number(musicItem.id) );
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

        ui->WLD_MusicList->addItem( item );
    }

    tmpList.clear();
    tmpGrpList.clear();

    lock_Wgrp=false;
    lock_Wcat=false;

    mw()->ui->menuNew->setEnabled(true);
    mw()->ui->actionNew->setEnabled(true);
    LogDebugQD("WorldTools -> done");
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

        QString customMusicFile;
        if((unsigned)item->data(3).toInt()==mw()->configs.music_w_custom_id)
        {
            QString dirPath;
            WorldEdit * edit = mw()->activeWldEditWin();
            if(!edit) return;

            dirPath = edit->WldData.meta.path;

            if(edit->isUntitled)
            {
                QMessageBox::information(this, tr("Please, save file"), tr("Please, save file first, if you want to select custom music file."), QMessageBox::Ok);
                return;
            }

            MusicFileList musicList( dirPath, "" );
            if( musicList.exec() == QDialog::Accepted )
                customMusicFile = musicList.SelectedFile;
            else
                return;
        }

        WldPlacingItems::MusicSet.music_file = customMusicFile;
        mw()->SwitchPlacingItem(ItemTypes::WLD_MusicBox, item->data(3).toInt());

        //Play selected music
        mw()->activeWldEditWin()->currentMusic = item->data(3).toInt();
        mw()->activeWldEditWin()->currentCustomMusic = customMusicFile;
        LvlMusPlay::setMusic(LvlMusPlay::WorldMusic,
                             mw()->activeWldEditWin()->currentMusic,
                             mw()->activeWldEditWin()->currentCustomMusic);
        mw()->setMusic();
    }
}

