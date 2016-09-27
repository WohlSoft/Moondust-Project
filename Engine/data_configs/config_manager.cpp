/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <common_features/pge_texture.h>
#include <common_features/version_cmp.h>
#include "config_manager.h"
#include <graphics/gl_renderer.h>
#include <graphics/window.h>
#include <gui/pge_msgbox.h>
#include "../version.h"

#include <QDir>
#include <QFileInfo>

#include <QDesktopServices>
#include <QUrl>

#include <QtDebug>

ConfigManager::ConfigManager()
{}

DataFolders      ConfigManager::dirs;
QString          ConfigManager::config_dir;
QString          ConfigManager::config_id="dummy";
QString          ConfigManager::data_dir;
int              ConfigManager::default_grid=32;

static ScriptsSetup ConfigManager::setup_Scripts;

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
//MenuBox setup
MenuBoxSetup ConfigManager::setup_menu_box;
//Menu setup
MenuSetup ConfigManager::setup_menus;

QList<PGE_Texture > ConfigManager::common_textures;


//Level config Data
QList<PGE_Texture >   ConfigManager::level_textures; //Texture bank


QList<PGE_Texture > ConfigManager::world_textures;


QString ConfigManager::imgFile, ConfigManager::imgFileM;
QString ConfigManager::tmpstr;
QStringList ConfigManager::tmp;




void ConfigManager::setConfigPath(QString p)
{
    config_dir = p + "/";
    config_id = QDir(p).dirName();
}

static void msgBox(QString title, QString text)
{
    std::string ttl = title.toStdString();
    std::string msg = text.toStdString();
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                             ttl.c_str(), msg.c_str(),
                             PGE_Window::window);
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
        msgBox( "Config error",
                QString("CONFIG DIR NOT FOUND AT: %1").arg(config_dir));
        return false;
    }

    QString main_ini = config_dir + "main.ini";

    if(!QFileInfo(main_ini).exists())
    {
        msgBox("Config error",
               QString("Can't open the 'main.ini' config file!"));
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

        if(QDir(ApplicationPath+"/"+data_dir).exists())//Check as relative
            data_dir = ApplicationPath+"/"+data_dir;
        else
        if(!QDir(data_dir).exists())//Check as absolute
        {
            msgBox("Config error",
                   QString("Config data path not exists: %1").arg(data_dir));
            return false;
        }

        data_dir = QDir(data_dir).absolutePath()+"/";

        QString url     = mainset.value("home-page", "http://engine.wohlnet.ru/config_packs/").toString();
        QString version = mainset.value("pge-engine-version", "0.0").toString();
        bool ver_notify = mainset.value("enable-version-notify", true).toBool();
        if(ver_notify && (version != VersionCmp::compare(QString("%1").arg(_LATEST_STABLE), version)))
        {
            std::string title = "Legacy configuration package";
            std::string msg = QString("You have a legacy configuration package.\n"
                                      "Game will be started, but you may have a some problems with gameplay.\n\n"
                                      "Please download and install latest version of a configuration package:\n\n"
                                      "Download: %1\n"
                                      "Note: most of config packs are updates togeter with PGE,\n"
                                      "therefore you can use same link to get updated version")
                                      /*.arg("<a href=\"%1\">%1</a>")*/.arg(url).toStdString();

            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING,
                                     title.c_str(), msg.c_str(),
                                     PGE_Window::window);

            QDesktopServices::openUrl(QUrl(url));
            /*QMessageBox box;
            box.setWindowTitle( "Legacy configuration package" );
            box.setTextFormat(Qt::RichText);
            box.setTextInteractionFlags(Qt::TextBrowserInteraction);
            box.setText();
            box.setStandardButtons(QMessageBox::Ok);
            box.setIcon(QMessageBox::Warning);
            box.exec();*/
        }

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

        setup_Scripts.lvl_local  = mainset.value("local-script-name-lvl", "level.lua").toString();
        setup_Scripts.lvl_common = mainset.value("common-script-name-lvl", "level.lua").toString();
        setup_Scripts.wld_local  = mainset.value("local-script-name-lvl", "world.lua").toString();
        setup_Scripts.wld_common = mainset.value("common-script-name-lvl", "world.lua").toString();

        dirs.gcustom = data_dir + mainset.value("custom-data", "data-custom").toString() + "/";
    mainset.endGroup();




    mainset.beginGroup("graphics");
        default_grid = mainset.value("default-grid", 32).toInt();
    mainset.endGroup();

    if( mainset.status() != QSettings::NoError )
    {
        msgBox( "Config error",
                QString("ERROR LOADING main.ini N:%1").arg(mainset.status()) );
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
    for(int i=0; i<level_textures.size(); i++)
    {
        GlRenderer::deleteTexture( level_textures[i] );
    }
    level_textures.clear();

    resetPlayableTexuresState();

    /***************Clear animators*************/
    Animator_Blocks.clear();
    Animator_BGO.clear();
    Animator_NPC.clear();
    Animator_BG.clear();
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
    for(int i=0; i<world_textures.size(); i++)
    {
        GlRenderer::deleteTexture( world_textures[i] );
    }
    world_textures.clear();

    resetPlayableTexuresState();
    resetPlayableTexuresStateWld();

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
        if(QFile(root+imgPath).exists())
            imgPath = root+imgPath;
        else
            imgPath = "";
    }
}

