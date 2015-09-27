#ifdef GME_MUSIC

/* This file supports Game Music Emulators music streams */

#include "SDL_mixer_ext.h"
#include "music_gme.h"

#include "gme/gme.h"
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

Music_Emu* game_emu=NULL;

int gme_is_playing=-1;
int gme_t_sample_rate=44100;

short last_left=0;
short last_right=0;

/* Initialize the Game Music Emulators player, with the given mixer settings
   This function returns 0, or -1 if there was an error.
 */

/* This is the format of the audio mixer data */
static SDL_AudioSpec mixer;

int GME_init(SDL_AudioSpec *mixerfmt)
{
    if(game_emu)
    {
        return 0;
    }

    gme_t_sample_rate = mixerfmt->freq;

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
void GME_exit(void)
{
    if (game_emu) {
        gme_delete( game_emu );
        game_emu=NULL;
    }
}

/* Set the volume for a MOD stream */
void GME_setvolume(struct MUSIC_GME *music, int volume)
{
    if(music)
    {
        music->volume=(int)round(128.0f*sqrt(((float)volume)*(1.f/128.f) ));
    }
}

struct MUSIC_GME *GME_LoadSongRW(SDL_RWops *src, int trackNum)
{
    if(src != NULL) {

        void *bytes=0;
        long spcsize;

        Sint64 length=0;
        length = SDL_RWseek(src, 0, RW_SEEK_END);
        if (length < 0)
        {
            Mix_SetError("GAME-EMU: wrong file\n");
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
            Mix_SetError("GAME-EMU: wrong file\n");
            return NULL;
        }

        char *err = (char*)gme_open_data( bytes, spcsize, &game_emu, gme_t_sample_rate );
        //spc_load_spc( snes_spc, bytes, spcsize );
        free(bytes);
        if(err!=0)
        {
            Mix_SetError("GAME-EMU: %s", err);
            return NULL;
        }

        err = (char*)gme_start_track( game_emu, trackNum );
        if(err!=0)
        {
            Mix_SetError("GAME-EMU: %s", err);
            return NULL;
        }

        struct MUSIC_GME *spcSpec = (struct MUSIC_GME*)malloc(sizeof(struct MUSIC_GME));
        spcSpec->playing=0;
        spcSpec->gme_t_sample_rate=mixer.freq;
        spcSpec->volume=MIX_MAX_VOLUME;
        spcSpec->mus_title=NULL;
        gme_info_t *musInfo;
        err = (char*)gme_track_info(game_emu, &musInfo, 0);
        if(err!=0)
        {
            Mix_SetError("GAME-EMU: %s", err);
            return NULL;
        }
        spcSpec->mus_title = (char *)SDL_malloc(sizeof(char)*strlen(musInfo->song)+1);
        strcpy(spcSpec->mus_title, musInfo->song);
        gme_free_info( musInfo );

        return spcSpec;
    }
    return NULL;
}

/* Load a Game Music Emulators stream from an SDL_RWops object */
struct MUSIC_GME *GME_new_RW(struct SDL_RWops *src, int freesrc, int trackNum)
{
    struct MUSIC_GME *gmeMusic;

    GME_exit();

    gme_is_playing=-1;

    gmeMusic = GME_LoadSongRW(src, trackNum);
    if (!gmeMusic)
    {
        Mix_SetError("GAME-EMU: Can't load file");
        return NULL;
    }
    if ( freesrc ) {
        SDL_RWclose(src);
    }
    return gmeMusic;
}

/* Start playback of a given Game Music Emulators stream */
void GME_play(struct MUSIC_GME *music)
{
    if(music)
        music->playing=1;
    gme_is_playing=1;
}

/* Return non-zero if a stream is currently playing */
int GME_playing(struct MUSIC_GME *music)
{
    if(music)
        return music->playing;
    else
        return gme_is_playing;
}

/* Play some of a stream previously started with GME_play() */
int GME_playAudio(struct MUSIC_GME *music, Uint8 *stream, int len)
{
    if(music==NULL) return 1;
    if(game_emu==NULL) return 1;
    if(gme_is_playing==-1) return 1;

    short buf[len];
    Uint8 dstt[len];
    int new_len = len/2;
    gme_play( game_emu, new_len, buf );

    //Spliting channels
    int i,j;
    //float p, q, r;
    for(i=0, j=0; i<len; i+=4, j+=2)
    {
        //left[i] =buf[j];
        //right[i]=buf[j+1];
        dstt[i] = buf[j]&0xff;
        dstt[i+1]= (buf[j]>>8)&0xff;
        dstt[i+2] = buf[j+1]&0xff;
        dstt[i+3] = (buf[j+1]>>8)&0xff;
    }

    if ( music->volume == MIX_MAX_VOLUME )
    {
        SDL_memcpy(stream, &dstt, len);
    } else {
        SDL_MixAudio(stream, dstt, len, music->volume);
    }
    return 0;
}

/* Stop playback of a stream previously started with GME_play() */
void GME_stop(struct MUSIC_GME *music)
{
    if(music)
        music->playing=-1;
    gme_is_playing=-1;
}

/* Close the given Game Music Emulators stream */
void GME_delete(struct MUSIC_GME *music)
{
    if( music->mus_title )
    {
        SDL_free(music->mus_title);
    }
    if(music)
    {
        music->playing=-1;
        free(music);
    }    
    GME_exit();
}

/* Jump (seek) to a given position (time is in seconds) */
void GME_jump_to_time(struct MUSIC_GME *music, double time)
{
    if(music)
    {
        gme_seek(game_emu, (int)round(time*1000));
    }
}

#endif
