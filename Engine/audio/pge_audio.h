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

#ifndef PGE_AUDIO_H
#define PGE_AUDIO_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer_ext.h>
#include <SDL2/SDL_stdinc.h>

#include <data_configs/obj_sound_roles.h>

namespace PGE_Audio
{
    int  init(int sampleRate = 44100,
              int allocateChannels = 32,
              int bufferSize = 4096);
    int  quit();

    const bool& isLoaded();
    void resetMusicSampleCounter();

    Uint64 sampleCount();
    Uint64 sampleCountMus();

    void playSound(long soundID);
    void playSoundByRole(obj_sound_role::roles role);
};

#endif // PGE_AUDIO_H
