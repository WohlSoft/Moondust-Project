/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef GLOBAL_SETTINGS_H
#define GLOBAL_SETTINGS_H

#include <QString>
#include <QMdiArea>

struct LevelEditingSettings
{
    //Common
    bool animationEnabled;
    bool collisionsEnabled;
    //World map only
    bool semiTransparentPaths;
};

struct SETTINGS_ItemDefaults {
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

struct SETTINGS_TestSettings {
    bool xtra_god;
    bool xtra_flyup;
    bool xtra_chuck;
    bool xtra_debug;
    bool xtra_physdebug;
    bool xtra_worldfreedom;
    int  numOfPlayers;
    int  p1_char;
    int  p1_state;
    int  p2_char;
    int  p2_state;
};

class GlobalSettings
{
public:

    static LevelEditingSettings LvlOpts;
    static SETTINGS_ItemDefaults LvlItemDefaults;

    static SETTINGS_TestSettings testing; //Testing settings

    static QString locale; //Current language
    static long animatorItemsLimit; //If level map have too many items, animation will be stopped

    //Paths
    static QString savePath;
    static QString savePath_npctxt;
    static QString openPath;

    //External tools
    static QString tools_sox_bin_path;

    //Music player
    static bool autoPlayMusic;
    static int musicVolume;

    //Toolboxes visibly states
    static bool LevelItemBoxVis;
    static bool LevelSectionBoxVis;
    static bool LevelDoorsBoxVis;
    static bool LevelLayersBoxVis;
    static bool LevelEventsBoxVis;
    static bool LevelSearchBoxVis;

    static bool WorldItemBoxVis;
    static bool WorldSettingsToolboxVis;
    static bool WorldSearchBoxVis;

    static bool TilesetBoxVis;
    static bool DebuggerBoxVis;
    static bool BookmarksBoxVis;

    // Editor flags
    static bool MidMouse_allowDuplicate;
    static bool MidMouse_allowSwitchToPlace;
    static bool MidMouse_allowSwitchToDrag;

    //Placing mode flahs
    static bool Placing_dontShowPropertiesBox;

    //Max Limit if history elements
    static int historyLimit;

    //Last active file type state
    static int lastWinType;

    static QMdiArea::ViewMode MainWindowView;
    static QTabWidget::TabPosition LVLToolboxPos;
    static QTabWidget::TabPosition WLDToolboxPos;
    static QTabWidget::TabPosition TSTToolboxPos;
    static QString currentTheme;

    static bool ShowTipOfDay;
};

class ConfStatus
{
//Configuration status
public:
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

};

#endif // GLOBAL_SETTINGS_H
