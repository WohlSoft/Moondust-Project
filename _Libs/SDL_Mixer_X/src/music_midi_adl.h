#ifndef MUSIC_MIDI_ADL_H
#define MUSIC_MIDI_ADL_H

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

#ifdef USE_ADL_MIDI

#include "audio_codec.h"

struct ADL_MIDIPlayer;

/* This structure supports ADLMIDI-based MIDI music streams */
struct MUSIC_MIDIADL
{
    struct ADL_MIDIPlayer *adlmidi;
    int playing;
    int volume;
    int gme_t_sample_rate;
    char *mus_title;
    SDL_AudioCVT cvt;
};

extern int ADLMIDI_init2(AudioCodec *codec, SDL_AudioSpec *mixer);

/*Setup editing functions (changes applying on file reopening)*/
extern int  ADLMIDI_getBanksCount();
extern const char *const *ADLMIDI_getBankNames();
extern int  ADLMIDI_getBankID();
extern void ADLMIDI_setBankID(int bnk);
extern int  ADLMIDI_getTremolo();
extern void ADLMIDI_setTremolo(int tr);
extern int  ADLMIDI_getVibrato();
extern void ADLMIDI_setVibrato(int vib);
extern int  ADLMIDI_getAdLibDrumsMode();
extern void ADLMIDI_setAdLibDrumsMode(int ald);
extern int  ADLMIDI_getScaleMod();
extern void ADLMIDI_setScaleMod(int sc);
extern int  ADLMIDI_getLogarithmicVolumes();
extern void ADLMIDI_setLogarithmicVolumes(int vm);
extern int  ADLMIDI_getVolumeModel();
extern void ADLMIDI_setVolumeModel(int vm);
/* Reset all properties to default values */
extern void ADLMIDI_setDefaults();

extern void ADLMIDI_setInfiniteLoop(struct MUSIC_MIDIADL *music, int loop);

/* Set the volume for a ADL MIDI stream */
extern void ADLMIDI_setvolume(void *music_p, int volume);

/* Load a ADL MIDI stream from an SDL_RWops object */
extern void *ADLMIDI_new_RW(SDL_RWops *rw, int freerw);

/* Start playback of a given ADL MIDI stream */
extern void ADLMIDI_play(void *music_p);

/* Return non-zero if a stream is currently playing */
extern int ADLMIDI_playing(void *music_p);

/* Play some of a stream previously started with ADLMIDI_play() */
extern int ADLMIDI_playAudio(void *music_p, Uint8 *stream, int len);

/* Stop playback of a stream previously started with ADLMIDI_play() */
extern void ADLMIDI_stop(void *music_p);

/* Close the given ADL MIDI stream */
extern void ADLMIDI_delete(void *music_p);

/* Jump (seek) to a given position (time is in seconds) */
extern void ADLMIDI_jump_to_time(void *music_p, double time);

#endif /* USE_ADL_MIDI */

#endif /* MUSIC_MIDI_ADL_H */
