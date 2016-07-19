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

#include <common_features/main_window_ptr.h>
#include <common_features/util.h>
#include <common_features/graphics_funcs.h>
#include <common_features/data_array.h>
#include <common_features/items.h>

#include <editing/_dialogs/musicfilelist.h>

#include "itemselectdialog.h"
#include <ui_itemselectdialog.h>

static QString allLabel = "[all]";
static QString customLabel = "[custom]";
static QString grp_blocks = "";
static QString grp_bgo = "";
static QString grp_npc = "";
static bool lock_grp=false;
static bool lock_cat=false;

QString allWLabel_F = "[all]";
QString customWLabel_F = "[custom]";

bool lock_Wgrp_F=false;
bool lock_Wcat_F=false;

static QString grp_tiles = "";
static QString grp_paths = "";
static QString grp_scenes = "";

ItemSelectDialog::ItemSelectDialog(dataconfigs *conf, int tabs, int npcExtraData,
                                   int curSelIDBlock, int curSelIDBGO, int curSelIDNPC, int curSelIDTile, int curSelIDScenery, int curSelIDPath, int curSelIDLevel, int curSelIDMusic, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ItemSelectDialog)
{

    this->conf = conf;
    removalFlags = 0;
    ui->setupUi(this);

    isMultiSelect=false;

    LvlScene *scene_lvl = nullptr;
    WldScene *scene_wld = nullptr;
    LevelEdit *edit_lvl = MainWinConnect::pMainWin->activeLvlEditWin();
    if(edit_lvl)
    {
        scene_lvl = edit_lvl->sceneCreated ? edit_lvl->scene : 0;
    }
    WorldEdit *edit_wld = MainWinConnect::pMainWin->activeWldEditWin();
    if(edit_wld)
    {
        scene_wld = edit_wld->sceneCreated ? edit_wld->scene : 0;
    }

    QFont font;
    font.setItalic(true);
    QListWidgetItem * empBlock = new QListWidgetItem(ui->Sel_List_Block);
    QListWidgetItem * empBGO = new QListWidgetItem(ui->Sel_List_BGO);
    QListWidgetItem * empNPC = new QListWidgetItem(ui->Sel_List_NPC);
    QListWidgetItem * empScenery = new QListWidgetItem(ui->Sel_List_Scenery);
    QListWidgetItem * empLevel = new QListWidgetItem(ui->Sel_List_Level);
    QListWidgetItem * empMusic = new QListWidgetItem(ui->Sel_List_Music);
        empBlock->setFont(font);
        empBGO->setFont(font);
        empNPC->setFont(font);
        empScenery->setFont(font);
        empLevel->setFont(font);
        empMusic->setFont(font);
    QString emTxt = tr("[Empty]");
    empBlock->setText(emTxt);
    empBGO->setText(emTxt);
    empNPC->setText(emTxt);
    empScenery->setText(emTxt);
    empLevel->setText(emTxt);
    empMusic->setText(emTxt);
    empBlock->setData(3, QVariant(0));
    empBGO->setData(3, QVariant(0));
    empNPC->setData(3, QVariant(0));
    empScenery->setData(3, QVariant(0));
    empLevel->setData(3, QVariant(0));
    empMusic->setData(3, QVariant(0));

    ui->Sel_List_Block->insertItem(0,empBlock);
    ui->Sel_List_BGO->insertItem(0,empBGO);
    ui->Sel_List_NPC->insertItem(0,empNPC);
    ui->Sel_List_Scenery->insertItem(0,empScenery);
    ui->Sel_List_Level->insertItem(0,empLevel);
    ui->Sel_List_Music->insertItem(0,empMusic);

    bool blockTab = tabs & TAB_BLOCK;
    bool bgoTab = tabs & TAB_BGO;
    bool npcTab = tabs & TAB_NPC;
    bool tileTab = tabs & TAB_TILE;
    bool sceneryTab = tabs & TAB_SCENERY;
    bool pathTab = tabs & TAB_PATH;
    bool levelTab = tabs & TAB_LEVEL;
    bool musicTab = tabs & TAB_MUSIC;
    bool isCoinSel = npcExtraData & NPCEXTRA_ISCOINSELECTED;

    if(!blockTab)
        ui->Sel_TabCon_ItemType->removeTab(ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_Block));

    if(!bgoTab)
        ui->Sel_TabCon_ItemType->removeTab(ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_BGO));

    if(!npcTab)
        ui->Sel_TabCon_ItemType->removeTab(ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_NPC));

    if(!tileTab)
        ui->Sel_TabCon_ItemType->removeTab(ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_Tile));

    if(!sceneryTab)
        ui->Sel_TabCon_ItemType->removeTab(ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_Scenery));

    if(!pathTab)
        ui->Sel_TabCon_ItemType->removeTab(ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_Path));

    if(!levelTab)
        ui->Sel_TabCon_ItemType->removeTab(ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_Level));

    if(!musicTab)
        ui->Sel_TabCon_ItemType->removeTab(ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_Music));


    if(npcExtraData & NPCEXTRA_WITHCOINS)
    {
        npcFromList = new QRadioButton(tr("NPC from List"), this);
        npcCoins = new QRadioButton(tr("Coins"), this);
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
        npcFromList = 0;
        npcCoins = 0;
        npcCoinsSel = 0;
    }

    if(blockTab)
    {
        PGE_DataArray<obj_block>* array = scene_lvl ? &scene_lvl->m_localConfigBlocks : &conf->main_block;
        for(int i=1; i< array->size(); i++) //Add user images
        {
            obj_block &blockItem = (*array)[i];
            QPixmap tmpI;
            Items::getItemGFX(&blockItem, tmpI, false, QSize(16,16));
            QListWidgetItem* item = new QListWidgetItem( blockItem.name, ui->Sel_List_Block);
            item->setIcon( QIcon( tmpI ) );
            item->setData(3, QString::number( blockItem.id ) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );
            ui->Sel_List_Block->addItem(item);
        }
    }

    if(bgoTab)
    {
        PGE_DataArray<obj_bgo>* array = scene_lvl ? &scene_lvl->m_localConfigBGOs : &conf->main_bgo;
        for(int i=1; i< array->size(); i++) //Add user images
        {
            obj_bgo &bgoD = (*array)[i];
            QPixmap tmpI;
            Items::getItemGFX(&bgoD, tmpI, false, QSize(16,16));
            QListWidgetItem* item = new QListWidgetItem( bgoD.name, ui->Sel_List_BGO );
            item->setIcon( QIcon( tmpI ) );
            item->setData(3, QString::number(bgoD.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );
            ui->Sel_List_BGO->addItem(item);
        }
    }

    if(npcTab)
    {
        PGE_DataArray<obj_npc>* array = scene_lvl ? &scene_lvl->m_localConfigNPCs : &conf->main_npc;
        for(int i=1; i< array->size(); i++) //Add user images
        {
            obj_npc &npcItem = (*array)[i];
            QPixmap tmpI;
            Items::getItemGFX(&npcItem, tmpI, false, QSize(16,16));
            QListWidgetItem* item = new QListWidgetItem( npcItem.name, ui->Sel_List_NPC );
            item->setIcon( QIcon( tmpI ) );
            item->setData(3, QString::number(npcItem.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );
            ui->Sel_List_NPC->addItem(item);
        }
    }


    if(sceneryTab)
    {
        PGE_DataArray<obj_w_scenery>* array = scene_wld ? &scene_wld->uScenes : &conf->main_wscene;
        for(int i=1; i<array->size(); i++)
        {
            obj_w_scenery &sceneryItem = (*array)[i];
            //Add category
            QListWidgetItem* item = new QListWidgetItem(QString("scene-%1").arg(sceneryItem.id), ui->Sel_List_Scenery);
            QPixmap tmpI;
            Items::getItemGFX(&sceneryItem, tmpI, false, QSize(16,16));
            item->setIcon( QIcon( tmpI ) );
            item->setData(3, QString::number(sceneryItem.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );
            ui->Sel_List_Scenery->addItem(item);
        }
    }

    if(levelTab)
    {
        PGE_DataArray<obj_w_level>* array = scene_wld ? &scene_wld->uLevels : &conf->main_wlevels;
        for(int i=0; i<array->size(); i++)
        {
            obj_w_level &levelItem = (*array)[i];
            //Add category
            QListWidgetItem* item = new QListWidgetItem(QString("level-%1").arg(levelItem.id), ui->Sel_List_Level);
            QPixmap tmpI;
            Items::getItemGFX(&levelItem, tmpI, false, QSize(16,16));
            item->setIcon( QIcon( tmpI ) );
            item->setData(3, QString::number(levelItem.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );
            ui->Sel_List_Level->addItem(item);
        }
    }

    if(musicTab)
    {
        for(int i=1; i < conf->main_music_wld.size(); i++)
        {
            obj_music &musicItem = conf->main_music_wld[i];
            //Add category
            QListWidgetItem* item = new QListWidgetItem(QString("wldmusic-%1").arg(musicItem.id), ui->Sel_List_Music);
            item->setIcon( QIcon( QPixmap(":/images/playmusic.png") ) );
            item->setData(3, QString::number(musicItem.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );
            ui->Sel_List_Music->addItem(item);
        }
    }

    updateBoxes();
    setWldItemBoxes();

    on_Sel_TabCon_ItemType_currentChanged(ui->Sel_TabCon_ItemType->currentIndex());

    selectListItem(ui->Sel_List_Block, curSelIDBlock);
    selectListItem(ui->Sel_List_BGO, curSelIDBGO);
    selectListItem(ui->Sel_List_Scenery, curSelIDScenery);
    selectListItem(ui->Sel_List_Level, curSelIDLevel);
    selectListItem(ui->Sel_List_Music, curSelIDMusic);

    selectListItem(ui->Sel_List_Tile, curSelIDTile);
    selectListItem(ui->Sel_List_Path, curSelIDPath);

    if(!isCoinSel)
        selectListItem(ui->Sel_List_NPC, curSelIDNPC);
}

ItemSelectDialog::~ItemSelectDialog()
{
    delete ui;
}

void ItemSelectDialog::removeEmptyEntry(int tabs)
{
    if(tabs & TAB_BLOCK && ui->Sel_List_Block->item(0)->data(3).toInt() == 0)
        delete ui->Sel_List_Block->item(0);

    if(tabs & TAB_BGO && ui->Sel_List_BGO->item(0)->data(3).toInt() == 0)
        delete ui->Sel_List_BGO->item(0);

    if(tabs & TAB_NPC && ui->Sel_List_NPC->item(0)->data(3).toInt() == 0)
        delete ui->Sel_List_NPC->item(0);

    if(tabs & TAB_TILE && ui->Sel_List_Tile->item(0,0)->data(3).toInt() == 0)
        delete ui->Sel_List_Tile->item(0,0);

    if(tabs & TAB_SCENERY && ui->Sel_List_Scenery->item(0)->data(3).toInt() == 0)
        delete ui->Sel_List_Scenery->item(0);

    if(tabs & TAB_PATH && ui->Sel_List_Path->item(0,0)->data(3).toInt() == 0)
        delete ui->Sel_List_Path->item(0,0);

    if(tabs & TAB_LEVEL && ui->Sel_List_Level->item(0)->data(3).toInt() == 0)
        delete ui->Sel_List_Level->item(0);

    if(tabs & TAB_MUSIC && ui->Sel_List_Music->item(0)->data(3).toInt() == 0)
        delete ui->Sel_List_Music->item(0);

    removalFlags = tabs;

}

void ItemSelectDialog::setMultiSelect(bool _multiselect)
{
    isMultiSelect = _multiselect;
    QAbstractItemView::SelectionMode mode = _multiselect?
                                        QAbstractItemView::MultiSelection:
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
    worldMapRoot=dir;
    if(!worldMapRoot.endsWith('/'))
        worldMapRoot.append('/');
}

void ItemSelectDialog::addExtraDataControl(QWidget *control)
{
    ui->verticalLayout->insertWidget(ui->verticalLayout->count()-1, control);
}

void ItemSelectDialog::updateBoxes(bool setGrp, bool setCat)
{
    allLabel    = MainWindow::tr("[all]");
    customLabel = MainWindow::tr("[custom]");

    LvlScene *scene_lvl = nullptr;
    LevelEdit *edit_lvl = MainWinConnect::pMainWin->activeLvlEditWin();
    if(edit_lvl)
    {
        scene_lvl = edit_lvl->sceneCreated ? edit_lvl->scene : 0;
    }

    if(!setCat)
    {
        lock_cat=true;
        cat_blocks = allLabel;
        cat_bgos = allLabel;
        cat_npcs = allLabel;
        if(!setGrp)
        {
            lock_grp=true;
            grp_blocks = allLabel;
            grp_bgo = allLabel;
            grp_npc = allLabel;
        }
    }

    LogDebug("LevelTools -> Clear current");
    ui->Sel_List_BGO->clear();
    ui->Sel_List_Block->clear();
    ui->Sel_List_NPC->clear();

    LogDebug("LevelTools -> Declare new");
    QListWidgetItem * item;
    QPixmap tmpI;

    QStringList tmpList, tmpGrpList;
    bool needToAdd = true;

    tmpList.clear();
    tmpGrpList.clear();

    //set custom Block items from loaded level
    if((ui->Sel_Combo_CategoryBlock->currentText()==customLabel)&&(setCat)&&(setGrp))
    {
        if(scene_lvl)
        {
            for(int i=0; i < scene_lvl->m_customBlocks.size(); i++)
            {
                obj_block &block=*(scene_lvl->m_customBlocks[i]);
                Items::getItemGFX(&block, tmpI, false, QSize(16,16));
                item = new QListWidgetItem( block.name );
                item->setIcon( QIcon( tmpI ) );
                item->setData(3, QString::number(block.id) );
                item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

                ui->Sel_List_Block->addItem( item );
            }
        }
    }
    else
    {
        PGE_DataArray<obj_block>* array = scene_lvl ? &scene_lvl->m_localConfigBlocks : &conf->main_block;
        for(int i=1; i<array->size(); i++)
        {
            obj_block &blockItem = (*array)[i];

            //Add Group
            needToAdd = true;
            if( blockItem.group.isEmpty() )
            {
                needToAdd=false;
            } //Skip empty values
            else
            if(!tmpList.isEmpty())
            {
                foreach( QString grp, tmpGrpList )
                {
                    if(blockItem.group == grp)
                    {
                        needToAdd=false; break;
                    }
                }
            }
            if(needToAdd)
            {
                tmpGrpList.push_back(blockItem.group);
            }

            //Add category
            needToAdd = true;
            if( (blockItem.group != grp_blocks) && (grp_blocks != allLabel) )
            {
                needToAdd=false;
            }
            else if( !tmpList.isEmpty() )
            {
                foreach( QString cat, tmpList )
                {
                    if( blockItem.category == cat )
                    {
                        needToAdd=false; break;
                    }
                }
            }

            if(needToAdd)
            {
                tmpList.push_back(blockItem.category);
            }

            if(
                ( (blockItem.group == grp_blocks) || ( grp_blocks == allLabel) || (grp_blocks == "") )&&
                ( (blockItem.category == cat_blocks) || ( cat_blocks == allLabel) )
              )
            {
                Items::getItemGFX(&blockItem, tmpI, false, QSize(16,16));
                item = new QListWidgetItem( blockItem.name );
                item->setIcon( QIcon( tmpI ) );
                item->setData(3, QString::number(blockItem.id) );
                item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

                ui->Sel_List_Block->addItem( item );
            }

        }
    }
    tmpList.sort();
    tmpList.push_front(customLabel);
    tmpList.push_front(allLabel);
    tmpGrpList.sort();
    tmpGrpList.push_front(allLabel);

    //apply group list
    if(!setGrp)
    {
        ui->Sel_Combo_GroupsBlock->clear();
        ui->Sel_Combo_GroupsBlock->addItems(tmpGrpList);
    }

    //apply category list
    if(!setCat)
    {
        ui->Sel_Combo_CategoryBlock->clear();
        ui->Sel_Combo_CategoryBlock->addItems(tmpList);
    }

    tmpList.clear();
    tmpGrpList.clear();

    //set custom BGO items from loaded level
    if((ui->Sel_Combo_CategoryBGO->currentText()==customLabel)&&(setCat)&&(setGrp))
    {
        if(scene_lvl)
        {
            for(int i=0; i<scene_lvl->m_customBGOs.size(); i++)
            {
                obj_bgo &bgo = *(scene_lvl->m_customBGOs[i]);
                Items::getItemGFX(&bgo, tmpI, false, QSize(16,16));
                item = new QListWidgetItem( bgo.name );
                item->setIcon( QIcon( tmpI ) );
                item->setData(3, QString::number(bgo.id) );
                item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

                ui->Sel_List_BGO->addItem( item );
            }
        }
    }
    else
    {
        PGE_DataArray<obj_bgo>* array = scene_lvl ? &scene_lvl->m_localConfigBGOs : &conf->main_bgo;
        for(int i=1; i<array->size(); i++) //Add user images
        {
            obj_bgo &bgoItem = (*array)[i];

            //Add Group
            needToAdd = true;
            if(bgoItem.group.isEmpty())
            {
                needToAdd=false;
            }//Skip empty values
            else
            if(!tmpList.isEmpty())
            {
                foreach(QString grp, tmpGrpList)
                {
                    if(bgoItem.group==grp)
                    {
                        needToAdd=false;
                        break;
                    }
                }
            }
            if(needToAdd)
            {
                tmpGrpList.push_back(bgoItem.group);
            }

            //Add category
            needToAdd = true;
            if( (bgoItem.group != grp_bgo) && (grp_bgo != allLabel) )
            {
                needToAdd = false;
            }
            else
            if(!tmpList.isEmpty())
            {
                foreach(QString cat, tmpList)
                {
                    if(bgoItem.category==cat)
                    {
                        needToAdd = false; break;
                    }
                }
            }

            if(needToAdd)
            {
                tmpList.push_back(bgoItem.category);
            }

            if(
                ( (bgoItem.group==grp_bgo) || (grp_bgo==allLabel) || (grp_bgo=="") )&&
                ( (bgoItem.category==cat_bgos) || (cat_bgos==allLabel) )
              )
            {
                Items::getItemGFX(&bgoItem, tmpI, false, QSize(16,16));

                item = new QListWidgetItem( bgoItem.name );
                item->setIcon( QIcon( tmpI ) );
                item->setData(3, QString::number(bgoItem.id) );
                item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

                ui->Sel_List_BGO->addItem( item );
            }
        }
    }

    tmpList.sort();
    tmpList.push_front(customLabel);
    tmpList.push_front(allLabel);
    tmpGrpList.sort();
    tmpGrpList.push_front(allLabel);

    //apply group list
    if(!setGrp)
    {
        ui->Sel_Combo_GroupsBGO->clear();
        ui->Sel_Combo_GroupsBGO->addItems(tmpGrpList);
    }
    //apply category list
    if(!setCat)
    {
        ui->Sel_Combo_CategoryBGO->clear();
        ui->Sel_Combo_CategoryBGO->addItems(tmpList);
    }

    tmpList.clear();
    tmpGrpList.clear();

    //set custom NPC items from loaded level
    if((ui->Sel_Combo_CategoryNPC->currentText()==customLabel)&&(setCat)&&(setGrp))
    {
        if(edit_lvl)
        {
            for(int i=0; i<scene_lvl->m_customNPCs.size();i++)
            {
                obj_npc& npc = *(scene_lvl->m_customNPCs[i]);
                Items::getItemGFX(&npc, tmpI, false, QSize(16,16));
                item = new QListWidgetItem( npc.name );
                item->setIcon( QIcon( tmpI ) );
                item->setData(3, QString::number(npc.id) );
                item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

                ui->Sel_List_NPC->addItem( item );
            }
        }
    }
    else
    {
        PGE_DataArray<obj_npc>* array = scene_lvl ? &scene_lvl->m_localConfigNPCs : &conf->main_npc;
        for(int i=1; i<array->size(); i++) //Add user images
        {
            obj_npc &npcItem = (*array)[i];

            //Add Group
            needToAdd = true;
            if( npcItem.group.isEmpty() )
            {
                needToAdd=false;
            }//Skip empty values
            else
            if(!tmpList.isEmpty())
            {
                foreach(QString grp, tmpGrpList)
                {
                    if( npcItem.group==grp )
                    {
                        needToAdd=false; break;
                    }
                }
            }

            if(needToAdd)
            {
                tmpGrpList.push_back(npcItem.group);
            }

            //Add category
            needToAdd = true;
            if( (npcItem.group != grp_npc) && (grp_npc != allLabel) )
            {
                needToAdd = false;
            }
            else
            if( !tmpList.isEmpty() )
            {
                foreach(QString cat, tmpList)
                {
                    if(npcItem.category==cat)
                    {
                        needToAdd = false; break;
                    }
                }
            }
            if(needToAdd)
            {
                tmpList.push_back(npcItem.category);
            }

            if(
                ( (npcItem.group==grp_npc) || (grp_npc==allLabel) || (grp_npc=="") )&&
                ( (npcItem.category==cat_npcs) || (cat_npcs==allLabel) )
              )
            {
                Items::getItemGFX(&npcItem, tmpI, false, QSize(16,16));
                item = new QListWidgetItem( npcItem.name );
                item->setIcon( QIcon( tmpI ) );
                item->setData(3, QString::number(npcItem.id) );
                item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

                ui->Sel_List_NPC->addItem( item );
            }
        }
    }
    tmpList.sort();
    tmpList.push_front(customLabel);
    tmpList.push_front(allLabel);
    tmpGrpList.sort();
    tmpGrpList.push_front(allLabel);

    //apply group list
    if(!setGrp)
    {
        ui->Sel_Combo_GroupsNPC->clear();
        ui->Sel_Combo_GroupsNPC->addItems(tmpGrpList);
    }

    //apply category list
    if(!setCat)
    {
        ui->Sel_Combo_CategoryNPC->clear();
        ui->Sel_Combo_CategoryNPC->addItems(tmpList);
    }

    makeEmptyItem(ui->Sel_List_Block, TAB_BLOCK);
    makeEmptyItem(ui->Sel_List_BGO, TAB_BGO);
    makeEmptyItem(ui->Sel_List_NPC, TAB_NPC);

    lock_grp=false;
    lock_cat=false;

    updateFilters();
}

void ItemSelectDialog::setWldItemBoxes(bool setGrp, bool setCat)
{
    allWLabel_F    = MainWindow::tr("[all]");
    customWLabel_F = MainWindow::tr("[custom]");

    WldScene *scene_wld = nullptr;
    WorldEdit *edit_wld = MainWinConnect::pMainWin->activeWldEditWin();
    if(edit_wld)
    {
        scene_wld = edit_wld->sceneCreated ? edit_wld->scene : 0;
    }

    if(!setCat)
    {
        lock_Wcat_F=true;
        cat_blocks = allWLabel_F;
        cat_bgos = allWLabel_F;
        cat_npcs = allWLabel_F;
        if(!setGrp)
        {
            lock_Wgrp_F=true;
            grp_tiles = allWLabel_F;
            grp_paths = allWLabel_F;
            grp_scenes = allWLabel_F;
        }
    }

    LogDebug("WorldTools -> Clear current");

    util::memclear(ui->Sel_List_Tile);
    util::memclear(ui->Sel_List_Scenery);
    util::memclear(ui->Sel_List_Path);
    util::memclear(ui->Sel_List_Level);
    util::memclear(ui->Sel_List_Music);


    //util::memclear(ui->BlockItemsList);
    //util::memclear(ui->NPCItemsList);

    LogDebug("WorldTools -> Declare new");
    QListWidgetItem * item;
    QPixmap tmpI;

    QStringList tmpList, tmpGrpList;
    //bool found = false;

    tmpList.clear();
    tmpGrpList.clear();

    unsigned int tableRows=0;
    unsigned int tableCols=0;

    LogDebug("WorldTools -> Table size");

    int roff = (removalFlags & TAB_TILE ? 0 : 1);

    PGE_DataArray<obj_w_tile>*      array_tiles = scene_wld ? &scene_wld->uTiles : &conf->main_wtiles;
    PGE_DataArray<obj_w_scenery>*   array_scenes = scene_wld ? &scene_wld->uScenes : &conf->main_wscene;
    PGE_DataArray<obj_w_path>*      array_paths = scene_wld ? &scene_wld->uPaths : &conf->main_wpaths;
    PGE_DataArray<obj_w_level>*     array_levels = scene_wld ? &scene_wld->uLevels : &conf->main_wlevels;

    //get Table size
    for(int i=1; i<array_tiles->size(); i++)
    {
        obj_w_tile &tileItem = (*array_tiles)[i];
        if(tableRows<tileItem.row+1+roff) tableRows=tileItem.row+1+roff;
        if(tableCols<tileItem.col+1) tableCols=tileItem.col+1;
    }

    LogDebug("WorldTools -> set size");
    ui->Sel_List_Tile->setRowCount(tableRows);
    ui->Sel_List_Tile->setColumnCount(tableCols);
    ui->Sel_List_Tile->setStyleSheet("QTableWidget::item { padding: 0px; margin: 0px; }");

    LogDebug("WorldTools -> Table of tiles");
    for(int i=1; i<array_tiles->size(); i++)
    {
        obj_w_tile &tileItem = (*array_tiles)[i];
        Items::getItemGFX(&tileItem, tmpI, false, QSize(32, 32));
        QTableWidgetItem * Titem = ui->Sel_List_Tile->item(tileItem.row, tileItem.col);

        if ( (!Titem) || ( (Titem!=nullptr)&&(Titem->text().isEmpty())) )
        {
            Titem = new QTableWidgetItem();
            Titem->setIcon( QIcon( tmpI ) );
            Titem->setText( nullptr );
            Titem->setSizeHint(QSize(32,32));
            Titem->setData(3, QString::number(tileItem.id) );
            Titem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

            ui->Sel_List_Tile->setRowHeight(tileItem.row, 34);
            ui->Sel_List_Tile->setColumnWidth(tileItem.col, 34);

            ui->Sel_List_Tile->setItem(tileItem.row+roff,tileItem.col, Titem);
        }
    }

    if(roff==1){
        QTableWidgetItem * Titem = ui->Sel_List_Tile->item(0, 0);
        if ( (!Titem) || ( (Titem!=nullptr)&&(Titem->text().isEmpty())) )
        {
            Titem = new QTableWidgetItem();
            QImage I(32,32,QImage::Format_ARGB32);
            I.fill(Qt::transparent);
            QPainter e(&I);
            QFont t = e.font();
            t.setPointSize(14);
            e.setFont(t);
            e.drawText(6,2,28,28,Qt::TextSingleLine,"E");
            Titem->setIcon( QIcon( QPixmap::fromImage(I) ) );
            Titem->setText( nullptr );
            Titem->setSizeHint(QSize(32,32));
            Titem->setData(3, 0 );
            Titem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

            ui->Sel_List_Tile->setRowHeight(0, 34);
            ui->Sel_List_Tile->setColumnWidth(0, 34);

            ui->Sel_List_Tile->setItem(0,0, Titem);
        }

    }

    LogDebug("WorldTools -> List of sceneries");
    for(int i=1; i<array_scenes->size(); i++)
    {
        obj_w_scenery &sceneItem=(*array_scenes)[i];
        Items::getItemGFX(&sceneItem, tmpI, false, QSize(32, 32));
        item = new QListWidgetItem();
        item->setIcon( QIcon( tmpI ) );
        item->setText( nullptr );
        item->setData(3, QString::number(sceneItem.id) );
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

        ui->Sel_List_Scenery->addItem( item );
    }

    tableRows=0;
    tableCols=0;

    roff = (removalFlags & TAB_TILE ? 0 : 1);

    LogDebug("WorldTools -> Table of paths size");
    //get Table size
    for( int i=1; i<array_paths->size(); i++ )
    {
        obj_w_path &pathItem = (*array_paths)[i];
        if(tableRows<pathItem.row+1+roff) tableRows=pathItem.row+1+roff;
        if(tableCols<pathItem.col+1) tableCols=pathItem.col+1;
    }

    LogDebug("WorldTools -> Table of paths size define");
    ui->Sel_List_Path->setRowCount(tableRows);
    ui->Sel_List_Path->setColumnCount(tableCols);
    ui->Sel_List_Path->setStyleSheet("QTableWidget::item { padding: 0px; margin: 0px; }");

    LogDebug("WorldTools -> Table of paths");
    for( int i=1; i<array_paths->size(); i++ )
    {
        obj_w_path &pathItem = (*array_paths)[i];
        Items::getItemGFX(&pathItem, tmpI, false, QSize(32, 32));

        QTableWidgetItem * Titem = ui->Sel_List_Path->item(pathItem.row, pathItem.col);

        if ( (!Titem) || ( (Titem!=nullptr)&&(Titem->text().isEmpty())) )
        {
            Titem = new QTableWidgetItem();
            Titem->setIcon( QIcon( tmpI ) );
            Titem->setText( nullptr );
            Titem->setSizeHint(QSize(32,32));
            Titem->setData(3, QString::number(pathItem.id) );
            Titem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

            ui->Sel_List_Path->setRowHeight(pathItem.row+roff, 34);
            ui->Sel_List_Path->setColumnWidth(pathItem.col, 34);

            ui->Sel_List_Path->setItem(pathItem.row+roff,pathItem.col, Titem);
        }
    }

    if(roff==1)
    {
        QTableWidgetItem * Titem = ui->Sel_List_Path->item(0, 0);
        if ( (!Titem) || ( (Titem!=nullptr)&&(Titem->text().isEmpty())) )
        {
            Titem = new QTableWidgetItem();
            QImage I(32,32,QImage::Format_ARGB32);
            I.fill(Qt::transparent);
            QPainter e(&I);
            QFont t = e.font();
            t.setPointSize(14);
            e.setFont(t);
            e.drawText(6,2,28,28,Qt::TextSingleLine,"E");
            Titem->setIcon( QIcon( QPixmap::fromImage(I) ) );
            Titem->setText( nullptr );
            Titem->setSizeHint(QSize(32,32));
            Titem->setData(3, 0 );
            Titem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);

            ui->Sel_List_Path->setRowHeight(0, 34);
            ui->Sel_List_Path->setColumnWidth(0, 34);

            ui->Sel_List_Path->setItem(0,0, Titem);
        }

    }

    LogDebug("WorldTools -> List of levels");
    for(int i=0; i < array_levels->size(); i++)
    {
        obj_w_level &levelItem = (*array_levels)[i];
        if((conf->marker_wlvl.path==levelItem.id)||
           (conf->marker_wlvl.bigpath==levelItem.id))
            continue;

        Items::getItemGFX(&levelItem, tmpI, false, QSize(32, 32));

        item = new QListWidgetItem();
        item->setIcon( QIcon( tmpI ) );
        item->setText( nullptr );
        item->setData(3, QString::number(levelItem.id) );
        item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

        ui->Sel_List_Level->addItem( item );
    }

    LogDebug("WorldTools -> List of musics");
    for(int i=1; i<conf->main_music_wld.size(); i++)
    {
            obj_music &musicItem = conf->main_music_wld[i];
            item = new QListWidgetItem();
            item->setIcon( QIcon( QPixmap(":/images/playmusic.png").scaled( QSize(32,32), Qt::KeepAspectRatio ) ) );
            item->setText( musicItem.name );
            item->setData(3, QString::number(musicItem.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

            ui->Sel_List_Music->addItem( item );
    }


    makeEmptyItem(ui->Sel_List_Scenery, TAB_SCENERY);
    makeEmptyItem(ui->Sel_List_Level, TAB_LEVEL);
    makeEmptyItem(ui->Sel_List_Music, TAB_MUSIC);

    tmpList.clear();
    tmpGrpList.clear();

    lock_Wgrp_F=false;
    lock_Wcat_F=false;

    //updateFilters();

    LogDebug("WorldTools -> done");
}

void ItemSelectDialog::updateFilters()
{
    if(ui->Sel_TabCon_ItemType->currentWidget() == ui->Sel_Tab_Block){
        util::updateFilter(ui->Sel_Text_FilterBlock, ui->Sel_List_Block, ui->Sel_Combo_FiltertypeBlock);
    }else if(ui->Sel_TabCon_ItemType->currentWidget() == ui->Sel_Tab_BGO){
        util::updateFilter(ui->Sel_Text_FilterBGO, ui->Sel_List_BGO, ui->Sel_Combo_FiltertypeBGO);
    }else if(ui->Sel_TabCon_ItemType->currentWidget() == ui->Sel_Tab_NPC){
        util::updateFilter(ui->Sel_Text_NPC, ui->Sel_List_NPC, ui->Sel_Combo_FiltertypeNPC);
    }
}

void ItemSelectDialog::on_Sel_TabCon_ItemType_currentChanged(int index)
{
    Q_UNUSED(index)

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
    ui->Sel_Combo_FiltertypeNPC->setEnabled(npcFromList->isChecked());
    ui->Sel_Combo_GroupsNPC->setEnabled(npcFromList->isChecked());
    npcCoinsSel->setEnabled(npcCoins->isChecked());
    isCoin = npcCoins->isChecked();
}



void ItemSelectDialog::on_Sel_List_Block_itemDoubleClicked(QListWidgetItem *)
{
    on_Sel_DiaButtonBox_accepted();
}

void ItemSelectDialog::on_Sel_List_BGO_itemDoubleClicked(QListWidgetItem *)
{
    on_Sel_DiaButtonBox_accepted();
}

void ItemSelectDialog::on_Sel_List_NPC_itemDoubleClicked(QListWidgetItem *)
{
    on_Sel_DiaButtonBox_accepted();
}


void ItemSelectDialog::on_Sel_List_Tile_itemDoubleClicked(QTableWidgetItem *)
{
    on_Sel_DiaButtonBox_accepted();
}

void ItemSelectDialog::on_Sel_List_Scenery_itemDoubleClicked(QListWidgetItem *)
{
    on_Sel_DiaButtonBox_accepted();
}

void ItemSelectDialog::on_Sel_List_Path_itemDoubleClicked(QTableWidgetItem *)
{
    on_Sel_DiaButtonBox_accepted();
}

void ItemSelectDialog::on_Sel_List_Level_itemDoubleClicked(QListWidgetItem *)
{
    on_Sel_DiaButtonBox_accepted();
}

void ItemSelectDialog::on_Sel_List_Music_itemDoubleClicked(QListWidgetItem *)
{
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

    if((unsigned)musicID==conf->music_w_custom_id)
    {
        QString dirPath;
        WorldEdit * edit = MainWinConnect::pMainWin->activeWldEditWin();
        if(!edit)
        {
            return;
        }
        dirPath = edit->WldData.meta.path;
        if(edit->isUntitled)
        {
            QMessageBox::information(this, tr("Please, save file"), tr("Please, save file first, if you want to select custom music file."), QMessageBox::Ok);
            return;
        }

        MusicFileList musicList( dirPath, "" );
        if( musicList.exec() == QDialog::Accepted )
            musicFile = musicList.SelectedFile;
        else
            return;
    }

    if(ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_NPC)!=-1){
        if(npcCoins){
            if(npcCoins->isChecked()){
                npcID = npcCoinsSel->value();
            }else{
                if(!ui->Sel_List_NPC->selectedItems().isEmpty()){
                    npcID = ui->Sel_List_NPC->selectedItems()[0]->data(3).toInt();
                }
            }
        }else{
            if(!ui->Sel_List_NPC->selectedItems().isEmpty()){
                npcID = ui->Sel_List_NPC->selectedItems()[0]->data(3).toInt();
            }
        }
    }

    this->accept();
}

void ItemSelectDialog::on_Sel_Combo_GroupsBlock_currentIndexChanged(int index)
{
    if(lock_grp) return;
    grp_blocks=ui->Sel_Combo_GroupsBlock->itemText(index);
    updateBoxes(true);
}

void ItemSelectDialog::on_Sel_Combo_CategoryBlock_currentIndexChanged(int index)
{
    if(lock_cat) return;
    cat_blocks=ui->Sel_Combo_CategoryBlock->itemText(index);
    updateBoxes(true, true);
}

void ItemSelectDialog::on_Sel_Combo_GroupsBGO_currentIndexChanged(int index)
{
    if(lock_grp) return;
    grp_bgo=ui->Sel_Combo_GroupsBGO->itemText(index);
    updateBoxes(true);
}

void ItemSelectDialog::on_Sel_Combo_CategoryBGO_currentIndexChanged(int index)
{
    if(lock_cat) return;
    cat_bgos=ui->Sel_Combo_CategoryBGO->itemText(index);
    updateBoxes(true, true);
}

void ItemSelectDialog::on_Sel_Combo_GroupsNPC_currentIndexChanged(int index)
{
    if(lock_grp) return;
    grp_npc=ui->Sel_Combo_GroupsNPC->itemText(index);
    updateBoxes(true);
}

void ItemSelectDialog::on_Sel_Combo_CategoryNPC_currentIndexChanged(int index)
{
    if(lock_cat) return;
    cat_npcs=ui->Sel_Combo_CategoryNPC->itemText(index);
    updateBoxes(true, true);
}

void ItemSelectDialog::on_Sel_Text_FilterBlock_textChanged(const QString &arg1)
{
    updateFilters();
    if(arg1.isEmpty()) return; //Dummy
}

void ItemSelectDialog::on_Sel_Text_FilterBGO_textChanged(const QString &arg1)
{
    updateFilters();
    if(arg1.isEmpty()) return; //Dummy
}

void ItemSelectDialog::on_Sel_Text_NPC_textChanged(const QString &arg1)
{
    updateFilters();
    if(arg1.isEmpty()) return; //Dummy
}

void ItemSelectDialog::on_Sel_Combo_FiltertypeBlock_currentIndexChanged(int)
{
    updateFilters();
}

void ItemSelectDialog::on_Sel_Combo_FiltertypeBGO_currentIndexChanged(int)
{
    updateFilters();
}

void ItemSelectDialog::on_Sel_Combo_FiltertypeNPC_currentIndexChanged(int)
{
    updateFilters();
}

void ItemSelectDialog::selectListItem(QListWidget* w, int array_id)
{
    for(int i = 0; i < w->count(); i++){
        if(w->item(i)->data(3).toInt()==array_id){
            w->setCurrentRow(i);
        }
    }
}

void ItemSelectDialog::selectListItem(QTableWidget *w, int array_id)
{
    for(int x = 0; x < w->columnCount(); ++x){
        for(int y = 0; y < w->rowCount(); ++y){
            QTableWidgetItem* i = w->item(x,y);
            if(i){
                if(i->data(3)==array_id){
                    w->setCurrentItem(i);
                }
            }
        }
    }
}

void ItemSelectDialog::makeEmptyItem(QListWidget *wid, int rmflag)
{
    QFont font;
    font.setItalic(true);
    QListWidgetItem * empTar = new QListWidgetItem();
    empTar->setFont(font);
    QString emTxt = tr("[Empty]");
    empTar->setText(emTxt);
    empTar->setData(3, QVariant(0));
    wid->insertItem(0,empTar);
    if(removalFlags & rmflag && wid->item(0)->data(3).toInt() == 0)
        delete wid->item(0);
}

void ItemSelectDialog::checkExtraDataVis(QList<QWidget*> &l, QWidget *t)
{
    if(!l.isEmpty()){
        for(QList<QWidget*>::iterator it = l.begin(); it != l.end(); ++it){
            ((QWidget*)(*it))->setVisible((ui->Sel_TabCon_ItemType->indexOf(t)!=-1 ? ui->Sel_TabCon_ItemType->indexOf(t)==ui->Sel_TabCon_ItemType->currentIndex() : false));
        }
    }
}

bool ItemSelectDialog::updateLabelVis(QList<QWidget*> &l, QWidget *t)
{
    if(  (ui->Sel_TabCon_ItemType->indexOf(t)!=-1 ?
            ui->Sel_TabCon_ItemType->indexOf(t)==ui->Sel_TabCon_ItemType->currentIndex()
            :false)
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

int ItemSelectDialog::extractID(QListWidget *w)
{
    if(ui->Sel_TabCon_ItemType->indexOf(w->parentWidget())!=-1){
        if(!w->selectedItems().isEmpty()){
            return w->selectedItems()[0]->data(3).toInt();
        }
    }
    return 0;
}

int ItemSelectDialog::extractID(QTableWidget *w)
{
    if(ui->Sel_TabCon_ItemType->indexOf(w->parentWidget())!=-1){
        if(!w->selectedItems().isEmpty()){
            return w->selectedItems()[0]->data(3).toInt();
        }
    }
    return 0;
}

QList<int> ItemSelectDialog::extractIDs(QListWidget *w)
{
    QList<int> elements;
    if(ui->Sel_TabCon_ItemType->indexOf(w->parentWidget())!=-1){
        for(int i=0; i<w->selectedItems().size(); i++) {
            elements.push_back(w->selectedItems()[i]->data(3).toInt());
        }
    }
    return elements;
}

QList<int> ItemSelectDialog::extractIDs(QTableWidget *w)
{
    QList<int> elements;
    if(ui->Sel_TabCon_ItemType->indexOf(w->parentWidget())!=-1){
        for(int i=0; i<w->selectedItems().size(); i++) {
            elements.push_back(w->selectedItems()[i]->data(3).toInt());
        }
    }
    return elements;
}
