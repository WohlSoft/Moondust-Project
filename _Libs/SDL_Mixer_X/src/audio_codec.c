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

#include "audio_codec.h"

void audioCodec_dummy_cb_void_1arg(AudioCodecStream* music)
{
    (void)music;
}

int  audioCodec_dummy_cb_int_1arg(AudioCodecStream* music)
{
    (void)music;
    return 0;
}

const char *audioCodec_dummy_meta_tag(AudioCodecStream* music)
{
    (void)music;
    return "";
}

void audioCodec_dummy_cb_seek(AudioCodecStream* music, double position)
{
    (void)music;
    (void)position;
}

double audioCodec_dummy_cb_tell(AudioCodecStream* music)
{
    (void)music;
    return 0.0;
}

void audioCodec_dummy_cb_regulator(AudioCodecStream *music, int value)
{
    (void)music;
    (void)value;
}
