/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef PLAY_MUSIC_H
#define PLAY_MUSIC_H

#include <string>

class PGE_MusPlayer
{
public:
    static std::string getTitle();

    static void play();
    static void fadeIn(int ms);
    static void pause();
    static void stop();
    static void fadeOut(int ms);

    static void setVolume(int vlm);
    static void openFile(std::string musFile);

    static bool isPlaying();
    static bool isPaused();
    static bool isFading();

    static void freeStream();
};

#endif
