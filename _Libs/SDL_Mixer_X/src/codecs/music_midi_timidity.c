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

#include "music_midi_timidity.h"
#include <timidity.h>

static int samplesize = 2;

static int Timidity_playAudio(void *music_p, Uint8 *stream, int len)
{
    (void)music_p;
    int samples = len / samplesize;
    Timidity_PlaySome(stream, samples);
    return 0;
}

static Uint32 Timidity_capabilities()
{
    return ACODEC_SINGLETON;
}

static void Timidity_delete(void *music_p)
{
    MidiSong*music = (MidiSong*)music_p;
    Timidity_FreeSong(music);
}

int Timidity_init2(AudioCodec *codec, SDL_AudioSpec *mixer)
{
    samplesize = (int)mixer->size / mixer->samples;

    initAudioCodec(codec);

    codec->isValid = (Timidity_Init(mixer->freq,
                                    mixer->format,
                                    mixer->channels,
                                    mixer->samples) == 0) ? 1 : 0;

    codec->capabilities     = Timidity_capabilities;

    codec->open             = Timidity_LoadSong_RW;
    codec->openEx           = audioCodec_dummy_cb_openEx;
    codec->close            = Timidity_delete;

    codec->play             = Timidity_Start;
    codec->pause            = audioCodec_dummy_cb_void_1arg;
    codec->resume           = audioCodec_dummy_cb_void_1arg;
    codec->stop             = Timidity_Stop;

    codec->isPlaying        = Timidity_Active;
    codec->isPaused         = audioCodec_dummy_cb_int_1arg;

    codec->setLoops         = audioCodec_dummy_cb_regulator;
    codec->setVolume        = Timidity_SetVolume;

    codec->jumpToTime       = audioCodec_dummy_cb_seek;
    codec->getCurrentTime   = audioCodec_dummy_cb_tell;
    codec->getTimeLength    = audioCodec_dummy_cb_tell;

    codec->metaTitle        = audioCodec_dummy_meta_tag;
    codec->metaArtist       = audioCodec_dummy_meta_tag;
    codec->metaAlbum        = audioCodec_dummy_meta_tag;
    codec->metaCopyright    = audioCodec_dummy_meta_tag;

    codec->playAudio        = Timidity_playAudio;

    return(codec->isValid ? 0 : -1);
}
