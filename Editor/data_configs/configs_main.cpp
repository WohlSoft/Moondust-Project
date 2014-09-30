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

#include "../common_features/app_path.h"

#include "../main_window/global_settings.h"

long ConfStatus::total_blocks=0;
long ConfStatus::total_bgo=0;
long ConfStatus::total_bg=0;
long ConfStatus::total_npc=0;

long ConfStatus::total_wtile=0;
long ConfStatus::total_wpath=0;
long ConfStatus::total_wscene=0;
long ConfStatus::total_wlvl=0;

long ConfStatus::total_music_lvl=0;
long ConfStatus::total_music_wld=0;
long ConfStatus::total_music_spc=0;
long ConfStatus::total_sound=0;

QString ConfStatus::configName="";
QString ConfStatus::configPath="";

QString ConfStatus::defaultTheme="";


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

void dataconfigs::setConfigPath(QString p)
{
    config_dir = ApplicationPath + "/" +  "configs/" + p + "/";
}

void dataconfigs::loadBasics()
{
    QString gui_ini = config_dir + "main.ini";
    QSettings guiset(gui_ini, QSettings::IniFormat);
    guiset.setIniCodec("UTF-8");

    guiset.beginGroup("gui");
        splash_logo = guiset.value("editor-splash", "").toString();
        ConfStatus::defaultTheme = guiset.value("default-theme", "").toString();
    guiset.endGroup();

    guiset.beginGroup("main");
        data_dir = (guiset.value("application-dir", "0").toBool() ?
                        ApplicationPath + "/" : config_dir + "data/" );
    guiset.endGroup();



    //Default splash image
    //if(splash_logo .isEmpty())
        //splash_logo = ;//Themes::Image(Themes::splash)
    if(!splash_logo .isEmpty())
    {
        splash_logo = data_dir + splash_logo;
        if(QPixmap(splash_logo).isNull())
        {
            WriteToLog(QtWarningMsg, QString("Wrong splash image: %1").arg(splash_logo));
            splash_logo = "";//Themes::Image(Themes::splash);
        }
    }
}

bool dataconfigs::loadconfigs(QProgressDialog *prgs)
{
    //unsigned long i;//, prgs=0;

    total_data=0;
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

        data_dir = (dirset.value("application-dir", false).toBool() ?
                        ApplicationPath + "/" : config_dir + "data/" );

        ConfStatus::configName = dirset.value("config_name", QDir(config_dir).dirName()).toString();

        dirs.worlds = data_dir + dirset.value("worlds", "worlds").toString() + "/";

        dirs.music = data_dir + dirset.value("music", "data/music").toString() + "/";
        dirs.sounds = data_dir + dirset.value("sound", "data/sound").toString() + "/";

        dirs.glevel = data_dir + dirset.value("graphics-level", "data/graphics/level").toString() + "/";
        dirs.gworld= data_dir + dirset.value("graphics-worldmap", "data/graphics/worldmap").toString() + "/";
        dirs.gplayble = data_dir + dirset.value("graphics-characters", "data/graphics/characters").toString() + "/";

        dirs.gcustom = data_dir + dirset.value("custom-data", "data-custom").toString() + "/";
    dirset.endGroup();

    ConfStatus::configPath = config_dir;

    dirset.beginGroup("graphics");
        default_grid = dirset.value("default-grid", 32).toInt();
    dirset.endGroup();

    if( dirset.status() != QSettings::NoError )
    {
        WriteToLog(QtCriticalMsg, QString("ERROR LOADING main.ini N:%1").arg(dirset.status()));
    }


    characters.clear();

    dirset.beginGroup("characters");
        int characters_q = dirset.value("characters", 0).toInt();
        for(int i=1; i<= characters_q; i++)
        {
            obj_playable_character pchar;
            pchar.id = i;
            pchar.name = dirset.value(QString("character%1-name").arg(i), QString("Character #%1").arg(i)).toString();
            characters.push_back(pchar);
        }
    dirset.endGroup();


    ////////////////////////////////Preparing////////////////////////////////////////
    bgoPath =   dirs.glevel +  "background/";
    BGPath =    dirs.glevel +  "background2/";
    blockPath = dirs.glevel +  "block/";
    npcPath =   dirs.glevel +  "npc/";

    tilePath =  dirs.gworld +  "tile/";
    scenePath = dirs.gworld +  "scene/";
    pathPath =  dirs.gworld +  "path/";
    wlvlPath =  dirs.gworld +  "level/";

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

    ///////////////////////////////////////Level items////////////////////////////////////////////
    loadLevelBackgrounds(prgs);
    loadLevelBGO(prgs);
    loadLevelBlocks(prgs);
    loadLevelNPC(prgs);
    ///////////////////////////////////////Level items////////////////////////////////////////////

    ///////////////////////////////////////World map items////////////////////////////////////////
    loadWorldTiles(prgs);
    loadWorldScene(prgs);
    loadWorldPaths(prgs);
    loadWorldLevels(prgs);
    ///////////////////////////////////////World map items////////////////////////////////////////


    //progress.setLabelText("Loading Music Data");
    ///////////////////////////////////////Music////////////////////////////////////////////
    loadMusic(prgs);
    ///////////////////////////////////////Music////////////////////////////////////////////

    ///////////////////////////////////////Sound////////////////////////////////////////////
    loadSound(prgs);
    ///////////////////////////////////////Sound////////////////////////////////////////////


    ///////////////////////////////////////Tilesets////////////////////////////////////////////
    loadTilesets();
    ///////////////////////////////////////Tilesets////////////////////////////////////////////

    /*if((!progress.wasCanceled())&&(!nobar))
        progress.close();*/

    WriteToLog(QtDebugMsg, QString("-------------------------"));
    WriteToLog(QtDebugMsg, QString("Config status 1"));
    WriteToLog(QtDebugMsg, QString("-------------------------"));
    WriteToLog(QtDebugMsg, QString("Loaded blocks          %1/%2").arg(main_block.size()).arg(ConfStatus::total_blocks));
    WriteToLog(QtDebugMsg, QString("Loaded BGOs            %1/%2").arg(main_bgo.size()).arg(ConfStatus::total_bgo));
    WriteToLog(QtDebugMsg, QString("Loaded NPCs            %1/%2").arg(main_npc.size()).arg(ConfStatus::total_npc));
    WriteToLog(QtDebugMsg, QString("Loaded Backgrounds     %1/%2").arg(main_bg.size()).arg(ConfStatus::total_bg));
    WriteToLog(QtDebugMsg, QString("Loaded Tiles           %1/%2").arg(main_wtiles.size()).arg(ConfStatus::total_wtile));
    WriteToLog(QtDebugMsg, QString("Loaded Sceneries       %1/%2").arg(main_wscene.size()).arg(ConfStatus::total_wscene));
    WriteToLog(QtDebugMsg, QString("Loaded Path images     %1/%2").arg(main_wpaths.size()).arg(ConfStatus::total_wpath));
    WriteToLog(QtDebugMsg, QString("Loaded Level images    %1/%2").arg(main_wlevels.size()).arg(ConfStatus::total_wlvl));
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
    (main_wtiles.size()<=0)||
    (main_wscene.size()<=0)||
    (main_wpaths.size()<=0)||
    (main_wlevels.size()<=0)||
    (main_music_lvl.size()<=0)||
    (main_music_wld.size()<=0)||
    (main_music_spc.size()<=0)||
    (main_sound.size()<=0)
            );
}


