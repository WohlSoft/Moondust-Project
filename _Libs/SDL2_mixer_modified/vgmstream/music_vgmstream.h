#ifndef MUSIC_VGMSTREAM_H
#define MUSIC_VGMSTREAM_H

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

/* $Id: music_vgmstream.h Wohlstand $ */

#ifdef VGMSTREAM_MUSIC

/* First parameter of most gme_ functions is a pointer to the Music_Emu */
typedef struct VGMSTREAM VGMSTREAM;

/* This file supports Game Music Emulator music streams */
struct MUSIC_VGMSTREAM
{
    VGMSTREAM* stream;
    int playing;
    int volume;
    int sample_rate;
    char *mus_title;
    char *mus_artist;
    char *mus_album;
    char *mus_copyright;
    SDL_AudioCVT cvt;
};

/* Initialize the Ogg Vorbis player, with the given mixer settings
   This function returns 0, or -1 if there was an error.
 */
extern int VGMSTREAM_init(SDL_AudioSpec *mixer);

/* Uninitialize the music players */
extern void VGMSTREAM_exit(void);

/* Set the volume for a Game Music Emulators stream */
extern void VGMSTREAM_setvolume(struct MUSIC_VGMSTREAM *music, int volume);

/* Load a Game Music Emulators stream from an SDL_RWops object */
extern struct MUSIC_VGMSTREAM *VGMSTREAM_new_RW(SDL_RWops *rw, int freerw, int trackNum);

/* Start playback of a given Game Music Emulators stream */
extern void VGMSTREAM_play(struct MUSIC_VGMSTREAM *music);

/* Return non-zero if a stream is currently playing */
extern int VGMSTREAM_playing(struct MUSIC_VGMSTREAM *music);

/* Play some of a stream previously started with VGMSTREAM_play() */
extern int VGMSTREAM_playAudio(struct MUSIC_VGMSTREAM *music, Uint8 *stream, int len);

/* Stop playback of a stream previously started with VGMSTREAM_play() */
extern void VGMSTREAM_stop(struct MUSIC_VGMSTREAM *music);

/* Close the given Game Music Emulators stream */
extern void VGMSTREAM_delete(struct MUSIC_VGMSTREAM *music);

/* Jump (seek) to a given position (time is in seconds) */
extern void VGMSTREAM_jump_to_time(struct MUSIC_VGMSTREAM *music, double time);

#endif /* VGMSTREAM_MUSIC */

#endif // MUSIC_VGMSTREAM_H

