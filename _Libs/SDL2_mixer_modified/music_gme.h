#ifndef MUSIC_GME_H
#define MUSIC_GME_H

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

/* $Id: music_gme.h Wohlstand $ */

#ifdef GME_MUSIC

/* This file supports Game Music Emulator music streams */

struct MUSIC_GME
{
    int playing;
    int volume;
    int gme_t_sample_rate;
    char *mus_title;
    char *mus_artist;
    char *mus_album;
    char *mus_copyright;
};

/* Initialize the Ogg Vorbis player, with the given mixer settings
   This function returns 0, or -1 if there was an error.
 */
extern int GME_init(SDL_AudioSpec *mixer);

/* Uninitialize the music players */
extern void GME_exit(void);

/* Set the volume for a Game Music Emulators stream */
extern void GME_setvolume(struct MUSIC_GME *music, int volume);

/* Load a Game Music Emulators stream from an SDL_RWops object */
extern struct MUSIC_GME *GME_new_RW(SDL_RWops *rw, int freerw, int trackNum);

/* Start playback of a given Game Music Emulators stream */
extern void GME_play(struct MUSIC_GME *music);

/* Return non-zero if a stream is currently playing */
extern int GME_playing(struct MUSIC_GME *music);

/* Play some of a stream previously started with GME_play() */
extern int GME_playAudio(struct MUSIC_GME *music, Uint8 *stream, int len);

/* Stop playback of a stream previously started with GME_play() */
extern void GME_stop(struct MUSIC_GME *music);

/* Close the given Game Music Emulators stream */
extern void GME_delete(struct MUSIC_GME *music);

/* Jump (seek) to a given position (time is in seconds) */
extern void GME_jump_to_time(struct MUSIC_GME *music, double time);

#endif /* GME_MUSIC */

#endif // MUSIC_GME_H

