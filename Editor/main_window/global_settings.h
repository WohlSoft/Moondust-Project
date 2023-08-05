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

#pragma once
#ifndef GLOBAL_SETTINGS_H
#define GLOBAL_SETTINGS_H

#include <memory>
#include <QString>
#include <QMdiArea>

struct EditingSettings
{
    EditingSettings();
    // Common
    bool animationEnabled = true;
    bool collisionsEnabled = true;
    bool grid_snap = true;
    bool grid_override = false;
    QSize customGrid;
    bool grid_show = false;
    bool camera_grid_show = false;
    unsigned int default_zoom = 100;
    // World map only
    bool semiTransparentPaths = false;
    // Auto-saving
    bool autoSave_enable = true;
    int  autoSave_interval = 3; //!< Autosave interval in minutes
};

struct SETTINGS_ItemDefaults
{
    double LockedItemOpacity;
    int npc_direction;
    int npc_generator_type;
    int npc_generator_delay;
    int warp_type;
    bool classicevents_tabs_layviz;
    bool classicevents_tabs_laymov;
    bool classicevents_tabs_autoscroll;
    bool classicevents_tabs_secset;
    bool classicevents_tabs_common;
    bool classicevents_tabs_buttons;
    bool classicevents_tabs_trigger;
};

struct SETTINGS_TestSettings
{
    //! Custom path to PGE Engine (in case if original was not found)
    QString enginePath;
    //! Custom path to LunaLua (in case if original was not found)
    QString lunaLuaPath;
    bool xtra_god = false;
    bool xtra_flyup = false;
    bool xtra_chuck = false;
    bool xtra_debug = false;
    bool xtra_showFPS = false;
    bool xtra_physdebug = false;
    bool xtra_worldfreedom = false;
    int  xtra_starsNum = 0;
    int  numOfPlayers = 1;
    int  p1_char = 1;
    int  p1_state = 1;
    int  p1_vehicleID = 0;
    int  p1_vehicleType = 0;
    int  p2_char = 2;
    int  p2_state = 1;
    int  p2_vehicleID = 0;
    int  p2_vehicleType = 0;
};

struct SETTINGS_ScreenGrabSettings
{
    enum GrabSize
    {
        GRAB_Fit,
        GRAB_Custom
    };
    int sizeType = GRAB_Fit;
    int width   = 800;
    int height  = 600;
};

struct SETTINGS_Extra
{
    bool attr_hdpi = true;
    QString maintainerPath;
};

/**
 * @brief A container of globally visible settings
 */
class GlobalSettings
{
    // TODO: Make it being a non-static global singleton
public:
    static EditingSettings  LvlOpts;
    static SETTINGS_ItemDefaults LvlItemDefaults;

    //! Testing settings
    static SETTINGS_TestSettings testing;
    //! Screen capture settings
    static SETTINGS_ScreenGrabSettings screenGrab;
    //! Extra settings
    static SETTINGS_Extra extra;

    //! Current language
    static QString locale;
    //! Max limit of animated elements (on exciding, animation will be paused)
    static long animatorItemsLimit;//If level map have too many items, animation will be stopped

    //Paths
    //! Recent file-save path for Levels and World maps
    static QString savePath;
    //! Recent file-save path for NPC.txt files
    static QString savePath_npctxt;
    //! Recent open path
    static QString openPath;

    //External tools
    //! Path to SoX utility
    static QString tools_sox_bin_path;

    //Music player
    //! Play music automatically on file open
    static bool autoPlayMusic;
    //! Music volume
    static int  musicVolume;
    //! Recent state of the music playing button
    static bool recentMusicPlayingState;

    // Editor flags
    //! Enalbe middle-mouse group duplicating feature
    static bool MidMouse_allowDuplicate;
    //! Enalbe middle-mouse quick-grabbing-for-place feature
    static bool MidMouse_allowSwitchToPlace;
    //! Enalbe middle-mouse switch to drag mode feature
    static bool MidMouse_allowSwitchToDrag;

    //Placement mode flahs
    static bool Placing_dontShowPropertiesBox;

    //!Max Limit if history elements
    static int  historyLimit;

    //!Last active file type state
    static int  lastWinType;

    static QMdiArea::ViewMode       MainWindowView;
    static QTabWidget::TabPosition  LVLToolboxPos;
    static QTabWidget::TabPosition  WLDToolboxPos;
    static QTabWidget::TabPosition  TSTToolboxPos;
    static QString                  currentTheme;
    static int                      currentPallete;

    static std::unique_ptr<QFont>   fontDefault;
    static std::unique_ptr<QFont>   font;
    static int                      fontSize;

    static bool ShowTipOfDay;

    static void cleanUp();
};


/**
 * @brief A container of current config pack status information
 */
class ConfStatus
{
    // TODO: Make it being a non-static global singleton
public:
    static long total_characters;
    static long total_blocks;
    static long total_bgo;
    static long total_bg;
    static long total_npc;

    static long total_wtile;
    static long total_wpath;
    static long total_wscene;
    static long total_wlvl;

    static long total_music_lvl;
    static long total_music_wld;
    static long total_music_spc;

    static long total_sound;

    static QString configName;
    static QString defaultTheme;
    static QString configPath;
    //! Config pack specific settings (they won't go between config packs when reloading)
    static QString configLocalSettingsFile;
    //! Identify the configuration package is integrational (opposed to standalone)
    static bool    configIsIntegrational;
    static QString configConfigureTool;

    static QString configDataPath;

    static QString SmbxEXE_Name;

    enum TestEngineType
    {
        ENGINE_MOONDUST,
        ENGINE_LUNA,
        ENGINE_THEXTECH,
        ENGINE_38A
    };
    static TestEngineType defaultTestEngine;
    static bool           hideNonDefaultEngines;
};

#endif // GLOBAL_SETTINGS_H
