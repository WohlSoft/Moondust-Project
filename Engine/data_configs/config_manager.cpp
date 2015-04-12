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

#include "../common_features/pge_texture.h"
#include "config_manager.h"

#include "../common_features/graphics_funcs.h"

#include <QMessageBox>
#include <QDir>
#include <QFileInfo>

#include <QtDebug>

ConfigManager::ConfigManager()
{
}

DataFolders      ConfigManager::dirs;
QString          ConfigManager::config_dir;
QString          ConfigManager::config_id="dummy";
QString          ConfigManager::data_dir;
int              ConfigManager::default_grid=32;


QStringList ConfigManager::errorsList;

//Common Data
unsigned int ConfigManager::screen_width=800;
unsigned int ConfigManager::screen_height=600;

ConfigManager::screenType ConfigManager::screen_type = ConfigManager::SCR_Static;

//Fonts
FontsSetup ConfigManager::setup_fonts;
//Cursors data
MainCursors ConfigManager::setup_cursors;
//MessageBox setup
MessageBoxSetup ConfigManager::setup_message_box;
//Menu setup
MenuSetup ConfigManager::setup_menus;

//Loading Screen setings
LoadingScreenSetup ConfigManager::setup_LoadingScreen;

//Title Screen settings
TitleScreenSetup ConfigManager::setup_TitleScreen;

//World map settings
WorldMapData ConfigManager::setup_WorldMap;

QList<PGE_Texture > ConfigManager::common_textures;

unsigned long ConfigManager::music_custom_id;
unsigned long ConfigManager::music_w_custom_id;
QList<obj_music > ConfigManager::main_music_lvl;
QList<obj_music > ConfigManager::main_music_wld;
QList<obj_music > ConfigManager::main_music_spc;

QList<obj_sound > ConfigManager::main_sound;


//Level config Data
QList<PGE_Texture >   ConfigManager::level_textures; //Texture bank


QList<PGE_Texture > ConfigManager::world_textures;


QString ConfigManager::imgFile, ConfigManager::imgFileM;
QString ConfigManager::tmpstr;
QStringList ConfigManager::tmp;

unsigned long ConfigManager::total_data;
QString ConfigManager::bgoPath;
QString ConfigManager::BGPath;
QString ConfigManager::blockPath;
QString ConfigManager::npcPath;
QString ConfigManager::effectPath;
QString ConfigManager::playerLvlPath;
QString ConfigManager::playerWldPath;

QString ConfigManager::tilePath;
QString ConfigManager::scenePath;
QString ConfigManager::pathPath;
QString ConfigManager::wlvlPath;

QString ConfigManager::commonGPath;



void ConfigManager::setConfigPath(QString p)
{
    config_dir = ApplicationPath + "/" +  "configs/" + p + "/";
    config_id = p;
}


bool ConfigManager::loadBasics()
{
    QString gui_ini = config_dir + "main.ini";
    QSettings guiset(gui_ini, QSettings::IniFormat);
    guiset.setIniCodec("UTF-8");

    guiset.beginGroup("main");
        data_dir = (guiset.value("application-dir", "0").toBool() ?
                        ApplicationPath + "/" : config_dir + "data/" );
        guiset.endGroup();


    total_data=0;
    errorsList.clear();

    //dirs
    if((!QDir(config_dir).exists())||(QFileInfo(config_dir).isFile()))
    {
        QMessageBox::critical(NULL, "Config error",
                              QString("CONFIG DIR NOT FOUND AT: %1").arg(config_dir),
                              QMessageBox::Ok);
        return false;
    }

    QString main_ini = config_dir + "main.ini";

    if(!QFileInfo(main_ini).exists())
    {
        QMessageBox::critical(NULL, "Config error",
                              QString("Can't open the 'main.ini' config file!"),
                              QMessageBox::Ok);
        return false;
    }

    QSettings mainset(main_ini, QSettings::IniFormat);
    mainset.setIniCodec("UTF-8");

    QString customAppPath = ApplicationPath;
    mainset.beginGroup("main");
        customAppPath = mainset.value("application-path", ApplicationPath).toString();
        customAppPath.replace('\\', '/');
        bool appDir = mainset.value("application-dir", false).toBool();
        data_dir = (appDir ? customAppPath + "/" : config_dir + "data/" );

        if(appDir)
            dirs.worlds = customAppPath+"/"+mainset.value("worlds", config_id+"_worlds").toString() + "/";
        else
            dirs.worlds = AppPathManager::userAppDir()+"/"+mainset.value("worlds", config_id+"_worlds").toString() + "/";
        if(!QDir(dirs.worlds).exists())
            QDir().mkpath(dirs.worlds);

        dirs.music = data_dir + mainset.value("music", "data/music").toString() + "/";
        dirs.sounds = data_dir + mainset.value("sound", "data/sound").toString() + "/";

        dirs.glevel = data_dir + mainset.value("graphics-level", "data/graphics/level").toString() + "/";
        dirs.gworld= data_dir + mainset.value("graphics-worldmap", "data/graphics/worldmap").toString() + "/";
        dirs.gplayble = data_dir + mainset.value("graphics-characters", "data/graphics/characters").toString() + "/";

        dirs.gcommon = config_dir + "data/" + mainset.value("graphics-common", "data-custom").toString() + "/";

        dirs.gcustom = data_dir + mainset.value("custom-data", "data-custom").toString() + "/";
    mainset.endGroup();

    mainset.beginGroup("graphics");
        default_grid = mainset.value("default-grid", 32).toInt();
    mainset.endGroup();

    if( mainset.status() != QSettings::NoError )
    {
        QMessageBox::critical(NULL, "Config error",
                              QString("ERROR LOADING main.ini N:%1").arg(mainset.status()),
                              QMessageBox::Ok);
        return false;
    }


    ////////////////////////////////Preparing////////////////////////////////////////
    bgoPath =   dirs.glevel +  "background/";
    BGPath =    dirs.glevel +  "background2/";
    blockPath = dirs.glevel +  "block/";
    npcPath =   dirs.glevel +  "npc/";
    effectPath= dirs.glevel +  "effect/";
    playerLvlPath = dirs.gplayble;
    playerWldPath = dirs.gworld + "player/";

    tilePath =  dirs.gworld +  "tile/";
    scenePath = dirs.gworld +  "scene/";
    pathPath =  dirs.gworld +  "path/";
    wlvlPath =  dirs.gworld +  "level/";

    commonGPath = dirs.gcommon + "/";
    //////////////////////////////////////////////////////////////////////////////////

    QString engine_ini = config_dir + "engine.ini";

    if(!QFileInfo(engine_ini).exists())
    {
        QMessageBox::critical(NULL, "Config error",
                              QString("Can't open the 'engine.ini' config file!"),
                              QMessageBox::Ok);
        return false;
    }

    QSettings engineset(engine_ini, QSettings::IniFormat);
    engineset.setIniCodec("UTF-8");

    engineset.beginGroup("fonts");
        setup_fonts.fontname = engineset.value("font-file", "").toString();
        setup_fonts.double_pixled = engineset.value("double-pixled", false).toBool();
    engineset.endGroup();

    engineset.beginGroup("common");
        screen_width = engineset.value("screen-width", 800).toInt();
        screen_height = engineset.value("screen-height", 600).toInt();
        QString scrType = engineset.value("screen-type", "static").toString();

        if(scrType == "dynamic")
            screen_type = SCR_Dynamic;
        else
            screen_type = SCR_Static;

        setup_cursors.normal = engineset.value("cursor-image-normal", "").toString();
        checkForImage(setup_cursors.normal, dirs.gcommon);

        setup_cursors.rubber = engineset.value("cursor-image-rubber", "").toString();
        checkForImage(setup_cursors.rubber, dirs.gcommon);
    engineset.endGroup();

    engineset.beginGroup("message-box");
        setup_message_box.sprite = engineset.value("image", "").toString();
        checkForImage(setup_message_box.sprite, dirs.gcommon);
        setup_message_box.borderWidth = (unsigned)engineset.value("border-width", 32).toInt();
    engineset.endGroup();

    engineset.beginGroup("menu");
        setup_menus.selector = engineset.value("selector", "").toString();
        checkForImage(setup_menus.selector, dirs.gcommon);

        setup_menus.scrollerUp = engineset.value("scroll-up", "").toString();
        checkForImage(setup_menus.scrollerUp, dirs.gcommon);

        setup_menus.scrollerDown = engineset.value("scroll-down", "").toString();
        checkForImage(setup_menus.scrollerDown, dirs.gcommon);
    engineset.endGroup();

    ////// World map settings

    engineset.beginGroup("world-map");
        setup_WorldMap.backgroundImg = engineset.value("background", "").toString();
        checkForImage(setup_WorldMap.backgroundImg, dirs.gcommon);
        setup_WorldMap.viewport_x = engineset.value("viewport-x", "").toInt();
        setup_WorldMap.viewport_y = engineset.value("viewport-y", "").toInt();
        setup_WorldMap.viewport_w = engineset.value("viewport-width", "").toInt();
        setup_WorldMap.viewport_h = engineset.value("viewport-height", "").toInt();

        setup_WorldMap.title_x = engineset.value("level-title-x", "").toInt();
        setup_WorldMap.title_y = engineset.value("level-title-y", "").toInt();
        setup_WorldMap.title_w = engineset.value("level-title-w", "").toInt();

        QString ttlAlign = engineset.value("level-title-align", "left").toString();
        if(ttlAlign=="center")
            setup_WorldMap.title_align = WorldMapData::align_center;
        else
        if(ttlAlign=="right")
            setup_WorldMap.title_align = WorldMapData::align_right;
        else
            setup_WorldMap.title_align = WorldMapData::align_left;

        setup_WorldMap.points_en = engineset.value("points-counter", "").toBool();
        setup_WorldMap.points_x = engineset.value("points-counter-x", "").toInt();
        setup_WorldMap.points_y = engineset.value("points-counter-y", "").toInt();

        setup_WorldMap.health_en = engineset.value("health-counter", "").toBool();
        setup_WorldMap.health_x = engineset.value("health-counter-x", "").toInt();
        setup_WorldMap.health_y = engineset.value("health-counter-y", "").toInt();

        setup_WorldMap.star_en = engineset.value("star-counter", "").toBool();
        setup_WorldMap.star_x = engineset.value("star-counter-x", "").toInt();
        setup_WorldMap.star_y = engineset.value("star-counter-y", "").toInt();

        setup_WorldMap.coin_en = engineset.value("coin-counter", "").toBool();
        setup_WorldMap.coin_x = engineset.value("coin-counter-x", "").toInt();
        setup_WorldMap.coin_y = engineset.value("coin-counter-y", "").toInt();

        setup_WorldMap.portrait_en = engineset.value("portrait", "").toBool();
        setup_WorldMap.portrait_x = engineset.value("portrait-x", "").toInt();
        setup_WorldMap.portrait_y = engineset.value("portrait-y", "").toInt();

    engineset.endGroup();


    ////////// Loading scene settings

    int LoadScreenImages=0;
    engineset.beginGroup("loading-scene");
        setup_LoadingScreen.backgroundColor.setNamedColor(engineset.value("bg-color", "#000000").toString());
        setup_LoadingScreen.backgroundImg = engineset.value("background", "").toString();
        checkForImage(setup_LoadingScreen.backgroundImg, dirs.gcommon);

        setup_LoadingScreen.updateDelay = engineset.value("updating-time", 128).toInt();
        LoadScreenImages = engineset.value("additional-images", 0).toInt();
    engineset.endGroup();


    setup_LoadingScreen.AdditionalImages.clear();
    for(int i=1; i<=LoadScreenImages; i++)
    {
        engineset.beginGroup(QString("loading-image-%1").arg(i));
        LoadingScreenAdditionalImage img;

        img.imgFile = engineset.value("image", "").toString();
        checkForImage(img.imgFile, dirs.gcommon);

        img.animated = engineset.value("animated", false).toBool();
        if(img.animated)
            img.frames = engineset.value("frames", 1).toInt();
        else
            img.frames = 1;
        if(img.frames<=0) img.frames = 1;

        img.x =  engineset.value("pos-x", 1).toInt();
        img.y =  engineset.value("pos-y", 1).toInt();
        setup_LoadingScreen.AdditionalImages.push_back(img);

        engineset.endGroup();
    }

    ////////// Title screen (main menu) settings

    int TitleScreenImages=0;
    engineset.beginGroup("title-screen");
        setup_TitleScreen.backgroundImg = engineset.value("background", "").toString();
        setup_TitleScreen.backgroundColor.setNamedColor(engineset.value("bg-color", "#000000").toString());
        checkForImage(setup_TitleScreen.backgroundImg, dirs.gcommon);
        TitleScreenImages = engineset.value("additional-images", 0).toInt();
    engineset.endGroup();


    setup_TitleScreen.AdditionalImages.clear();
    for(int i=1; i<=TitleScreenImages; i++)
    {
        engineset.beginGroup(QString("title-image-%1").arg(i));
        TitleScreenAdditionalImage img;

        img.imgFile = engineset.value("image", "").toString();
        checkForImage(img.imgFile, dirs.gcommon);

        img.animated = engineset.value("animated", false).toBool();
        if(img.animated)
        {
            img.frames = engineset.value("frames", 1).toInt();
            img.framespeed = engineset.value("framespeed", 128).toInt();
        }
        else
        {
            img.frames = 1;
            img.framespeed = 128;
        }
        if(img.frames<=0) img.frames = 1;

        img.x =  engineset.value("pos-x", 0).toInt();
        img.y =  engineset.value("pos-y", 0).toInt();

        img.center_x =  engineset.value("center-x", false).toBool();
        img.center_y =  engineset.value("center-y", false).toBool();

        QString align =   engineset.value("align", "none").toString();

        if(align=="left")
            img.align_to = TitleScreenAdditionalImage::LEFT_ALIGN;
        else
        if(align=="top")
            img.align_to = TitleScreenAdditionalImage::TOP_ALIGN;
        else
        if(align=="right")
            img.align_to = TitleScreenAdditionalImage::RIGHT_ALIGN;
        else
        if(align=="bottom")
            img.align_to = TitleScreenAdditionalImage::BOTTOM_ALIGN;
        else
        if(align=="center")
            img.align_to = TitleScreenAdditionalImage::CENTER_ALIGN;
        else
        img.align_to = TitleScreenAdditionalImage::NO_ALIGN;

        setup_TitleScreen.AdditionalImages.push_back(img);

        engineset.endGroup();
    }

    /********Payable characters config should be loaded always!*******/
    if(!loadPlayableCharacters())
        return false;



    return true;
}

void ConfigManager::addError(QString bug, QtMsgType level)
{
    Q_UNUSED(level);
    qWarning() << bug;
    errorsList<<bug;
}



bool ConfigManager::unloadLevelConfigs()
{

    ///Clear texture bank
    while(!level_textures.isEmpty())
    {
        glDisable(GL_TEXTURE_2D);
        glDeleteTextures( 1, &(level_textures.last().texture) );
        level_textures.pop_back();
    }

    resetPlayableTexuresState();

    /***************Clear animators*************/
    Animator_Blocks.clear();
    Animator_BGO.clear();
    Animator_BG.clear();
    Animator_EFFECT.clear();
    /***************Clear settings*************/

    lvl_block_indexes.clear();
    lvl_blocks.clear();

    lvl_bgo_indexes.clear();
    lvl_bgo.clear();

    lvl_bg_indexes.clear();
    lvl_bg.clear();

    lvl_effects_indexes.clear();
    lvl_effects.clear();

    return true;
}



void ConfigManager::checkForImage(QString &imgPath, QString root)
{
    if(!imgPath.isEmpty())
    {
        if(!QImage(root+imgPath).isNull())
            imgPath = root+imgPath;
        else
            imgPath = "";
    }
}
