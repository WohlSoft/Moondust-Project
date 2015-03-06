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

#include <PGE_File_Formats/file_formats.h>
#include <audio/music_player.h>
#include <main_window/global_settings.h>
#include <main_window/dock/lvl_warp_props.h>
#include <main_window/dock/lvl_sctc_props.h>
#include <main_window/dock/lvl_layers_box.h>
#include <main_window/dock/wld_settings_box.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>


void MainWindow::openFilesByArgs(QStringList args)
{
    for(int i=1; i<args.size(); i++)
    {
        if(QFile::exists(args[i])) OpenFile(args[i]);
    }
}

void MainWindow::on_OpenFile_triggered()
{
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
    if(!continueLoad) return;
    qApp->setActiveWindow(this);

    QMdiSubWindow *existing = findOpenedFileWin(FilePath);
            if (existing) {
                ui->centralWidget->setActiveSubWindow(existing);
                return;
            }

    QFile file(FilePath);

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("File open error"),
        tr("Can't open the file."), QMessageBox::Ok);
        return;
    }

    QFileInfo in_1(FilePath);

    GlobalSettings::openPath = in_1.absoluteDir().absolutePath();

    if((in_1.suffix().toLower() == "lvl")||(in_1.suffix().toLower() == "lvlx"))
    {

        LevelData FileData;

        WriteToLog(QtDebugMsg, "> parsing level file format");
        if(in_1.suffix().toLower() == "lvl")
            FileData = FileFormats::ReadLevelFile(file);         //Read SMBX LVL File
        else
            FileData = FileFormats::ReadExtendedLevelFile(file); //Read PGE LVLX File

        if( !FileData.ReadFileValid ) return;
        WriteToLog(QtDebugMsg, "File was read!");
        FileData.filename = in_1.baseName();
        FileData.path = in_1.absoluteDir().absolutePath();
        FileData.playmusic = GlobalSettings::autoPlayMusic;

        file.close();
        WriteToLog(QtDebugMsg, "> Opem meta-file");
        file.setFileName(FilePath+".meta");
        if(QFileInfo(FilePath+".meta").exists())
        {
            WriteToLog(QtDebugMsg, "> meta-file found, open them!");
            if (file.open(QIODevice::ReadOnly))
            {
                QString metaRaw;
                QTextStream meta(&file);
                meta.setCodec("UTF-8");
                metaRaw = meta.readAll();
                if(FileData.metaData.script != NULL)
                {
                    delete FileData.metaData.script;
                    FileData.metaData.script = NULL;
                }

                FileData.metaData = FileFormats::ReadNonSMBX64MetaData(metaRaw, FilePath+".meta");
                WriteToLog(QtDebugMsg, "Meta-File was read!");
            }
            else
            {
                QMessageBox::critical(this, tr("File open error"),
                tr("Can't open the file."), QMessageBox::Ok);
            }
        }

        WriteToLog(QtDebugMsg, "Creating of sub-window");
        LevelEdit *child = createLvlChild();
        if ( (bool)(child->loadFile(FilePath, FileData, configs, GlobalSettings::LvlOpts)) )
        {
            child->show();
            child->updateGeometry();
            child->ResetPosition();
            statusBar()->showMessage(tr("Level file loaded"), 2000);
            updateMenus(true);
            SetCurrentLevelSection(0);
            dock_LvlWarpProps->init();
            dock_LvlLayers->setLayersBox();

            if(GlobalSettings::autoPlayMusic) ui->actionPlayMusic->setChecked(true);
            on_actionPlayMusic_triggered(ui->actionPlayMusic->isChecked());

        } else {
            WriteToLog(QtDebugMsg, ">>File loading aborted");
            child->show();
            WriteToLog(QtDebugMsg, ">>Window showed");
            if(activeChildWindow()==1) activeLvlEditWin()->LvlData.modified = false;
            WriteToLog(QtDebugMsg, ">> Option seted");
            ui->centralWidget->activeSubWindow()->close();
            WriteToLog(QtDebugMsg, ">>Windows closed");
        }
    }
    else
    if((in_1.suffix().toLower() == "wld")||(in_1.suffix().toLower() == "wldx"))
    {
        WorldData FileData;
        if(in_1.suffix().toLower() == "wld")
            FileData= FileFormats::ReadWorldFile(file);
        else
            FileData= FileFormats::ReadExtendedWorldFile(file);

        if( !FileData.ReadFileValid ) return;

        file.close();
        file.setFileName(FilePath+".meta");
        if(QFileInfo(FilePath+".meta").exists())
        {
            if (file.open(QIODevice::ReadOnly))
            {
                QString metaRaw;
                QTextStream meta(&file);
                meta.setCodec("UTF-8");
                metaRaw = meta.readAll();
                FileData.metaData = FileFormats::ReadNonSMBX64MetaData(metaRaw, FilePath+".meta");
            }
            else
            {
                QMessageBox::critical(this, tr("File open error"),
                tr("Can't open the file."), QMessageBox::Ok);
            }
        }

        WorldEdit *child = createWldChild();
        if ( (bool)(child->loadFile(FilePath, FileData, configs, GlobalSettings::LvlOpts)) ) {
            child->show();
            child->updateGeometry();
            child->ResetPosition();
            updateMenus(true);
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
        NPCConfigFile FileData = FileFormats::ReadNpcTXTFile(file);
        if( !FileData.ReadFileValid ) return;

        NpcEdit *child = createNPCChild();
        if (child->loadFile(FilePath, FileData)) {
            statusBar()->showMessage(tr("NPC Config loaded"), 2000);
            child->show();
            updateMenus(true);
        } else {
            child->close();
        }
    }
    else
    if(in_1.suffix().toLower() == "sav")
    {
        GamesaveData FileData;

        QTextStream in(&file);   //Read File
        in.setAutoDetectUnicode(true);
        in.setLocale(QLocale::system());
        in.setCodec(QTextCodec::codecForLocale());

        QString statistics;
        FileData = FileFormats::ReadSMBX64SavFile( in.readAll(), file.fileName() );

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

            if(FileData.characterStates[i].itemID>0)
            {
              int item= configs.getNpcI(FileData.characterStates[i].itemID);
              if(item>=0)
              {
                statistics += QString("%2Has item: %1").arg(configs.main_npc[item].name)
                        .arg(FileData.characterStates[i].mountType>0?",    ":"");
              }
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
