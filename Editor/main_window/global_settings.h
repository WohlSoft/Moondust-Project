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

class GlobalSettings
{
public:

    static LevelEditingSettings LvlOpts;

    static QString locale; //Current language
    static long animatorItemsLimit; //If level map have too many items, animation will be stopped

    //Paths
    static QString savePath;
    static QString savePath_npctxt;
    static QString openPath;

    //Music player
    static bool autoPlayMusic;
    static int musicVolume;

    //Toolboxes visibly states
    static bool LevelToolBoxVis;
    static bool SectionToolBoxVis;
    static bool LevelDoorsBoxVis;
    static bool LevelLayersBoxVis;
    static bool LevelEventsBoxVis;
    static bool LevelSearchBoxVis;

    static bool WorldToolBoxVis;
    static bool WorldSettingsToolboxVis;
    static bool WorldSearchBoxVis;

    static bool TilesetBoxVis;
    static bool DebuggerBoxVis;

    // Editor flags
    static bool MidMouse_allowDuplicate;
    static bool MidMouse_allowSwitchToPlace;
    static bool MidMouse_allowSwitchToDrag;

    //Last active file type state
    static int lastWinType;

    static QMdiArea::ViewMode MainWindowView;
    static QTabWidget::TabPosition LVLToolboxPos;
    static QTabWidget::TabPosition WLDToolboxPos;

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
