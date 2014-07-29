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


#include <QtWidgets>
#include <QGraphicsItem>
#include <QPixmap>
#include <QGraphicsScene>
#include <QProgressDialog>

#include "world_edit.h"
#include "../ui_world_edit.h"

#include "../file_formats/file_formats.h"
#include "../world_scene/wld_scene.h"

//#include "saveimage.h"

#include "../common_features/logger.h"

#include "../common_features/mainwinconnect.h"

#include "../main_window/music_player.h"
#include "../main_window/global_settings.h"
#include "../main_window/savingnotificationdialog.h"


void WorldEdit::ExportToImage_fn()
{
//        long x, y, h, w, th, tw;

//        bool proportion;
//        QString inifile = QApplication::applicationDirPath() + "/" + "pge_editor.ini";
//        QSettings settings(inifile, QSettings::IniFormat);
//        settings.beginGroup("Main");
//        latest_export_path = settings.value("export-path", QApplication::applicationDirPath()).toString();
//        proportion = settings.value("export-proportions", true).toBool();
//        settings.endGroup();


//        x=WldData.sections[WldData.CurSection].size_left;
//        y=WldData.sections[WldData.CurSection].size_top;
//        w=WldData.sections[WldData.CurSection].size_right;
//        h=WldData.sections[WldData.CurSection].size_bottom;
//        w=(long)fabs(x-w);
//        h=(long)fabs(y-h);

//        tw=w;
//        th=h;
//        QVector<long> imgSize;

//        imgSize.push_back(th);
//        imgSize.push_back(tw);
//        imgSize.push_back((int)proportion);

//        ExportToImage ExportImage(imgSize);
//        ExportImage.setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
//        ExportImage.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, ExportImage.size(), qApp->desktop()->availableGeometry()));
//        if(ExportImage.exec()!=QDialog::Rejected)
//            imgSize = ExportImage.imageSize;
//        else return;

//        if(imgSize.size()>=3)
//            if((imgSize[0]<0)||(imgSize[1]<0))
//                return;

//        QString fileName = QFileDialog::getSaveFileName(this, tr("Export current section to image"),
//            latest_export_path + "/" +
//            QString("%1_Section_%2.png").arg( QFileInfo(curFile).baseName() ).arg(WldData.CurSection+1), tr("PNG Image (*.png)"));
//        if (fileName.isEmpty())
//            return;

//        QFileInfo exported(fileName);

//        QProgressDialog progress(tr("Saving section image..."), tr("Abort"), 0, 100, this);
//        progress.setWindowTitle(tr("Please wait..."));
//        progress.setWindowModality(Qt::WindowModal);
//        progress.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
//        progress.setFixedSize(progress.size());
//        progress.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, progress.size(), qApp->desktop()->availableGeometry()));
//        progress.setCancelButton(0);
//        progress.setMinimumDuration(0);

//        //progress.show();

//        if(!progress.wasCanceled()) progress.setValue(0);

//        qApp->processEvents();
//        if(scene->opts.animationEnabled) scene->stopAnimation(); //Reset animation to 0 frame
//        if(ExportImage.HideWatersAndDoors()) scene->hideWarpsAndDoors(false);

//        if(!progress.wasCanceled()) progress.setValue(10);
//        qApp->processEvents();
//        scene->clearSelection(); // Clear selection on export

//        latest_export_path = exported.absoluteDir().path();
//        proportion = imgSize[2];

//        th=imgSize[0];
//        tw=imgSize[1];

//        qApp->processEvents();
//        QImage img(tw,th,QImage::Format_ARGB32_Premultiplied);

//        if(!progress.wasCanceled()) progress.setValue(20);

//        qApp->processEvents();
//        QPainter p(&img);

//        if(!progress.wasCanceled()) progress.setValue(30);
//        qApp->processEvents();
//        scene->render(&p, QRectF(0,0,tw,th),QRectF(x,y,w,h));

//        qApp->processEvents();
//        p.end();

//        if(!progress.wasCanceled()) progress.setValue(40);
//        qApp->processEvents();
//        img.save(fileName);

//        qApp->processEvents();
//        if(!progress.wasCanceled()) progress.setValue(90);

//        qApp->processEvents();
//        if(scene->opts.animationEnabled) scene->startBlockAnimation(); // Restart animation
//        if(ExportImage.HideWatersAndDoors()) scene->hideWarpsAndDoors(true);

//        if(!progress.wasCanceled()) progress.setValue(100);
//        if(!progress.wasCanceled())
//            progress.close();

//        settings.beginGroup("Main");
//            settings.setValue("export-path", latest_export_path);
//            settings.setValue("export-proportions", proportion);
//        settings.endGroup();
}



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
                             tr("Cannot create level file:\nbecause object configurations are not loaded\n."
                                "Please check that the ""config/SMBX"" directory exists and contains the *.INI files with object settings."));

        WriteToLog(QtCriticalMsg, QString(" << close subWindow"));

        this->close();

        WriteToLog(QtCriticalMsg, QString(" << closed, return false"));
        return;
    }

    scene = new WldScene(configs, WldData);
    scene->opts = options;

    //scene->InitSection(0);
    //scene->drawSpace();
    scene->buildAnimators();

    if(!sceneCreated)
    {
        ui->graphicsView->setScene(scene);
        sceneCreated = true;
    }

    if(options.animationEnabled) scene->startAnimation();
    setAutoUpdateTimer(31);
}


bool WorldEdit::save()
{
    if (isUntitled) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool WorldEdit::saveAs()
{
    SavingNotificationDialog* sav = new SavingNotificationDialog(false);
    sav->setSavingTitle(tr("Please enter a episode title for '%1'!").arg(userFriendlyCurrentFile()));
    sav->setWindowTitle(tr("Saving ") + userFriendlyCurrentFile());
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


    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
        (isUntitled)?GlobalSettings::savePath+QString("/")+curFile:curFile, QString("SMBX64 (1.3) World map file (*.wld)"));
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

bool WorldEdit::saveFile(const QString &fileName, const bool addToRecent)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("File save error"),
                             tr("Cannot save file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    GlobalSettings::savePath = QFileInfo(fileName).path();

    QTextStream out(&file);

    QApplication::setOverrideCursor(Qt::WaitCursor);


    // ////////////////////// Write SMBX64 WLD //////////////////////////////
    out << FileFormats::WriteSMBX64WldFile(WldData);
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
                             tr("Cannot open level file %1:\nbecause object configurations are not loaded\n."
                                "Please check that the ""config/SMBX"" directory exists and contains the *.INI files with object settings.")
                             .arg(fileName));

        WriteToLog(QtCriticalMsg, QString(" << close subWindow"));

        this->close();

        WriteToLog(QtCriticalMsg, QString(" << closed, return false"));
        return false;
    }

    WriteToLog(QtDebugMsg, QString(">>Starting to load file"));

    //Declaring of the scene
    scene = new WldScene(configs, WldData);

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
        SavingNotificationDialog* sav = new SavingNotificationDialog(true);
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
        //LvlMusPlay::musicForceReset = true;
        //MainWinConnect::pMainWin->setMusicButton(false);
        //MainWinConnect::pMainWin->setMusic(false);

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
    setWindowTitle(WldData.EpisodeTitle =="" ? userFriendlyCurrentFile() : WldData.EpisodeTitle );
}

QString WorldEdit::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

