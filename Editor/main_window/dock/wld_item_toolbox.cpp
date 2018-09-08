/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "item_tooltip_make.hpp"
#include "itembox_list_model.h"

WorldItemBox::WorldItemBox(QWidget *parent) :
    QDockWidget(parent),
    MWDock_Base(parent),
    ui(new Ui::WorldItemBox)
{
    setVisible(false);
    setAttribute(Qt::WA_ShowWithoutActivating);
    ui->setupUi(this);
    this->setAttribute(Qt::WA_X11DoNotAcceptFocus, true);

    m_terrainModel = new ItemBoxListModel(this);
    ui->WLD_TilesList->setModel(m_terrainModel);
    connect(ui->WLD_TilesList, &QTableView::clicked, this, &WorldItemBox::TerrainTilesTable_itemClicked);
    ui->WLD_TilesList->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->WLD_TilesList->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    m_sceneryModel = new ItemBoxListModel(this);
    ui->WLD_SceneList->setModel(m_sceneryModel);
    connect(ui->WLD_SceneList, &QListView::clicked, this, &WorldItemBox::SceneList_itemClicked);
    m_sceneryModel->setSort(ItemBoxListModel::Sort_ById, false);

    m_pathsModel = new ItemBoxListModel(this);
    ui->WLD_PathsList->setModel(m_pathsModel);
    connect(ui->WLD_PathsList, &QTableView::clicked, this, &WorldItemBox::PathsTable_itemClicked);
    ui->WLD_PathsList->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->WLD_PathsList->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    m_levelsModel = new ItemBoxListModel(this);
    ui->WLD_LevelList->setModel(m_levelsModel);
    connect(ui->WLD_LevelList, &QListView::clicked, this, &WorldItemBox::LevelList_itemClicked);
    m_levelsModel->setSort(ItemBoxListModel::Sort_ById, false);

    m_musicboxModel = new ItemBoxListModel(this);
    ui->WLD_MusicList->setModel(m_musicboxModel);
    connect(ui->WLD_MusicList, &QListView::clicked, this, &WorldItemBox::MusicList_itemClicked);
    m_musicboxModel->setSort(ItemBoxListModel::Sort_ById, false);

    mw()->addDockWidget(Qt::LeftDockWidgetArea, this);
    connect(mw(), SIGNAL(languageSwitched()), this, SLOT(re_translate()));
    connect(this, SIGNAL(visibilityChanged(bool)), mw()->ui->actionWLDToolBox, SLOT(setChecked(bool)));

    m_lastVisibilityState = isVisible();
    mw()->docks_world.
          addState(this, &m_lastVisibilityState);
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

void MainWindow::on_actionWLDToolBox_triggered(bool checked)
{
    dock_WldItemBox->setVisible(checked);
    if(checked) dock_WldItemBox->raise();
}



void WorldItemBox::initItemLists()
{
    if(mw()->activeChildWindow() != MainWindow::WND_World)
        return;

    WorldEdit *edit = mw()->activeWldEditWin();
    if((edit==NULL) || (!edit->sceneCreated))
        return;

    WldScene* scene = edit->scene;
    if(!scene)
        return;

    mw()->ui->menuNew->setEnabled(false);

    LogDebugQD("WorldTools -> Clear current");

    m_terrainModel->clear();
    m_sceneryModel->clear();
    m_pathsModel->clear();
    m_levelsModel->clear();
    m_musicboxModel->clear();

    LogDebugQD("WorldTools -> Declare new");


    LogDebug("LevelTools -> Fill list of Terrain tiles");
    {
        QSet<uint64_t> tilesCustomId;
        for(int i = 0; i < scene->m_customTerrain.size(); i++)
        {
            obj_w_tile &tiles = *scene->m_customTerrain[i];
            tilesCustomId.insert(tiles.setup.id);
        }

        m_terrainModel->addElementsBegin();
        m_terrainModel->setShowLabels(false);
        PGE_DataArray<obj_w_tile> *array = &scene->m_localConfigTerrain;
        {
            uint32_t rows = 0;
            uint32_t cols = 0;
            for(int i = 1; i < array->size(); i++)
            {
                obj_w_tile &tileItem = (*array)[i];
                if(rows < tileItem.setup.row)
                    rows = tileItem.setup.row;
                if(cols < tileItem.setup.col)
                    cols = tileItem.setup.col;
            }
            m_terrainModel->setTableMode(true, cols + 1, rows + 1);
        }

        for(int i = 1; i < array->size(); i++)
        {
            obj_w_tile &tileItem = (*array)[i];
            ItemBoxListModel::Element e;
            Items::getItemGFX(&tileItem, e.pixmap, false, QSize(32, 32));
            e.name = tileItem.setup.name.isEmpty() ? QString("tile-%1").arg(tileItem.setup.id) : tileItem.setup.name;
            e.description = makeToolTipSimple("Terrain tile", tileItem.setup);
            e.elementId = tileItem.setup.id;
            e.isCustom = tilesCustomId.contains(tileItem.setup.id);
            e.isValid = true;
            m_terrainModel->addElementCell(tileItem.setup.col, tileItem.setup.row, e, tileItem.setup.group, tileItem.setup.category);
        }
        m_terrainModel->addElementsEnd();
    }


    LogDebug("LevelTools -> Fill list of Sceneries");
    {
        QSet<uint64_t> sceneryCustomId;
        for(int i = 0; i < scene->m_customSceneries.size(); i++)
        {
            obj_w_scenery &scenery = *scene->m_customSceneries[i];
            sceneryCustomId.insert(scenery.setup.id);
        }
        m_sceneryModel->addElementsBegin();
        m_sceneryModel->setShowLabels(false);

        PGE_DataArray<obj_w_scenery> *array = &scene->m_localConfigScenery;
        for(int i = 1; i < array->size(); i++)
        {
            obj_w_scenery &sceneryItem = (*array)[i];
            ItemBoxListModel::Element e;
            Items::getItemGFX(&sceneryItem, e.pixmap, false, QSize(32, 32));
            e.name = sceneryItem.setup.name.isEmpty() ? QString("scene-%1").arg(sceneryItem.setup.id) : sceneryItem.setup.name;
            e.description = makeToolTipSimple("Scenery", sceneryItem.setup);
            e.elementId = sceneryItem.setup.id;
            e.isCustom = sceneryCustomId.contains(sceneryItem.setup.id);
            e.isValid = true;
            m_sceneryModel->addElement(e, sceneryItem.setup.group, sceneryItem.setup.category);
        }
        m_sceneryModel->addElementsEnd();
    }

    LogDebug("LevelTools -> Fill list of Path cells");
    {
        QSet<uint64_t> pathCustomId;
        for(int i = 0; i < scene->m_customPaths.size(); i++)
        {
            obj_w_path &paths = *scene->m_customPaths[i];
            pathCustomId.insert(paths.setup.id);
        }

        m_pathsModel->addElementsBegin();
        m_pathsModel->setShowLabels(false);
        PGE_DataArray<obj_w_path> *array = &scene->m_localConfigPaths;
        {
            uint32_t rows = 0;
            uint32_t cols = 0;
            for(int i = 1; i < array->size(); i++)
            {
                obj_w_path &pathItem = (*array)[i];
                if(rows < pathItem.setup.row)
                    rows = pathItem.setup.row;
                if(cols < pathItem.setup.col)
                    cols = pathItem.setup.col;
            }
            m_pathsModel->setTableMode(true, cols + 1, rows + 1);
        }

        for(int i = 1; i < array->size(); i++)
        {
            obj_w_path &pathItem = (*array)[i];
            ItemBoxListModel::Element e;
            Items::getItemGFX(&pathItem, e.pixmap, false, QSize(32, 32));
            e.name = pathItem.setup.name.isEmpty() ? QString("path-%1").arg(pathItem.setup.id) : pathItem.setup.name;
            e.description = makeToolTipSimple("Path cell", pathItem.setup);
            e.elementId = pathItem.setup.id;
            e.isCustom = pathCustomId.contains(pathItem.setup.id);
            e.isValid = true;
            m_pathsModel->addElementCell(pathItem.setup.col, pathItem.setup.row, e, pathItem.setup.group, pathItem.setup.category);
        }
        m_pathsModel->addElementsEnd();
    }

    LogDebug("LevelTools -> Fill list of Level cells");
    {
        QSet<uint64_t> levelCustomId;
        for(int i = 0; i < scene->m_customLevels.size(); i++)
        {
            obj_w_scenery &scenery = *scene->m_customLevels[i];
            levelCustomId.insert(scenery.setup.id);
        }

        m_levelsModel->addElementsBegin();
        m_levelsModel->setShowLabels(false);

        PGE_DataArray<obj_w_level> *array = &scene->m_localConfigLevels;
        for(int i = 0; i < array->size(); i++)
        {
            obj_w_level &levelItem = (*array)[i];
            ItemBoxListModel::Element e;
            Items::getItemGFX(&levelItem, e.pixmap, false, QSize(32, 32));
            e.name = levelItem.setup.name.isEmpty() ? QString("level-%1").arg(levelItem.setup.id) : levelItem.setup.name;
            e.description = makeToolTipSimple("Level entry point", levelItem.setup);
            e.elementId = levelItem.setup.id;
            e.isCustom = levelCustomId.contains(levelItem.setup.id);
            e.isValid = true;
            m_levelsModel->addElement(e, levelItem.setup.group, levelItem.setup.category);
        }
        m_levelsModel->addElementsEnd();
    }

    LogDebug("LevelTools -> Fill list of Music Boxes");
    {
        m_musicboxModel->addElementsBegin();
        for(int i = 1; i < mw()->configs.main_music_wld.size(); i++)
        {
            obj_music &musicItem = mw()->configs.main_music_wld[i];
            ItemBoxListModel::Element e;
            e.pixmap = QPixmap(":/images/playmusic.png");
            e.name = musicItem.name.isEmpty() ? QString("musicbox-%1").arg(musicItem.id) : musicItem.name;
            e.description = QString("ID: %1").arg(musicItem.id);
            e.elementId = musicItem.id;
            e.isCustom = false;
            e.isValid = true;
            m_musicboxModel->addElement(e);
        }
        m_musicboxModel->addElementsEnd();
    }

    mw()->ui->menuNew->setEnabled(true);
    LogDebugQD("WorldTools -> done");
}



void WorldItemBox::TerrainTilesTable_itemClicked(const QModelIndex &item)
{
    if(!item.isValid())
        return;//Ignore invalid
    //placeTile
    if((mw()->activeChildWindow() == MainWindow::WND_World) && (ui->WLD_TilesList->hasFocus()))
    {
        if(!m_terrainModel->data(item, ItemBoxListModel::ItemBox_ItemIsValid).toBool())
            return;
        int id = m_terrainModel->data(item, ItemBoxListModel::ItemBox_ItemId).toInt();
        mw()->SwitchPlacingItem(ItemTypes::WLD_Tile, id);
    }
}


void WorldItemBox::SceneList_itemClicked(const QModelIndex &item)
{
    if(!item.isValid())
        return;//Ignore invalid
    //placeScenery
    if((mw()->activeChildWindow() == MainWindow::WND_World) && (ui->WLD_SceneList->hasFocus()))
    {
        if(!m_sceneryModel->data(item, ItemBoxListModel::ItemBox_ItemIsValid).toBool())
            return;
        int id = m_sceneryModel->data(item, ItemBoxListModel::ItemBox_ItemId).toInt();
        mw()->SwitchPlacingItem(ItemTypes::WLD_Scenery, id);
    }
}

void WorldItemBox::PathsTable_itemClicked(const QModelIndex &item)
{
    if(!item.isValid())
        return;//Ignore invalid
    //placePath
    if ((mw()->activeChildWindow() == MainWindow::WND_World) && (ui->WLD_PathsList->hasFocus()))
    {
        if(!m_pathsModel->data(item, ItemBoxListModel::ItemBox_ItemIsValid).toBool())
            return;
        int id = m_pathsModel->data(item, ItemBoxListModel::ItemBox_ItemId).toInt();
        mw()->SwitchPlacingItem(ItemTypes::WLD_Path, id);
    }
}

void WorldItemBox::LevelList_itemClicked(const QModelIndex &item)
{
    if(!item.isValid())
        return;//Ignore invalid
    //placeLevel
    if ((mw()->activeChildWindow() == MainWindow::WND_World) && (ui->WLD_LevelList->hasFocus()))
    {
        if(!m_levelsModel->data(item, ItemBoxListModel::ItemBox_ItemIsValid).toBool())
            return;
        int id = m_levelsModel->data(item, ItemBoxListModel::ItemBox_ItemId).toInt();
        mw()->SwitchPlacingItem(ItemTypes::WLD_Level, id);
    }
}

void WorldItemBox::MusicList_itemClicked(const QModelIndex &item)
{
    if(!item.isValid())
        return;//Ignore invalid
    //placeLevel
    if ((mw()->activeChildWindow() == MainWindow::WND_World) && (ui->WLD_MusicList->hasFocus()))
    {
        WorldEdit * edit = mw()->activeWldEditWin();
        if(!edit)
            return;
        if(!m_musicboxModel->data(item, ItemBoxListModel::ItemBox_ItemIsValid).toBool())
            return;
        int id = m_musicboxModel->data(item, ItemBoxListModel::ItemBox_ItemId).toInt();

        QString customMusicFile;
        if((uint64_t)id == mw()->configs.music_w_custom_id)
        {
            if(edit->isUtitled())
            {
                QMessageBox::information(this, tr("Please, save file"), tr("Please, save file first, if you want to select custom music file."), QMessageBox::Ok);
                return;
            }

            QString dirPath = edit->WldData.meta.path;
            MusicFileList musicList(dirPath, "");
            if(musicList.exec() == QDialog::Accepted)
                customMusicFile = musicList.SelectedFile;
            else
                return;
        }

        WldPlacingItems::MusicSet.music_file = customMusicFile;
        mw()->SwitchPlacingItem(ItemTypes::WLD_MusicBox, id);

        //Play selected music
        edit->currentMusic = id;
        edit->currentCustomMusic = customMusicFile;
        LvlMusPlay::setMusic(LvlMusPlay::WorldMusic,
                             edit->currentMusic,
                             edit->currentCustomMusic);
        mw()->setMusic();
    }
}

