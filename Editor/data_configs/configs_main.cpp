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

#include "data_configs.h"

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

bool dataconfigs::loadconfigs(/*bool nobar*/)
{
    //unsigned long i;//, prgs=0;
    total_data=0;
    config_dir = QApplication::applicationDirPath() + "/" +  "configs/SMBX/";

    //dirs
    if((!QDir(config_dir).exists())||(QFileInfo(config_dir).isFile()))
    {
        WriteToLog(QtCriticalMsg, QString("CONFIG DIR NOT EXIST %1").arg(config_dir));
        return false;
    }

    QString dirs_ini = config_dir + "main.ini";
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


    ////////////////////////////////Preparing////////////////////////////////////////
    bgoPath =   dirs.glevel +  "background/";
    BGPath =    dirs.glevel +  "background2/";
    blockPath = dirs.glevel +  "block/";
    npcPath =   dirs.glevel +  "npc/";


    //////////////////////////////////////////////////////////////////////////////////


    //QProgressDialog progress("Loading BackGround Data", "Abort", 0, total_data);

    /*
    if(!nobar)
        {
             progress.setWindowTitle("Loading config...");
             progress.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint| Qt::WindowStaysOnTopHint);
             progress.setFixedSize(progress.size());
             progress.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, progress.size(),
                    qApp->desktop()->availableGeometry()));
             progress.setCancelButton(0);
        ////////////////////////////////Preparing////////////////////////////////////////

         progress.setLabelText("Loading BackGround Data");
        }
        else progress.close();
    */

    ///////////////////////////////////////BackGround////////////////////////////////////////////
    loadLevelBackgrounds();
    ///////////////////////////////////////BackGround////////////////////////////////////////////

    //progress.setLabelText("Loading BGO Data");
    ///////////////////////////////////////BGO////////////////////////////////////////////
    loadLevelBGO();
    ///////////////////////////////////////BGO////////////////////////////////////////////

    //progress.setLabelText("Loading Block Data");
    ///////////////////////////////////////Block////////////////////////////////////////////
    loadLevelBlocks();
    ///////////////////////////////////////Block////////////////////////////////////////////

    ///////////////////////////////////////NPC////////////////////////////////////////////
    loadLevelNPC();
    ///////////////////////////////////////NPC////////////////////////////////////////////


    //progress.setLabelText("Loading Music Data");
    ///////////////////////////////////////Music////////////////////////////////////////////
    loadMusic();
    ///////////////////////////////////////Music////////////////////////////////////////////

    /*if((!progress.wasCanceled())&&(!nobar))
        progress.close();*/

    return true;
}


