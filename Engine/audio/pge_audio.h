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

#ifndef PGE_AUDIO_H
#define PGE_AUDIO_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer_ext.h>
#include <SDL2/SDL_stdinc.h>

#include <data_configs/obj_sound_roles.h>

namespace PGE_Audio
{
    int  init(Uint32 sampleRate = 44100,
              Uint32 allocateChannels = 32,
              Uint32 bufferSize = 4096);
    int  quit();

    const bool& isLoaded();
    void resetMusicSampleCounter();

    Uint64 sampleCount();
    Uint64 sampleCountMus();

    void playSound(size_t soundID);
    void playSoundByRole(obj_sound_role::roles role);
}

#endif // PGE_AUDIO_H
