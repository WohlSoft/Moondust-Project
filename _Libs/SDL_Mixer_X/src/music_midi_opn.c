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

#ifdef USE_OPN2_MIDI

/* This file supports libOPNMIDI music streams */

#include <SDL_mixer_ext/SDL_mixer_ext.h>
#include "music_midi_opn.h"

#include "OPNMIDI/opnmidi.h"
#include "OPNMIDI/gm_opn_bank.h"

#include <stdio.h>

/* Global OPNMIDI flags which are applying on initializing of MIDI player with a file */
static int opnmidi_scalemod     = 0;
static int opnmidi_logVolumes   = 0;
static int opnmidi_volumeModel  = 0;
static const char* opnmidi_customBankPath = NULL;

int OPNMIDI_getScaleMod()
{
    return opnmidi_scalemod;
}

void OPNMIDI_setScaleMod(int sc)
{
    opnmidi_scalemod = sc;
}

int OPNMIDI_getLogarithmicVolumes()
{
    return opnmidi_logVolumes;
}

void OPNMIDI_setLogarithmicVolumes(int vm)
{
    opnmidi_logVolumes = vm;
}

int OPNMIDI_getVolumeModel()
{
    return opnmidi_volumeModel;
}

void OPNMIDI_setVolumeModel(int vm)
{
    opnmidi_volumeModel = vm;
    if(vm < 0)
        opnmidi_volumeModel = 0;
}

void OPNMIDI_setLoops(void *music_p, int loop)
{
    struct MUSIC_MIDIOPN *music = (struct MUSIC_MIDIOPN*)music_p;
    if(music)
        opn2_setLoopEnabled(music->opnmidi, (loop >=0 ? 0 : 1));
}

void OPNMIDI_setDefaults()
{
    opnmidi_scalemod    = 0;
}

/*
 * Initialize the OPNMIDI player, with the given mixer settings
 * This function returns 0, or -1 if there was an error.
 */

/* This is the format of the audio mixer data */
static SDL_AudioSpec mixer;

int OPNMIDI_init2(AudioCodec *codec, SDL_AudioSpec *mixerfmt)
{
    mixer = *mixerfmt;

    initAudioCodec(codec);

    codec->isValid = 1;

    codec->capabilities     = audioCodec_default_capabilities;

    codec->open             = OPNMIDI_new_RW;
    codec->openEx           = audioCodec_dummy_cb_openEx;
    codec->close            = OPNMIDI_delete;

    codec->play             = OPNMIDI_play;
    codec->pause            = audioCodec_dummy_cb_void_1arg;
    codec->resume           = audioCodec_dummy_cb_void_1arg;
    codec->stop             = OPNMIDI_stop;

    codec->isPlaying        = OPNMIDI_playing;
    codec->isPaused         = audioCodec_dummy_cb_int_1arg;

    codec->setLoops         = OPNMIDI_setLoops;
    codec->setVolume        = OPNMIDI_setvolume;

    codec->jumpToTime       = OPNMIDI_jump_to_time;
    codec->getCurrentTime   = audioCodec_dummy_cb_tell;
    codec->getTimeLength    = audioCodec_dummy_cb_tell;

    codec->metaTitle        = audioCodec_dummy_meta_tag;
    codec->metaArtist       = audioCodec_dummy_meta_tag;
    codec->metaAlbum        = audioCodec_dummy_meta_tag;
    codec->metaCopyright    = audioCodec_dummy_meta_tag;

    codec->playAudio        = OPNMIDI_playAudio;

    return(0);
}

int OPNMIDI_init(SDL_AudioSpec *mixerfmt)
{
    if(!mixerfmt)
    {
        Mix_SetError("OPN2-MIDI: Null audio spec pointer\n");
        return -1;
    }
    mixer = *mixerfmt;
    return 0;
}

/* Uninitialize the music players */
void OPNMIDI_exit(void) {}

/* Set the volume for a OPNMIDI stream */
void OPNMIDI_setvolume(void *music_p, int volume)
{
    struct MUSIC_MIDIOPN *music = (struct MUSIC_MIDIOPN*)music_p;
    if(music)
    {
        music->volume = (int)round(128.0*sqrt(((double)volume)*(1.0/128.0) ));
    }
}

void OPNMIDI_setCustomBankFile(const char *bank_wonp_path)
{
    opnmidi_customBankPath = bank_wonp_path;
}

struct MUSIC_MIDIOPN *OPNMIDI_LoadSongRW(SDL_RWops *src)
{
    if(src != NULL)
    {
        void *bytes=0;
        long filesize = 0;
        int err = 0;
        Sint64 length=0;
        size_t bytes_l;
        unsigned char byte[1];
        struct OPN2_MIDIPlayer* opn_midiplayer = NULL;
        struct MUSIC_MIDIOPN *adlMidi = NULL;

        length = SDL_RWseek(src, 0, RW_SEEK_END);
        if (length < 0)
        {
            Mix_SetError("OPN2-MIDI: wrong file\n");
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
            Mix_SetError("OPN2-MIDI: wrong file\n");
            SDL_free(bytes);
            return NULL;
        }

        opn_midiplayer = opn2_init( mixer.freq );
        if(opnmidi_customBankPath)
            err = opn2_openBankFile(opn_midiplayer, (char*)opnmidi_customBankPath);
        else
            err = opn2_openBankData( opn_midiplayer, g_gm_opn2_bank, sizeof(g_gm_opn2_bank) );
        if( err < 0 )
        {
            Mix_SetError("OPN2-MIDI: %s", opn2_errorString());
            opn2_close( opn_midiplayer );
            SDL_free(bytes);
            return NULL;
        }

        opn2_setLogarithmicVolumes( opn_midiplayer, opnmidi_logVolumes );
        opn2_setVolumeRangeModel( opn_midiplayer, opnmidi_volumeModel );
        opn2_setNumCards( opn_midiplayer, 4 );

        err = opn2_openData( opn_midiplayer, bytes, filesize );
        SDL_free(bytes);

        if(err != 0)
        {
            Mix_SetError("OPN2-MIDI: %s", opn2_errorString());
            return NULL;
        }

        adlMidi = (struct MUSIC_MIDIOPN*)SDL_malloc(sizeof(struct MUSIC_MIDIOPN));
        adlMidi->opnmidi                = opn_midiplayer;
        adlMidi->playing                = 0;
        adlMidi->gme_t_sample_rate      = mixer.freq;
        adlMidi->volume                 = MIX_MAX_VOLUME;
        adlMidi->mus_title              = NULL;
        SDL_BuildAudioCVT(&adlMidi->cvt, AUDIO_S16, 2, mixer.freq, mixer.format, mixer.channels, mixer.freq);

        return adlMidi;
    }
    return NULL;
}

/* Load OPNMIDI stream from an SDL_RWops object */
void *OPNMIDI_new_RW(struct SDL_RWops *src, int freesrc)
{
    struct MUSIC_MIDIOPN *adlmidiMusic;

    OPNMIDI_exit();

    adlmidiMusic = OPNMIDI_LoadSongRW(src);
    if (!adlmidiMusic)
    {
        Mix_SetError("OPN2-MIDI: Can't load file: %s", opn2_errorString());
        return NULL;
    }
    if( freesrc )
        SDL_RWclose(src);

    return adlmidiMusic;
}

/* Start playback of a given Game Music Emulators stream */
void OPNMIDI_play(void *music_p)
{
    struct MUSIC_MIDIOPN *music = (struct MUSIC_MIDIOPN*)music_p;
    if(music)
        music->playing = 1;
}

/* Return non-zero if a stream is currently playing */
int OPNMIDI_playing(void *music_p)
{
    struct MUSIC_MIDIOPN *music = (struct MUSIC_MIDIOPN*)music_p;
    if(music)
        return music->playing;
    else
        return 0;
}

/* Play some of a stream previously started with OPNMIDI_play() */
int OPNMIDI_playAudio(void *music_p, Uint8 *stream, int len)
{
    struct MUSIC_MIDIOPN *music = (struct MUSIC_MIDIOPN*)music_p;
    int srgArraySize, srcLen, gottenLen, dest_len;
    short* buf = NULL;

    if( music == NULL )
        return 0;
    if( music->opnmidi == NULL )
        return 0;
    if( music->playing <= 0 )
        return 0;
    if( len < 0 )
        return 0;

    srgArraySize = len * music->cvt.len_mult;
    buf = (short*)SDL_malloc((size_t)srgArraySize);
    srcLen = (int)((double)(len/2.0)/music->cvt.len_ratio);

    gottenLen = opn2_play( music->opnmidi, srcLen, buf );
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

/* Stop playback of a stream previously started with OPNMIDI_play() */
void OPNMIDI_stop(void *music_p)
{
    struct MUSIC_MIDIOPN *music = (struct MUSIC_MIDIOPN*)music_p;
    if(music)
    {
        music->playing = 0;
        opn2_reset(music->opnmidi);
    }
}

/* Close the given Game Music Emulators stream */
void OPNMIDI_delete(void *music_p)
{
    struct MUSIC_MIDIOPN *music = (struct MUSIC_MIDIOPN*)music_p;
    if(music)
    {
        if( music->mus_title )
        {
            SDL_free(music->mus_title);
        }
        music->playing = 0;
        if(music->opnmidi)
            opn2_close( music->opnmidi );
        music->opnmidi = NULL;
        SDL_free( music );
    }
    OPNMIDI_exit();
}

/* Jump (seek) to a given position (time is in seconds) */
void OPNMIDI_jump_to_time(void *music_p, double time)
{
    struct MUSIC_MIDIOPN *music = (struct MUSIC_MIDIOPN*)music_p;
    (void)time;
    if( music )
    {
        /* gme_seek(adl_midiplayer, (int)round(time*1000)); */
    }
}

#endif /*USE_OPN2_MIDI*/

