/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QLineEdit>
#include <QCheckBox>
#include <QInputDialog>

#include <common_features/app_path.h>
#include <common_features/main_window_ptr.h>
#include <common_features/logger.h>
#include <common_features/util.h>
#include <common_features/file_keeper.h>
#include <main_window/global_settings.h>
#include <PGE_File_Formats/file_formats.h>
#include <data_functions/smbx64_validation_messages.h>
#include <audio/music_player.h>
#include <editing/_scenes/level/lvl_scene.h>
#include <editing/_dialogs/savingnotificationdialog.h>

#include "level_edit.h"
#include <ui_leveledit.h>

#include "qfile_dialogs_default_options.hpp"


bool LevelEdit::newFile(DataConfig &configs, EditingSettings options)
{
    static int sequenceNumber = 1;
    m_isUntitled = true;
    curFile = tr("Untitled %1").arg(sequenceNumber++);
    setWindowTitle(QString(curFile).replace("&", "&&&"));
    FileFormats::CreateLevelData(LvlData);
    LvlData.meta.untitled = true;

    switch(configs.editor.default_file_formats.level)
    {
    case EditorSetup::DefaultFileFormats::SMBX64:
        LvlData.meta.RecentFormat = LevelData::SMBX64;
        LvlData.meta.RecentFormatVersion = 64;
        LvlData.meta.smbx64strict = true;
        break;
    case EditorSetup::DefaultFileFormats::PGEX:
        LvlData.meta.RecentFormat = LevelData::PGEX;
        LvlData.meta.smbx64strict = false;
        break;
    case EditorSetup::DefaultFileFormats::SMBX38A:
        LvlData.meta.RecentFormat = LevelData::SMBX38A;
        LvlData.meta.smbx64strict = false;
        break;
    }

    StartLvlData = LvlData;
    ui->graphicsView->setBackgroundBrush(QBrush(Qt::darkGray));

    //Check if data configs exists
    if(configs.check())
    {
        LogCritical(QString("Error! *.INI configs not loaded"));
        this->close();
        return false;
    }

    scene = new LvlScene(m_mw, ui->graphicsView, configs, LvlData, this);
    scene->m_opts = options;
    scene->InitSection(0);
    scene->setPlayerPoints();
    scene->drawSpace();
    scene->buildAnimators();

    if(!sceneCreated)
    {
        ui->graphicsView->setScene(scene);
        sceneCreated = true;
        connect(scene, SIGNAL(screenshotSizeCaptured()), this, SLOT(ExportingReady()));
    }

    if(options.animationEnabled)
        scene->startAnimation();
    else
        stopAutoUpdateTimer();

    return true;
}

bool LevelEdit::save(bool savOptionsDialog)
{
    if(m_isUntitled)
        return saveAs(savOptionsDialog);
    else
        return saveFile(curFile);
}

bool LevelEdit::saveAs(bool savOptionsDialog)
{
    bool makeCustomFolder = false;

    if(savOptionsDialog)
    {
        SavingNotificationDialog *sav = new SavingNotificationDialog(false, SavingNotificationDialog::D_QUESTION, this);
        sav->setSavingTitle(tr("Please enter a level title for '%1'!").arg(userFriendlyCurrentFile()));
        sav->setWindowTitle(tr("Saving") + " " + userFriendlyCurrentFile());
        QLineEdit *lvlNameBox = new QLineEdit(sav);
        QCheckBox *mkDirCustom = new QCheckBox(sav);
        mkDirCustom->setText(QString(""));
        sav->addUserItem(tr("Level title: "), lvlNameBox);
        sav->addUserItem(tr("Make custom folder"), mkDirCustom);
        lvlNameBox->setText(LvlData.LevelName);
        sav->adjustSize();
        util::DialogToCenter(sav, true);
        sav->fixSize();

        if(sav->exec() == QDialog::Accepted)
        {
            LvlData.LevelName = lvlNameBox->text().trimmed();
            makeCustomFolder = mkDirCustom->isChecked();
            lvlNameBox->deleteLater();
            mkDirCustom->deleteLater();
            sav->deleteLater();

            if(sav->savemode == SavingNotificationDialog::SAVE_CANCLE)
                return false;
        }
        else
            return false;
    }

    bool isNotDone = true;
    bool isSMBX64limit = false;
    QString fileName = (m_isUntitled) ? GlobalSettings::savePath + QString("/") +
                       (LvlData.LevelName.isEmpty() ? curFile : util::filePath(LvlData.LevelName)) : curFile;
    QString fileSMBX64  = "SMBX64 (1.3) Level file (*.lvl)";
    QString fileSMBXany = "SMBX0...64 Level file [choose version] (*.lvl)";
    QString fileSMBX38A = "SMBX-38a Level file (*.lvl)";
    QString filePGEX    = "Extended Level file (*.lvlx)";
    QString selectedFilter;

    switch(LvlData.meta.RecentFormat)
    {
    case LevelData::PGEX:
        selectedFilter = filePGEX;
        break;

    case LevelData::SMBX64:
        if(LvlData.meta.RecentFormatVersion >= 64)
            selectedFilter = fileSMBX64;
        else
            selectedFilter = fileSMBXany;
        break;

    case LevelData::SMBX38A:
        selectedFilter = fileSMBX38A;
        break;
    }

    QString filter =
        fileSMBX64 + ";;" +
        fileSMBXany + ";;" +
        fileSMBX38A + ";;" +
        filePGEX;
    bool ret;
RetrySave:
    isSMBX64limit = false;
    isNotDone     = true;

    while(isNotDone)
    {
        fileName = QFileDialog::getSaveFileName(this, tr("Save As"), fileName, filter, &selectedFilter, c_fileDialogOptions);

        if(fileName.isEmpty())
            return false;

        if(selectedFilter == fileSMBXany)
        {
            unsigned int file_format = LvlData.meta.RecentFormatVersion;
            bool ok = true;
            file_format = static_cast<unsigned int>(
                              QInputDialog::getInt(this, tr("SMBX file version"),
                                                   tr("Which version do you want to save as? (from 0 to 64)\n"
                                                           "List of known SMBX versions and format codes:\n%1\n"
                                                           "(To allow level file work in specific SMBX version,\n"
                                                           "version code must be less or equal specific code)"
                                                     ).arg(" 1 - SMBX 1.0.0\n"
                                                             " 2 - SMBX 1.0.2\n"
                                                             "18 - SMBX 1.1.1\n"
                                                             "20 - SMBX 1.1.2\n"
                                                             "28 - SMBX 1.2.0 Beta 3\n"
                                                             "51 - SMBX 1.2.1\n"
                                                             "58 - SMBX 1.2.2\n"
                                                             "59 - SMBX 1.2.2 (with some patch)\n"
                                                             "64 - SMBX 1.3\n"),
                                                   static_cast<int>(LvlData.meta.RecentFormatVersion),
                                                   0, 64, 1, &ok)
                          );

            if(!ok)
                return false;

            LvlData.meta.RecentFormatVersion = file_format;
        }
        else
            LvlData.meta.RecentFormatVersion = 64;

        if(selectedFilter == filePGEX)
            LvlData.meta.RecentFormat = LevelData::PGEX;
        else if((selectedFilter == fileSMBX64) || (selectedFilter == fileSMBXany))
            LvlData.meta.RecentFormat = LevelData::SMBX64;
        else if((selectedFilter == fileSMBX38A))
        {
            QMessageBox::information(this, "====THIS FILE FORMAT IS EXPERIMENTAL====",
                                     "Saving into SMBX-38A Level file format is experimental!\n"
                                     "Some values are may be lost or distorted. Please also save same file into another file format until reload or close it!");
            LvlData.meta.RecentFormat = LevelData::SMBX38A;
        }

        if((selectedFilter == fileSMBXany) || (selectedFilter == fileSMBX64) || (selectedFilter == fileSMBX38A))
        {
            if(fileName.endsWith(".lvlx", Qt::CaseInsensitive))
                fileName.remove(fileName.size() - 1, 1);

            if(!fileName.endsWith(".lvl", Qt::CaseInsensitive))
                fileName.append(".lvl");
        }
        else if(selectedFilter == filePGEX)
        {
            if(fileName.endsWith(".lvl", Qt::CaseInsensitive))
                fileName.append("x");

            if(!fileName.endsWith(".lvlx", Qt::CaseInsensitive))
                fileName.append(".lvlx");
        }

        if((!fileName.endsWith(".lvl", Qt::CaseInsensitive)) && (!fileName.endsWith(".lvlx", Qt::CaseInsensitive)))
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

    ret = saveFile(fileName, true, &isSMBX64limit);

    if(isSMBX64limit) goto RetrySave;

    return ret;
}

bool LevelEdit::saveFile(const QString &fileName, const bool addToRecent, bool *out_WarningIsAborted)
{
    if((!fileName.endsWith(".lvl", Qt::CaseInsensitive)) && (!fileName.endsWith(".lvlx", Qt::CaseInsensitive)))
    {
        QMessageBox::warning(this, tr("Extension is not set"),
                             tr("File Extension isn't defined, please enter file extension!"), QMessageBox::Ok);
        return false;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    // Prepare data for saving
    prepareLevelFile(LvlData);

    // ////////////////// Write Extended LVL file (LVLX)/////////////////////
    if(LvlData.meta.RecentFormat == LevelData::PGEX)
    {
        if(!savePGEXLVL(fileName, false))
            return false;

        LvlData.meta.smbx64strict = false; //Disable strict mode
    }
    // //////////////////////////////////////////////////////////////////////
    // ////////////////////// Write SMBX64 LVL //////////////////////////////
    else if(LvlData.meta.RecentFormat == LevelData::SMBX64)
    {
        if(!saveSMBX64LVL(fileName, false, out_WarningIsAborted))
            return false;

        LvlData.meta.smbx64strict = true; //Enable SMBX64 standard strict mode
    }
    // //////////////////////////////////////////////////////////////////////
    // ////////////////////// Write SMBX-38A LVL //////////////////////////////
    else if(LvlData.meta.RecentFormat == LevelData::SMBX38A)
    {
        if(!saveSMBX38aLVL(fileName, false))
            return false;

        LvlData.meta.smbx64strict = false; //Disable strict mode
    }

    // //////////////////////////////////////////////////////////////////////
    QFileInfo finfo(fileName);
    GlobalSettings::savePath = finfo.path();
    LogDebug(QString("-------------------------------\n"
                     "Saving file name: %1").arg(fileName));
    LogDebug(QString("-------------------------------\n"
                     "Saving path: %1").arg(finfo.path()));
    LogDebug(QString("-------------------------------\n"
                     "Saving filename computed: %1").arg(finfo.fileName()));
    LvlData.meta.path = finfo.path();
    LvlData.meta.filename = util::getBaseFilename(finfo.fileName());
    QApplication::restoreOverrideCursor();
    setCurrentFile(fileName);
    LvlData.meta.modified = false;
    LvlData.meta.untitled = false;

    if(addToRecent)
    {
        m_mw->AddToRecentFiles(fileName);
        m_mw->SyncRecentFiles();
    }

    //Refresh Strict SMBX64 flag
    emit m_mw->setSMBX64Strict(LvlData.meta.smbx64strict);
    return true;
}

bool LevelEdit::savePGEXLVL(QString fileName, bool silent)
{
    FileKeeper fileKeeper = FileKeeper(fileName);

    if(!FileFormats::SaveLevelFile(LvlData, fileKeeper.tempPath(), FileFormats::LVL_PGEX))
    {
        if(!silent)
            QMessageBox::warning(this, tr("File save error"),
                                 tr("Cannot save file %1:\n%2.")
                                 .arg(fileName)
                                 .arg(LvlData.meta.ERROR_info));

        return false;
    }

    // Swap old file with new
    fileKeeper.restore();

    return true;
}


bool LevelEdit::saveSMBX38aLVL(QString fileName, bool silent)
{
    FileKeeper fileKeeper = FileKeeper(fileName);

    if(!FileFormats::SaveLevelFile(LvlData, fileKeeper.tempPath(), FileFormats::LVL_SMBX38A))
    {
        if(!silent)
            QMessageBox::warning(this, tr("File save error"),
                                 tr("Cannot save file %1:\n%2.")
                                 .arg(fileName)
                                 .arg(LvlData.meta.ERROR_info));

        return false;
    }

    // Swap old file with new
    fileKeeper.restore();

    return true;
}



bool LevelEdit::saveSMBX64LVL(QString fileName, bool silent, bool *out_WarningIsAborted)
{
    FileKeeper fileKeeper = FileKeeper(fileName);

    //SMBX64 Standard check
    bool isSMBX64limit = false;

    if(out_WarningIsAborted)
        *out_WarningIsAborted = false;

    int ErrorCode = FileFormats::smbx64LevelCheckLimits(LvlData);

    if(ErrorCode != FileFormats::SMBX64_FINE)
    {
        if(!silent)
            QMessageBox::warning(this,
                                 tr("The SMBX64 limit has been exceeded"),
                                 smbx64ErrMsgs(LvlData, ErrorCode),
                                 QMessageBox::Ok);

        isSMBX64limit = true;
    }

    if(isSMBX64limit)
    {
        if(!silent)
        {
            if(!silent)
                QApplication::restoreOverrideCursor();

            if(QMessageBox::question(this,
                                     tr("The SMBX64 limit has been exceeded"),
                                     tr("Do you want to save file anyway?\nExciting of SMBX64 limits may crash SMBX with 'Subscript out of range' error.\n\nInstalled LunaLUA partially extends than limits."),
                                     QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
            {
                if(out_WarningIsAborted)
                    *out_WarningIsAborted = isSMBX64limit;

                return false;
            }
            else
                isSMBX64limit = false;

            QApplication::setOverrideCursor(Qt::WaitCursor);
        }
    }

    if(!FileFormats::SaveLevelFile(LvlData, fileKeeper.tempPath(), FileFormats::LVL_SMBX64, LvlData.meta.RecentFormatVersion))
    {
        QApplication::restoreOverrideCursor();

        if(!silent)
            QMessageBox::warning(this, tr("File save error"),
                                 tr("Cannot save file %1:\n%2.")
                                 .arg(fileName)
                                 .arg(LvlData.meta.ERROR_info));

        return false;
    }

    // Swap old file with new
    fileKeeper.restore();

    QApplication::restoreOverrideCursor();
    return true;
}



bool LevelEdit::loadFile(const QString &fileName, LevelData &FileData, DataConfig &configs, EditingSettings options)
{
    QFile file(fileName);
    LvlData = FileData;
    bool modifystate = false;
    bool untitledstate = false;
    LvlData.meta.modified = false;
    LvlData.meta.untitled = false;
    QString curFName = fileName;

    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::warning(this, tr("File read error"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    StartLvlData = LvlData; //Save current history for made reset

    //Restore internal information after restoring backups saved after crash
    if(LvlData.metaData.crash.used)
    {
        modifystate                 = LvlData.metaData.crash.modifyed;
        untitledstate               = LvlData.metaData.crash.untitled;
        LvlData.meta.RecentFormat        = LvlData.metaData.crash.fmtID;
        LvlData.meta.RecentFormatVersion = LvlData.metaData.crash.fmtVer;
        m_isUntitled                  = LvlData.metaData.crash.untitled;
        LvlData.meta.smbx64strict   = LvlData.metaData.crash.strictModeSMBX64;
        LvlData.meta.filename            = LvlData.metaData.crash.filename;
        LvlData.meta.path                = LvlData.metaData.crash.path;
        curFName                    = LvlData.metaData.crash.fullPath;
        LvlData.metaData.crash.reset();
    }

    setCurrentFile(curFName);
    ui->graphicsView->setBackgroundBrush(QBrush(Qt::darkGray));

    //Check if data configs exists
    if(configs.check())
    {
        LogCritical(QString("Error! *.INI configs not loaded"));
        this->close();
        return false;
    }

    LogDebug(QString(">>Starting to load file"));
    //Declaring of the scene
    scene = new LvlScene(m_mw, ui->graphicsView, configs, LvlData, this);
    scene->m_opts = options;
    int DataSize = 0;
    DataSize += 3;
    DataSize += 6; /*LvlData.sections.size()*2;
    DataSize += LvlData.bgo.size();
    DataSize += LvlData.blocks.size();
    DataSize += LvlData.npc.size();
    DataSize += LvlData.water.size();
    DataSize += LvlData.doors.size();*/
    QProgressDialog progress(tr("Loading level data"), tr("Abort"), 0, DataSize, m_mw);
    progress.setWindowTitle(tr("Loading level data"));
    progress.setWindowModality(Qt::WindowModal);
    progress.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
    progress.setFixedSize(progress.size());
    progress.setGeometry(util::alignToScreenCenter(progress.size()));
    progress.setMinimumDuration(0);
    progress.setAutoClose(false);

    if(! DrawObjects(progress))
    {
        LvlData.meta.modified = false;
        this->close();
        return false;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    if(!progress.wasCanceled())
        progress.close();

    QApplication::restoreOverrideCursor();

    if(options.animationEnabled)
        scene->startAnimation();
    else
        stopAutoUpdateTimer();

    setCurrentFile(curFName);
    LvlData.meta.modified = modifystate;
    LvlData.meta.untitled = untitledstate;
    progress.deleteLater();

    return true;
}

void LevelEdit::showCustomStuffWarnings()
{
    if(m_mw->configs.checkCustom())
    {
        QString errorsList;
        for(QString &e : m_mw->configs.errorsList[DataConfig::ERR_CUSTOM])
            errorsList += " - " + e + "\n";
        QMessageBox::warning(m_mw,
                             tr("Incorrect custom configs"),
                             tr("This level has some incorrect config files which are can't be loaded. "
                                "To avoid this message box in next time, please fix next errors in "
                                "your config files in the the current and in the custom folders:"
                                "\n\n%1").arg(errorsList),
                             QMessageBox::Ok);
        m_mw->configs.errorsList[DataConfig::ERR_CUSTOM].clear();
    }
}



void LevelEdit::documentWasModified()
{
    //LvlData.modified = true;
}

bool LevelEdit::maybeSave()
{
    if(LvlData.meta.modified)
    {
        SavingNotificationDialog *sav = new SavingNotificationDialog(true, SavingNotificationDialog::D_WARN, this);
        sav->setSavingTitle(tr("'%1' has been modified.\n"
                               "Do you want to save your changes?").arg(userFriendlyCurrentFile()));
        sav->setWindowTitle(userFriendlyCurrentFile() + tr(" not saved"));
        QLineEdit *lvlNameBox = new QLineEdit();
        sav->addUserItem(tr("Level title: "), lvlNameBox);
        lvlNameBox->setText(LvlData.LevelName);
        sav->adjustSize();
        util::DialogToCenter(sav, true);
        sav->fixSize();

        if(sav->exec() == QDialog::Accepted)
        {
            LvlData.LevelName = lvlNameBox->text().trimmed();
            lvlNameBox->deleteLater();
            sav->deleteLater();

            if(sav->savemode == SavingNotificationDialog::SAVE_SAVE)
                return save(false);
            else if(sav->savemode == SavingNotificationDialog::SAVE_CANCLE)
                return false;
        }
        else
            return false;
    }

    return true;
}


//////// Common
QString LevelEdit::userFriendlyCurrentFile()
{
    return strippedName(curFile);
}

bool LevelEdit::trySave()
{
    return maybeSave();
}

bool LevelEdit::isUntitled()
{
    return m_isUntitled;
}

bool LevelEdit::isModified()
{
    return LvlData.meta.modified;
}

void LevelEdit::markForForceClose()
{
    m_isUntitled = false;
    LvlData.meta.modified = false;
}

void LevelEdit::closeEvent(QCloseEvent *event)
{
    if(!sceneCreated)
    {
        event->accept();
        goto clearScene;
    }
    else
        m_mw->on_actionSelect_triggered();

    if(maybeSave())
        goto clearScene;
    else
        event->ignore();

    return;
clearScene:

    if(scene)
    {
        stopAutoUpdateTimer();
        scene->setLabelBoxItem(false);
        scene->clear();
        LogDebug("!<-Cleared->!");
        LogDebug("!<-Delete animators->!");

        while(! scene->m_animatorsBGO.isEmpty())
        {
            SimpleAnimator *tmp = scene->m_animatorsBGO.first();
            scene->m_animatorsBGO.pop_front();

            if(tmp != nullptr) delete tmp;
        }

        while(! scene->m_animatorsBlocks.isEmpty())
        {
            SimpleAnimator *tmp = scene->m_animatorsBlocks.first();
            scene->m_animatorsBlocks.pop_front();

            if(tmp != nullptr) delete tmp;
        }

        while(! scene->m_animatorsNPC.isEmpty())
        {
            AdvNpcAnimator *tmp = scene->m_animatorsNPC.first();
            scene->m_animatorsNPC.pop_front();

            if(tmp != nullptr) delete tmp;
        }

        scene->m_localConfigBGOs.clear();
        scene->m_localConfigBackgrounds.clear();
        scene->m_localConfigBlocks.clear();
        scene->m_localConfigNPCs.clear();
        LogDebug("!<-Delete scene->!");
        sceneCreated = false;
        delete scene;
        scene = nullptr;
        LogDebug("!<-Deleted->!");
    }
}

void LevelEdit::doSave()
{
    save();
}

void LevelEdit::setCurrentFile(const QString &fileName)
{
    QFileInfo info(fileName);
    curFile = info.canonicalFilePath();
    m_isUntitled = false;
    LvlData.meta.path = info.absoluteDir().absolutePath();
    LvlData.meta.filename = util::getBaseFilename(info.fileName());
    LvlData.meta.untitled = false;
    //document()->setModified(false);
    setWindowModified(false);
    setWindowTitle(QString(LvlData.LevelName == "" ? userFriendlyCurrentFile() : LvlData.LevelName).replace("&", "&&&"));
}

QString LevelEdit::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}
