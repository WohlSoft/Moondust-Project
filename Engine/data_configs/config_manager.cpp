/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#include <common_features/pge_texture.h>
#include <common_features/version_cmp.h>
#include <common_features/logger.h>
#include <common_features/tr.h>
#include "config_manager.h"
#include "config_manager_private.h"
#include <graphics/gl_renderer.h>
#include <graphics/window.h>
#include <gui/pge_msgbox.h>
#include "../version.h"

#include <DirManager/dirman.h>
#include <Utils/files.h>
#include <Utils/open_url.h>
#include <fmt/fmt_format.h>
#include <fmt/fmt_qformat.h>
#include <algorithm>

std::string     ConfigManager::configPackId;

DataFolders     ConfigManager::dirs;
std::string     ConfigManager::config_name;
std::string     ConfigManager::config_dirSTD;
std::string     ConfigManager::config_idSTD = "dummy";
std::string     ConfigManager::data_dirSTD;
unsigned int    ConfigManager::default_grid = 32u;

ScriptsSetup ConfigManager::setup_Scripts;

Strings::List ConfigManager::errorsList;

//Common Data
unsigned int ConfigManager::viewport_width = 800;
unsigned int ConfigManager::viewport_height = 600;

ConfigManager::screenType ConfigManager::viewport_type = ConfigManager::SCR_Static;

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

ConfigManager::TexturesBank ConfigManager::common_textures;


/* *** Texture banks *** */
ConfigManager::TexturesBank ConfigManager::level_textures;
ConfigManager::TexturesBank ConfigManager::world_textures;

std::string ConfigManager::imgFile, ConfigManager::imgFileM;
std::string ConfigManager::tmpstr;
Strings::List ConfigManager::tmp;




void ConfigManager::setConfigPath(std::string p)
{
    config_dirSTD = p;
    if(!config_dirSTD.empty())
    {
        if(config_dirSTD.back() != '/')
            config_dirSTD.push_back('/');
    }
    config_idSTD = Files::dirname(DirMan(config_dirSTD).absolutePath());
}

static void msgBox(std::string title, std::string text)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
                             title.c_str(), text.c_str(),
                             PGE_Window::window);
}

bool ConfigManager::loadBasics()
{
    std::string gui_ini = config_dirSTD + "main.ini";
    IniProcessing guiset(gui_ini);

    guiset.beginGroup("main");
    {
        data_dirSTD = (guiset.value("application-dir", false).toBool() ?
                        ApplicationPathSTD : config_dirSTD + "data/");
    }
    guiset.endGroup();
    errorsList.clear();

    //dirs
    if(!DirMan::exists(config_dirSTD))
    {
        //% "Config pack directory error"
        std::string title = qtTrId("ERR_CONFIG_DIR_MISSING_TTL");
        //% "Config directory is not found at:\n%1"
        std::string msg = qtTrId("ERR_CONFIG_DIR_MISSING");
        msgBox(title, fmt::qformat(msg, config_dirSTD));
        return false;
    }

    std::string main_ini = config_dirSTD + "main.ini";

    if(!Files::fileExists(main_ini))
    {
        //% "Missing main.ini"
        std::string title = qtTrId("ERR_CONFIG_ERROR_MAININI_TTL");
        //% "Can't open the 'main.ini' config file!"
        std::string msg = qtTrId("ERR_CONFIG_ERROR_MAININI_MSG");
        msgBox(title, msg);
        return false;
    }

    IniProcessing mainset(main_ini);

    std::string customAppPath = ApplicationPathSTD;
    mainset.beginGroup("main");
    {
        config_name = mainset.value("config_name").toString();
        customAppPath = mainset.value("application-path", ApplicationPathSTD).toString();
        std::replace(customAppPath.begin(), customAppPath.end(), '\\', '/');
        bool appDir = mainset.value("application-dir", false).toBool();
        data_dirSTD = (appDir ? customAppPath + "/" : config_dirSTD + "data/");

        if(DirMan::exists(ApplicationPathSTD + data_dirSTD)) //Check as relative
            data_dirSTD = ApplicationPathSTD + data_dirSTD;
        else if(!DirMan::exists(data_dirSTD)) //Check as absolute
        {
            //% "Missing config pack data directory"
            std::string title = qtTrId("ERR_CONFIG_ERROR_DATADIR_TTL");
            //% "Config pack data path not exists:\n%1"
            std::string msg = qtTrId("ERR_CONFIG_ERROR_DATADIR_MSG");
            msgBox(title, fmt::qformat(msg, data_dirSTD));
            return false;
        }

        data_dirSTD = DirMan(data_dirSTD).absolutePath() + "/";

        std::string url     = mainset.value("home-page", "http://wohlsoft.ru/config_packs/").toString();
        std::string version = mainset.value("pge-engine-version", "0.0").toString();
        bool    hasApiVersion = mainset.hasKey("api-version");
        unsigned int apiVersion = mainset.value("api-version", 1).toUInt();
        bool ver_notify = mainset.value("enable-version-notify", true).toBool();
        bool    ver_invalid = false;

        mainset.read("config-pack-id", configPackId, std::string());

        if(hasApiVersion)
            ver_invalid = (apiVersion != V_CP_API) || (apiVersion < 41);
        else
            ver_invalid = (version != VersionCmp::compare(_LATEST_STABLE, version));

        pLogDebug("Config pack version validation: "
                   "has API version: %d, has invalid version: %d",
                   static_cast<int>(hasApiVersion),
                   static_cast<int>(ver_invalid));

        if(ver_notify && ver_invalid)
        {
            pLogWarning("Config pack version is invalid: "
                        "has API version: %d, has invalid version: %d, current version %s, version wanted: %s",
                        static_cast<int>(hasApiVersion), static_cast<int>(ver_invalid), _LATEST_STABLE, version.c_str());

            //% "Legacy configuration package"
            std::string title = qtTrId("WARNING_LEGACY_CONFIG_PACK_TTL");
            /*% "You have a legacy configuration package.\n"
                "Game will be started, but you may have a some problems with gameplay.\n\n"
                "Please download and install latest version of a configuration package:\n\n"
                "Download: %1\n\n"
                "Note: most of config packs are updates togeter with PGE,\n"
                "therefore you can use same link to get updated version." */
            std::string msg = fmt::qformat(qtTrId("WARNING_LEGACY_CONFIG_PACK"), url);
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING,
                                     title.c_str(), msg.c_str(),
                                     PGE_Window::window);
            Utils::openUrl(url);
        }

        if(appDir)
            dirs.worlds = customAppPath + "/" + mainset.value("worlds", config_idSTD + "_worlds").toString() + "/";
        else
            dirs.worlds = AppPathManager::userAppDirSTD() + "/" + mainset.value("worlds", config_idSTD + "_worlds").toString() + "/";

        if(!DirMan::exists(dirs.worlds))
            DirMan::mkAbsPath(dirs.worlds);

        dirs.music = data_dirSTD + mainset.value("music", "data/music").toString() + "/";
        dirs.sounds = data_dirSTD + mainset.value("sound", "data/sound").toString() + "/";
        dirs.glevel = data_dirSTD + mainset.value("graphics-level", "data/graphics/level").toString() + "/";
        dirs.gworld = data_dirSTD + mainset.value("graphics-worldmap", "data/graphics/worldmap").toString() + "/";
        dirs.gplayble = data_dirSTD + mainset.value("graphics-characters", "data/graphics/characters").toString() + "/";
        dirs.gcommon  = config_dirSTD + "data/" + mainset.value("graphics-common", "graphics/common").toString() + "/";
        setup_Scripts.lvl_local  = mainset.value("local-script-name-lvl", "level.lua").toString();
        setup_Scripts.lvl_common = mainset.value("common-script-name-lvl", "level.lua").toString();
        setup_Scripts.wld_local  = mainset.value("local-script-name-wld", "world.lua").toString();
        setup_Scripts.wld_common = mainset.value("common-script-name-wld", "world.lua").toString();
        dirs.gcustom = data_dirSTD + mainset.value("custom-data", "data-custom").toString() + "/";
        D_pLogDebugNA("=============Standard directories=============");
        D_pLogDebug("Music:                         %s", dirs.music.c_str());
        D_pLogDebug("SFX:                           %s", dirs.sounds.c_str());
        D_pLogDebug("Level graphics:                %s", dirs.glevel.c_str());
        D_pLogDebug("World map graphics:            %s", dirs.gworld.c_str());
        D_pLogDebug("Playable characters graphics:  %s", dirs.gplayble.c_str());
        D_pLogDebug("Common graphics:               %s", dirs.gcommon.c_str());
        D_pLogDebug("Custom data (reserved):        %s", dirs.gcustom.c_str());
        D_pLogDebugNA("==============================================");
    }
    mainset.endGroup();
    mainset.beginGroup("graphics");
    {
        mainset.read("default-grid", default_grid, 32);
    }
    mainset.endGroup();

    if(mainset.lastError() != IniProcessing::ERR_OK)
    {
        //% "main.ini is invalid"
        std::string title = qtTrId("ERROR_CONFIG_MAININI_FAILED_TTL");
        //% "Error has occouped in main.ini in line %1."
        std::string msg = qtTrId("ERROR_CONFIG_MAININI_FAILED_MSG");
        msgBox(title, fmt::qformat(msg, mainset.lineWithError()));
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
    Dir_PlayerCalibrations.setCustomDirs("", "", playerCalibrationsPath);
    if(!loadPlayableCharacters()) //!< Load lvl_characters.ini file
        return false;

    return true;
}

void ConfigManager::addError(std::string bug)
{
    pLogWarning("%s", bug.c_str());
    errorsList.push_back(bug);
}



bool ConfigManager::unloadLevelConfigs()
{
    ///Clear texture bank
    for(size_t i = 0; i < level_textures.size(); i++)
        GlRenderer::deleteTexture(level_textures[i]);
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
    for(size_t i = 0; i < world_textures.size(); i++)
        GlRenderer::deleteTexture(world_textures[i]);
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

void ConfigManager::checkForImage(std::string &imgPath, std::string root)
{
    if(!imgPath.empty())
    {
        if(Files::fileExists(root + imgPath))
            imgPath = root + imgPath;
        else
            imgPath = "";
    }
}

bool ConfigManager::loadExtraFoldersList(const std::string &episodePath, std::vector<std::string> &out_paths)
{
    std::string episodePath_s = episodePath;
    if(!episodePath_s.empty())
    {
        if(episodePath_s.back() != '/')
            episodePath_s.push_back('/');
    }
    std::string foldersIni = episodePath_s + "folders.ini";
    out_paths.clear();

    if(!Files::fileExists(foldersIni))
    {
        pLogDebug("Extra folders.ini is not exists [%s], skipping...", foldersIni.c_str());
        return false;
    }

    IniProcessing fIni(foldersIni);

    fIni.beginGroup("folders");
    auto keys = fIni.allKeys();
    std::string out;
    for(std::string &k : keys)
    {
        fIni.read(k.c_str(), out, "");
        if(!out.empty())
            out_paths.push_back(episodePath_s + out);
    }
    fIni.endGroup();

    pLogDebug("Extra folders.ini has been loaded with %zu paths in the list.", out_paths.size());
    return true;
}
