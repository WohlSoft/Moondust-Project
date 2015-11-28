#ifndef MUSIC_MIDI_ADL_H
#define MUSIC_MIDI_ADL_H

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

#ifdef USE_ADL_MIDI

/* This file supports Game Music Emulator music streams */

struct MUSIC_MIDIADL
{
    int playing;
    int volume;
    int gme_t_sample_rate;
    char *mus_title;
};

/*Setup editing functions (changes applying on file reopening)*/
extern int  ADLMIDI_getBankID();
extern void ADLMIDI_setBankID(int bnk);
extern int  ADLMIDI_getTremolo();
extern void ADLMIDI_setTremolo(int tr);
extern int  ADLMIDI_getVibrato();
extern void ADLMIDI_setVibrato(int vib);
extern int  ADLMIDI_getScaleMod();
extern void ADLMIDI_setScaleMod(int sc);

extern int ADLMIDI_init(SDL_AudioSpec *mixer);

/* Uninitialize the music players */
extern void ADLMIDI_exit(void);

/* Set the volume for a ADM MIDI stream */
extern void ADLMIDI_setvolume(struct MUSIC_MIDIADL *music, int volume);

/* Load a ADM MIDI stream from an SDL_RWops object */
extern struct MUSIC_MIDIADL *ADLMIDI_new_RW(SDL_RWops *rw, int freerw);

/* Start playback of a given ADM MIDI stream */
extern void ADLMIDI_play(struct MUSIC_MIDIADL *music);

/* Return non-zero if a stream is currently playing */
extern int ADLMIDI_playing(struct MUSIC_MIDIADL *music);

/* Play some of a stream previously started with ADLMIDI_play() */
extern int ADLMIDI_playAudio(struct MUSIC_MIDIADL *music, Uint8 *stream, int len);

/* Stop playback of a stream previously started with ADLMIDI_play() */
extern void ADLMIDI_stop(struct MUSIC_MIDIADL *music);

/* Close the given ADM MIDI stream */
extern void ADLMIDI_delete(struct MUSIC_MIDIADL *music);

/* Jump (seek) to a given position (time is in seconds) */
extern void ADLMIDI_jump_to_time(struct MUSIC_MIDIADL *music, double time);

#endif /* USE_ADL_MIDI */

#endif // MUSIC_MIDI_ADL_H

