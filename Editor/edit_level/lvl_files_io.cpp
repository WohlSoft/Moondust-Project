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

#include "level_edit.h"
#include "../ui_leveledit.h"

#include "../common_features/app_path.h"

#include "../file_formats/file_formats.h"
#include "../level_scene/lvlscene.h"
#include "saveimage.h"
#include "../common_features/logger.h"
#include "../common_features/util.h"

#include "../common_features/mainwinconnect.h"
#include "../main_window/music_player.h"
#include "../main_window/global_settings.h"
#include "../main_window/savingnotificationdialog.h"

#include <QDebug>


//Export whole section
void leveledit::ExportToImage_fn()
{
    if(!sceneCreated) return;
    if(!scene) return;

    scene->setScreenshotSelector();
}


//Export piece
void leveledit::ExportToImage_fn_piece()
{
    if(!sceneCreated) return;
    if(!scene) return;

    MainWinConnect::pMainWin->on_actionSelect_triggered();

    qreal zoom=1.0;
    if(QString(ui->graphicsView->metaObject()->className())=="GraphicsWorkspace")
    {
        zoom = static_cast<GraphicsWorkspace *>(ui->graphicsView)->zoom();
    }

    scene->captutedSize.setX(qRound(qreal(ui->graphicsView->horizontalScrollBar()->value())/zoom)+10 );
    scene->captutedSize.setY(qRound(qreal(ui->graphicsView->verticalScrollBar()->value())/zoom)+10 );
    scene->captutedSize.setWidth(qRound(qreal(ui->graphicsView->viewport()->width())/zoom)-20);
    scene->captutedSize.setHeight(qRound(qreal(ui->graphicsView->viewport()->height())/zoom)-20);

    scene->setScreenshotSelector(true);
}


void leveledit::ExportingReady() //slot
{
    if(!sceneCreated) return;
    if(!scene) return;

        long x, y, h, w, th, tw;

        bool proportion;
        bool forceTiled=false;
        QString inifile = ApplicationPath + "/" + "pge_editor.ini";
        QSettings settings(inifile, QSettings::IniFormat);
        settings.beginGroup("Main");
        latest_export_path = settings.value("export-path", ApplicationPath).toString();
        proportion = settings.value("export-proportions", true).toBool();
        settings.endGroup();


        if(scene->isFullSection)
        {
            x=LvlData.sections[LvlData.CurSection].size_left;
            y=LvlData.sections[LvlData.CurSection].size_top;
            w=LvlData.sections[LvlData.CurSection].size_right;
            h=LvlData.sections[LvlData.CurSection].size_bottom;
            w=(long)fabs(x-w);
            h=(long)fabs(y-h);
        }
        else
        {
            x=qRound(scene->captutedSize.x());
            y=qRound(scene->captutedSize.y());
            w=qRound(scene->captutedSize.width());
            h=qRound(scene->captutedSize.height());
        }

        tw=w;
        th=h;
        QVector<long> imgSize;

        imgSize.push_back(th);
        imgSize.push_back(tw);
        imgSize.push_back((int)proportion);

        ExportToImage ExportImage(imgSize);
        ExportImage.setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
        ExportImage.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, ExportImage.size(), qApp->desktop()->availableGeometry()));
        if(ExportImage.exec()!=QDialog::Rejected)
            imgSize = ExportImage.imageSize;
        else return;

        if(imgSize.size()>=3)
            if((imgSize[0]<0)||(imgSize[1]<0))
                return;

        QString fileName = QFileDialog::getSaveFileName(this, tr("Export current section to image"),
            latest_export_path + "/" +
            QString("%1_Section_%2%3.png").arg( QFileInfo(curFile).baseName() )
                                                        .arg(LvlData.CurSection+1)
                                                        .arg(scene->isFullSection?"":("_"+QString::number(qrand()))),
                                                        tr("PNG Image (*.png)"));
        if (fileName.isEmpty())
            return;

        forceTiled = ExportImage.TiledBackground();

        QFileInfo exported(fileName);

        QProgressDialog progress(tr("Saving section image..."), tr("Abort"), 0, 100, this);
        progress.setWindowTitle(tr("Please wait..."));
        progress.setWindowModality(Qt::WindowModal);
        progress.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
        progress.setFixedSize(progress.size());
        progress.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, progress.size(), qApp->desktop()->availableGeometry()));
        progress.setCancelButton(0);
        progress.setMinimumDuration(0);

        //progress.show();

        if(!progress.wasCanceled()) progress.setValue(0);

        qApp->processEvents();
        if(scene->opts.animationEnabled) scene->stopAnimation(); //Reset animation to 0 frame
        if(ExportImage.HideWatersAndDoors()) scene->hideWarpsAndDoors(false);
        if(forceTiled) scene->setTiledBackground(true);

        if(!progress.wasCanceled()) progress.setValue(10);
        qApp->processEvents();
        scene->clearSelection(); // Clear selection on export

        latest_export_path = exported.absoluteDir().path();
        proportion = imgSize[2];

        th=imgSize[0];
        tw=imgSize[1];

        qApp->processEvents();
        QImage img(tw,th,QImage::Format_ARGB32_Premultiplied);

        if(!progress.wasCanceled()) progress.setValue(20);

        qApp->processEvents();
        QPainter p(&img);

        if(!progress.wasCanceled()) progress.setValue(30);
        qApp->processEvents();
        scene->render(&p, QRectF(0,0,tw,th),QRectF(x,y,w,h));

        qApp->processEvents();
        p.end();

        if(!progress.wasCanceled()) progress.setValue(40);
        qApp->processEvents();
        img.save(fileName);

        qApp->processEvents();
        if(!progress.wasCanceled()) progress.setValue(90);

        qApp->processEvents();
        if(scene->opts.animationEnabled) scene->startBlockAnimation(); // Restart animation
        if(ExportImage.HideWatersAndDoors()) scene->hideWarpsAndDoors(true);
        if(forceTiled) scene->setTiledBackground(false);

        if(!progress.wasCanceled()) progress.setValue(100);
        if(!progress.wasCanceled())
            progress.close();

        settings.beginGroup("Main");
            settings.setValue("export-path", latest_export_path);
            settings.setValue("export-proportions", proportion);
        settings.endGroup();
}





void leveledit::newFile(dataconfigs &configs, LevelEditingSettings options)
{
    static int sequenceNumber = 1;

    isUntitled = true;
    curFile = tr("Untitled %1").arg(sequenceNumber++);
    setWindowTitle(QString(curFile).replace("&", "&&&"));
    LvlData = FileFormats::dummyLvlDataArray();
    LvlData.untitled = true;
    StartLvlData = LvlData;

    ui->graphicsView->setBackgroundBrush(QBrush(Qt::darkGray));

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

    scene = new LvlScene(configs, LvlData);
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

    if(options.animationEnabled) scene->startBlockAnimation();
    setAutoUpdateTimer(31);
}


bool leveledit::save(bool savOptionsDialog)
{
    if (isUntitled) {
        return saveAs(savOptionsDialog);
    } else {
        return saveFile(curFile);
    }
}

namespace lvl_file_io
{
    bool isSMBX64limit=false;
}

bool leveledit::saveAs(bool savOptionsDialog)
{
    using namespace lvl_file_io;
    bool makeCustomFolder = false;

    if(savOptionsDialog){
        SavingNotificationDialog* sav = new SavingNotificationDialog(false);
        sav->setSavingTitle(tr("Please enter a level title for '%1'!").arg(userFriendlyCurrentFile()));
        sav->setWindowTitle(tr("Saving ") + userFriendlyCurrentFile());
        QLineEdit* lvlNameBox = new QLineEdit();
        QCheckBox* mkDirCustom = new QCheckBox();
        mkDirCustom->setText(QString(""));
        sav->addUserItem(tr("Level title: "),lvlNameBox);
        sav->addUserItem(tr("Make custom folder"), mkDirCustom);
        sav->setAdjustSize(400,150);
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
    QString filePGEX="Extended Level file (*.lvlx)";

    QString selectedFilter;
    if(fileName.endsWith(".lvlx", Qt::CaseInsensitive))
        selectedFilter = filePGEX;
    else
        selectedFilter = fileSMBX64;

    QString filter =
            fileSMBX64+";;"+
            filePGEX;

    bool ret;

    RetrySave:

    isSMBX64limit=false;
    isNotDone=true;
    while(isNotDone)
    {
        fileName = QFileDialog::getSaveFileName(this, tr("Save As"), fileName, filter, &selectedFilter);

        if (fileName.isEmpty())
            return false;

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

bool leveledit::saveFile(const QString &fileName, const bool addToRecent)
{
    using namespace lvl_file_io;

    if( (!fileName.endsWith(".lvl", Qt::CaseInsensitive)) && (!fileName.endsWith(".lvlx", Qt::CaseInsensitive)) )
    {
        QMessageBox::warning(this, tr("Extension is not set"),
           tr("File Extension isn't defined, please enter file extension!"), QMessageBox::Ok);
        return false;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    // ////////////////////// Write SMBX64 LVL //////////////////////////////
    if(fileName.endsWith(".lvl", Qt::CaseInsensitive))
    {
        //SMBX64 Standard check

        isSMBX64limit=false;
        //Blocks limit
        if(LvlData.blocks.size()>16384)
        {
            QMessageBox::warning(this, tr("The SMBX64 limit has been exceeded"),
             tr("SMBX64 standard isn't allow to save %1 blocks\n"
                "The maximum number of blocks is %2.\n\n"
                "Please remove excess blocks from this level or save file into LVLX format.")
             .arg(LvlData.blocks.size()).arg(16384), QMessageBox::Ok);
            isSMBX64limit=true;
        }
        //BGO limits
        if(LvlData.bgo.size()>8000)
        {
            QMessageBox::warning(this, tr("The SMBX64 limit has been exceeded"),
             tr("SMBX64 standard isn't allow to save %1 Background Objects\n"
                "The maximum number of Background Objects is %2.\n\n"
                "Please remove excess Background Objects from this level or save file into LVLX format.")
             .arg(LvlData.bgo.size()).arg(8000), QMessageBox::Ok);
            isSMBX64limit=true;
        }
        //NPC limits
        if(LvlData.npc.size()>5000)
        {
            QMessageBox::warning(this, tr("The SMBX64 limit has been exceeded"),
             tr("SMBX64 standard isn't allow to save %1 Non-Playable Characters\n"
                "The maximum number of Non-Playable Characters is %2.\n\n"
                "Please remove excess Non-Playable Characters from this level or save file into LVLX format.")
             .arg(LvlData.npc.size()).arg(5000), QMessageBox::Ok);
            isSMBX64limit=true;
        }
        //Warps limits
        if(LvlData.doors.size()>199)
        {
            QMessageBox::warning(this, tr("The SMBX64 limit has been exceeded"),
             tr("SMBX64 standard isn't allow to save %1 Warps\n"
                "The maximum number of Warps is %2.\n\n"
                "Please remove excess Warps from this level or save file into LVLX format.")
             .arg(LvlData.doors.size()).arg(199), QMessageBox::Ok);
            isSMBX64limit=true;
        }
        //Physical Environment zones
        if(LvlData.physez.size()>400)
        {
            QMessageBox::warning(this, tr("The SMBX64 limit has been exceeded"),
             tr("SMBX64 standard isn't allow to save %1 Water Boxes\n"
                "The maximum number of Water Boxes is %2.\n\n"
                "Please remove excess Water Boxes from this level or save file into LVLX format.")
             .arg(LvlData.physez.size()).arg(400), QMessageBox::Ok);
            isSMBX64limit=true;
        }
        //Layers limits
        if(LvlData.layers.size()>100)
        {
            QMessageBox::warning(this, tr("The SMBX64 limit has been exceeded"),
             tr("SMBX64 standard isn't allow to save %1 Layers\n"
                "The maximum number of Layers is %2.\n\n"
                "Please remove excess Layers from this level or save file into LVLX format.")
             .arg(LvlData.layers.size()).arg(100), QMessageBox::Ok);
            isSMBX64limit=true;
        }
        //Events limits
        if(LvlData.events.size()>100)
        {
            QMessageBox::warning(this, tr("The SMBX64 limit has been exceeded"),
             tr("SMBX64 standard isn't allow to save %1 Events\n"
                "The maximum number of Events is %2.\n\n"
                "Please remove excess Events from this level or save file into LVLX format.")
             .arg(LvlData.events.size()).arg(100), QMessageBox::Ok);
            isSMBX64limit=true;
        }

        if(isSMBX64limit)
        {
            QApplication::restoreOverrideCursor();
            return false;
        }

        //set SMBX64 specified option to BGO
        for(int q=0; q< LvlData.bgo.size(); q++)
        {
            if(LvlData.bgo[q].smbx64_sp < 0)
            {
                if( LvlData.bgo[q].id < (unsigned long) MainWinConnect::pMainWin->configs.index_bgo.size() )
                    LvlData.bgo[q].smbx64_sp_apply = MainWinConnect::pMainWin->configs.index_bgo[LvlData.bgo[q].id].smbx64_sp;
            }
            else
                LvlData.bgo[q].smbx64_sp_apply = LvlData.bgo[q].smbx64_sp;
            //WriteToLog(QtDebugMsg, QString("BGO SMBX64 sort -> ID-%1 SORT-%2").arg(LvlData.bgo[q].id).arg(LvlData.bgo[q].smbx64_sp) );
        }
        LvlData.smbx64strict = true; //Enable SMBX64 standard strict mode

        QFile file(fileName);
        if (!file.open(QFile::WriteOnly | QFile::Text))
        {
            QMessageBox::warning(this, tr("File save error"),
                                 tr("Cannot save file %1:\n%2.")
                                 .arg(fileName)
                                 .arg(file.errorString()));
            return false;
        }

        QTextStream out(&file);
        out << FileFormats::WriteSMBX64LvlFile(LvlData);
        file.close();
        GlobalSettings::savePath = QFileInfo(fileName).path();
    }
    // //////////////////////////////////////////////////////////////////////

    // ////////////////// Write Extended LVL file (LVLX)/////////////////////
    else if(fileName.endsWith(".lvlx", Qt::CaseInsensitive))
    {
        LvlData.smbx64strict = false; //Disable strict mode

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
        out << FileFormats::WriteExtendedLvlFile(LvlData);
        file.close();
        GlobalSettings::savePath = QFileInfo(fileName).path();
    }
    // //////////////////////////////////////////////////////////////////////

    QApplication::restoreOverrideCursor();
    setCurrentFile(fileName);


    LvlData.modified = false;
    LvlData.untitled = false;
    if(addToRecent)
    {
        MainWinConnect::pMainWin->AddToRecentFiles(fileName);
        MainWinConnect::pMainWin->SyncRecentFiles();
    }

    return true;
}


bool leveledit::loadFile(const QString &fileName, LevelData FileData, dataconfigs &configs, LevelEditingSettings options)
{
    QFile file(fileName);
    LvlData = FileData;
    LvlData.modified = false;
    LvlData.untitled = false;
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Read file error"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }
    StartLvlData = LvlData; //Save current history for made reset
    setCurrentFile(fileName);

    ui->graphicsView->setBackgroundBrush(QBrush(Qt::darkGray));

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
    scene = new LvlScene(configs, LvlData);

    scene->opts = options;

    int DataSize=0;

    DataSize += 3;
    DataSize += 6; /*LvlData.sections.size()*2;
    DataSize += LvlData.bgo.size();
    DataSize += LvlData.blocks.size();
    DataSize += LvlData.npc.size();
    DataSize += LvlData.water.size();
    DataSize += LvlData.doors.size();*/

    QProgressDialog progress(tr("Loading level data"), tr("Abort"), 0, DataSize, this);
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

    setCurrentFile(fileName);
    LvlData.modified = false;
    LvlData.untitled = false;

    progress.deleteLater();

    return true;
}


void leveledit::documentWasModified()
{
    //LvlData.modified = true;
}

bool leveledit::maybeSave()
{
    if (LvlData.modified) {
        SavingNotificationDialog* sav = new SavingNotificationDialog(true);
        sav->setSavingTitle(tr("'%1' has been modified.\n"
                               "Do you want to save your changes?").arg(userFriendlyCurrentFile()));
        sav->setWindowTitle(userFriendlyCurrentFile()+tr(" not saved"));
        QLineEdit* lvlNameBox = new QLineEdit();
        sav->addUserItem(tr("Level title: "),lvlNameBox);
        sav->setAdjustSize(400,150);
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
QString leveledit::userFriendlyCurrentFile()
{
    return strippedName(curFile);
}

void leveledit::closeEvent(QCloseEvent *event)
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

        if(MainWinConnect::pMainWin->subWins()<=1) //Stop music only if this subwindow - last
        {
            LvlMusPlay::musicForceReset = true;
            MainWinConnect::pMainWin->setMusicButton(false);
            MainWinConnect::pMainWin->setMusic(false);
        }

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

void leveledit::setCurrentFile(const QString &fileName)
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

QString leveledit::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

