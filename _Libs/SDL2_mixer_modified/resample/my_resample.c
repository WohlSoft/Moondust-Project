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

static void SDL_Upsample_S16LSB_1c_KoKoKo(struct MyResampler *res)
{
#ifdef DEBUG_CONVERT
    fprintf(stderr, "Upsample arbitrary (x%f) AUDIO_U16LSB, 1 channel.\n", cvt->rate_incr);
#endif
    const int dstsize = (int)(((double)res->buf_len/2.0) * res->ratio) * 2;
    Uint16 *s_src = (Uint16 *)(res->buf);
    Uint16 *s_dst = (Uint16*)SDL_malloc((size_t)dstsize * sizeof(Uint16));

    double p_src=0.0, q_dst=0.0, r_ps=0.0, s_size=res->buf_len, d_size=dstsize;
    int oldSam0 = (int)p_src;
    Uint16 old0 = s_src[oldSam0];
    double offset=1.0/res->ratio;

    //Resample it!
    for(p_src=0.0, q_dst=0.0, r_ps=0.0; (p_src < s_size) && (q_dst < d_size); q_dst+=1.0 )
    {
        s_dst[(int)q_dst] = old0;
        r_ps += offset;
        if( r_ps >= 1.0 )
        {
            p_src += 1.0;
            r_ps -= 1.0;
            oldSam0 = (int)p_src;
            old0 = s_src[oldSam0];
        }
    }

    int i;
    for(i=0; i<dstsize; ++i)
        s_src[i] = s_dst[i];
    res->buf_len = dstsize;
    free(s_dst);
}

static void SDL_Downsample_S16LSB_1c_KoKoKo(struct MyResampler *res)
{
#ifdef DEBUG_CONVERT
    fprintf(stderr, "Upsample arbitrary (x%f) AUDIO_U16LSB, 1 channel.\n", cvt->rate_incr);
#endif
    const int dstsize = (int)(((double)res->buf_len/2.0) * res->ratio) * 2;
    Uint16 *s_src = (Uint16 *) (res->buf);
    Uint16 *s_dst = (Uint16*)SDL_malloc((size_t)dstsize * sizeof(Uint16));

    double p_src=0.0, q_dst=0.0, r_ps=0.0, s_size=res->buf_len, d_size=dstsize;
    int oldSam0 = (int)p_src;
    Uint16 old0 = s_src[oldSam0];
    s_dst[(int)q_dst]   = old0;

    double offset=res->ratio;
    //Resample it!
    for(p_src=0.0, q_dst=0.0, r_ps=0.0; (p_src < s_size) && (q_dst < d_size); p_src+=1.0 )
    {
        r_ps += offset;
        if( r_ps >= 1.0 )
        {
            oldSam0 = (int)p_src;
            old0 = s_src[oldSam0];
            s_dst[(int)q_dst]   = old0;
            q_dst += 1.0;
            r_ps -= 1.0;
        }
    }

    int i;
    for(i=0; i < dstsize; ++i)
        s_src[i] = s_dst[i];
    res->buf_len = dstsize;
    free(s_dst);
}


static void SDL_Upsample_S16LSB_2c_KoKoKo(struct MyResampler *res)
{
#ifdef DEBUG_CONVERT
    fprintf(stderr, "Upsample arbitrary (x%f) AUDIO_S16LSB, 2 channels.\n", cvt->rate_incr);
#endif
    const int dstsize = (int)(((double)res->buf_len/4.0) * res->ratio) * 4;
    Uint16 *s_src = (Uint16 *) (res->buf);
    Uint16 *s_dst = (Uint16*)SDL_malloc((size_t)dstsize * sizeof(Uint16));

    double p_src=0.0, q_dst=0.0, r_ps=0.0, s_size=res->buf_len, d_size=dstsize;
    int oldSam1 = (int)p_src+1;
    int oldSam0 = (int)p_src;
    Uint16 old1 = s_src[oldSam1];
    Uint16 old0 = s_src[oldSam0];
    double offset=1.0/res->ratio;

    //Resample it!
    for(p_src=0.0, q_dst=0.0, r_ps=0.0; (p_src < s_size) && (q_dst < d_size); q_dst+=2.0 )
    {
        s_dst[(int)q_dst+1] = old1;
        s_dst[(int)q_dst]   = old0;
        r_ps += offset;
        if( r_ps >= 1.0 )
        {
            p_src += 2.0;
            r_ps -= 1.0;
            oldSam1 = (int)p_src+1;
            oldSam0 = (int)p_src;
            old1 = s_src[oldSam1];
            old0 = s_src[oldSam0];
        }
    }

    int i;
    for(i=0; i < dstsize; ++i)
        s_src[i] = s_dst[i];
    res->buf_len = dstsize;
    free(s_dst);
}

static void SDL_Downsample_S16LSB_2c_KoKoKo(struct MyResampler *res)
{
#ifdef DEBUG_CONVERT
    fprintf(stderr, "Upsample arbitrary (x%f) AUDIO_S16LSB, 2 channels.\n", cvt->rate_incr);
#endif
    const int dstsize = (int)(((double)res->buf_len/4.0) * res->ratio) * 4;
    Uint16 *s_src = (Uint16 *) (res->buf);
    Uint16 *s_dst = (Uint16*)SDL_malloc((size_t)dstsize * sizeof(Uint16));

    double p_src = 0.0, q_dst = 0.0, r_ps = 0.0, s_size=res->buf_len, d_size = dstsize;
    int oldSam1 = (int)p_src+1;
    int oldSam0 = (int)p_src;
    Uint16 old1 = s_src[oldSam1];
    Uint16 old0 = s_src[oldSam0];
    s_dst[(int)q_dst+1] = old1;
    s_dst[(int)q_dst]   = old0;

    double offset = res->ratio;
    //Resample it!
    for(p_src=0.0, q_dst=0.0, r_ps=0.0; (p_src < s_size) && (q_dst < d_size); p_src+=2.0 )
    {
        r_ps += offset;
        if( r_ps >= 1.0 )
        {
            oldSam1 = (int)p_src+1;
            oldSam0 = (int)p_src;
            old1 = s_src[oldSam1];
            old0 = s_src[oldSam0];
            s_dst[(int)q_dst+1] = old1;
            s_dst[(int)q_dst]   = old0;
            q_dst += 2.0;
            r_ps -= 1.0;
        }
    }

    int i;
    for(i=0; i<dstsize; ++i)
        s_src[i] = s_dst[i];
    res->buf_len = dstsize;
    free(s_dst);
}


//Upsample ANY number of channels
static void SDL_Upsample_S16LSB_Xc_KoKoKo(struct MyResampler *res)
{
#ifdef DEBUG_CONVERT
    fprintf(stderr, "Upsample arbitrary (x%f) AUDIO_S16LSB, %i channels.\n", res->ratio, res->channels );
#endif

    int channels = res->channels;
    double channells_f = (double)channels;
    int chan_coeff=2*res->channels;

    const int dstsize = (int)(((double)res->buf_len/chan_coeff) * res->ratio) * chan_coeff;
    Uint16 *s_src = (Uint16 *) (res->buf);
    Uint16 *s_dst = (Uint16*)SDL_malloc((size_t)dstsize * sizeof(Uint16));

    double p_src=0.0, q_dst=0.0, r_ps=0.0, s_size=res->buf_len, d_size=dstsize;

    int i;
    int *oldSam = (int*)SDL_malloc((size_t)channels * sizeof(int));
    Uint16 *old = (Uint16*)SDL_malloc((size_t)channels * sizeof(Uint16));
    for(i=0;i<channels;++i)
    {
        oldSam[i] = (int)p_src+i;
        old[i] = s_src[oldSam[i]];
    }

    double offset=1.0/res->ratio;

    //Resample it!
    for(p_src=0.0, q_dst=0.0, r_ps=0.0; (p_src < s_size) && (q_dst < d_size); q_dst+=channells_f )
    {
        for(i=0;i<channels;++i)
            s_dst[(int)q_dst+i] = old[i];
        r_ps += offset;
        if( r_ps >= 1.0 )
        {
            p_src += channells_f;
            r_ps -= 1.0;
            for(i=0;i<channels;i++)
            {
                oldSam[i] = (int)p_src+i;
                old[i] = s_src[oldSam[i]];
            }
        }
    }

    for(i=0; i < dstsize; ++i)
        s_src[i] = s_dst[i];
    res->buf_len = dstsize;
    free(s_dst);
    free(oldSam);
    free(old);
}

//Downsample ANY number of channels
static void SDL_Downsample_S16LSB_Xc_KoKoKo(struct MyResampler *res)
{
#ifdef DEBUG_CONVERT
    fprintf(stderr, "Upsample arbitrary (x%f) AUDIO_S16LSB, %i channels.\n", res->ratio, res->channels );
#endif
    if(res->channels<=0) return;

    int channels = res->channels;
    double channells_f = (double)channels;
    int chan_coeff=2*res->channels;

    const int dstsize = (int)(((double)res->buf_len/(double)chan_coeff) * res->ratio) * chan_coeff;
    Uint16 *s_src = (Uint16 *) (res->buf);
    Uint16 *s_dst = (Uint16*)SDL_malloc((size_t)dstsize * sizeof(Uint16));

    double p_src=0.0, q_dst=0.0, r_ps=0.0, s_size=res->buf_len, d_size=dstsize;

    int i=0;
    int *oldSam = (int*)SDL_malloc((size_t)channels * sizeof(int));
    Uint16 *old = (Uint16*)SDL_malloc((size_t)channels * sizeof(Uint16));

    for(i=0; i<channels; ++i)
    {
        oldSam[i] = (int)p_src+i;
        old[i]    = s_src[oldSam[i]];
        s_dst[(int)q_dst+i] = old[i];
    }

    double offset = res->ratio;

    //Resample it!
    for(p_src=0.0, q_dst=0.0, r_ps=0.0; (p_src < s_size) && (q_dst < d_size); p_src+=channells_f )
    {
        r_ps += offset;
        if( r_ps >= 1.0 )
        {
            for(i=0;i<channels;++i)
            {
                oldSam[i] = (int)p_src+1;
                old[i] = s_src[oldSam[i]];
                s_dst[(int)q_dst+i] = old[i];
            }
            q_dst += channells_f;
            r_ps -= 1.0;
        }
    }

    for(i=0; i < dstsize; ++i)
        s_src[i] = s_dst[i];
    res->buf_len = dstsize;
    free(s_dst);
    free(oldSam);
    free(old);
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
        {
            switch(res->channels)
            {
                case 1://Upsample mono stream
                    res->filter = &SDL_Upsample_S16LSB_1c_KoKoKo;break;
                case 2://Upsample stereo stream
                    res->filter = &SDL_Upsample_S16LSB_2c_KoKoKo;break;
                default://Upsample stream with X channels number
                    res->filter = &SDL_Upsample_S16LSB_Xc_KoKoKo;break;
            }
        } else {
            switch(res->channels)
            {
                case 1://Downsample mono stream
                    res->filter = &SDL_Downsample_S16LSB_1c_KoKoKo;break;
                case 2://Downsample stereo stream
                    res->filter = &SDL_Downsample_S16LSB_2c_KoKoKo;break;
                default://Downsample stream with X channels number
                    res->filter = &SDL_Downsample_S16LSB_Xc_KoKoKo;break;
            }
        }
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
