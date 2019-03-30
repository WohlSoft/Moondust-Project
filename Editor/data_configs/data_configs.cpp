/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifdef __linux__
#include <QProcessEnvironment>
#elif _WIN32
#include <QSysInfo>
#endif

#include <QApplication>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>

#include <common_features/app_path.h>
#include <common_features/version_cmp.h>
#include <main_window/global_settings.h>
#include "../version.h"

#include "data_configs.h"

dataconfigs::dataconfigs()
{
    total_data = 0;

    music_custom_id = 1;
    music_w_custom_id = 1;

    defaultGrid.general = 0;
    defaultGrid.block = 32;     //-V112
    defaultGrid.bgo = 32;       //-V112
    defaultGrid.npc = 32;       //-V112
    defaultGrid.terrain = 32;   //-V112
    defaultGrid.scenery = 16;
    defaultGrid.paths = 32;     //-V112
    defaultGrid.levels = 32;    //-V112

    engine.screen_w = 800;
    engine.screen_h = 600;

    engine.wld_viewport_w = 668;
    engine.wld_viewport_h = 403;
}

/*
[background-1]
name="Smallest bush"        ;background name, default="background-%n"
type="scenery"          ;Background type, default="Scenery"
grid=32             ; 32 | 16 Default="32"
view=background         ; background | foreground, default="background"
image="background-1.gif"    ;Image file with level file ; the image mask will be have *m.gif name.
climbing=0          ; default = 0
animated = 0            ; default = 0 - no
frames = 1          ; default = 1
frame-speed=125         ; default = 125 ms, etc. 8 frames per sec
*/

QString dataconfigs::getFullIniPath(QString iniFileName)
{
    QString path_ini = config_dir + iniFileName;
    if(!QFile::exists(path_ini))
    {
        addError(QString("ERROR LOADING %1: file does not exist").arg(iniFileName), PGE_LogLevel::Critical);
        return "";
    }
    return path_ini;
}

bool dataconfigs::openSection(IniProcessing *config, const std::string &section, bool tryGeneral)
{
    //Check for availability of the INI section
    if(!config->beginGroup(section))
    {
        if(tryGeneral && config->beginGroup("General"))
            return true;//Allow section-less custom config files

        addError(QString("ERROR LOADING %1: [%2] section is missed!")
                 .arg(StdToPGEString(config->fileName()))
                 .arg(StdToPGEString(section)), PGE_LogLevel::Critical);
        return false;
    }
    return true;
}

void dataconfigs::addError(QString bug, PGE_LogLevel level)
{
    WriteToLog(level, QString("LoadConfig -> %1").arg(bug));
    errorsList[m_errOut] << bug;
}

void dataconfigs::setConfigPath(QString p)
{
    config_dir = p;
    if(!config_dir.endsWith('/'))
        config_dir.append('/');
}

bool dataconfigs::loadBasics()
{
    errorsList[ERR_GLOBAL].clear();
    errorsList[ERR_CUSTOM].clear();
    m_errOut = ERR_GLOBAL;

    QString gui_ini = getFullIniPath("main.ini");
    if(gui_ini.isEmpty())
        return false;

    IniProcessing guiset(gui_ini);

    int Animations = 0;
    guiset.beginGroup("gui");
    {
        guiset.read("editor-splash", splash_logo, "");
#ifdef __linux__
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        QString envir = env.value("XDG_CURRENT_DESKTOP", "");
#   if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
        qApp->setStyle("GTK3");
#   else
        qApp->setStyle("GTK");
#   endif
        if(envir == "KDE" || envir == "XFCE")
            ConfStatus::defaultTheme = "Breeze";
        else
            guiset.read("default-theme", ConfStatus::defaultTheme, "");
#elif __APPLE__
        ConfStatus::defaultTheme = "Breeze";
#elif _WIN32
        if(QSysInfo::WindowsVersion == QSysInfo::WV_WINDOWS10)
            ConfStatus::defaultTheme = "Breeze";
        else
            guiset.read("default-theme", ConfStatus::defaultTheme, "");
#else
        guiset.read("default-theme", ConfStatus::defaultTheme, "");
#endif
        guiset.read("animations", Animations, 0);
    }
    guiset.endGroup();

    guiset.beginGroup("widgets-default-visibility");
    {
        // Visibility
        guiset.read("lvl-itembox", editor.default_visibility.lvl_itembox, true);
        guiset.read("lvl-section-props", editor.default_visibility.lvl_section_props, false);
        guiset.read("lvl-warp-props", editor.default_visibility.lvl_warp_props, true);
        guiset.read("lvl-layers", editor.default_visibility.lvl_layers, true);
        guiset.read("lvl-events", editor.default_visibility.lvl_events, true);
        guiset.read("lvl-search", editor.default_visibility.lvl_search, false);

        guiset.read("wld-itembox", editor.default_visibility.wld_itembox, true);
        guiset.read("wld-settings", editor.default_visibility.wld_settings, false);
        guiset.read("wld-search", editor.default_visibility.wld_search, false);

        guiset.read("tilesets-box", editor.default_visibility.tilesets_box, true);
        guiset.read("debugger-box", editor.default_visibility.debugger_box, false);
        guiset.read("bookmarks-box", editor.default_visibility.bookmarks_box, false);
        guiset.read("variables-box", editor.default_visibility.variables_box, false);

        // Enforcing default value
        guiset.read("lvl-itembox-enforce-default", editor.default_visibility_enforce.lvl_itembox, false);
        guiset.read("lvl-section-props-enforce-default", editor.default_visibility_enforce.lvl_section_props, false);
        guiset.read("lvl-warp-props-enforce-default", editor.default_visibility_enforce.lvl_warp_props, false);
        guiset.read("lvl-layers-enforce-default", editor.default_visibility_enforce.lvl_layers, false);
        guiset.read("lvl-events-enforce-default", editor.default_visibility_enforce.lvl_events, false);
        guiset.read("lvl-search-enforce-default", editor.default_visibility_enforce.lvl_search, false);

        guiset.read("wld-itembox-enforce-default", editor.default_visibility_enforce.wld_itembox, false);
        guiset.read("wld-settings-enforce-default", editor.default_visibility_enforce.wld_settings, false);
        guiset.read("wld-search-enforce-default", editor.default_visibility_enforce.wld_search, false);

        guiset.read("tilesets-box-enforce-default", editor.default_visibility_enforce.tilesets_box, false);
        guiset.read("debugger-box-enforce-default", editor.default_visibility_enforce.debugger_box, false);
        guiset.read("bookmarks-box-enforce-default", editor.default_visibility_enforce.bookmarks_box, false);
        guiset.read("variables-box-enforce-default", editor.default_visibility_enforce.variables_box, false);
    }
    guiset.endGroup();

    guiset.beginGroup("file-formats");
    {
        const IniProcessing::StrEnumMap formatEnum =
        {
            {"smbx64", EditorSetup::DefaultFileFormats::SMBX64},
            {"pgex", EditorSetup::DefaultFileFormats::PGEX},
            {"smbx38a", EditorSetup::DefaultFileFormats::SMBX38A}
        };
        guiset.readEnum("level", editor.default_file_formats.level, EditorSetup::DefaultFileFormats::SMBX64, formatEnum);
        guiset.readEnum("world", editor.default_file_formats.world, EditorSetup::DefaultFileFormats::SMBX64, formatEnum);
    }
    guiset.endGroup();

    if(!openSection(&guiset, "main"))
        return false;

    data_dir = (guiset.value("application-dir", "0").toBool() ?
                ApplicationPath + "/" : config_dir + "data/");

    QString url     = guiset.value("home-page", "http://wohlsoft.ru/config_packs/").toQString();
    QString version = guiset.value("pge-editor-version", "0.0").toQString();
    bool    hasApiVersion = guiset.hasKey("api-version");
    unsigned int apiVersion = guiset.value("api-version", 1).toUInt();
    bool ver_notify = guiset.value("enable-version-notify", true).toBool();
    bool ver_invalid = false;

    if(hasApiVersion)
        ver_invalid = (apiVersion != V_CP_API) || (apiVersion < 41);
    else
        ver_invalid = (version != VersionCmp::compare(QString("%1").arg(V_FILE_VERSION), version));

    LogDebug(QString("Config pack version validation: "
                     "has API version: %1, has invalid version: %2")
                     .arg(hasApiVersion).arg(ver_invalid));

    if(ver_notify && ver_invalid)
    {
        LogWarning(QString("Config pack version is invalid: "
                           "has API version: %1, has invalid version: %2, current version %3, version wanted: %4")
                           .arg(hasApiVersion).arg(ver_invalid).arg(V_FILE_VERSION).arg(version));
        QMessageBox box;
        box.setWindowTitle("Legacy configuration package");
        box.setTextFormat(Qt::RichText);
        #if (QT_VERSION >= 0x050100)
        box.setTextInteractionFlags(Qt::TextBrowserInteraction);
        #endif
        box.setText(tr("You have a legacy configuration package.\n<br>"
                       "Editor will be started, but you may have a some problems with items or settings.\n<br>\n<br>"
                       "Please download and install latest version of a configuration package:\n<br>\n<br>Download: %1\n<br>"
                       "Note: most of config packs are updates togeter with PGE,<br>\n"
                       "therefore you can use same link to get updated version")
                    .arg("<a href=\"%1\">%1</a>").arg(url));
        box.setStandardButtons(QMessageBox::Ok);
        box.setIcon(QMessageBox::Warning);
        box.exec();
    }
    closeSection(&guiset);

    if(!splash_logo.isEmpty())
    {
        splash_logo = data_dir + splash_logo;
        if(QPixmap(splash_logo).isNull())
        {
            LogWarning(QString("Wrong splash image: %1, using internal default").arg(splash_logo));
            splash_logo = "";//Themes::Image(Themes::splash);
        }

        obj_splash_ani tempAni;

        animations.clear();
        for(; Animations > 0; Animations--)
        {
            guiset.beginGroup(QString("splash-animation-%1").arg(Animations).toStdString());
            {
                QString img =   guiset.value("image", "").toQString();
                if(img.isEmpty())
                    goto skip;
                tempAni.img = QPixmap(data_dir + img);
                if(tempAni.img.isNull())
                    goto skip;
                guiset.read("frames", tempAni.frames, 1);
                guiset.read("speed", tempAni.speed, 128);
                guiset.read("x", tempAni.x, 0);
                guiset.read("y", tempAni.y, 0);
                animations.push_front(tempAni);
            }
        skip:
            guiset.endGroup();
        }
    }

    return true;
}

bool dataconfigs::loadconfigs()
{
    total_data = 0;
    defaultGrid.general = 0;

    errorsList[ERR_GLOBAL].clear();
    errorsList[ERR_CUSTOM].clear();
    m_errOut = ERR_GLOBAL;

    LogDebug("=== Starting of global configuration loading ===");

    //dirs
    if((!QDir(config_dir).exists()) || (QFileInfo(config_dir).isFile()))
    {
        LogCritical(QString("CONFIG DIR NOT FOUND AT: %1").arg(config_dir));
        return false;
    }

    emit progressPartsTotal(12);
    emit progressPartNumber(0);

    QString main_ini = getFullIniPath("main.ini");
    if(main_ini.isEmpty())
        return false;

    IniProcessing mainSet(main_ini);

    QString customAppPath = ApplicationPath;

    LogDebug("Loading main.ini...");
    if(!openSection(&mainSet, "main"))
        return false;
    {
        mainSet.read("application-path", customAppPath, ApplicationPath);
        customAppPath.replace('\\', '/');
        bool lookAppDir = mainSet.value("application-dir", false).toBool();
        data_dir = (lookAppDir ? customAppPath + "/" : config_dir + "data/");
        if(QDir(ApplicationPath + "/" + data_dir).exists()) //Check as relative
            data_dir = ApplicationPath + "/" + data_dir;
        else if(!QDir(data_dir).exists()) //Check as absolute
        {
            LogCritical(QString("Config data path not exists: %1").arg(data_dir));
            return false;
        }

        data_dir = QDir(data_dir).absolutePath() + "/";
        ConfStatus::configDataPath = data_dir;

        mainSet.read("config_name", ConfStatus::configName, QDir(config_dir).dirName());
        #ifdef _WIN32
        mainSet.read("smbx-exe-name",           ConfStatus::SmbxEXE_Name,           "smbx.exe");
        mainSet.read("smbx-test-by-default",    ConfStatus::SmbxTest_By_Default,    false);
        #endif

        dirs.worlds     = data_dir + mainSet.value("worlds", "worlds").toQString() + "/";

        dirs.music      = data_dir + mainSet.value("music", "data/music").toQString() + "/";
        dirs.sounds     = data_dir + mainSet.value("sound", "data/sound").toQString() + "/";

        dirs.glevel     = data_dir + mainSet.value("graphics-level", "data/graphics/level").toQString() + "/";
        dirs.gworld     = data_dir + mainSet.value("graphics-worldmap", "data/graphics/worldmap").toQString() + "/";
        dirs.gplayble   = data_dir + mainSet.value("graphics-characters", "data/graphics/characters").toQString() + "/";

        localScriptName_lvl  = mainSet.value("local-script-name-lvl", "level.lua").toQString();
        commonScriptName_lvl = mainSet.value("common-script-name-lvl", "level.lua").toQString();
        localScriptName_wld  = mainSet.value("local-script-name-wld", "world.lua").toQString();
        commonScriptName_wld = mainSet.value("common-script-name-wld", "world.lua").toQString();

        dirs.gcustom = data_dir + mainSet.value("custom-data", "data-custom").toQString() + "/";
    }
    closeSection(&mainSet);

    //Check existing of most important graphics paths
    if(!QDir(dirs.glevel).exists())
    {
        LogCritical(QString("Level graphics path not exists: %1").arg(dirs.glevel));
        return false;
    }
    if(!QDir(dirs.gworld).exists())
    {
        LogCritical(QString("World map graphics path not exists: %1").arg(dirs.gworld));
        return false;
    }

    ConfStatus::configPath = config_dir;

    mainSet.beginGroup("graphics");
    defaultGrid.general = mainSet.value("default-grid", 32).toUInt();   //-V112
    mainSet.endGroup();

    if(mainSet.lastError() != IniProcessing::ERR_OK)
    {
        LogCriticalQD(QString("ERROR LOADING main.ini N:%1").arg(mainSet.lastError()));
        return false;
    }

    emit progressPartNumber(0);

    LogDebug("Loading some of engine.ini...");
    //Basic settings of engine
    QString engine_ini = config_dir + "engine.ini";
    if(QFile::exists(engine_ini)) //Load if exist, is not required
    {
        QSettings engineSet(engine_ini, QSettings::IniFormat);
        engineSet.setIniCodec("UTF-8");

        engineSet.beginGroup("common");
        engine.screen_w = engineSet.value("viewport-width", engine.screen_w).toUInt();
        engine.screen_h = engineSet.value("viewport-height", engine.screen_h).toUInt();
        engine.screen_w = engineSet.value("screen-width", engine.screen_w).toUInt();
        engine.screen_h = engineSet.value("screen-height", engine.screen_h).toUInt();
        engineSet.endGroup();

        engineSet.beginGroup("world-map");
        engine.wld_viewport_w = engineSet.value("viewport-width", engine.wld_viewport_w).toUInt();
        engine.wld_viewport_h = engineSet.value("viewport-height", engine.wld_viewport_h).toUInt();
        engineSet.endGroup();
    }


    ////////////////////////////////Preparing////////////////////////////////////////
    folderLvlBgo.graphics           = dirs.glevel +  "background/";
    folderLvlBG.graphics            = dirs.glevel +  "background2/";
    folderLvlBlocks.graphics        = dirs.glevel +  "block/";
    folderLvlNPC.graphics           = dirs.glevel +  "npc/";

    folderWldTerrain.graphics       = dirs.gworld +  "tile/";
    folderWldScenery.graphics       = dirs.gworld +  "scene/";
    folderWldPaths.graphics         = dirs.gworld +  "path/";
    folderWldLevelPoints.graphics   = dirs.gworld +  "level/";
    //////////////////////////////////////////////////////////////////////////////////


    ///////////////////////////////////////Level items////////////////////////////////////////////
    LogDebug("Loading of lvl_characters.ini...");
    loadPlayers();
    LogDebug("Loading of lvl_bkgrd.ini...");
    loadLevelBackgrounds();
    LogDebug("Loading of lvl_bgo.ini...");
    loadLevelBGO();
    LogDebug("Loading of lvl_blocks.ini...");
    loadLevelBlocks();
    LogDebug("Loading of lvl_npc.ini...");
    loadLevelNPC();
    ///////////////////////////////////////Level items////////////////////////////////////////////

    ///////////////////////////////////////World map items////////////////////////////////////////
    LogDebug("Loading of wld_tiles.ini...");
    loadWorldTiles();
    LogDebug("Loading of wld_scenery.ini...");
    loadWorldScene();
    LogDebug("Loading of wld_paths.ini...");
    loadWorldPaths();
    LogDebug("Loading of wld_levels.ini...");
    loadWorldLevels();
    ///////////////////////////////////////World map items////////////////////////////////////////


    //progress.setLabelText("Loading Music Data");
    ///////////////////////////////////////Music////////////////////////////////////////////
    LogDebug("Loading of music.ini...");
    loadMusic();
    ///////////////////////////////////////Music////////////////////////////////////////////

    ///////////////////////////////////////Sound////////////////////////////////////////////
    LogDebug("Loading of sounds.ini...");
    loadSound();
    ///////////////////////////////////////Sound////////////////////////////////////////////


    ///////////////////////////////////////Tilesets////////////////////////////////////////////
    LogDebug("Loading global tilesets INI files...");
    loadTilesets();
    ///////////////////////////////////////Tilesets////////////////////////////////////////////

    ///////////////////////////////////////Rotation rules table////////////////////////////////////////////
    LogDebug("Loading rotation_table.ini...");
    loadRotationTable();
    ///////////////////////////////////////Rotation rules table////////////////////////////////////////////

    emit progressMax(100);
    emit progressValue(100);
    emit progressTitle(QObject::tr("Finishing loading..."));

    /*if((!progress.wasCanceled())&&(!nobar))
        progress.close();*/

    LogDebug(QString("-------------------------"));
    LogDebug(QString("Config status 1"));
    LogDebug(QString("-------------------------"));
    LogDebug(QString("Loaded blocks          %1/%2").arg(main_block.stored()).arg(ConfStatus::total_blocks));
    LogDebug(QString("Loaded BGOs            %1/%2").arg(main_bgo.stored()).arg(ConfStatus::total_bgo));
    LogDebug(QString("Loaded NPCs            %1/%2").arg(main_npc.stored()).arg(ConfStatus::total_npc));
    LogDebug(QString("Loaded Backgrounds     %1/%2").arg(main_bg.stored()).arg(ConfStatus::total_bg));
    LogDebug(QString("Loaded Pl. Characters  %1/%2").arg(main_characters.size()).arg(ConfStatus::total_bg));
    LogDebug(QString("Loaded Tiles           %1/%2").arg(main_wtiles.stored()).arg(ConfStatus::total_wtile));
    LogDebug(QString("Loaded Sceneries       %1/%2").arg(main_wscene.stored()).arg(ConfStatus::total_wscene));
    LogDebug(QString("Loaded Path images     %1/%2").arg(main_wpaths.stored()).arg(ConfStatus::total_wpath));
    LogDebug(QString("Loaded Level images    %1/%2").arg(main_wlevels.stored()).arg(ConfStatus::total_wlvl));
    LogDebug(QString("Loaded Level music     %1/%2").arg(main_music_lvl.stored()).arg(ConfStatus::total_music_lvl));
    LogDebug(QString("Loaded Special music   %1/%2").arg(main_music_spc.stored()).arg(ConfStatus::total_music_spc));
    LogDebug(QString("Loaded World music     %1/%2").arg(main_music_wld.stored()).arg(ConfStatus::total_music_wld));
    LogDebug(QString("Loaded Sounds          %1/%2").arg(main_sound.stored()).arg(ConfStatus::total_sound));
    LogDebug(QString("-------------------------"));

    return true;
}

bool dataconfigs::check()
{
    return
        (
            (main_bgo.stored() <= 0) ||
            (main_bg.stored() <= 0) ||
            (main_block.stored() <= 0) ||
            (main_npc.stored() <= 0) ||
            (main_wtiles.stored() <= 0) ||
            (main_wscene.stored() <= 0) ||
            (main_wpaths.stored() <= 0) ||
            (main_wlevels.stored() <= 0) ||
            (main_music_lvl.stored() <= 0) ||
            (main_music_wld.stored() <= 0) ||
            (main_music_spc.stored() <= 0) ||
            (main_sound.stored() <= 0) ||
            !errorsList[ERR_GLOBAL].isEmpty()
                )/* && (!m_isValid)*/;
}

bool dataconfigs::checkCustom()
{
    return !errorsList[ERR_CUSTOM].isEmpty();
}

long dataconfigs::getCharacterI(unsigned long itemID)
{
    int j;
    bool found = false;

    for(j = 0; j < main_characters.size(); j++)
    {
        if(main_characters[j].id == itemID)
        {
            found = true;
            break;
        }
    }
    if(!found) j = -1;
    return static_cast<long>(j);
}


QString dataconfigs::getBgoPath()
{
    return folderLvlBgo.graphics;
}

QString dataconfigs::getBGPath()
{
    return folderLvlBG.graphics;
}

QString dataconfigs::getBlockPath()
{
    return folderLvlBlocks.graphics;
}

QString dataconfigs::getNpcPath()
{
    return folderLvlNPC.graphics;
}

QString dataconfigs::getTilePath()
{
    return folderWldTerrain.graphics;
}

QString dataconfigs::getScenePath()
{
    return folderWldScenery.graphics;
}

QString dataconfigs::getPathPath()
{
    return folderWldPaths.graphics;
}

QString dataconfigs::getWlvlPath()
{
    return folderWldLevelPoints.graphics;
}


QString dataconfigs::getBgoExtraSettingsPath()
{
    if(folderLvlBgo.extraSettings.isEmpty())
        return config_dir + "items/bgo";
    else
        return config_dir + folderLvlBgo.extraSettings;
}

QString dataconfigs::getBlockExtraSettingsPath()
{
    if(folderLvlBlocks.extraSettings.isEmpty())
        return config_dir + "items/blocks";
    else
        return config_dir + folderLvlBlocks.extraSettings;
}

QString dataconfigs::getNpcExtraSettingsPath()
{
    if(folderLvlNPC.extraSettings.isEmpty())
        return config_dir + "items/npc";
    else
        return config_dir + folderLvlNPC.extraSettings;
}

QString dataconfigs::getTileExtraSettingsPath()
{
    if(folderWldTerrain.extraSettings.isEmpty())
        return config_dir + "items/terrain";
    else
        return config_dir + folderWldTerrain.extraSettings;
}

QString dataconfigs::getSceneExtraSettingsPath()
{
    if(folderWldScenery.extraSettings.isEmpty())
        return config_dir + "items/scenery";
    else
        return config_dir + folderWldScenery.extraSettings;
}

QString dataconfigs::getPathExtraSettingsPath()
{
    if(folderWldPaths.extraSettings.isEmpty())
        return config_dir + "items/paths";
    else
        return config_dir + folderWldPaths.extraSettings;
}

QString dataconfigs::getWlvlExtraSettingsPath()
{
    if(folderWldLevelPoints.extraSettings.isEmpty())
        return config_dir + "items/levels";
    else
        return config_dir + folderWldLevelPoints.extraSettings;
}
