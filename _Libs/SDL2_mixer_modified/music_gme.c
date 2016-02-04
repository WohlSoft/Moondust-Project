#ifdef GME_MUSIC

/* This file supports Game Music Emulators music streams */

#include "SDL_mixer_ext.h"
#include "music_gme.h"

#include "gme/gme.h"
#include "resample/sox_resample.h"

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

static int gme_t_sample_rate = 44100;

/* Initialize the Game Music Emulators player, with the given mixer settings
   This function returns 0, or -1 if there was an error.
 */

/* This is the format of the audio mixer data */
static SDL_AudioSpec mixer;

int GME_init(SDL_AudioSpec *mixerfmt)
{
    gme_t_sample_rate = mixerfmt->freq;

    current_output_channels = mixerfmt->channels;
    current_output_format = mixerfmt->format;

    mixer = *mixerfmt;

    return 0;
}

/* Uninitialize the music players */
void GME_exit(void) {}

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

        Music_Emu* game_emu;

        char *err = (char*)gme_open_data( bytes, spcsize, &game_emu, gme_t_sample_rate );
        //spc_load_spc( snes_spc, bytes, spcsize );
        free(bytes);
        if(err!=0)
        {
            Mix_SetError("GAME-EMU: %s", err);
            return NULL;
        }

        if((trackNum<0)||(trackNum >= gme_track_count(game_emu)))
            trackNum = gme_track_count(game_emu)-1;

        err = (char*)gme_start_track( game_emu, trackNum );
        if(err!=0)
        {
            Mix_SetError("GAME-EMU: %s", err);
            return NULL;
        }

        struct MUSIC_GME *spcSpec = (struct MUSIC_GME*)malloc(sizeof(struct MUSIC_GME));
        spcSpec->game_emu=game_emu;
        spcSpec->playing=0;
        spcSpec->gme_t_sample_rate=mixer.freq;
        spcSpec->volume=MIX_MAX_VOLUME;
        spcSpec->mus_title=NULL;
        spcSpec->mus_artist=NULL;
        spcSpec->mus_album=NULL;
        spcSpec->mus_copyright=NULL;
        gme_info_t *musInfo;
        err = (char*)gme_track_info(spcSpec->game_emu, &musInfo, trackNum);
        if(err!=0)
        {
            gme_delete(spcSpec->game_emu);
            free(spcSpec);
            Mix_SetError("GAME-EMU: %s", err);
            return NULL;
        }
        spcSpec->mus_title = (char *)SDL_malloc(sizeof(char)*strlen(musInfo->song)+1);
        strcpy(spcSpec->mus_title, musInfo->song);
        spcSpec->mus_artist = (char *)SDL_malloc(sizeof(char)*strlen(musInfo->author)+1);
        strcpy(spcSpec->mus_artist, musInfo->author);
        spcSpec->mus_album = (char *)SDL_malloc(sizeof(char)*strlen(musInfo->game)+1);
        strcpy(spcSpec->mus_album, musInfo->game);
        spcSpec->mus_copyright = (char *)SDL_malloc(sizeof(char)*strlen(musInfo->copyright)+1);
        strcpy(spcSpec->mus_copyright, musInfo->copyright);
        gme_free_info( musInfo );

        return spcSpec;
    }
    return NULL;
}

/* Load a Game Music Emulators stream from an SDL_RWops object */
struct MUSIC_GME *GME_new_RW(struct SDL_RWops *src, int freesrc, int trackNum)
{
    struct MUSIC_GME *gmeMusic;

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
}

/* Return non-zero if a stream is currently playing */
int GME_playing(struct MUSIC_GME *music)
{
    if(music)
        return music->playing;
    else
        return -1;
}

/* Play some of a stream previously started with GME_play() */
int GME_playAudio(struct MUSIC_GME *music, Uint8 *stream, int len)
{
    if(music==NULL) return 1;
    if(music->game_emu==NULL) return 1;
    if(music->playing==-1) return 1;

    short buf[len];
    gme_play( music->game_emu, len/2, buf );
    Uint8*src = (Uint8*)buf;

    if ( music->volume == MIX_MAX_VOLUME )
    {
        SDL_memcpy(stream, src, len);
    } else {
        SDL_MixAudio(stream, src, len, music->volume);
    }
    return 0;
}

/* Stop playback of a stream previously started with GME_play() */
void GME_stop(struct MUSIC_GME *music)
{
    if(music)
        music->playing=-1;
}

/* Close the given Game Music Emulators stream */
void GME_delete(struct MUSIC_GME *music)
{
    if(music)
    {
        if( music->mus_title )
        {
            SDL_free(music->mus_title);
        }
        if( music->mus_artist)
        {
            SDL_free(music->mus_artist);
        }
        if( music->mus_album)
        {
            SDL_free(music->mus_album);
        }
        if( music->mus_copyright)
        {
            SDL_free(music->mus_copyright);
        }
        if (music->game_emu)
        {
            gme_delete( music->game_emu );
            music->game_emu=NULL;
        }
        music->playing=-1;
        free(music);
    }
}

/* Jump (seek) to a given position (time is in seconds) */
void GME_jump_to_time(struct MUSIC_GME *music, double time)
{
    if(music)
    {
        gme_seek(music->game_emu, (int)round(time*1000));
    }
}

#endif
