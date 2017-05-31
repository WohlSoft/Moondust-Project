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

#ifdef MODPLUG_MUSIC

#include <SDL_mixer_ext/SDL_mixer_ext.h>
#include "dynamic_modplug.h"
#include "music_modplug.h"

#ifdef MODPLUG_HEADER
#include MODPLUG_HEADER
#else
#include "modplug/modplug.h"
#endif
#include <SDL2/SDL_rwops.h>
#include <SDL2/SDL_audio.h>
#include <SDL_mixer_ext/SDL_mixer_ext.h>

typedef struct {
    ModPlugFile *file;
    int playing;
    char *mus_title;
} modplug_data;

static int current_output_channels = 0;
static int music_swap8 = 0;
static int music_swap16 = 0;
static ModPlug_Settings settings;

/* Load a modplug stream from an SDL_RWops object */
static void *modplug_new_RW(SDL_RWops *rw, int freerw);
/* Close the given modplug stream */
static void modplug_delete(void *music);

/* Set the volume for a modplug stream */
static void modplug_setvolume(void *music_p, int volume);

/* Start playback of a given modplug stream */
static void modplug_play(void *music_p);
/* Stop playback of a stream previously started with modplug_play() */
static void modplug_stop(void *music_p);

/* Return non-zero if a stream is currently playing */
static int modplug_playing(void *music_p);

static const char *modplug_metaTitle(void *music_p);

/* Jump (seek) to a given position (time is in seconds) */
static void modplug_jump_to_time(void *music_p, double time);
static double modplug_get_current_time(void *music_p);

/* Play some of a stream previously started with modplug_play() */
static int modplug_playAudio(void *music_p, Uint8 *stream, int len);


int modplug_init2(AudioCodec *codec, SDL_AudioSpec *spec)
{
    if(!Mix_Init(MIX_INIT_MODPLUG))
        return -1;

    modplug.ModPlug_GetSettings(&settings);
    settings.mFlags = MODPLUG_ENABLE_OVERSAMPLING;
    current_output_channels = spec->channels;
    settings.mChannels = spec->channels > 1 ? 2 : 1;
    settings.mBits = spec->format & 0xFF;

    music_swap8 = 0;
    music_swap16 = 0;

    switch(spec->format)
    {
    case AUDIO_U8:
    case AUDIO_S8:
        {
            if(spec->format == AUDIO_S8)
                music_swap8 = 1;
            settings.mBits = 8;
        }
    break;

    case AUDIO_S16LSB:
    case AUDIO_S16MSB:
        {
            #if SDL_BYTEORDER == SDL_LIL_ENDIAN
            #define MP_SPEC_FORMAT AUDIO_S16MSB
            #else
            #define MP_SPEC_FORMAT AUDIO_S16LSB
            #endif
            /* See if we need to correct MikMod mixing */
            if(spec->format == MP_SPEC_FORMAT)
                music_swap16 = 1;
            #undef MP_SPEC_FORMAT
            settings.mBits = 16;
        }
    break;

    default:
        {
            Mix_SetError("Unknown hardware audio format");
            return -1;
        }
    }

    settings.mFrequency = spec->freq; /*TODO: limit to 11025, 22050, or 44100 ? */
    settings.mResamplingMode = MODPLUG_RESAMPLE_FIR;
    settings.mReverbDepth = 0;
    settings.mReverbDelay = 100;
    settings.mBassAmount = 0;
    settings.mBassRange = 50;
    settings.mSurroundDepth = 0;
    settings.mSurroundDelay = 10;
    settings.mLoopCount = -1;
    modplug.ModPlug_SetSettings(&settings);

    codec->isValid = 1;

    codec->open  = modplug_new_RW;
    codec->openEx= audioCodec_dummy_cb_openEx;
    codec->close = modplug_delete;

    codec->play   = modplug_play;
    codec->pause  = audioCodec_dummy_cb_void_1arg;
    codec->resume = audioCodec_dummy_cb_void_1arg;
    codec->stop   = modplug_stop;

    codec->isPlaying   = modplug_playing;
    codec->isPaused    = audioCodec_dummy_cb_int_1arg;

    codec->setLoops    = audioCodec_dummy_cb_regulator;
    codec->setVolume   = modplug_setvolume;

    codec->jumpToTime     = modplug_jump_to_time;
    codec->getCurrentTime = modplug_get_current_time;

    codec->metaTitle    = modplug_metaTitle;
    codec->metaArtist   = audioCodec_dummy_meta_tag;
    codec->metaAlbum    = audioCodec_dummy_meta_tag;
    codec->metaCopyright = audioCodec_dummy_meta_tag;

    codec->playAudio    = modplug_playAudio;

    return 0;
}

/* Set the volume for a modplug stream */
static void modplug_setvolume(void *music_p, int volume)
{
    modplug_data *music = (modplug_data *)music_p;
    modplug.ModPlug_SetMasterVolume(music->file, volume * 4);
}

/* Load a modplug stream from an SDL_RWops object */
static void *modplug_new_RW(SDL_RWops *src, int freesrc)
{
    modplug_data *music = NULL;
    Sint64 offset;
    size_t sz;
    char *buf;

    /* Make sure the modplug library is loaded */
    if(!Mix_Init(MIX_INIT_MODPLUG))
        return NULL;

    offset = SDL_RWtell(src);
    SDL_RWseek(src, 0, RW_SEEK_END);
    sz = (size_t)(SDL_RWtell(src) - offset);
    SDL_RWseek(src, offset, RW_SEEK_SET);
    buf = (char *)SDL_malloc(sz);
    if(buf)
    {
        if(SDL_RWread(src, buf, sz, 1) == 1)
        {
            music = (modplug_data *)SDL_malloc(sizeof(modplug_data));
            if(music)
            {
                music->playing = 0;
                music->mus_title = NULL;
                music->file = modplug.ModPlug_Load(buf, sz);
                if(!music->file)
                {
                    SDL_free(music);
                    music = NULL;
                }
                else
                    music->mus_title = (char *)ModPlug_GetName(music->file);
            }
            else
                SDL_OutOfMemory();
        }
        SDL_free(buf);
    }
    else
        SDL_OutOfMemory();
    if(music && freesrc)
        SDL_RWclose(src);
    return music;
}

/* Start playback of a given modplug stream */
static void modplug_play(void *music_p)
{
    modplug_data *music = (modplug_data *)music_p;
    modplug.ModPlug_Seek(music->file, 0);
    music->playing = 1;
}

/* Return non-zero if a stream is currently playing */
static int modplug_playing(void *music_p)
{
    modplug_data *music = (modplug_data *)music_p;
    return music && music->playing;
}

/* Play some of a stream previously started with modplug_play() */
static int modplug_playAudio(void *music_p, Uint8 *stream, int len)
{
    modplug_data *music = (modplug_data *)music_p;
    if(current_output_channels > 2)
    {
        int small_len = 2 * len / current_output_channels;
        int i;
        Uint8 *src, *dst;

        i = modplug.ModPlug_Read(music->file, stream, small_len);
        if(i < small_len)
        {
            SDL_memset(stream + i, 0, small_len - i);
            music->playing = 0;
        }
        /* and extend to len by copying channels */
        src = stream + small_len;
        dst = stream + len;

        switch(settings.mBits)
        {
        case 8:
            for(i = small_len / 2; i; --i)
            {
                src -= 2;
                dst -= current_output_channels;
                dst[0] = src[0];
                dst[1] = src[1];
                dst[2] = src[0];
                dst[3] = src[1];
                if(current_output_channels == 6)
                {
                    dst[4] = src[0];
                    dst[5] = src[1];
                }
            }
            break;
        case 16:
            for(i = small_len / 4; i; --i)
            {
                src -= 4;
                dst -= 2 * current_output_channels;
                dst[0] = src[0];
                dst[1] = src[1];
                dst[2] = src[2];
                dst[3] = src[3];
                dst[4] = src[0];
                dst[5] = src[1];
                dst[6] = src[2];
                dst[7] = src[3];
                if(current_output_channels == 6)
                {
                    dst[8] = src[0];
                    dst[9] = src[1];
                    dst[10] = src[2];
                    dst[11] = src[3];
                }
            }
            break;
        }
    }
    else
    {
        int i = modplug.ModPlug_Read(music->file, stream, len);
        if(i < len)
        {
            SDL_memset(stream + i, 0, len - i);
            music->playing = 0;
        }
    }
    if(music_swap8)
    {
        Uint8 *dst;
        int i;

        dst = stream;
        for(i = len; i; --i)
            *dst++ ^= 0x80;
    }
    else if(music_swap16)
    {
        Uint8 *dst, tmp;
        int i;

        dst = stream;
        for(i = (len / 2); i; --i)
        {
            tmp = dst[0];
            dst[0] = dst[1];
            dst[1] = tmp;
            dst += 2;
        }
    }
    return 0;
}

/* Stop playback of a stream previously started with modplug_play() */
static void modplug_stop(void *music_p)
{
    modplug_data *music = (modplug_data *)music_p;
    music->playing = 0;
}

/* Close the given modplug stream */
static void modplug_delete(void *music_p)
{
    modplug_data *music = (modplug_data *)music_p;
    modplug.ModPlug_Unload(music->file);
    /*     if( music->mus_title ) */
    /*         SDL_free(music->mus_title); */
    SDL_free(music);
}

static const char *modplug_metaTitle(void *music_p)
{
    modplug_data *music = (modplug_data *)music_p;
    return music->mus_title ? music->mus_title : "";
}

/* Jump (seek) to a given position (time is in seconds) */
static void modplug_jump_to_time(void *music_p, double time)
{
    modplug_data *music = (modplug_data *)music_p;
    modplug.ModPlug_Seek(music->file, (int)(time * 1000));
}

static double modplug_get_current_time(void *music_p)
{
    modplug_data *music = (modplug_data *)music_p;
    return 0.0;
}
#endif /* MODPLUG_MUSIC */


