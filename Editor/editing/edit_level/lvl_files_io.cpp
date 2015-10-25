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

#include <QtWidgets>

#include <common_features/app_path.h>
#include <common_features/mainwinconnect.h>
#include <common_features/logger.h>
#include <common_features/util.h>
#include <script/scriptholder.h>
#include <main_window/global_settings.h>
#include <PGE_File_Formats/file_formats.h>
#include <data_functions/smbx64_validation_messages.h>
#include <audio/music_player.h>
#include <editing/_scenes/level/lvl_scene.h>
#include <editing/_dialogs/savingnotificationdialog.h>

#include "level_edit.h"
#include <ui_leveledit.h>

bool LevelEdit::newFile(dataconfigs &configs, LevelEditingSettings options)
{
    static int sequenceNumber = 1;

    isUntitled = true;
    curFile = tr("Untitled %1").arg(sequenceNumber++);
    setWindowTitle(QString(curFile).replace("&", "&&&"));
    LvlData = FileFormats::CreateLevelData();
    LvlData.metaData.script = new ScriptHolder;
    LvlData.untitled = true;
    StartLvlData = LvlData;

    ui->graphicsView->setBackgroundBrush(QBrush(Qt::darkGray));

    //Check if data configs exists
    if( configs.check() )
    {
        WriteToLog(QtCriticalMsg, QString("Error! *.INI configs not loaded"));
        this->close();
        return false;
    }

    scene = new LvlScene(ui->graphicsView, configs, LvlData, this);
    scene->opts = options;

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

    if(options.animationEnabled) scene->startAnimation();
    setAutoUpdateTimer(31);
    return true;
}

namespace lvl_file_io
{
    bool isSMBX64limit=false;
    bool choiceVersionID=false;
}

bool LevelEdit::save(bool savOptionsDialog)
{
    if (isUntitled) {
        return saveAs(savOptionsDialog);
    } else {
        lvl_file_io::choiceVersionID=false;
        return saveFile(curFile);
    }
}

bool LevelEdit::saveAs(bool savOptionsDialog)
{
    using namespace lvl_file_io;
    bool makeCustomFolder = false;

    if(savOptionsDialog)
    {
        SavingNotificationDialog* sav = new SavingNotificationDialog(false, SavingNotificationDialog::D_QUESTION, this);
        util::DialogToCenter(sav, true);
        sav->setSavingTitle(tr("Please enter a level title for '%1'!").arg(userFriendlyCurrentFile()));
        sav->setWindowTitle(tr("Saving") + " " + userFriendlyCurrentFile());
        QLineEdit* lvlNameBox = new QLineEdit();
        QCheckBox* mkDirCustom = new QCheckBox();
        mkDirCustom->setText(QString(""));
        sav->addUserItem(tr("Level title: "),lvlNameBox);
        sav->addUserItem(tr("Make custom folder"), mkDirCustom);
        sav->setAdjustSize(400,120);
        lvlNameBox->setText(LvlData.LevelName);
        if(sav->exec() == QDialog::Accepted){
            LvlData.LevelName = lvlNameBox->text();
            makeCustomFolder = mkDirCustom->isChecked();
            lvlNameBox->deleteLater();
            mkDirCustom->deleteLater();
            sav->deleteLater();
            if(sav->savemode == SavingNotificationDialog::SAVE_CANCLE){
                return false;
            }
        }else{
            return false;
        }
    }

    bool isNotDone=true;
    QString fileName = (isUntitled)?GlobalSettings::savePath+QString("/")+
                                    (LvlData.LevelName.isEmpty()?curFile:util::filePath(LvlData.LevelName)):curFile;

    QString fileSMBX64="SMBX64 (1.3) Level file (*.lvl)";
    QString fileSMBXany="SMBX0...64 Level file (*.lvl) [choose version]";
    QString filePGEX="Extended Level file (*.lvlx)";

    QString selectedFilter;
    if(fileName.endsWith(".lvlx", Qt::CaseInsensitive))
        selectedFilter = filePGEX;
    else
        selectedFilter = fileSMBX64;

    QString filter =
            fileSMBX64+";;"+
            fileSMBXany+";;"+
            filePGEX;

    bool ret;

    RetrySave:

    isSMBX64limit=false;
    choiceVersionID=false;
    isNotDone=true;
    while(isNotDone)
    {
        fileName = QFileDialog::getSaveFileName(this, tr("Save As"), fileName, filter, &selectedFilter);

        if (fileName.isEmpty())
            return false;

        if(selectedFilter==fileSMBXany)
            choiceVersionID=true;

        if((selectedFilter==fileSMBXany)||(selectedFilter==fileSMBX64))
        {
            if(fileName.endsWith(".lvlx", Qt::CaseInsensitive))
                fileName.remove(fileName.size()-1, 1);
            if(!fileName.endsWith(".lvl", Qt::CaseInsensitive))
                fileName.append(".lvl");
        }
        else if(selectedFilter==filePGEX)
        {
            if(fileName.endsWith(".lvl", Qt::CaseInsensitive))
                fileName.append("x");
            if(!fileName.endsWith(".lvlx", Qt::CaseInsensitive))
                fileName.append(".lvlx");
        }

        if( (!fileName.endsWith(".lvl", Qt::CaseInsensitive)) && (!fileName.endsWith(".lvlx", Qt::CaseInsensitive)) )
        {
            QMessageBox::warning(this, tr("Extension is not set"),
               tr("File Extension isn't defined, please enter file extension!"), QMessageBox::Ok);
            continue;
        }

        if(makeCustomFolder)
        {
            QDir dir = fileName.section("/",0,-2);
            dir.mkdir(fileName.section("/",-1,-1).section(".",0,0));
        }
        isNotDone=false;
    }

    ret = saveFile(fileName);
    if(isSMBX64limit) goto RetrySave;

    return ret;
}

bool LevelEdit::saveFile(const QString &fileName, const bool addToRecent)
{
    using namespace lvl_file_io;

    if( (!fileName.endsWith(".lvl", Qt::CaseInsensitive)) && (!fileName.endsWith(".lvlx", Qt::CaseInsensitive)) )
    {
        QMessageBox::warning(this, tr("Extension is not set"),
           tr("File Extension isn't defined, please enter file extension!"), QMessageBox::Ok);
        return false;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    //Mark stars
    for(int q=0; q< LvlData.npc.size(); q++)
    {
        int id = MainWinConnect::pMainWin->configs.getNpcI(LvlData.npc[q].id);
        if(id<0) continue;
        LvlData.npc[q].is_star = MainWinConnect::pMainWin->configs.main_npc[id].is_star;
        if((LvlData.npc[q].is_star) && (LvlData.npc[q].friendly))
            LvlData.npc[q].is_star=false;
    }

    // ////////////////////// Write SMBX64 LVL //////////////////////////////
    if(fileName.endsWith(".lvl", Qt::CaseInsensitive))
    {
        if(!saveSMBX64LVL(fileName, false))
            return false;
        LvlData.smbx64strict = true; //Enable SMBX64 standard strict mode
    }
    // //////////////////////////////////////////////////////////////////////

    // ////////////////// Write Extended LVL file (LVLX)/////////////////////
    else if(fileName.endsWith(".lvlx", Qt::CaseInsensitive))
    {
        if(!savePGEXLVL(fileName, false))
            return false;
        LvlData.smbx64strict = false; //Disable strict mode
    }
    // //////////////////////////////////////////////////////////////////////
    GlobalSettings::savePath = QFileInfo(fileName).path();
    LvlData.path = QFileInfo(fileName).path();
    LvlData.filename = QFileInfo(fileName).baseName();

    QApplication::restoreOverrideCursor();
    setCurrentFile(fileName);


    LvlData.modified = false;
    LvlData.untitled = false;
    if(addToRecent)
    {
        MainWinConnect::pMainWin->AddToRecentFiles(fileName);
        MainWinConnect::pMainWin->SyncRecentFiles();
    }

    //Refresh Strict SMBX64 flag
    emit MainWinConnect::pMainWin->setSMBX64Strict(LvlData.smbx64strict);

    return true;
}

bool LevelEdit::savePGEXLVL(QString fileName, bool silent)
{
    using namespace lvl_file_io;
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        if(!silent)
        QMessageBox::warning(this, tr("File save error"),
                             tr("Cannot save file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }
    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << FileFormats::WriteExtendedLvlFile(LvlData);
    file.close();
    return true;
}

bool LevelEdit::saveSMBX64LVL(QString fileName, bool silent)
{
    using namespace lvl_file_io;

    //SMBX64 Standard check
    isSMBX64limit=false;

    int file_format=64;
    if(choiceVersionID)
    {
        QApplication::restoreOverrideCursor();
        bool ok=true;
        file_format = QInputDialog::getInt(this, tr("SMBX file version"),
                              tr("Which version you wish to save? (from 0 to 64)"), 64, 0, 64, 1, &ok);
        if(!ok) return false;
        QApplication::setOverrideCursor(Qt::WaitCursor);
    }

    int ErrorCode=FileFormats::smbx64LevelCheckLimits(LvlData);
    if(ErrorCode!=FileFormats::SMBX64_FINE)
    {
        if(!silent)
            QMessageBox::warning(this,
                                 tr("The SMBX64 limit has been exceeded"),
                                 smbx64ErrMsgs(LvlData, ErrorCode),
                                 QMessageBox::Ok);
        isSMBX64limit=true;
    }

    if(isSMBX64limit)
    {
        if(!silent)
        {
            if(QMessageBox::question(this, tr("The SMBX64 limit has been exceeded"),
             tr("Do you want to save file anyway?\nExciting of SMBX64 limits may crash SMBX with 'overflow' error.\n\nInstalled LunaLUA partially extends than limits."), QMessageBox::Yes|QMessageBox::No)==QMessageBox::No)
            {
                if(!silent)
                    QApplication::restoreOverrideCursor();
                return false;
            }
            else
                isSMBX64limit=false;
        }
    }

    //Apply SMBX64-specific things to entire array
    FileFormats::smbx64LevelPrepare(LvlData);

    QFile file(fileName);
    if(!file.open(QFile::WriteOnly))
    {
        QMessageBox::warning(this, tr("File save error"),
                             tr("Cannot save file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QString raw = FileFormats::WriteSMBX64LvlFile(LvlData, file_format);
    for(int i=0; i<raw.size(); i++)
    {
        if(raw[i]=='\n')
        {
            //Force writing CRLF to prevent fakse damage of file on SMBX in Windows
            const char bytes[2] = {0x0D, 0x0A};
            file.write((const char*)(&bytes), 2);
        }
        else
        {
            const char byte[1] = {raw[i].toLatin1()};
            file.write((const char*)(&byte), 1);
        }
    }
    file.close();

    //save additional meta data
    if( (!LvlData.metaData.bookmarks.isEmpty())
        ||((LvlData.metaData.script!=NULL)&&(!LvlData.metaData.script->events().isEmpty())) )
    {
        file.setFileName(fileName+".meta");
        if (!file.open(QFile::WriteOnly | QFile::Text))
        {
            QMessageBox::warning(this, tr("File save error"),
                                 tr("Cannot save file %1:\n%2.")
                                 .arg(fileName+".meta")
                                 .arg(file.errorString()));
            return false;
        }
        QTextStream out(&file);
        out.setCodec("UTF-8");
        out << FileFormats::WriteNonSMBX64MetaData(LvlData.metaData);
        file.close();
    }
    return true;
}




bool LevelEdit::loadFile(const QString &fileName, LevelData FileData, dataconfigs &configs, LevelEditingSettings options)
{
    QFile file(fileName);
    LvlData = FileData;
    if(!LvlData.metaData.script)
        LvlData.metaData.script = new ScriptHolder;
    bool modifystate = false;
    bool untitledstate = false;
    LvlData.modified = false;
    LvlData.untitled = false;
    QString curFName=fileName;
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Read file error"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }
    StartLvlData = LvlData; //Save current history for made reset
    setCurrentFile(curFName);

    //Restore internal information after crash
    if(LvlData.metaData.crash.used)
    {
        modifystate=LvlData.metaData.crash.modifyed;
        untitledstate=LvlData.metaData.crash.untitled;
        isUntitled = LvlData.metaData.crash.untitled;
        LvlData.filename = LvlData.metaData.crash.filename;
        LvlData.path = LvlData.metaData.crash.path;
        curFName = LvlData.metaData.crash.fullPath;
        setCurrentFile(LvlData.metaData.crash.fullPath);
        LvlData.metaData.crash.reset();
    }

    ui->graphicsView->setBackgroundBrush(QBrush(Qt::darkGray));

    //Check if data configs exists
    if( configs.check() )
    {
        WriteToLog(QtCriticalMsg, QString("Error! *.INI configs not loaded"));
        this->close();
        return false;
    }

    WriteToLog(QtDebugMsg, QString(">>Starting to load file"));

    //Declaring of the scene
    scene = new LvlScene(ui->graphicsView, configs, LvlData, this);

    scene->opts = options;

    int DataSize=0;

    DataSize += 3;
    DataSize += 6; /*LvlData.sections.size()*2;
    DataSize += LvlData.bgo.size();
    DataSize += LvlData.blocks.size();
    DataSize += LvlData.npc.size();
    DataSize += LvlData.water.size();
    DataSize += LvlData.doors.size();*/

    QProgressDialog progress(tr("Loading level data"), tr("Abort"), 0, DataSize, MainWinConnect::pMainWin);
         progress.setWindowTitle(tr("Loading level data"));
         progress.setWindowModality(Qt::WindowModal);
         progress.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
         progress.setFixedSize(progress.size());
         progress.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, progress.size(), qApp->desktop()->availableGeometry()));
         progress.setMinimumDuration(500);
         //progress.setCancelButton(0);

    if(! DrawObjects(progress) )
    {
        LvlData.modified = false;
        this->close();
        return false;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    if( !progress.wasCanceled() )
        progress.close();

    QApplication::restoreOverrideCursor();

    setAutoUpdateTimer(31);

    setCurrentFile(curFName);
    LvlData.modified = modifystate;
    LvlData.untitled = untitledstate;

    progress.deleteLater();

    return true;
}


void LevelEdit::documentWasModified()
{
    //LvlData.modified = true;
}

bool LevelEdit::maybeSave()
{
    if (LvlData.modified)
    {
        SavingNotificationDialog* sav = new SavingNotificationDialog(true, SavingNotificationDialog::D_WARN, this);
        util::DialogToCenter(sav, true);
        sav->setSavingTitle(tr("'%1' has been modified.\n"
                               "Do you want to save your changes?").arg(userFriendlyCurrentFile()));
        sav->setWindowTitle(userFriendlyCurrentFile()+tr(" not saved"));
        QLineEdit* lvlNameBox = new QLineEdit();
        sav->addUserItem(tr("Level title: "),lvlNameBox);
        sav->setAdjustSize(400,130);
        lvlNameBox->setText(LvlData.LevelName);
        if(sav->exec() == QDialog::Accepted){
            LvlData.LevelName = lvlNameBox->text();
            lvlNameBox->deleteLater();
            sav->deleteLater();
            if(sav->savemode == SavingNotificationDialog::SAVE_SAVE){
                return save(false);
            }else if(sav->savemode == SavingNotificationDialog::SAVE_CANCLE){
                return false;
            }
        }else{
            return false;
        }
    }

    return true;
}


//////// Common
QString LevelEdit::userFriendlyCurrentFile()
{
    return strippedName(curFile);
}

void LevelEdit::makeCrashState()
{
    this->isUntitled = true;
    this->LvlData.modified = true;
    this->LvlData.untitled = true;
}

void LevelEdit::closeEvent(QCloseEvent *event)
{
    if(!sceneCreated)
    {
        event->accept();
        return;
    }
    else
        MainWinConnect::pMainWin->on_actionSelect_triggered();

    if(maybeSave()) {
        stopAutoUpdateTimer();

        LvlMusPlay::stopMusic();

        scene->setMessageBoxItem(false);
        scene->clear();
        WriteToLog(QtDebugMsg, "!<-Cleared->!");
        scene->uBGOs.clear();
        scene->uBGs.clear();
        scene->uBlocks.clear();
        scene->uNPCs.clear();

        WriteToLog(QtDebugMsg, "!<-Delete animators->!");
        while(! scene->animates_BGO.isEmpty() )
        {
            SimpleAnimator* tmp = scene->animates_BGO.first();
            scene->animates_BGO.pop_front();
            if(tmp!=NULL) delete tmp;
        }
        while(! scene->animates_Blocks.isEmpty() )
        {
            SimpleAnimator* tmp = scene->animates_Blocks.first();
            scene->animates_Blocks.pop_front();
            if(tmp!=NULL) delete tmp;
        }
        while(! scene->animates_NPC.isEmpty() )
        {
            AdvNpcAnimator* tmp = scene->animates_NPC.first();
            scene->animates_NPC.pop_front();
            if(tmp!=NULL) delete tmp;
        }
        WriteToLog(QtDebugMsg, "!<-Delete scene->!");
        delete scene;
        sceneCreated=false;
        WriteToLog(QtDebugMsg, "!<-Deleted->!");
        //ui->graphicsView->cl
        event->accept();
    } else {
        event->ignore();
    }
}

void LevelEdit::setCurrentFile(const QString &fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    LvlData.path = QFileInfo(fileName).absoluteDir().absolutePath();
    LvlData.filename = QFileInfo(fileName).baseName();
    LvlData.untitled = false;
    //document()->setModified(false);
    setWindowModified(false);
    setWindowTitle(QString(LvlData.LevelName=="" ? userFriendlyCurrentFile() : LvlData.LevelName).replace("&", "&&&"));
}

QString LevelEdit::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

