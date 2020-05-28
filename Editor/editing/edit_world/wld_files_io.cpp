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

#include <QGraphicsItem>
#include <QPixmap>
#include <QGraphicsScene>
#include <QProgressDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QInputDialog>

#include <common_features/logger.h>
#include <common_features/util.h>
#include <common_features/main_window_ptr.h>
#include <common_features/file_keeper.h>
#include <editing/_scenes/world/wld_scene.h>
#include <editing/_dialogs/savingnotificationdialog.h>
#include <main_window/global_settings.h>
#include <audio/music_player.h>
#include <PGE_File_Formats/file_formats.h>
#include <data_functions/smbx64_validation_messages.h>

#include "world_edit.h"
#include <ui_world_edit.h>

bool WorldEdit::newFile(DataConfig &configs, EditingSettings options)
{
    static int sequenceNumber = 1;
    m_isUntitled = true;
    curFile = tr("Untitled %1").arg(sequenceNumber++);
    setWindowTitle(curFile);
    FileFormats::CreateWorldData(WldData);
    WldData.meta.modified = true;
    WldData.meta.untitled = true;

    switch(configs.editor.default_file_formats.world)
    {
    case EditorSetup::DefaultFileFormats::SMBX64:
        WldData.meta.RecentFormat = LevelData::SMBX64;
        WldData.meta.RecentFormatVersion = 64;
        WldData.meta.smbx64strict = true;
        break;
    case EditorSetup::DefaultFileFormats::PGEX:
        WldData.meta.RecentFormat = LevelData::PGEX;
        WldData.meta.smbx64strict = false;
        break;
    case EditorSetup::DefaultFileFormats::SMBX38A:
        // WldData.meta.RecentFormat = LevelData::SMBX38A;
        WldData.meta.RecentFormat = LevelData::PGEX;//TODO: Change to real 38A when PGE-FL will support write of WLD files
        WldData.meta.smbx64strict = false;
        break;
    }

    StartWldData = WldData;
    ui->graphicsView->setBackgroundBrush(QBrush(Qt::black));

    //Check if data configs exists
    if(configs.check())
    {
        LogCritical("Error! *.INI configs not loaded");
        this->close();
        return false;
    }

    scene = new WldScene(m_mw, ui->graphicsView, configs, WldData, this);
    scene->m_opts = options;
    //scene->InitSection(0);
    //scene->drawSpace();
    scene->buildAnimators();

    if(!sceneCreated)
    {
        ui->graphicsView->setScene(scene);
        sceneCreated = true;
        connect(scene, SIGNAL(screenshotSizeCaptured()), this, SLOT(ExportingReady()));
    }

    if(options.animationEnabled) scene->startAnimation();

    setAutoUpdateTimer(31);
    return true;
}

namespace wld_file_io
{
    static bool isSMBX64limit = false;
    static bool choiceVersionID = false;
}

bool WorldEdit::save(bool savOptionsDialog)
{
    if(m_isUntitled)
        return saveAs(savOptionsDialog);
    else
    {
        wld_file_io::choiceVersionID = false;
        return saveFile(curFile);
    }
}

bool WorldEdit::saveAs(bool savOptionsDialog)
{
    using namespace wld_file_io;
    bool makeCustomFolder = false;

    if(savOptionsDialog)
    {
        SavingNotificationDialog *sav = new SavingNotificationDialog(false, SavingNotificationDialog::D_QUESTION, this);
        sav->setSavingTitle(tr("Please enter a episode title for '%1'!").arg(userFriendlyCurrentFile()));
        sav->setWindowTitle(tr("Saving") + " " + userFriendlyCurrentFile());
        QLineEdit *wldNameBox = new QLineEdit();
        QCheckBox *mkDirCustom = new QCheckBox();
        sav->addUserItem(tr("Episode Title: "), wldNameBox);
        sav->addUserItem(tr("Make custom folder"), mkDirCustom);
        mkDirCustom->setToolTip(tr("Note: Custom folders are not supported for legacy SMBX Engine!"));
        wldNameBox->setText(WldData.EpisodeTitle);
        sav->adjustSize();
        util::DialogToCenter(sav, true);
        sav->fixSize();

        if(sav->exec() == QDialog::Accepted)
        {
            WldData.EpisodeTitle = wldNameBox->text();
            makeCustomFolder = mkDirCustom->isChecked();
            wldNameBox->deleteLater();
            mkDirCustom->deleteLater();
            sav->deleteLater();

            if(sav->savemode == SavingNotificationDialog::SAVE_CANCLE)
                return false;
        }
        else
            return false;
    }

    bool isNotDone = true;
    QString fileName = (m_isUntitled) ? GlobalSettings::savePath + QString("/") +
                       (WldData.EpisodeTitle.isEmpty() ? curFile : util::filePath(WldData.EpisodeTitle)) : curFile;
    QString fileSMBX64 = "SMBX64 (1.3) World map file (*.wld)";
    QString fileSMBXany = "SMBX0...64 Level file [choose version] (*.wld)";
    QString filePGEX = "Extended World map file (*.wldx)";
    QString selectedFilter;

    switch(WldData.meta.RecentFormat)
    {
    case LevelData::PGEX:
        selectedFilter = filePGEX;
        break;

    case LevelData::SMBX64:
        if(WldData.meta.RecentFormatVersion >= 64)
            selectedFilter = fileSMBX64;
        else
            selectedFilter = fileSMBXany;

        break;

    case LevelData::SMBX38A:
        // selectedFilter = fileSMBX38A;
        selectedFilter = filePGEX;//TODO: Put 38A target once PGE-FL gets support for SMBX-38A
        break;
    }

    QString filter =
        fileSMBX64 + ";;" +
        fileSMBXany + ";;" +
        filePGEX;
    bool ret;
RetrySave:
    isSMBX64limit = false;
    choiceVersionID = false;
    isNotDone = true;

    while(isNotDone)
    {
        fileName = QFileDialog::getSaveFileName(this, tr("Save As"), fileName, filter, &selectedFilter);

        if(fileName.isEmpty())
            return false;

        if(selectedFilter == fileSMBXany)
            choiceVersionID = true;

        if((selectedFilter == fileSMBXany) || (selectedFilter == fileSMBX64))
        {
            if(fileName.endsWith(".wldx", Qt::CaseInsensitive))
                fileName.remove(fileName.size() - 1, 1);

            if(!fileName.endsWith(".wld", Qt::CaseInsensitive))
                fileName.append(".wld");
        }
        else if(selectedFilter == filePGEX)
        {
            if(fileName.endsWith(".wld", Qt::CaseInsensitive))
                fileName.append("x");

            if(!fileName.endsWith(".wldx", Qt::CaseInsensitive))
                fileName.append(".wldx");
        }

        if((!fileName.endsWith(".wld", Qt::CaseInsensitive)) && (!fileName.endsWith(".wldx", Qt::CaseInsensitive)))
        {
            QMessageBox::warning(this, tr("Extension is not set"),
                                 tr("File Extension isn't defined, please enter file extension!"), QMessageBox::Ok);
            continue;
        }

        if(makeCustomFolder)
        {
            QFileInfo finfo(fileName);
            finfo.absoluteDir().mkpath(finfo.absoluteDir().absolutePath() +
                                       "/" + util::getBaseFilename(finfo.fileName())
                                      );
        }

        isNotDone = false;
    }

    ret = saveFile(fileName);

    if(isSMBX64limit) goto RetrySave;

    return ret;
    /*

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
        (isUntitled)?GlobalSettings::savePath+QString("/")+
                     (WldData.EpisodeTitle.isEmpty()?curFile:WldData.EpisodeTitle):curFile, QString("SMBX64 (1.3) World map file (*.wld)"));
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
    */
}

bool WorldEdit::saveFile(const QString &fileName, const bool addToRecent)
{
    using namespace wld_file_io;

    if((!fileName.endsWith(".wld", Qt::CaseInsensitive)) && (!fileName.endsWith(".wldx", Qt::CaseInsensitive)))
    {
        QMessageBox::warning(this, tr("Extension is not set"),
                             tr("File Extension isn't defined, please enter file extension!"), QMessageBox::Ok);
        return false;
    }

    WldData.meta.configPackId = getMainWindow()->configs.configPackId;

    QApplication::setOverrideCursor(Qt::WaitCursor);

    FileKeeper fileKeeper = FileKeeper(fileName);

    FileFormats::WorldPrepare(WldData); // Sort all data arrays

    // ////////////////////// Write SMBX64 WLD //////////////////////////////
    if(fileName.endsWith(".wld", Qt::CaseInsensitive))
    {
        //SMBX64 Standard check
        isSMBX64limit = false;
        int file_format = 64;

        if(choiceVersionID)
        {
            QApplication::restoreOverrideCursor();
            bool ok = true;
            file_format = QInputDialog::getInt(this, tr("SMBX file version"),
                                               tr("Which version do you want to save as? (from 0 to 64)"), 64, 0, 64, 1, &ok);

            if(!ok) return false;

            QApplication::setOverrideCursor(Qt::WaitCursor);
        }

        int ErrorCode = FileFormats::smbx64WorldCheckLimits(WldData);

        if(ErrorCode != FileFormats::SMBX64_FINE)
        {
            QMessageBox::warning(this,
                                 tr("The SMBX64 limit has been exceeded"),
                                 smbx64ErrMsgs(WldData, ErrorCode),
                                 QMessageBox::Ok);
            isSMBX64limit = true;
        }

        if(isSMBX64limit)
        {
            if(QMessageBox::question(this, tr("The SMBX64 limit has been exceeded"),
                                     tr("Do you want to save file anyway?\nExciting of SMBX64 limits may crash SMBX with 'overflow' error.\n\nInstalled LunaLUA partially extends than limits."), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
            {
                QApplication::restoreOverrideCursor();
                return false;
            }
            else
                isSMBX64limit = false;
        }

        if(!FileFormats::SaveWorldFile(WldData, fileKeeper.tempPath(), FileFormats::WLD_SMBX64, static_cast<unsigned int>(file_format)))
        {
            QMessageBox::warning(this, tr("File save error"),
                                 tr("Cannot save file %1:\n%2.")
                                 .arg(fileName)
                                 .arg(WldData.meta.ERROR_info));
            return false;
        }

        WldData.meta.smbx64strict = true; //Enable SMBX64 standard strict mode
        GlobalSettings::savePath = QFileInfo(fileName).path();
    }
    // //////////////////////////////////////////////////////////////////////
    // ////////////////// Write Extended WLD file (WLDX)/////////////////////
    else if(fileName.endsWith(".wldx", Qt::CaseInsensitive))
    {
        WldData.meta.smbx64strict = false; //Disable strict mode

        if(!FileFormats::SaveWorldFile(WldData, fileKeeper.tempPath(), FileFormats::WLD_PGEX))
        {
            QMessageBox::warning(this, tr("File save error"),
                                 tr("Cannot save file %1:\n%2.")
                                 .arg(fileName)
                                 .arg(WldData.meta.ERROR_info));
            return false;
        }

        GlobalSettings::savePath = QFileInfo(fileName).path();
    }

    // Swap old file with new
    fileKeeper.restore();

    // //////////////////////////////////////////////////////////////////////
    QApplication::restoreOverrideCursor();
    setCurrentFile(fileName);
    WldData.meta.modified = false;
    WldData.meta.untitled = false;

    if(addToRecent)
    {
        MainWinConnect::pMainWin->AddToRecentFiles(fileName);
        MainWinConnect::pMainWin->SyncRecentFiles();
    }

    return true;
}


bool WorldEdit::loadFile(const QString &fileName, WorldData FileData, DataConfig &configs, EditingSettings options)
{
    QFile file(fileName);
    WldData = FileData;
    bool modifystate = false;
    bool untitledstate = false;
    QString curFName = fileName;
    WldData.meta.modified = false;
    WldData.meta.untitled = false;

    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("File read error"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    //Restore internal information after restoring backups saved after crash
    if(WldData.metaData.crash.used)
    {
        modifystate                 = WldData.metaData.crash.modifyed;
        untitledstate               = WldData.metaData.crash.untitled;
        m_isUntitled                  = WldData.metaData.crash.untitled;
        WldData.meta.smbx64strict   = WldData.metaData.crash.strictModeSMBX64;
        WldData.meta.RecentFormat        = WldData.metaData.crash.fmtID;
        WldData.meta.RecentFormatVersion = WldData.metaData.crash.fmtVer;
        WldData.meta.filename            = WldData.metaData.crash.filename;
        WldData.meta.path                = WldData.metaData.crash.path;
        curFName                    = WldData.metaData.crash.fullPath;
        WldData.metaData.crash.reset();
    }

    setCurrentFile(curFName);
    StartWldData = WldData; //Save current history for made reset
    ui->graphicsView->setBackgroundBrush(QBrush(Qt::black));

    //Check if data configs exists
    if(configs.check())
    {
        LogCritical("Error! *.INI configs not loaded");
        this->close();
        return false;
    }

    LogDebug(QString(">>Starting to load file"));
    //Declaring of the scene
    scene = new WldScene(m_mw, ui->graphicsView, configs, WldData, this);
    scene->m_opts = options;
    int DataSize = 0;
    DataSize += 3;
    DataSize += 6;
    QProgressDialog progress(tr("Loading World map data"), tr("Abort"), 0, DataSize, MainWinConnect::pMainWin);
    progress.setWindowTitle(tr("Loading World map data"));
    progress.setWindowModality(Qt::WindowModal);
    progress.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
    progress.setFixedSize(progress.size());
    progress.setGeometry(util::alignToScreenCenter(progress.size()));
    progress.setMinimumDuration(0);
    progress.setAutoClose(false);

    if(!DrawObjects(progress))
    {
        WldData.meta.modified = false;
        this->close();
        return false;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    if(!progress.wasCanceled())
        progress.close();

    QApplication::restoreOverrideCursor();
    setAutoUpdateTimer(31);
    WldData.meta.modified = modifystate;
    WldData.meta.untitled = untitledstate;
    progress.deleteLater();

    return true;
}

void WorldEdit::showCustomStuffWarnings()
{
    if(m_mw->configs.checkCustom())
    {
        QString errorsList;
        for(QString &e : m_mw->configs.errorsList[DataConfig::ERR_CUSTOM])
            errorsList += " - " + e + "\n";
        QMessageBox::warning(m_mw,
                             tr("Incorrect custom configs"),
                             tr("This world map has some incorrect config files which are can't be loaded. "
                                "To avoid this message box in next time, please fix next errors in "
                                "your config files in the current and in the custom folders:"
                                "\n\n%1").arg(errorsList),
                             QMessageBox::Ok);
        m_mw->configs.errorsList[DataConfig::ERR_CUSTOM].clear();
    }
}


void WorldEdit::documentWasModified()
{
    WldData.meta.modified = true;
}

bool WorldEdit::maybeSave()
{
    if(WldData.meta.modified)
    {
        SavingNotificationDialog *sav = new SavingNotificationDialog(true, SavingNotificationDialog::D_WARN, this);
        sav->setSavingTitle(tr("'%1' has been modified.\n"
                               "Do you want to save your changes?").arg(userFriendlyCurrentFile()));
        sav->setWindowTitle(userFriendlyCurrentFile() + tr(" not saved"));
        QLineEdit *wldNameBox = new QLineEdit(sav);
        sav->addUserItem(tr("World title:"), wldNameBox);
        wldNameBox->setText(WldData.EpisodeTitle);
        sav->adjustSize();
        util::DialogToCenter(sav, true);
        sav->fixSize();

        if(sav->exec() == QDialog::Accepted)
        {
            WldData.EpisodeTitle = wldNameBox->text();
            wldNameBox->deleteLater();
            sav->deleteLater();

            if(sav->savemode == SavingNotificationDialog::SAVE_SAVE)
                return save();
            else if(sav->savemode == SavingNotificationDialog::SAVE_CANCLE)
                return false;
        }
        else
            return false;
    }

    return true;
}


//////// Common
QString WorldEdit::userFriendlyCurrentFile()
{
    return strippedName(curFile);
}

bool WorldEdit::trySave()
{
    return maybeSave();
}

bool WorldEdit::isUntitled()
{
    return m_isUntitled;
}

bool WorldEdit::isModified()
{
    return WldData.meta.modified;
}

void WorldEdit::markForForceClose()
{
    m_isUntitled = false;
    WldData.meta.modified = false;
}

void WorldEdit::closeEvent(QCloseEvent *event)
{
    if(!sceneCreated)
    {
        event->accept();
        return;
    }
    else
        MainWinConnect::pMainWin->on_actionSelect_triggered();

    if(maybeSave())
    {
        stopAutoUpdateTimer();
        scene->setMessageBoxItem(false);
        scene->clear();
        LogDebug("!<-Cleared->!");
        LogDebug("!<-Delete animators->!");

        while(! scene->m_animatorsTerrain.isEmpty())
        {
            SimpleAnimator *tmp = scene->m_animatorsTerrain.first();
            scene->m_animatorsTerrain.pop_front();

            if(tmp != NULL) delete tmp;
        }

        while(! scene->m_animatorsScenery.isEmpty())
        {
            SimpleAnimator *tmp = scene->m_animatorsScenery.first();
            scene->m_animatorsScenery.pop_front();

            if(tmp != NULL) delete tmp;
        }

        while(! scene->m_animatorsPaths.isEmpty())
        {
            SimpleAnimator *tmp = scene->m_animatorsPaths.first();
            scene->m_animatorsPaths.pop_front();

            if(tmp != NULL) delete tmp;
        }

        while(! scene->m_animatorsLevels.isEmpty())
        {
            SimpleAnimator *tmp = scene->m_animatorsLevels.first();
            scene->m_animatorsLevels.pop_front();

            if(tmp != NULL) delete tmp;
        }

        LogDebug("!<-Delete scene->!");
        sceneCreated = false;
        delete scene;
        scene = NULL;
        LogDebug("!<-Deleted->!");
        //ui->graphicsView->cl
        event->accept();
    }
    else
        event->ignore();
}

void WorldEdit::setCurrentFile(const QString &fileName)
{
    QFileInfo info(fileName);
    curFile = info.canonicalFilePath();
    m_isUntitled = false;
    WldData.meta.path = info.absoluteDir().absolutePath();
    WldData.meta.filename = util::getBaseFilename(info.fileName());
    WldData.meta.untitled = false;
    //document()->setModified(false);
    setWindowModified(false);
    setWindowTitle(QString(WldData.EpisodeTitle == "" ? userFriendlyCurrentFile() : WldData.EpisodeTitle).replace("&", "&&&"));
}

QString WorldEdit::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}
