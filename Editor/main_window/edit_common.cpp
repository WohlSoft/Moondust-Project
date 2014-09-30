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

#include "../ui_mainwindow.h"
#include "../mainwindow.h"

#include "../file_formats/file_formats.h"
#include "music_player.h"
#include "global_settings.h"

//Reload opened file data
void MainWindow::on_actionReload_triggered()
{
    QString filePath;
    QRect wnGeom;

    if (activeChildWindow()==1)
    {
        LevelData FileData;
        filePath = activeLvlEditWin()->curFile;

        QFile fileIn(filePath);

        if (!fileIn.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("File open error"),
        tr("Can't open the file."), QMessageBox::Ok);
            return;
        }

        QFileInfo in_1(filePath);

        if(in_1.suffix().toLower() == "lvl")
            FileData = FileFormats::ReadLevelFile(fileIn);         //Read SMBX LVL File
        else
            FileData = FileFormats::ReadExtendedLevelFile(fileIn); //Read PGE LVLX File

        //FileData = FileFormats::ReadLevelFile(fileIn); //function in file_formats.cpp
        if( !FileData.ReadFileValid ){
            statusBar()->showMessage(tr("Reloading error"), 2000);
            return;}

        FileData.filename = QFileInfo(filePath).baseName();
        FileData.path = QFileInfo(filePath).absoluteDir().absolutePath();
        FileData.playmusic = GlobalSettings::autoPlayMusic;
        activeLvlEditWin()->LvlData.modified = false;
        activeLvlEditWin()->close();
        wnGeom = ui->centralWidget->activeSubWindow()->geometry();
        ui->centralWidget->activeSubWindow()->close();

        leveledit *child = createLvlChild();
        if ((bool) (child->loadFile(filePath, FileData, configs, GlobalSettings::LvlOpts))) {
            child->show();
            ui->centralWidget->activeSubWindow()->setGeometry(wnGeom);
            child->updateGeometry();
            child->ResetPosition();
            statusBar()->showMessage(tr("Level file reloaded"), 2000);
            updateMenus(true);
            SetCurrentLevelSection(0);

            if(GlobalSettings::autoPlayMusic) ui->actionPlayMusic->setChecked(true);
            LvlMusPlay::musicForceReset=true; //reset musics
            on_actionPlayMusic_triggered(ui->actionPlayMusic->isChecked());

        } else {
                WriteToLog(QtDebugMsg, ">>File loading aborted");
            child->show();
                WriteToLog(QtDebugMsg, ">>Window showed");
            if(activeChildWindow()==1) activeLvlEditWin()->LvlData.modified = false;
                WriteToLog(QtDebugMsg, ">>Option set");
            ui->centralWidget->activeSubWindow()->close();
                WriteToLog(QtDebugMsg, ">>Windows closed");
        }
    }
    else
    if (activeChildWindow()==2)
    {
        filePath = activeNpcEditWin()->curFile;
        QFile fileIn(filePath);

        if (!fileIn.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("File open error"),
        tr("Can't open the file."), QMessageBox::Ok);
            return;
        }

        NPCConfigFile FileData = FileFormats::ReadNpcTXTFile(fileIn);
        if( !FileData.ReadFileValid ) return;
        wnGeom = ui->centralWidget->activeSubWindow()->geometry();
        activeNpcEditWin()->isModyfied = false;
        //activeNpcEditWin()->close();
        ui->centralWidget->activeSubWindow()->close();

        npcedit *child = createNPCChild();
        if (child->loadFile(filePath, FileData)) {
            statusBar()->showMessage(tr("NPC Config reloaded"), 2000);
            child->show();
            ui->centralWidget->activeSubWindow()->setGeometry(wnGeom);
            updateMenus(true);
        } else {
            child->close();
        }
    }
    else
    if (activeChildWindow()==3)
    {
        WorldData FileData;
        filePath = activeWldEditWin()->curFile;

        QFile fileIn(filePath);

        if (!fileIn.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("File open error"),
        tr("Can't open the file."), QMessageBox::Ok);
            return;
        }

        QFileInfo in_1(filePath);

        //FileData = FileFormats::ReadWorldFile(fileIn); //function in file_formats.cpp
        if(in_1.suffix().toLower() == "wld")
            FileData = FileFormats::ReadWorldFile(fileIn);         //Read SMBX WLD File
        else
            FileData = FileFormats::ReadExtendedWorldFile(fileIn); //Read PGE WLDX File


        if( !FileData.ReadFileValid ){
            statusBar()->showMessage(tr("Reloading error"), 2000);
            return;}

        FileData.filename = QFileInfo(filePath).baseName();
        FileData.path = QFileInfo(filePath).absoluteDir().absolutePath();
        FileData.playmusic = GlobalSettings::autoPlayMusic;
        activeWldEditWin()->WldData.modified = false;
        activeWldEditWin()->close();
        wnGeom = ui->centralWidget->activeSubWindow()->geometry();
        ui->centralWidget->activeSubWindow()->close();

        WorldEdit *child = createWldChild();
        if ( (bool)(child->loadFile(filePath, FileData, configs, GlobalSettings::LvlOpts)) ) {
            child->show();
            ui->centralWidget->activeSubWindow()->setGeometry(wnGeom);
            child->updateGeometry();
            child->ResetPosition();
            updateMenus(true);
            setCurrentWorldSettings();
            if(FileData.HubStyledWorld)
            {
                ui->WorldSettings->setVisible(true);
                ui->WorldSettings->raise();
            }
            statusBar()->showMessage(tr("World map file loaded"), 2000);
        } else {
            WriteToLog(QtDebugMsg, ">>File loading aborted");
            child->show();
            WriteToLog(QtDebugMsg, ">>Window showed");
            if(activeChildWindow()==1) activeWldEditWin()->WldData.modified = false;
            WriteToLog(QtDebugMsg, ">>Option set");
            ui->centralWidget->activeSubWindow()->close();
            WriteToLog(QtDebugMsg, ">>Windows closed");
        }

    }

    clearFilter();
}


void MainWindow::on_actionExport_to_image_triggered()
{
    on_actionSelect_triggered();

    if(activeChildWindow()==1)
    {
        activeLvlEditWin()->ExportToImage_fn_piece();
    }
    else
    if(activeChildWindow()==3)
    {
        activeWldEditWin()->ExportToImage_fn();
    }
}

void MainWindow::on_actionExport_to_image_section_triggered()
{
    if(activeChildWindow()==1)
    {
        activeLvlEditWin()->ExportToImage_fn();
    }
}

void MainWindow::on_actionReset_position_triggered()
{
    if (activeChildWindow()==1)
    {
       activeLvlEditWin()->ResetPosition();
    }
    else
    if (activeChildWindow()==3)
    {
       activeWldEditWin()->ResetPosition();
    }
}



void MainWindow::on_actionAnimation_triggered(bool checked)
{
    GlobalSettings::LvlOpts.animationEnabled = checked;
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->opts.animationEnabled = GlobalSettings::LvlOpts.animationEnabled;
        if(GlobalSettings::LvlOpts.animationEnabled)
        {
            activeLvlEditWin()->scene->startBlockAnimation();
        }
        else
            activeLvlEditWin()->scene->stopAnimation();
    }
    else
    if (activeChildWindow()==3)
    {
        activeWldEditWin()->scene->opts.animationEnabled = GlobalSettings::LvlOpts.animationEnabled;
        if(GlobalSettings::LvlOpts.animationEnabled)
        {
            activeWldEditWin()->scene->startAnimation();
        }
        else
            activeWldEditWin()->scene->stopAnimation();
    }
}


void MainWindow::on_actionCollisions_triggered(bool checked)
{
    GlobalSettings::LvlOpts.collisionsEnabled = checked;
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->opts.collisionsEnabled = GlobalSettings::LvlOpts.collisionsEnabled;
    }
    else
    if (activeChildWindow()==3)
    {
        activeWldEditWin()->scene->opts.collisionsEnabled = GlobalSettings::LvlOpts.collisionsEnabled;
    }

}

// //////////////////////////////////////////////////////////////


void MainWindow::on_actionGridEn_triggered(bool checked)
{
    if (activeChildWindow()==1)
    {
       activeLvlEditWin()->scene->grid = checked;
    }
    else
    if (activeChildWindow()==3)
    {
       activeWldEditWin()->scene->grid = checked;
    }
}

// //History Manager
void MainWindow::on_actionUndo_triggered()
{
    ui->ItemProperties->hide();
    ui->WLD_ItemProps->hide();
    if (activeChildWindow()==1)
    {
        //Here must be call
        activeLvlEditWin()->scene->historyBack();
        ui->actionUndo->setEnabled( activeLvlEditWin()->scene->canUndo() );
        ui->actionRedo->setEnabled( activeLvlEditWin()->scene->canRedo() );
    }
    else if(activeChildWindow()==3)
    {
        activeWldEditWin()->scene->historyBack();
        ui->actionUndo->setEnabled( activeWldEditWin()->scene->canUndo() );
        ui->actionRedo->setEnabled( activeWldEditWin()->scene->canRedo() );
    }
}

void MainWindow::on_actionRedo_triggered()
{
    ui->ItemProperties->hide();
    ui->WLD_ItemProps->hide();
    if (activeChildWindow()==1)
    {
        //Here must be call
        activeLvlEditWin()->scene->historyForward();
        ui->actionUndo->setEnabled( activeLvlEditWin()->scene->canUndo() );
        ui->actionRedo->setEnabled( activeLvlEditWin()->scene->canRedo() );
    }
    else if(activeChildWindow()==3)
    {
        activeWldEditWin()->scene->historyForward();
        ui->actionUndo->setEnabled( activeWldEditWin()->scene->canUndo() );
        ui->actionRedo->setEnabled( activeWldEditWin()->scene->canRedo() );
    }
}
