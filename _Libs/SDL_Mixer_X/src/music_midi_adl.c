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

#ifdef USE_ADL_MIDI

/* This file supports libADLMIDI music streams */

#include <SDL_mixer_ext/SDL_mixer_ext.h>
#include "music_midi_adl.h"

#include "ADLMIDI/adlmidi.h"

#include <stdio.h>

/* Count of four-operator channnels per bank */
static const int tableOf_num4opChans[] =
{
    0,/* 0 */
    0,/* 1 */
    0,/* 2 */
    0,/* 3 */
    0,/* 4 */
    0,/* 5 */
    0,/* 6 */
    0,/* 7 */
    0,/* 8 */
    0,/* 9 */
    0,/* 10 */
    0,/* 11 */
    0,/* 12 */
    0,/* 13 */
    0,/* 14 */
    0,/* 15 */
    0,/* 16 */
    0,/* 17 */
    0,/* 18 */
    0,/* 19 */
    24,/* 20 */
    0,/* 21 */
    0,/* 22 */
    0,/* 23 */
    0,/* 24 */
    0,/* 25 */
    0,/* 26 */
    0,/* 27 */
    0,/* 28 */
    0,/* 29 */
    0,/* 30 */
    24,/* 31 */
    0,/* 32 */
    0,/* 33 */
    0,/* 34 */
    0,/* 35 */
    24,/* 36 */
    0,/* 37 */
    24,/* 38 */
    24,/* 39 */
    0,/* 40 */
    0,/* 41 */
    0,/* 42 */
    0,/* 43 */
    18,/* 44 */
    18,/* 45 */
    0,/* 46 */
    18,/* 47 */
    18,/* 48 */
    0,/* 49 */
    0,/* 50 */
    0,/* 51 */
    0,/* 52 */
    24,/* 53 */
    24,/* 54 */
    0,/* 55 */
    0,/* 56 */
    0,/* 57 */
    0,/* 58 */
    24,/* 59 */
    0,/* 60 */
    0,/* 61 */
    0,/* 62 */
    0,/* 63 */
    0,/* 64 */
    0,/* 65 */
    0,/* 66 */
    0,/* 67 */
    18,/* 68 */
    0,/* 69 */
    0,/* 70 */
    0,/* 71 */
    0,/* 72 */
};


/* Global ADLMIDI flags which are applying on initializing of MIDI player with a file */
static int adlmidi_bank         = 58;
static int adlmidi_tremolo      = 1;
static int adlmidi_vibrato      = 1;
static int adlmidi_scalemod     = 0;
static int adlmidi_adlibdrums   = 0;
static int adlmidi_logVolumes   = 0;
static int adlmidi_volumeModel  = 0;

int ADLMIDI_getBanksCount()
{
    return adl_getBanksCount();
}

const char * const*ADLMIDI_getBankNames()
{
    return adl_getBankNames();
}

int ADLMIDI_getBankID()
{
    return adlmidi_bank;
}

void ADLMIDI_setBankID(int bnk)
{
    adlmidi_bank = bnk;
}

int ADLMIDI_getTremolo()
{
    return adlmidi_tremolo;
}
void ADLMIDI_setTremolo(int tr)
{
    adlmidi_tremolo = tr;
}

int ADLMIDI_getVibrato()
{
    return adlmidi_vibrato;
}

void ADLMIDI_setVibrato(int vib)
{
    adlmidi_vibrato = vib;
}


int ADLMIDI_getAdLibDrumsMode()
{
    return adlmidi_adlibdrums;
}

void ADLMIDI_setAdLibDrumsMode(int ald)
{
    adlmidi_adlibdrums = ald;
}

int ADLMIDI_getScaleMod()
{
    return adlmidi_scalemod;
}

void ADLMIDI_setScaleMod(int sc)
{
    adlmidi_scalemod = sc;
}

int ADLMIDI_getLogarithmicVolumes()
{
    return adlmidi_logVolumes;
}

void ADLMIDI_setLogarithmicVolumes(int vm)
{
    adlmidi_logVolumes = vm;
}

int ADLMIDI_getVolumeModel()
{
    return adlmidi_volumeModel;
}

void ADLMIDI_setVolumeModel(int vm)
{
    adlmidi_volumeModel = vm;
    if(vm < 0)
        adlmidi_volumeModel = 0;
}

void ADLMIDI_setInfiniteLoop(struct MUSIC_MIDIADL *music, int loop)
{
    if(music)
        adl_setLoopEnabled(music->adlmidi, loop);
}

static void ADLMIDI_setLoops(void*music_p, int loops)
{
    struct MUSIC_MIDIADL *music = (struct MUSIC_MIDIADL *)music_p;
    ADLMIDI_setInfiniteLoop(music, loops < 0 ? 1 : 0);
}

void ADLMIDI_setDefaults()
{
    adlmidi_tremolo     = 1;
    adlmidi_vibrato     = 1;
    adlmidi_scalemod    = 0;
    adlmidi_adlibdrums  = 0;
    adlmidi_bank        = 58;
}

/*
 * Initialize the ADLMIDI player, with the given mixer settings
 * This function returns 0, or -1 if there was an error.
 */

/* This is the format of the audio mixer data */
static SDL_AudioSpec mixer;

int ADLMIDI_init2(AudioCodec *codec, SDL_AudioSpec *mixerfmt)
{
    mixer = *mixerfmt;

    initAudioCodec(codec);

    codec->isValid = 1;

    codec->capabilities     = audioCodec_default_capabilities;

    codec->open             = ADLMIDI_new_RW;
    codec->openEx           = audioCodec_dummy_cb_openEx;
    codec->close            = ADLMIDI_delete;

    codec->play             = ADLMIDI_play;
    codec->pause            = audioCodec_dummy_cb_void_1arg;
    codec->resume           = audioCodec_dummy_cb_void_1arg;
    codec->stop             = ADLMIDI_stop;

    codec->isPlaying        = ADLMIDI_playing;
    codec->isPaused         = audioCodec_dummy_cb_int_1arg;

    codec->setLoops         = ADLMIDI_setLoops;
    codec->setVolume        = ADLMIDI_setvolume;

    codec->jumpToTime       = ADLMIDI_jump_to_time;
    codec->getCurrentTime   = audioCodec_dummy_cb_tell;
    codec->getTimeLength    = audioCodec_dummy_cb_tell;

    codec->metaTitle        = audioCodec_dummy_meta_tag;
    codec->metaArtist       = audioCodec_dummy_meta_tag;
    codec->metaAlbum        = audioCodec_dummy_meta_tag;
    codec->metaCopyright    = audioCodec_dummy_meta_tag;

    codec->playAudio        = ADLMIDI_playAudio;

    return(0);
}

/* Set the volume for a ADLMIDI stream */
void ADLMIDI_setvolume(void *music_p, int volume)
{
    struct MUSIC_MIDIADL* music = (struct MUSIC_MIDIADL*)music_p;
    if(music)
    {
        music->volume = (int)round(128.0*sqrt(((double)volume)*(1.0/128.0) ));
    }
}

struct MUSIC_MIDIADL *ADLMIDI_LoadSongRW(SDL_RWops *src)
{
    if(src != NULL)
    {
        void *bytes=0;
        long filesize = 0;
        int err = 0;
        Sint64 length=0;
        size_t bytes_l;
        unsigned char byte[1];
        struct ADL_MIDIPlayer* adl_midiplayer = NULL;
        struct MUSIC_MIDIADL *adlMidi = NULL;

        length = SDL_RWseek(src, 0, RW_SEEK_END);
        if (length < 0)
        {
            Mix_SetError("ADL-MIDI: wrong file\n");
            return NULL;
        }

        SDL_RWseek(src, 0, RW_SEEK_SET);
        bytes = SDL_malloc((size_t)length);

        filesize = 0;
        while( (bytes_l = SDL_RWread(src, &byte, sizeof(Uint8), 1)) != 0)
        {
            ((unsigned char*)bytes)[filesize] = byte[0];
            filesize++;
        }

        if (filesize == 0)
        {
            Mix_SetError("ADL-MIDI: wrong file\n");
            SDL_free(bytes);
            return NULL;
        }

        adl_midiplayer = adl_init( mixer.freq );

        adl_setHVibrato( adl_midiplayer, adlmidi_vibrato );
        adl_setHTremolo( adl_midiplayer, adlmidi_tremolo );
        if( adl_setBank( adl_midiplayer, adlmidi_bank ) < 0 )
        {
            Mix_SetError("ADL-MIDI: %s", adl_errorString());
            adl_close( adl_midiplayer );
            SDL_free(bytes);
            return NULL;
        }

        adl_setScaleModulators( adl_midiplayer, adlmidi_scalemod );
        adl_setPercMode( adl_midiplayer, adlmidi_adlibdrums );
        adl_setNumFourOpsChn( adl_midiplayer, tableOf_num4opChans[adlmidi_bank] );
        adl_setLogarithmicVolumes( adl_midiplayer, adlmidi_logVolumes );
        adl_setVolumeRangeModel( adl_midiplayer, adlmidi_volumeModel );
        adl_setNumCards( adl_midiplayer, 4 );

        err = adl_openData( adl_midiplayer, bytes, filesize );
        SDL_free(bytes);

        if(err != 0)
        {
            Mix_SetError("ADL-MIDI: %s", adl_errorString());
            return NULL;
        }

        adlMidi = (struct MUSIC_MIDIADL*)SDL_malloc(sizeof(struct MUSIC_MIDIADL));
        adlMidi->adlmidi                = adl_midiplayer;
        adlMidi->playing                = 0;
        adlMidi->gme_t_sample_rate      = mixer.freq;
        adlMidi->volume                 = MIX_MAX_VOLUME;
        adlMidi->mus_title              = NULL;
        SDL_BuildAudioCVT(&adlMidi->cvt, AUDIO_S16, 2, mixer.freq, mixer.format, mixer.channels, mixer.freq);

        return adlMidi;
    }
    return NULL;
}

/* Load ADLMIDI stream from an SDL_RWops object */
void *ADLMIDI_new_RW(struct SDL_RWops *src, int freesrc)
{
    struct MUSIC_MIDIADL *adlmidiMusic;

    adlmidiMusic = ADLMIDI_LoadSongRW(src);
    if (!adlmidiMusic)
    {
        Mix_SetError("ADL-MIDI: Can't load file: %s", adl_errorString());
        return NULL;
    }
    if( freesrc )
        SDL_RWclose(src);

    return adlmidiMusic;
}

/* Start playback of a given Game Music Emulators stream */
void ADLMIDI_play(void *music_p)
{
    struct MUSIC_MIDIADL* music = (struct MUSIC_MIDIADL*)music_p;
    if(music)
        music->playing = 1;
}

/* Return non-zero if a stream is currently playing */
int ADLMIDI_playing(void *music_p)
{
    struct MUSIC_MIDIADL* music = (struct MUSIC_MIDIADL*)music_p;
    if(music)
        return music->playing;
    else
        return 0;
}

/* Play some of a stream previously started with ADLMIDI_play() */
int ADLMIDI_playAudio(void *music_p, Uint8 *stream, int len)
{
    struct MUSIC_MIDIADL* music = (struct MUSIC_MIDIADL*)music_p;

    int srgArraySize, srcLen, gottenLen, dest_len;
    short* buf = NULL;

    if( music == NULL )
        return 0;
    if( music->adlmidi == NULL )
        return 0;
    if( music->playing <= 0 )
        return 0;
    if( len < 0 )
        return 0;

    srgArraySize = len * music->cvt.len_mult;
    buf = (short*)SDL_malloc((size_t)srgArraySize);
    srcLen = (int)((double)(len/2.0)/music->cvt.len_ratio);

    gottenLen = adl_play( music->adlmidi, srcLen, buf );
    if( gottenLen <= 0 )
    {
        SDL_free(buf);
        music->playing = 0;
        return len - 1;
    }

    dest_len = gottenLen * 2;

    if( music->cvt.needed )
    {
        music->cvt.len = dest_len;
        music->cvt.buf = (Uint8*)buf;
        SDL_ConvertAudio(&music->cvt);
        dest_len = music->cvt.len_cvt;
    }

    if( music->volume == MIX_MAX_VOLUME )
    {
        SDL_memcpy( stream, (Uint8*)buf, (size_t)dest_len );
    } else {
        SDL_MixAudioFormat( stream, (Uint8*)buf, mixer.format, (Uint32)dest_len, music->volume );
    }

    SDL_free(buf);
    return len - dest_len;
}

/* Stop playback of a stream previously started with ADLMIDI_play() */
void ADLMIDI_stop(void *music_p)
{
    struct MUSIC_MIDIADL* music = (struct MUSIC_MIDIADL*)music_p;
    if(music)
    {
        music->playing = 0;
        adl_reset(music->adlmidi);
    }
}

/* Close the given Game Music Emulators stream */
void ADLMIDI_delete(void *music_p)
{
    struct MUSIC_MIDIADL* music = (struct MUSIC_MIDIADL*)music_p;
    if(music)
    {
        if( music->mus_title )
        {
            SDL_free(music->mus_title);
        }
        music->playing = 0;
        if(music->adlmidi)
            adl_close( music->adlmidi );
        music->adlmidi = NULL;
        SDL_free( music );
    }
}

/* Jump (seek) to a given position (time is in seconds) */
void ADLMIDI_jump_to_time(void *music_p, double time)
{
    struct MUSIC_MIDIADL* music = (struct MUSIC_MIDIADL*)music_p;
    (void)time;
    if( music )
    {
        /* gme_seek(adl_midiplayer, (int)round(time*1000)); */
    }
}
#endif


