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
#include "ui_itemselectdialog.h"
#include "../common_features/mainwinconnect.h"
#include "../common_features/util.h"

static QString allLabel = "[all]";
static QString customLabel = "[custom]";
static QString grp_blocks = "";
static QString grp_bgo = "";
static QString grp_npc = "";
static bool lock_grp=false;
static bool lock_cat=false;

ItemSelectDialog::ItemSelectDialog(dataconfigs *configs, int tabs, int npcExtraData,
                                   int curSelIDBlock, int curSelIDBGO, int curSelIDNPC, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ItemSelectDialog)
{
    conf = configs;
    removalFlags = 0;
    ui->setupUi(this);

    QFont font;
    font.setItalic(true);
    QListWidgetItem * empBlock = new QListWidgetItem();
    QListWidgetItem * empBGO = new QListWidgetItem();
    QListWidgetItem * empNPC = new QListWidgetItem();
    empBlock->setFont(font);
    empBGO->setFont(font);
    empNPC->setFont(font);
    QString emTxt = tr("[Empty]");
    empBlock->setText(emTxt);
    empBGO->setText(emTxt);
    empNPC->setText(emTxt);
    empBlock->setData(3, QVariant(0));
    empBGO->setData(3, QVariant(0));
    empNPC->setData(3, QVariant(0));

    ui->Sel_List_Block->insertItem(0,empBlock);
    ui->Sel_List_BGO->insertItem(0,empBGO);
    ui->Sel_List_NPC->insertItem(0,empNPC);


    bool blockTab = tabs & TAB_BLOCK;
    bool bgoTab = tabs & TAB_BGO;
    bool npcTab = tabs & TAB_NPC;
    bool isCoinSel = npcExtraData & NPCEXTRA_ISCOINSELECTED;

    if(!blockTab)
        ui->Sel_TabCon_ItemType->removeTab(ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_Block));

    if(!bgoTab)
        ui->Sel_TabCon_ItemType->removeTab(ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_BGO));

    if(!npcTab)
        ui->Sel_TabCon_ItemType->removeTab(ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_NPC));

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
            QPixmap tmpI = blockItem.image;

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
            QPixmap tmpI = npcItem.image.copy(0,0, npcItem.image.width(), npcItem.gfx_h );

            QListWidgetItem* item = new QListWidgetItem( npcItem.name );
            item->setIcon( QIcon( tmpI ) );
            item->setData(3, QString::number(npcItem.id) );
            item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled );
            ui->Sel_List_NPC->addItem(item);
        }
    }

    updateBoxes();
    on_Sel_TabCon_ItemType_currentChanged(ui->Sel_TabCon_ItemType->currentIndex());

    selectListItem(ui->Sel_List_Block, curSelIDBlock);
    selectListItem(ui->Sel_List_BGO, curSelIDBGO);
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
            leveledit * edit = MainWinConnect::pMainWin->activeLvlEditWin();
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
                    tmpI = block.image.copy(0,0,
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
    //set Block item box from global configs
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
                tmpI = blockItem.image.copy(0,0,
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
            leveledit * edit = MainWinConnect::pMainWin->activeLvlEditWin();
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
                    tmpI = bgo.image.copy(0,0,
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
                tmpI = bgoItem.image.copy(0,0,
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
            leveledit * edit = MainWinConnect::pMainWin->activeLvlEditWin();
            foreach(UserNPCs npc, edit->scene->uNPCs)
            {

                tmpI = edit->scene->getNPCimg(npc.id);

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
            tmpI = npcItem.image.copy(0,0, npcItem.image.width(), npcItem.gfx_h );

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

    QFont font;
    font.setItalic(true);
    QListWidgetItem * empBlock = new QListWidgetItem();
    QListWidgetItem * empBGO = new QListWidgetItem();
    QListWidgetItem * empNPC = new QListWidgetItem();
    empBlock->setFont(font);
    empBGO->setFont(font);
    empNPC->setFont(font);
    QString emTxt = tr("[Empty]");
    empBlock->setText(emTxt);
    empBGO->setText(emTxt);
    empNPC->setText(emTxt);
    empBlock->setData(3, QVariant(0));
    empBGO->setData(3, QVariant(0));
    empNPC->setData(3, QVariant(0));

    ui->Sel_List_Block->insertItem(0,empBlock);
    ui->Sel_List_BGO->insertItem(0,empBGO);
    ui->Sel_List_NPC->insertItem(0,empNPC);

    if(removalFlags & TAB_BLOCK && ui->Sel_List_Block->item(0)->data(3).toInt() == 0)
        delete ui->Sel_List_Block->item(0);

    if(removalFlags & TAB_BGO && ui->Sel_List_BGO->item(0)->data(3).toInt() == 0)
        delete ui->Sel_List_BGO->item(0);

    if(removalFlags & TAB_NPC && ui->Sel_List_NPC->item(0)->data(3).toInt() == 0)
        delete ui->Sel_List_NPC->item(0);

    lock_grp=false;
    lock_cat=false;

    updateFilters();
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
    if(!extraBlockWid.isEmpty()){
        for(QList<QWidget*>::iterator it = extraBlockWid.begin(); it != extraBlockWid.end(); ++it){
            ((QWidget*)(*it))->setVisible((ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_Block)!=-1 ? ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_Block)==index : false));
        }
    }
    if(!extraBGOWid.isEmpty()){
        for(QList<QWidget*>::iterator it = extraBGOWid.begin(); it != extraBGOWid.end(); ++it){
            ((QWidget*)(*it))->setVisible((ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_BGO)!=-1 ? ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_BGO)==index : false));
        }
    }
    if(!extraNPCWid.isEmpty()){
        for(QList<QWidget*>::iterator it = extraNPCWid.begin(); it != extraNPCWid.end(); ++it){
            ((QWidget*)(*it))->setVisible((ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_NPC)!=-1 ? ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_NPC)==index : false));
        }
    }

    //update label
    if((ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_Block)!=-1 ? ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_Block)==index : false)){
        ui->Sel_Label_ExtraData->setVisible(!extraBlockWid.isEmpty());
        if(extraBlockWid.isEmpty()){
            ui->horizontalLayout->removeItem(ui->verticalLayout);
        }else{
            ui->horizontalLayout->addLayout(ui->verticalLayout);
        }
        return;
    }

    if((ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_BGO)!=-1 ? ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_BGO)==index : false)){
        ui->Sel_Label_ExtraData->setVisible(!extraBGOWid.isEmpty());
        if(extraBGOWid.isEmpty()){
            ui->horizontalLayout->removeItem(ui->verticalLayout);
        }else{
            ui->horizontalLayout->addLayout(ui->verticalLayout);
        }
        return;
    }

    if((ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_NPC)!=-1 ? ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_NPC)==index : false)){
        ui->Sel_Label_ExtraData->setVisible(!extraNPCWid.isEmpty());
        if(extraNPCWid.isEmpty()){
            ui->horizontalLayout->removeItem(ui->verticalLayout);
        }else{
            ui->horizontalLayout->addLayout(ui->verticalLayout);
        }
        return;
    }
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

void ItemSelectDialog::on_Sel_DiaButtonBox_accepted()
{
    blockID = 0;
    bgoID = 0;
    npcID = 0;
    isCoin = (npcCoins ? npcCoins->isChecked() : false);

    if(ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_Block)!=-1){
        if(!ui->Sel_List_Block->selectedItems().isEmpty()){
            blockID = ui->Sel_List_Block->selectedItems()[0]->data(3).toInt();
        }
    }

    if(ui->Sel_TabCon_ItemType->indexOf(ui->Sel_Tab_BGO)!=-1){
        if(!ui->Sel_List_BGO->selectedItems().isEmpty()){
            bgoID = ui->Sel_List_BGO->selectedItems()[0]->data(3).toInt();
        }
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
