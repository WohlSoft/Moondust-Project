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
    ItemSelectDialog* selBlock = new ItemSelectDialog(&configs, ItemSelectDialog::TAB_BLOCK,0,curSearchBlockID);
    if(selBlock->exec()==QDialog::Accepted){
        int selected = selBlock->blockID;
        curSearchBlockID = selected;
        ui->Find_Button_TypeBlock->setText(((selected>0)?QString("Block-%1").arg(selected):tr("[empty]")));
    }
    delete selBlock;
}

void MainWindow::on_Find_Button_TypeBGO_clicked()
{
    ItemSelectDialog* selBgo = new ItemSelectDialog(&configs, ItemSelectDialog::TAB_BGO,0,0,curSearchBGOID);
    if(selBgo->exec()==QDialog::Accepted){
        int selected = selBgo->bgoID;
        curSearchBGOID = selected;
        ui->Find_Button_TypeBGO->setText(((selected>0)?QString("BGO-%1").arg(selected):tr("[empty]")));
    }
    delete selBgo;
}

void MainWindow::on_Find_Button_TypeNPC_clicked()
{
    ItemSelectDialog* selNpc = new ItemSelectDialog(&configs, ItemSelectDialog::TAB_NPC,0,0,0,curSearchNPCID);
    if(selNpc->exec()==QDialog::Accepted){
        int selected = selNpc->npcID;
        curSearchNPCID = selected;
        ui->Find_Button_TypeNPC->setText(((selected>0)?QString("NPC-%1").arg(selected):tr("[empty]")));
    }

    delete selNpc;
}

void MainWindow::on_Find_Button_ResetBlock_clicked()
{
    if(!(currentSearches & SEARCH_BLOCK)){
        ui->Find_Check_TypeBlock->setChecked(true);
        ui->Find_Button_TypeBlock->setText(tr("[empty]"));
        curSearchBlockID = 0;
    }else{
        currentSearches ^= SEARCH_BLOCK;
        ui->Find_Button_ResetBlock->setText(tr("Reset Search Fields"));
        ui->FindStartBlock->setText(tr("Search Block"));
        curSearchBlockIndex = 0;
    }
}

void MainWindow::on_Find_Button_ResetBGO_clicked()
{
    if(!(currentSearches & SEARCH_BGO)){
        ui->Find_Check_TypeBGO->setChecked(true);
        ui->Find_Button_TypeBGO->setText(tr("[empty]"));
        curSearchBGOID = 0;
    }else{
        currentSearches ^= SEARCH_BGO;
        ui->Find_Button_ResetBGO->setText(tr("Reset Search Fields"));
        ui->FindStartBGO->setText(tr("Search BGO"));
        curSearchBGOIndex = 0;
    }
}

void MainWindow::on_Find_Button_ResetNPC_clicked()
{
    if(!(currentSearches & SEARCH_NPC)){
        ui->Find_Check_TypeNPC->setChecked(true);
        ui->Find_Button_TypeNPC->setText(tr("[empty]"));
        curSearchNPCID = 0;
    }else{
        currentSearches ^= SEARCH_NPC;
        ui->Find_Button_ResetNPC->setText(tr("Reset Search Fields"));
        ui->FindStartNPC->setText(tr("Search NPC"));
        curSearchNPCIndex = 0;
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
    if(currentSearches & SEARCH_BLOCK) on_Find_Button_ResetBlock_clicked();
    if(currentSearches & SEARCH_BGO) on_Find_Button_ResetBGO_clicked();
    if(currentSearches & SEARCH_NPC) on_Find_Button_ResetNPC_clicked();
}

//return true when finish searching
bool MainWindow::doSearchBlock(leveledit *edit)
{
    QList<QGraphicsItem*> gr = edit->scene->items();
    if(curSearchBlockIndex+1 < gr.size()){
        for(int i = curSearchBlockIndex+1; i < gr.size(); ++i){
            if(gr[i]->data(0).toString()=="Block"){
                bool toBeFound = true;
                if(ui->Find_Check_TypeBlock->isChecked()&&toBeFound){
                    toBeFound = ((ItemBlock*)gr[i])->blockData.id == (unsigned int)curSearchBlockID;
                }
                if(toBeFound){
                    foreach (QGraphicsItem* i, edit->scene->selectedItems())
                    {
                        i->setSelected(false);
                    }
                    gr[i]->setSelected(true);
                    edit->goTo(((ItemBlock*)gr[i])->blockData.x, ((ItemBlock*)gr[i])->blockData.y, true, QPoint(-300, -300));
                    curSearchBlockIndex = i;
                    return false;
                }
            }
        }
    }
    //end search
    curSearchBlockIndex = 0;
    return true;
}

bool MainWindow::doSearchBGO(leveledit *edit)
{
    QList<QGraphicsItem*> gr = edit->scene->items();
    if(curSearchBGOIndex+1 < gr.size()){
        for(int i = curSearchBGOIndex+1; i < gr.size(); ++i){
            if(gr[i]->data(0).toString()=="BGO"){
                bool toBeFound = true;
                if(ui->Find_Check_TypeBGO->isChecked()&&toBeFound){
                    toBeFound = ((ItemBGO*)gr[i])->bgoData.id == (unsigned int)curSearchBGOID;
                }
                if(toBeFound){
                    foreach (QGraphicsItem* i, edit->scene->selectedItems())
                    {
                        i->setSelected(false);
                    }
                    gr[i]->setSelected(true);
                    edit->goTo(((ItemBGO*)gr[i])->bgoData.x, ((ItemBGO*)gr[i])->bgoData.y, true, QPoint(-300, -300));
                    curSearchBGOIndex = i;
                    return false;
                }
            }
        }
    }
    //end search
    curSearchBGOIndex = 0;
    return true;
}

bool MainWindow::doSearchNPC(leveledit *edit)
{
    QList<QGraphicsItem*> gr = edit->scene->items();
    if(curSearchNPCIndex+1 < gr.size()){
        for(int i = curSearchNPCIndex+1; i < gr.size(); ++i){
            if(gr[i]->data(0).toString()=="NPC"){
                bool toBeFound = true;
                if(ui->Find_Check_TypeNPC->isChecked()&&toBeFound){
                    toBeFound = ((ItemNPC*)gr[i])->npcData.id == (unsigned int)curSearchNPCID;
                }
                if(toBeFound){
                    foreach (QGraphicsItem* i, edit->scene->selectedItems())
                    {
                        i->setSelected(false);
                    }
                    gr[i]->setSelected(true);
                    edit->goTo(((ItemNPC*)gr[i])->npcData.x, ((ItemNPC*)gr[i])->npcData.y, true, QPoint(-300, -300));
                    curSearchNPCIndex = i;
                    return false;
                }
            }
        }
    }
    //end search
    curSearchNPCIndex = 0;
    return true;
}

void MainWindow::toggleNewWindow(QMdiSubWindow */*window*/)
{
    resetAllSearches();
}
