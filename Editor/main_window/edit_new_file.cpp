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

#include "ui_mainwindow.h"
#include "mainwindow.h"

#include "npc_dialog/npcdialog.h"
#include "item_select_dialog/itemselectdialog.h"
#include "music_player.h"

////////////////////////New files templates///////////////////////////

void MainWindow::on_actionNewNPC_config_triggered()
{
    //NpcDialog * npcList = new NpcDialog(&configs);
    ItemSelectDialog * npcList = new ItemSelectDialog(&configs, ItemSelectDialog::TAB_NPC);
    npcList->removeEmptyEntry(ItemSelectDialog::TAB_NPC);
    npcList->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    npcList->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, npcList->size(), qApp->desktop()->availableGeometry()));
    npcList->setWindowTitle(tr("Create new NPC.txt configuration file"));
    if(npcList->exec()==QDialog::Accepted)
    {
        npcedit *child = createNPCChild();
        child->newFile( npcList->npcID );
        child->show();
    }
    delete npcList;

}



void MainWindow::on_actionNewLevel_triggered()
{
    leveledit *child = createLvlChild();
    child->newFile(configs, GlobalSettings::LvlOpts);
    child->show();
    updateMenus(true);
    SetCurrentLevelSection(0);
    on_actionSelect_triggered();
    setDoorsToolbox();
    setLayersBox();

    if(GlobalSettings::autoPlayMusic) ui->actionPlayMusic->setChecked(true);
    LvlMusPlay::musicForceReset=true; //reset musics
    on_actionPlayMusic_triggered(ui->actionPlayMusic->isChecked());
}


void MainWindow::on_actionNewWorld_map_triggered()
{
    QMessageBox::information(this, "Comming soon", "World map editor in this version is not implemented", QMessageBox::Ok);
}
