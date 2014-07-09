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

#include "../../level_scene/lvl_item_placing.h"
#include "../../file_formats/file_formats.h"
#include "../../common_features/util.h"

#include "../../data_configs/custom_data.h"


static QString allLabel = "[all]";
static QString customLabel = "[custom]";

static bool lock_grp=false;
static bool lock_cat=false;


void MainWindow::UpdateCustomItems()
{
    if((ui->BlockCatList->currentText()==customLabel)||
       (ui->BGOCatList->currentText()==customLabel)||
       (ui->NPCCatList->currentText()==customLabel))
    {
        setItemBoxes(true);
    }

}

static QString grp_blocks = "";
static QString grp_bgo = "";
static QString grp_npc = "";

void MainWindow::setItemBoxes(bool setGrp, bool setCat)
{
    allLabel    = MainWindow::tr("[all]");
    customLabel = MainWindow::tr("[custom]");

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

    WriteToLog(QtDebugMsg, "LevelTools -> Clear current");

    util::memclear(ui->BGOItemsList);
    util::memclear(ui->BlockItemsList);
    util::memclear(ui->NPCItemsList);

    WriteToLog(QtDebugMsg, "LevelTools -> Declare new");
    QListWidgetItem * item;
    QPixmap tmpI;

    QStringList tmpList, tmpGrpList;
    bool found = false;

    tmpList.clear();
    tmpGrpList.clear();

    //set custom Block items from loaded level
    if((ui->BlockCatList->currentText()==customLabel)&&(setCat)&&(setGrp))
    {
        if(activeChildWindow()==1)
        {
            long j=0;
            bool isIndex=false;
            leveledit * edit = activeLvlEditWin();
            foreach(UserBlocks block, edit->scene->uBlocks)
            {

                //Check for index
                if(block.id < (unsigned long)configs.index_blocks.size())
                {
                    if(block.id == configs.main_block[configs.index_blocks[block.id].i].id)
                    {
                        j = configs.index_blocks[block.id].i;
                        isIndex=true;
                    }
                }
                //In index is false, fetch array
                if(!isIndex)
                {
                    for(int i=0; i < configs.main_block.size(); i++)
                    {
                        if(configs.main_block[i].id == block.id)
                        {
                            j = 0;
                            isIndex=true;
                            break;
                        }
                    }
                    if(!isIndex) j=0;
                }


                if(configs.main_block[j].animated)
                    tmpI = block.image.copy(0,0,
                                block.image.width(),
                                (int)round(block.image.height() / configs.main_block[j].frames));
                else
                    tmpI = block.image;

                item = new QListWidgetItem( QString("block-%1").arg(block.id) );
                item->setIcon( QIcon( tmpI ) );
                item->setData(3, QString::number(block.id) );
                item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

                ui->BlockItemsList->addItem( item );
            }

        }

    }
    else
    //set Block item box from global configs
    foreach(obj_block blockItem, configs.main_block)
    {
        //Add Group
        found = false;
        if(tmpList.size()!=0)
            foreach(QString grp, tmpGrpList)
            {
                if(blockItem.group.isEmpty())
                {found=true; break;}//Skip empty values
                if(blockItem.group==grp)
                {found=true; break;}
            }
        if(!found) tmpGrpList.push_back(blockItem.group);

        //Add category
        found = false;
        if(tmpList.size()!=0)
            foreach(QString cat, tmpList)
            {
                if(blockItem.category==cat)
                {found=true; break;}
                if((blockItem.group!=grp_blocks)&&(grp_blocks!=allLabel))
                {found=true; break;}
            }
        if(!found) tmpList.push_back(blockItem.category);

        if(
                ((blockItem.group==grp_blocks)||(grp_blocks==allLabel)||(grp_blocks==""))&&
                ((blockItem.category==cat_blocks)||(cat_blocks==allLabel)))
        {
            if(blockItem.animated)
                tmpI = blockItem.image.copy(0,0,
                            blockItem.image.width(),
                            (int)round(blockItem.image.height() / blockItem.frames));
            else
                tmpI = blockItem.image;

            item = new QListWidgetItem( blockItem.name );
            item->setIcon( QIcon( tmpI ) );
            item->setData(3, QString::number(blockItem.id) );
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

    //set custom BGO items from loaded level
    if((ui->BGOCatList->currentText()==customLabel)&&(setCat)&&(setGrp))
    {
        if(activeChildWindow()==1)
        {
            long j=0;
            bool isIndex=false;
            leveledit * edit = activeLvlEditWin();
            foreach(UserBGOs bgo, edit->scene->uBGOs)
            {

                //Check for index
                if(bgo.id < (unsigned long)configs.index_bgo.size())
                {
                    if(bgo.id == configs.main_bgo[configs.index_bgo[bgo.id].i].id)
                    {
                        j = configs.index_bgo[bgo.id].i;
                        isIndex=true;
                    }
                }
                //In index is false, fetch array
                if(!isIndex)
                {
                    for(int i=0; i < configs.main_bgo.size(); i++)
                    {
                        if(configs.main_bgo[i].id == bgo.id)
                        {
                            j = 0;
                            isIndex=true;
                            break;
                        }
                    }
                    if(!isIndex) j=0;
                }


                if(configs.main_bgo[j].animated)
                    tmpI = bgo.image.copy(0,0,
                                bgo.image.width(),
                                (int)round(bgo.image.height() / configs.main_bgo[j].frames));
                else
                    tmpI = bgo.image;

                item = new QListWidgetItem( QString("bgo-%1").arg(bgo.id) );
                item->setIcon( QIcon( tmpI ) );
                item->setData(3, QString::number(bgo.id) );
                item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

                ui->BGOItemsList->addItem( item );
            }

        }

    }
    else
    //set BGO item box from global array
    foreach(obj_bgo bgoItem, configs.main_bgo)
    {
        //Add Group
        found = false;
        if(tmpList.size()!=0)
            foreach(QString grp, tmpGrpList)
            {
                if(bgoItem.group.isEmpty())
                {found=true;break;}//Skip empty values
                if(bgoItem.group==grp)
                {found=true; break;}
            }
        if(!found) tmpGrpList.push_back(bgoItem.group);

        //Add category
        found = false;
        if(tmpList.size()!=0)
            foreach(QString cat, tmpList)
            {
                if(bgoItem.category==cat)
                {found = true; break;}
                if((bgoItem.group!=grp_bgo)&&(grp_bgo!=allLabel))
                {found = true; break;}
            }
        if(!found) tmpList.push_back(bgoItem.category);

        if(
                ((bgoItem.group==grp_bgo)||(grp_bgo==allLabel)||(grp_bgo==""))&&
                ((bgoItem.category==cat_bgos)||(cat_bgos==allLabel))
           )
        {
            if(bgoItem.animated)
                tmpI = bgoItem.image.copy(0,0,
                            bgoItem.image.width(),
                            (int)round(bgoItem.image.height() / bgoItem.frames) );
            else
                tmpI = bgoItem.image;

            item = new QListWidgetItem( bgoItem.name );
            item->setIcon( QIcon( tmpI ) );
            item->setData(3, QString::number(bgoItem.id) );
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

    //set custom NPC items from loaded level
    if((ui->NPCCatList->currentText()==customLabel)&&(setCat)&&(setGrp))
    {
        if(activeChildWindow()==1)
        {
            //long j=0;
            //bool isIndex=false;
            leveledit * edit = activeLvlEditWin();
            foreach(UserNPCs npc, edit->scene->uNPCs)
            {

                tmpI = edit->scene->getNPCimg(npc.id);

                item = new QListWidgetItem( QString("npc-%1").arg(npc.id) );
                item->setIcon( QIcon( tmpI ) );
                item->setData(3, QString::number(npc.id) );
                item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

                ui->NPCItemsList->addItem( item );
            }

        }

    }
    else
    //set NPC item box from global config
    foreach(obj_npc npcItem, configs.main_npc)
    {
        //Add Group
        found = false;
        if(tmpList.size()!=0)
            foreach(QString grp, tmpGrpList)
            {
                if(npcItem.group.isEmpty())
                {found=true;break;}//Skip empty values
                if(npcItem.group==grp)
                {found=true; break;}
            }
        if(!found) tmpGrpList.push_back(npcItem.group);

        //Add category
        found = false;
        if(tmpList.size()!=0)
            foreach(QString cat, tmpList)
            {
                if(npcItem.category==cat)
                {found = true; break;}
                if((npcItem.group!=grp_npc)&&(grp_npc!=allLabel))
                {found = true; break;}
            }
        if(!found) tmpList.push_back(npcItem.category);

        if(
                ((npcItem.group==grp_npc)||(grp_npc==allLabel)||(grp_npc==""))&&
                ((npcItem.category==cat_npcs)||(cat_npcs==allLabel))
          )
        {
            tmpI = npcItem.image.copy(0,0, npcItem.image.width(), npcItem.gfx_h );

            item = new QListWidgetItem( npcItem.name );
            item->setIcon( QIcon( tmpI ) );
            item->setData(3, QString::number(npcItem.id) );
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

}

// ///////////////////////////////////
void MainWindow::on_BlockGroupList_currentIndexChanged(const QString &arg1)
{
    if(lock_grp) return;
    grp_blocks=arg1;
    setItemBoxes(true);
}

void MainWindow::on_BGOGroupList_currentIndexChanged(const QString &arg1)
{
    if(lock_grp) return;
    grp_bgo=arg1;
    setItemBoxes(true);
}

void MainWindow::on_NPCGroupList_currentIndexChanged(const QString &arg1)
{
    if(lock_grp) return;
    grp_npc=arg1;
    setItemBoxes(true);
}

// ///////////////////////////////////
void MainWindow::on_BlockCatList_currentIndexChanged(const QString &arg1)
{
    if(lock_cat) return;
    cat_blocks=arg1;
    setItemBoxes(true, true);
}


void MainWindow::on_BGOCatList_currentIndexChanged(const QString &arg1)
{
    if(lock_cat) return;
    cat_bgos=arg1;
    setItemBoxes(true, true);
}


void MainWindow::on_NPCCatList_currentIndexChanged(const QString &arg1)
{
    if(lock_cat) return;
    cat_npcs=arg1;
    setItemBoxes(true, true);
}




// ///////////////////////////////////

void MainWindow::on_BGOUniform_clicked(bool checked)
{
    ui->BGOItemsList->setUniformItemSizes(checked);
    setItemBoxes(true, true);
}

void MainWindow::on_BlockUniform_clicked(bool checked)
{
    ui->BlockItemsList->setUniformItemSizes(checked);
    setItemBoxes(true, true);
}


void MainWindow::on_NPCUniform_clicked(bool checked)
{
    ui->NPCItemsList->setUniformItemSizes(checked);
    setItemBoxes(true, true);
}


// ///////////////////////////////////

void MainWindow::on_BlockItemsList_itemClicked(QListWidgetItem *item)
{
    resetEditmodeButtons();
   //placeBlock

    if ((activeChildWindow()==1) && (ui->BlockItemsList->hasFocus()))
    {
       activeLvlEditWin()->scene->clearSelection();
       activeLvlEditWin()->changeCursor(2);
       activeLvlEditWin()->scene->EditingMode = 2;
       activeLvlEditWin()->scene->disableMoveItems=false;
       activeLvlEditWin()->scene->DrawMode=true;
       activeLvlEditWin()->scene->EraserEnabled = false;
       LvlPlacingItems::fillingMode = false;

       activeLvlEditWin()->scene->setItemPlacer(0, item->data(3).toInt() );

       LvlItemProps(0,LvlPlacingItems::blockSet,
                                 FileFormats::dummyLvlBgo(),
                                 FileFormats::dummyLvlNpc(), true);

       activeLvlEditWin()->setFocus();
    }
}

void MainWindow::on_BGOItemsList_itemClicked(QListWidgetItem *item)
{
    resetEditmodeButtons();
   //placeBGO

    if ((activeChildWindow()==1) && (ui->BGOItemsList->hasFocus()))
    {
       activeLvlEditWin()->scene->clearSelection();
       activeLvlEditWin()->changeCursor(2);
       activeLvlEditWin()->scene->EditingMode = 2;
       activeLvlEditWin()->scene->disableMoveItems=false;
       activeLvlEditWin()->scene->DrawMode=true;
       activeLvlEditWin()->scene->EraserEnabled = false;

       LvlPlacingItems::fillingMode = false;

       activeLvlEditWin()->scene->setItemPlacer(1, item->data(3).toInt() );

       LvlItemProps(1,FileFormats::dummyLvlBlock(),
                                 LvlPlacingItems::bgoSet,
                                 FileFormats::dummyLvlNpc(), true);

       activeLvlEditWin()->setFocus();
    }

}

void MainWindow::on_NPCItemsList_itemClicked(QListWidgetItem *item)
{
    resetEditmodeButtons();

    //placeNPC
    if ((activeChildWindow()==1) && (ui->NPCItemsList->hasFocus()))
    {
       activeLvlEditWin()->scene->clearSelection();
       activeLvlEditWin()->changeCursor(2);
       activeLvlEditWin()->scene->EditingMode = 2;
       activeLvlEditWin()->scene->disableMoveItems=false;
       activeLvlEditWin()->scene->DrawMode=true;
       activeLvlEditWin()->scene->EraserEnabled = false;
       activeLvlEditWin()->scene->setItemPlacer(2, item->data(3).toInt() );

       LvlItemProps(2,FileFormats::dummyLvlBlock(),
                                 FileFormats::dummyLvlBgo(),
                                 LvlPlacingItems::npcSet, true);

       activeLvlEditWin()->setFocus();
    }

}
void MainWindow::updateFilters()
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

void MainWindow::clearFilter()
{
    ui->BlockFilterField->setText("");
    ui->BGOFilterField->setText("");
    ui->NPCFilterField->setText("");
    updateFilters();
}

void MainWindow::on_BlockFilterField_textChanged(const QString &arg1)
{
    updateFilters();

    if(arg1.isEmpty()) return; //Dummy
}

void MainWindow::on_BlockFilterType_currentIndexChanged(int /*index*/)
{
    updateFilters();
}

void MainWindow::on_BGOFilterField_textChanged(const QString &arg1)
{
    updateFilters();

    if(arg1.isEmpty()) return; //Dummy
}

void MainWindow::on_BGOFilterType_currentIndexChanged(int /*index*/)
{
    updateFilters();
}

void MainWindow::on_NPCFilterField_textChanged(const QString &arg1)
{
    updateFilters();
    if(arg1.isEmpty()) return; //Dummy
}

void MainWindow::on_NPCFilterType_currentIndexChanged(int /*index*/)
{
    updateFilters();
}
