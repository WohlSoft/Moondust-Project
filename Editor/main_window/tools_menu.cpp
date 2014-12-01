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

#include "../common_features/app_path.h"

#include "../smart_import/smartimporter.h"

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
    //QMessageBox::information(this, "Dummy", "This feature comming soon!", QMessageBox::Ok);
    if(activeChildWindow()==1)
    {
        QStringList filesForRemove;
        leveledit * box = activeLvlEditWin();
        if(!box->sceneCreated) return;

        LvlScene * s = box->scene;
        QString levelDirectory(box->LvlData.path);

        QString levelCustomDirectory(box->LvlData.path+"/"+box->LvlData.filename);
        QDir levelCustomDirectory_dir(levelCustomDirectory);

        filesForRemove.append( levelCustomDirectory_dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot) );

        qDebug() << levelCustomDirectory << "Total custom files" << filesForRemove.size();


        QRegExp reg;
        reg.setPatternSyntax(QRegExp::Wildcard);
        reg.setCaseSensitivity(Qt::CaseInsensitive);


        //SMBX64 entries
        QStringList whiteList;
        whiteList << "lunadll.txt"; //LunaDLL stuff
        whiteList << "lunadll.lua";

        whiteList << "*.tileset.ini"; //PGE Tilesets

        whiteList << "effect-*.gif"; //All SMBX64 effects

        whiteList << "mario-*.gif"; //All SMBX64 playable characters
        whiteList << "peach-*.gif";
        whiteList << "toad-*.gif";
        whiteList << "luigi-*.gif";
        whiteList << "link-*.gif";
        whiteList << "yoshit-*.gif";
        whiteList << "yoshib-*.gif";


        QString npcFile = "npc-%1.gif|npc-%1.txt|npc-%1m.txt";

        typedef QPair<unsigned long, QStringList > bossNPC;
        QList<bossNPC > bossNPCs;

        bossNPCs << bossNPC(15, npcFile.arg(16).split('|') ); //boom-boom
        bossNPCs << bossNPC(39, npcFile.arg(41).split('|') ); //Birdo

        //giant coopa
        bossNPCs << bossNPC(72, npcFile.arg(73).split('|') );

        //green parakoopa
        bossNPCs << bossNPC(76, npcFile.arg(4).split('|') );
        bossNPCs << bossNPC(76, npcFile.arg(5).split('|') );
        bossNPCs << bossNPC(4, npcFile.arg(5).split('|') );

        bossNPCs << bossNPC(176, npcFile.arg(173).split('|') );
        bossNPCs << bossNPC(176, npcFile.arg(172).split('|') );
        bossNPCs << bossNPC(173, npcFile.arg(172).split('|') );

        //red parakoopa
        bossNPCs << bossNPC(161, npcFile.arg(6).split('|') );
        bossNPCs << bossNPC(161, npcFile.arg(7).split('|') );
        bossNPCs << bossNPC(6, npcFile.arg(7).split('|') );

        bossNPCs << bossNPC(177, npcFile.arg(175).split('|') );
        bossNPCs << bossNPC(177, npcFile.arg(174).split('|') );
        bossNPCs << bossNPC(175, npcFile.arg(174).split('|') );

        //Blue parakoopa
        bossNPCs << bossNPC(111, npcFile.arg(115).split('|') );
        bossNPCs << bossNPC(111, npcFile.arg(55).split('|') );
        bossNPCs << bossNPC(111, npcFile.arg(119).split('|') );

        bossNPCs << bossNPC(123, npcFile.arg(111).split('|') );
        bossNPCs << bossNPC(123, npcFile.arg(115).split('|') );
        bossNPCs << bossNPC(123, npcFile.arg(55).split('|') );
        bossNPCs << bossNPC(123, npcFile.arg(119).split('|') );

        //Red parakoopa
        bossNPCs << bossNPC(110, npcFile.arg(118).split('|') );
        bossNPCs << bossNPC(110, npcFile.arg(114).split('|') );

        bossNPCs << bossNPC(122, npcFile.arg(110).split('|') );
        bossNPCs << bossNPC(123, npcFile.arg(118).split('|') );
        bossNPCs << bossNPC(123, npcFile.arg(114).split('|') );

        //Green parakoopa
        bossNPCs << bossNPC(109, npcFile.arg(113).split('|') );//shell
        bossNPCs << bossNPC(109, npcFile.arg(117).split('|') );//beach

        bossNPCs << bossNPC(121, npcFile.arg(109).split('|') );//koopa
        bossNPCs << bossNPC(121, npcFile.arg(113).split('|') );//shell
        bossNPCs << bossNPC(121, npcFile.arg(117).split('|') );//beach

        //Yellow parakoopa
        bossNPCs << bossNPC(112, npcFile.arg(116).split('|') );//shell
        bossNPCs << bossNPC(112, npcFile.arg(120).split('|') );//beach

        bossNPCs << bossNPC(124, npcFile.arg(112).split('|') );//koopa
        bossNPCs << bossNPC(124, npcFile.arg(116).split('|') );//shell
        bossNPCs << bossNPC(124, npcFile.arg(120).split('|') );//beach

        bossNPCs << bossNPC(124, npcFile.arg(194).split('|') );
        bossNPCs << bossNPC(112, npcFile.arg(194).split('|') );
        bossNPCs << bossNPC(116, npcFile.arg(194).split('|') );


        //Add into white SMBX64 dependencied of exists ID
        for(int q=0;q<box->LvlData.npc.size();q++)
        {
            for(int r=0;r<bossNPCs.size();r++)
                if(box->LvlData.npc[q].id==bossNPCs[r].first)
                {
                    whiteList << bossNPCs[r].second;
                    bossNPCs.removeAt(r);
                    r=-1;//reset loot to 0
                }
            if(bossNPCs.isEmpty()) break;
        }


        //SMBX64 system items
        whiteList << npcFile.arg(40).split('|'); //bullets
        whiteList << npcFile.arg(85).split('|');
        whiteList << npcFile.arg(87).split('|');
        whiteList << npcFile.arg(159).split('|');
        whiteList << npcFile.arg(13).split('|');
        whiteList << npcFile.arg(30).split('|');
        whiteList << npcFile.arg(171).split('|');
        whiteList << npcFile.arg(263).split('|');
        whiteList << npcFile.arg(237).split('|');
        whiteList << npcFile.arg(265).split('|');
        whiteList << npcFile.arg(269).split('|');
        whiteList << npcFile.arg(368).split('|');
        whiteList << npcFile.arg(266).split('|');
        whiteList << npcFile.arg(282).split('|');
        whiteList << npcFile.arg(281).split('|');
        whiteList << npcFile.arg(191).split('|');
        whiteList << npcFile.arg(210).split('|');
        whiteList << npcFile.arg(133).split('|');
        whiteList << npcFile.arg(292).split('|');
        whiteList << npcFile.arg(50).split('|');
        whiteList << npcFile.arg(246).split('|');
        whiteList << npcFile.arg(276).split('|');
        whiteList << npcFile.arg(202).split('|');
        whiteList << npcFile.arg(108).split('|');

        //shoes
        whiteList << npcFile.arg(35).split('|');
        whiteList << npcFile.arg(191).split('|');
        whiteList << npcFile.arg(193).split('|');

        //Yoshis
        whiteList << npcFile.arg(95).split('|');
        whiteList << npcFile.arg(98).split('|');
        whiteList << npcFile.arg(99).split('|');
        whiteList << npcFile.arg(100).split('|');
        whiteList << npcFile.arg(148).split('|');
        whiteList << npcFile.arg(149).split('|');
        whiteList << npcFile.arg(150).split('|');
        whiteList << npcFile.arg(228).split('|');


        whiteList << "background-98.gif"; //door lock
        whiteList << "background-98m.gif";
        whiteList << "background-160.gif"; //star lock
        whiteList << "background-160m.gif";


        for(int i=0; i < filesForRemove.size(); i++)
        {
            bool removed=false;
            if(QFileInfo(levelCustomDirectory+"/"+filesForRemove[i]).isDir())
                {filesForRemove.removeAt(i); i--; continue;}
            else
            foreach(QString f, whiteList)
            {
                reg.setPattern(f);
                if(reg.exactMatch(filesForRemove[i]))
                    {filesForRemove.removeAt(i); i--; removed=true; break;}
            }

            if(!removed)
                filesForRemove[i] = levelCustomDirectory+"/"+filesForRemove[i];
        }



        //Get custom data list
        foreach(LevelSection y, box->LvlData.sections)
        {
            if(y.music_file.isEmpty()) continue;

            bool musicFileRemoved = !QFileInfo(levelDirectory+"/"+y.music_file).exists();

            for(int i=0; i < filesForRemove.size(); i++)
            {
                if(musicFileRemoved) break;

                if( filesForRemove[i].toLower()==(levelDirectory+"/"+y.music_file).toLower() )
                {filesForRemove.removeAt(i); i--; musicFileRemoved=true;}
            }
        }

        foreach(UserBGs x, s->uBGs)
        {
            foreach(LevelSection y, box->LvlData.sections)
            {
                if(y.background == x.id)
                {

                    QString image;

                    long I = configs.getBgI(x.id);
                    if(I>=0)
                    {
                        image = levelCustomDirectory + "/" + configs.main_bg[I].image_n;
                    }
                    bool imageRemoved = !QFileInfo(image).exists();

                    for(int i=0; i < filesForRemove.size(); i++)
                    {
                        if(imageRemoved) break;

                        if(filesForRemove[i].toLower()==image.toLower())
                        {filesForRemove.removeAt(i); i--; imageRemoved=true;}
                    }
                    break;
                }
            }
        }

        foreach(UserBlocks x, s->uBlocks)
        {
            foreach(LevelBlock y, box->LvlData.blocks)
            {
                if(y.id == x.id)
                {

                    QString image;
                    QString mask;
                    long transformTo=0;
                    QString transformTo_image;
                    QString transformTo_mask;

                    long I = configs.getBlockI(x.id);
                    if(I>=0)
                    {
                        image = levelCustomDirectory + "/" + configs.main_block[I].image_n;
                        mask = levelCustomDirectory + "/" + configs.main_block[I].mask_n;
                        transformTo = configs.main_block[I].transfororm_on_hit_into;
                    }

                    if(transformTo>0)
                    {
                        I = configs.getBlockI(transformTo);
                        if(I>=0)
                        {
                            transformTo_image = levelCustomDirectory + "/" + configs.main_block[I].image_n;
                            transformTo_mask = levelCustomDirectory + "/" + configs.main_block[I].mask_n;
                        }
                    }

                    bool imageRemoved = !QFileInfo(image).exists();
                    bool maskRemoved = !QFileInfo(mask).exists();
                    bool transformTo_img = !QFileInfo(transformTo_image).exists();;
                    bool transformTo_msk = !QFileInfo(transformTo_mask).exists();;

                    for(int i=0; i < filesForRemove.size(); i++)
                    {
                        if(imageRemoved && maskRemoved &&
                           transformTo_img && transformTo_msk) break;

                        if(filesForRemove[i].toLower()==image.toLower())
                        {filesForRemove.removeAt(i); i--; imageRemoved=true;}
                        else
                        if(filesForRemove[i].toLower()==mask.toLower())
                        {filesForRemove.removeAt(i); i--;maskRemoved=true;}
                        else
                        if(filesForRemove[i].toLower()==transformTo_image.toLower())
                        {filesForRemove.removeAt(i); i--;transformTo_img=true;}
                        else
                        if(filesForRemove[i].toLower()==transformTo_mask.toLower())
                        {filesForRemove.removeAt(i); i--;transformTo_msk=true;}
                    }
                    break;
                }
            }

        }

        foreach(UserBGOs x, s->uBGOs)
        {
            foreach(LevelBGO y, box->LvlData.bgo)
            {
                if(y.id == x.id)
                {

                    QString image;
                    QString mask;
                    long I = configs.getBgoI(x.id);
                    if(I>=0)
                    {
                        image = levelCustomDirectory + "/" + configs.main_bgo[I].image_n;
                        mask = levelCustomDirectory + "/" + configs.main_bgo[I].mask_n;
                    }
                    bool imageRemoved = !QFileInfo(image).exists();
                    bool maskRemoved = !QFileInfo(mask).exists();
                    for(int i=0; i < filesForRemove.size(); i++)
                    {
                        if(imageRemoved && maskRemoved) break;

                        if(filesForRemove[i].toLower()==image.toLower())
                        {filesForRemove.removeAt(i); i--; imageRemoved=true;}
                        else
                        if(filesForRemove[i].toLower()==mask.toLower())
                        {filesForRemove.removeAt(i); i--;maskRemoved=true;}
                    }
                    break;
                }
            }

        }



        foreach(UserNPCs x, s->uNPCs)
        {
            QList<unsigned long > usedIDs;

            //Used in level
            foreach(LevelNPC y, box->LvlData.npc)
            {
                if(y.id==x.id)
                    usedIDs.push_back(y.id);
            }

            //included into blocks
            foreach(LevelBlock y, box->LvlData.blocks)
            {
                if((y.npc_id>0) && ((unsigned long)y.npc_id == x.id))
                    usedIDs.push_back(y.id);
            }

            foreach(unsigned long npcID, usedIDs)
            {
                QString image;
                QString mask;
                QString textConfig;
                long I = configs.getNpcI(npcID);
                if(I>=0)
                {
                    if(x.withTxt)
                    {
                        image = levelCustomDirectory + "/" + x.merged.image_n;
                        mask = levelCustomDirectory + "/" + x.merged.mask_n;
                    }
                    else
                    {
                        image = levelCustomDirectory + "/" + configs.main_npc[I].image_n;
                        mask = levelCustomDirectory + "/" + configs.main_npc[I].mask_n;
                    }
                    textConfig = levelCustomDirectory + "/npc-"+QString::number(x.id)+".txt";
                }

                bool imageRemoved = !QFileInfo(image).exists();
                bool maskRemoved = !QFileInfo(mask).exists();
                bool textRemoved = !QFileInfo(textConfig).exists();

                for(int i=0; i < filesForRemove.size(); i++)
                {
                    if(imageRemoved && maskRemoved && textRemoved) break;

                    if(filesForRemove[i].toLower()==image.toLower())
                    {filesForRemove.removeAt(i); i--; imageRemoved=true;}
                    else
                    if(filesForRemove[i].toLower()==mask.toLower())
                    {filesForRemove.removeAt(i); i--;maskRemoved=true;}
                    else
                    if(filesForRemove[i].toLower()==textConfig.toLower())
                    {filesForRemove.removeAt(i); i--;textRemoved=true;}
                }
                break;
            }
        }

        if(filesForRemove.isEmpty())
        {
            QMessageBox::information(this, "No unused fules",
                                     "This level haven't unused custom data",
                                     QMessageBox::Ok );
            return;
        }


        //Ask for removing

        /*
         *  Here add dialog with QListWidget where user will check/uncheck data for removing
        */

        if(QMessageBox::question(this, "Files to remove",
                                 "Found "+QString::number(filesForRemove.size())+" unused files"
                                 "\n\nDo you wanna remove them?",
                              QMessageBox::Yes, QMessageBox::No ) != QMessageBox::Yes )
            return;

        //Remove selected
        foreach(QString f, filesForRemove)
        {
            QFile x(f);
            if(x.exists())
                x.remove();
        }

        QMessageBox::information(this, "Unused files removed",
                                 "All unused files successfully removed from custom directory!",
                                 QMessageBox::Ok );
    }
}


void MainWindow::on_actionCDATA_Import_triggered()
{

    QString sourceDir = ApplicationPath;
    if(activeChildWindow()==1)
    {
        sourceDir = QFileInfo(activeLvlEditWin()->curFile).absoluteDir().path();
    }
    else
        return;

    //QMessageBox::information(this, "Dummy", "This feature comming soon!", QMessageBox::Ok);
    QString fileName = QFileDialog::getExistingDirectory(this, tr("Select directory with custom data to import"),
                                                 sourceDir,
                                                 QFileDialog::DontResolveSymlinks);
    if(fileName.isEmpty()) return;

    this->setFocus();
    this->raise();
    qApp->setActiveWindow(this);

    if(activeChildWindow()==1)
    {
        leveledit * box = activeLvlEditWin();
        if(QFileInfo(fileName).isDir())
        {
            SmartImporter * importer = new SmartImporter((QWidget*)box, fileName, (QWidget*)box);
            if(importer->isValid())
            {
                if(importer->attemptFastImport())
                {
                    qDebug()<<"Imported!";
                    on_actionReload_triggered();
                }
                else qDebug()<<"Import fail";
            }
            delete importer;
        }
    }
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
        if(e->isUntitled)
        {
            QMessageBox::warning(this, tr("File is untitled"),
                tr("File doesn't use custom graphics.\n"));
            return;
        }
        path = e->LvlData.path + "/";
        path_custom = e->LvlData.path+"/"+e->LvlData.filename+"/";

        OPath = path;
        OPath_custom = path_custom;
        doIt=true;
    }
    else
    if(activeChildWindow()==3)
    {
        WorldEdit *e = activeWldEditWin();
        if(e->isUntitled)
        {
            QMessageBox::warning(this, tr("File is untitled"),
                tr("File doesn't use custom graphics.\n"));
            return;
        }
        path = e->WldData.path + "/";
        path_custom = e->WldData.path+"/"+e->WldData.filename+"/";

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
        if(fileList.size()+fileList_custom.size() <= 0)
        {
            QMessageBox::warning(this, tr("Nothing to do"),
                tr("This file is not use GIF graphics with transparent masks or haven't custom graphics."));
            return;
        }

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
