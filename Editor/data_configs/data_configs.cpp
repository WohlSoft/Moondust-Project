/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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
    m_isValid = false;

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

dataconfigs::~dataconfigs()
{}

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

bool dataconfigs::openSection(IniProcessing *config, const std::string &section)
{
    //Cache name of recent INI-file and it's sections
    if(m_recentIniFile != config->fileName())
    {
        m_recentIniFile = config->fileName();
    }

    //Check for availability of the INI section
    if(!config->beginGroup(section))
    {
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
        qApp->setStyle("GTK");
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

    if(!openSection(&guiset, "main"))
        return false;

    data_dir = (guiset.value("application-dir", "0").toBool() ?
                ApplicationPath + "/" : config_dir + "data/");

    QString url     = guiset.value("home-page", "http://wohlsoft.ru/config_packs/").toQString();
    QString version = guiset.value("pge-editor-version", "0.0").toQString();
    bool ver_notify = guiset.value("enable-version-notify", true).toBool();
    if(ver_notify && (version != VersionCmp::compare(QString("%1").arg(_FILE_VERSION), version)))
    {
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

    m_recentIniFile.clear();

    return true;
}

bool dataconfigs::loadconfigs()
{
    //unsigned long i;//, prgs=0;

    m_isValid = false;

    total_data = 0;
    defaultGrid.general = 0;

    errorsList[ERR_GLOBAL].clear();
    errorsList[ERR_CUSTOM].clear();
    m_errOut = ERR_GLOBAL;

    m_recentIniFile.clear();

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

    IniProcessing mainset(main_ini);

    QString customAppPath = ApplicationPath;

    LogDebug("Loading main.ini...");
    if(!openSection(&mainset, "main"))
        return false;
    {
        mainset.read("application-path", customAppPath, ApplicationPath);
        customAppPath.replace('\\', '/');
        bool lookAppDir = mainset.value("application-dir", false).toBool();
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

        mainset.read("config_name", ConfStatus::configName, QDir(config_dir).dirName());
        #ifdef _WIN32
        mainset.read("smbx-exe-name",           ConfStatus::SmbxEXE_Name,           "smbx.exe");
        mainset.read("smbx-test-by-default",    ConfStatus::SmbxTest_By_Default,    false);
        #endif

        dirs.worlds     = data_dir + mainset.value("worlds", "worlds").toQString() + "/";

        dirs.music      = data_dir + mainset.value("music", "data/music").toQString() + "/";
        dirs.sounds     = data_dir + mainset.value("sound", "data/sound").toQString() + "/";

        dirs.glevel     = data_dir + mainset.value("graphics-level", "data/graphics/level").toQString() + "/";
        dirs.gworld     = data_dir + mainset.value("graphics-worldmap", "data/graphics/worldmap").toQString() + "/";
        dirs.gplayble   = data_dir + mainset.value("graphics-characters", "data/graphics/characters").toQString() + "/";

        localScriptName_lvl  = mainset.value("local-script-name-lvl", "level.lua").toQString();
        commonScriptName_lvl = mainset.value("common-script-name-lvl", "level.lua").toQString();
        localScriptName_wld  = mainset.value("local-script-name-wld", "world.lua").toQString();
        commonScriptName_wld = mainset.value("common-script-name-wld", "world.lua").toQString();

        dirs.gcustom = data_dir + mainset.value("custom-data", "data-custom").toQString() + "/";
    }
    closeSection(&mainset);

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

    mainset.beginGroup("graphics");
    defaultGrid.general = mainset.value("default-grid", 32).toUInt();   //-V112
    mainset.endGroup();

    if(mainset.lastError() != IniProcessing::ERR_OK)
    {
        LogCriticalQD(QString("ERROR LOADING main.ini N:%1").arg(mainset.lastError()));
        return false;
    }

    emit progressPartNumber(0);

    LogDebug("Loading some of engine.ini...");
    //Basic settings of engine
    QString engine_ini = config_dir + "engine.ini";
    if(QFile::exists(engine_ini)) //Load if exist, is not required
    {
        QSettings engineset(engine_ini, QSettings::IniFormat);
        engineset.setIniCodec("UTF-8");

        engineset.beginGroup("common");
        engine.screen_w = engineset.value("screen-width", engine.screen_w).toInt();
        engine.screen_h = engineset.value("screen-height", engine.screen_h).toInt();
        engineset.endGroup();

        engineset.beginGroup("world-map");
        engine.wld_viewport_w = engineset.value("viewport-width", engine.wld_viewport_w).toInt();
        engine.wld_viewport_h = engineset.value("viewport-height", engine.wld_viewport_h).toInt();
        engineset.endGroup();
    }


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

    m_recentIniFile.clear();
    LogDebug(QString("-------------------------"));

    m_isValid = !check();

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

