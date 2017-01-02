/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef PLAY_SFX_H
#define PLAY_SFX_H

#include <unordered_map>
#include <QString>

struct Mix_Chunk;
class PGE_SfxPlayer
{
public:
    static void playFile(QString sndFile);
    static void clearSoundBuffer();
    static Mix_Chunk *openSFX(QString sndFile);

private:
    typedef std::unordered_map<std::string, Mix_Chunk* > sfxHash;
    static  sfxHash chunksBuffer;
};

#endif // PLAY_SFX_H
