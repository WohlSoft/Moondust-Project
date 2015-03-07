#ifdef SPC_MUSIC

/* This file supports MOD tracker music streams */

#include "SDL_mixer.h"
#include "music_spc.h"

#include "snes_spc/spc.h"
#include "resample/mad_resample.h"

#include <stdio.h>

#define SDL_SURROUND
#ifdef SDL_SURROUND
#define MAX_OUTPUT_CHANNELS 6
#else
#define MAX_OUTPUT_CHANNELS 2
#endif

/* Reference for converting mikmod output to 4/6 channels */
static int current_output_channels;
static Uint16 current_output_format;

static int music_swap8;
static int music_swap16;

SNES_SPC* snes_spc=NULL;
static SPC_Filter* spc_filter=NULL;

int spc_is_playing=-1;
int spc_t_sample_rate=44100;

short last_left=0;
short last_right=0;

//struct resample_state spc_resampler_state_l;
//struct resample_state spc_resampler_state_r;

/* Initialize the MOD player, with the given mixer settings
   This function returns 0, or -1 if there was an error.
 */

/* This is the format of the audio mixer data */
static SDL_AudioSpec mixer;

int SPC_init(SDL_AudioSpec *mixerfmt)
{
    if(snes_spc)
    {
        return 0;
    }
    if(spc_filter)
    {
        return 0;
    }
    spc_is_playing=-1;

    snes_spc = spc_new();
    spc_filter = spc_filter_new();
    if ( !snes_spc || !spc_filter )
    {
        Mix_SetError("SNES_SPC: Out of memory");
        return -1;
    }

    spc_t_sample_rate = mixerfmt->freq;

    current_output_channels = mixerfmt->channels;
    current_output_format = mixerfmt->format;

    music_swap8 = 0;
    music_swap16 = 0;

    mixer = *mixerfmt;

    switch (mixerfmt->format) {

        case AUDIO_U8:
        case AUDIO_S8: {
            if ( mixerfmt->format == AUDIO_S8 ) {
                music_swap8 = 1;
            }
        }
        break;

        case AUDIO_S16LSB:
        case AUDIO_S16MSB: {
            /* See if we need to correct MikMod mixing */
        #if SDL_BYTEORDER == SDL_LIL_ENDIAN
                    if ( mixerfmt->format == AUDIO_S16MSB ) {
        #else
                    if ( mixerfmt->format == AUDIO_S16LSB ) {
        #endif
                music_swap16 = 1;
            }
        }
        break;

        default: {
            Mix_SetError("Unknown hardware audio format");
            return -1;
        }
    }
    return 0;
}

/* Uninitialize the music players */
void SPC_exit(void)
{
    if(spc_filter)
    {
        spc_filter_delete( spc_filter );
        spc_filter=NULL;
    }

    if (snes_spc) {
        spc_delete( snes_spc );
        snes_spc=NULL;
    }
}

/* Set the volume for a MOD stream */
void SPC_setvolume(struct MUSIC_SPC *music, int volume)
{
    if(music)
        music->volume=volume;
}

struct MUSIC_SPC *SnesSPC_LoadSongRW(SDL_RWops *src)
{
    if(src != NULL) {

        void *bytes=0;
        long spcsize;

        Sint64 length=0;
        length = SDL_RWseek(src, 0, RW_SEEK_END);
        if (length < 0)
        {
            Mix_SetError("SNES_SPC: wrong file\n");
            return NULL;
        }

        SDL_RWseek(src, 0, RW_SEEK_SET);
        bytes = malloc(length);

        long bytes_l;
        unsigned char byte[1];
        spcsize=0;
        while( (bytes_l=SDL_RWread(src, &byte, sizeof(unsigned char), 1))!=0)
        {
            ((unsigned char*)bytes)[spcsize] = byte[0];
            spcsize++;
        }

        if (spcsize == 0)
        {
            Mix_SetError("SNES_SPC: wrong file\n");
            return NULL;
        }

        char *err = (char*)spc_load_spc( snes_spc, bytes, spcsize );
        free(bytes);
        if(err!=0)
        {
            Mix_SetError("SNES_SPC: %s", err);
            return NULL;
        }

        struct MUSIC_SPC *spcSpec = (struct MUSIC_SPC*)malloc(sizeof(struct MUSIC_SPC));
        spcSpec->playing=0;
        spcSpec->spc_t_sample_rate=mixer.freq;
        spcSpec->volume=MIX_MAX_VOLUME;
        return spcSpec;
    }
    return NULL;
}

/* Load a MOD stream from an SDL_RWops object */
struct MUSIC_SPC *SPC_new_RW(struct SDL_RWops *src, int freesrc)
{
    struct MUSIC_SPC *spcMusic;

    SPC_exit();
    snes_spc = spc_new();
    spc_filter = spc_filter_new();
    if ( !snes_spc || !spc_filter )
    {
        Mix_SetError("SNES_SPC: Out of memory");
        return NULL;
    }
    /* Make sure the mikmod library is loaded */
    if ( snes_spc==NULL )
    {
        Mix_SetError("SNES_SPC: SPC Emulator is not loaded");
        return NULL;
    }

    spc_is_playing=-1;

    spcMusic = SnesSPC_LoadSongRW(src);
    if (!spcMusic)
    {
        Mix_SetError("SNES_SPC: Can't load file");
        return NULL;
    }

    /* Most SPC files have garbage data in the echo buffer, so clear that */
    spc_clear_echo( snes_spc );

    /* Clear filter before playing */
    spc_filter_clear( spc_filter );

    if ( freesrc ) {
        SDL_RWclose(src);
    }
    return spcMusic;
}

/* Start playback of a given MOD stream */
void SPC_play(struct MUSIC_SPC *music)
{
    if(music)
        music->playing=1;
    spc_is_playing=1;
}

/* Return non-zero if a stream is currently playing */
int SPC_playing(struct MUSIC_SPC *music)
{
    if(music)
        return music->playing;
    else
        return spc_is_playing;
}

short CosineInterpolate(
   short y1,short y2,
   float mu)
{
   float mu2;
   mu2 = (1 - cosf(mu*3.141592) )/2;
   return( (short)(y1*(1-mu2)+y2*mu2) );
}

short LinearInterpolate(
   short y1,short y2,
   float mu)
{
   return( (short)((float)y1*(1.f-mu) + (float)y2*mu));
}

/* Play some of a stream previously started with SPC_play() */
int SPC_playAudio(struct MUSIC_SPC *music, Uint8 *stream, int len)
{
    if(music==NULL) return 1;
    if(snes_spc==NULL) return 1;
    if(spc_is_playing==-1) return 1;

    short buf[len];
    Uint8 dstt[len];
    short left[len];
    short right[len];
    short left_new[len];
    short right_new[len];

    float ratio=(float)mixer.freq/(float)spc_sample_rate;
    int old_len2 = len/ratio;
    int old_len = old_len2/2;
    int old_len_c = old_len2/2;
    int new_len = len/2;

    spc_play( snes_spc, old_len, buf);
    spc_filter_run( spc_filter, buf, old_len );

    //Spliting channels
    int i,j;
    float p, q, r;
    for(i=0, j=0; i<old_len_c; i++, j+=2)
    {
        left[i] =buf[j];
        right[i]=buf[j+1];
    }

    //Resample left
    for(p=0.f, q=0.f, r=0.f; q<=old_len_c; q+=1.f )
    {
        int oldSam = (int)p;
        left_new[(int)q] = left[oldSam];
        //left_new[(int)q] = CosineInterpolate(last_left, left[oldSam], r);
        r+= 1/ratio;
        if(r>=1)
        {
            last_left = left[oldSam];
            p = roundf(q/ratio);
            r=0.f;
        }
    }
    last_left = left[(int)p];

    //Resample right
    for(p=0.f, q=0.f, r=0.f; q<=old_len_c; q+=1.f )
    {
        int oldSam = (int)p;
        right_new[(int)q] = right[oldSam];
        //right_new[(int)q] = CosineInterpolate(last_right, right[oldSam], r);
        r+= 1/ratio;
        if(r>=1)
        {
            last_right = right[oldSam];
            p = roundf(q/ratio);
            r=0.f;
        }
    }
    last_right = right[(int)p];

    //Merge stereo
    for(i=0, j=0; j<new_len; i+=4, j++)
    {
        dstt[i] = left_new[j]&0xff;
        dstt[i+1]= (left_new[j]>>8)&0xff;
        dstt[i+2] = right_new[j]&0xff;
        dstt[i+3] = (right_new[j]>>8)&0xff;
    }

    if ( music->volume == MIX_MAX_VOLUME )
    {
        SDL_memcpy(stream, &dstt, len);
    } else {
        SDL_MixAudio(stream, dstt, len, music->volume);
    }
    return 0;
}

/* Stop playback of a stream previously started with SPC_play() */
void SPC_stop(struct MUSIC_SPC *music)
{
    if(music)
        music->playing=-1;
    spc_is_playing=-1;
}

/* Close the given MOD stream */
void SPC_delete(struct MUSIC_SPC *music)
{
    if(music)
    {
        music->playing=-1;
        free(music);
    }
    SPC_exit();
}

/* Jump (seek) to a given position (time is in seconds) */
void SPC_jump_to_time(struct MUSIC_SPC *music, double time)
{
    if(music)
        time=time+0;
}

#endif
