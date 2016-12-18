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

/* $Id$ */

/* This file supports safe and better resampling than built-in with SDL.
   Currently works for Signed 16-bit audio only, there are would work slower than SDL's */

#include "my_resample.h"

//Upsample ANY number of channels
static void SDL_Upsample_S16LSB_Xc_MixerX(struct MyResampler *res)
{
#ifdef DEBUG_CONVERT
    fprintf(stderr, "Upsample arbitrary (x%f) AUDIO_S16LSB, %i channels.\n", res->ratio, res->channels );
#endif
#define SAMPLE_TYPE Sint16
    static const int sampleSize = sizeof(SAMPLE_TYPE);
    int channels = res->channels;
    // Size of one sample multipled to number of channels
    int cpy = sampleSize * channels;
    int s_size = res->buf_len / cpy;
    int d_size = (int)(((double)s_size) * res->ratio);

    SAMPLE_TYPE *s_src = (SAMPLE_TYPE *)(res->buf);
    SAMPLE_TYPE *p_src = s_src + (s_size * channels) - channels;
    SAMPLE_TYPE *p_dst = s_src + (d_size * channels) - channels;

    SAMPLE_TYPE *sample = (SAMPLE_TYPE*)SDL_malloc((size_t)cpy);
    #if USE_INTERPOLATE
    SAMPLE_TYPE *last_sample = (SAMPLE_TYPE*)SDL_malloc((size_t)cpy);
    #endif

    double r_ps     = 0.0;
    double offset   = 1.0 / res->ratio;

    SDL_memcpy(sample, p_src, cpy);
    #ifdef USE_INTERPOLATE
    SDL_memcpy(last_sample, p_src, cpy);
    #endif

    #ifdef USE_INTERPOLATE
    int i = 0;
    #endif
    while(p_dst > s_src)
    {
        SDL_memcpy(p_dst, sample, cpy);
        p_dst -= channels;
        r_ps += offset;
        if(r_ps >= 1.0)
        {
            p_src -= channels;
            #if USE_INTERPOLATE
            for (i = 0; i < channels; i++) {
                sample[i] = (SAMPLE_TYPE)( ((((double)p_src[i]) + (double)last_sample[i]) * 0.5) );
            }
            SDL_memcpy(last_sample, sample, cpy);
            #else
            SDL_memcpy(sample, p_src, cpy);
            #endif
            r_ps -= 1.0;
        }
    }

    res->buf_len = (int)(d_size*cpy);
    free(sample);
    #ifdef USE_INTERPOLATE
    free(last_sample);
    #endif
#undef SAMPLE_TYPE
}

//Downsample ANY number of channels
static void SDL_Downsample_S16LSB_Xc_MixerX(struct MyResampler *res)
{
#ifdef DEBUG_CONVERT
    fprintf(stderr, "Upsample arbitrary (x%f) AUDIO_S16LSB, %i channels.\n", res->ratio, res->channels );
#endif
#define SAMPLE_TYPE Sint16
    static const int sampleSize = sizeof(SAMPLE_TYPE);
    int channels = res->channels;
    // Size of one sample multipled to number of channels
    int cpy = sampleSize * channels;
    int s_size = res->buf_len / cpy;
    int d_size = (int)(((double)s_size) * res->ratio);

    SAMPLE_TYPE *s_src = (SAMPLE_TYPE *)(res->buf);
    SAMPLE_TYPE *s_src_end = s_src + (s_size * channels);
    SAMPLE_TYPE *p_src = s_src;
    SAMPLE_TYPE *p_dst = s_src;

    SAMPLE_TYPE *sample = (SAMPLE_TYPE*)SDL_malloc((size_t)cpy);
    #if USE_INTERPOLATE
    SAMPLE_TYPE *last_sample = (SAMPLE_TYPE*)SDL_malloc((size_t)cpy);
    #endif

    double r_ps     = 0.0;
    double offset   = res->ratio;

    SDL_memcpy(sample, p_src, cpy);
    #ifdef USE_INTERPOLATE
    SDL_memcpy(last_sample, p_src, cpy);
    #endif

    #ifdef USE_INTERPOLATE
    int i = 0;
    #endif
    while(p_src < s_src_end)
    {
        p_src += channels;
        r_ps += offset;
        if(r_ps >= 1.0)
        {
            SDL_memcpy(p_dst, sample, cpy);
            p_dst += channels;
            #if USE_INTERPOLATE
            for (i = 0; i < channels; i++) {
                sample[i] = (SAMPLE_TYPE) ((((double) p_src[i]) + ((double)last_sample[i])) * 0.5);
            }
            SDL_memcpy(last_sample, sample, cpy);
            #else
            SDL_memcpy(sample, p_src, cpy);
            #endif
            r_ps -= 1.0;
        }
    }

    res->buf_len = (int)(d_size*cpy);
    free(sample);
    #ifdef USE_INTERPOLATE
    free(last_sample);
    #endif
#undef SAMPLE_TYPE
}

static void doNothing(struct MyResampler *res) {
    (void)res;
}

void MyResample_init(struct MyResampler *res, int rate_in, int rate_out , int channels, SDL_AudioFormat format)
{
    (void)format; // Temporary not needed :-P
    res->needed = (rate_in != rate_out);
    res->ratio = (double)rate_out/(double)rate_in;
    res->len_mult = (res->ratio >= 1.0) ? (int)ceil(res->ratio) : 1;
    res->buf_len = 0;
    res->channels = channels;
    res->filter = &doNothing;
    if(res->needed)
    {
        if(res->ratio > 1.0)
            res->filter = &SDL_Upsample_S16LSB_Xc_MixerX;
        else
            res->filter = &SDL_Downsample_S16LSB_Xc_MixerX;
    }
}

void MyResample_addSource(struct MyResampler *res, Uint8* in_buffer, int in_len)
{
    SDL_memcpy(res->buf, in_buffer, (size_t)in_len);
    res->buf_len = in_len;
}

void MyResample_Process(struct MyResampler *res)
{
    if(!res) return;
    if(res->needed == 0)
        return;
    if(res->buf_len == 0)
        return;

    res->filter(res);
}

void MyResample_dequeueBytes(struct MyResampler *res, int numbytes)
{
    if( (res->buf_len-numbytes) > 0 )
    {   //Move tail of buffer into begin of array
        int i=numbytes;
        Uint8 *dst = &res->buf[0];
        Uint8 *src = &res->buf[i];
        for(; i<res->buf_len;++i)
            *dst++ = *src++;
        res->buf_len -= numbytes;
    } else {
        res->buf_len = 0;
    }
}

void MyResample_clear(struct MyResampler *res)
{
    res->buf_len = 0;
}

void MyResample_zero(struct MyResampler *res)
{
    res->buf_len = 0;
    res->channels = 2;
    res->ratio = 1.0;
    res->len_mult = 1;
    res->needed = 0;
    res->filter = &doNothing;
}
