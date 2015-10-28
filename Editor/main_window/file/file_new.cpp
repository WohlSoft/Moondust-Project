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
#include <audio/music_player.h>
#include <main_window/dock/toolboxes.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>

void MainWindow::on_actionNew_triggered()
{
    ui->menuNew->exec( this->cursor().pos() );
}


////////////////////////New files templates///////////////////////////

void MainWindow::on_actionNewNPC_config_triggered()
{
    //Check if data configs are valid
    if( configs.check() )
    {
        WriteToLog(QtCriticalMsg, QString("Error! *.INI configs not loaded"));
        QMessageBox::warning(this, tr("Configuration is loaded with errors"),
                             tr("Cannot create NPC config file:\nConfiguration package loaded with errors.").arg(ConfStatus::configPath));
        //Show configuration status window
        on_actionCurConfig_triggered();
        return;
    }

    //NpcDialog * npcList = new NpcDialog(&configs);
    ItemSelectDialog * npcList = new ItemSelectDialog(&configs, ItemSelectDialog::TAB_NPC,0,0,0,0,0,0,0,0,0,this);
    npcList->removeEmptyEntry(ItemSelectDialog::TAB_NPC);
    npcList->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    npcList->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, npcList->size(), qApp->desktop()->availableGeometry()));
    npcList->setWindowTitle(tr("Create new NPC.txt configuration file"));
    if(npcList->exec()==QDialog::Accepted)
    {
        NpcEdit *child = createNPCChild();
        child->newFile( npcList->npcID );
        child->show();
    }
    delete npcList;
    updateMenus(true);
}



void MainWindow::on_actionNewLevel_triggered()
{
    //Check if data configs are valid
    if( configs.check() )
    {
        WriteToLog(QtCriticalMsg, QString("Error! *.INI configs not loaded"));
        QMessageBox::warning(this, tr("Configuration is loaded with errors"),
                             tr("Cannot create level file:\nConfiguration package loaded with errors.").arg(ConfStatus::configPath));
        //Show configuration status window
        on_actionCurConfig_triggered();
        return;
    }

    LevelEdit *child = createLvlChild();
    if(child->newFile(configs, GlobalSettings::LvlOpts))
    {
        child->show();
        child->updateGeometry();
        child->ResetPosition();
        updateMenus(true);
        SetCurrentLevelSection(0);
        on_actionSelect_triggered();
        dock_LvlWarpProps->init();
        dock_LvlLayers->setLayersBox();

        if(GlobalSettings::autoPlayMusic) ui->actionPlayMusic->setChecked(true);
        LvlMusPlay::musicForceReset=true; //reset musics
        on_actionPlayMusic_triggered(ui->actionPlayMusic->isChecked());
    } else {
        child->show();
        if(activeChildWindow()==1) activeLvlEditWin()->LvlData.modified = false;
        ui->centralWidget->activeSubWindow()->close();
    }
}


void MainWindow::on_actionNewWorld_map_triggered()
{
    //Check if data configs are valid
    if( configs.check() )
    {
        WriteToLog(QtCriticalMsg, QString("Error! *.INI configs not loaded"));
        QMessageBox::warning(this, tr("Configuration is loaded with errors"),
                             tr("Cannot create world map file:\nConfiguration package loaded with errors.").arg(ConfStatus::configPath));
        //Show configuration status window
        on_actionCurConfig_triggered();
        return;
    }

    WorldEdit *child = createWldChild();
    if(child->newFile(configs, GlobalSettings::LvlOpts))
    {
        child->show();
        child->updateGeometry();
        child->ResetPosition();
        updateMenus(true);
        on_actionSelect_triggered();
    } else {
        child->show();
        if(activeChildWindow()==3) activeWldEditWin()->WldData.modified = false;
        ui->centralWidget->activeSubWindow()->close();
    }
}

