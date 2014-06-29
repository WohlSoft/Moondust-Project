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

#include "data_configs.h"

#include "../main_window/global_settings.h"

long ConfStatus::total_blocks=0;
long ConfStatus::total_bgo=0;
long ConfStatus::total_bg=0;
long ConfStatus::total_npc=0;

long ConfStatus::total_music_lvl=0;
long ConfStatus::total_music_wld=0;
long ConfStatus::total_music_spc=0;
long ConfStatus::total_sound=0;

QString ConfStatus::configName="";

dataconfigs::dataconfigs()
{
    default_grid=0;
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

void dataconfigs::addError(QString bug, QtMsgType level)
{
    WriteToLog(level, QString("LoadConfig -> ")+bug);
    errorsList<<bug;
}

bool dataconfigs::loadconfigs(QProgressDialog *prgs)
{
    //unsigned long i;//, prgs=0;

    total_data=0;
    config_dir = QApplication::applicationDirPath() + "/" +  "configs/SMBX/";
    default_grid=0;
    errorsList.clear();

    //dirs
    if((!QDir(config_dir).exists())||(QFileInfo(config_dir).isFile()))
    {
        WriteToLog(QtCriticalMsg, QString("CONFIG DIR NOT FOUND AT: %1").arg(config_dir));
        return false;
    }

    QString dirs_ini = config_dir + "main.ini";
    QSettings dirset(dirs_ini, QSettings::IniFormat);
    dirset.setIniCodec("UTF-8");

    dirset.beginGroup("main");

        ConfStatus::configName = dirset.value("config_name", config_dir).toString();

        dirs.worlds = QApplication::applicationDirPath() + "/" + dirset.value("worlds", "worlds").toString() + "/";

        dirs.music = QApplication::applicationDirPath() + "/" + dirset.value("music", "data/music").toString() + "/";
        dirs.sounds = QApplication::applicationDirPath() + "/" + dirset.value("sound", "data/sound").toString() + "/";

        dirs.glevel = QApplication::applicationDirPath() + "/" + dirset.value("graphics-level", "data/graphics/level").toString() + "/";
        dirs.gworld= QApplication::applicationDirPath() + "/" + dirset.value("graphics-worldmap", "data/graphics/worldmap").toString() + "/";
        dirs.gplayble = QApplication::applicationDirPath() + "/" + dirset.value("graphics-characters", "data/graphics/characters").toString() + "/";

        dirs.gcustom = QApplication::applicationDirPath() + "/" + dirset.value("custom-data", "data-custom").toString() + "/";
    dirset.endGroup();

    dirset.beginGroup("graphics");
        default_grid = dirset.value("default-grid", 32).toInt();
    dirset.endGroup();

    if( dirset.status() != QSettings::NoError )
    {
        WriteToLog(QtCriticalMsg, QString("ERROR LOADING main.ini N:%1").arg(dirset.status()));
    }

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
    loadLevelBackgrounds(prgs);
    ///////////////////////////////////////BackGround////////////////////////////////////////////

    //progress.setLabelText("Loading BGO Data");
    ///////////////////////////////////////BGO////////////////////////////////////////////
    loadLevelBGO(prgs);
    ///////////////////////////////////////BGO////////////////////////////////////////////

    //progress.setLabelText("Loading Block Data");
    ///////////////////////////////////////Block////////////////////////////////////////////
    loadLevelBlocks(prgs);
    ///////////////////////////////////////Block////////////////////////////////////////////

    ///////////////////////////////////////NPC////////////////////////////////////////////
    loadLevelNPC(prgs);
    ///////////////////////////////////////NPC////////////////////////////////////////////


    //progress.setLabelText("Loading Music Data");
    ///////////////////////////////////////Music////////////////////////////////////////////
    loadMusic(prgs);
    ///////////////////////////////////////Music////////////////////////////////////////////

    ///////////////////////////////////////Sound////////////////////////////////////////////
    loadSound(prgs);
    ///////////////////////////////////////Sound////////////////////////////////////////////

    /*if((!progress.wasCanceled())&&(!nobar))
        progress.close();*/

    WriteToLog(QtDebugMsg, QString("-------------------------"));
    WriteToLog(QtDebugMsg, QString("Config status 1"));
    WriteToLog(QtDebugMsg, QString("-------------------------"));
    WriteToLog(QtDebugMsg, QString("Loaded blocks          %1/%2").arg(main_block.size()).arg(ConfStatus::total_blocks));
    WriteToLog(QtDebugMsg, QString("Loaded BGOs            %1/%2").arg(main_bgo.size()).arg(ConfStatus::total_bgo));
    WriteToLog(QtDebugMsg, QString("Loaded NPCs            %1/%2").arg(main_npc.size()).arg(ConfStatus::total_npc));
    WriteToLog(QtDebugMsg, QString("Loaded Backgrounds     %1/%2").arg(main_bg.size()).arg(ConfStatus::total_bg));
    WriteToLog(QtDebugMsg, QString("Loaded Level music     %1/%2").arg(main_music_lvl.size()).arg(ConfStatus::total_music_lvl));
    WriteToLog(QtDebugMsg, QString("Loaded Special music   %1/%2").arg(main_music_spc.size()).arg(ConfStatus::total_music_spc));
    WriteToLog(QtDebugMsg, QString("Loaded World music     %1/%2").arg(main_music_wld.size()).arg(ConfStatus::total_music_wld));
    WriteToLog(QtDebugMsg, QString("Loaded Sounds          %1/%2").arg(main_sound.size()).arg(ConfStatus::total_sound));

    return true;
}

bool dataconfigs::check()
{
    return
            (
    (main_bgo.size()<=0)||
    (main_bg.size()<=0)||
    (main_block.size()<=0)||
    (main_npc.size()<=0)||
    (main_music_lvl.size()<=0)||
    (main_music_wld.size()<=0)||
    (main_music_spc.size()<=0)||
    (main_sound.size()<=0)
            );
}


