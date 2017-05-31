#ifndef MUSIC_GME_H
#define MUSIC_GME_H

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
*/

/* $Id: music_gme.h Wohlstand $ */

#ifdef GME_MUSIC

#include "audio_codec.h"

/* Initialize the Game Music Emulators player, with the given mixer settings
   This function returns 0, or -1 if there was an error.
 */
extern int GME_init2(AudioCodec *codec, SDL_AudioSpec *mixer);

#endif /* GME_MUSIC */

#endif /* MUSIC_GME_H */

