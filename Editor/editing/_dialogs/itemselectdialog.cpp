/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2023 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <common_features/main_window_ptr.h>
#include <common_features/util.h>
#include <common_features/graphics_funcs.h>
#include <common_features/data_array.h>
#include <common_features/items.h>

#include "file_list_browser/musicfilelist.h"

#include "itemselectdialog.h"
#include <ui_itemselectdialog.h>

#include "../../main_window/dock/itembox_list_model.h"
#include "../../main_window/dock/item_tooltip_make.hpp"

static QString grp_blocks = "";
static QString grp_bgo = "";
static QString grp_npc = "";
static bool lock_grp = false;
static bool lock_cat = false;

static QString allWLabel_F = "[all]";
static QString customWLabel_F = "[custom]";

static QString grp_tiles = "";
static QString grp_paths = "";
static QString grp_scenes = "";

static QPixmap emptyPixmap(QSize size)
{
    QImage img(size.width(), size.height(), QImage::Format_ARGB32);
    img.fill(Qt::transparent);
    QPainter e(&img);
    QFont t = e.font();
    t.setPixelSize(size.width() - 4);
    e.setFont(t);
    e.drawText(6, 2, size.width() - 6, size.height() - 2, Qt::TextSingleLine, "E");
    return QPixmap::fromImage(img);
}

ItemSelectDialog::ItemSelectDialog(DataConfig *conf, int tabs, int npcExtraData,
                                   int curSelIDBlock, int curSelIDBGO, int curSelIDNPC,
                                   int curSelIDTile, int curSelIDScenery, int curSelIDPath,
                                   int curSelIDLevel, int curSelIDMusic,
                                   QWidget *parent,
                                   int noEmptyTypes) :
    QDialog(parent),
    ui(new Ui::ItemSelectDialog)
{

    this->conf = conf;
    currentTab = 0;
    ui->setupUi(this);

    m_allLabel = MainWindow::tr("[all]");

    isMultiSelect = false;

    LvlScene *scene_lvl = nullptr;
    WldScene *scene_wld = nullptr;

    LevelEdit *edit_lvl = MainWinConnect::pMainWin->activeLvlEditWin();
    if(edit_lvl)
        scene_lvl = edit_lvl->sceneCreated ? edit_lvl->scene : nullptr;

    WorldEdit *edit_wld = MainWinConnect::pMainWin->activeWldEditWin();
    if(edit_wld)
        scene_wld = edit_wld->sceneCreated ? edit_wld->scene : nullptr;

    m_blockModel = new ItemBoxListModel(this);
    m_blockModel->setCategoryAllKey(m_allLabel);
    m_blockModel->setGroupAllKey(m_allLabel);
    ui->Sel_List_Block->setModel(m_blockModel);
    connect(ui->Sel_List_Block, &QListView::doubleClicked, this, &ItemSelectDialog::SelListBlock_itemDoubleClicked);

    m_bgoModel = new ItemBoxListModel(this);
    m_bgoModel->setCategoryAllKey(m_allLabel);
    m_bgoModel->setGroupAllKey(m_allLabel);
    ui->Sel_List_BGO->setModel(m_bgoModel);
    connect(ui->Sel_List_BGO, &QListView::doubleClicked, this, &ItemSelectDialog::SelListBGO_itemDoubleClicked);

    m_npcModel = new ItemBoxListModel(this);
    m_npcModel->setCategoryAllKey(m_allLabel);
    m_npcModel->setGroupAllKey(m_allLabel);
    ui->Sel_List_NPC->setModel(m_npcModel);
    connect(ui->Sel_List_NPC, &QListView::doubleClicked, this, &ItemSelectDialog::SelListNPC_itemDoubleClicked);

    m_tileModel = new ItemBoxListModel(this);
    m_tileModel->setCategoryAllKey(m_allLabel);
    m_tileModel->setGroupAllKey(m_allLabel);
    ui->Sel_List_Tile->setModel(m_tileModel);
    connect(ui->Sel_List_Tile, &QListView::doubleClicked, this, &ItemSelectDialog::SelListTile_itemDoubleClicked);
    ui->Sel_List_Tile->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->Sel_List_Tile->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    m_sceneModel = new ItemBoxListModel(this);
    m_sceneModel->setCategoryAllKey(m_allLabel);
    m_sceneModel->setGroupAllKey(m_allLabel);
    ui->Sel_List_Scenery->setModel(m_sceneModel);
    connect(ui->Sel_List_Scenery, &QListView::doubleClicked, this, &ItemSelectDialog::SelListScenery_itemDoubleClicked);

    m_pathModel = new ItemBoxListModel(this);
    m_pathModel->setCategoryAllKey(m_allLabel);
    m_pathModel->setGroupAllKey(m_allLabel);
    ui->Sel_List_Path->setModel(m_pathModel);
    connect(ui->Sel_List_Path, &QListView::doubleClicked, this, &ItemSelectDialog::SelListPath_itemDoubleClicked);
    ui->Sel_List_Path->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->Sel_List_Path->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    m_levelModel = new ItemBoxListModel(this);
    m_levelModel->setCategoryAllKey(m_allLabel);
    m_levelModel->setGroupAllKey(m_allLabel);
    ui->Sel_List_Level->setModel(m_levelModel);
    connect(ui->Sel_List_Level, &QListView::doubleClicked, this, &ItemSelectDialog::SelListLevel_itemDoubleClicked);

    m_musboxModel = new ItemBoxListModel(this);
    m_musboxModel->setCategoryAllKey(m_allLabel);
    m_musboxModel->setGroupAllKey(m_allLabel);
    ui->Sel_List_Music->setModel(m_musboxModel);
    connect(ui->Sel_List_Music, &QListView::doubleClicked, this, &ItemSelectDialog::SelListMusic_itemDoubleClicked);


    makeFilterSetupMenu(m_blockFilterSetup, m_blockModel, ui->Sel_Text_FilterBlock);
    ui->Sel_FilterSetupBlock->setMenu(&m_blockFilterSetup);

    makeFilterSetupMenu(m_bgoFilterSetup, m_bgoModel, ui->Sel_Text_FilterBGO);
    ui->Sel_FilterSetupBGO->setMenu(&m_bgoFilterSetup);

    makeFilterSetupMenu(m_npcFilterSetup, m_npcModel, ui->Sel_Text_NPC);
    ui->Sel_FilterSetupNPC->setMenu(&m_npcFilterSetup);

    makeFilterSetupMenu(m_tilesFilterSetup, m_tileModel, ui->Sel_Text_Tile);
    ui->Sel_FilterSetupTerrain->setMenu(&m_tilesFilterSetup);

    makeFilterSetupMenu(m_sceneFilterSetup, m_sceneModel, ui->Sel_Text_Scenery);
    ui->Sel_FilterSetupScenery->setMenu(&m_sceneFilterSetup);

    makeFilterSetupMenu(m_pathFilterSetup, m_pathModel, ui->Sel_Text_Path);
    ui->Sel_FilterSetupPath->setMenu(&m_pathFilterSetup);

    makeFilterSetupMenu(m_levelFilterSetup, m_levelModel, ui->Sel_Text_Level);
    ui->Sel_FilterSetupLevel->setMenu(&m_levelFilterSetup);

    makeFilterSetupMenu(m_musicFilterSetup, m_musboxModel, ui->Sel_Text_Music);
    ui->Sel_FilterSetupMusicBox->setMenu(&m_musicFilterSetup);


    QFont font;
    font.setItalic(true);

//    empBlock->setFont(font);
//    empBGO->setFont(font);
//    empNPC->setFont(font);
//    empScenery->setFont(font);
//    empLevel->setFont(font);
//    empMusic->setFont(font);

    QString emTxt = tr("[Empty]");


//    empBlock->setData(Qt::UserRole, QVariant(0));
//    empBGO->setData(Qt::UserRole, QVariant(0));
//    empNPC->setData(Qt::UserRole, QVariant(0));
//    empScenery->setData(Qt::UserRole, QVariant(0));
//    empLevel->setData(Qt::UserRole, QVariant(0));
//    empMusic->setData(Qt::UserRole, QVariant(0));

    ui->Sel_Tab_Block->setProperty("tabType",   TAB_BLOCK);
    ui->Sel_Tab_BGO->setProperty("tabType",     TAB_BGO);
    ui->Sel_Tab_NPC->setProperty("tabType",     TAB_NPC);
    ui->Sel_Tab_Tile->setProperty("tabType",    TAB_TILE);
    ui->Sel_Tab_Scenery->setProperty("tabType", TAB_SCENERY);
    ui->Sel_Tab_Path->setProperty("tabType",    TAB_PATH);
    ui->Sel_Tab_Level->setProperty("tabType",   TAB_LEVEL);
    ui->Sel_Tab_Music->setProperty("tabType",   TAB_MUSIC);

    bool blockTab   = tabs & TAB_BLOCK;
    bool bgoTab     = tabs & TAB_BGO;
    bool npcTab     = tabs & TAB_NPC;
    bool tileTab    = tabs & TAB_TILE;
    bool sceneryTab = tabs & TAB_SCENERY;
    bool pathTab    = tabs & TAB_PATH;
    bool levelTab   = tabs & TAB_LEVEL;
    bool musicTab   = tabs & TAB_MUSIC;
    bool isCoinSel  = npcExtraData & NPCEXTRA_ISCOINSELECTED;

    if(!blockTab)
        ui->Sel_TabCon_ItemType->removeTab(ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_Block));
    else if(currentTab == 0)
        currentTab = TAB_BLOCK;

    if(!bgoTab)
        ui->Sel_TabCon_ItemType->removeTab(ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_BGO));
    else if(currentTab == 0)
        currentTab = TAB_BGO;

    if(!npcTab)
        ui->Sel_TabCon_ItemType->removeTab(ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_NPC));
    else if(currentTab == 0)
        currentTab = TAB_NPC;

    if(!tileTab)
        ui->Sel_TabCon_ItemType->removeTab(ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_Tile));
    else if(currentTab == 0)
        currentTab = TAB_TILE;

    if(!sceneryTab)
        ui->Sel_TabCon_ItemType->removeTab(ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_Scenery));
    else if(currentTab == 0)
        currentTab = TAB_SCENERY;

    if(!pathTab)
        ui->Sel_TabCon_ItemType->removeTab(ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_Path));
    else if(currentTab == 0)
        currentTab = TAB_PATH;

    if(!levelTab)
        ui->Sel_TabCon_ItemType->removeTab(ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_Level));
    else if(currentTab == 0)
        currentTab = TAB_LEVEL;

    if(!musicTab)
        ui->Sel_TabCon_ItemType->removeTab(ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_Music));
    else if(currentTab == 0)
        currentTab = TAB_MUSIC;

    if(npcExtraData & NPCEXTRA_WITHCOINS)
    {
        npcFromList = new QRadioButton(tr("NPC from List"), this);
        npcCoins    = new QRadioButton(tr("Coins"), this);
        npcCoinsSel = new QSpinBox(this);
        npcCoinsSel->setMinimum(1);
        npcCoinsSel->setEnabled(false);
        extraNPCWid << npcFromList << npcCoins << npcCoinsSel;
        addExtraDataControl(npcFromList);
        addExtraDataControl(npcCoins);
        addExtraDataControl(npcCoinsSel);
        connect(npcFromList, SIGNAL(toggled(bool)), this, SLOT(npcTypeChange(bool)));
        if(isCoinSel)
        {
            npcCoinsSel->setValue(curSelIDNPC);
            npcCoins->setChecked(true);
            npcTypeChange(true);
        }
        else
        {
            npcFromList->setChecked(true);
        }
    }
    else
    {
        npcFromList = nullptr;
        npcCoins = nullptr;
        npcCoinsSel = nullptr;
    }

    if(blockTab)
    {
        m_blockModel->addElementsBegin();
        if((noEmptyTypes & TAB_BLOCK) == 0)
        {
            ItemBoxListModel::Element empBlock;
            empBlock.name = emTxt;
            empBlock.isValid = true;
            empBlock.pixmap = emptyPixmap(QSize(16, 16));
            empBlock.description = "Empty element";
            m_blockModel->setSortSkipFirst(true);
            m_blockModel->addElement(empBlock);
        }

        //set Block item box from global configs
        QSet<uint64_t> blockCustomId;
        if(scene_lvl)
        {
            for(int i = 0; i < scene_lvl->m_customBlocks.size(); i++)
            {
                obj_block &block = *scene_lvl->m_customBlocks[i];
                blockCustomId.insert(block.setup.id);
            }
        }

        PGE_DataArray<obj_block> *array = scene_lvl ? &scene_lvl->m_localConfigBlocks : &conf->main_block;
        for(int i = 1; i < array->size(); i++) //Add user images
        {
            obj_block &blockItem = (*array)[i];
            ItemBoxListModel::Element e;
            Items::getItemGFX(&blockItem, e.pixmap, false, QSize(16, 16));
            e.name = blockItem.setup.name.isEmpty() ? QString("block-%1").arg(blockItem.setup.id) : blockItem.setup.name;
            e.description = makeToolTip("block", blockItem.setup);
            e.elementId = blockItem.setup.id;
            e.isCustom = blockCustomId.contains(blockItem.setup.id);
            e.isValid = true;
            m_blockModel->addElement(e, blockItem.setup.group, blockItem.setup.category);
        }

        //apply group list
        ui->Sel_Combo_GroupsBlock->clear();
        ui->Sel_Combo_GroupsBlock->addItems(m_blockModel->getGroupsList(m_allLabel));
        //apply category list
        ui->Sel_Combo_CategoryBlock->clear();
        ui->Sel_Combo_CategoryBlock->addItems(m_blockModel->getCategoriesList(m_allLabel));

        m_blockModel->addElementsEnd();
    }

    if(bgoTab)
    {
        m_bgoModel->addElementsBegin();
        if((noEmptyTypes & TAB_BGO) == 0)
        {
            ItemBoxListModel::Element empBGO;
            empBGO.name = emTxt;
            empBGO.isValid = true;
            empBGO.pixmap = emptyPixmap(QSize(16, 16));
            empBGO.description = "Empty element";
            m_bgoModel->setSortSkipFirst(true);
            m_bgoModel->addElement(empBGO);
        }

        //set BGO item box from global configs
        QSet<uint64_t> bgoCustomId;
        if(scene_lvl)
        {
            for(int i = 0; i < scene_lvl->m_customBGOs.size(); i++)
            {
                obj_bgo &bgo = *scene_lvl->m_customBGOs[i];
                bgoCustomId.insert(bgo.setup.id);
            }
        }

        PGE_DataArray<obj_bgo> *array = scene_lvl ? &scene_lvl->m_localConfigBGOs : &conf->main_bgo;
        for(int i = 1; i < array->size(); i++) //Add user images
        {
            obj_bgo &bgoItem = (*array)[i];
            ItemBoxListModel::Element e;
            Items::getItemGFX(&bgoItem, e.pixmap, false, QSize(16, 16));
            e.name = bgoItem.setup.name.isEmpty() ? QString("bgo-%1").arg(bgoItem.setup.id) : bgoItem.setup.name;
            e.description = makeToolTip("bgo", bgoItem.setup);
            e.elementId = bgoItem.setup.id;
            e.isCustom = bgoCustomId.contains(bgoItem.setup.id);
            e.isValid = true;
            m_bgoModel->addElement(e, bgoItem.setup.group, bgoItem.setup.category);
        }

        //apply group list
        ui->Sel_Combo_GroupsBGO->clear();
        ui->Sel_Combo_GroupsBGO->addItems(m_bgoModel->getGroupsList(m_allLabel));
        //apply category list
        ui->Sel_Combo_CategoryBGO->clear();
        ui->Sel_Combo_CategoryBGO->addItems(m_bgoModel->getCategoriesList(m_allLabel));

        m_bgoModel->addElementsEnd();
    }

    if(npcTab)
    {
        m_npcModel->addElementsBegin();
        if((noEmptyTypes & TAB_NPC) == 0)
        {
            ItemBoxListModel::Element empNPC;
            empNPC.name = emTxt;
            empNPC.isValid = true;
            empNPC.pixmap = emptyPixmap(QSize(16, 16));
            empNPC.description = "Empty element";
            m_npcModel->setSortSkipFirst(true);
            m_npcModel->addElement(empNPC);
        }

        QSet<uint64_t> npcCustomId;
        if(scene_lvl)
        {
            for(int i = 0; i < scene_lvl->m_customNPCs.size(); i++)
            {
                obj_npc &npc = *scene_lvl->m_customNPCs[i];
                npcCustomId.insert(npc.setup.id);
            }
        }

        PGE_DataArray<obj_npc> *array = scene_lvl ? &scene_lvl->m_localConfigNPCs : &conf->main_npc;
        for(int i = 1; i < array->size(); i++) //Add user images
        {
            obj_npc &npcItem = (*array)[i];
            ItemBoxListModel::Element e;
            Items::getItemGFX(&npcItem, e.pixmap, false, QSize(16, 16));
            e.name = npcItem.setup.name.isEmpty() ? QString("npc-%1").arg(npcItem.setup.id) : npcItem.setup.name;
            e.description = makeToolTip("npc", npcItem.setup);
            e.elementId = npcItem.setup.id;
            e.isCustom = npcCustomId.contains(npcItem.setup.id);
            e.isValid = true;
            m_npcModel->addElement(e, npcItem.setup.group, npcItem.setup.category);
        }

        //apply group list
        ui->Sel_Combo_GroupsNPC->clear();
        ui->Sel_Combo_GroupsNPC->addItems(m_npcModel->getGroupsList(m_allLabel));
        //apply category list
        ui->Sel_Combo_CategoryNPC->clear();
        ui->Sel_Combo_CategoryNPC->addItems(m_npcModel->getCategoriesList(m_allLabel));

        m_npcModel->addElementsEnd();
    }

    if(tileTab)
    {
        m_tileModel->addElementsBegin();
        m_tileModel->setTableMode(true, 1, 1);
        m_tileModel->setShowLabels(false);
        bool hasEmpty = false;

        if((noEmptyTypes & TAB_TILE) == 0)
        {
            ItemBoxListModel::Element empTerrain;
            empTerrain.name = emTxt;
            empTerrain.isValid = true;
            empTerrain.pixmap = emptyPixmap(QSize(32, 32));
            empTerrain.description = "Empty element";
            m_tileModel->setSortSkipFirst(true);
            m_tileModel->addElementCell(0, 0, empTerrain);
            hasEmpty = true;
        }

        QSet<uint64_t> tilesCustomId;
        if(scene_wld)
        {
            for(int i = 0; i < scene_wld->m_customTerrain.size(); i++)
            {
                obj_w_tile &tiles = *scene_wld->m_customTerrain[i];
                tilesCustomId.insert(tiles.setup.id);
            }
        }

        PGE_DataArray<obj_w_tile> *array = scene_wld ? &scene_wld->m_localConfigTerrain : &conf->main_wtiles;
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

        m_tileModel->setTableMode(true,
                                  static_cast<int>(cols + 1),
                                  static_cast<int>(rows + (hasEmpty ? 1 : 0) + 1));

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
            m_tileModel->addElementCell(static_cast<int>(tileItem.setup.col),
                                        static_cast<int>(tileItem.setup.row + (hasEmpty ? 1 : 0)),
                                        e, tileItem.setup.group, tileItem.setup.category);
        }

        //apply group list
        ui->Sel_Combo_GroupsTile->clear();
        ui->Sel_Combo_GroupsTile->addItems(m_tileModel->getGroupsList(m_allLabel));
        //apply category list
        ui->Sel_Combo_CategoryTile->clear();
        ui->Sel_Combo_CategoryTile->addItems(m_tileModel->getCategoriesList(m_allLabel));

        m_tileModel->addElementsEnd();
    }

    if(pathTab)
    {
        m_pathModel->addElementsBegin();
        m_pathModel->setTableMode(true, 1, 1);
        m_pathModel->setShowLabels(false);
        bool hasEmpty = false;

        if((noEmptyTypes & TAB_PATH) == 0)
        {
            ItemBoxListModel::Element empPath;
            empPath.name = emTxt;
            empPath.isValid = true;
            empPath.pixmap = emptyPixmap(QSize(32, 32));
            empPath.description = "Empty element";
            m_pathModel->setSortSkipFirst(true);
            m_pathModel->addElementCell(0, 0, empPath);
            hasEmpty = true;
        }

        QSet<uint64_t> pathCustomId;
        if(scene_wld)
        {
            for(int i = 0; i < scene_wld->m_customPaths.size(); i++)
            {
                obj_w_path &paths = *scene_wld->m_customPaths[i];
                pathCustomId.insert(paths.setup.id);
            }
        }

        PGE_DataArray<obj_w_path> *array = scene_wld ? &scene_wld->m_localConfigPaths : &conf->main_wpaths;
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

        m_pathModel->setTableMode(true,
                                  static_cast<int>(cols + 1),
                                  static_cast<int>(rows + (hasEmpty ? 1 : 0) + 1));

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
            m_pathModel->addElementCell(static_cast<int>(pathItem.setup.col),
                                        static_cast<int>(pathItem.setup.row + (hasEmpty ? 1 : 0)),
                                        e, pathItem.setup.group, pathItem.setup.category);
        }

        //apply group list
        ui->Sel_Combo_GroupsPath->clear();
        ui->Sel_Combo_GroupsPath->addItems(m_pathModel->getGroupsList(m_allLabel));
        //apply category list
        ui->Sel_Combo_CategoryPath->clear();
        ui->Sel_Combo_CategoryPath->addItems(m_pathModel->getCategoriesList(m_allLabel));

        m_pathModel->addElementsEnd();
    }

    if(sceneryTab)
    {
        m_sceneModel->addElementsBegin();
        if((noEmptyTypes & TAB_SCENERY) == 0)
        {
            ItemBoxListModel::Element empScenery;
            empScenery.name = emTxt;
            empScenery.isValid = true;
            empScenery.pixmap = emptyPixmap(QSize(16, 16));
            empScenery.description = "Empty element";
            m_sceneModel->setSortSkipFirst(true);
            m_sceneModel->addElement(empScenery);
        }

        QSet<uint64_t> sceneryCustomId;
        if(scene_wld)
        {
            for(int i = 0; i < scene_wld->m_customSceneries.size(); i++)
            {
                obj_w_scenery &scenery = *scene_wld->m_customSceneries[i];
                sceneryCustomId.insert(scenery.setup.id);
            }
        }

        PGE_DataArray<obj_w_scenery> *array = scene_wld ? &scene_wld->m_localConfigScenery : &conf->main_wscene;
        for(int i = 1; i < array->size(); i++)
        {
            obj_w_scenery &sceneryItem = (*array)[i];
            ItemBoxListModel::Element e;
            Items::getItemGFX(&sceneryItem, e.pixmap, false, QSize(16, 16));
            e.name = sceneryItem.setup.name.isEmpty() ? QString("scene-%1").arg(sceneryItem.setup.id) : sceneryItem.setup.name;
            e.description = makeToolTipSimple("Scenery", sceneryItem.setup);
            e.elementId = sceneryItem.setup.id;
            e.isCustom = sceneryCustomId.contains(sceneryItem.setup.id);
            e.isValid = true;
            m_sceneModel->addElement(e, sceneryItem.setup.group, sceneryItem.setup.category);
        }

        //apply group list
        ui->Sel_Combo_GroupsScenery->clear();
        ui->Sel_Combo_GroupsScenery->addItems(m_sceneModel->getGroupsList(m_allLabel));
        //apply category list
        ui->Sel_Combo_CategoryScenery->clear();
        ui->Sel_Combo_CategoryScenery->addItems(m_sceneModel->getCategoriesList(m_allLabel));

        m_sceneModel->addElementsEnd();
    }

    if(levelTab)
    {
        m_levelModel->addElementsBegin();
        if((noEmptyTypes & TAB_LEVEL) == 0)
        {
            ItemBoxListModel::Element empLevel;
            empLevel.name = emTxt;
            empLevel.isValid = true;
            empLevel.pixmap = emptyPixmap(QSize(16, 16));
            empLevel.description = "Empty element";
            m_levelModel->setSortSkipFirst(true);
            m_levelModel->addElement(empLevel);
        }

        QSet<uint64_t> levelCustomId;
        if(scene_wld)
        {
            for(int i = 0; i < scene_wld->m_customLevels.size(); i++)
            {
                obj_w_scenery &scenery = *scene_wld->m_customLevels[i];
                levelCustomId.insert(scenery.setup.id);
            }
        }

        PGE_DataArray<obj_w_level> *array = scene_wld ? &scene_wld->m_localConfigLevels : &conf->main_wlevels;
        for(int i = 0; i < array->size(); i++)
        {
            obj_w_level &levelItem = (*array)[i];
            ItemBoxListModel::Element e;
            Items::getItemGFX(&levelItem, e.pixmap, false, QSize(16, 16));
            e.name = levelItem.setup.name.isEmpty() ? QString("level-%1").arg(levelItem.setup.id) : levelItem.setup.name;
            e.description = makeToolTipSimple("Level entry point", levelItem.setup);
            e.elementId = levelItem.setup.id;
            e.isCustom = levelCustomId.contains(levelItem.setup.id);
            e.isValid = true;
            m_levelModel->addElement(e, levelItem.setup.group, levelItem.setup.category);
        }

        //apply group list
        ui->Sel_Combo_GroupsLevel->clear();
        ui->Sel_Combo_GroupsLevel->addItems(m_levelModel->getGroupsList(m_allLabel));
        //apply category list
        ui->Sel_Combo_CategoryLevel->clear();
        ui->Sel_Combo_CategoryLevel->addItems(m_levelModel->getCategoriesList(m_allLabel));

        m_levelModel->addElementsEnd();
    }

    if(musicTab)
    {
        m_musboxModel->addElementsBegin();
        if((noEmptyTypes & TAB_MUSIC) == 0)
        {
            ItemBoxListModel::Element empMusic;
            empMusic.name = emTxt;
            empMusic.isValid = true;
            empMusic.pixmap = emptyPixmap(QSize(24, 24));
            empMusic.description = "Empty element";
            m_musboxModel->setSortSkipFirst(true);
            m_musboxModel->addElement(empMusic);
        }

        for(int i = 1; i < conf->main_music_wld.size(); i++)
        {
            obj_music &musicItem = conf->main_music_wld[i];
            ItemBoxListModel::Element e;
            e.pixmap = QPixmap(":/toolbar/playmusic.png");
            e.name = musicItem.name.isEmpty() ? QString("musicbox-%1").arg(musicItem.id) : musicItem.name;
            e.description = QString("ID: %1").arg(musicItem.id);
            e.elementId = musicItem.id;
            e.isCustom = false;
            e.isValid = true;
            m_musboxModel->addElement(e);
        }
        m_musboxModel->addElementsEnd();
    }

    on_Sel_TabCon_ItemType_currentChanged(ui->Sel_TabCon_ItemType->currentIndex());

    selectListItem(ui->Sel_List_Block, m_blockModel, curSelIDBlock);
    selectListItem(ui->Sel_List_BGO, m_bgoModel, curSelIDBGO);
    selectListItem(ui->Sel_List_Scenery, m_sceneModel, curSelIDScenery);
    selectListItem(ui->Sel_List_Level, m_levelModel, curSelIDLevel);
    selectListItem(ui->Sel_List_Music, m_musboxModel, curSelIDMusic);

    selectListItem(ui->Sel_List_Tile, m_tileModel, curSelIDTile);
    selectListItem(ui->Sel_List_Path, m_pathModel, curSelIDPath);

    if(!isCoinSel)
        selectListItem(ui->Sel_List_NPC, m_npcModel, curSelIDNPC);
}

ItemSelectDialog::~ItemSelectDialog()
{
    clearFocus();
    delete ui;
}

void ItemSelectDialog::setTabsOrder(QVector<int> tabIds)
{
    QVector<QPair<QString, QWidget *>> widgets;
    while(ui->Sel_TabCon_ItemType->count())
    {
        widgets.push_back({ui->Sel_TabCon_ItemType->tabText(0), ui->Sel_TabCon_ItemType->widget(0)});
        ui->Sel_TabCon_ItemType->removeTab(0);
    }

    for(int i = 0; i < tabIds.size() && i < widgets.size(); i++)
    {
        int destType = tabIds[i];
        for(int j = 0; j < widgets.size(); j++)
        {
            int type = widgets[j].second->property("tabType").toInt();
            if(destType == type)
            {
                ui->Sel_TabCon_ItemType->addTab(widgets[j].second, widgets[j].first);
                break;
            }
        }
    }
}

void ItemSelectDialog::setMultiSelect(bool _multiselect)
{
    isMultiSelect = _multiselect;
    QAbstractItemView::SelectionMode mode = _multiselect ?
                                            QAbstractItemView::MultiSelection :
                                            QAbstractItemView::SingleSelection;

    ui->Sel_List_Block->setSelectionMode(mode);
    ui->Sel_List_BGO->setSelectionMode(mode);
    ui->Sel_List_NPC->setSelectionMode(mode);

    ui->Sel_List_Tile->setSelectionMode(mode);
    ui->Sel_List_Scenery->setSelectionMode(mode);
    ui->Sel_List_Path->setSelectionMode(mode);
    ui->Sel_List_Level->setSelectionMode(mode);
    ui->Sel_List_Music->setSelectionMode(mode);

    if(_multiselect)//Clear selection on toggling multiselection
    {
        ui->Sel_List_Block->clearSelection();
        ui->Sel_List_BGO->clearSelection();
        ui->Sel_List_NPC->clearSelection();

        ui->Sel_List_Tile->clearSelection();
        ui->Sel_List_Scenery->clearSelection();
        ui->Sel_List_Path->clearSelection();
        ui->Sel_List_Level->clearSelection();
        ui->Sel_List_Music->clearSelection();
    }
}

void ItemSelectDialog::setWorldMapRootDir(QString dir)
{
    worldMapRoot = dir;
    if(!worldMapRoot.endsWith('/'))
        worldMapRoot.append('/');
}

void ItemSelectDialog::addExtraDataControl(QWidget *control)
{
    ui->verticalLayout->insertWidget(ui->verticalLayout->count() - 1, control);
}

void ItemSelectDialog::on_Sel_TabCon_ItemType_currentChanged(int index)
{
    Q_UNUSED(index)

    if(ui->Sel_TabCon_ItemType->count())
        currentTab = ui->Sel_TabCon_ItemType->currentWidget()->property("tabType").toInt();
    else
        currentTab = 0;

    checkExtraDataVis(extraBlockWid, ui->Sel_Tab_Block);
    checkExtraDataVis(extraBGOWid, ui->Sel_Tab_BGO);
    checkExtraDataVis(extraNPCWid, ui->Sel_Tab_NPC);
    checkExtraDataVis(extraTileWid, ui->Sel_Tab_Tile);
    checkExtraDataVis(extraSceneryWid, ui->Sel_Tab_Scenery);
    checkExtraDataVis(extraPathWid, ui->Sel_Tab_Path);
    checkExtraDataVis(extraLevelWid, ui->Sel_Tab_Level);
    checkExtraDataVis(extraMusicWid, ui->Sel_Tab_Music);

    if(updateLabelVis(extraBlockWid, ui->Sel_Tab_Block))
        return;
    if(updateLabelVis(extraBGOWid, ui->Sel_Tab_BGO))
        return;
    if(updateLabelVis(extraNPCWid, ui->Sel_Tab_NPC))
        return;
    if(updateLabelVis(extraTileWid, ui->Sel_Tab_Tile))
        return;
    if(updateLabelVis(extraSceneryWid, ui->Sel_Tab_Scenery))
        return;
    if(updateLabelVis(extraPathWid, ui->Sel_Tab_Path))
        return;
    if(updateLabelVis(extraLevelWid, ui->Sel_Tab_Level))
        return;
    if(updateLabelVis(extraMusicWid, ui->Sel_Tab_Music))
        return;
}

void ItemSelectDialog::npcTypeChange(bool /*toggled*/)
{
    ui->Sel_List_NPC->setEnabled(npcFromList->isChecked());
    ui->Sel_Text_NPC->setEnabled(npcFromList->isChecked());
    ui->Sel_Combo_CategoryNPC->setEnabled(npcFromList->isChecked());
    ui->Sel_FilterSetupNPC->setEnabled(npcFromList->isChecked());
    ui->Sel_Combo_GroupsNPC->setEnabled(npcFromList->isChecked());
    npcCoinsSel->setEnabled(npcCoins->isChecked());
    isCoin = npcCoins->isChecked();
}



void ItemSelectDialog::SelListBlock_itemDoubleClicked(const QModelIndex &index)
{
    if(!m_blockModel->data(index, ItemBoxListModel::ItemBox_ItemIsValid).toBool())
        return;
    on_Sel_DiaButtonBox_accepted();
}

void ItemSelectDialog::SelListBGO_itemDoubleClicked(const QModelIndex &index)
{
    if(!m_bgoModel->data(index, ItemBoxListModel::ItemBox_ItemIsValid).toBool())
        return;
    on_Sel_DiaButtonBox_accepted();
}

void ItemSelectDialog::SelListNPC_itemDoubleClicked(const QModelIndex &index)
{
    if(!m_npcModel->data(index, ItemBoxListModel::ItemBox_ItemIsValid).toBool())
        return;
    on_Sel_DiaButtonBox_accepted();
}


void ItemSelectDialog::SelListTile_itemDoubleClicked(const QModelIndex &index)
{
    if(!m_tileModel->data(index, ItemBoxListModel::ItemBox_ItemIsValid).toBool())
        return;
    on_Sel_DiaButtonBox_accepted();
}

void ItemSelectDialog::SelListScenery_itemDoubleClicked(const QModelIndex &index)
{
    if(!m_sceneModel->data(index, ItemBoxListModel::ItemBox_ItemIsValid).toBool())
        return;
    on_Sel_DiaButtonBox_accepted();
}

void ItemSelectDialog::SelListPath_itemDoubleClicked(const QModelIndex &index)
{
    if(!m_pathModel->data(index, ItemBoxListModel::ItemBox_ItemIsValid).toBool())
        return;
    on_Sel_DiaButtonBox_accepted();
}

void ItemSelectDialog::SelListLevel_itemDoubleClicked(const QModelIndex &index)
{
    if(!m_levelModel->data(index, ItemBoxListModel::ItemBox_ItemIsValid).toBool())
        return;
    on_Sel_DiaButtonBox_accepted();
}

void ItemSelectDialog::SelListMusic_itemDoubleClicked(const QModelIndex &index)
{
    if(!m_musboxModel->data(index, ItemBoxListModel::ItemBox_ItemIsValid).toBool())
        return;
    on_Sel_DiaButtonBox_accepted();
}

void ItemSelectDialog::on_Sel_DiaButtonBox_accepted()
{
    blockID = 0;
    bgoID = 0;
    npcID = 0;
    tileID = 0;
    sceneryID = 0;
    pathID = 0;
    levelID = 0;
    musicID = 0;

    isCoin = (npcCoins ? npcCoins->isChecked() : false);

    blockID = extractID(ui->Sel_List_Block);
    bgoID = extractID(ui->Sel_List_BGO);
    tileID = extractID(ui->Sel_List_Tile);
    sceneryID = extractID(ui->Sel_List_Scenery);
    pathID = extractID(ui->Sel_List_Path);
    levelID = extractID(ui->Sel_List_Level);
    musicID = extractID(ui->Sel_List_Music);

    blockIDs = extractIDs(ui->Sel_List_Block);
    bgoIDs = extractIDs(ui->Sel_List_BGO);
    npcIDs = extractIDs(ui->Sel_List_NPC);
    tileIDs = extractIDs(ui->Sel_List_Tile);
    sceneryIDs = extractIDs(ui->Sel_List_Scenery);
    pathIDs = extractIDs(ui->Sel_List_Path);
    levelIDs = extractIDs(ui->Sel_List_Level);
    musicIDs = extractIDs(ui->Sel_List_Music);

    if((unsigned)musicID == conf->music_w_custom_id)
    {
        QString dirPath;
        WorldEdit *edit = MainWinConnect::pMainWin->activeWldEditWin();
        if(!edit)
        {
            return;
        }
        dirPath = edit->WldData.meta.path;
        if(edit->isUntitled())
        {
            QMessageBox::information(this, tr("Please, save file"), tr("Please, save file first, if you want to select custom music file."), QMessageBox::Ok);
            return;
        }

        MusicFileList musicList(dirPath, "");
        if(musicList.exec() == QDialog::Accepted)
            musicFile = musicList.currentFile();
        else
            return;
    }

    if(ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_NPC) != -1)
    {
        if(npcCoins && npcCoins->isChecked())
        {
            npcID = npcCoinsSel->value();
        }
        else
        {
            QModelIndexList list = ui->Sel_List_NPC->selectionModel()->selectedIndexes();
            if(!list.isEmpty())
            {
                npcID = m_npcModel->data(list[0], ItemBoxListModel::ItemBox_ItemId).toInt();
            }
        }
    }

    ui->Sel_Text_FilterBlock->disconnect();
    ui->Sel_Text_FilterBGO->disconnect();
    ui->Sel_Text_NPC->disconnect();
    ui->Sel_Text_Tile->disconnect();
    ui->Sel_Text_Scenery->disconnect();
    ui->Sel_Text_Path->disconnect();
    ui->Sel_Text_Level->disconnect();
    ui->Sel_Text_Music->disconnect();

    this->accept();
}

void ItemSelectDialog::on_Sel_Combo_GroupsBlock_currentIndexChanged(int index)
{
    if(lock_grp) return;
    m_blockModel->setGroupFilter(ui->Sel_Combo_GroupsBlock->itemText(index));
    lock_cat = true;
    ui->Sel_Combo_CategoryBlock->clear();
    ui->Sel_Combo_CategoryBlock->addItems(m_blockModel->getCategoriesList(m_allLabel));
    lock_cat = false;
}

void ItemSelectDialog::on_Sel_Combo_CategoryBlock_currentIndexChanged(int index)
{
    if(lock_cat) return;
    m_blockModel->setCategoryFilter(ui->Sel_Combo_CategoryBlock->itemText(index));
}

void ItemSelectDialog::on_Sel_Combo_GroupsBGO_currentIndexChanged(int index)
{
    if(lock_grp) return;
    m_bgoModel->setGroupFilter(ui->Sel_Combo_GroupsBGO->itemText(index));
    lock_cat = true;
    ui->Sel_Combo_CategoryBGO->clear();
    ui->Sel_Combo_CategoryBGO->addItems(m_bgoModel->getCategoriesList(m_allLabel));
    lock_cat = false;
}

void ItemSelectDialog::on_Sel_Combo_CategoryBGO_currentIndexChanged(int index)
{
    if(lock_cat) return;
    m_bgoModel->setCategoryFilter(ui->Sel_Combo_CategoryBGO->itemText(index));
}

void ItemSelectDialog::on_Sel_Combo_GroupsNPC_currentIndexChanged(int index)
{
    if(lock_grp) return;
    m_npcModel->setGroupFilter(ui->Sel_Combo_GroupsNPC->itemText(index));
    lock_cat = true;
    ui->Sel_Combo_CategoryNPC->clear();
    ui->Sel_Combo_CategoryNPC->addItems(m_npcModel->getCategoriesList(m_allLabel));
    lock_cat = false;
}

void ItemSelectDialog::on_Sel_Combo_CategoryNPC_currentIndexChanged(int index)
{
    if(lock_cat) return;
    m_npcModel->setCategoryFilter(ui->Sel_Combo_CategoryNPC->itemText(index));
}

void ItemSelectDialog::selectListItem(QListView *w, ItemBoxListModel *m, int itemId)
{
    QModelIndex index = m->findVisibleItemById(itemId);
    if(index.isValid())
    {
        w->selectionModel()->select(index, QItemSelectionModel::Select);
        w->scrollTo(index, QAbstractItemView::PositionAtCenter);
    }
}

void ItemSelectDialog::selectListItem(QTableView *w, ItemBoxListModel *m, int itemId)
{
    QModelIndex index = m->findVisibleItemById(itemId);
    if(index.isValid())
    {
        w->selectionModel()->select(index, QItemSelectionModel::Select);
        w->scrollTo(index, QAbstractItemView::PositionAtCenter);
    }
}

void ItemSelectDialog::selectListItem(QListView *w, int array_id)
{
    QModelIndex index = w->model()->index(array_id, 0);
    if(index.isValid())
        w->selectionModel()->select(index, QItemSelectionModel::Select);
}

void ItemSelectDialog::selectListItem(QTableWidget *w, int array_id)
{
    for(int x = 0; x < w->columnCount(); ++x)
    {
        for(int y = 0; y < w->rowCount(); ++y)
        {
            QTableWidgetItem *i = w->item(x, y);
            if(i)
            {
                if(i->data(Qt::UserRole) == array_id)
                {
                    w->setCurrentItem(i);
                }
            }
        }
    }
}

void ItemSelectDialog::checkExtraDataVis(QList<QWidget *> &l, QWidget *t)
{
    if(!l.isEmpty())
    {
        for(QList<QWidget *>::iterator it = l.begin(); it != l.end(); ++it)
        {
            ((QWidget *)(*it))->setVisible((ui->Sel_TabCon_ItemType->indexOf(t) != -1 ? ui->Sel_TabCon_ItemType->indexOf(t) == ui->Sel_TabCon_ItemType->currentIndex() : false));
        }
    }
}

bool ItemSelectDialog::updateLabelVis(QList<QWidget *> &l, QWidget *t)
{
    if((ui->Sel_TabCon_ItemType->indexOf(t) != -1 ?
        ui->Sel_TabCon_ItemType->indexOf(t) == ui->Sel_TabCon_ItemType->currentIndex()
        : false)
      )
    {
        ui->Sel_Label_ExtraData->setVisible(!l.isEmpty());
        if(l.isEmpty())
        {
            ui->horizontalLayout->removeItem(ui->verticalLayout);
        }
        else
        {
            ui->horizontalLayout->addLayout(ui->verticalLayout);
        }
        return true;
    }
    return false;
}

int ItemSelectDialog::extractID(QListView *w)
{
    if(ui->Sel_TabCon_ItemType->indexOf(w->parentWidget()) != -1)
    {
        QModelIndexList list = w->selectionModel()->selectedIndexes();
        if(!list.isEmpty())
        {
            QModelIndex item_i = list[0];
            return w->model()->data(item_i, ItemBoxListModel::ItemBox_ItemId).toInt();
        }
    }
    return 0;
}

int ItemSelectDialog::extractID(QTableView *w)
{
    if(ui->Sel_TabCon_ItemType->indexOf(w->parentWidget()) != -1)
    {
        QModelIndexList list = w->selectionModel()->selectedIndexes();
        if(!list.isEmpty())
        {
            QModelIndex item_i = list[0];
            return w->model()->data(item_i, ItemBoxListModel::ItemBox_ItemId).toInt();
        }
    }
    return 0;
}

QList<int> ItemSelectDialog::extractIDs(QListView *w)
{
    QList<int> elements;
    if(ui->Sel_TabCon_ItemType->indexOf(w->parentWidget()) != -1)
    {
        QModelIndexList list = w->selectionModel()->selectedIndexes();
        for(int i = 0; i < list.size(); i++)
        {
            QModelIndex item_i = list[i];
            elements.push_back(w->model()->data(item_i, ItemBoxListModel::ItemBox_ItemId).toInt());
        }
    }
    return elements;
}

QList<int> ItemSelectDialog::extractIDs(QTableView *w)
{
    QList<int> elements;
    if(ui->Sel_TabCon_ItemType->indexOf(w->parentWidget()) != -1)
    {
        QModelIndexList list = w->selectionModel()->selectedIndexes();
        for(int i = 0; i < list.size(); i++)
        {
            QModelIndex item_i = list[i];
            elements.push_back(w->model()->data(item_i, ItemBoxListModel::ItemBox_ItemId).toInt());
        }
    }
    return elements;
}
