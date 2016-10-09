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

LevelItemBox::LevelItemBox(QWidget *parent) :
    QDockWidget(parent),
    MWDock_Base(parent),
    ui(new Ui::LevelItemBox)
{
    setVisible(false);
    setAttribute(Qt::WA_ShowWithoutActivating);
    ui->setupUi(this);
    this->setAttribute(Qt::WA_X11DoNotAcceptFocus, true);

    allLabel = "[all]";
    customLabel = "[custom]";

    lock_grp=false;
    lock_cat=false;

    grp_blocks = "";
    grp_bgo = "";
    grp_npc = "";

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
    setLvlItemBoxes();
}

void MainWindow::on_actionLVLToolBox_triggered(bool checked)
{
    dock_LvlItemBox->setVisible(checked);
    if(checked) dock_LvlItemBox->raise();
}

void LevelItemBox::setLvlItemBoxes(bool setGrp, bool setCat)
{
//    if( (mw()->activeChildWindow() != 1) )
//        return;
    if( (setGrp) && ( mw()->activeChildWindow() != 1 ) )
        return;

    LevelEdit *edit = mw()->activeLvlEditWin();
    if((edit==NULL) || (!edit->sceneCreated))
        return;

    LvlScene* scene = edit->scene;
    if(!scene)
        return;

    allLabel    = MainWindow::tr("[all]");
    customLabel = MainWindow::tr("[custom]");

    mw()->ui->menuNew->setEnabled(false);
    mw()->ui->actionNew->setEnabled(false);

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

    util::memclear(ui->BGOItemsList);
    util::memclear(ui->BlockItemsList);
    util::memclear(ui->NPCItemsList);

    LogDebug("LevelTools -> Declare new");
    QListWidgetItem * item;
    QPixmap tmpI;

    QStringList tmpList, tmpGrpList;
    bool needToAdd = false;

    tmpList.clear();
    tmpGrpList.clear();

    LogDebug("LevelTools -> List ob blocks");
    //set custom Block items from loaded level
    if( (ui->BlockCatList->currentText()==customLabel) && (setCat) && (setGrp) )
    {
        for(int i=0; i<edit->scene->m_customBlocks.size(); i++)
        {
            obj_block &block = *scene->m_customBlocks[i];
            Items::getItemGFX(&block, tmpI, false, QSize(48,48));
            item = new QListWidgetItem( block.setup.name );
            item->setIcon( QIcon( tmpI ) );
            item->setToolTip( QString("ID: %1").arg(block.setup.id) );
            item->setData(Qt::UserRole, int(block.setup.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

            ui->BlockItemsList->addItem( item );
        }
    }
    else
    //set Block item box from global configs
    for(int i=1; i < scene->m_localConfigBlocks.size(); i++)
    {
        obj_block &blockItem =  scene->m_localConfigBlocks[i];

        //Add Group
        needToAdd = true;
        if( blockItem.setup.group.isEmpty() )
        {
            needToAdd=false;
        } //Skip empty values
        else
        if(!tmpList.isEmpty())
        {
            foreach( QString grp, tmpGrpList )
            {
                if(blockItem.setup.group == grp)
                {
                    needToAdd=false; break;
                }
            }
        }
        if(needToAdd)
        {
            tmpGrpList.push_back(blockItem.setup.group);
        }

        //Add category
        needToAdd = true;
        if( (blockItem.setup.group != grp_blocks) && (grp_blocks != allLabel) )
        {
            needToAdd=false;
        }
        else if( !tmpList.isEmpty() )
        {
            foreach( QString cat, tmpList )
            {
                if( blockItem.setup.category == cat )
                {
                    needToAdd=false; break;
                }
            }
        }

        if(needToAdd)
        {
            tmpList.push_back(blockItem.setup.category);
        }

        if(
            ( (blockItem.setup.group == grp_blocks) || ( grp_blocks == allLabel) || (grp_blocks == "") )&&
            ( (blockItem.setup.category == cat_blocks) || ( cat_blocks == allLabel) )
          )
        {
            Items::getItemGFX(&blockItem, tmpI, false, QSize(48,48));
            item = new QListWidgetItem( blockItem.setup.name );
            item->setIcon( QIcon( tmpI ) );
            item->setToolTip( QString("ID: %1").arg(blockItem.setup.id) );
            item->setData(Qt::UserRole, int(blockItem.setup.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

            ui->BlockItemsList->addItem( item );
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
        ui->BlockGroupList->clear();
        ui->BlockGroupList->addItems(tmpGrpList);
    }

    //apply category list
    if(!setCat)
    {
        ui->BlockCatList->clear();
        ui->BlockCatList->addItems(tmpList);
    }

    tmpList.clear();
    tmpGrpList.clear();

    LogDebug("LevelTools -> List ob BGOs");
    //set custom BGO items from loaded level
    if((ui->BGOCatList->currentText()==customLabel)&&(setCat)&&(setGrp))
    {
        for(int i=0; i < scene->m_customBGOs.size(); i++)
        {
            obj_bgo &bgo = *scene->m_customBGOs[i];
            Items::getItemGFX(&bgo, tmpI, false, QSize(48,48));

            item = new QListWidgetItem( bgo.setup.name );
            item->setIcon( QIcon( tmpI ) );
            item->setToolTip( QString("ID: %1").arg(bgo.setup.id) );
            item->setData(Qt::UserRole, int(bgo.setup.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );
            ui->BGOItemsList->addItem( item );
        }
    }
    else
    //set BGO item box from global array
    for(int i=1; i<scene->m_localConfigBGOs.size(); i++)
    {
        obj_bgo &bgoItem = scene->m_localConfigBGOs[i];

        //Add Group
        needToAdd = true;
        if(bgoItem.setup.group.isEmpty())
        {
            needToAdd=false;
        }//Skip empty values
        else
        if(!tmpList.isEmpty())
        {
            foreach(QString grp, tmpGrpList)
            {
                if(bgoItem.setup.group==grp)
                {
                    needToAdd=false;
                    break;
                }
            }
        }
        if(needToAdd)
        {
            tmpGrpList.push_back(bgoItem.setup.group);
        }

        //Add category
        needToAdd = true;
        if( (bgoItem.setup.group != grp_bgo) && (grp_bgo != allLabel) )
        {
            needToAdd = false;
        }
        else
        if(!tmpList.isEmpty())
        {
            foreach(QString cat, tmpList)
            {
                if(bgoItem.setup.category==cat)
                {
                    needToAdd = false; break;
                }
            }
        }

        if(needToAdd)
        {
            tmpList.push_back(bgoItem.setup.category);
        }

        if(
            ( (bgoItem.setup.group==grp_bgo) || (grp_bgo==allLabel) || (grp_bgo=="") )&&
            ( (bgoItem.setup.category==cat_bgos) || (cat_bgos==allLabel) )
          )
        {
            Items::getItemGFX(&bgoItem, tmpI, false, QSize(48,48));
            item = new QListWidgetItem( bgoItem.setup.name );
            item->setIcon( QIcon( tmpI ) );
            item->setToolTip( QString("ID: %1").arg(bgoItem.setup.id) );
            item->setData(Qt::UserRole, int(bgoItem.setup.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

            ui->BGOItemsList->addItem( item );
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
        ui->BGOGroupList->clear();
        ui->BGOGroupList->addItems(tmpGrpList);
    }
    //apply category list
    if(!setCat)
    {
        ui->BGOCatList->clear();
        ui->BGOCatList->addItems(tmpList);
    }

    tmpList.clear();
    tmpGrpList.clear();


    LogDebug("LevelTools -> List ob NPCs");
    //set custom NPC items from loaded level
    if( (ui->NPCCatList->currentText()==customLabel) && (setCat) && (setGrp) )
    {
        for(int i=0; i< scene->m_customNPCs.size(); i++)
        {
            obj_npc &npc = *edit->scene->m_customNPCs[i];

            Items::getItemGFX(&npc, tmpI, false, QSize(48,48));

            item = new QListWidgetItem( npc.setup.name.isEmpty() ? QString("npc-%1").arg(npc.setup.id) : npc.setup.name );
            item->setIcon( QIcon( tmpI ) );
            item->setToolTip( QString("ID: %1").arg(npc.setup.id) );
            item->setData(Qt::UserRole, int(npc.setup.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

            ui->NPCItemsList->addItem( item );
        }
    }
    else
    //set NPC item box from global config
    for(int i=1; i < scene->m_localConfigNPCs.size(); i++)
    {
        obj_npc &npcItem = scene->m_localConfigNPCs[i];

        //Add Group
        needToAdd = true;
        if( npcItem.setup.group.isEmpty() )
        {
            needToAdd=false;
        }//Skip empty values
        else
        if(!tmpList.isEmpty())
        {
            foreach(QString grp, tmpGrpList)
            {
                if( npcItem.setup.group==grp )
                {
                    needToAdd=false; break;
                }
            }
        }

        if(needToAdd)
        {
            tmpGrpList.push_back(npcItem.setup.group);
        }

        //Add category
        needToAdd = true;
        if( (npcItem.setup.group != grp_npc) && (grp_npc != allLabel) )
        {
            needToAdd = false;
        }
        else
        if( !tmpList.isEmpty() )
        {
            foreach(QString cat, tmpList)
            {
                if(npcItem.setup.category==cat)
                {
                    needToAdd = false; break;
                }
            }
        }
        if(needToAdd)
        {
            tmpList.push_back(npcItem.setup.category);
        }

        if(
            ( (npcItem.setup.group==grp_npc) || (grp_npc==allLabel) || (grp_npc=="") )&&
            ( (npcItem.setup.category==cat_npcs) || (cat_npcs==allLabel) )
          )
        {
            Items::getItemGFX(&npcItem, tmpI, false, QSize(48,48));
            item = new QListWidgetItem( npcItem.setup.name );
            item->setIcon( QIcon( tmpI ) );
            item->setToolTip( QString("ID: %1").arg(npcItem.setup.id) );
            item->setData(Qt::UserRole, int(npcItem.setup.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

            ui->NPCItemsList->addItem( item );
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
        ui->NPCGroupList->clear();
        ui->NPCGroupList->addItems(tmpGrpList);
    }

    //apply category list
    if(!setCat)
    {
        ui->NPCCatList->clear();
        ui->NPCCatList->addItems(tmpList);
    }

    lock_grp=false;
    lock_cat=false;

    updateFilters();

    mw()->ui->menuNew->setEnabled(true);
    mw()->ui->actionNew->setEnabled(true);
}

void LevelItemBox::on_BlockItemsList_customContextMenuRequested(const QPoint &pos)
{
    if(ui->BlockItemsList->selectedItems().isEmpty())
        return;

    LevelEdit * edit = mw()->activeLvlEditWin();
    if(!edit)
        return;

    QListWidgetItem *item = ui->BlockItemsList->selectedItems()[0];
    QString episodeDir = edit->LvlData.meta.path;
    QString customDir  = edit->LvlData.meta.path + "/" + edit->LvlData.meta.filename;
    int itemID = item->data(Qt::UserRole).toInt();

    obj_block &block = mw()->configs.main_block[itemID];
    QString newImg = block.setup.image_n;
    if(newImg.endsWith(".gif", Qt::CaseInsensitive))
        newImg.replace(newImg.size()-4, 4, ".png");
    QPixmap &orig = block.image;

    QMenu itemMenu(this);
    QAction *copyToC = itemMenu.addAction( tr("Copy graphic to custom folder") );
    QAction *copyToE = itemMenu.addAction( tr("Copy graphic to episode folder") );
    QAction *reply = itemMenu.exec( ui->BlockItemsList->mapToGlobal(pos) );

    if(reply==copyToC)
    {
        if(!QFile::exists(customDir + "/" + newImg))
            orig.save(customDir + "/" + newImg, "PNG");
    }
    else
    if(reply==copyToE)
    {
        if(!QFile::exists(episodeDir + "/" + newImg))
            orig.save(episodeDir + "/" + newImg, "PNG");
    }
}

void LevelItemBox::on_BGOItemsList_customContextMenuRequested(const QPoint &pos)
{
    if(ui->BGOItemsList->selectedItems().isEmpty())
        return;

    LevelEdit * edit = mw()->activeLvlEditWin();
    if(!edit)
        return;

    QListWidgetItem *item = ui->BGOItemsList->selectedItems()[0];
    QString episodeDir = edit->LvlData.meta.path;
    QString customDir  = edit->LvlData.meta.path + "/" + edit->LvlData.meta.filename;
    int itemID = item->data(Qt::UserRole).toInt();

    obj_bgo &bgo = mw()->configs.main_bgo[itemID];
    QString newImg = bgo.setup.image_n;
    if(newImg.endsWith(".gif", Qt::CaseInsensitive))
        newImg.replace(newImg.size()-4, 4, ".png");
    QPixmap &orig = bgo.image;

    QMenu itemMenu(this);
    QAction *copyToC = itemMenu.addAction( tr("Copy graphic to custom folder") );
    QAction *copyToE = itemMenu.addAction( tr("Copy graphic to episode folder") );
    QAction *reply = itemMenu.exec( ui->BlockItemsList->mapToGlobal(pos) );

    if(reply==copyToC)
    {
        if(!QFile::exists(customDir + "/" + newImg))
            orig.save(customDir + "/" + newImg, "PNG");
    }
    else
    if(reply==copyToE)
    {
        if(!QFile::exists(episodeDir + "/" + newImg))
            orig.save(episodeDir + "/" + newImg, "PNG");
    }
}

void LevelItemBox::on_NPCItemsList_customContextMenuRequested(const QPoint &pos)
{
    if(ui->NPCItemsList->selectedItems().isEmpty())
        return;

    LevelEdit * edit = mw()->activeLvlEditWin();
    if(!edit)
        return;

    QListWidgetItem *item = ui->NPCItemsList->selectedItems()[0];
    QString episodeDir = edit->LvlData.meta.path;
    QString customDir  = edit->LvlData.meta.path + "/" + edit->LvlData.meta.filename;
    int itemID = item->data(Qt::UserRole).toInt();

    obj_npc &npc = mw()->configs.main_npc[itemID];
    QString newImg = npc.setup.image_n;
    if(newImg.endsWith(".gif", Qt::CaseInsensitive))
        newImg.replace(newImg.size()-4, 4, ".png");
    QPixmap &orig = npc.image;

    QMenu itemMenu(this);
    QAction *copyToC = itemMenu.addAction( tr("Copy graphic to custom folder") );
    QAction *copyToE = itemMenu.addAction( tr("Copy graphic to episode folder") );
    QAction *reply = itemMenu.exec( ui->BlockItemsList->mapToGlobal(pos) );

    if(reply==copyToC)
    {
        if(!QFile::exists(customDir + "/" + newImg))
            orig.save(customDir + "/" + newImg, "PNG");
    }
    else
    if(reply==copyToE)
    {
        if(!QFile::exists(episodeDir + "/" + newImg))
            orig.save(episodeDir + "/" + newImg, "PNG");
    }
}


// ///////////////////////////////////
void LevelItemBox::on_BlockGroupList_currentIndexChanged(const QString &arg1)
{
    if(lock_grp) return;
    grp_blocks=arg1;
    setLvlItemBoxes(true);
}

void LevelItemBox::on_BGOGroupList_currentIndexChanged(const QString &arg1)
{
    if(lock_grp) return;
    grp_bgo=arg1;
    setLvlItemBoxes(true);
}

void LevelItemBox::on_NPCGroupList_currentIndexChanged(const QString &arg1)
{
    if(lock_grp) return;
    grp_npc=arg1;
    setLvlItemBoxes(true);
}

// ///////////////////////////////////
void LevelItemBox::on_BlockCatList_currentIndexChanged(const QString &arg1)
{
    if(lock_cat) return;
    cat_blocks=arg1;
    setLvlItemBoxes(true, true);
}


void LevelItemBox::on_BGOCatList_currentIndexChanged(const QString &arg1)
{
    if(lock_cat) return;
    cat_bgos=arg1;
    setLvlItemBoxes(true, true);
}


void LevelItemBox::on_NPCCatList_currentIndexChanged(const QString &arg1)
{
    if(lock_cat) return;
    cat_npcs=arg1;
    setLvlItemBoxes(true, true);
}




// ///////////////////////////////////

void LevelItemBox::on_BGOUniform_clicked(bool checked)
{
    ui->BGOItemsList->setUniformItemSizes(checked);
    setLvlItemBoxes(true, true);
}

void LevelItemBox::on_BlockUniform_clicked(bool checked)
{
    ui->BlockItemsList->setUniformItemSizes(checked);
    setLvlItemBoxes(true, true);
}


void LevelItemBox::on_NPCUniform_clicked(bool checked)
{
    ui->NPCItemsList->setUniformItemSizes(checked);
    setLvlItemBoxes(true, true);
}


// ///////////////////////////////////

void LevelItemBox::on_BlockItemsList_itemClicked(QListWidgetItem *item)
{
    //placeBlock
    if ((mw()->activeChildWindow()==1) && (ui->BlockItemsList->hasFocus()))
    {
        mw()->SwitchPlacingItem(ItemTypes::LVL_Block, item->data(Qt::UserRole).toInt());
    }
}

void LevelItemBox::on_BGOItemsList_itemClicked(QListWidgetItem *item)
{
    //placeBGO
    if ((mw()->activeChildWindow()==1) && (ui->BGOItemsList->hasFocus()))
    {
        mw()->SwitchPlacingItem(ItemTypes::LVL_BGO, item->data(Qt::UserRole).toInt());
    }
}

void LevelItemBox::on_NPCItemsList_itemClicked(QListWidgetItem *item)
{
    //placeNPC
    if ((mw()->activeChildWindow()==1) && (ui->NPCItemsList->hasFocus()))
    {
        mw()->SwitchPlacingItem(ItemTypes::LVL_NPC, item->data(Qt::UserRole).toInt());
    }
}

void LevelItemBox::updateFilters()
{
    int current = ui->LevelToolBoxTabs->currentIndex();
    if(current == 0){
        util::updateFilter(ui->BlockFilterField, ui->BlockItemsList, ui->BlockFilterType);
    }else if(current == 1){
        util::updateFilter(ui->BGOFilterField, ui->BGOItemsList, ui->BGOFilterType);
    }else if(current == 2){
        util::updateFilter(ui->NPCFilterField, ui->NPCItemsList, ui->NPCFilterType);
    }
}

void LevelItemBox::clearFilter()
{
    ui->BlockFilterField->setText("");
    ui->BGOFilterField->setText("");
    ui->NPCFilterField->setText("");
    updateFilters();
}

void LevelItemBox::on_BlockFilterField_textChanged(const QString &arg1)
{
    updateFilters();

    if(arg1.isEmpty()) return; //Dummy
}

void LevelItemBox::on_BlockFilterType_currentIndexChanged(int /*index*/)
{
    updateFilters();
}

void LevelItemBox::on_BGOFilterField_textChanged(const QString &arg1)
{
    updateFilters();

    if(arg1.isEmpty()) return; //Dummy
}

void LevelItemBox::on_BGOFilterType_currentIndexChanged(int /*index*/)
{
    updateFilters();
}

void LevelItemBox::on_NPCFilterField_textChanged(const QString &arg1)
{
    updateFilters();
    if(arg1.isEmpty()) return; //Dummy
}

void LevelItemBox::on_NPCFilterType_currentIndexChanged(int /*index*/)
{
    updateFilters();
}


