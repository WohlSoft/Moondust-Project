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
#include <QGraphicsItem>
#include <QPixmap>
#include <QGraphicsScene>
#include <QProgressDialog>

#include <common_features/logger.h>
#include <common_features/util.h>
#include <common_features/mainwinconnect.h>
#include <editing/_scenes/world/wld_scene.h>
#include <editing/_dialogs/savingnotificationdialog.h>
#include <main_window/global_settings.h>
#include <audio/music_player.h>
#include <file_formats/file_formats.h>

#include "world_edit.h"
#include <ui_world_edit.h>

void WorldEdit::newFile(dataconfigs &configs, LevelEditingSettings options)
{
    static int sequenceNumber = 1;

    isUntitled = true;
    curFile = tr("Untitled %1").arg(sequenceNumber++);
    setWindowTitle(curFile);
    WldData = FileFormats::dummyWldDataArray();
    WldData.modified = true;
    WldData.untitled = true;
    StartWldData = WldData;

    ui->graphicsView->setBackgroundBrush(QBrush(Qt::black));

    //Check if data configs exists
    if( configs.check() )
    {
        WriteToLog(QtCriticalMsg, QString("Error! *.INI configs not loaded"));

        QMessageBox::warning(this, tr("Configurations not loaded"),
                             tr("Cannot create world file:\nbecause object configurations are not loaded\n."
                                "Please check that the ""config/SMBX"" directory exists and contains the *.INI files with object settings."));

        WriteToLog(QtCriticalMsg, QString(" << close subWindow"));

        this->close();

        WriteToLog(QtCriticalMsg, QString(" << closed, return false"));
        return;
    }

    scene = new WldScene(ui->graphicsView, configs, WldData);
    scene->opts = options;

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
}

namespace wld_file_io
{
    bool isSMBX64limit=false;
    bool choiceVersionID=false;
}

bool WorldEdit::save(bool savOptionsDialog)
{
    if (isUntitled) {
        return saveAs(savOptionsDialog);
    } else {
        wld_file_io::choiceVersionID=false;
        return saveFile(curFile);
    }
}

bool WorldEdit::saveAs(bool savOptionsDialog)
{
    using namespace wld_file_io;

    if(savOptionsDialog){
        SavingNotificationDialog* sav = new SavingNotificationDialog(false, SavingNotificationDialog::D_QUESTION, this);
        util::DialogToCenter(sav, true);
        sav->setSavingTitle(tr("Please enter a episode title for '%1'!").arg(userFriendlyCurrentFile()));
        sav->setWindowTitle(tr("Saving") + " " + userFriendlyCurrentFile());
        QLineEdit* wldNameBox = new QLineEdit();
        sav->addUserItem(tr("Episode Title: "),wldNameBox);
        sav->setAdjustSize(400,150);
        wldNameBox->setText(WldData.EpisodeTitle);
        if(sav->exec() == QDialog::Accepted){
            WldData.EpisodeTitle = wldNameBox->text();
            wldNameBox->deleteLater();
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
                                    (WldData.EpisodeTitle.isEmpty()?curFile:util::filePath(WldData.EpisodeTitle)):curFile;

    QString fileSMBX64="SMBX64 (1.3) World map file (*.wld)";
    QString fileSMBXany="SMBX0...64 Level file (*.wld) [choose version]";
    QString filePGEX="Extended World map file (*.wldx)";

    QString selectedFilter;
    if(fileName.endsWith(".wldx", Qt::CaseInsensitive))
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

        if( (!fileName.endsWith(".wld", Qt::CaseInsensitive)) && (!fileName.endsWith(".wldx", Qt::CaseInsensitive)) )
        {
            QMessageBox::warning(this, tr("Extension is not set"),
               tr("File Extension isn't defined, please enter file extension!"), QMessageBox::Ok);
            continue;
        }

        /*if(makeCustomFolder)
        {
            QDir dir = fileName.section("/",0,-2);
            dir.mkdir(fileName.section("/",-1,-1).section(".",0,0));
        }*/
        isNotDone=false;
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

    if( (!fileName.endsWith(".wld", Qt::CaseInsensitive)) && (!fileName.endsWith(".wldx", Qt::CaseInsensitive)) )
    {
        QMessageBox::warning(this, tr("Extension is not set"),
           tr("File Extension isn't defined, please enter file extension!"), QMessageBox::Ok);
        return false;
    }


    QApplication::setOverrideCursor(Qt::WaitCursor);
    // ////////////////////// Write SMBX64 WLD //////////////////////////////
    if(fileName.endsWith(".wld", Qt::CaseInsensitive))
    {
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

        //Tiles limit
        if(WldData.tiles.size()>20000)
        {
            QMessageBox::warning(this, tr("The SMBX64 limit has been exceeded"),
             tr("SMBX64 standard isn't allow to save %1 Tiles\n"
                "The maximum number of Tiles is %2.\n\n"
                "Please remove excess Tiles from this world map or save file into WLDX format.")
             .arg(WldData.tiles.size()).arg(20000), QMessageBox::Ok);
            isSMBX64limit=true;
        }
        //Sceneries limit
        if(WldData.scenery.size()>5000)
        {
            QMessageBox::warning(this, tr("The SMBX64 limit has been exceeded"),
             tr("SMBX64 standard isn't allow to save %1 Sceneries\n"
                "The maximum number of Sceneries is %2.\n\n"
                "Please remove excess Sceneries from this world map or save file into WLDX format.")
             .arg(WldData.scenery.size()).arg(5000), QMessageBox::Ok);
            isSMBX64limit=true;
        }
        //Paths limit
        if(WldData.paths.size()>2000)
        {
            QMessageBox::warning(this, tr("The SMBX64 limit has been exceeded"),
             tr("SMBX64 standard isn't allow to save %1 Paths\n"
                "The maximum number of Paths is %2.\n\n"
                "Please remove excess Paths from this world map or save file into WLDX format.")
             .arg(WldData.paths.size()).arg(2000), QMessageBox::Ok);
            isSMBX64limit=true;
        }
        //Levels limit
        if(WldData.levels.size()>400)
        {
            QMessageBox::warning(this, tr("The SMBX64 limit has been exceeded"),
             tr("SMBX64 standard isn't allow to save %1 Levels\n"
                "The maximum number of Levels is %2.\n\n"
                "Please remove excess Paths from this world map or save file into WLDX format.")
             .arg(WldData.levels.size()).arg(400), QMessageBox::Ok);
            isSMBX64limit=true;
        }

        //Music boxes limit
        if(WldData.music.size()>1000)
        {
            QMessageBox::warning(this, tr("The SMBX64 limit has been exceeded"),
             tr("SMBX64 standard isn't allow to save %1 Music Boxes\n"
                "The maximum number of Music Boxes is %2.\n\n"
                "Please remove excess Music Boxes from this world map or save file into WLDX format.")
             .arg(WldData.music.size()).arg(1000), QMessageBox::Ok);
            isSMBX64limit=true;
        }

        if(isSMBX64limit)
        {
            QApplication::restoreOverrideCursor();
            return false;
        }

        QFile file(fileName);
        if (!file.open(QFile::WriteOnly | QFile::Text)) {
            QMessageBox::warning(this, tr("File save error"),
                                 tr("Cannot save file %1:\n%2.")
                                 .arg(fileName)
                                 .arg(file.errorString()));
            return false;
        }
        WldData.smbx64strict = true; //Enable SMBX64 standard strict mode


        QString raw = FileFormats::WriteSMBX64WldFile(WldData, file_format);
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
        if(!WldData.metaData.bookmarks.isEmpty())
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
            out << FileFormats::WriteNonSMBX64MetaData(WldData.metaData);
            file.close();
        }

        GlobalSettings::savePath = QFileInfo(fileName).path();
    }
    // //////////////////////////////////////////////////////////////////////

    // ////////////////// Write Extended WLD file (WLDX)/////////////////////
    else if(fileName.endsWith(".wldx", Qt::CaseInsensitive))
    {
        WldData.smbx64strict = false; //Disable strict mode

        QFile file(fileName);
        if (!file.open(QFile::WriteOnly | QFile::Text)) {
            QMessageBox::warning(this, tr("File save error"),
                                 tr("Cannot save file %1:\n%2.")
                                 .arg(fileName)
                                 .arg(file.errorString()));
            return false;
        }
        QTextStream out(&file);
        out.setCodec("UTF-8");
        out << FileFormats::WriteExtendedWldFile(WldData);
        file.close();
        GlobalSettings::savePath = QFileInfo(fileName).path();
    }
    // //////////////////////////////////////////////////////////////////////


    QApplication::restoreOverrideCursor();
    setCurrentFile(fileName);

    WldData.modified = false;
    WldData.untitled = false;

    if(addToRecent){
        MainWinConnect::pMainWin->AddToRecentFiles(fileName);
        MainWinConnect::pMainWin->SyncRecentFiles();
    }
    return true;
}


bool WorldEdit::loadFile(const QString &fileName, WorldData FileData, dataconfigs &configs, LevelEditingSettings options)
{
    QFile file(fileName);
    WldData = FileData;
    setCurrentFile(fileName);
    WldData.modified = false;
    WldData.untitled = false;
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Read file error"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }
    StartWldData = WldData; //Save current history for made reset

    ui->graphicsView->setBackgroundBrush(QBrush(Qt::black));

    //Check if data configs exists
    if( configs.check() )
    {
        WriteToLog(QtCriticalMsg, QString("Error! *.INI configs not loaded"));

        QMessageBox::warning(this, tr("Configurations not loaded"),
                             tr("Cannot open level world %1:\nbecause object configurations are not loaded\n."
                                "Please check that the ""config/SMBX"" directory exists and contains the *.INI files with object settings.")
                             .arg(fileName));

        WriteToLog(QtCriticalMsg, QString(" << close subWindow"));

        this->close();

        WriteToLog(QtCriticalMsg, QString(" << closed, return false"));
        return false;
    }

    WriteToLog(QtDebugMsg, QString(">>Starting to load file"));

    //Declaring of the scene
    scene = new WldScene(ui->graphicsView, configs, WldData);

    scene->opts = options;

    int DataSize=0;

    DataSize += 3;
    DataSize += 6;

    QProgressDialog progress(tr("Loading World map data"), tr("Abort"), 0, DataSize, this);
         progress.setWindowTitle(tr("Loading World data"));
         progress.setWindowModality(Qt::WindowModal);
         progress.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
         progress.setFixedSize(progress.size());
         progress.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, progress.size(), qApp->desktop()->availableGeometry()));
         progress.setMinimumDuration(0);

    if(! DrawObjects(progress) )
    {
        WldData.modified = false;
        this->close();
        return false;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    if( !progress.wasCanceled() )
        progress.close();

    QApplication::restoreOverrideCursor();

    setAutoUpdateTimer(31);

    WldData.modified = false;
    WldData.untitled = false;

    progress.deleteLater();

    return true;
}


void WorldEdit::documentWasModified()
{
    WldData.modified = true;
}

bool WorldEdit::maybeSave()
{
    if (WldData.modified) {
        SavingNotificationDialog* sav = new SavingNotificationDialog(true,SavingNotificationDialog::D_WARN, this);
        util::DialogToCenter(sav, true);
        sav->setSavingTitle(tr("'%1' has been modified.\n"
                               "Do you want to save your changes?").arg(userFriendlyCurrentFile()));
        sav->setWindowTitle(userFriendlyCurrentFile()+tr(" not saved"));
        QLineEdit* wldNameBox = new QLineEdit();
        sav->addUserItem(tr("Episode title: "),wldNameBox);
        sav->setAdjustSize(400,150);
        wldNameBox->setText(WldData.EpisodeTitle);
        if(sav->exec() == QDialog::Accepted){
            WldData.EpisodeTitle = wldNameBox->text();
            wldNameBox->deleteLater();
            sav->deleteLater();
            if(sav->savemode == SavingNotificationDialog::SAVE_SAVE){
                return save();
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
QString WorldEdit::userFriendlyCurrentFile()
{
    return strippedName(curFile);
}

void WorldEdit::makeCrashState()
{
    this->isUntitled = true;
    this->WldData.untitled = true;
    this->WldData.modified = true;
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

    if(maybeSave()) {
        stopAutoUpdateTimer();

        LvlMusPlay::stopMusic();

        scene->setMessageBoxItem(false);
        scene->clear();
        WriteToLog(QtDebugMsg, "!<-Cleared->!");
        scene->uTiles.clear();
        scene->uScenes.clear();
        scene->uPaths.clear();
        scene->uLevels.clear();

        WriteToLog(QtDebugMsg, "!<-Delete animators->!");
        while(! scene->animates_Tiles.isEmpty() )
        {
            SimpleAnimator* tmp = scene->animates_Tiles.first();
            scene->animates_Tiles.pop_front();
            if(tmp!=NULL) delete tmp;
        }
        while(! scene->animates_Scenery.isEmpty() )
        {
            SimpleAnimator* tmp = scene->animates_Scenery.first();
            scene->animates_Scenery.pop_front();
            if(tmp!=NULL) delete tmp;
        }
        while(! scene->animates_Paths.isEmpty() )
        {
            SimpleAnimator* tmp = scene->animates_Paths.first();
            scene->animates_Paths.pop_front();
            if(tmp!=NULL) delete tmp;
        }
        while(! scene->animates_Levels.isEmpty() )
        {
            SimpleAnimator* tmp = scene->animates_Levels.first();
            scene->animates_Levels.pop_front();
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

void WorldEdit::setCurrentFile(const QString &fileName)
{
    curFile = QFileInfo(fileName).canonicalFilePath();
    isUntitled = false;
    WldData.path = QFileInfo(fileName).absoluteDir().absolutePath();
    WldData.filename = QFileInfo(fileName).baseName();
    WldData.untitled = false;
    //document()->setModified(false);
    setWindowModified(false);
    setWindowTitle(QString(WldData.EpisodeTitle =="" ? userFriendlyCurrentFile() : WldData.EpisodeTitle).replace("&", "&&&") );
}

QString WorldEdit::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

