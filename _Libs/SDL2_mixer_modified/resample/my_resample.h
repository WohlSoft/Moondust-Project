/*
  SDL_mixer_ext:  An extended audio mixer library, forked from SDL_mixer
  Copyright (C) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>

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

# ifndef RESAMPLE_H
# define RESAMPLE_H

#include "SDL_mixer_ext.h"

#define MyResampler_BUFFER_SIZE 204800

struct MyResampler
{
    int   needed;
    double ratio;
    int buf_len;
    int channels;
    Uint8 buf[MyResampler_BUFFER_SIZE];//200 KB for backup stuff;
    void (*filter)(struct MyResampler *res);
};

void MyResample_zero(struct MyResampler *res);

void MyResample_init(struct MyResampler *res, int rate_in, int rate_out, int channels, SDL_AudioFormat format);

void MyResample_addSource(struct MyResampler *res, Uint8* in_buffer, int in_len);

void MyResample_Process(struct MyResampler *res);

void MyResample_dequeueBytes(struct MyResampler *res, int numbytes);

void MyResample_clear(struct MyResampler *res);

# endif
