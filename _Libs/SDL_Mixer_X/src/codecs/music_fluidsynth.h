/*
  SDL_mixer:  An audio mixer library based on the SDL library
  Copyright (C) 1997-2017 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  James Le Cuirot
  chewi@aura-online.co.uk
*/

#ifndef FLUIDSYNTH_H_
#define FLUIDSYNTH_H_

#ifdef USE_FLUIDSYNTH_MIDI

#include "../audio_codec.h"

#include "dynamic_fluidsynth.h"
#include <SDL2/SDL_rwops.h>
#include <SDL2/SDL_audio.h>

typedef struct {
    SDL_AudioCVT convert;
    fluid_synth_t *synth;
    fluid_player_t* player;
} FluidSynthMidiSong;

extern int fluidsynth_init2(AudioCodec *codec, SDL_AudioSpec *mixer);

void    *fluidsynth_loadsong_RW(SDL_RWops *rw, int freerw);
void    fluidsynth_freesong(void *song_p);
void    fluidsynth_start(void *song_p);
void    fluidsynth_stop(void *song_p);
int     fluidsynth_active(void *song_p);
void    fluidsynth_setvolume(void *song_p, int volume);
int     fluidsynth_playsome(void *song_p, void *stream, int len);

#endif /* USE_FLUIDSYNTH_MIDI */

#endif /* FLUIDSYNTH_H_ */
