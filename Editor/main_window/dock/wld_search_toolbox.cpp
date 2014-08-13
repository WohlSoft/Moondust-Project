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



void MainWindow::on_FindStartLevel_clicked()
{

}

bool MainWindow::doSearchLevel(WorldEdit *edit)
{
    QList<QGraphicsItem*> gr = edit->scene->items();
    if(curSearchLevel.index+1 < (unsigned int)gr.size()){
        for(int i = curSearchLevel.index+1; i < gr.size(); ++i){
            if(gr[i]->data(0).toString()=="LEVEL"){
                bool toBeFound = true;
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
