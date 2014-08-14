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
#include "../../file_formats/wld_filedata.h"
#include "../../world_scene/item_level.h"
#include "../../world_scene/item_music.h"
#include "../../world_scene/item_path.h"
#include "../../world_scene/item_scene.h"
#include "../../world_scene/item_tile.h"
bool lockResetWorld = false;


void MainWindow::on_actionWLD_SearchBox_triggered(bool checked)
{
    ui->WorldFindDock->setVisible(checked);
    if(checked)ui->WorldFindDock->raise();
}

void MainWindow::on_WorldFindDock_visibilityChanged(bool visible)
{
    ui->actionWLD_SearchBox->setChecked(visible);
}

void MainWindow::on_Find_Button_ResetLevel_clicked()
{
    if(!(currentSearches & SEARCH_LEVEL)){
        ui->Find_Check_TypeLevel->setChecked(true);
        ui->Find_Button_TypeLevel->setText(tr("[empty]"));
        curSearchLevel.id = 0;
        ui->Find_Check_PathBackgroundActive->setChecked(false);
    }else{
        currentSearches ^= SEARCH_LEVEL;
        ui->Find_Button_ResetLevel->setText(tr("Reset Search Fields"));
        ui->FindStartLevel->setText(tr("Search Level"));
        curSearchLevel.index = 0;
    }
}

void MainWindow::on_Find_Button_TypeLevel_clicked()
{
    ItemSelectDialog* selBlock = new ItemSelectDialog(&configs, ItemSelectDialog::TAB_LEVEL,0,0,0,0,0,0,0,curSearchLevel.id);
    if(selBlock->exec()==QDialog::Accepted){
        int selected = selBlock->levelID;
        curSearchLevel.id = selected;
        ui->Find_Button_TypeLevel->setText(((selected>0)?QString("Level-%1").arg(selected):tr("[empty]")));
    }
    delete selBlock;
}


void MainWindow::on_FindStartLevel_clicked()
{
    if(!(currentSearches & SEARCH_LEVEL)){ //start search
        if(activeChildWindow()==3){
            currentSearches |= SEARCH_LEVEL;
            ui->FindStartLevel->setText(tr("Next Level"));
            ui->Find_Button_ResetLevel->setText(tr("Stop Search"));
            WorldEdit* edit = activeWldEditWin();
            if(doSearchLevel(edit)){
                currentSearches ^= SEARCH_LEVEL;
                ui->Find_Button_ResetLevel->setText(tr("Reset Search Fields"));
                ui->FindStartLevel->setText(tr("Search Level"));
                QMessageBox::information(this, tr("Search Complete"), tr("Level search completed!"));
            }
        }
    }else{ //middle in a search
        if(activeChildWindow()==3){
            WorldEdit* edit = activeWldEditWin();
            if(doSearchLevel(edit)){
                currentSearches ^= SEARCH_LEVEL;
                ui->Find_Button_ResetLevel->setText(tr("Reset Search Fields"));
                ui->FindStartLevel->setText(tr("Search Level"));
                QMessageBox::information(this, tr("Search Complete"), tr("Level search completed!"));
            }
        }
    }
}

bool MainWindow::doSearchLevel(WorldEdit *edit)
{
    QList<QGraphicsItem*> gr = edit->scene->items();
    if(curSearchLevel.index+1 < (unsigned int)gr.size()){
        for(int i = curSearchLevel.index+1; i < gr.size(); ++i){
            if(gr[i]->data(0).toString()=="LEVEL"){
                bool toBeFound = true;
                if(ui->Find_Check_TypeLevel->isChecked()&&curSearchLevel.id!=0&&toBeFound){
                    toBeFound = ((ItemLevel*)gr[i])->levelData.id == (unsigned int)curSearchLevel.id;
                }
                if(ui->Find_Check_PathBackground->isChecked()&&toBeFound){
                    toBeFound = ((ItemLevel*)gr[i])->levelData.pathbg == ui->Find_Check_PathBackgroundActive->isChecked();
                }
                if(toBeFound){
                    foreach (QGraphicsItem* i, edit->scene->selectedItems())
                    {
                        i->setSelected(false);
                    }
                    gr[i]->setSelected(true);
                    edit->goTo(((ItemLevel*)gr[i])->levelData.x, ((ItemLevel*)gr[i])->levelData.y, false, QPoint(-300, -300));
                    curSearchLevel.index = i;
                    return false;
                }
            }
        }
    }
    //end search
    curSearchLevel.index = 0;
    return true;
}

void MainWindow::resetAllSearchFieldsWLD()
{
    resetAllSearchesWLD();

    on_Find_Button_ResetLevel_clicked();
}

void MainWindow::resetAllSearchesWLD()
{
    resetLevelSearch();
}

void MainWindow::resetLevelSearch()
{
    if(lockResetWorld) return;
    lockResetWorld = true;
    if(currentSearches & SEARCH_LEVEL) on_Find_Button_ResetLevel_clicked();
    lockResetWorld = false;
}
