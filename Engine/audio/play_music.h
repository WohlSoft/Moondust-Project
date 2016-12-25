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

#ifndef PLAY_MUSIC_H
#define PLAY_MUSIC_H

#include <QString>

class PGE_MusPlayer
{
public:
    static QString getTitle();

    static void play();
    static void fadeIn(int ms);
    static void pause();
    static void stop();
    static void fadeOut(int ms);

    static void setVolume(int vlm);
    static void openFile(QString musFile);

    static bool isPlaying();
    static bool isPaused();
    static bool IsFading();

    static void freeStream();
};

#endif
