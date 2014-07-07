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
#include "../../item_select_dialog/itemselectdialog.h"
#include "../../level_scene/item_block.h"
#include "../../level_scene/item_bgo.h"
#include "../../level_scene/item_npc.h"

static bool lockReset = false;

void MainWindow::on_FindStartBlock_clicked()
{
    if(!(currentSearches & SEARCH_BLOCK)){ //start search
        if(activeChildWindow()==1){
            currentSearches |= SEARCH_BLOCK;
            ui->FindStartBlock->setText(tr("Next Block"));
            ui->Find_Button_ResetBlock->setText(tr("Stop Search"));
            leveledit* edit = activeLvlEditWin();
            if(doSearchBlock(edit)){
                currentSearches ^= SEARCH_BLOCK;
                ui->Find_Button_ResetBlock->setText(tr("Reset Search Fields"));
                ui->FindStartBlock->setText(tr("Search Block"));
                QMessageBox::information(this, tr("Search Complete"), tr("Block search completed!"));
            }
        }
    }else{ //middle in a search
        if(activeChildWindow()==1){
            leveledit* edit = activeLvlEditWin();
            if(doSearchBlock(edit)){
                currentSearches ^= SEARCH_BLOCK;
                ui->Find_Button_ResetBlock->setText(tr("Reset Search Fields"));
                ui->FindStartBlock->setText(tr("Search Block"));
                QMessageBox::information(this, tr("Search Complete"), tr("Block search completed!"));
            }
        }
    }
}

void MainWindow::on_FindStartBGO_clicked()
{
    if(!(currentSearches & SEARCH_BGO)){ //start search
        if(activeChildWindow()==1){
            currentSearches |= SEARCH_BGO;
            ui->FindStartBGO->setText(tr("Next BGO"));
            ui->Find_Button_ResetBGO->setText(tr("Stop Search"));
            leveledit* edit = activeLvlEditWin();
            if(doSearchBGO(edit)){
                currentSearches ^= SEARCH_BGO;
                ui->Find_Button_ResetBGO->setText(tr("Reset Search Fields"));
                ui->FindStartBGO->setText(tr("Search BGO"));
                QMessageBox::information(this, tr("Search Complete"), tr("BGO search completed!"));
            }
        }
    }else{ //middle in a search
        if(activeChildWindow()==1){
            leveledit* edit = activeLvlEditWin();
            if(doSearchBGO(edit)){
                currentSearches ^= SEARCH_BGO;
                ui->Find_Button_ResetBGO->setText(tr("Reset Search Fields"));
                ui->FindStartBGO->setText(tr("Search BGO"));
                QMessageBox::information(this, tr("Search Complete"), tr("BGO search completed!"));
            }
        }
    }
}

void MainWindow::on_FindStartNPC_clicked()
{
    if(!(currentSearches & SEARCH_NPC)){ //start search
        if(activeChildWindow()==1){
            currentSearches |= SEARCH_NPC;
            ui->FindStartNPC->setText(tr("Next NPC"));
            ui->Find_Button_ResetNPC->setText(tr("Stop Search"));
            leveledit* edit = activeLvlEditWin();
            if(doSearchNPC(edit)){
                currentSearches ^= SEARCH_NPC;
                ui->Find_Button_ResetNPC->setText(tr("Reset Search Fields"));
                ui->FindStartNPC->setText(tr("Search NPC"));
                QMessageBox::information(this, tr("Search Complete"), tr("NPC search completed!"));
            }
        }
    }else{ //middle in a search
        if(activeChildWindow()==1){
            leveledit* edit = activeLvlEditWin();
            if(doSearchNPC(edit)){
                currentSearches ^= SEARCH_NPC;
                ui->Find_Button_ResetNPC->setText(tr("Reset Search Fields"));
                ui->FindStartNPC->setText(tr("Search NPC"));
                QMessageBox::information(this, tr("Search Complete"), tr("NPC search completed!"));
            }
        }
    }
}

void MainWindow::on_Find_Button_TypeBlock_clicked()
{
    ItemSelectDialog* selBlock = new ItemSelectDialog(&configs, ItemSelectDialog::TAB_BLOCK,0,curSearchBlock.id);
    if(selBlock->exec()==QDialog::Accepted){
        int selected = selBlock->blockID;
        curSearchBlock.id = selected;
        ui->Find_Button_TypeBlock->setText(((selected>0)?QString("Block-%1").arg(selected):tr("[empty]")));
    }
    delete selBlock;
}

void MainWindow::on_Find_Button_ContainsNPCBlock_clicked()
{
    ItemSelectDialog * npcList = new ItemSelectDialog(&configs, ItemSelectDialog::TAB_NPC,
                                                   ItemSelectDialog::NPCEXTRA_WITHCOINS | (curSearchBlock.npc_id < 1000 && curSearchBlock.npc_id != 0 ? ItemSelectDialog::NPCEXTRA_ISCOINSELECTED : 0),0,0,
                                                   (curSearchBlock.npc_id < 1000 && curSearchBlock.npc_id != 0 ? curSearchBlock.npc_id : curSearchBlock.npc_id-1000));
    if(npcList->exec()==QDialog::Accepted){
        int selected = 0;
        if(npcList->npcID!=0){
            if(npcList->isCoin)
                selected = npcList->npcID;
            else
                selected = npcList->npcID+1000;
        }
        curSearchBlock.npc_id = selected;
        ui->Find_Button_ContainsNPCBlock->setText(((selected>0) ? ((selected>1000) ? QString("NPC-%1").arg(selected-1000) : tr("%1 coins").arg(selected)) : tr("[empty]")));
    }
    delete npcList;
}

void MainWindow::on_Find_Button_TypeBGO_clicked()
{
    ItemSelectDialog* selBgo = new ItemSelectDialog(&configs, ItemSelectDialog::TAB_BGO,0,0,curSearchBGO.id);
    if(selBgo->exec()==QDialog::Accepted){
        int selected = selBgo->bgoID;
        curSearchBGO.id = selected;
        ui->Find_Button_TypeBGO->setText(((selected>0)?QString("BGO-%1").arg(selected):tr("[empty]")));
    }
    delete selBgo;
}

void MainWindow::on_Find_Button_TypeNPC_clicked()
{
    ItemSelectDialog* selNpc = new ItemSelectDialog(&configs, ItemSelectDialog::TAB_NPC,0,0,0,curSearchNPC.id);
    if(selNpc->exec()==QDialog::Accepted){
        int selected = selNpc->npcID;
        curSearchNPC.id = selected;
        ui->Find_Button_TypeNPC->setText(((selected>0)?QString("NPC-%1").arg(selected):tr("[empty]")));
    }

    delete selNpc;
}

void MainWindow::on_Find_Button_ResetBlock_clicked()
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
        curSearchBlock.index = 0;
    }
}

void MainWindow::on_Find_Button_ResetBGO_clicked()
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
        curSearchBGO.index = 0;
    }
}

void MainWindow::on_Find_Button_ResetNPC_clicked()
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
    }else{
        currentSearches ^= SEARCH_NPC;
        ui->Find_Button_ResetNPC->setText(tr("Reset Search Fields"));
        ui->FindStartNPC->setText(tr("Search NPC"));
        curSearchNPC.index = 0;
    }
}

void MainWindow::resetAllSearchFields()
{
    resetAllSearches();

    on_Find_Button_ResetBlock_clicked();
    on_Find_Button_ResetBGO_clicked();
    on_Find_Button_ResetNPC_clicked();
}

void MainWindow::resetAllSearches()
{
    resetBlockSearch();
    resetBGOSearch();
    resetNPCSearch();
}

void MainWindow::resetBlockSearch()
{
    if(lockReset) return;
    lockReset = true;
    if(currentSearches & SEARCH_BLOCK) on_Find_Button_ResetBlock_clicked();
    lockReset = false;
}

void MainWindow::resetBGOSearch()
{
    if(lockReset) return;
    lockReset = true;
    if(currentSearches & SEARCH_BGO) on_Find_Button_ResetBGO_clicked();
    lockReset = false;
}

void MainWindow::resetNPCSearch()
{
    if(lockReset) return;
    lockReset = true;
    if(currentSearches & SEARCH_NPC) on_Find_Button_ResetNPC_clicked();
    lockReset = false;
}

//return true when finish searching
bool MainWindow::doSearchBlock(leveledit *edit)
{
    QList<QGraphicsItem*> gr = edit->scene->items();
    if(curSearchBlock.index+1 < (unsigned int)gr.size()){
        for(int i = curSearchBlock.index+1; i < gr.size(); ++i){
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
    curSearchBlock.index = 0;
    return true;
}

bool MainWindow::doSearchBGO(leveledit *edit)
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
    curSearchBGO.index = 0;
    return true;
}

bool MainWindow::doSearchNPC(leveledit *edit)
{
    QList<QGraphicsItem*> gr = edit->scene->items();
    if(curSearchNPC.index+1 < (unsigned int)gr.size()){
        for(int i = curSearchNPC.index+1; i < gr.size(); ++i){
            if(gr[i]->data(0).toString()=="NPC"){
                bool toBeFound = true;
                LevelNPC tmp = ((ItemNPC*)gr[i])->npcData;
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
    curSearchNPC.index = 0;
    return true;
}

void MainWindow::toggleNewWindow(QMdiSubWindow */*window*/)
{
    resetAllSearches();
}
