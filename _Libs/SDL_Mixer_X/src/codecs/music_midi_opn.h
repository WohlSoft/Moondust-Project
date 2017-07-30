#ifndef MUSIC_MIDI_OPN_H
#define MUSIC_MIDI_OPN_H

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

#ifdef USE_OPN2_MIDI

#include "../audio_codec.h"

struct OPN2_MIDIPlayer;

/* This structure supports OPNMIDI-based MIDI music streams */
struct MUSIC_MIDIOPN
{
    struct OPN2_MIDIPlayer *opnmidi;
    int playing;
    int volume;
    int gme_t_sample_rate;
    char *mus_title;
    SDL_AudioCVT cvt;
};

extern int OPNMIDI_init2(AudioCodec *codec, SDL_AudioSpec *mixer);

/*Setup editing functions (changes applying on file reopening)*/
extern int  OPNMIDI_getScaleMod();
extern void OPNMIDI_setScaleMod(int sc);
extern int  OPNMIDI_getLogarithmicVolumes();
extern void OPNMIDI_setLogarithmicVolumes(int vm);
extern int  OPNMIDI_getVolumeModel();
extern void OPNMIDI_setVolumeModel(int vm);

extern void OPNMIDI_setCustomBankFile(const char *bank_wonp_path);

/* Reset all properties to default values */
extern void OPNMIDI_setDefaults();

extern void OPNMIDI_setLoops(void *music_p, int loop);

extern int OPNMIDI_init(SDL_AudioSpec *mixer);

/* Uninitialize the music players */
extern void OPNMIDI_exit(void);

/* Set the volume for a OPN MIDI stream */
extern void OPNMIDI_setvolume(void *music_p, int volume);

/* Load a OPN MIDI stream from an SDL_RWops object */
extern void *OPNMIDI_new_RW(SDL_RWops *rw, int freerw);

/* Start playback of a given OPN MIDI stream */
extern void OPNMIDI_play(void *music_p);

/* Return non-zero if a stream is currently playing */
extern int OPNMIDI_playing(void *music_p);

/* Play some of a stream previously started with OPNMIDI_play() */
extern int OPNMIDI_playAudio(void *music_p, Uint8 *stream, int len);

/* Stop playback of a stream previously started with OPNMIDI_play() */
extern void OPNMIDI_stop(void *music_p);

/* Close the given OPN MIDI stream */
extern void OPNMIDI_delete(void *music_p);

/* Jump (seek) to a given position (time is in seconds) */
extern void OPNMIDI_jump_to_time(void *music_p, double time);

#endif /* USE_OPN2_MIDI */

#endif /* MUSIC_MIDI_OPN_H */
