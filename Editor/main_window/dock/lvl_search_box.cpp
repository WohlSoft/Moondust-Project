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

#include <editing/_dialogs/itemselectdialog.h>
#include <editing/_scenes/level/items/item_block.h>
#include <editing/_scenes/level/items/item_bgo.h>
#include <editing/_scenes/level/items/item_npc.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>

#include "lvl_search_box.h"
#include "ui_lvl_search_box.h"

#define NPC_Name(npcid) ((npcid!=0) ?\
    ((npcid<0)?QString("%1 Coins").arg(npcid*(-1)) : QString("NPC-%1").arg(npcid))\
    :QString("[empty]"))

LvlSearchBox::LvlSearchBox(QWidget *parent) :
    QDockWidget(parent),
    MWDock_Base(parent),
    ui(new Ui::LvlSearchBox)
{
    setVisible(false);
    setAttribute(Qt::WA_ShowWithoutActivating);
    ui->setupUi(this);

    QRect mwg = mw()->geometry();
    int GOffset=240;
    mw()->addDockWidget(Qt::RightDockWidgetArea, this);
    connect(mw(), SIGNAL(languageSwitched()), this, SLOT(re_translate()));
    #ifdef Q_OS_WIN
    setFloating(true);
    #endif
    setGeometry(
                mwg.x()+mwg.width()-width()-GOffset,
                mwg.y()+120,
                width(),
                height()
                );

    mw()->docks_level.
          addState(this, &GlobalSettings::LevelSearchBoxVis);

    lockReset = false;

    curSearchBlock.id = 0;
    curSearchBlock.index = -1; //When incrementing then starting with 0
    curSearchBlock.npc_id = 0;

    curSearchBGO.id = 0;
    curSearchBGO.index = -1; //When incrementing then starting with 0

    curSearchNPC.id = 0;
    curSearchNPC.index = -1; //When incrementing then starting with 0

    currentSearches=0;

    //enable & disable
    connect(ui->Find_Check_TypeBlock, SIGNAL(toggled(bool)), ui->Find_Button_TypeBlock, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_TypeBGO, SIGNAL(toggled(bool)), ui->Find_Button_TypeBGO, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_TypeNPC, SIGNAL(toggled(bool)), ui->Find_Button_TypeNPC, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_PriorityBGO, SIGNAL(toggled(bool)), ui->Find_Spin_PriorityBGO, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_LayerBlock, SIGNAL(toggled(bool)), ui->Find_Combo_LayerBlock, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_LayerBGO, SIGNAL(toggled(bool)), ui->Find_Combo_LayerBGO, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_LayerNPC, SIGNAL(toggled(bool)), ui->Find_Combo_LayerNPC, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_InvisibleBlock, SIGNAL(toggled(bool)), ui->Find_Check_InvisibleActiveBlock, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_SlipperyBlock, SIGNAL(toggled(bool)), ui->Find_Check_SlipperyActiveBlock, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_ContainsNPCBlock, SIGNAL(toggled(bool)), ui->Find_Button_ContainsNPCBlock, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_EventDestoryedBlock, SIGNAL(toggled(bool)), ui->Find_Combo_EventDestoryedBlock, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_EventHitedBlock, SIGNAL(toggled(bool)), ui->Find_Combo_EventHitedBlock, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_EventLayerEmptyBlock, SIGNAL(toggled(bool)), ui->Find_Combo_EventLayerEmptyBlock, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_DirNPC, SIGNAL(toggled(bool)), ui->Find_Radio_DirLeftNPC, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_DirNPC, SIGNAL(toggled(bool)), ui->Find_Radio_DirRandomNPC, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_DirNPC, SIGNAL(toggled(bool)), ui->Find_Radio_DirRightNPC, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_FriendlyNPC, SIGNAL(toggled(bool)), ui->Find_Check_FriendlyActiveNPC, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_NotMoveNPC, SIGNAL(toggled(bool)), ui->Find_Check_NotMoveActiveNPC, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_BossNPC, SIGNAL(toggled(bool)), ui->Find_Check_BossActiveNPC, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_MsgNPC, SIGNAL(toggled(bool)), ui->Find_Edit_MsgNPC, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_MsgNPC, SIGNAL(toggled(bool)), ui->Find_Check_MsgSensitiveNPC, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_Layer_AttachedNPC, SIGNAL(toggled(bool)), ui->Find_Combo_Layer_AttachedNPC, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_Event_ActivateNPC, SIGNAL(toggled(bool)), ui->Find_Combo_Event_ActivateNPC, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_Event_DeathNPC, SIGNAL(toggled(bool)), ui->Find_Combo_Event_DeathNPC, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_Event_TalkNPC, SIGNAL(toggled(bool)), ui->Find_Combo_Event_TalkNPC, SLOT(setEnabled(bool)));
    connect(ui->Find_Check_Event_Empty_LayerNPC, SIGNAL(toggled(bool)), ui->Find_Combo_Event_Empty_LayerNPC, SLOT(setEnabled(bool)));

    //reset if modify
    connect(ui->Find_Button_TypeBlock, SIGNAL(clicked()), this, SLOT(resetBlockSearch()));
    connect(ui->Find_Button_TypeBGO, SIGNAL(clicked()), this, SLOT(resetBGOSearch()));
    connect(ui->Find_Button_TypeNPC, SIGNAL(clicked()), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Spin_PriorityBGO, SIGNAL(valueChanged(int)), this, SLOT(resetBGOSearch()));
    connect(ui->Find_Combo_LayerBlock, SIGNAL(activated(int)), this, SLOT(resetBlockSearch()));
    connect(ui->Find_Combo_LayerBGO, SIGNAL(activated(int)), this, SLOT(resetBGOSearch()));
    connect(ui->Find_Combo_LayerNPC, SIGNAL(activated(int)), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Check_InvisibleActiveBlock, SIGNAL(clicked()), this, SLOT(resetBlockSearch()));
    connect(ui->Find_Check_SlipperyActiveBlock, SIGNAL(clicked()), this, SLOT(resetBlockSearch()));
    connect(ui->Find_Button_ContainsNPCBlock, SIGNAL(clicked()), this, SLOT(resetBlockSearch()));
    connect(ui->Find_Combo_EventDestoryedBlock, SIGNAL(activated(int)), this, SLOT(resetBlockSearch()));
    connect(ui->Find_Combo_EventHitedBlock, SIGNAL(activated(int)), this, SLOT(resetBlockSearch()));
    connect(ui->Find_Combo_EventLayerEmptyBlock, SIGNAL(activated(int)), this, SLOT(resetBlockSearch()));
    connect(ui->Find_Radio_DirLeftNPC, SIGNAL(clicked()), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Radio_DirRandomNPC, SIGNAL(clicked()), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Radio_DirRightNPC, SIGNAL(clicked()), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Check_FriendlyActiveNPC, SIGNAL(clicked()), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Check_NotMoveActiveNPC, SIGNAL(clicked()), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Check_BossActiveNPC, SIGNAL(clicked()), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Edit_MsgNPC, SIGNAL(textEdited(QString)), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Check_MsgSensitiveNPC, SIGNAL(clicked()), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Combo_Layer_AttachedNPC, SIGNAL(activated(int)), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Combo_Event_ActivateNPC, SIGNAL(activated(int)), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Combo_Event_DeathNPC, SIGNAL(activated(int)), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Combo_Event_TalkNPC, SIGNAL(activated(int)), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Combo_Event_Empty_LayerNPC, SIGNAL(activated(int)), this, SLOT(resetNPCSearch()));

    //also checkboxes
    connect(ui->Find_Check_TypeBlock, SIGNAL(clicked()), this, SLOT(resetBlockSearch()));
    connect(ui->Find_Check_LayerBlock, SIGNAL(clicked()), this, SLOT(resetBlockSearch()));
    connect(ui->Find_Check_InvisibleBlock, SIGNAL(clicked()), this, SLOT(resetBlockSearch()));
    connect(ui->Find_Check_SlipperyBlock, SIGNAL(clicked()), this, SLOT(resetBlockSearch()));
    connect(ui->Find_Check_ContainsNPCBlock, SIGNAL(clicked()), this, SLOT(resetBlockSearch()));
    connect(ui->Find_Check_EventDestoryedBlock, SIGNAL(clicked()), this, SLOT(resetBlockSearch()));
    connect(ui->Find_Check_EventLayerEmptyBlock, SIGNAL(clicked()), this, SLOT(resetBlockSearch()));

    connect(ui->Find_Check_TypeBGO, SIGNAL(clicked()), this, SLOT(resetBGOSearch()));
    connect(ui->Find_Check_LayerBGO, SIGNAL(clicked()), this, SLOT(resetBGOSearch()));
    connect(ui->Find_Check_PriorityBGO, SIGNAL(clicked()), this, SLOT(resetBGOSearch()));

    connect(ui->Find_Check_TypeNPC, SIGNAL(clicked()), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Check_LayerNPC, SIGNAL(clicked()), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Check_DirNPC, SIGNAL(clicked()), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Check_FriendlyNPC, SIGNAL(clicked()), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Check_NotMoveNPC, SIGNAL(clicked()), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Check_BossNPC, SIGNAL(clicked()), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Check_MsgNPC, SIGNAL(clicked()), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Check_Layer_AttachedNPC, SIGNAL(clicked()), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Check_Event_ActivateNPC, SIGNAL(clicked()), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Check_Event_DeathNPC, SIGNAL(clicked()), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Check_Event_TalkNPC, SIGNAL(clicked()), this, SLOT(resetNPCSearch()));
    connect(ui->Find_Check_Event_Empty_LayerNPC, SIGNAL(clicked()), this, SLOT(resetNPCSearch()));

    connect(mw()->ui->centralWidget, SIGNAL(subWindowActivated(QMdiSubWindow*)), this, SLOT(toggleNewWindowLVL(QMdiSubWindow*)));
}

LvlSearchBox::~LvlSearchBox()
{
    delete ui;
}

/****************************Comboboxes***************************/
QComboBox *LvlSearchBox::cbox_layer_block()
{
    return ui->Find_Combo_LayerBlock;
}

QComboBox *LvlSearchBox::cbox_layer_bgo()
{
    return ui->Find_Combo_LayerBGO;
}

QComboBox *LvlSearchBox::cbox_layer_npc()
{
    return ui->Find_Combo_LayerNPC;
}

QComboBox *LvlSearchBox::cbox_layer_attached_npc()
{
    return ui->Find_Combo_Layer_AttachedNPC;
}

QComboBox *LvlSearchBox::cbox_event_block_dest()
{
    return ui->Find_Combo_EventDestoryedBlock;
}

QComboBox *LvlSearchBox::cbox_event_block_hit()
{
    return ui->Find_Combo_EventHitedBlock;
}

QComboBox *LvlSearchBox::cbox_event_block_le()
{
    return ui->Find_Combo_EventLayerEmptyBlock;
}

QComboBox *LvlSearchBox::cbox_event_npc_activate()
{
    return ui->Find_Combo_Event_ActivateNPC;
}

QComboBox *LvlSearchBox::cbox_event_npc_death()
{
    return ui->Find_Combo_Event_DeathNPC;
}

QComboBox *LvlSearchBox::cbox_event_npc_talk()
{
    return ui->Find_Combo_Event_TalkNPC;
}

QComboBox *LvlSearchBox::cbox_event_npc_empty_layer()
{
    return ui->Find_Combo_Event_Empty_LayerNPC;
}

//QComboBox *LvlSearchBox::cbox_event_npc_act()
//{
//    return ui->Find;
//}
//QComboBox *LvlSearchBox::cbox_event_npc_die()
//{
//    return ui->PROPS_NpcEventDeath;
//}
//QComboBox *LvlSearchBox::cbox_event_npc_talk()
//{
//    return ui->PROPS_NpcEventTalk;
//}
//QComboBox *LvlSearchBox::cbox_event_npc_nm()
//{
//    return ui->PROPS_NpcEventEmptyLayer;
//}
/****************************Comboboxes***************************/

void LvlSearchBox::re_translate()
{
    ui->retranslateUi(this);
}

void LvlSearchBox::on_LvlSearchBox_visibilityChanged(bool visible)
{
    mw()->ui->actionLVLSearchBox->setChecked(visible);
}

void MainWindow::on_actionLVLSearchBox_triggered(bool checked)
{
    dock_LvlSearchBox->setVisible(checked);
    if(checked) dock_LvlSearchBox->raise();
}


void LvlSearchBox::on_FindStartBlock_clicked()
{
    if(!(currentSearches & SEARCH_BLOCK)){ //start search
        if(mw()->activeChildWindow()==1)
        {
            LevelEdit* edit = mw()->activeLvlEditWin();
            if(!edit) return;
            currentSearches |= SEARCH_BLOCK;
            ui->FindStartBlock->setText(tr("Next Block"));
            ui->Find_Button_ResetBlock->setText(tr("Stop Search"));
            if(doSearchBlock(edit))
            {
                currentSearches ^= SEARCH_BLOCK;
                ui->Find_Button_ResetBlock->setText(tr("Reset Search Fields"));
                ui->FindStartBlock->setText(tr("Search Block"));
                QMessageBox::information(mw(), tr("Search Complete"), tr("Block search completed!"));
            }
        }
    }else{ //middle in a search
        if(mw()->activeChildWindow()==1){
            LevelEdit* edit = mw()->activeLvlEditWin();
            if(!edit) return;
            if(doSearchBlock(edit)){
                currentSearches ^= SEARCH_BLOCK;
                ui->Find_Button_ResetBlock->setText(tr("Reset Search Fields"));
                ui->FindStartBlock->setText(tr("Search Block"));
                QMessageBox::information(mw(), tr("Search Complete"), tr("Block search completed!"));
            }
        }
    }
}

void LvlSearchBox::on_FindStartBGO_clicked()
{
    if(!(currentSearches & SEARCH_BGO)){ //start search
        if(mw()->activeChildWindow()==1){
            LevelEdit* edit = mw()->activeLvlEditWin();
            if(!edit) return;
            currentSearches |= SEARCH_BGO;
            ui->FindStartBGO->setText(tr("Next BGO"));
            ui->Find_Button_ResetBGO->setText(tr("Stop Search"));
            if(doSearchBGO(edit)){
                currentSearches ^= SEARCH_BGO;
                ui->Find_Button_ResetBGO->setText(tr("Reset Search Fields"));
                ui->FindStartBGO->setText(tr("Search BGO"));
                QMessageBox::information(mw(), tr("Search Complete"), tr("BGO search completed!"));
            }
        }
    }else{ //middle in a search
        if(mw()->activeChildWindow()==1){
            LevelEdit* edit = mw()->activeLvlEditWin();
            if(!edit) return;
            if(doSearchBGO(edit)){
                currentSearches ^= SEARCH_BGO;
                ui->Find_Button_ResetBGO->setText(tr("Reset Search Fields"));
                ui->FindStartBGO->setText(tr("Search BGO"));
                QMessageBox::information(mw(), tr("Search Complete"), tr("BGO search completed!"));
            }
        }
    }
}

void LvlSearchBox::on_FindStartNPC_clicked()
{
    if(!(currentSearches & SEARCH_NPC)){ //start search
        if(mw()->activeChildWindow()==1){
            LevelEdit* edit = mw()->activeLvlEditWin();
            if(!edit) return;
            currentSearches |= SEARCH_NPC;
            ui->FindStartNPC->setText(tr("Next NPC"));
            ui->Find_Button_ResetNPC->setText(tr("Stop Search"));
            if(doSearchNPC(edit)){
                currentSearches ^= SEARCH_NPC;
                ui->Find_Button_ResetNPC->setText(tr("Reset Search Fields"));
                ui->FindStartNPC->setText(tr("Search NPC"));
                QMessageBox::information(this, tr("Search Complete"), tr("NPC search completed!"));
            }
        }
    }else{ //middle in a search
        if(mw()->activeChildWindow()==1){
            LevelEdit* edit = mw()->activeLvlEditWin();
            if(!edit) return;
            if(doSearchNPC(edit)){
                currentSearches ^= SEARCH_NPC;
                ui->Find_Button_ResetNPC->setText(tr("Reset Search Fields"));
                ui->FindStartNPC->setText(tr("Search NPC"));
                QMessageBox::information(this, tr("Search Complete"), tr("NPC search completed!"));
            }
        }
    }
}

void LvlSearchBox::on_Find_Button_TypeBlock_clicked()
{
    ItemSelectDialog* selBlock = new ItemSelectDialog(&(mw()->configs), ItemSelectDialog::TAB_BLOCK,0,curSearchBlock.id);
    if(selBlock->exec()==QDialog::Accepted)
    {
        int selected = selBlock->blockID;
        curSearchBlock.id = selected;
        ui->Find_Button_TypeBlock->setText(((selected>0)?QString("Block-%1").arg(selected):tr("[empty]")));
    }
    delete selBlock;
}

void LvlSearchBox::on_Find_Button_ContainsNPCBlock_clicked()
{
    ItemSelectDialog * npcList = new ItemSelectDialog(&(mw()->configs), ItemSelectDialog::TAB_NPC,
                                                   ItemSelectDialog::NPCEXTRA_WITHCOINS | (curSearchBlock.npc_id < 0 && curSearchBlock.npc_id != 0 ? ItemSelectDialog::NPCEXTRA_ISCOINSELECTED : 0),0,0,
                                                   (curSearchBlock.npc_id < 0 && curSearchBlock.npc_id != 0 ? curSearchBlock.npc_id*(-1) : curSearchBlock.npc_id));
    if(npcList->exec()==QDialog::Accepted){
        int selected = 0;
        if(npcList->npcID!=0){
            if(npcList->isCoin)
                selected = npcList->npcID*(-1);
            else
                selected = npcList->npcID;
        }
        curSearchBlock.npc_id = selected;
        ui->Find_Button_ContainsNPCBlock->setText(NPC_Name(selected));
    }
    delete npcList;
}

void LvlSearchBox::on_Find_Button_TypeBGO_clicked()
{
    ItemSelectDialog* selBgo = new ItemSelectDialog(&(mw()->configs), ItemSelectDialog::TAB_BGO,0,0,curSearchBGO.id);
    if(selBgo->exec()==QDialog::Accepted){
        int selected = selBgo->bgoID;
        curSearchBGO.id = selected;
        ui->Find_Button_TypeBGO->setText(((selected>0)?QString("BGO-%1").arg(selected):tr("[empty]")));
    }
    delete selBgo;
}

void LvlSearchBox::on_Find_Button_TypeNPC_clicked()
{
    ItemSelectDialog* selNpc = new ItemSelectDialog(&(mw()->configs), ItemSelectDialog::TAB_NPC,0,0,0,curSearchNPC.id);
    if(selNpc->exec()==QDialog::Accepted){
        int selected = selNpc->npcID;
        curSearchNPC.id = selected;
        ui->Find_Button_TypeNPC->setText(NPC_Name(selected));
    }
    delete selNpc;
}

void LvlSearchBox::on_Find_Button_ResetBlock_clicked()
{
    if(!(currentSearches & SEARCH_BLOCK)){
        ui->Find_Check_TypeBlock->setChecked(true);
        ui->Find_Button_TypeBlock->setText(tr("[empty]"));
        curSearchBlock.id = 0;
        curSearchBlock.npc_id = 0;
        ui->Find_Button_ContainsNPCBlock->setText(tr("[empty]"));
        ui->Find_Combo_LayerBlock->setCurrentText("Default");
        ui->Find_Check_InvisibleActiveBlock->setChecked(false);
        ui->Find_Check_SlipperyActiveBlock->setChecked(false);
    }else{
        currentSearches ^= SEARCH_BLOCK;
        ui->Find_Button_ResetBlock->setText(tr("Reset Search Fields"));
        ui->FindStartBlock->setText(tr("Search Block"));
        curSearchBlock.index = -1; //When incrementing then starting with 0
    }
}

void LvlSearchBox::on_Find_Button_ResetBGO_clicked()
{
    if(!(currentSearches & SEARCH_BGO)){
        ui->Find_Check_TypeBGO->setChecked(true);
        ui->Find_Button_TypeBGO->setText(tr("[empty]"));
        ui->Find_Combo_LayerBGO->setCurrentText("Default");
        ui->Find_Spin_PriorityBGO->setValue(-1);
        curSearchBGO.id = 0;
    }else{
        currentSearches ^= SEARCH_BGO;
        ui->Find_Button_ResetBGO->setText(tr("Reset Search Fields"));
        ui->FindStartBGO->setText(tr("Search BGO"));
        curSearchBGO.index = -1; //When incrementing then starting with 0
    }
}

void LvlSearchBox::on_Find_Button_ResetNPC_clicked()
{
    if(!(currentSearches & SEARCH_NPC)){
        ui->Find_Check_TypeNPC->setChecked(true);
        ui->Find_Button_TypeNPC->setText(tr("[empty]"));
        ui->Find_Combo_LayerNPC->setCurrentText("Default");
        curSearchNPC.id = 0;
        ui->Find_Radio_DirLeftNPC->setChecked(true);
        ui->Find_Check_FriendlyActiveNPC->setChecked(false);
        ui->Find_Check_NotMoveActiveNPC->setChecked(false);
        ui->Find_Check_BossActiveNPC->setChecked(false);
        ui->Find_Edit_MsgNPC->setText("");
        ui->Find_Check_MsgSensitiveNPC->setChecked(false);
        ui->Find_Check_Layer_AttachedNPC->setChecked(false);
    }else{
        currentSearches ^= SEARCH_NPC;
        ui->Find_Button_ResetNPC->setText(tr("Reset Search Fields"));
        ui->FindStartNPC->setText(tr("Search NPC"));
        curSearchNPC.index = -1; //When incrementing then starting with 0
    }
}

void LvlSearchBox::resetAllSearchFields()
{
    resetAllSearches();

    on_Find_Button_ResetBlock_clicked();
    on_Find_Button_ResetBGO_clicked();
    on_Find_Button_ResetNPC_clicked();
}

void LvlSearchBox::resetAllSearches()
{
    resetBlockSearch();
    resetBGOSearch();
    resetNPCSearch();
}

void LvlSearchBox::resetBlockSearch()
{
    if(lockReset) return;
    lockReset = true;
    if(currentSearches & SEARCH_BLOCK) on_Find_Button_ResetBlock_clicked();
    lockReset = false;
}

void LvlSearchBox::resetBGOSearch()
{
    if(lockReset) return;
    lockReset = true;
    if(currentSearches & SEARCH_BGO) on_Find_Button_ResetBGO_clicked();
    lockReset = false;
}

void LvlSearchBox::resetNPCSearch()
{
    if(lockReset) return;
    lockReset = true;
    if(currentSearches & SEARCH_NPC) on_Find_Button_ResetNPC_clicked();
    lockReset = false;
}

//return true when finish searching
bool LvlSearchBox::doSearchBlock(LevelEdit *edit)
{
    QList<QGraphicsItem*> gr = edit->scene->items();
    if(curSearchBlock.index+1 < (unsigned int)gr.size()){
        for(int i = curSearchBlock.index+1; i < gr.size(); ++i){
            qDebug() << "Next Index: " << i;
            if(gr[i]->data(0).toString()=="Block"){
                bool toBeFound = true;
                if(ui->Find_Check_TypeBlock->isChecked()&&curSearchBlock.id!=0&&toBeFound){
                    toBeFound = ((ItemBlock*)gr[i])->blockData.id == (unsigned int)curSearchBlock.id;
                }
                if(ui->Find_Check_LayerBlock->isChecked()&&toBeFound){
                    toBeFound = ((ItemBlock*)gr[i])->blockData.layer == ui->Find_Combo_LayerBlock->currentText();
                }
                if(ui->Find_Check_InvisibleBlock->isChecked()&&toBeFound){
                    toBeFound = ((ItemBlock*)gr[i])->blockData.invisible == ui->Find_Check_InvisibleActiveBlock->isChecked();
                }
                if(ui->Find_Check_SlipperyBlock->isChecked()&&toBeFound){
                    toBeFound = ((ItemBlock*)gr[i])->blockData.slippery == ui->Find_Check_SlipperyActiveBlock->isChecked();
                }
                if(ui->Find_Check_ContainsNPCBlock->isChecked()&&toBeFound){
                    toBeFound = ((ItemBlock*)gr[i])->blockData.npc_id == curSearchBlock.npc_id;
                }
                if(ui->Find_Check_EventDestoryedBlock->isChecked()&&toBeFound){
                    toBeFound = ((ItemBlock*)gr[i])->blockData.event_destroy == ui->Find_Combo_EventDestoryedBlock->currentText();
                }
                if(ui->Find_Check_EventHitedBlock->isChecked()&&toBeFound){
                    toBeFound = ((ItemBlock*)gr[i])->blockData.event_hit == ui->Find_Combo_EventHitedBlock->currentText();
                }
                if(ui->Find_Check_EventLayerEmptyBlock->isChecked()&&toBeFound){
                    toBeFound = ((ItemBlock*)gr[i])->blockData.event_no_more == ui->Find_Combo_EventLayerEmptyBlock->currentText();
                }
                if(toBeFound){
                    foreach (QGraphicsItem* i, edit->scene->selectedItems())
                    {
                        i->setSelected(false);
                    }
                    gr[i]->setSelected(true);
                    edit->goTo(((ItemBlock*)gr[i])->blockData.x, ((ItemBlock*)gr[i])->blockData.y, true, QPoint(-300, -300));
                    curSearchBlock.index = i;
                    return false;
                }
            }
        }
    }
    //end search
    curSearchBlock.index = -1; //When incrementing then starting with 0
    return true;
}

bool LvlSearchBox::doSearchBGO(LevelEdit *edit)
{
    QList<QGraphicsItem*> gr = edit->scene->items();
    if(curSearchBGO.index+1 < (unsigned int)gr.size()){
        for(int i = curSearchBGO.index+1; i < gr.size(); ++i){
            if(gr[i]->data(0).toString()=="BGO"){
                bool toBeFound = true;
                if(ui->Find_Check_TypeBGO->isChecked()&&curSearchBGO.id!=0&&toBeFound){
                    toBeFound = ((ItemBGO*)gr[i])->bgoData.id == (unsigned int)curSearchBGO.id;
                }
                if(ui->Find_Check_LayerBGO->isChecked()&&toBeFound){
                    toBeFound = ((ItemBGO*)gr[i])->bgoData.layer == ui->Find_Combo_LayerBGO->currentText();
                }
                if(ui->Find_Check_PriorityBGO->isChecked()&&toBeFound){
                    toBeFound = ((ItemBGO*)gr[i])->bgoData.smbx64_sp == ui->Find_Spin_PriorityBGO->value();
                }
                if(toBeFound){
                    foreach (QGraphicsItem* i, edit->scene->selectedItems())
                    {
                        i->setSelected(false);
                    }
                    gr[i]->setSelected(true);
                    edit->goTo(((ItemBGO*)gr[i])->bgoData.x, ((ItemBGO*)gr[i])->bgoData.y, true, QPoint(-300, -300));
                    curSearchBGO.index = i;
                    return false;
                }
            }
        }
    }
    //end search
    curSearchBGO.index = -1; //When incrementing then starting with 0
    return true;
}

bool LvlSearchBox::doSearchNPC(LevelEdit *edit)
{
    QList<QGraphicsItem*> gr = edit->scene->items();
    if(curSearchNPC.index+1 < (unsigned int)gr.size()){
        for(int i = curSearchNPC.index+1; i < gr.size(); ++i){
            if(gr[i]->data(0).toString()=="NPC"){
                bool toBeFound = true;
                if(ui->Find_Check_TypeNPC->isChecked()&&curSearchNPC.id!=0&&toBeFound){
                    toBeFound = ((ItemNPC*)gr[i])->npcData.id == (unsigned int)curSearchNPC.id;
                }
                if(ui->Find_Check_LayerNPC->isChecked()&&toBeFound){
                    toBeFound = ((ItemNPC*)gr[i])->npcData.layer == ui->Find_Combo_LayerNPC->currentText();
                }
                if(ui->Find_Check_DirNPC->isChecked()&&toBeFound){
                    if(ui->Find_Radio_DirLeftNPC->isChecked()){
                        toBeFound = ((ItemNPC*)gr[i])->npcData.direct == -1;
                    }else if(ui->Find_Radio_DirRandomNPC->isChecked()){
                        toBeFound = ((ItemNPC*)gr[i])->npcData.direct == 0;
                    }else if(ui->Find_Radio_DirRightNPC->isChecked()){
                        toBeFound = ((ItemNPC*)gr[i])->npcData.direct == 1;
                    }
                }
                if(ui->Find_Check_FriendlyNPC->isChecked()&&toBeFound){
                    toBeFound = ((ItemNPC*)gr[i])->npcData.friendly == ui->Find_Check_FriendlyActiveNPC->isChecked();
                }
                if(ui->Find_Check_NotMoveNPC->isChecked()&&toBeFound){
                    toBeFound = ((ItemNPC*)gr[i])->npcData.nomove == ui->Find_Check_NotMoveActiveNPC->isChecked();
                }
                if(ui->Find_Check_BossNPC->isChecked()&&toBeFound){
                    toBeFound = ((ItemNPC*)gr[i])->npcData.legacyboss == ui->Find_Check_BossActiveNPC->isChecked();
                }
                if(ui->Find_Check_MsgNPC->isChecked()&&toBeFound){
                    toBeFound = ((ItemNPC*)gr[i])->npcData.msg.contains(ui->Find_Edit_MsgNPC->text(),
                                                                        (ui->Find_Check_MsgSensitiveNPC->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive));
                }
                if(ui->Find_Check_Layer_AttachedNPC->isChecked()&&toBeFound){
                    toBeFound = ((ItemNPC*)gr[i])->npcData.attach_layer == ui->Find_Combo_Layer_AttachedNPC->currentText()
                                || (((ItemNPC*)gr[i])->npcData.attach_layer == "" && ui->Find_Combo_Layer_AttachedNPC->currentText() == "[None]");
                }
                if(ui->Find_Check_Event_ActivateNPC->isChecked()&&toBeFound){
                    toBeFound = ((ItemNPC*)gr[i])->npcData.event_activate == ui->Find_Combo_Event_ActivateNPC->currentText()
                                || (((ItemNPC*)gr[i])->npcData.event_activate == "" && ui->Find_Combo_Event_ActivateNPC->currentText() == "[None]");
                }
                if(ui->Find_Check_Event_DeathNPC->isChecked()&&toBeFound){
                    toBeFound = ((ItemNPC*)gr[i])->npcData.event_die == ui->Find_Combo_Event_DeathNPC->currentText()
                                || (((ItemNPC*)gr[i])->npcData.event_die == "" && ui->Find_Combo_Event_DeathNPC->currentText() == "[None]");
                }
                if(ui->Find_Check_Event_TalkNPC->isChecked()&&toBeFound){
                    toBeFound = ((ItemNPC*)gr[i])->npcData.event_talk == ui->Find_Combo_Event_TalkNPC->currentText()
                                || (((ItemNPC*)gr[i])->npcData.event_talk == "" && ui->Find_Combo_Event_TalkNPC->currentText() == "[None]");
                }
                if(ui->Find_Check_Event_Empty_LayerNPC->isChecked()&&toBeFound){
                    toBeFound = ((ItemNPC*)gr[i])->npcData.event_nomore == ui->Find_Combo_Event_Empty_LayerNPC->currentText()
                                || (((ItemNPC*)gr[i])->npcData.event_nomore == "" && ui->Find_Combo_Event_Empty_LayerNPC->currentText() == "[None]");
                }
                if(toBeFound){
                    foreach (QGraphicsItem* i, edit->scene->selectedItems())
                    {
                        i->setSelected(false);
                    }
                    gr[i]->setSelected(true);
                    edit->goTo(((ItemNPC*)gr[i])->npcData.x, ((ItemNPC*)gr[i])->npcData.y, true, QPoint(-300, -300));
                    curSearchNPC.index = i;
                    return false;
                }
            }
        }
    }
    //end search
    curSearchNPC.index = -1; //When incrementing then starting with 0
    return true;
}

void LvlSearchBox::toggleNewWindowLVL(QMdiSubWindow */*window*/)
{
    resetAllSearches();
}

