/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2015 Vitaly Novichkov <admin@wohlnet.ru>
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

QList<PGE_Texture > ConfigManager::common_textures;


//Level config Data
QList<PGE_Texture >   ConfigManager::level_textures; //Texture bank


QList<PGE_Texture > ConfigManager::world_textures;


QString ConfigManager::imgFile, ConfigManager::imgFileM;
QString ConfigManager::tmpstr;
QStringList ConfigManager::tmp;

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
    config_dir = p + "/";
    config_id = QDir(p).dirName();
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
    refreshPaths();
    //////////////////////////////////////////////////////////////////////////////////

    if(!loadEngineSettings()) //!< Load engine.ini file
        return false;

    if(!loadDefaultMusics())
        return false;

    if(!loadDefaultSounds())
        return false;

    if(!loadSoundRolesTable())
        return false;


    /********Payable characters config should be loaded always!*******/
    if(!loadPlayableCharacters()) //!< Load lvl_characters.ini file
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

    lvl_bgo_indexes.clear();

    lvl_bg_indexes.clear();

    lvl_effects_indexes.clear();

    return true;
}


bool ConfigManager::unloadWorldConfigs()
{

    ///Clear texture bank
    while(!world_textures.isEmpty())
    {
        glDisable(GL_TEXTURE_2D);
        glDeleteTextures( 1, &(world_textures.last().texture) );
        world_textures.pop_back();
    }

    resetPlayableTexuresState();

    /***************Clear animators*************/
    Animator_Tiles.clear();
    Animator_Scenery.clear();
    Animator_WldPaths.clear();
    Animator_WldLevel.clear();
    /***************Clear settings*************/

    wld_tiles.clear();
    wld_scenery.clear();
    wld_paths.clear();
    wld_levels.clear();

    return true;
}

void ConfigManager::unluadAll()
{
    unloadLevelConfigs();
    unloadWorldConfigs();
    clearSoundIndex();
    playable_characters.clear();
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
