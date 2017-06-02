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

#ifdef MP3_MUSIC

#include <SDL2/SDL_loadso.h>
#include <SDL_mixer_ext/SDL_mixer_ext.h>
#include "dynamic_mp3.h"

smpeg_loader smpeg = {
    0, NULL
};

#ifdef MP3_DYNAMIC
int Mix_InitMP3()
{
    if ( smpeg.loaded == 0 ) {
        smpeg.handle = SDL_LoadObject(MP3_DYNAMIC);
        if ( smpeg.handle == NULL ) {
            return -1;
        }
        smpeg.SMPEG_actualSpec =
            (void (*)( SMPEG *, SDL_AudioSpec * ))
            SDL_LoadFunction(smpeg.handle, "SMPEG_actualSpec");
        if ( smpeg.SMPEG_actualSpec == NULL ) {
            SDL_UnloadObject(smpeg.handle);
            return -1;
        }
        smpeg.SMPEG_delete =
            (void (*)( SMPEG* ))
            SDL_LoadFunction(smpeg.handle, "SMPEG_delete");
        if ( smpeg.SMPEG_delete == NULL ) {
            SDL_UnloadObject(smpeg.handle);
            return -1;
        }
        smpeg.SMPEG_enableaudio =
            (void (*)( SMPEG*, int ))
            SDL_LoadFunction(smpeg.handle, "SMPEG_enableaudio");
        if ( smpeg.SMPEG_enableaudio == NULL ) {
            SDL_UnloadObject(smpeg.handle);
            return -1;
        }
        smpeg.SMPEG_enablevideo =
            (void (*)( SMPEG*, int ))
            SDL_LoadFunction(smpeg.handle, "SMPEG_enablevideo");
        if ( smpeg.SMPEG_enablevideo == NULL ) {
            SDL_UnloadObject(smpeg.handle);
            return -1;
        }
        smpeg.SMPEG_new_rwops =
            (SMPEG* (*)(SDL_RWops *, SMPEG_Info*, int, int))
            SDL_LoadFunction(smpeg.handle, "SMPEG_new_rwops");
        if ( smpeg.SMPEG_new_rwops == NULL ) {
            SDL_UnloadObject(smpeg.handle);
            return -1;
        }
        smpeg.SMPEG_play =
            (void (*)( SMPEG* ))
            SDL_LoadFunction(smpeg.handle, "SMPEG_play");
        if ( smpeg.SMPEG_play == NULL ) {
            SDL_UnloadObject(smpeg.handle);
            return -1;
        }
        smpeg.SMPEG_playAudio =
            (int (*)( SMPEG *, Uint8 *, int ))
            SDL_LoadFunction(smpeg.handle, "SMPEG_playAudio");
        if ( smpeg.SMPEG_playAudio == NULL ) {
            SDL_UnloadObject(smpeg.handle);
            return -1;
        }
        smpeg.SMPEG_rewind =
            (void (*)( SMPEG* ))
            SDL_LoadFunction(smpeg.handle, "SMPEG_rewind");
        if ( smpeg.SMPEG_rewind == NULL ) {
            SDL_UnloadObject(smpeg.handle);
            return -1;
        }
        smpeg.SMPEG_setvolume =
            (void (*)( SMPEG*, int ))
            SDL_LoadFunction(smpeg.handle, "SMPEG_setvolume");
        if ( smpeg.SMPEG_setvolume == NULL ) {
            SDL_UnloadObject(smpeg.handle);
            return -1;
        }
        smpeg.SMPEG_skip =
            (void (*)( SMPEG*, float ))
            SDL_LoadFunction(smpeg.handle, "SMPEG_skip");
        if ( smpeg.SMPEG_skip == NULL ) {
            SDL_UnloadObject(smpeg.handle);
            return -1;
        }
        smpeg.SMPEG_status =
            (SMPEGstatus (*)( SMPEG* ))
            SDL_LoadFunction(smpeg.handle, "SMPEG_status");
        if ( smpeg.SMPEG_status == NULL ) {
            SDL_UnloadObject(smpeg.handle);
            return -1;
        }
        smpeg.SMPEG_stop =
            (void (*)( SMPEG* ))
            SDL_LoadFunction(smpeg.handle, "SMPEG_stop");
        if ( smpeg.SMPEG_stop == NULL ) {
            SDL_UnloadObject(smpeg.handle);
            return -1;
        }
    }
    ++smpeg.loaded;

    return 0;
}
void Mix_QuitMP3()
{
    if ( smpeg.loaded == 0 ) {
        return;
    }
    if ( smpeg.loaded == 1 ) {
        SDL_UnloadObject(smpeg.handle);
    }
    --smpeg.loaded;
}
#else
int Mix_InitMP3()
{
    if ( smpeg.loaded == 0 ) {
#ifdef __MACOSX__
        extern SMPEG* SMPEG_new_rwops(SDL_RWops*, SMPEG_Info*, int, int) __attribute__((weak_import));
        if ( SMPEG_new_rwops == NULL )
        {
            /* Missing weakly linked framework */
            Mix_SetError("Missing smpeg2.framework");
            return -1;
        }
#endif /*  __MACOSX__ */

        smpeg.SMPEG_actualSpec = SMPEG_actualSpec;
        smpeg.SMPEG_delete = SMPEG_delete;
        smpeg.SMPEG_enableaudio = SMPEG_enableaudio;
        smpeg.SMPEG_enablevideo = SMPEG_enablevideo;
        smpeg.SMPEG_new_rwops = SMPEG_new_rwops;
        smpeg.SMPEG_play = SMPEG_play;
        smpeg.SMPEG_playAudio = SMPEG_playAudio;
        smpeg.SMPEG_rewind = SMPEG_rewind;
        smpeg.SMPEG_setvolume = SMPEG_setvolume;
        smpeg.SMPEG_skip = SMPEG_skip;
        smpeg.SMPEG_status = SMPEG_status;
        smpeg.SMPEG_stop = SMPEG_stop;
    }
    ++smpeg.loaded;

    return 0;
}
void Mix_QuitMP3()
{
    if ( smpeg.loaded == 0 ) {
        return;
    }
    if ( smpeg.loaded == 1 ) {
    }
    --smpeg.loaded;
}
#endif /* MP3_DYNAMIC */

static SDL_AudioSpec global_mixer;

static void *SMPEG_openFileRW(SDL_RWops *src, int freesrc)
{
    SMPEG *mp3;
    SMPEG_Info info;
    mp3 = smpeg.SMPEG_new_rwops(src, &info, freesrc, 0);
    if(!info.has_audio)
    {
        Mix_SetError("MPEG file does not have any audio stream.");
        smpeg.SMPEG_delete(mp3);
        /* Deleting the MP3 closed the source if desired */
        freesrc = SDL_FALSE;
        mp3 = NULL;
    }
    else
    {
        smpeg.SMPEG_actualSpec(mp3, &global_mixer);
    }
    return mp3;
}

static void SMPEG_closeFile(void *mp3_p)
{
    SMPEG *mp3 = (SMPEG*)mp3_p;
    smpeg.SMPEG_delete(mp3);
}

static int SMPEG_isPlaying(void *mp3_p)
{
    SMPEG *mp3 = (SMPEG*)mp3_p;
    return smpeg.SMPEG_status(mp3) == SMPEG_PLAYING ? 1 : 0;
}

static void SMPEG_start(void *mp3_p)
{
    SMPEG *mp3 = (SMPEG*)mp3_p;
    smpeg.SMPEG_enableaudio(mp3, 1);
    smpeg.SMPEG_enablevideo(mp3, 0);
    smpeg.SMPEG_play(mp3);
}

static void SMPEG_stopMix(void *mp3_p)
{
    SMPEG *mp3 = (SMPEG*)mp3_p;
    smpeg.SMPEG_stop(mp3);
}

static void SMPEG_setVolume(void *mp3_p, int volume)
{
    SMPEG *mp3 = (SMPEG*)mp3_p;
    smpeg.SMPEG_setvolume(mp3, (int)(((float)volume / (float)MIX_MAX_VOLUME) * 100.0f));
}

static void SMPEG_seekMix(void *mp3_p, double position)
{
    SMPEG *mp3 = (SMPEG*)mp3_p;
    smpeg.SMPEG_rewind(mp3);
    smpeg.SMPEG_play(mp3);
    if(position > 0.0)
        smpeg.SMPEG_skip(mp3, (float)position);
}

static int SMPEG_getSamples(void *mp3_p, Uint8 *stream, int len)
{
    SMPEG *mp3 = (SMPEG*)mp3_p;
    return (len - smpeg.SMPEG_playAudio(mp3, stream, len));
}


int SMPEG_init2(AudioCodec* codec, SDL_AudioSpec *mixerfmt)
{
    global_mixer = *mixerfmt;

    codec->isValid = 1;

    codec->capabilities     = audioCodec_default_capabilities;

    codec->open             = SMPEG_openFileRW;
    codec->openEx           = audioCodec_dummy_cb_openEx;
    codec->close            = SMPEG_closeFile;

    codec->play             = SMPEG_start;
    codec->pause            = audioCodec_dummy_cb_void_1arg;
    codec->resume           = audioCodec_dummy_cb_void_1arg;
    codec->stop             = SMPEG_stopMix;

    codec->isPlaying        = SMPEG_isPlaying;
    codec->isPaused         = audioCodec_dummy_cb_int_1arg;

    codec->setLoops         = audioCodec_dummy_cb_regulator;
    codec->setVolume        = SMPEG_setVolume;

    codec->jumpToTime       = SMPEG_seekMix;
    codec->getCurrentTime   = audioCodec_dummy_cb_tell;

    codec->metaTitle        = audioCodec_dummy_meta_tag;
    codec->metaArtist       = audioCodec_dummy_meta_tag;
    codec->metaAlbum        = audioCodec_dummy_meta_tag;
    codec->metaCopyright    = audioCodec_dummy_meta_tag;

    codec->playAudio        = SMPEG_getSamples;

    return(0);
}

#else
int  Mix_InitMP3() {return 0;}
void Mix_QuitMP3() {}
#endif /* MP3_MUSIC */
