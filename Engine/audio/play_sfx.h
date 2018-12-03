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

#ifndef PLAY_SFX_H
#define PLAY_SFX_H

#include <unordered_map>
#include <string>

struct Mix_Chunk;
class PGE_SfxPlayer
{
public:
    static void playFile(std::string sndFile);
    static void clearSoundBuffer();
    static Mix_Chunk *openSFX(std::string sndFile);

private:
    typedef std::unordered_map<std::string, Mix_Chunk* > sfxHash;
    static  sfxHash chunksBuffer;
};

#endif // PLAY_SFX_H
