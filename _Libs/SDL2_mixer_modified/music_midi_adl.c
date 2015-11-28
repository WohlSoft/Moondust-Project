
#ifdef USE_ADL_MIDI

/* This file supports Game Music Emulators music streams */

#include "SDL_mixer_ext.h"
#include "music_midi_adl.h"

#include "ADLMIDI/adlmidi.h"

#include <stdio.h>

/* Reference for converting mikmod output to 4/6 channels */
static int current_output_channels;
static Uint16 current_output_format;

static int music_swap8;
static int music_swap16;

struct ADL_MIDIPlayer* adl_midiplayer=NULL;

static int adlmidi_tremolo = 1;
static int adlmidi_vibrato = 1;
static int adlmidi_scalemod = 0;

static int adlmidi_bank = 58;

int adlmidi_is_playing=-1;
int adlmidi_t_sample_rate=44100;


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

int ADLMIDI_getScaleMod()
{
    return adlmidi_scalemod;
}
void ADLMIDI_setScaleMod(int sc)
{
    adlmidi_scalemod = sc;
}

/* Initialize the Game Music Emulators player, with the given mixer settings
   This function returns 0, or -1 if there was an error.
 */

/* This is the format of the audio mixer data */
static SDL_AudioSpec mixer;

int ADLMIDI_init(SDL_AudioSpec *mixerfmt)
{
    if(adl_midiplayer)
    {
        return 0;
    }

    adlmidi_t_sample_rate = mixerfmt->freq;

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
void ADLMIDI_exit(void)
{
    if (adl_midiplayer) {
        adl_close( adl_midiplayer );
        adl_midiplayer=NULL;
    }
}

/* Set the volume for a MOD stream */
void ADLMIDI_setvolume(struct MUSIC_MIDIADL *music, int volume)
{
    if(music)
    {
        music->volume=(int)round(128.0f*sqrt(((float)volume)*(1.f/128.f) ));
    }
}

struct MUSIC_MIDIADL *ADLMIDI_LoadSongRW(SDL_RWops *src)
{
    if(src != NULL) {

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

        adl_midiplayer= adl_init(adlmidi_t_sample_rate);

        adl_setHVibrato(adl_midiplayer, adlmidi_vibrato);
        adl_setHTremolo(adl_midiplayer, adlmidi_tremolo);
        adl_setBank(adl_midiplayer, adlmidi_bank);
        adl_setNumCards(adl_midiplayer, 4);
        adl_setScaleModulators(adl_midiplayer, adlmidi_scalemod);

        int err = adl_openData( adl_midiplayer, bytes, spcsize);
        free(bytes);
        if(err!=0)
        {
            Mix_SetError("ADL-MIDI: %s", adl_errorString());
            return NULL;
        }

        struct MUSIC_MIDIADL *adlMidi = (struct MUSIC_MIDIADL*)malloc(sizeof(struct MUSIC_MIDIADL));
        adlMidi->playing=0;
        adlMidi->gme_t_sample_rate=mixer.freq;
        adlMidi->volume=MIX_MAX_VOLUME;
        adlMidi->mus_title=NULL;

        return adlMidi;
    }
    return NULL;
}

/* Load a Game Music Emulators stream from an SDL_RWops object */
struct MUSIC_MIDIADL *ADLMIDI_new_RW(struct SDL_RWops *src, int freesrc)
{
    struct MUSIC_MIDIADL *adlmidiMusic;

    ADLMIDI_exit();

    adlmidi_is_playing=-1;

    adlmidiMusic = ADLMIDI_LoadSongRW(src);
    if (!adlmidiMusic)
    {
        Mix_SetError("ADL-MIDI: Can't load file");
        return NULL;
    }
    if ( freesrc ) {
        SDL_RWclose(src);
    }
    return adlmidiMusic;
}

/* Start playback of a given Game Music Emulators stream */
void ADLMIDI_play(struct MUSIC_MIDIADL *music)
{
    if(music)
        music->playing=1;
    adlmidi_is_playing=1;
}

/* Return non-zero if a stream is currently playing */
int ADLMIDI_playing(struct MUSIC_MIDIADL *music)
{
    if(music)
        return music->playing;
    else
        return adlmidi_is_playing;
}

/* Play some of a stream previously started with ADLMIDI_play() */
int ADLMIDI_playAudio(struct MUSIC_MIDIADL *music, Uint8 *stream, int len)
{
    if(music==NULL) return 0;
    if(adl_midiplayer==NULL) return 0;
    if(adlmidi_is_playing==-1) return 0;
    len -= len%2;//Only ODD sample requests
    if(len<0) return 0;

    int gottenLen=0;
    int buf[len];
    Uint8 dstt[len];
    int new_len = len/2;
    gottenLen=adl_play( adl_midiplayer, new_len, buf );
    if(gottenLen<=0) return 0;

    int i,j;
    for(i=0, j=0; i<(gottenLen*2); i+=4, j+=2)
    {
        dstt[i] = buf[j]&0xff;
        dstt[i+1]= (buf[j]>>8)&0xff;
        dstt[i+2] = buf[j+1]&0xff;
        dstt[i+3] = (buf[j+1]>>8)&0xff;
    }

    if ( music->volume == MIX_MAX_VOLUME )
    {
        SDL_memcpy(stream, &dstt, (gottenLen*2));
    } else {
        SDL_MixAudio(stream, dstt, (gottenLen*2), music->volume);
    }
    return len-(gottenLen*2);
}

/* Stop playback of a stream previously started with ADLMIDI_play() */
void ADLMIDI_stop(struct MUSIC_MIDIADL *music)
{
    if(music)
        music->playing=-1;
    adlmidi_is_playing=-1;
    adl_reset(adl_midiplayer);
}

/* Close the given Game Music Emulators stream */
void ADLMIDI_delete(struct MUSIC_MIDIADL *music)
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
    ADLMIDI_exit();
}

/* Jump (seek) to a given position (time is in seconds) */
void ADLMIDI_jump_to_time(struct MUSIC_MIDIADL *music, double time)
{
    if(music)
    {
        //gme_seek(adl_midiplayer, (int)round(time*1000));
    }
}

#endif
