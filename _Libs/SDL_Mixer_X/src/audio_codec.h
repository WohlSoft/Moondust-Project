/*
  SDL_mixer:  An audio mixer library based on the SDL library
  Copyright (C) 1997-2017 Sam Lantinga <slouken@libsdl.org>

  SDL Mixer X: A fork of SDL_mixer audio mixing library
  Copyright (C) 2015-2017 Vitaly Novichkov <admin@wohlnet.ru>

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

#ifndef AUDIO_CODEC_H
#define AUDIO_CODEC_H

#include <SDL2/SDL_types.h>
#include <SDL2/SDL_rwops.h>
#include <SDL2/SDL_audio.h>

typedef void AudioCodecStream;

typedef enum
{
    ACODEC_NOCAPS               = 0x00000000,/* Codec has no any special capabilities */

    ACODEC_ASYNC                = 0x00000001,/* Asyncronious player which plays audio into separately opened output
                                                (Examples: CMD-Music, when music playing by external tool,
                                                           Native-Midi, when music playing via external driver
                                                           provided by operating system)*/

    ACODEC_NEED_VOLUME_INIT     = 0x00000002,/* Need to additionally initialize the volume value before playing begin */

    ACODEC_NEED_VOLUME_INIT_POST= 0x00000004,/* Need to additionally initialize the volume value after playing began */

    ACODEC_SINGLETON            = 0x00000008,/* Codec is able to play only one song in same time.
                                                You can't play two or more concurrent tracks playing by this same codec
                                                (Examples: MikMod because of interface of library,
                                                           NativeMidi because one external device
                                                           is able to play only one MIDI file) */
    ACODEC_HAS_PAUSE            = 0x00000010 /* Codec has own pause state */
} AudioCodec_Caps;

/*
    Dummy callbacks. Use them if codec doesn't supports some features
*/
Uint32      audioCodec_default_capabilities();
void       *audioCodec_dummy_cb_open(SDL_RWops* src, int freesrc);
void       *audioCodec_dummy_cb_openEx(SDL_RWops* src, int freesrc, const char *extraSettings);
void        audioCodec_dummy_cb_void_1arg(AudioCodecStream* music);
int         audioCodec_dummy_cb_int_1arg(AudioCodecStream* music);
const char *audioCodec_dummy_meta_tag(AudioCodecStream* music);
void        audioCodec_dummy_cb_regulator(AudioCodecStream* music, int value);
void        audioCodec_dummy_cb_seek(AudioCodecStream* music, double position);
double      audioCodec_dummy_cb_tell(AudioCodecStream* music);
int         audioCodec_dummy_playAudio(AudioCodecStream* music, Uint8* data, int length);

/* A generic audio playing codec interface interface */
typedef struct _AudioCodec
{
    int     isValid;

    /* Capabilities of the codec */
    Uint32 (*capabilities)(void);

    AudioCodecStream* (*open)(SDL_RWops* src, int freesrc);
    AudioCodecStream* (*openEx)(SDL_RWops* src, int freesrc, const char *extraSettings);
    void  (*close)(AudioCodecStream* music);

    void  (*play)(AudioCodecStream* music);
    void  (*pause)(AudioCodecStream* music);
    void  (*resume)(AudioCodecStream* music);
    void  (*stop)(AudioCodecStream* music);

    int   (*isPlaying)(AudioCodecStream* music);
    int   (*isPaused)(AudioCodecStream* music);

    void  (*setLoops)(AudioCodecStream* music, int loopsCount);
    void  (*setVolume)(AudioCodecStream* music, int volume);

    double (*getCurrentTime)(AudioCodecStream* music);
    void  (*jumpToTime)(AudioCodecStream* music, double position);

    const char* (*metaTitle)(AudioCodecStream* music);
    const char* (*metaArtist)(AudioCodecStream* music);
    const char* (*metaAlbum)(AudioCodecStream* music);
    const char* (*metaCopyright)(AudioCodecStream* music);

    int   (*playAudio)(AudioCodecStream* music, Uint8* data, int length);

} AudioCodec;

#endif /* AUDIO_CODEC_H*/

