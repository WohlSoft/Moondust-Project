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
#include <data_configs/custom_data.h>
#include <editing/_scenes/level/lvl_item_placing.h>
#include <PGE_File_Formats/file_formats.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>

#include "lvl_item_toolbox.h"
#include "ui_lvl_item_toolbox.h"

#include "item_tooltip_make.hpp"

#include "itembox_list_model.h"

LevelItemBox::LevelItemBox(QWidget *parent) :
    QDockWidget(parent),
    MWDock_Base(parent),
    ui(new Ui::LevelItemBox)
{
    setVisible(false);
    setAttribute(Qt::WA_ShowWithoutActivating);
    ui->setupUi(this);
    this->setAttribute(Qt::WA_X11DoNotAcceptFocus, true);

    m_blockModel = new ItemBoxListModel(ui->BlockItemsList);
    ui->BlockItemsList->setModel(m_blockModel);
    connect(ui->BlockItemsList, &QListView::clicked, this, &LevelItemBox::BlockList_itemClicked);

    m_bgoModel = new ItemBoxListModel(ui->BGOItemsList);
    ui->BGOItemsList->setModel(m_bgoModel);
    connect(ui->BGOItemsList, &QListView::clicked, this, &LevelItemBox::BGOList_itemClicked);

    m_npcModel = new ItemBoxListModel(ui->NPCItemsList);
    ui->NPCItemsList->setModel(m_npcModel);
    connect(ui->NPCItemsList, &QListView::clicked, this, &LevelItemBox::NPCList_itemClicked);

    makeFilterSetupMenu(m_blockFilterSetup, m_blockModel, ui->BlockFilterField, ui->BlockItemsList, true);
    ui->BlockFilterSetup->setMenu(&m_blockFilterSetup);

    makeFilterSetupMenu(m_bgoFilterSetup, m_bgoModel, ui->BGOFilterField, ui->BGOItemsList, true);
    ui->BGOFilterSetup->setMenu(&m_bgoFilterSetup);

    makeFilterSetupMenu(m_npcFilterSetup, m_npcModel, ui->NPCFilterField, ui->NPCItemsList, true);
    ui->NPCFilterSetup->setMenu(&m_npcFilterSetup);

    mw()->addDockWidget(Qt::LeftDockWidgetArea, this);
    connect(mw(), SIGNAL(languageSwitched()), this, SLOT(re_translate()));
    connect(this, SIGNAL(visibilityChanged(bool)), mw()->ui->actionLVLToolBox, SLOT(setChecked(bool)));

    m_lastVisibilityState = isVisible();
    mw()->docks_level.
    addState(this, &m_lastVisibilityState);
}

LevelItemBox::~LevelItemBox()
{
    delete ui;
}

QTabWidget *LevelItemBox::tabWidget()
{
    return ui->LevelToolBoxTabs;
}

void LevelItemBox::re_translate()
{
    ui->retranslateUi(this);
    initItemLists();
}

void MainWindow::on_actionLVLToolBox_triggered(bool checked)
{
    dock_LvlItemBox->setVisible(checked);
    if(checked) dock_LvlItemBox->raise();
}

void LevelItemBox::initItemLists()
{
    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    LevelEdit *edit = mw()->activeLvlEditWin();
    if((edit == NULL) || (!edit->sceneCreated))
        return;

    LvlScene *scene = edit->scene;
    if(!scene)
        return;

    m_allLabel = MainWindow::tr("[all]");

    mw()->ui->menuNew->setEnabled(false);

    lock_cat = true;
    QString cat_blocks = ui->BlockCatList->count() > 0 ? ui->BlockCatList->currentText() : m_allLabel;
    QString cat_bgos = ui->BGOCatList->count() > 0 ? ui->BGOCatList->currentText() : m_allLabel;
    QString cat_npcs = ui->NPCCatList->count() > 0 ? ui->NPCCatList->currentText() : m_allLabel;

    lock_grp = true;
    QString grp_blocks = ui->BlockGroupList->count() > 0 ? ui->BlockGroupList->currentText() : m_allLabel;
    QString grp_bgo = ui->BGOGroupList->count() > 0 ? ui->BGOGroupList->currentText() : m_allLabel;
    QString grp_npc = ui->NPCGroupList->count() > 0 ? ui->NPCGroupList->currentText() : m_allLabel;

    LogDebug("LevelTools -> Clear current");

    m_blockModel->clear();
    m_bgoModel->clear();
    m_npcModel->clear();

    m_blockModel->setGroupAllKey(m_allLabel);
    m_bgoModel->setGroupAllKey(m_allLabel);
    m_npcModel->setGroupAllKey(m_allLabel);
    m_blockModel->setCategoryAllKey(m_allLabel);
    m_bgoModel->setCategoryAllKey(m_allLabel);
    m_npcModel->setCategoryAllKey(m_allLabel);



    LogDebug("LevelTools -> Fill list ob blocks");

    //set Block item box from global configs
    QSet<uint64_t> blockCustomId;
    for(int i = 0; i < edit->scene->m_customBlocks.size(); i++)
    {
        obj_block &block = *scene->m_customBlocks[i];
        blockCustomId.insert(block.setup.id);
    }

    m_blockModel->addElementsBegin(scene->m_localConfigBlocks.size());
    for(int i = 1; i < scene->m_localConfigBlocks.size(); i++)
    {
        obj_block &blockItem =  scene->m_localConfigBlocks[i];

        ItemBoxListModel::Element e;
        Items::getItemGFX(&blockItem, e.pixmap, false, QSize(48, 48));
        e.name = blockItem.setup.name.isEmpty() ? QString("block-%1").arg(blockItem.setup.id) : blockItem.setup.name;
        e.description = makeToolTip("block", blockItem.setup);
        e.elementId = blockItem.setup.id;
        e.isCustom = blockCustomId.contains(blockItem.setup.id);
        e.isValid = true;
        m_blockModel->addElement(e, blockItem.setup.group, blockItem.setup.category);
    }
    m_blockModel->addElementsEnd();

    //apply group list
    ui->BlockGroupList->clear();
    ui->BlockGroupList->addItems(m_blockModel->getGroupsList(m_allLabel));
    if(grp_blocks != m_allLabel)
    {
        ui->BlockGroupList->setCurrentText(grp_blocks);
        m_blockModel->setGroupFilter(grp_blocks);
    }

    //apply category list
    ui->BlockCatList->clear();
    ui->BlockCatList->addItems(m_blockModel->getCategoriesList(m_allLabel));
    if(cat_blocks != m_allLabel)
    {
        ui->BlockCatList->setCurrentText(cat_blocks);
        m_blockModel->setCategoryFilter(cat_blocks);
    }




    LogDebug("LevelTools -> Fill list ob BGOs");

    //set Block item box from global configs
    QSet<uint64_t> bgoCustomId;
    for(int i = 0; i < edit->scene->m_customBGOs.size(); i++)
    {
        obj_bgo &bgo = *scene->m_customBGOs[i];
        bgoCustomId.insert(bgo.setup.id);
    }

    m_bgoModel->addElementsBegin(scene->m_localConfigBGOs.size());
    for(int i = 1; i < scene->m_localConfigBGOs.size(); i++)
    {
        obj_bgo &bgoItem = scene->m_localConfigBGOs[i];
        ItemBoxListModel::Element e;
        Items::getItemGFX(&bgoItem, e.pixmap, false, QSize(48, 48));
        e.name = bgoItem.setup.name.isEmpty() ? QString("bgo-%1").arg(bgoItem.setup.id) : bgoItem.setup.name;
        e.description = makeToolTip("bgo", bgoItem.setup);
        e.elementId = bgoItem.setup.id;
        e.isCustom = bgoCustomId.contains(bgoItem.setup.id);
        e.isValid = true;
        m_bgoModel->addElement(e, bgoItem.setup.group, bgoItem.setup.category);
    }
    m_bgoModel->addElementsEnd();

    //apply group list
    ui->BGOGroupList->clear();
    ui->BGOGroupList->addItems(m_bgoModel->getGroupsList(m_allLabel));
    if(grp_bgo != m_allLabel)
    {
        ui->BGOGroupList->setCurrentText(grp_bgo);
        m_bgoModel->setGroupFilter(grp_bgo);
    }

    //apply category list
    ui->BGOCatList->clear();
    ui->BGOCatList->addItems(m_bgoModel->getCategoriesList(m_allLabel));
    if(cat_bgos != m_allLabel)
    {
        ui->BGOCatList->setCurrentText(cat_bgos);
        m_bgoModel->setCategoryFilter(cat_bgos);
    }


    LogDebug("LevelTools -> Fill list ob NPCs");
    //set custom NPC items from loaded level
    QSet<uint64_t> npcCustomId;
    for(int i = 0; i < edit->scene->m_customNPCs.size(); i++)
    {
        obj_npc &npc = *scene->m_customNPCs[i];
        npcCustomId.insert(npc.setup.id);
    }

    m_npcModel->addElementsBegin(scene->m_localConfigNPCs.size());
    for(int i = 1; i < scene->m_localConfigNPCs.size(); i++)
    {
        obj_npc &npcItem =  scene->m_localConfigNPCs[i];
        ItemBoxListModel::Element e;
        Items::getItemGFX(&npcItem, e.pixmap, false, QSize(48, 48));
        e.name = npcItem.setup.name.isEmpty() ? QString("npc-%1").arg(npcItem.setup.id) : npcItem.setup.name;
        e.description = makeToolTip("npc", npcItem.setup);
        e.elementId = npcItem.setup.id;
        e.isCustom = npcCustomId.contains(npcItem.setup.id);
        e.isValid = true;
        m_npcModel->addElement(e, npcItem.setup.group, npcItem.setup.category);
    }
    m_npcModel->addElementsEnd();

    //apply group list
    ui->NPCGroupList->clear();
    ui->NPCGroupList->addItems(m_npcModel->getGroupsList(m_allLabel));
    if(grp_npc != m_allLabel)
    {
        ui->NPCGroupList->setCurrentText(grp_npc);
        m_npcModel->setGroupFilter(grp_npc);
    }

    //apply category list
    ui->NPCCatList->clear();
    ui->NPCCatList->addItems(m_npcModel->getCategoriesList(m_allLabel));
    if(cat_npcs != m_allLabel)
    {
        ui->NPCCatList->setCurrentText(cat_npcs);
        m_npcModel->setCategoryFilter(cat_npcs);
    }

    lock_grp = false;
    lock_cat = false;

    mw()->ui->menuNew->setEnabled(true);
}

void LevelItemBox::on_BlockItemsList_customContextMenuRequested(const QPoint &pos)
{
    QModelIndexList list = ui->BlockItemsList->selectionModel()->selectedIndexes();
    if(list.isEmpty())
        return;
    LevelEdit *edit = mw()->activeLvlEditWin();
    if(!edit)
        return;

    QModelIndex item_i = list[0];
    QString episodeDir = edit->LvlData.meta.path;
    QString customDir  = edit->LvlData.meta.path + "/" + edit->LvlData.meta.filename;
    int itemID = m_blockModel->data(item_i, ItemBoxListModel::ItemBox_ItemId).toInt();

    obj_block &block = mw()->configs.main_block[itemID];
    QString newImg = block.setup.image_n;
    if(newImg.endsWith(".gif", Qt::CaseInsensitive))
        newImg.replace(newImg.size() - 4, 4, ".png");
    QPixmap &orig = block.image;

    QMenu itemMenu(this);

    if(edit->isUntitled)
    {
        QAction *nothing = itemMenu.addAction(tr("<Save file first>"));
        nothing->setEnabled(false);
        itemMenu.exec(ui->BlockItemsList->mapToGlobal(pos));
        return;
    }

    QAction *copyToC = itemMenu.addAction(tr("Copy graphic to custom folder"));
    QAction *copyToE = itemMenu.addAction(tr("Copy graphic to episode folder"));
    QAction *reply = itemMenu.exec(ui->BlockItemsList->mapToGlobal(pos));

    if(reply == copyToC)
    {
        QDir cDir(customDir);
        if(!cDir.exists())
            cDir.mkpath(customDir);
        if(!QFile::exists(customDir + "/" + newImg))
            orig.save(customDir + "/" + newImg, "PNG");
    }
    else if(reply == copyToE)
    {
        if(!QFile::exists(episodeDir + "/" + newImg))
            orig.save(episodeDir + "/" + newImg, "PNG");
    }
}

void LevelItemBox::on_BGOItemsList_customContextMenuRequested(const QPoint &pos)
{
    QModelIndexList list = ui->BGOItemsList->selectionModel()->selectedIndexes();
    if(list.isEmpty())
        return;
    LevelEdit *edit = mw()->activeLvlEditWin();
    if(!edit)
        return;

    QModelIndex item_i = list[0];
    QString episodeDir = edit->LvlData.meta.path;
    QString customDir  = edit->LvlData.meta.path + "/" + edit->LvlData.meta.filename;
    int itemID = m_bgoModel->data(item_i, ItemBoxListModel::ItemBox_ItemId).toInt();

    obj_bgo &bgo = mw()->configs.main_bgo[itemID];
    QString newImg = bgo.setup.image_n;
    if(newImg.endsWith(".gif", Qt::CaseInsensitive))
        newImg.replace(newImg.size() - 4, 4, ".png");
    QPixmap &orig = bgo.image;

    QMenu itemMenu(this);

    if(edit->isUntitled)
    {
        QAction *nothing = itemMenu.addAction(tr("<Save file first>"));
        nothing->setEnabled(false);
        itemMenu.exec(ui->BGOItemsList->mapToGlobal(pos));
        return;
    }

    QAction *copyToC = itemMenu.addAction(tr("Copy graphic to custom folder"));
    QAction *copyToE = itemMenu.addAction(tr("Copy graphic to episode folder"));
    QAction *reply = itemMenu.exec(ui->BGOItemsList->mapToGlobal(pos));

    if(reply == copyToC)
    {
        QDir cDir(customDir);
        if(!cDir.exists())
            cDir.mkpath(customDir);
        if(!QFile::exists(customDir + "/" + newImg))
            orig.save(customDir + "/" + newImg, "PNG");
    }
    else if(reply == copyToE)
    {
        if(!QFile::exists(episodeDir + "/" + newImg))
            orig.save(episodeDir + "/" + newImg, "PNG");
    }
}

void LevelItemBox::on_NPCItemsList_customContextMenuRequested(const QPoint &pos)
{
    QModelIndexList list = ui->NPCItemsList->selectionModel()->selectedIndexes();
    if(list.isEmpty())
        return;
    LevelEdit *edit = mw()->activeLvlEditWin();
    if(!edit)
        return;

    QModelIndex item_i = list[0];
    QString episodeDir = edit->LvlData.meta.path;
    QString customDir  = edit->LvlData.meta.path + "/" + edit->LvlData.meta.filename;
    int itemID = m_npcModel->data(item_i, ItemBoxListModel::ItemBox_ItemId).toInt();

    obj_npc &npc = mw()->configs.main_npc[itemID];
    QString newImg = npc.setup.image_n;
    if(newImg.endsWith(".gif", Qt::CaseInsensitive))
        newImg.replace(newImg.size() - 4, 4, ".png");
    QPixmap &orig = npc.image;

    QMenu itemMenu(this);

    if(edit->isUntitled)
    {
        QAction *nothing = itemMenu.addAction(tr("<Save file first>"));
        nothing->setEnabled(false);
        itemMenu.exec(ui->NPCItemsList->mapToGlobal(pos));
        return;
    }

    QAction *copyToC = itemMenu.addAction(tr("Copy graphic to custom folder"));
    QAction *copyToE = itemMenu.addAction(tr("Copy graphic to episode folder"));
    QAction *reply = itemMenu.exec(ui->NPCItemsList->mapToGlobal(pos));

    if(reply == copyToC)
    {
        QDir cDir(customDir);
        if(!cDir.exists())
            cDir.mkpath(customDir);
        if(!QFile::exists(customDir + "/" + newImg))
            orig.save(customDir + "/" + newImg, "PNG");
    }
    else if(reply == copyToE)
    {
        if(!QFile::exists(episodeDir + "/" + newImg))
            orig.save(episodeDir + "/" + newImg, "PNG");
    }
}


// ///////////////////////////////////
void LevelItemBox::on_BlockGroupList_currentIndexChanged(const QString &arg1)
{
    if(lock_grp)
        return;
    m_blockModel->setGroupFilter(arg1);
    lock_cat = true;
    ui->BlockCatList->clear();
    ui->BlockCatList->addItems(m_blockModel->getCategoriesList(m_allLabel));
    lock_cat = false;
}

void LevelItemBox::on_BGOGroupList_currentIndexChanged(const QString &arg1)
{
    if(lock_grp)
        return;

    m_bgoModel->setGroupFilter(arg1);

    lock_cat = true;
    ui->BGOCatList->clear();
    ui->BGOCatList->addItems(m_bgoModel->getCategoriesList(m_allLabel));
    lock_cat = false;
}

void LevelItemBox::on_NPCGroupList_currentIndexChanged(const QString &arg1)
{
    if(lock_grp)
        return;

    m_npcModel->setGroupFilter(arg1);

    lock_cat = true;
    ui->NPCCatList->clear();
    ui->NPCCatList->addItems(m_npcModel->getCategoriesList(m_allLabel));
    lock_cat = false;
}

// ///////////////////////////////////
void LevelItemBox::on_BlockCatList_currentIndexChanged(const QString &arg1)
{
    if(lock_cat)
        return;
    m_blockModel->setCategoryFilter(arg1);
}

void LevelItemBox::on_BGOCatList_currentIndexChanged(const QString &arg1)
{
    if(lock_cat)
        return;
    m_bgoModel->setCategoryFilter(arg1);
}

void LevelItemBox::on_NPCCatList_currentIndexChanged(const QString &arg1)
{
    if(lock_cat)
        return;
    m_npcModel->setCategoryFilter(arg1);
}


// ///////////////////////////////////

void LevelItemBox::BlockList_itemClicked(const QModelIndex &item)
{
    //placeBlock
    if((mw()->activeChildWindow() == MainWindow::WND_Level) && (ui->BlockItemsList->hasFocus()))
    {
        int id = m_blockModel->data(item, ItemBoxListModel::ItemBox_ItemId).toInt();
        mw()->SwitchPlacingItem(ItemTypes::LVL_Block, id);
    }
}

void LevelItemBox::BGOList_itemClicked(const QModelIndex &item)
{
    //placeBGO
    if((mw()->activeChildWindow() == MainWindow::WND_Level) && (ui->BGOItemsList->hasFocus()))
    {
        int id = m_bgoModel->data(item, ItemBoxListModel::ItemBox_ItemId).toInt();
        mw()->SwitchPlacingItem(ItemTypes::LVL_BGO, id);
    }
}

void LevelItemBox::NPCList_itemClicked(const QModelIndex &item)
{
    //placeNPC
    if((mw()->activeChildWindow() == MainWindow::WND_Level) && (ui->NPCItemsList->hasFocus()))
    {
        int id = m_npcModel->data(item, ItemBoxListModel::ItemBox_ItemId).toInt();
        mw()->SwitchPlacingItem(ItemTypes::LVL_NPC, id);
    }
}

void LevelItemBox::clearFilter()
{
    ui->BlockFilterField->setText("");
    m_blockModel->setFilterCriteria("");
    ui->BGOFilterField->setText("");
    m_bgoModel->setFilterCriteria("");
    ui->NPCFilterField->setText("");
    m_npcModel->setFilterCriteria("");
}
