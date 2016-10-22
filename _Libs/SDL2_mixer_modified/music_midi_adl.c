
#ifdef USE_ADL_MIDI

/* This file supports libADLMIDI music streams */

#include "SDL_mixer_ext.h"
#include "music_midi_adl.h"

#include "ADLMIDI/adlmidi.h"

#include <stdio.h>

/* Count of four-operator channnels per bank */
static const int tableOf_num4opChans[] =
{
    0,//0
    0,//1
    0,//2
    0,//3
    0,//4
    0,//5
    0,//6
    0,//7
    0,//8
    0,//9
    0,//10
    0,//11
    0,//12
    0,//13
    0,//14
    0,//15
    0,//16
    0,//17
    0,//18
    0,//19
    24,//20
    0,//21
    0,//22
    0,//23
    0,//24
    0,//25
    0,//26
    0,//27
    0,//28
    0,//29
    0,//30
    24,//31
    0,//32
    0,//33
    0,//34
    0,//35
    24,//36
    0,//37
    24,//38
    24,//39
    0,//40
    0,//41
    0,//42
    0,//43
    18,//44
    18,//45
    0,//46
    18,//47
    18,//48
    0,//49
    0,//50
    0,//51
    0,//52
    24,//53
    24,//54
    0,//55
    0,//56
    0,//57
    0,//58
    24,//59
    0,//60
    0,//61
    0,//62
    0,//63
    0,//64
    0,//65
    0,//66
    0,//67
    18,//68
    0,//69
    0,//70
    0,//71
};


/* Global ADLMIDI flags which are applying on initializing of MIDI player with a file */
static int adlmidi_bank         = 58;
static int adlmidi_tremolo      = 1;
static int adlmidi_vibrato      = 1;
static int adlmidi_scalemod     = 0;
static int adlmidi_adlibdrums   = 0;

int ADLMIDI_getBanksCount()
{
    return adl_getBanksCount();
}

const char * const*ADLMIDI_getBankNames()
{
    return adl_getBankNames();
}

int ADLMIDI_getBankID()
{
    return adlmidi_bank;
}

void ADLMIDI_setBankID(int bnk)
{
    adlmidi_bank = bnk;
}

int ADLMIDI_getTremolo()
{
    return adlmidi_tremolo;
}
void ADLMIDI_setTremolo(int tr)
{
    adlmidi_tremolo = tr;
}

int ADLMIDI_getVibrato()
{
    return adlmidi_vibrato;
}

void ADLMIDI_setVibrato(int vib)
{
    adlmidi_vibrato = vib;
}


int ADLMIDI_getAdLibDrumsMode()
{
    return adlmidi_adlibdrums;
}

void ADLMIDI_setAdLibDrumsMode(int ald)
{
    adlmidi_adlibdrums = ald;
}

int ADLMIDI_getScaleMod()
{
    return adlmidi_scalemod;
}

void ADLMIDI_setScaleMod(int sc)
{
    adlmidi_scalemod = sc;
}

void ADLMIDI_setDefaults()
{
    adlmidi_tremolo     = 1;
    adlmidi_vibrato     = 1;
    adlmidi_scalemod    = 0;
    adlmidi_adlibdrums  = 0;
    adlmidi_bank        = 58;
}

/*
 * Initialize the ADLMIDI player, with the given mixer settings
 * This function returns 0, or -1 if there was an error.
 */

/* This is the format of the audio mixer data */
static SDL_AudioSpec mixer;

int ADLMIDI_init(SDL_AudioSpec *mixerfmt)
{
    if(!mixerfmt)
    {
        Mix_SetError("ADL-MIDI: Null audio spec pointer\n");
        return -1;
    }
    mixer = *mixerfmt;
    return 0;
}

/* Uninitialize the music players */
void ADLMIDI_exit(void) {}

/* Set the volume for a ADLMIDI stream */
void ADLMIDI_setvolume(struct MUSIC_MIDIADL *music, int volume)
{
    if(music)
    {
        music->volume=(int)round(128.0f*sqrt(((float)volume)*(1.f/128.f) ));
    }
}

struct MUSIC_MIDIADL *ADLMIDI_LoadSongRW(SDL_RWops *src)
{
    if(src != NULL)
    {
        void *bytes=0;
        long spcsize;

        Sint64 length=0;
        length = SDL_RWseek(src, 0, RW_SEEK_END);
        if (length < 0)
        {
            Mix_SetError("ADL-MIDI: wrong file\n");
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
            Mix_SetError("ADL-MIDI: wrong file\n");
            return NULL;
        }

        struct ADL_MIDIPlayer* adl_midiplayer=NULL;
        adl_midiplayer = adl_init( mixer.freq );

        adl_setHVibrato( adl_midiplayer, adlmidi_vibrato );
        adl_setHTremolo( adl_midiplayer, adlmidi_tremolo );
        if( adl_setBank( adl_midiplayer, adlmidi_bank ) < 0 )
        {
            Mix_SetError("ADL-MIDI: %s", adl_errorString());
            adl_close( adl_midiplayer );
            return NULL;
        }

        adl_setScaleModulators( adl_midiplayer, adlmidi_scalemod );
        adl_setPercMode( adl_midiplayer, adlmidi_adlibdrums );
        adl_setNumFourOpsChn( adl_midiplayer, tableOf_num4opChans[adlmidi_bank] );
        adl_setNumCards( adl_midiplayer, 4 );

        int err = adl_openData( adl_midiplayer, bytes, spcsize );
        free(bytes);

        if(err != 0)
        {
            Mix_SetError("ADL-MIDI: %s", adl_errorString());
            return NULL;
        }

        struct MUSIC_MIDIADL *adlMidi   = (struct MUSIC_MIDIADL*)malloc(sizeof(struct MUSIC_MIDIADL));
        adlMidi->adlmidi                = adl_midiplayer;
        adlMidi->playing                = 0;
        adlMidi->gme_t_sample_rate      = mixer.freq;
        adlMidi->volume                 = MIX_MAX_VOLUME;
        adlMidi->mus_title              = NULL;
        SDL_BuildAudioCVT(&adlMidi->cvt, AUDIO_S16, 2, mixer.freq, mixer.format, mixer.channels, mixer.freq);

        return adlMidi;
    }
    return NULL;
}

/* Load ADLMIDI stream from an SDL_RWops object */
struct MUSIC_MIDIADL *ADLMIDI_new_RW(struct SDL_RWops *src, int freesrc)
{
    struct MUSIC_MIDIADL *adlmidiMusic;

    ADLMIDI_exit();

    adlmidiMusic = ADLMIDI_LoadSongRW(src);
    if (!adlmidiMusic)
    {
        Mix_SetError("ADL-MIDI: Can't load file: %s", adl_errorString());
        return NULL;
    }
    if( freesrc )
        SDL_RWclose(src);

    return adlmidiMusic;
}

/* Start playback of a given Game Music Emulators stream */
void ADLMIDI_play(struct MUSIC_MIDIADL *music)
{
    if(music)
        music->playing=1;
}

/* Return non-zero if a stream is currently playing */
int ADLMIDI_playing(struct MUSIC_MIDIADL *music)
{
    if(music)
        return music->playing;
    else
        return -1;
}

/* Play some of a stream previously started with ADLMIDI_play() */
int ADLMIDI_playAudio(struct MUSIC_MIDIADL *music, Uint8 *stream, int len)
{
    if( music==NULL ) return 0;
    if( music->adlmidi == NULL ) return 0;
    if( music->playing == -1 ) return 0;
    if( len < 0 ) return 0;
    int     srgArraySize = (int)ceil( (double)len / music->cvt.len_ratio );
    short   buf[srgArraySize];
    int srcLen = (int)((double)(len/2.0)/music->cvt.len_ratio);

    int gottenLen = adl_play( music->adlmidi, srcLen, buf );
    if( gottenLen <= 0 )
    {
        return 0;
    }

    int dest_len = gottenLen*2;

    if( music->cvt.needed )
    {
        music->cvt.len = dest_len;
        music->cvt.buf = (Uint8*)buf;
        SDL_ConvertAudio(&music->cvt);
        dest_len = music->cvt.len_cvt;
    }

    if( music->volume == MIX_MAX_VOLUME )
    {
        SDL_memcpy( stream, (Uint8*)buf, (size_t)dest_len );
    } else {
        SDL_MixAudioFormat( stream, (Uint8*)buf, mixer.format, (Uint32)dest_len, music->volume );
    }
    return len-dest_len;
}

/* Stop playback of a stream previously started with ADLMIDI_play() */
void ADLMIDI_stop(struct MUSIC_MIDIADL *music)
{
    if(music)
    {
        music->playing=-1;
        adl_reset(music->adlmidi);
    }
}

/* Close the given Game Music Emulators stream */
void ADLMIDI_delete(struct MUSIC_MIDIADL *music)
{
    if(music)
    {
        if( music->mus_title )
        {
            SDL_free(music->mus_title);
        }
        music->playing=-1;
        if(music->adlmidi)
            adl_close( music->adlmidi );
        music->adlmidi = NULL;
        free( music );
    }
    ADLMIDI_exit();
}

/* Jump (seek) to a given position (time is in seconds) */
void ADLMIDI_jump_to_time(struct MUSIC_MIDIADL *music, double time)
{
    (void)time;
    if( music )
    {
        //gme_seek(adl_midiplayer, (int)round(time*1000));
    }
}

#endif
