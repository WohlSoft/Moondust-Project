#ifdef VGMSTREAM_MUSIC

/* This file supports vgmstream library music streams */

#include <SDL_mixer_ext/SDL_mixer_ext.h>
#include "music_vgmstream.h"

#include "vgmstream/vgmstream.h"
#include "resample/my_resample.h"

#include <stdio.h>

/*
 * Initialize the Game Music Emulators player, with the given mixer settings
 * This function returns 0, or -1 if there was an error.
 */

/* This is the format of the audio mixer data */
static SDL_AudioSpec mixer;

int VGMSTREAM_init(SDL_AudioSpec *mixerfmt)
{
    mixer = *mixerfmt;
    return 0;
}

/* Uninitialize the music players */
void VGMSTREAM_exit(void) {}

/* Set the volume for a MOD stream */
void VGMSTREAM_setvolume(struct MUSIC_VGMSTREAM *music, int volume)
{
    if(music)
    {
        music->volume=(int)round(128.0f*sqrt(((float)volume)*(1.f/128.f) ));
    }
}

struct MUSIC_VGMSTREAM *VGMSTREAM_LoadSongRW(SDL_RWops *src, int trackNum)
{
    if(src != NULL) {

        char *fileName=0;
        long spcsize;

        Sint64 length=0;
        length = SDL_RWseek(src, 0, RW_SEEK_END);
        if (length < 0)
        {
            Mix_SetError("VGMSTREAM: wrong file\n");
            return NULL;
        }

        SDL_RWseek(src, 0, RW_SEEK_SET);
        fileName = malloc(length);

        long bytes_l;
        unsigned char byte[1];
        spcsize=0;
        while( (bytes_l = SDL_RWread(src, &byte, sizeof(unsigned char), 1)) != 0 )
        {
            ((unsigned char*)fileName)[spcsize] = byte[0];
            spcsize++;
        }

        if (spcsize == 0)
        {
            Mix_SetError("VGMSTREAM: wrong file\n");
            return NULL;
        }

        VGMSTREAM* stream;

        //open_stdio_streamfile()

        char *err = (char*)gme_open_data( fileName, spcsize, &stream, mixer.freq );
        //spc_load_spc( snes_spc, bytes, spcsize );
        free(fileName);
        if(err!=0)
        {
            Mix_SetError("VGMSTREAM: %s", err);
            return NULL;
        }

        if((trackNum<0)||(trackNum >= gme_track_count(stream)))
            trackNum = gme_track_count(stream)-1;

        err = (char*)gme_start_track( stream, trackNum );
        if(err!=0)
        {
            Mix_SetError("VGMSTREAM: %s", err);
            return NULL;
        }

        struct MUSIC_VGMSTREAM *spcSpec = (struct MUSIC_VGMSTREAM*)malloc(sizeof(struct MUSIC_VGMSTREAM));
        spcSpec->stream = stream;
        spcSpec->playing = 0;
        spcSpec->sample_rate=mixer.freq;
        spcSpec->volume=MIX_MAX_VOLUME;
        spcSpec->mus_title=NULL;
        spcSpec->mus_artist=NULL;
        spcSpec->mus_album=NULL;
        spcSpec->mus_copyright=NULL;
        gme_info_t *musInfo;
        err = (char*)gme_track_info(spcSpec->stream, &musInfo, trackNum);
        if( err != 0 )
        {
            gme_delete(spcSpec->stream);
            free(spcSpec);
            Mix_SetError("VGMSTREAM: %s", err);
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

        SDL_BuildAudioCVT(&spcSpec->cvt, AUDIO_S16, 2,
                          mixer.freq,
                          mixer.format,
                          mixer.channels,
                          mixer.freq);

        return spcSpec;
    }
    return NULL;
}

/* Load a Game Music Emulators stream from an SDL_RWops object */
struct MUSIC_VGMSTREAM *VGMSTREAM_new_RW(struct SDL_RWops *src, int freesrc, int trackNum)
{
    struct MUSIC_VGMSTREAM *gmeMusic;

    gmeMusic = VGMSTREAM_LoadSongRW(src, trackNum);
    if (!gmeMusic)
    {
        Mix_SetError("VGMSTREAM: Can't load file");
        return NULL;
    }
    if ( freesrc ) {
        SDL_RWclose(src);
    }
    return gmeMusic;
}

/* Start playback of a given Game Music Emulators stream */
void VGMSTREAM_play(struct MUSIC_VGMSTREAM *music)
{
    if(music)
        music->playing=1;
}

/* Return non-zero if a stream is currently playing */
int VGMSTREAM_playing(struct MUSIC_VGMSTREAM *music)
{
    if(music)
        return music->playing;
    else
        return -1;
}

/* Play some of a stream previously started with VGMSTREAM_play() */
int VGMSTREAM_playAudio(struct MUSIC_VGMSTREAM *music, Uint8 *stream, int len)
{
    if(music==NULL) return 1;
    if(music->stream==NULL) return 1;
    if(music->playing==-1) return 1;
    if( len<0 ) return 0;
    int srgArraySize = len/music->cvt.len_ratio;
    short buf[srgArraySize];
    int srcLen = (int)((double)(len/2)/music->cvt.len_ratio);

    char *err = (char*)gme_play( music->stream, srcLen, buf );
    if( err != NULL)
    {
        Mix_SetError("VGMSTREAM: %s", err);
        return 0;
    }
    int dest_len = srcLen*2;

    if( music->cvt.needed ) {
        music->cvt.len = dest_len;
        music->cvt.buf = (Uint8*)buf;
        SDL_ConvertAudio(&music->cvt);
        dest_len = music->cvt.len_cvt;
    }

    if ( music->volume == MIX_MAX_VOLUME )
    {
        SDL_memcpy(stream, (Uint8*)buf, dest_len);
    } else {
        SDL_MixAudioFormat(stream, (Uint8*)buf, mixer.format, dest_len, music->volume);
    }
    return len-dest_len;
}

/* Stop playback of a stream previously started with VGMSTREAM_play() */
void VGMSTREAM_stop(struct MUSIC_VGMSTREAM *music)
{
    if(music)
        music->playing=-1;
}

/* Close the given Game Music Emulators stream */
void VGMSTREAM_delete(struct MUSIC_VGMSTREAM *music)
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
        if (music->stream)
        {
            gme_delete( music->stream );
            music->stream=NULL;
        }
        music->playing=-1;
        free(music);
    }
}

/* Jump (seek) to a given position (time is in seconds) */
void VGMSTREAM_jump_to_time(struct MUSIC_VGMSTREAM *music, double time)
{
    if(music)
    {
        gme_seek(music->stream, (int)round(time*1000));
    }
}

#endif
