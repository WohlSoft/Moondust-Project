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

#include "itemselectdialog.h"
#include <ui_itemselectdialog.h>
#include "../../common_features/mainwinconnect.h"
#include "../../common_features/util.h"
#include "../../common_features/graphics_funcs.h"

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

    QFont font;
    font.setItalic(true);
    QListWidgetItem * empBlock = new QListWidgetItem();
    QListWidgetItem * empBGO = new QListWidgetItem();
    QListWidgetItem * empNPC = new QListWidgetItem();
    QListWidgetItem * empScenery = new QListWidgetItem();
    QListWidgetItem * empLevel = new QListWidgetItem();
    QListWidgetItem * empMusic = new QListWidgetItem();
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


    if(npcExtraData & NPCEXTRA_WITHCOINS){
        npcFromList = new QRadioButton(tr("NPC from List"));
        npcCoins = new QRadioButton(tr("Coins"));
        npcCoinsSel = new QSpinBox();
        npcCoinsSel->setMinimum(1);
        npcCoinsSel->setEnabled(false);
        extraNPCWid << npcFromList << npcCoins << npcCoinsSel;
        addExtraDataControl(npcFromList);
        addExtraDataControl(npcCoins);
        addExtraDataControl(npcCoinsSel);
        connect(npcFromList, SIGNAL(toggled(bool)), this, SLOT(npcTypeChange(bool)));
        if(isCoinSel){
            npcCoinsSel->setValue(curSelIDNPC);
            npcCoins->setChecked(true);
            //npcFromList->setChecked(false);
            npcTypeChange(true);
        }else{
            npcFromList->setChecked(true);
        }
    }else{
        npcFromList = 0;
        npcCoins = 0;
        npcCoinsSel = 0;
    }

    if(blockTab){
        foreach(obj_block blockItem, conf->main_block)
        {
            //Add category
            QPixmap tmpI = GraphicsHelps::squareImage(
                                blockItem.image.copy(0,
                                             blockItem.frame_h*blockItem.display_frame,
                                                blockItem.image.width(),
                                                blockItem.frame_h),
                                                      QSize(16,16));

            QListWidgetItem* item = new QListWidgetItem( blockItem.name );
            item->setIcon( QIcon( tmpI ) );
            item->setData(3, QString::number(blockItem.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );
            ui->Sel_List_Block->addItem(item);
        }
    }

    if(bgoTab){
        foreach(obj_bgo bgoItem, conf->main_bgo)
        {
            //Add category
            QPixmap tmpI = bgoItem.image;

            QListWidgetItem* item = new QListWidgetItem( bgoItem.name );
            item->setIcon( QIcon( tmpI ) );
            item->setData(3, QString::number(bgoItem.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );
            ui->Sel_List_BGO->addItem(item);
        }
    }

    if(npcTab){
        foreach(obj_npc npcItem, conf->main_npc)
        {
            //Add category
            QPixmap tmpI = GraphicsHelps::squareImage(
                        npcItem.image.copy(0,npcItem.gfx_h*npcItem.display_frame, npcItem.image.width(), npcItem.gfx_h ),
                        QSize(16,16) );

            QListWidgetItem* item = new QListWidgetItem( npcItem.name );
            item->setIcon( QIcon( tmpI ) );
            item->setData(3, QString::number(npcItem.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );
            ui->Sel_List_NPC->addItem(item);
        }
    }


    if(sceneryTab){
        foreach (obj_w_scenery sceneryItem, conf->main_wscene) {
            //Add category
            QListWidgetItem* item = new QListWidgetItem(QString("tile-%1").arg(sceneryItem.id));
            item->setIcon( QIcon( sceneryItem.image ) );
            item->setData(3, QString::number(sceneryItem.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );
            ui->Sel_List_Scenery->addItem(item);
        }
    }

    if(levelTab){
        foreach (obj_w_level levelItem, conf->main_wlevels) {
            //Add category
            QListWidgetItem* item = new QListWidgetItem(QString("tile-%1").arg(levelItem.id));
            item->setIcon( QIcon( levelItem.image ) );
            item->setData(3, QString::number(levelItem.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );
            ui->Sel_List_Level->addItem(item);
        }
    }

    if(musicTab){
        foreach (obj_music musicItem, conf->main_music_wld) {
            //Add category
            QListWidgetItem* item = new QListWidgetItem(QString("tile-%1").arg(musicItem.id));
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

void ItemSelectDialog::addExtraDataControl(QWidget *control)
{
    ui->verticalLayout->insertWidget(ui->verticalLayout->count()-1, control);
}

void ItemSelectDialog::updateBoxes(bool setGrp, bool setCat)
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
    ui->Sel_List_BGO->clear();
    ui->Sel_List_Block->clear();
    ui->Sel_List_NPC->clear();

    WriteToLog(QtDebugMsg, "LevelTools -> Declare new");
    QListWidgetItem * item;
    QPixmap tmpI;

    QStringList tmpList, tmpGrpList;
    bool found = false;

    tmpList.clear();
    tmpGrpList.clear();

    //set custom Block items from loaded level
    if((ui->Sel_Combo_CategoryBlock->currentText()==customLabel)&&(setCat)&&(setGrp))
    {
        if(MainWinConnect::pMainWin->activeChildWindow()==1)
        {
            long j=0;
            bool isIndex=false;
            LevelEdit * edit = MainWinConnect::pMainWin->activeLvlEditWin();
            foreach(UserBlocks block, edit->scene->uBlocks)
            {

                //Check for index
                if(block.id < (unsigned long)conf->index_blocks.size())
                {
                    if(block.id == conf->main_block[conf->index_blocks[block.id].i].id)
                    {
                        j = conf->index_blocks[block.id].i;
                        isIndex=true;
                    }
                }
                //In index is false, fetch array
                if(!isIndex)
                {
                    for(int i=0; i < conf->main_block.size(); i++)
                    {
                        if(conf->main_block[i].id == block.id)
                        {
                            j = 0;
                            isIndex=true;
                            break;
                        }
                    }
                    if(!isIndex) j=0;
                }


                if(conf->main_block[j].animated)
                    tmpI = block.image.copy(0,
                                (int)round(block.image.height() / conf->main_block[j].frames)*conf->main_block[j].display_frame,
                                block.image.width(),
                                (int)round(block.image.height() / conf->main_block[j].frames));
                else
                    tmpI = block.image;

                item = new QListWidgetItem( QString("block-%1").arg(block.id) );
                item->setIcon( QIcon( tmpI ) );
                item->setData(3, QString::number(block.id) );
                item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

                ui->Sel_List_Block->addItem( item );
            }

        }

    }
    else
    //set Block item box from global conf
    foreach(obj_block blockItem, conf->main_block)
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
                tmpI = blockItem.image.copy(0,
                            (int)round(blockItem.image.height() / blockItem.frames) * blockItem.display_frame,
                            blockItem.image.width(),
                            (int)round(blockItem.image.height() / blockItem.frames));
            else
                tmpI = blockItem.image;

            item = new QListWidgetItem( blockItem.name );
            item->setIcon( QIcon( tmpI ) );
            item->setData(3, QString::number(blockItem.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

            ui->Sel_List_Block->addItem( item );
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
        if(MainWinConnect::pMainWin->activeChildWindow()==1)
        {
            long j=0;
            bool isIndex=false;
            LevelEdit * edit = MainWinConnect::pMainWin->activeLvlEditWin();
            foreach(UserBGOs bgo, edit->scene->uBGOs)
            {

                //Check for index
                if(bgo.id < (unsigned long)conf->index_bgo.size())
                {
                    if(bgo.id == conf->main_bgo[conf->index_bgo[bgo.id].i].id)
                    {
                        j = conf->index_bgo[bgo.id].i;
                        isIndex=true;
                    }
                }
                //In index is false, fetch array
                if(!isIndex)
                {
                    for(int i=0; i < conf->main_bgo.size(); i++)
                    {
                        if(conf->main_bgo[i].id == bgo.id)
                        {
                            j = 0;
                            isIndex=true;
                            break;
                        }
                    }
                    if(!isIndex) j=0;
                }


                if(conf->main_bgo[j].animated)
                    tmpI = bgo.image.copy(0,
                                (int)round(bgo.image.height() / conf->main_bgo[j].frames)*conf->main_bgo[j].display_frame,
                                bgo.image.width(),
                                (int)round(bgo.image.height() / conf->main_bgo[j].frames));
                else
                    tmpI = bgo.image;

                item = new QListWidgetItem( QString("bgo-%1").arg(bgo.id) );
                item->setIcon( QIcon( tmpI ) );
                item->setData(3, QString::number(bgo.id) );
                item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

                ui->Sel_List_BGO->addItem( item );
            }

        }

    }
    else
    //set BGO item box from global array
    foreach(obj_bgo bgoItem, conf->main_bgo)
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
                tmpI = bgoItem.image.copy(0,
                            (int)round(bgoItem.image.height() / bgoItem.frames)*bgoItem.display_frame,
                            bgoItem.image.width(),
                            (int)round(bgoItem.image.height() / bgoItem.frames) );
            else
                tmpI = bgoItem.image;

            item = new QListWidgetItem( bgoItem.name );
            item->setIcon( QIcon( tmpI ) );
            item->setData(3, QString::number(bgoItem.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

            ui->Sel_List_BGO->addItem( item );
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
        if(MainWinConnect::pMainWin->activeChildWindow()==1)
        {
            //long j=0;
            //bool isIndex=false;
            LevelEdit * edit = MainWinConnect::pMainWin->activeLvlEditWin();
            foreach(UserNPCs npc, edit->scene->uNPCs)
            {

                tmpI = GraphicsHelps::squareImage(edit->scene->getNPCimg(npc.id),QSize(16,16));

                item = new QListWidgetItem( QString("npc-%1").arg(npc.id) );
                item->setIcon( QIcon( tmpI ) );
                item->setData(3, QString::number(npc.id) );
                item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

                ui->Sel_List_NPC->addItem( item );
            }

        }

    }
    else
    //set NPC item box from global config
    foreach(obj_npc npcItem, conf->main_npc)
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
            tmpI = GraphicsHelps::squareImage(npcItem.image.copy(0,0, npcItem.image.width(), npcItem.gfx_h ),
                                              QSize(16,16));

            item = new QListWidgetItem( npcItem.name );
            item->setIcon( QIcon( tmpI ) );
            item->setData(3, QString::number(npcItem.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );

            ui->Sel_List_NPC->addItem( item );
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

    WriteToLog(QtDebugMsg, "WorldTools -> Clear current");

    util::memclear(ui->Sel_List_Tile);
    util::memclear(ui->Sel_List_Scenery);
    util::memclear(ui->Sel_List_Path);
    util::memclear(ui->Sel_List_Level);
    util::memclear(ui->Sel_List_Music);


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

    int roff = (removalFlags & TAB_TILE ? 0 : 1);

    //get Table size
    foreach(obj_w_tile tileItem, conf->main_wtiles )
    {
        if(tableRows<tileItem.row+1+roff) tableRows=tileItem.row+1+roff;
        if(tableCols<tileItem.col+1) tableCols=tileItem.col+1;
    }

    WriteToLog(QtDebugMsg, "WorldTools -> set size");
    ui->Sel_List_Tile->setRowCount(tableRows);
    ui->Sel_List_Tile->setColumnCount(tableCols);
    ui->Sel_List_Tile->setStyleSheet("QTableWidget::item { padding: 0px; margin: 0px; }");

    WriteToLog(QtDebugMsg, "WorldTools -> Table of tiles");
    foreach(obj_w_tile tileItem, conf->main_wtiles )
    {
            if(tileItem.animated)
                tmpI = tileItem.image.copy(0,
                            (int)round(tileItem.image.height() / tileItem.frames) * tileItem.display_frame,
                            tileItem.image.width(),
                            (int)round(tileItem.image.height() / tileItem.frames));
            else
                tmpI = tileItem.image;

        QTableWidgetItem * Titem = ui->Sel_List_Tile->item(tileItem.row, tileItem.col);

        if ( (!Titem) || ( (Titem!=NULL)&&(Titem->text().isEmpty())) )
        {
            Titem = new QTableWidgetItem();
            Titem->setIcon( QIcon( tmpI.scaled( QSize(32,32), Qt::KeepAspectRatio ) ) );
            Titem->setText( NULL );
            Titem->setSizeHint(QSize(32,32));
            Titem->setData(3, QString::number(tileItem.id) );
            Titem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);

            ui->Sel_List_Tile->setRowHeight(tileItem.row, 34);
            ui->Sel_List_Tile->setColumnWidth(tileItem.col, 34);

            ui->Sel_List_Tile->setItem(tileItem.row+roff,tileItem.col, Titem);
        }
    }

    if(roff==1){
        QTableWidgetItem * Titem = ui->Sel_List_Tile->item(0, 0);
        if ( (!Titem) || ( (Titem!=NULL)&&(Titem->text().isEmpty())) )
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
            Titem->setText( NULL );
            Titem->setSizeHint(QSize(32,32));
            Titem->setData(3, 0 );
            Titem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);

            ui->Sel_List_Tile->setRowHeight(0, 34);
            ui->Sel_List_Tile->setColumnWidth(0, 34);

            ui->Sel_List_Tile->setItem(0,0, Titem);
        }

    }

    WriteToLog(QtDebugMsg, "WorldTools -> List of sceneries");
    foreach(obj_w_scenery sceneItem, conf->main_wscene)
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

            ui->Sel_List_Scenery->addItem( item );
    }

    tableRows=0;
    tableCols=0;

    roff = (removalFlags & TAB_TILE ? 0 : 1);

    WriteToLog(QtDebugMsg, "WorldTools -> Table of paths size");
    //get Table size
    foreach(obj_w_path pathItem, conf->main_wpaths )
    {
        if(tableRows<pathItem.row+1+roff) tableRows=pathItem.row+1+roff;
        if(tableCols<pathItem.col+1) tableCols=pathItem.col+1;
    }

    WriteToLog(QtDebugMsg, "WorldTools -> Table of paths size define");
    ui->Sel_List_Path->setRowCount(tableRows);
    ui->Sel_List_Path->setColumnCount(tableCols);
    ui->Sel_List_Path->setStyleSheet("QTableWidget::item { padding: 0px; margin: 0px; }");

    WriteToLog(QtDebugMsg, "WorldTools -> Table of paths");
    foreach(obj_w_path pathItem, conf->main_wpaths )
    {
            if(pathItem.animated)
                tmpI = pathItem.image.copy(0,
                            (int)round(pathItem.image.height() / pathItem.frames)*pathItem.display_frame,
                            pathItem.image.width(),
                            (int)round(pathItem.image.height() / pathItem.frames));
            else
                tmpI = pathItem.image;

        QTableWidgetItem * Titem = ui->Sel_List_Path->item(pathItem.row, pathItem.col);

        if ( (!Titem) || ( (Titem!=NULL)&&(Titem->text().isEmpty())) )
        {
            Titem = new QTableWidgetItem();
            Titem->setIcon( QIcon( tmpI.scaled( QSize(32,32), Qt::KeepAspectRatio ) ) );
            Titem->setText( NULL );
            Titem->setSizeHint(QSize(32,32));
            Titem->setData(3, QString::number(pathItem.id) );
            Titem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);

            ui->Sel_List_Path->setRowHeight(pathItem.row+roff, 34);
            ui->Sel_List_Path->setColumnWidth(pathItem.col, 34);

            ui->Sel_List_Path->setItem(pathItem.row+roff,pathItem.col, Titem);
        }
    }

    if(roff==1){
        QTableWidgetItem * Titem = ui->Sel_List_Path->item(0, 0);
        if ( (!Titem) || ( (Titem!=NULL)&&(Titem->text().isEmpty())) )
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
            Titem->setText( NULL );
            Titem->setSizeHint(QSize(32,32));
            Titem->setData(3, 0 );
            Titem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);

            ui->Sel_List_Path->setRowHeight(0, 34);
            ui->Sel_List_Path->setColumnWidth(0, 34);

            ui->Sel_List_Path->setItem(0,0, Titem);
        }

    }

    WriteToLog(QtDebugMsg, "WorldTools -> List of levels");
    foreach(obj_w_level levelItem, conf->main_wlevels)
    {
            if((conf->marker_wlvl.path==levelItem.id)||
               (conf->marker_wlvl.bigpath==levelItem.id))
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

            ui->Sel_List_Level->addItem( item );
    }

    WriteToLog(QtDebugMsg, "WorldTools -> List of musics");
    foreach(obj_music musicItem, conf->main_music_wld)
    {
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

    WriteToLog(QtDebugMsg, "WorldTools -> done");
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


void ItemSelectDialog::on_Sel_List_NPC_itemDoubleClicked(QListWidgetItem *)
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
    if((ui->Sel_TabCon_ItemType->indexOf(t)!=-1 ? ui->Sel_TabCon_ItemType->indexOf(t)==ui->Sel_TabCon_ItemType->currentIndex() : false)){
        ui->Sel_Label_ExtraData->setVisible(!l.isEmpty());
        if(l.isEmpty()){
            ui->horizontalLayout->removeItem(ui->verticalLayout);
        }else{
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
