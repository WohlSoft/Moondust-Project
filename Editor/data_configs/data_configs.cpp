/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2023 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <DirManager/dirman.h>
#include <Utils/files.h>
#include <common_features/app_path.h>
#include <common_features/version_cmp.h>
#include <main_window/global_settings.h>
#include "../version.h"

#include "data_configs.h"

DataConfig::DataConfig()
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

QString DataConfig::getFullIniPath(QString iniFileName)
{
    QString path_ini = config_dir + iniFileName;
    if(!QFile::exists(path_ini))
    {
        addError(QString("ERROR LOADING %1: file does not exist").arg(iniFileName), PGE_LogLevel::Critical);
        return "";
    }
    return path_ini;
}

bool DataConfig::openSection(IniProcessing *config, const std::string &section, bool tryGeneral)
{
    //Check for availability of the INI section
    if(!config->beginGroup(section))
    {
        if(tryGeneral && config->beginGroup("General"))
            return true;//Allow section-less custom config files

        if(!tryGeneral) // Don't warn about blank custom files
        {
            addError(QString("ERROR LOADING %1: [%2] section is missed!")
                     .arg(StdToPGEString(config->fileName()))
                     .arg(StdToPGEString(section)), PGE_LogLevel::Critical);
        }
        return false;
    }
    return true;
}

void DataConfig::addError(QString bug, PGE_LogLevel level)
{
    WriteToLog(level, QString("LoadConfig -> %1").arg(bug));
    errorsList[m_errOut] << bug;
}

void DataConfig::setConfigPath(const QString &p, const QString &appDir)
{
    config_dir = p;
    if(!config_dir.endsWith('/'))
        config_dir.append('/');

    arg_config_app_dir = appDir;
    if(!arg_config_app_dir.isEmpty() && !arg_config_app_dir.endsWith('/'))
        arg_config_app_dir.append('/');
}

static void s_readWidgetDefaults(IniProcessing &s, QString name, EditorSetup::DefaultToolboxPositions::State &target, const EditorSetup::DefaultToolboxPositions::State &defaults)
{
    const IniProcessing::StrEnumMap stateEnum =
    {
        {"floating", EditorSetup::DefaultToolboxPositions::State::F_FLOATING},
        {"docked-left", EditorSetup::DefaultToolboxPositions::State::F_DOCKED_LEFT},
        {"docked-right", EditorSetup::DefaultToolboxPositions::State::F_DOCKED_RIGHT},
        {"docked-bottom", EditorSetup::DefaultToolboxPositions::State::F_DOCKED_BOTTOM},
    };
    s.readEnum(QString("%1-dock").arg(name).toUtf8(), target.dock, defaults.dock, stateEnum);
    s.read(QString("%1-x").arg(name).toUtf8(), target.x, defaults.x);
    s.read(QString("%1-y").arg(name).toUtf8(), target.y, defaults.y);
    s.read(QString("%1-width").arg(name).toUtf8(), target.width, defaults.width);
    s.read(QString("%1-height").arg(name).toUtf8(), target.height, defaults.height);
}

bool DataConfig::loadBasics()
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
        guiset.read("default-theme", ConfStatus::defaultTheme, "");
#ifdef __linux__
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        QString envir = env.value("XDG_CURRENT_DESKTOP", "");
#   if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
        qApp->setStyle("GTK3");
#   else
        qApp->setStyle("GTK");
#   endif
        if(ConfStatus::defaultTheme.isEmpty() && (envir == "KDE" || envir == "XFCE"))
            ConfStatus::defaultTheme = "Breeze";
#elif __APPLE__
        if(ConfStatus::defaultTheme.isEmpty())
            ConfStatus::defaultTheme = "Breeze";
#elif _WIN32
        if(ConfStatus::defaultTheme.isEmpty() && QSysInfo::WindowsVersion == QSysInfo::WV_WINDOWS10)
            ConfStatus::defaultTheme = "Breeze";
#endif
        guiset.read("animations", Animations, 0);
    }
    guiset.endGroup();

    guiset.beginGroup("widgets-default-state");
    {
        typedef EditorSetup::DefaultToolboxPositions::State S;
        s_readWidgetDefaults(guiset, "level-item-browser", editor.default_widget_state.level_item_browser, S(S::F_DOCKED_LEFT));
        s_readWidgetDefaults(guiset, "level-warps-box", editor.default_widget_state.level_warps_box, S(S::F_FLOATING));
        s_readWidgetDefaults(guiset, "level-item-properties", editor.default_widget_state.level_item_properties, S(S::F_FLOATING));
        s_readWidgetDefaults(guiset, "level-search-box", editor.default_widget_state.level_search_box, S(S::F_FLOATING));
        s_readWidgetDefaults(guiset, "level-classic-events-box", editor.default_widget_state.level_classic_events_box, S(S::F_FLOATING));
        s_readWidgetDefaults(guiset, "level-layers-box", editor.default_widget_state.level_layers_box, S(S::F_FLOATING));
        s_readWidgetDefaults(guiset, "level-section-properties", editor.default_widget_state.level_section_properties, S(S::F_FLOATING));

        s_readWidgetDefaults(guiset, "world-item-browser", editor.default_widget_state.world_item_browser, S(S::F_DOCKED_LEFT));
        s_readWidgetDefaults(guiset, "world-music-boxes", editor.default_widget_state.world_music_boxes, S(S::F_DOCKED_LEFT));
        s_readWidgetDefaults(guiset, "world-item-properties", editor.default_widget_state.world_item_properties, S(S::F_FLOATING));
        s_readWidgetDefaults(guiset, "world-settings-box", editor.default_widget_state.world_settings_box, S(S::F_FLOATING));
        s_readWidgetDefaults(guiset, "world-search-box", editor.default_widget_state.world_search_box, S(S::F_FLOATING));

        s_readWidgetDefaults(guiset, "bookmarks-box", editor.default_widget_state.bookmarks_box, S(S::F_FLOATING));
        s_readWidgetDefaults(guiset, "debugger-box", editor.default_widget_state.debugger_box, S(S::F_FLOATING));
        s_readWidgetDefaults(guiset, "variables-box", editor.default_widget_state.variables_box, S(S::F_FLOATING));
        s_readWidgetDefaults(guiset, "tilesets-item-box", editor.default_widget_state.tilesets_item_box, S(S::F_FLOATING));
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
        guiset.read("wld-musicboxes", editor.default_visibility.wld_musicboxes, false);
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
        guiset.read("wld-musicboxes-enforce-default", editor.default_visibility_enforce.wld_musicboxes, false);
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

    guiset.beginGroup("editor-defaults");
    {
        guiset.read("enable-first-launch-greeting", editor.enable_first_launch_greeting, true);
        guiset.read("enable-tip-of-the-day", editor.enable_tip_of_the_day, true);
    }
    guiset.endGroup();

    guiset.beginGroup("supported-features");
    {
        const IniProcessing::StrEnumMap formatEnum =
        {
                {"false", EditorSetup::FeaturesSupport::F_DISABLED},
                {"disabled", EditorSetup::FeaturesSupport::F_DISABLED},
                {"inactive", EditorSetup::FeaturesSupport::F_DISABLED},
                {"0", EditorSetup::FeaturesSupport::F_DISABLED},

                {"enabled", EditorSetup::FeaturesSupport::F_ENABLED},
                {"active", EditorSetup::FeaturesSupport::F_ENABLED},
                {"true", EditorSetup::FeaturesSupport::F_ENABLED},
                {"1", EditorSetup::FeaturesSupport::F_ENABLED},

                {"hidden", EditorSetup::FeaturesSupport::F_HIDDEN},
                {"2", EditorSetup::FeaturesSupport::F_HIDDEN}
        };

        EditorSetup::FeaturesSupport::State defaultState = EditorSetup::FeaturesSupport::F_ENABLED;
        // Default state for all fields listed below
        guiset.readEnum("default", defaultState, EditorSetup::FeaturesSupport::F_ENABLED, formatEnum);

        guiset.readEnum("level-section-vertical-wrap", editor.supported_features.level_section_vertical_wrap, defaultState, formatEnum);

        guiset.readEnum("level-section-21-plus", editor.supported_features.level_section_21plus, defaultState, formatEnum);

        guiset.readEnum("level-phys-ez-new-types", editor.supported_features.level_phys_ez_new_types, defaultState, formatEnum);

        guiset.readEnum("level-bgo-z-layer", editor.supported_features.level_bgo_z_layer, defaultState, formatEnum);
        guiset.readEnum("level-bgo-z-position", editor.supported_features.level_bgo_z_position, defaultState, formatEnum);
        guiset.readEnum("level-bgo-smbx64-sp", editor.supported_features.level_bgo_smbx64_sp, defaultState, formatEnum);

        guiset.readEnum("level-warp-transition-type", editor.supported_features.level_warp_transit_type, defaultState, formatEnum);

        guiset.readEnum("level-warp-two-way", editor.supported_features.level_warp_two_way, defaultState, formatEnum);
        guiset.readEnum("level-warp-portal", editor.supported_features.level_warp_portal, defaultState, formatEnum);
        guiset.readEnum("level-warp-bomb-exit", editor.supported_features.level_warp_bomb_exit, defaultState, formatEnum);
        guiset.readEnum("level-warp-allow-special-state-only", editor.supported_features.level_warp_allow_sp_state_only, defaultState, formatEnum);
        guiset.readEnum("level-warp-allow-needs-floor", editor.supported_features.level_warp_allow_needs_floor, defaultState, formatEnum);
        guiset.readEnum("level-warp-hide-interlevel-scene", editor.supported_features.level_warp_hide_interlevel_scene, defaultState, formatEnum);
        guiset.readEnum("level-warp-allow-interlevel-npc", editor.supported_features.level_warp_allow_interlevel_npc, defaultState, formatEnum);
        guiset.readEnum("level-warp-hide-stars", editor.supported_features.level_warp_hide_stars, defaultState, formatEnum);
        guiset.readEnum("level-warp-needed-stars-message", editor.supported_features.level_warp_needstars_message, defaultState, formatEnum);
        guiset.readEnum("level-warp-on-enter-event", editor.supported_features.level_warp_on_enter_event, defaultState, formatEnum);
        guiset.readEnum("level-warp-cannon-exit", editor.supported_features.level_warp_cannon_exit, defaultState, formatEnum);

        guiset.readEnum("level-event-new-autoscroll", editor.supported_features.level_event_new_autoscroll, defaultState, formatEnum);
    }
    guiset.endGroup();

    guiset.beginGroup("compatibility");
    {
        guiset.read("extra-settings-local-at-root", m_extraSettingsLocalAtRoot, true);
    }
    guiset.endGroup();

    if(!openSection(&guiset, "main"))
        return false;

    if(guiset.value("application-dir", false).toBool())
        data_dir = arg_config_app_dir.isEmpty() ? ApplicationPath + "/" : arg_config_app_dir;
    else
        data_dir = config_dir + "data/";

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
        QString config_data = config_dir + "data/";
        splash_logo = config_data + splash_logo;
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
                tempAni.img = QPixmap(config_data + img);
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

bool DataConfig::loadFullConfig()
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
        emit errorOccured();
        return false;
    }

    emit progressPartsTotal(12);
    emit progressPartNumber(0);

    QString main_ini = getFullIniPath("main.ini");
    if(main_ini.isEmpty())
    {
        emit errorOccured();
        return false;
    }

    IniProcessing mainSet(main_ini);

    // PreSetup
    QString preSetup_customAppPath = ApplicationPath;
    bool    preSetup_lookAppDir = false;
    QString preSetup_worlds;
    QString preSetup_music;
    QString preSetup_sound;
    QString preSetup_gfxLevel;
    QString preSetup_gfxWorldMap;
    QString preSetup_gfxCharacters;
    QString preSetup_custom;
    // PreSetup END

    // Load the main.ini settings (read-only)
    LogDebug("Loading main.ini...");
    if(!openSection(&mainSet, "main"))
    {
        emit errorOccured();
        return false;
    }
    {
        mainSet.read("application-path", preSetup_customAppPath, ApplicationPath);
        preSetup_customAppPath.replace('\\', '/');
        preSetup_lookAppDir = mainSet.value("application-dir", false).toBool();

        QString cpDirName = QDir(config_dir).dirName();

        mainSet.read("config_name", ConfStatus::configName, cpDirName);
        // Default executable name for the LunaTester
        mainSet.read("smbx-exe-name",  ConfStatus::SmbxEXE_Name, "smbx.exe");

        mainSet.read("config-pack-id", configPackId, QString());
        mainSet.read("target-engine-name", targetEngineName, QString());

        mainSet.readEnum("default-engine-type",
                         ConfStatus::defaultTestEngine,
                         ConfStatus::ENGINE_MOONDUST,
        {
            {"moondust", ConfStatus::ENGINE_MOONDUST},
            {"pge", ConfStatus::ENGINE_MOONDUST},

            {"luna", ConfStatus::ENGINE_LUNA},
            {"lunatester", ConfStatus::ENGINE_LUNA},
            {"smbx", ConfStatus::ENGINE_LUNA},

            {"xtech", ConfStatus::ENGINE_THEXTECH},
            {"thextech", ConfStatus::ENGINE_THEXTECH},

            {"38a", ConfStatus::ENGINE_38A},
            {"smbx38a", ConfStatus::ENGINE_38A},
        });
        mainSet.read("hide-non-default-engines", ConfStatus::hideNonDefaultEngines, false);

        { // Deprecated, kept for compatibility
            bool lunaDefault = false;
            bool hidePgeEngine = false;

            mainSet.read("smbx-test-by-default", lunaDefault, false);
            mainSet.read("smbx-test-hide-pge-engine-menu", hidePgeEngine, false);

            if(lunaDefault)
            {
                ConfStatus::defaultTestEngine = ConfStatus::ENGINE_LUNA;
                ConfStatus::hideNonDefaultEngines = hidePgeEngine;
            }
        }

        preSetup_worlds = mainSet.value("worlds", "worlds").toQString();

        preSetup_music = mainSet.value("music", "data/music").toQString();
        preSetup_sound = mainSet.value("sound", "data/sound").toQString();

        preSetup_gfxLevel = mainSet.value("graphics-level", "data/graphics/level").toQString();
        preSetup_gfxWorldMap = mainSet.value("graphics-worldmap", "data/graphics/worldmap").toQString();
        preSetup_gfxCharacters = mainSet.value("graphics-characters", "data/graphics/characters").toQString();

        localScriptName_lvl  = mainSet.value("local-script-name-lvl", "level.lua").toQString();
        commonScriptName_lvl = mainSet.value("common-script-name-lvl", "level.lua").toQString();
        localScriptName_wld  = mainSet.value("local-script-name-wld", "world.lua").toQString();
        commonScriptName_wld = mainSet.value("common-script-name-wld", "world.lua").toQString();

        preSetup_custom = mainSet.value("custom-data", "data-custom").toQString();
    }
    closeSection(&mainSet);


    // Load the local config pack settings (writable)
    ConfStatus::configLocalSettingsFile = buildLocalConfigPath(config_dir);
    {
        IniProcessing localSet(ConfStatus::configLocalSettingsFile);
        if(localSet.contains("main"))
        {
            localSet.beginGroup("main");
            localSet.read("application-path", preSetup_customAppPath, preSetup_customAppPath);
            preSetup_customAppPath.replace('\\', '/');
            preSetup_lookAppDir = localSet.value("application-dir", preSetup_lookAppDir).toBool();

            localSet.read("smbx-exe-name",  ConfStatus::SmbxEXE_Name, ConfStatus::SmbxEXE_Name);

            preSetup_worlds = localSet.value("worlds", preSetup_worlds).toQString();

            preSetup_music = localSet.value("music", preSetup_music).toQString();
            preSetup_sound = localSet.value("sound", preSetup_sound).toQString();

            preSetup_gfxLevel = localSet.value("graphics-level", preSetup_gfxLevel).toQString();
            preSetup_gfxWorldMap = localSet.value("graphics-worldmap", preSetup_gfxWorldMap).toQString();
            preSetup_gfxCharacters = localSet.value("graphics-characters", preSetup_gfxCharacters).toQString();
            preSetup_custom = localSet.value("custom-data", preSetup_custom).toQString();
            localSet.endGroup();
        }
    }

    // ================ Apply pre-Setup ================
    QString appPath = arg_config_app_dir.isEmpty() ? ApplicationPath : arg_config_app_dir;
    data_dir = (preSetup_lookAppDir ? preSetup_customAppPath + "/" : config_dir + "data/");

    if(QDir(appPath + "/" + data_dir).exists()) //Check as relative
        data_dir = appPath + "/" + data_dir;
    else if(!QDir(data_dir).exists()) //Check as absolute
    {
        LogCritical(QString("Config data path not exists: %1").arg(data_dir));
        emit errorOccured();
        return false;
    }

    data_dir = QDir(data_dir).absolutePath() + "/";
    ConfStatus::configDataPath = data_dir;

    dirs.worlds     = data_dir + preSetup_worlds + "/";

    dirs.music      = data_dir + preSetup_music + "/";
    dirs.sounds     = data_dir + preSetup_sound + "/";

    dirs.glevel     = data_dir + preSetup_gfxLevel + "/";
    dirs.gworld     = data_dir + preSetup_gfxWorldMap + "/";
    dirs.gplayble   = data_dir + preSetup_gfxCharacters + "/";
    dirs.gcustom = preSetup_custom;
    // ================ Apply pre-Setup ================



    //Check existing of most important graphics paths
    if(!QDir(dirs.glevel).exists())
    {
        LogCritical(QString("Level graphics path not exists: %1").arg(dirs.glevel));
        emit errorOccured();
        return false;
    }
    if(!QDir(dirs.gworld).exists())
    {
        LogCritical(QString("World map graphics path not exists: %1").arg(dirs.gworld));
        emit errorOccured();
        return false;
    }

    ConfStatus::configPath = config_dir;

    mainSet.beginGroup("graphics");
    defaultGrid.general = mainSet.value("default-grid", 32).toUInt();   //-V112
    mainSet.endGroup();

    if(mainSet.lastError() != IniProcessing::ERR_OK)
    {
        LogCriticalQD(QString("ERROR LOADING main.ini N:%1").arg(mainSet.lastError()));
        emit errorOccured();
        return false;
    }

    emit progressPartNumber(0);

    LogDebug("Loading some of engine.ini...");
    //Basic settings of engine
    QString engine_ini = config_dir + "engine.ini";
    if(QFile::exists(engine_ini)) //Load if exist, is not required
    {
        QSettings engineSet(engine_ini, QSettings::IniFormat);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        engineSet.setIniCodec("UTF-8");
#endif

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
    LogDebug("Loading of vehicles.ini...");
    loadVehicles();
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

    ///////////////////////////////////////Level exit codes////////////////////////////////////////////
    LogDebug("Loading wld_exit_types.ini...");
    loadExitCodes();
    ///////////////////////////////////////Level exit codes////////////////////////////////////////////

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

bool DataConfig::check()
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

bool DataConfig::checkCustom()
{
    return !errorsList[ERR_CUSTOM].isEmpty();
}

long DataConfig::getCharacterI(unsigned long itemID)
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


QString DataConfig::getBgoPath()
{
    return folderLvlBgo.graphics;
}

QString DataConfig::getBGPath()
{
    return folderLvlBG.graphics;
}

QString DataConfig::getBlockPath()
{
    return folderLvlBlocks.graphics;
}

QString DataConfig::getNpcPath()
{
    return folderLvlNPC.graphics;
}

QString DataConfig::getTilePath()
{
    return folderWldTerrain.graphics;
}

QString DataConfig::getScenePath()
{
    return folderWldScenery.graphics;
}

QString DataConfig::getPathPath()
{
    return folderWldPaths.graphics;
}

QString DataConfig::getWlvlPath()
{
    return folderWldLevelPoints.graphics;
}

bool DataConfig::isExtraSettingsLocalAtRoot()
{
    return m_extraSettingsLocalAtRoot;
}


QString DataConfig::getBgoExtraSettingsPath()
{
    if(folderLvlBgo.extraSettings.isEmpty())
        return config_dir + "items/bgo";
    else
    {
        if(Files::isAbsolute(folderLvlBgo.extraSettings.toStdString()))
            return folderLvlBgo.extraSettings;
        return config_dir + folderLvlBgo.extraSettings;
    }
}

QString DataConfig::getBlockExtraSettingsPath()
{
    if(folderLvlBlocks.extraSettings.isEmpty())
        return config_dir + "items/blocks";
    else
    {
        if(Files::isAbsolute(folderLvlBlocks.extraSettings.toStdString()))
            return folderLvlBlocks.extraSettings;
        return config_dir + folderLvlBlocks.extraSettings;
    }
}

QString DataConfig::getNpcExtraSettingsPath()
{
    if(folderLvlNPC.extraSettings.isEmpty())
        return config_dir + "items/npc";
    else
    {
        if(Files::isAbsolute(folderLvlNPC.extraSettings.toStdString()))
            return folderLvlNPC.extraSettings;
        return config_dir + folderLvlNPC.extraSettings;
    }
}

QString DataConfig::getTileExtraSettingsPath()
{
    if(folderWldTerrain.extraSettings.isEmpty())
        return config_dir + "items/terrain";
    else
    {
        if(Files::isAbsolute(folderWldTerrain.extraSettings.toStdString()))
            return folderWldTerrain.extraSettings;
        return config_dir + folderWldTerrain.extraSettings;
    }
}

QString DataConfig::getSceneExtraSettingsPath()
{
    if(folderWldScenery.extraSettings.isEmpty())
        return config_dir + "items/scenery";
    else
    {
        if(Files::isAbsolute(folderWldScenery.extraSettings.toStdString()))
            return folderWldScenery.extraSettings;
        return config_dir + folderWldScenery.extraSettings;
    }
}

QString DataConfig::getPathExtraSettingsPath()
{
    if(folderWldPaths.extraSettings.isEmpty())
        return config_dir + "items/paths";
    else
    {
        if(Files::isAbsolute(folderWldPaths.extraSettings.toStdString()))
            return folderWldPaths.extraSettings;
        return config_dir + folderWldPaths.extraSettings;
    }
}

QString DataConfig::getWlvlExtraSettingsPath()
{
    if(folderWldLevelPoints.extraSettings.isEmpty())
        return config_dir + "items/levels";
    else
    {
        if(Files::isAbsolute(folderWldLevelPoints.extraSettings.toStdString()))
            return folderWldLevelPoints.extraSettings;
        return config_dir + folderWldLevelPoints.extraSettings;
    }
}

QString DataConfig::buildLocalConfigPath(const QString &configPackPath)
{
    QString cpDirName = QDir(configPackPath).dirName();
    return AppPathManager::settingsPath() + "/pge_editor_config_" + cpDirName + ".ini";
}
