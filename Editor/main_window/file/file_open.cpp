/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QDesktopServices>
#include <QMdiSubWindow>

#include <PGE_File_Formats/file_formats.h>
#include <audio/music_player.h>
#include <main_window/global_settings.h>
#include <main_window/dock/lvl_warp_props.h>
#include <main_window/dock/lvl_sctc_props.h>
#include <main_window/dock/lvl_layers_box.h>
#include <main_window/dock/lvl_item_toolbox.h>
#include <main_window/dock/wld_item_toolbox.h>
#include <main_window/dock/wld_settings_box.h>
#include <common_features/bool_reseter.h>
#include <common_features/util.h>
#include <Utils/dir_list_ci_qt.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>

#include "qfile_dialogs_default_options.hpp"


void MainWindow::openFilesByArgs(QStringList args, int startAt)
{
    qDebug() << "Attempt to open files by arguments:" << args;
    for(int i = startAt; i < args.size(); i++)
    {
        if(QFile::exists(args[i]))
            OpenFile(args[i]);
    }
}

void MainWindow::on_action_openEpisodeFolder_triggered()
{
    QString path;
    bool isUntitled = false;
    if(activeChildWindow() == WND_Level)
    {
        LevelEdit *e = activeLvlEditWin();
        if(e)
        {
            path = e->LvlData.meta.path;
            isUntitled = e->isUntitled();
        }
    }
    else if(activeChildWindow() == WND_NpcTxt)
    {
        NpcEdit *e = activeNpcEditWin();
        if(e)
        {
            path = QFileInfo(e->curFile).absoluteDir().absolutePath();
            isUntitled = e->isUntitled();
        }
    }
    else if(activeChildWindow() == WND_World)
    {
        WorldEdit *e = activeWldEditWin();
        if(e)
        {
            path = e->WldData.meta.path;
            isUntitled = e->isUntitled();
        }
    }

    if(isUntitled)
    {
        QMessageBox::warning(this, tr("Untitled file"), tr("Please save file to the disk first."));
        return;
    }

    if(!path.isEmpty())
    {
        QDesktopServices::openUrl(QUrl("file:///" + path));
    }
}

void MainWindow::on_action_openCustomFolder_triggered()
{
    QString path;
    bool isUntitled = false;
    DirListCIQt ci;

    if(activeChildWindow() == WND_Level)
    {
        LevelEdit *e = activeLvlEditWin();
        if(e)
        {
            ci.setCurDir(e->LvlData.meta.path);
            path = e->LvlData.meta.path + "/" + ci.resolveDirCase(e->LvlData.meta.filename);
            isUntitled = e->isUntitled();
        }
    }
    else if(activeChildWindow() == WND_NpcTxt)
    {
        return;
    }
    else if(activeChildWindow() == WND_World)
    {
        WorldEdit *e = activeWldEditWin();
        if(e)
        {
            ci.setCurDir(e->WldData.meta.path);
            path = e->WldData.meta.path + "/" + ci.resolveDirCase(e->WldData.meta.filename);
            isUntitled = e->isUntitled();
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
        QDesktopServices::openUrl(QUrl("file:///" + path));
    }
}

void MainWindow::on_OpenFile_triggered()
{
    //Check if data configs are valid
    if(configs.check())
    {
        LogCritical(QString("Error! *.INI configs not loaded"));
        QMessageBox::warning(this, tr("Configuration is loaded with errors"),
                             tr("Editor cannot open files:\nConfiguration package is loaded with errors.").arg(ConfStatus::configPath));
        //Show configuration status window
        on_actionCurConfig_triggered();
        return;
    }

    QStringList fileName_DATA = QFileDialog::getOpenFileNames(this,
                                tr("Open file"),
                                GlobalSettings::openPath,
                                QString("All supported formats (*.lvlx *.wldx *.lvl *.wld npc-*.txt *.sav);;"
                                        "All SMBX files (*.lvl *.wld npc-*.txt);;"
                                        "All PGE files (*.lvlx *.wldx npc-*.txt);;"
                                        "SMBX Level (*.lvl);;"
                                        "PGE Level (*.lvlx);;"
                                        "SMBX World (*.wld);;"
                                        "PGE World (*.wldx);;"
                                        "SMBX NPC Config (npc-*.txt);;"
                                        "SMBX Game Save file (*.sav);;"
                                        "All Files (*.*)"), 0, c_fileDialogOptions);

    if(fileName_DATA.isEmpty())
        return;

    for(const QString &file : fileName_DATA)
        OpenFile(file, true);
}


static QString getEngineName(DataConfig *config)
{
    if(config && !config->targetEngineName.isEmpty())
        return config->targetEngineName;

    switch(ConfStatus::defaultTestEngine)
    {
    default:
    case ConfStatus::ENGINE_PGE:
        return "PGE Engine";

    case ConfStatus::ENGINE_LUNA:
        return "LunaLua-SMBX";

    case ConfStatus::ENGINE_THEXTECH:
        return "SMBX/TheXTech";

    case ConfStatus::ENGINE_38A:
        return "SMBX-38A";
    }
}

static void verifyCompatibilityLevel(MainWindow *mw,
                                     const QString &path,
                                     const QString &engineName,
                                     const QString &fileId,
                                     const QString &configPackId)
{
    if(fileId.isEmpty() || configPackId.isEmpty())
        return; // Nothing to check
    if(fileId == configPackId)
        return; // File is compatible
    QMessageBox::warning(mw,
                         MainWindow::tr("Level is incompatible"),
                         MainWindow::tr("This level file was created in an editor that was using an unrecognized config pack. This most likely means this "
                                        "level was designed to be used with a different engine rather than %1. It is likely that some "
                                        "blocks/NPCs/scripts/etc will not be compatible, and may cause unexpected gameplay results or errors."
                                        "\n\n"
                                        "Filename: %2\n"
                                        "Level's config pack ID: %3\n"
                                        "Expected config pack ID: %4")
                         .arg(engineName)
                         .arg(path)
                         .arg(fileId)
                         .arg(configPackId)
    );
}

static void verifyCompatibilityWorld(MainWindow *mw,
                                     const QString &path,
                                     const QString &engineName,
                                     const QString &fileId,
                                     const QString &configPackId)
{
    if(fileId.isEmpty() || configPackId.isEmpty())
        return; // Nothing to check
    if(fileId == configPackId)
        return; // File is compatible
    QMessageBox::warning(mw,
                         MainWindow::tr("World map is incompatible"),
                         MainWindow::tr("This world map file was created in an editor that was using an unrecognized config pack. This most likely means this "
                                        "world map was designed to be used with a different engine rather than %1. It is likely that some "
                                        "terrain/levels/scripts/etc will not be compatible, and may cause unexpected gameplay results or errors."
                                        "\n\n"
                                        "Filename: %2\n"
                                        "Level's config pack ID: %3\n"
                                        "Expected config pack ID: %4")
                         .arg(engineName)
                         .arg(path)
                         .arg(fileId)
                         .arg(configPackId)
    );
}

void MainWindow::OpenFile(QString FilePath, bool addToRecentList)
{
    if(m_isFileReloading) return;
    m_isFileReloading = true;
    BoolReseter rst(&m_isFileReloading);
    Q_UNUSED(rst);

    if(!m_isAppInited) return;
    qApp->setActiveWindow(this);

    //Check if data configs are valid
    if(configs.check())
    {
        LogCritical(QString("Error! *.INI configs not loaded"));
        QMessageBox::warning(this,
                             tr("Configuration is loaded with errors"),
                             tr("Cannot open file:\n"
                                "The configuration pack was loaded, but contains errors.")
                             .arg(ConfStatus::configPath));
        //Show configuration status window
        on_actionCurConfig_triggered();
        return;
    }

    QMdiSubWindow *newSubWin = nullptr;
    QMdiSubWindow *existing = findOpenedFileWin(FilePath);
    if(existing)
    {
        ui->centralWidget->setActiveSubWindow(existing);
        return;
    }

    QFile file(FilePath);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this, tr("File open error"),
                              tr("Can't open the file: %1").arg(file.errorString()), QMessageBox::Ok);
        return;
    }

    QFileInfo in_1(FilePath);

    GlobalSettings::openPath = in_1.absoluteDir().absolutePath();

    if((in_1.suffix().toLower() == "lvl") || (in_1.suffix().toLower() == "lvlx"))
    {
        LevelData FileData;

        LogDebug("> parsing level file format");
        if(!FileFormats::OpenLevelFile(FilePath, FileData))
        {
            formatErrorMsgBox(FilePath, FileData.meta.ERROR_info, FileData.meta.ERROR_linenum, FileData.meta.ERROR_linedata);
            return;
        }
        LogDebug("File was read!");
        FileData.meta.filename   = util::getBaseFilename(in_1.fileName());
        FileData.meta.path       = in_1.absoluteDir().absolutePath();
        FileData.playmusic  = GlobalSettings::autoPlayMusic;
        file.close();

        verifyCompatibilityLevel(this, in_1.fileName(),
                                 getEngineName(&configs),
                                 FileData.meta.configPackId, configs.configPackId);

        LogDebug("Creating of sub-window");
        LevelEdit *child = createLvlChild(&newSubWin);
        if(child->loadFile(FilePath, FileData, configs, GlobalSettings::LvlOpts))
        {
            child->show();
            updateMenus(newSubWin, true);
            child->updateGeometry();
            child->resetPosition();
            dock_LvlItemBox->initItemLists();
            statusBar()->showMessage(tr("Level file loaded"), 2000);
            setCurrentLevelSection(0);
            dock_LvlWarpProps->init();
            dock_LvlLayers->setLayersBox();

            if(GlobalSettings::autoPlayMusic) ui->actionPlayMusic->setChecked(true);
            on_actionPlayMusic_triggered(ui->actionPlayMusic->isChecked());
            child->showCustomStuffWarnings();
        }
        else
        {
            LogDebug(">>File loading aborted");
            //child->show();
            child->LvlData.meta.modified = false;
            newSubWin->close();
            LogDebug(">>Windows closed");
        }
    }
    else if((in_1.suffix().toLower() == "wld") || (in_1.suffix().toLower() == "wldx"))
    {
        WorldData FileData;

        LogDebug("> parsing world map file format");
        if(!FileFormats::OpenWorldFile(FilePath, FileData))
        {
            formatErrorMsgBox(FilePath, FileData.meta.ERROR_info, FileData.meta.ERROR_linenum, FileData.meta.ERROR_linedata);
            return;
        }
        LogDebug("File was read!");
        file.close();

        verifyCompatibilityWorld(this, in_1.fileName(),
                                 getEngineName(&configs),
                                 FileData.meta.configPackId, configs.configPackId);

        LogDebug("Creating of sub-window");
        WorldEdit *child = createWldChild(&newSubWin);
        if(child->loadFile(FilePath, FileData, configs, GlobalSettings::LvlOpts))
        {
            child->show();
            updateMenus(newSubWin, true);
            child->updateGeometry();
            child->ResetPosition();
            dock_WldItemBox->initItemLists();
            dock_WldSettingsBox->setCurrentWorldSettings();
            if(FileData.HubStyledWorld)
            {
                dock_WldSettingsBox->setVisible(true);
                dock_WldSettingsBox->raise();
            }
            statusBar()->showMessage(tr("World map file loaded"), 2000);
            child->showCustomStuffWarnings();
        }
        else
        {
            LogDebug(">>File loading aborted");
            child->close();
            newSubWin->close();
            LogDebug(">>Windows closed");
        }
    }
    else if(in_1.suffix().toLower() == "txt")
    {
        NPCConfigFile FileData;

        if(!FileFormats::ReadNpcTXTFileF(FilePath, FileData))
        {
            QMessageBox::critical(this, QObject::tr("File open error"), tr("Can't read the file"), QMessageBox::Ok);
            return;
        }

        if(!FileData.unknownLines.isEmpty())
        {
            QMessageBox::warning(this,
                                 QObject::tr("Unknown values are presented"),
                                 QObject::tr("Your file have an unknown values which will be removed\n"
                                             " when you will save file") +
                                 QString("\n====================================\n"
                                         "%1").arg(FileData.unknownLines),
                                 QMessageBox::Ok);
        }

        NpcEdit *child = createNPCChild(&newSubWin);
        if(child->loadFile(FilePath, FileData))
        {
            statusBar()->showMessage(tr("NPC Config loaded"), 2000);
            child->show();
            updateMenus(newSubWin, true);
        }
        else
        {
            child->close();
            newSubWin->close();
        }
    }
    else if(in_1.suffix().toLower() == "sav")
    {
        GamesaveData FileData;

        QString statistics;
        if(!FileFormats::ReadSMBX64SavFileF(FilePath, FileData))
        {
            formatErrorMsgBox(FilePath, FileData.meta.ERROR_info, FileData.meta.ERROR_linenum, FileData.meta.ERROR_linedata);
            return;
        }

        statistics += QString("SMBX Game Save file version %1\n\n").arg(FileData.meta.RecentFormatVersion);
        if(FileData.gameCompleted)
            statistics += "      ====This game was completed====\n\n";
        statistics += QString("Lives: %1,   Coins:%2,   ").arg(FileData.lives).arg(FileData.coins);
        statistics += QString("Total stars: %1,   Gotten stars: %2\n")
                      .arg(FileData.totalStars)
                      .arg(FileData.gottenStars.size());
        statistics += QString("Position X=%1, Y=%2\n")
                      .arg(FileData.worldPosX)
                      .arg(FileData.worldPosY);

        if(FileData.musicID > 0)
        {
            long item = configs.getMusWldI(FileData.musicID);
            if(item >= 0)
            {
                statistics += QString("Current music: %1\n\n").arg(configs.main_music_wld[item].name);
            }
        }

        statistics += "\n===========Players:============\n";
        for(int i = 0; i < FileData.characterStates.size(); i++)
        {
            if(i < configs.main_characters.size())
                statistics += QString("%1:\n").arg(configs.main_characters[i].name);
            else
                statistics += QString("<unknown character>:\n");
            statistics += QString("Health: %1,    ").arg(FileData.characterStates[i].health);
            statistics += QString("Power-UP: %1,    ").arg(FileData.characterStates[i].state);
            switch(FileData.characterStates[i].mountType)
            {
            case 0:
                break;
            case 1:
                statistics += QString("Mounted vehicle: Shoe - ");
                switch(FileData.characterStates[i].mountID)
                {
                case 1:
                    statistics += QString("Kuribo's (green)");
                    break;
                case 2:
                    statistics += QString("Podoboo's (red)");
                    break;
                case 3:
                    statistics += QString("Lakitu's (blue)");
                    break;
                default:
                    statistics += QString("of God Power :D");
                    break;
                }
                break;
            case 3:
                statistics += QString("Mounted vehicle: Yoshi");
                break;
            default:
                statistics += QString("Mounted vehicle: <unknown>");
                break;
            }

            if((FileData.characterStates[i].itemID > 0) && configs.main_npc.contains(FileData.characterStates[i].itemID))
            {
                statistics += QString("%2Has item: %1")
                              .arg(configs.main_npc[FileData.characterStates[i].itemID].setup.name)
                              .arg(FileData.characterStates[i].mountType > 0 ? ",    " : "");
            }

            if(i < FileData.characterStates.size() - 1)
                statistics += "\n----------------------------\n";
        }
        statistics += "\n=========================\n";

        if(!FileData.meta.ReadFileValid)
            return;

        QMessageBox::information(this,
                                 tr("Game save statistics"),
                                 statistics,
                                 QMessageBox::Ok);
    }
    else
    {
        QMessageBox::warning(this,
                             tr("Bad file"),
                             tr("This file have unknown extension"),
                             QMessageBox::Ok);
        return;
    }

    // Add to recent fileList
    if(addToRecentList)
    {
        AddToRecentFiles(FilePath);
        SyncRecentFiles();
    }
}
