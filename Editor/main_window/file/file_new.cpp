/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2023 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <audio/music_player.h>
#include <main_window/dock/toolboxes.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>

////////////////////////New files templates///////////////////////////


void MainWindow::on_actionNewFile_triggered()
{
    //QMessageBox::information(this, "BURP!", "BURP!");
    ui->menuNew->exec(QCursor::pos());
}

void MainWindow::on_actionNewNPC_config_triggered()
{
    //Check if data configs are valid
    if( configs.check() )
    {
        LogCritical(QString("Error! *.INI configs not loaded"));
        QMessageBox::warning(this, tr("Configuration is loaded with errors"),
                             tr("Cannot create NPC config file:\nThe configuration pack was loaded, but contains errors.").arg(ConfStatus::configPath));
        //Show configuration status window
        on_actionCurConfig_triggered();
        return;
    }

    //NpcDialog * npcList = new NpcDialog(&configs);
    ItemSelectDialog * npcList = new ItemSelectDialog(&configs, ItemSelectDialog::TAB_NPC,0,0,0,0,0,0,0,0,0,this,ItemSelectDialog::TAB_NPC);
    npcList->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    npcList->setGeometry(util::alignToScreenCenter(npcList->size()));
    npcList->setWindowTitle(tr("Create new NPC.txt configuration file"));
    if(npcList->exec() == QDialog::Accepted)
    {
        NpcEdit *child = createNPCChild();
        child->newFile((unsigned long)npcList->npcID);
        child->show();
    }
    delete npcList;
}



void MainWindow::on_actionNewLevel_triggered()
{
    //Check if data configs are valid
    if( configs.check() )
    {
        LogCritical(QString("Error! *.INI configs not loaded"));
        QMessageBox::warning(this, tr("Configuration is loaded with errors"),
                             tr("Cannot create level file:\nThe configuration pack was loaded, but contains errors.").arg(ConfStatus::configPath));
        //Show configuration status window
        on_actionCurConfig_triggered();
        return;
    }

    QMdiSubWindow * SubWindow;
    LevelEdit *child = createLvlChild(&SubWindow);
    if(child->newFile(configs, GlobalSettings::LvlOpts))
    {
        child->show();
        child->updateGeometry();
        child->resetPosition();
        dock_LvlItemBox->initItemLists();
        setCurrentLevelSection(0);
        on_actionSelect_triggered();
        dock_LvlWarpProps->init();
        dock_LvlLayers->setLayersBox();
        if(GlobalSettings::autoPlayMusic) ui->actionPlayMusic->setChecked(true);
        LvlMusPlay::musicForceReset=true; //reset musics
        on_actionPlayMusic_triggered(ui->actionPlayMusic->isChecked());
    } else {
        if( activeLvlEditWin(SubWindow) )
            activeLvlEditWin(SubWindow)->LvlData.meta.modified = false;
        SubWindow->close();
    }
}


void MainWindow::on_actionNewWorld_map_triggered()
{
    //Check if data configs are valid
    if( configs.check() )
    {
        LogCritical(QString("Error! *.INI configs not loaded"));
        QMessageBox::warning(this, tr("Configuration is loaded with errors"),
                             tr("Cannot create world map file:\nThe configuration pack was loaded, but contains errors.").arg(ConfStatus::configPath));
        //Show configuration status window
        on_actionCurConfig_triggered();
        return;
    }

    QMdiSubWindow* SubWindow;
    WorldEdit *child = createWldChild(&SubWindow);
    if(child->newFile(configs, GlobalSettings::LvlOpts))
    {
        child->show();
        child->updateGeometry();
        child->ResetPosition();
        dock_WldItemBox->initItemLists();
        on_actionSelect_triggered();
    } else {
        if( activeWldEditWin(SubWindow) )
            activeWldEditWin(SubWindow)->WldData.meta.modified = false;
        SubWindow->close();
    }
}

