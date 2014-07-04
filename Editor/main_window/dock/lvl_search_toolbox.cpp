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

void MainWindow::on_FindStartNPC_clicked()
{
    ItemSelectDialog* selDia = new ItemSelectDialog(&configs, ItemSelectDialog::TAB_BLOCK | ItemSelectDialog::TAB_NPC | ItemSelectDialog::TAB_BGO,
                                                    ItemSelectDialog::NPCEXTRA_WITHCOINS | ItemSelectDialog::NPCEXTRA_ISCOINSELECTED, 0, 0, 20);
    selDia->removeEmptyEntry(ItemSelectDialog::TAB_BLOCK | ItemSelectDialog::TAB_NPC | ItemSelectDialog::TAB_BGO);
    if(selDia->exec()==QDialog::Accepted){

    }
    delete selDia;
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
