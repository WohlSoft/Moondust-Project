/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2025 Vitaly Novichkov <admin@wohlnet.ru>
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
#ifndef MUSIC_PLAYER_H
#define MUSIC_PLAYER_H

#include <QString>

class MainWindow;

class LvlMusPlay
{
public:
    enum MusicType{
        LevelMusic = 0,
        WorldMusic,
        SpecialMusic
    };

    static QString currentCustomMusic;

    static QString currentMusicPath;
    static QString lastMusicPath;


    static long currentMusicId;
    static bool musicButtonChecked;
    static bool musicForceReset;
    static long currentWldMusicId;
    static long currentSpcMusicId;
    static int musicType;

    static void setMusic(MainWindow *mw, MusicType mt, unsigned long id, QString cmus);
    static void setNoMusic();
    static void setTempo(double tempo);
    static void setGain(double gain);
    static void updateMusic(MainWindow *mw);
    static void updatePlayerState(MainWindow *mw, bool playing = true, bool force = false);
    static void stopMusic(MainWindow *mw);

    static void previewCustomMusic(MainWindow *mw, QString cmus);
    static void previewCustomMusicAbs(QString absMusicPath);
    static void previewSilence();
    static void previewReset(MainWindow *mw);

};

#endif // MUSIC_PLAYER_H
