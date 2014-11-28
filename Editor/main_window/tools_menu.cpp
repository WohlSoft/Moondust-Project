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

#include <ui_mainwindow.h>
#include "../mainwindow.h"

#include "../tilesets/tilesetconfiguredialog.h"
#include "../tilesets/tilesetgroupeditor.h"
#include "../npc_dialog/npcdialog.h"
#include <QDesktopServices>
#include "../dev_console/devconsole.h"

#include "../external_tools/lazyfixtool_gui.h"
#include "../external_tools/gifs2png_gui.h"
#include "../external_tools/png2gifs_gui.h"

#include "../common_features/graphics_funcs.h"

void MainWindow::on_actionConfigure_Tilesets_triggered()
{
    TilesetConfigureDialog* tilesetConfDia = new TilesetConfigureDialog(&configs, NULL);
    tilesetConfDia->exec();
    delete tilesetConfDia;

    configs.loadTilesets();
    setTileSetBox();
}


void MainWindow::on_actionTileset_groups_editor_triggered()
{
    TilesetGroupEditor * groupDialog;
    if(activeChildWindow()==1)
        groupDialog = new TilesetGroupEditor(activeLvlEditWin()->scene, this);
    else if(activeChildWindow()==3)
        groupDialog = new TilesetGroupEditor(activeWldEditWin()->scene, this);
    else
        groupDialog = new TilesetGroupEditor(NULL, this);
    groupDialog->exec();
    delete groupDialog;

    configs.loadTilesets();
    setTileSetBox();
}


void MainWindow::on_actionShow_Development_Console_triggered()
{
    DevConsole::show();
    DevConsole::log("Showing DevConsole!","View");
}

LazyFixTool_gui * lazyfixGUI;
void MainWindow::on_actionLazyFixTool_triggered()
{
    if(lazyfixGUI)
    {
        lazyfixGUI->show();
        lazyfixGUI->raise();
        lazyfixGUI->setFocus();
        return;
    }
    lazyfixGUI = new LazyFixTool_gui;
    lazyfixGUI->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    lazyfixGUI->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, lazyfixGUI->size(), qApp->desktop()->availableGeometry()));
    lazyfixGUI->show();
}

gifs2png_gui * gifToPngGUI;
void MainWindow::on_actionGIFs2PNG_triggered()
{
    if(gifToPngGUI)
    {
        gifToPngGUI->show();
        gifToPngGUI->raise();
        gifToPngGUI->setFocus();
        return;
    }
    gifToPngGUI = new gifs2png_gui;
    gifToPngGUI->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    gifToPngGUI->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, gifToPngGUI->size(), qApp->desktop()->availableGeometry()));
    gifToPngGUI->show();
}

png2gifs_gui * pngToGifGUI;
void MainWindow::on_actionPNG2GIFs_triggered()
{
    if(pngToGifGUI)
    {
        pngToGifGUI->show();
        pngToGifGUI->raise();
        pngToGifGUI->setFocus();
        return;
    }
    pngToGifGUI = new png2gifs_gui;
    pngToGifGUI->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    pngToGifGUI->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, pngToGifGUI->size(), qApp->desktop()->availableGeometry()));
    pngToGifGUI->show();
}




void MainWindow::on_actionCDATA_clear_unused_triggered()
{
    QMessageBox::information(this, "Dummy", "This feature comming soon!", QMessageBox::Ok);
}


void MainWindow::on_actionCDATA_Import_triggered()
{
    QMessageBox::information(this, "Dummy", "This feature comming soon!", QMessageBox::Ok);
}

void MainWindow::on_actionSprite_editor_triggered()
{
    QMessageBox::information(this, "Dummy", "This feature comming soon!", QMessageBox::Ok);
}














void lazyFixTool_doMagicIn(QString path, QString q, QString OPath)
{
    QRegExp isMask = QRegExp("*m.gif");
    isMask.setPatternSyntax(QRegExp::Wildcard);

    QRegExp isBackupDir = QRegExp("*/_backup/");
    isBackupDir.setPatternSyntax(QRegExp::Wildcard);

    if(isBackupDir.exactMatch(path))
        return; //Skip backup directories

    if(isMask.exactMatch(q))
        return;

    QImage target;
    QString imgFileM;
    QStringList tmp = q.split(".", QString::SkipEmptyParts);
    if(tmp.size()==2)
        imgFileM = tmp[0] + "m." + tmp[1];
    else
        return;

    //skip unexists pairs
    if(!QFile(path+q).exists())
        return;

    if(!QFile(path+imgFileM).exists())
    {
        QString saveTo;

        QImage image = GraphicsHelps::loadQImage(path+q);
        if(image.isNull()) return;

        //qDebug() << QString(path+q+"\n").toUtf8().data();

        saveTo = QString(OPath+(tmp[0].toLower())+".gif");

        //overwrite source image (convert BMP to GIF)
        if(!GraphicsHelps::toGif( image, saveTo ) ) //Write gif
        {
            image.save(saveTo, "BMP"); //If failed, write BMP
        }
        return;
    }


    //create backup dir
    QDir backup(path+"_backup/");
    if(!backup.exists())
    {
        qDebug() << QString("Create backup with path %1\n").arg(path+"_backup");
        if(!backup.mkdir("."))
            qDebug() << QString("WARNING! Can't create backup directory %1\n").arg(path+"_backup");
    }

    //create Back UP of source images
    if(!QFile(path+"_backup/"+q).exists())
        QFile::copy(path+q, path+"_backup/"+q);
    if(!QFile(path+"_backup/"+imgFileM).exists())
        QFile::copy(path+imgFileM, path+"_backup/"+imgFileM);

    QImage image = GraphicsHelps::loadQImage(path+q);
    QImage mask = GraphicsHelps::loadQImage(path+imgFileM);

    if(mask.isNull()) //Skip null masks
        return;

    target = GraphicsHelps::setAlphaMask_VB(image, mask);

    if(!target.isNull())
    {
        //fix
        if(image.size()!= mask.size())
            mask = mask.copy(0,0, image.width(), image.height());

        mask = target.alphaChannel();
        mask.invertPixels();

        //Save after fix
        //target.save(OPath+tmp[0]+"_after.bmp", "BMP");
        QString saveTo;


        saveTo = QString(OPath+(tmp[0].toLower())+".gif");

        //overwrite source image (convert BMP to GIF)
        if(!GraphicsHelps::toGif(image, saveTo ) ) //Write gif
        {
            image.save(saveTo, "BMP"); //If failed, write BMP
        }

        saveTo = QString(OPath+(tmp[0].toLower())+"m.gif");

        //overwrite mask image
        if( !GraphicsHelps::toGif(mask, saveTo ) ) //Write gif
        {
            mask.save(saveTo, "BMP"); //If failed, write BMP
            //qDebug() <<"BMP-2\n";
        }
    }
    else
        qDebug() << path+q+" - WRONG FILE!\n";
}

void MainWindow::on_actionFixWrongMasks_triggered()
{
    bool doIt=false;
    QString path;
    QString OPath;

    QString path_custom;
    QString OPath_custom;

    if(activeChildWindow()==1)
    {
        leveledit *e = activeLvlEditWin();
        path = e->LvlData.path + "/";
        path_custom = e->LvlData.path+"/"+e->LvlData.filename+"/";

        OPath = path;
        OPath_custom = path_custom;
        doIt=true;
    }

    if(doIt)
    {
        QStringList filters;
        filters << "*.gif" << "*.GIF";
        QDir imagesDir;
        QDir cImagesDir;

        QStringList fileList;
        QStringList fileList_custom;

        imagesDir.setPath(path);
        cImagesDir.setPath(path_custom);

        if(imagesDir.exists())
            fileList << imagesDir.entryList(filters);

        if(cImagesDir.exists())
            fileList_custom << cImagesDir.entryList(filters);

        //qDebug() << fileList.size()+fileList_custom.size();

        QProgressDialog progress(tr("Fixing of masks..."), tr("Abort"), 0, fileList.size()+fileList_custom.size(), this);
        progress.setWindowTitle(tr("Please wait..."));
        progress.setWindowModality(Qt::WindowModal);
        progress.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
        progress.setFixedSize(progress.size());
        progress.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, progress.size(), qApp->desktop()->availableGeometry()));
        //progress.setCancelButton();
        progress.setMinimumDuration(0);

        foreach(QString q, fileList)
        {
            lazyFixTool_doMagicIn(path, q, OPath);
            progress.setValue(progress.value()+1);
            qApp->processEvents();
            if(progress.wasCanceled()) break;
        }

        if(!progress.wasCanceled())
        foreach(QString q, fileList_custom)
        {
            lazyFixTool_doMagicIn(path_custom, q, OPath_custom);
            progress.setValue(progress.value()+1);
            qApp->processEvents();
            if(progress.wasCanceled()) break;
        }


        if(!progress.wasCanceled())
        {
            progress.close();
            QMessageBox::information(this, tr("Done"),
                tr("Masks has been fixed!\nPlease reload current file to apply result."));
        }
    }

}
