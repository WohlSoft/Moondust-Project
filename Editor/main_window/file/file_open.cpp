/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <PGE_File_Formats/file_formats.h>
#include <audio/music_player.h>
#include <main_window/global_settings.h>
#include <main_window/dock/lvl_warp_props.h>
#include <main_window/dock/lvl_sctc_props.h>
#include <main_window/dock/lvl_layers_box.h>
#include <main_window/dock/wld_settings_box.h>
#include <common_features/bool_reseter.h>
#include <common_features/util.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>

void MainWindow::openFilesByArgs(QStringList args)
{
    for(int i=1; i<args.size(); i++)
    {
        if(QFile::exists(args[i])) OpenFile(args[i]);
    }
}

void MainWindow::on_action_openEpisodeFolder_triggered()
{
    QString path;
    bool isUntitled=false;
    if(activeChildWindow()==1)
    {
        LevelEdit *e = activeLvlEditWin();
        if(e)
        {
            path=e->LvlData.path;
            isUntitled=e->isUntitled;
        }
    } else if(activeChildWindow()==2) {
        NpcEdit *e = activeNpcEditWin();
        if(e)
        {
            path=QFileInfo(e->curFile).absoluteDir().absolutePath();
            isUntitled=e->isUntitled;
        }
    } else if(activeChildWindow()==3) {
        WorldEdit *e = activeWldEditWin();
        if(e)
        {
            path=e->WldData.path;
            isUntitled=e->isUntitled;
        }
    }

    if(isUntitled)
    {
        QMessageBox::warning(this, tr("Untitled file"), tr("Please save file to the disk first."));
        return;
    }

    if(!path.isEmpty())
    {
        QDesktopServices::openUrl(QUrl("file:///"+path));
    }
}

void MainWindow::on_action_openCustomFolder_triggered()
{
    QString path;
    bool isUntitled=false;
    if(activeChildWindow()==1)
    {
        LevelEdit *e = activeLvlEditWin();
        if(e)
        {
            path=e->LvlData.path+"/"+e->LvlData.filename;
            isUntitled=e->isUntitled;
        }
    } else if(activeChildWindow()==2) {
        return;
    } else if(activeChildWindow()==3) {
        WorldEdit *e = activeWldEditWin();
        if(e)
        {
            path=e->WldData.path+"/"+e->WldData.filename;
            isUntitled=e->isUntitled;
        }
    }

    if(isUntitled)
    {
        QMessageBox::warning(this, tr("Untitled file"), tr("Please save file to the disk first."));
        return;
    }

    if(!path.isEmpty())
    {
        if(!QFileInfo(path).dir().exists())
        {
            QDir(path).mkpath(path);
        }
        QDesktopServices::openUrl(QUrl("file:///"+path));
    }
}

void MainWindow::on_OpenFile_triggered()
{
    //Check if data configs are valid
    if( configs.check() )
    {
        LogCritical(QString("Error! *.INI configs not loaded"));
        QMessageBox::warning(this, tr("Configuration is loaded with errors"),
                             tr("Editor cannot open files:\nConfiguration package is loaded with errors.").arg(ConfStatus::configPath));
        //Show configuration status window
        on_actionCurConfig_triggered();
        return;
    }

     QString fileName_DATA = QFileDialog::getOpenFileName(this,
        trUtf8("Open file"),GlobalSettings::openPath,
        QString("All supported formats (*.lvlx *.wldx *.lvl *.wld npc-*.txt *.sav);;"
        "All SMBX files (*.lvl *.wld npc-*.txt);;"
        "All PGE files (*.lvlx *.wldx npc-*.txt);;"
        "SMBX Level (*.lvl);;"
        "PGE Level (*.lvlx);;"
        "SMBX World (*.wld);;"
        "PGE World (*.wldx);;"
        "SMBX NPC Config (npc-*.txt);;"
        "SMBX Game Save file (*.sav);;"
        "All Files (*.*)"),0);

        if(fileName_DATA==NULL) return;

        OpenFile(fileName_DATA);
}

void MainWindow::OpenFile(QString FilePath, bool addToRecentList)
{
    if(_is_reloading) return;
    _is_reloading=true;
    BoolReseter rst(&_is_reloading);
    Q_UNUSED(rst);

    if(!continueLoad) return;
    qApp->setActiveWindow(this);

    //Check if data configs are valid
    if( configs.check() )
    {
        LogCritical(QString("Error! *.INI configs not loaded"));
        QMessageBox::warning(this, tr("Configuration is loaded with errors"),
                             tr("Cannot open file:\nConfiguration package loaded with errors.").arg(ConfStatus::configPath));
        //Show configuration status window
        on_actionCurConfig_triggered();
        return;
    }

    QMdiSubWindow *existing = findOpenedFileWin(FilePath);
            if (existing) {
                ui->centralWidget->setActiveSubWindow(existing);
                return;
            }

    QFile file(FilePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this, tr("File open error"),
        tr("Can't open the file: %1").arg(file.errorString()), QMessageBox::Ok);
        return;
    }

    QFileInfo in_1(FilePath);

    GlobalSettings::openPath = in_1.absoluteDir().absolutePath();

    if((in_1.suffix().toLower() == "lvl")||(in_1.suffix().toLower() == "lvlx"))
    {
        LevelData FileData;

        LogDebug("> parsing level file format");
        if( !FileFormats::OpenLevelFile(FilePath, FileData) )
        {
            formatErrorMsgBox(FilePath, FileData.ERROR_info, FileData.ERROR_linenum, FileData.ERROR_linedata);
            return;
        }
        LogDebug("File was read!");
        FileData.filename = util::getBaseFilename(in_1.fileName());
        FileData.path = in_1.absoluteDir().absolutePath();
        FileData.playmusic = GlobalSettings::autoPlayMusic;
        file.close();

        LogDebug("Creating of sub-window");
        LevelEdit *child = createLvlChild();
        if ( (bool)(child->loadFile(FilePath, FileData, configs, GlobalSettings::LvlOpts)) )
        {
            child->show();
            child->updateGeometry();
            child->ResetPosition();
            statusBar()->showMessage(tr("Level file loaded"), 2000);
            //updateMenus(true);
            SetCurrentLevelSection(0);
            dock_LvlWarpProps->init();
            dock_LvlLayers->setLayersBox();

            if(GlobalSettings::autoPlayMusic) ui->actionPlayMusic->setChecked(true);
            on_actionPlayMusic_triggered(ui->actionPlayMusic->isChecked());

        } else {
            LogDebug(">>File loading aborted");
            child->show();
            if(activeChildWindow()==1) activeLvlEditWin()->LvlData.modified = false;
            ui->centralWidget->activeSubWindow()->close();
            LogDebug(">>Windows closed");
        }
    }
    else
    if((in_1.suffix().toLower() == "wld")||(in_1.suffix().toLower() == "wldx"))
    {
        WorldData FileData;
        if( !FileFormats::OpenWorldFile( FilePath, FileData ) )
        {
            formatErrorMsgBox(FilePath, FileData.ERROR_info, FileData.ERROR_linenum, FileData.ERROR_linedata);
            return;
        }

        file.close();

        WorldEdit *child = createWldChild();
        if ( (bool)(child->loadFile(FilePath, FileData, configs, GlobalSettings::LvlOpts)) ) {
            child->show();
            child->updateGeometry();
            child->ResetPosition();
            //updateMenus(true);
            dock_WldSettingsBox->setCurrentWorldSettings();
            if(FileData.HubStyledWorld)
            {
                dock_WldSettingsBox->setVisible(true);
                dock_WldSettingsBox->raise();
            }
            statusBar()->showMessage(tr("World map file loaded"), 2000);
        } else {
            child->close();
        }

        //QMessageBox::information(this, tr("Dummy"),
        //tr("Sorry, the World Maps support is not inplemented in this version."),
        //QMessageBox::Ok);

    }
    else
    if(in_1.suffix().toLower() == "txt")
    {
        NPCConfigFile FileData;
        if( !FileFormats::ReadNpcTXTFileF(FilePath, FileData) )
        {
            QMessageBox::critical(this, QObject::tr("File open error"), tr("Can't read the file"), QMessageBox::Ok);
            return;
        }

        NpcEdit *child = createNPCChild();
        if (child->loadFile(FilePath, FileData)) {
            statusBar()->showMessage(tr("NPC Config loaded"), 2000);
            child->show();
            //updateMenus(true);
        } else {
            child->close();
        }
    }
    else
    if(in_1.suffix().toLower() == "sav")
    {
        GamesaveData FileData;

        QString statistics;
        if(!FileFormats::ReadSMBX64SavFileF( FilePath, FileData))
        {
            formatErrorMsgBox( FilePath, FileData.ERROR_info, FileData.ERROR_linenum, FileData.ERROR_linedata );
            return;
        }

        statistics+= QString("SMBX Game Save file version %1\n\n").arg(FileData.version);
        if(FileData.gameCompleted)
            statistics+= "      ====This game was completed====\n\n";
        statistics+= QString("Lives: %1,   Coins:%2,   ").arg(FileData.lives).arg(FileData.coins);
        statistics+= QString("Total stars: %1,   Gotten stars: %2\n")
                .arg(FileData.totalStars)
                .arg(FileData.gottenStars.size());
        statistics+= QString("Position X=%1, Y=%2\n")
                .arg(FileData.worldPosX)
                .arg(FileData.worldPosY);

        if(FileData.musicID>0)
        {
            long item=configs.getMusWldI(FileData.musicID);
            if(item>=0)
            {
                statistics+= QString("Current music: %1\n\n").arg(configs.main_music_wld[item].name);
            }
        }

        statistics += "\n===========Players:============\n";
        for(int i=0; i<FileData.characterStates.size();i++)
        {
            if(i<configs.characters.size())
                statistics += QString("%1:\n").arg(configs.characters[i].name);
            else
                statistics += QString("<unknown character>:\n");
            statistics += QString("Health: %1,    ").arg(FileData.characterStates[i].health);
            statistics += QString("Power-UP: %1,    ").arg(FileData.characterStates[i].state);
            switch(FileData.characterStates[i].mountType)
            {
            case 0:
                break;
            case 1:
                statistics += QString("Mounted transport: Shoe - ");
                switch(FileData.characterStates[i].mountID)
                {
                case 1:
                    statistics += QString("Kuribo's (green)");break;
                case 2:
                    statistics += QString("Podoboo's (red)");break;
                case 3:
                    statistics += QString("Lakitu's (blue)");break;
                default:
                    statistics += QString("of God Power :D");break;
                }
                break;
            case 3:
                statistics += QString("Mounted transport: Yoshi");break;
            default:
                statistics += QString("Mounted transport: <unknown>");break;
            }

            if( (FileData.characterStates[i].itemID>0) && configs.main_npc.contains(FileData.characterStates[i].itemID) )
            {
                statistics += QString("%2Has item: %1").arg(configs.main_npc[FileData.characterStates[i].itemID].name)
                                .arg(FileData.characterStates[i].mountType>0?",    ":"");
            }

        if(i<FileData.characterStates.size()-1)
            statistics += "\n----------------------------\n";
        }
        statistics += "\n=========================\n";

        if( !FileData.ReadFileValid ) return;

        QMessageBox::information(this, tr("Game save statistics"),
         statistics,
         QMessageBox::Ok);

    }
    else
    {
    QMessageBox::warning(this, tr("Bad file"),
     tr("This file have unknown extension"),
     QMessageBox::Ok);
    return;
    }

    // Add to recent fileList
    if(addToRecentList){
        AddToRecentFiles(FilePath);
        SyncRecentFiles();
    }
}
