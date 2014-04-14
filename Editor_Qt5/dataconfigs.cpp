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
#include <QPixmap>
#include <QBitmap>
#include <QSettings>
#include <QProgressDialog>
#include <QApplication>
#include "dataconfigs.h"
#include "mainwindow.h"
#include <QDebug>

dataconfigs::dataconfigs()
{

}

/*
[background-1]
name="Smallest bush"		;background name, default="background-%n"
type="scenery"			;Background type, default="Scenery"
grid=32				; 32 | 16 Default="32"
view=background			; background | foreground, default="background"
image="background-1.gif"	;Image file with level file ; the image mask will be have *m.gif name.
climbing=0			; default = 0
animated = 0			; default = 0 - no
frames = 1			; default = 1
frame-speed=125			; default = 125 ms, etc. 8 frames per sec
*/

void dataconfigs::loadconfigs(QWidget * window, bool nobar)
{
    unsigned long total_data=0, i, prgs=0;
    QString config_dir = QApplication::applicationDirPath() + "/" +  "configs/SMBX/";

    //dirs

    QString dirs_ini = config_dir + "lvl_bgo.ini";
    QSettings dirset(dirs_ini, QSettings::IniFormat);

    dirset.beginGroup("main");
        dirs.worlds = QApplication::applicationDirPath() + "/" + dirset.value("worlds", "worlds").toString() + "/";

        dirs.music = QApplication::applicationDirPath() + "/" + dirset.value("music", "data/music").toString() + "/";
        dirs.sounds = QApplication::applicationDirPath() + "/" + dirset.value("sound", "data/sound").toString() + "/";

        dirs.glevel = QApplication::applicationDirPath() + "/" + dirset.value("graphics-level", "data/graphics/level").toString() + "/";
        dirs.gworld= QApplication::applicationDirPath() + "/" + dirset.value("graphics-worldmap", "data/graphics/worldmap").toString() + "/";
        dirs.gplayble = QApplication::applicationDirPath() + "/" + dirset.value("graphics-characters", "data/graphics/characters").toString() + "/";

        dirs.gcustom = QApplication::applicationDirPath() + "/" + dirset.value("custom-data", "data-custom").toString() + "/";
    dirset.endGroup();


    QBitmap mask;
    //QPixmap image;

    QString imgFile, imgFileM;
    QString tmpstr;
    QStringList tmp;

    ////////////////////////////////Preparing////////////////////////////////////////
    QString bgoPath = dirs.glevel + "background/";
    QString BGPath = dirs.glevel + "background2/";
    //QString blockPath = dirs.glevel + "block/";
    //QString npcPath = dirs.glevel + "npc/";

    //BackGrounds
    obj_BG sbg;
    unsigned long bg_total=0;

    QString bg_ini = config_dir + "lvl_bkgrd.ini";
    QSettings bgset(bg_ini, QSettings::IniFormat);
    main_bg.clear();   //Clear old

    bgset.beginGroup("background2-main");
        bg_total = bgset.value("total", "0").toInt();
        total_data +=bg_total;
    bgset.endGroup();

    //qDebug() << "BG Total: "+QString::number(bg_total)+"\nDir: "+BGPath;

    //BGO
    obj_bgo sbgo;
    unsigned long bgo_total=0;
    QString bgo_ini = config_dir + "lvl_bgo.ini";
    QSettings bgoset(bgo_ini, QSettings::IniFormat);
    main_bgo.clear();   //Clear old

    bgoset.beginGroup("background-main");
        bgo_total = bgoset.value("total", "0").toInt();
        total_data +=bgo_total;
    bgoset.endGroup();

    //////////////////////////////////////////////////////////////////////////////////


    QProgressDialog progress("Loading BackGround Data", "Abort", 0, total_data);

    if(!nobar)
    {
         progress.setWindowTitle("Loading config...");
         progress.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint| Qt::WindowStaysOnTopHint);
         progress.setFixedSize(progress.size());
         progress.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, progress.size(),
				qApp->desktop()->availableGeometry()));
         progress.setCancelButton(0);
    ////////////////////////////////Preparing////////////////////////////////////////

     ///////////////////////////////////////BackGround////////////////////////////////////////////
     progress.setLabelText("Loading BackGround Data");
    }
    else progress.close();

     for(i=1; i<=bg_total; i++)
     {
         bgset.beginGroup( QString("background2-"+QString::number(i)) );
             sbg.name = bgset.value("name", "").toString();
             tmpstr = bgoset.value("type", "single-row").toString();
                 if(tmpstr=="single-row")
                    sbg.type = 0;
                 else if(tmpstr=="double-row")
                    sbg.type = 2;
                 else if(tmpstr=="tiled")
                    sbg.type = 2;
                 else sbg.type = 0;
             sbg.repeat_h = bgset.value("repeat-h", "2").toInt();
             tmpstr = bgset.value("repeat-v", "NR").toString();
                 if(tmpstr=="NR")
                     sbg.repead_v = 0;
                 else if(tmpstr=="ZR")
                     sbg.repead_v = 1;
                 else if(tmpstr=="RP")
                     sbg.repead_v = 2;
                 else if(tmpstr=="RZ")
                     sbg.repead_v = 3;
                 else sbg.repead_v = 0;

             imgFile = bgset.value("image", "").toString();
             sbg.image_n = imgFile;
             if( (imgFile!="") )
             {
                 sbg.image = QPixmap(BGPath + imgFile);
             }
             else
             {
                 sbg.image = QPixmap(QApplication::applicationDirPath() + "/" + "data/nobg.gif");
             }

             sbg.attached = (int)(bgset.value("attached", "bottom").toString()=="top");

             sbg.magic = (bgset.value("magic", "0").toString()=="1");
             sbg.magic_strips = bgset.value("magic-strips", "1").toInt();
             sbg.magic_splits = bgset.value("magic-splits", "0").toString();
             sbg.magic_speeds = bgset.value("magic-speeds", "0").toString();

             sbg.animated = (bgset.value("animated", "0").toString()=="1");//animated
             sbg.frames = bgset.value("frames", "1").toInt();
             //frames

             if(sbg.type==2)
             {

                     imgFile = bgset.value("second-image", "").toString();
                     sbg.second_image_n = imgFile;
                     if( (imgFile!="") )
                     {
                         sbg.second_image = QPixmap(BGPath + imgFile);
                     }
                     else
                     {
                         sbg.second_image = QPixmap(QApplication::applicationDirPath() + "/" + "data/nobg.gif");
                     }
                     sbg.second_repeat_h = bgset.value("second-repeat-h", "2").toInt();
                     tmpstr = bgset.value("second-repeat-v", "NR").toString();
                         if(tmpstr=="NR")
                             sbg.second_repead_v = 0;
                         else if(tmpstr=="ZR")
                             sbg.second_repead_v = 1;
                         else if(tmpstr=="RP")
                             sbg.second_repead_v = 2;
                         else if(tmpstr=="RZ")
                             sbg.second_repead_v = 3;
                         else sbg.second_repead_v = 0;
                     tmpstr = bgset.value("second-attached", "overfirst").toString();
                         if(tmpstr=="overfirst")
                             sbg.second_repead_v = 0;
                         else if(tmpstr=="bottom")
                             sbg.second_repead_v = 1;
                         else if(tmpstr=="top")
                             sbg.second_repead_v = 2;
                         else sbg.second_repead_v = 0;
             }

             if(sbg.animated)
             {
                 sbg.image=sbg.image.copy(0, 0, sbg.image.width(), (int)round(sbg.image.height()/sbg.frames));
             }
             sbg.id = i;

             main_bg.push_back(sbg);
         bgset.endGroup();

         prgs++;
         if((!progress.wasCanceled())&&(!nobar))
             progress.setValue(prgs);
     }
     ///////////////////////////////////////BackGround////////////////////////////////////////////


    ///////////////////////////////////////BGO////////////////////////////////////////////
    progress.setLabelText("Loading BGO Data");

    for(i=1; i<=bgo_total; i++)
    {
        bgoset.beginGroup( QString("background-"+QString::number(i)) );
            sbgo.name = bgoset.value("name", "").toString();
            sbgo.type = bgoset.value("type", "other").toString();
            sbgo.grid = bgoset.value("grid", "32").toInt();
            sbgo.view = (int)(bgoset.value("view", "background").toString()=="foreground");
            imgFile = bgoset.value("image", "").toString();
            sbgo.image_n = imgFile;
            if( (imgFile!="") )
            {
                tmp = imgFile.split(".", QString::SkipEmptyParts);
                if(tmp.size()==2)
                    imgFileM = tmp[0] + "m." + tmp[1];
                else
                    imgFileM = "";
                sbgo.mask_n = imgFileM;
                if(tmp.size()==2) mask = QBitmap(bgoPath + imgFileM);
                sbgo.mask = mask;
                sbgo.image = QPixmap(bgoPath + imgFile);
                if(tmp.size()==2) sbgo.image.setMask(mask);
            }
            else
            {
                sbgo.image = QPixmap(QApplication::applicationDirPath() + "/" + "data/unknown_bgo.gif");
                sbgo.mask_n = "";
            }
            sbgo.climbing = (bgoset.value("climbing", "0").toString()=="1");
            sbgo.animated = (bgoset.value("animated", "0").toString()=="1");
            sbgo.frames = bgoset.value("frames", "1").toInt();
            sbgo.framespeed = bgoset.value("frame-speed", "125").toInt();
            sbgo.id = i;
            main_bgo.push_back(sbgo);
        bgoset.endGroup();

        prgs++;
        if((!progress.wasCanceled())&&(!nobar))
            progress.setValue(prgs);
    }
    ///////////////////////////////////////BGO////////////////////////////////////////////

    if((!progress.wasCanceled())&&(!nobar))
        progress.close();

}
