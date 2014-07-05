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
    bool animationEnabled;
    bool collisionsEnabled;
};

class GlobalSettings
{
public:

    static LevelEditingSettings LvlOpts;

    static QString locale;
    static long animatorItemsLimit;

    static QString savePath;
    static QString savePath_npctxt;
    static QString openPath;

    static bool autoPlayMusic;

    //Toolboxes visibly states
    static bool LevelToolBoxVis;
    static bool WorldToolBoxVis;
    static bool SectionToolBoxVis;
    static bool LevelDoorsBoxVis;
    static bool LevelLayersBoxVis;
    static bool LevelEventsBoxVis;
    static bool LevelSearchBoxVis;

    //Last active file type state
    static int lastWinType;

    static QMdiArea::ViewMode MainWindowView;

};

class ConfStatus
{
//Configuration status
public:
    static long total_blocks;
    static long total_bgo;
    static long total_bg;
    static long total_npc;

    static long total_music_lvl;
    static long total_music_wld;
    static long total_music_spc;

    static long total_sound;

    static QString configName;

};

#endif // GLOBAL_SETTINGS_H
