/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include <QtWidgets>
#include <QGraphicsItem>
#include <QPixmap>
#include <QGraphicsScene>
#include <QProgressDialog>

#include "leveledit.h"
#include "../ui_leveledit.h"

#include "../file_formats/file_formats.h"
#include "../level_scene/lvlscene.h"
#include "saveimage.h"
#include "../common_features/logger.h"

#include "../common_features/mainwinconnect.h"
#include "../main_window/music_player.h"

#include <QDebug>


void leveledit::ExportToImage_fn()
{
        long x, y, h, w, th, tw;

        bool proportion;
        QString inifile = QApplication::applicationDirPath() + "/" + "pge_editor.ini";
        QSettings settings(inifile, QSettings::IniFormat);
        settings.beginGroup("Main");
        latest_export_path = settings.value("export-path", QApplication::applicationDirPath()).toString();
        proportion = settings.value("export-proportions", true).toBool();
        settings.endGroup();


        x=LvlData.sections[LvlData.CurSection].size_left;
        y=LvlData.sections[LvlData.CurSection].size_top;
        w=LvlData.sections[LvlData.CurSection].size_right;
        h=LvlData.sections[LvlData.CurSection].size_bottom;
        w=(long)fabs(x-w);
        h=(long)fabs(y-h);

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
            QString("%1_Section_%2.png").arg( QFileInfo(curFile).baseName() ).arg(LvlData.CurSection+1), tr("PNG Image (*.png)"));
        if (fileName.isEmpty())
            return;

        QFileInfo exported(fileName);

        QProgressDialog progress(tr("Saving section image..."), tr("Abort"), 0, 2, this);
        progress.setWindowTitle(tr("Please, wait..."));
        progress.setWindowModality(Qt::WindowModal);
        progress.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
        progress.setFixedSize(progress.size());
        progress.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, progress.size(), qApp->desktop()->availableGeometry()));
        progress.setCancelButton(0);

        progress.show();

        if(!progress.wasCanceled()) progress.setValue(0);

        if(scene->opts.animationEnabled) scene->stopAnimation(); //Reset animation to 0 frame
        scene->clearSelection(); // Clear selection on export

        latest_export_path = exported.absoluteDir().path();
        proportion = imgSize[2];

        th=imgSize[0];
        tw=imgSize[1];

        QImage img(tw,th,QImage::Format_ARGB32_Premultiplied);

        if(!progress.wasCanceled()) progress.setValue(1);

        QPainter p(&img);
        scene->render(&p, QRectF(0,0,tw,th),QRectF(x,y,w,h));
        p.end();

        img.save(fileName);
        if(!progress.wasCanceled()) progress.setValue(2);

        if(scene->opts.animationEnabled) scene->startBlockAnimation(); // Restart animation

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
    setWindowTitle(curFile);
    LvlData = FileFormats::dummyLvlDataArray();
    StartLvlData = LvlData;

    scene = new LvlScene(configs, LvlData);
    scene->opts = options;

    scene->InitSection(0);
    scene->setPlayerPoints();
    scene->drawSpace();

    if(!sceneCreated)
    {
        ui->graphicsView->setScene(scene);
        sceneCreated = true;
    }

}


bool leveledit::save()
{
    if (isUntitled) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool leveledit::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save As"),
        curFile, QString("SMBX64 (1.3) Level file (*.lvl)"));
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

bool leveledit::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Write file error"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);

    QApplication::setOverrideCursor(Qt::WaitCursor);


    // ////////////////////// Write SMBX64 LVL //////////////////////////////

    //set SMBX64 specified option to BGO
    for(int q=0; q< LvlData.bgo.size(); q++)
    {
        if(LvlData.bgo[q].smbx64_sp < 0)
        {
        if( LvlData.bgo[q].id < (unsigned long) MainWinConnect::pMainWin->configs.index_bgo.size() )
            LvlData.bgo[q].smbx64_sp = MainWinConnect::pMainWin->configs.index_bgo[LvlData.bgo[q].id].smbx64_sp;
        }
        //WriteToLog(QtDebugMsg, QString("BGO SMBX64 sort -> ID-%1 SORT-%2").arg(LvlData.bgo[q].id).arg(LvlData.bgo[q].smbx64_sp) );
    }


    out << FileFormats::WriteSMBX64LvlFile(LvlData);
    // //////////////////////////////////////////////////////////////////////

    QApplication::restoreOverrideCursor();
    setCurrentFile(fileName);

    LvlData.modified = false;

    return true;
}


bool leveledit::loadFile(const QString &fileName, LevelData FileData, dataconfigs &configs, LevelEditingSettings options)
{
    QFile file(fileName);
    LvlData = FileData;
    LvlData.modified = false;

    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Read file error"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }
    StartLvlData = LvlData; //Save current history for made reset

    //Data configs exists
    if(
            (configs.main_bgo.size()<=0)||
            (configs.main_bg.size()<=0)||
            (configs.main_block.size()<=0)||
            (configs.main_npc.size()<=0)||
            (configs.main_music_lvl.size()<=0)||
            (configs.main_music_wld.size()<=0)||
            (configs.main_music_spc.size()<=0)
      )
    {
        WriteToLog(QtCriticalMsg, QString("Error! *.INI Configs not loaded"));

        QMessageBox::warning(this, tr("Configurations not loaded"),
                             tr("Cannot open level file %1:\nbecause object configurations not loaded\n."
                                "Please, check that the config/SMBX directory exists and contains the *.INI files with object settings.")
                             .arg(fileName));

        WriteToLog(QtCriticalMsg, QString(" << close subWindow"));

        this->close();

        WriteToLog(QtCriticalMsg, QString(" << closed, return false"));
        return false;
    }

    WriteToLog(QtDebugMsg, QString(">>Starting load file"));

    //Declaring of the scene
    scene = new LvlScene(configs, LvlData);

    scene->opts = options;

    int DataSize=0;

    DataSize += LvlData.sections.size()*2;
    DataSize += configs.main_bgo.size();

    DataSize += LvlData.bgo.size();
    DataSize += configs.main_block.size();
    DataSize += LvlData.blocks.size();

    DataSize += configs.main_npc.size();
    DataSize += LvlData.npc.size();
    DataSize += LvlData.water.size();
    DataSize += LvlData.doors.size();

    QProgressDialog progress(tr("Loading level data"), tr("Abort"), 0, DataSize, this);
         progress.setWindowTitle(tr("Loading level data"));
         progress.setWindowModality(Qt::WindowModal);
         progress.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
         progress.setFixedSize(progress.size());
         progress.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, progress.size(), qApp->desktop()->availableGeometry()));
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

    ui->graphicsView->verticalScrollBar()->setValue(265+LvlData.sections[0].size_bottom-602);
    ui->graphicsView->horizontalScrollBar()->setValue(330+LvlData.sections[0].size_left);

    //ResetPosition();

    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    LvlData.modified = false;

    return true;
}


void leveledit::documentWasModified()
{
    //LvlData.modified = true;
}

bool leveledit::maybeSave()
{
    if (LvlData.modified) {
    QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, userFriendlyCurrentFile()+tr(" not saved"),
                     tr("'%1' has been modified.\n"
                        "Do you want to save your changes?")
                     .arg(userFriendlyCurrentFile()),
                     QMessageBox::Save | QMessageBox::Discard
             | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
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
        LvlMusPlay::musicForceReset = true;
        MainWinConnect::pMainWin->setMusicButton(false);
        MainWinConnect::pMainWin->setMusic(false);
        scene->uBGOs.clear();
        scene->uBGs.clear();
        scene->uBlocks.clear();
        scene->clear();
        sceneCreated=false;
        MainWinConnect::pMainWin->updateMenus(true);
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
    //document()->setModified(false);
    setWindowModified(false);
    setWindowTitle(userFriendlyCurrentFile());
}

QString leveledit::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

