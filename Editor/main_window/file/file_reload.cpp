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

#include <file_formats/file_formats.h>
#include <audio/music_player.h>
#include <script/gui/additionalsettings.h>
#include <main_window/global_settings.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>

//Reload opened file data
void MainWindow::on_actionReload_triggered()
{
    QString filePath;
    QRect wnGeom;

    if (activeChildWindow()==1)
    {
        LevelData FileData;
        filePath = activeLvlEditWin()->curFile;

        if(activeLvlEditWin()->isUntitled)
        {
                    QMessageBox::warning(this, tr("File not saved"),
                    tr("File doesn't saved on disk."), QMessageBox::Ok);
                        return;
        }

        if (!QFileInfo(filePath).exists() ) {
            QMessageBox::critical(this, tr("File open error"),
            tr("Can't open the file.\nFile not exist."), QMessageBox::Ok);
                return;
        }

        if(activeLvlEditWin()->LvlData.modified)
        {
            QMessageBox::StandardButton ret = QMessageBox::question(this, tr("Reload file and custom stuff"),
            tr("Do you want to save before reload stuff?"), QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
            if((ret==QMessageBox::Cancel)||(ret==0))
                return;
            else
              if(ret==QMessageBox::Yes)
                save();
        }

        //Open level file
        FileData = FileFormats::OpenLevelFile(filePath);

        if( !FileData.ReadFileValid ){
            statusBar()->showMessage(tr("Reloading error"), 2000);
            return;}

        FileData.playmusic = GlobalSettings::autoPlayMusic;
        activeLvlEditWin()->LvlData.modified = false;

        QFile file(filePath+".meta");
        if(QFileInfo(filePath+".meta").exists())
        {
            if (file.open(QIODevice::ReadOnly))
            {
                QString metaRaw;
                QTextStream meta(&file);
                meta.setCodec("UTF-8");
                metaRaw = meta.readAll();
                if(FileData.metaData.script)
                {
                    delete FileData.metaData.script;
                    FileData.metaData.script = NULL;
                }
                FileData.metaData = FileFormats::ReadNonSMBX64MetaData(metaRaw, filePath+".meta");
            }
            else
            {
                QMessageBox::critical(this, tr("File open error"),
                tr("Can't open the file."), QMessageBox::Ok);
            }
        }

        activeLvlEditWin()->close();
        wnGeom = ui->centralWidget->activeSubWindow()->geometry();
        ui->centralWidget->activeSubWindow()->close();

        LevelEdit *child = createLvlChild();
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

        if(activeNpcEditWin()->isUntitled)
        {
                    QMessageBox::warning(this, tr("File not saved"),
                    tr("File doesn't saved on disk."), QMessageBox::Ok);
                        return;
        }

        if(activeNpcEditWin()->isModyfied)
        {
            QMessageBox::StandardButton ret = QMessageBox::question(this, tr("Reload file and custom stuff"),
            tr("Do you want to save before reload stuff?"), QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
            if((ret==QMessageBox::Cancel)||(ret==0))
                return;
            else
              if(ret==QMessageBox::Yes)
                save();
        }

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

        NpcEdit *child = createNPCChild();
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

        if(activeWldEditWin()->isUntitled)
        {
                    QMessageBox::warning(this, tr("File not saved"),
                    tr("File doesn't saved on disk."), QMessageBox::Ok);
                        return;
        }

        if (!QFileInfo(filePath).exists() ) {
            QMessageBox::critical(this, tr("File open error"),
            tr("Can't open the file.\nFile not exist."), QMessageBox::Ok);
                return;
        }

        if(activeWldEditWin()->WldData.modified){
            QMessageBox::StandardButton ret = QMessageBox::question(this, tr("Reload file and custom stuff"),
            tr("Do you want to save before reload stuff?"), QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
            if((ret==QMessageBox::Cancel)||(ret==0))
                return;
            else
              if(ret==QMessageBox::Yes)
                save();
        }

        FileData = FileFormats::OpenWorldFile(filePath);

        if( !FileData.ReadFileValid ){
            statusBar()->showMessage(tr("Reloading error"), 2000);
            return;}

        FileData.filename = QFileInfo(filePath).baseName();
        FileData.path = QFileInfo(filePath).absoluteDir().absolutePath();
        FileData.playmusic = GlobalSettings::autoPlayMusic;
        activeWldEditWin()->WldData.modified = false;

        QFile file(filePath+".meta");
        if(QFileInfo(filePath+".meta").exists())
        {
            if (file.open(QIODevice::ReadOnly))
            {
                QString metaRaw;
                QTextStream meta(&file);
                meta.setCodec("UTF-8");
                metaRaw = meta.readAll();
                FileData.metaData = FileFormats::ReadNonSMBX64MetaData(metaRaw, filePath+".meta");
            }
            else
            {
                QMessageBox::critical(this, tr("File open error"),
                tr("Can't open the file."), QMessageBox::Ok);
            }
        }

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


