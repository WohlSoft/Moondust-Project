#ifndef MUSIC_SPC_H
#define MUSIC_SPC_H

/*
  SDL_mixer:  An audio mixer library based on the SDL library
  Copyright (C) 1997-2013 Sam Lantinga <slouken@libsdl.org>

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

/* $Id: music_spc.h Wohlstnd $ */

#ifdef SPC_MUSIC

/* This file supports MOD tracker music streams */

struct MUSIC_SPC
{
    int playing;
    int volume;
    int spc_t_sample_rate;
};

/* Initialize the Ogg Vorbis player, with the given mixer settings
   This function returns 0, or -1 if there was an error.
 */
extern int SPC_init(SDL_AudioSpec *mixer);

/* Uninitialize the music players */
extern void SPC_exit(void);

/* Set the volume for a MOD stream */
extern void SPC_setvolume(struct MUSIC_SPC *music, int volume);

/* Load a MOD stream from an SDL_RWops object */
extern struct MUSIC_SPC *SPC_new_RW(SDL_RWops *rw, int freerw);

/* Start playback of a given MOD stream */
extern void SPC_play(struct MUSIC_SPC *music);

/* Return non-zero if a stream is currently playing */
extern int SPC_playing(struct MUSIC_SPC *music);

/* Play some of a stream previously started with MOD_play() */
extern int SPC_playAudio(struct MUSIC_SPC *music, Uint8 *stream, int len);

/* Stop playback of a stream previously started with MOD_play() */
extern void SPC_stop(struct MUSIC_SPC *music);

/* Close the given MOD stream */
extern void SPC_delete(struct MUSIC_SPC *music);

/* Jump (seek) to a given position (time is in seconds) */
extern void SPC_jump_to_time(struct MUSIC_SPC *music, double time);

#endif /* SPC_MUSIC */

#endif // MUSIC_SPC_H

