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

#ifdef GME_MUSIC

/* This file supports Game Music Emulators music streams */

#include "SDL_mixer_ext.h"
#include "music_gme.h"

#include "gme/gme.h"
#include "resample/my_resample.h"

#include <stdio.h>

#define SDL_SURROUND
#ifdef SDL_SURROUND
#define MAX_OUTPUT_CHANNELS 6
#else
#define MAX_OUTPUT_CHANNELS 2
#endif

/*
 * Initialize the Game Music Emulators player, with the given mixer settings
 * This function returns 0, or -1 if there was an error.
 */

/* This is the format of the audio mixer data */
static SDL_AudioSpec mixer;

int GME_init(SDL_AudioSpec *mixerfmt)
{
    mixer = *mixerfmt;
    return 0;
}

/* Uninitialize the music players */
void GME_exit(void) {}

/* Set the volume for a MOD stream */
void GME_setvolume(struct MUSIC_GME *music, int volume)
{
    if(music)
        music->volume = (int)round(128.0f * sqrt(((float)volume) * (1.f / 128.f)));
}

struct MUSIC_GME *GME_LoadSongRW(SDL_RWops *src, int trackNum)
{
    if(src != NULL)
    {
        void *bytes = 0;
        long spcsize, bytes_l;
        unsigned char byte[1];
        Music_Emu *game_emu;
        gme_info_t *musInfo;
        struct MUSIC_GME *spcSpec;
        char *err;

        Sint64 length = 0;

        length = SDL_RWseek(src, 0, RW_SEEK_END);
        if(length < 0)
        {
            Mix_SetError("GAME-EMU: wrong file\n");
            return NULL;
        }

        SDL_RWseek(src, 0, RW_SEEK_SET);
        bytes = SDL_malloc(length);

        spcsize = 0;
        while((bytes_l = SDL_RWread(src, &byte, sizeof(unsigned char), 1)) != 0)
        {
            ((unsigned char *)bytes)[spcsize] = byte[0];
            spcsize++;
        }

        if(spcsize == 0)
        {
            Mix_SetError("GAME-EMU: wrong file\n");
            return NULL;
        }

        err = (char *)gme_open_data(bytes, spcsize, &game_emu, mixer.freq);
        /* spc_load_spc( snes_spc, bytes, spcsize ); */
        SDL_free(bytes);
        if(err != 0)
        {
            Mix_SetError("GAME-EMU: %s", err);
            return NULL;
        }

        if((trackNum < 0) || (trackNum >= gme_track_count(game_emu)))
            trackNum = gme_track_count(game_emu) - 1;

        err = (char *)gme_start_track(game_emu, trackNum);
        if(err != 0)
        {
            Mix_SetError("GAME-EMU: %s", err);
            return NULL;
        }

        spcSpec = (struct MUSIC_GME *)SDL_malloc(sizeof(struct MUSIC_GME));
        spcSpec->game_emu = game_emu;
        spcSpec->playing = 0;
        spcSpec->gme_t_sample_rate = mixer.freq;
        spcSpec->volume = MIX_MAX_VOLUME;
        spcSpec->mus_title = NULL;
        spcSpec->mus_artist = NULL;
        spcSpec->mus_album = NULL;
        spcSpec->mus_copyright = NULL;

        err = (char *)gme_track_info(spcSpec->game_emu, &musInfo, trackNum);
        if(err != 0)
        {
            gme_delete(spcSpec->game_emu);
            SDL_free(spcSpec);
            Mix_SetError("GAME-EMU: %s", err);
            return NULL;
        }
        spcSpec->mus_title = (char *)SDL_malloc(sizeof(char) * strlen(musInfo->song) + 1);
        strcpy(spcSpec->mus_title, musInfo->song);
        spcSpec->mus_artist = (char *)SDL_malloc(sizeof(char) * strlen(musInfo->author) + 1);
        strcpy(spcSpec->mus_artist, musInfo->author);
        spcSpec->mus_album = (char *)SDL_malloc(sizeof(char) * strlen(musInfo->game) + 1);
        strcpy(spcSpec->mus_album, musInfo->game);
        spcSpec->mus_copyright = (char *)SDL_malloc(sizeof(char) * strlen(musInfo->copyright) + 1);
        strcpy(spcSpec->mus_copyright, musInfo->copyright);
        gme_free_info(musInfo);

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
struct MUSIC_GME *GME_new_RW(struct SDL_RWops *src, int freesrc, int trackNum)
{
    struct MUSIC_GME *gmeMusic;

    gmeMusic = GME_LoadSongRW(src, trackNum);
    if(!gmeMusic)
    {
        Mix_SetError("GAME-EMU: Can't load file");
        return NULL;
    }
    if(freesrc)
        SDL_RWclose(src);
    return gmeMusic;
}

/* Start playback of a given Game Music Emulators stream */
void GME_play(struct MUSIC_GME *music)
{
    if(music)
        music->playing = 1;
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
    int dest_len;
    int srgArraySize;
    int srcLen;
    short *buf = NULL;
    char *err = NULL;

    if(music == NULL)
        return 1;
    if(music->game_emu == NULL)
        return 1;
    if(music->playing == -1)
        return 1;
    if(len < 0)
        return 0;

    srgArraySize = len * music->cvt.len_mult;
    buf = (short *)SDL_malloc((size_t)srgArraySize);
    srcLen = (int)((double)(len / 2) / music->cvt.len_ratio);

    err = (char *)gme_play(music->game_emu, srcLen, buf);
    if(err != NULL)
    {
        Mix_SetError("GAME-EMU: %s", err);
        SDL_free(buf);
        return 0;
    }

    dest_len = srcLen * 2;

    if(music->cvt.needed)
    {
        music->cvt.len = dest_len;
        music->cvt.buf = (Uint8 *)buf;
        SDL_ConvertAudio(&music->cvt);
        dest_len = music->cvt.len_cvt;
    }

    if(music->volume == MIX_MAX_VOLUME)
        SDL_memcpy(stream, (Uint8 *)buf, dest_len);
    else
        SDL_MixAudioFormat(stream, (Uint8 *)buf, mixer.format, dest_len, music->volume);

    SDL_free(buf);
    return len - dest_len;
}

/* Stop playback of a stream previously started with GME_play() */
void GME_stop(struct MUSIC_GME *music)
{
    if(music)
        music->playing = -1;
}

/* Close the given Game Music Emulators stream */
void GME_delete(struct MUSIC_GME *music)
{
    if(music)
    {
        if(music->mus_title)
            SDL_free(music->mus_title);
        if(music->mus_artist)
            SDL_free(music->mus_artist);
        if(music->mus_album)
            SDL_free(music->mus_album);
        if(music->mus_copyright)
            SDL_free(music->mus_copyright);
        if(music->game_emu)
        {
            gme_delete(music->game_emu);
            music->game_emu = NULL;
        }
        music->playing = -1;
        SDL_free(music);
    }
}

/* Jump (seek) to a given position (time is in seconds) */
void GME_jump_to_time(struct MUSIC_GME *music, double time)
{
    if(music)
        gme_seek(music->game_emu, (int)round(time * 1000));
}

#endif
