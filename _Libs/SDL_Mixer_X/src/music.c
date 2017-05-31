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

/* $Id$ */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <SDL2/SDL_endian.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_timer.h>

#include <SDL_mixer_ext/SDL_mixer_ext.h>
#include "mixer.h"

#ifdef CMD_MUSIC
#include "music_cmd.h"
#endif
#ifdef WAV_MUSIC
#include "wavestream.h"
#endif
#ifdef MODPLUG_MUSIC
#include "music_modplug.h"
#endif
#ifdef MOD_MUSIC
#include "music_mod.h"
#endif
#ifdef MID_MUSIC
#  ifdef USE_ADL_MIDI
#    include "music_midi_adl.h"
#  endif
#  ifdef USE_OPN2_MIDI
#    include "music_midi_opn.h"
#  endif
#  ifdef USE_TIMIDITY_MIDI
#    include "timidity/timidity.h"
#  endif
#  ifdef USE_FLUIDSYNTH_MIDI
#    include "music_fluidsynth.h"
#  endif
#  ifdef USE_NATIVE_MIDI
#    include "native_midi/native_midi.h"
#  endif
#endif
#ifdef OGG_MUSIC
#include "music_ogg.h"
#endif
#ifdef MP3_MUSIC
#include "dynamic_mp3.h"
#endif
#ifdef MP3_MAD_MUSIC
#include "music_mad.h"
#endif
#ifdef FLAC_MUSIC
#include "music_flac.h"
#if defined(_WIN32)
#ifdef _MSC_VER
typedef long _off_t;
#endif
typedef _off_t off_t;
#endif
#endif
#ifdef GME_MUSIC
#include "music_gme.h"
#endif

/* DEPRECATED
#if defined(MP3_MUSIC) || defined(MP3_MAD_MUSIC)
static SDL_AudioSpec used_mixer;
#endif
*/

int volatile music_active = 1;
static int volatile music_stopped = 0;
static int music_loops = 0;
static char *music_cmd = NULL;
static char *music_file = NULL;
static char *music_args = NULL;
static char *music_filename = NULL;
static Mix_Music *volatile music_playing = NULL;
static int music_volume = MIX_MAX_VOLUME;

static int mididevice_next    = MIDI_ADLMIDI;
static int mididevice_current = MIDI_ADLMIDI;

static AudioCodec   available_codecs[MUS_KnownCodecs];

/* Reset MIDI settings every file reopening (to allow right argument passing) */
static int need_reset_midi = 1;

/* Denies MIDI arguments */
static int lock_midi_args = 0;

struct _Mix_Music
{
    AudioCodec          codec;
    AudioCodecStream*   music;
    Mix_MusicType       type;
    union
    {
        #ifdef CMD_MUSIC
        MusicCMD *cmd;
        #endif

        #ifdef WAV_MUSIC
        WAVStream *wave;
        #endif

        #ifdef MOD_MUSIC
        struct MODULE *module;
        #endif

        #ifdef MID_MUSIC
            #ifdef USE_TIMIDITY_MIDI
            MidiSong *midi;
            #endif

            #ifdef USE_ADL_MIDI
            struct MUSIC_MIDIADL *midi_adl;
            #endif

            #ifdef USE_OPN2_MIDI
            struct MUSIC_MIDIOPN *midi_opn;
            #endif

            #ifdef USE_FLUIDSYNTH_MIDI
            FluidSynthMidiSong *fluidsynthmidi;
            #endif

            #ifdef USE_NATIVE_MIDI
            NativeMidiSong *nativemidi;
            #endif
        #endif/*MID_MUSIC*/

        #ifdef MP3_MUSIC
        SMPEG *mp3;
        #endif
    } data;
    Mix_Fading fading;
    int fade_step;
    int fade_steps;
    int error;
};
#ifdef MID_MUSIC
#ifdef USE_TIMIDITY_MIDI
static int timidity_ok;
static int samplesize;
#endif
#ifdef USE_ADL_MIDI
static int adl_midi_ok;
#endif
#ifdef USE_OPN2_MIDI
static int opn2_midi_ok;
#endif
#ifdef USE_FLUIDSYNTH_MIDI
static int fluidsynth_ok;
#endif
#ifdef USE_NATIVE_MIDI
static int native_midi_ok;
#endif
#endif

/* Used to calculate fading steps */
static int ms_per_step;

/* rcg06042009 report available decoders at runtime. */
static const char **music_decoders = NULL;
static int num_decoders = 0;

/* Semicolon-separated SoundFont paths */
#ifdef MID_MUSIC
char *soundfont_paths = NULL;
#endif

int SDLCALLCC Mix_GetNumMusicDecoders(void)
{
    return(num_decoders);
}

const char *SDLCALLCC Mix_GetMusicDecoder(int index)
{
    if((index < 0) || (index >= num_decoders))
        return NULL;
    return(music_decoders[index]);
}

static void add_music_decoder(const char *decoder)
{
    void *ptr = SDL_realloc((void *)music_decoders, (num_decoders + 1) * sizeof(const char *));
    if(ptr == NULL)
    {
        return;  /* oh well, go on without it. */
    }
    music_decoders = (const char **) ptr;
    music_decoders[num_decoders++] = decoder;
}

/* Local low-level functions prototypes */
static void music_internal_initialize_volume(void);
static void music_internal_volume(int volume);
static int  music_internal_play(Mix_Music *music, double position);
static int  music_internal_position(double position);
static int  music_internal_playing();
static void music_internal_halt(void);


/* Support for hooking when the music has finished */
static void (*music_finished_hook)(void) = NULL;

void SDLCALLCC Mix_HookMusicFinished(void (*music_finished)(void))
{
    Mix_LockAudio();
    music_finished_hook = music_finished;
    Mix_UnlockAudio();
}


/* If music isn't playing, halt it if no looping is required, restart it */
/* othesrchise. NOP if the music is playing */
static int music_halt_or_loop(void)
{
    /* Restart music if it has to loop */

    if(!music_internal_playing())
    {
        #ifdef USE_NATIVE_MIDI
        /* Native MIDI handles looping internally */
        if(mididevice_current == MIDI_Native && music_playing->type == MUS_MID && native_midi_ok)
            music_loops = 0;
        #endif

        /* Restart music if it has to loop at a high level */
        if(music_loops)
        {
            Mix_Fading current_fade;
            if(music_loops > 0)
                --music_loops;
            current_fade = music_playing->fading;
            music_internal_play(music_playing, 0.0);
            music_playing->fading = current_fade;
        }
        else
        {
            music_internal_halt();
            if(music_finished_hook)
                music_finished_hook();

            return 0;
        }
    }

    return 1;
}



/* Mixing function */
void music_mixer(void *udata, Uint8 *stream, int len)
{
    int left = 0;

    if(music_playing && music_active)
    {
        /* Handle fading */
        if(music_playing->fading != MIX_NO_FADING)
        {
            if(music_playing->fade_step++ < music_playing->fade_steps)
            {
                int volume;
                int fade_step = music_playing->fade_step;
                int fade_steps = music_playing->fade_steps;

                if(music_playing->fading == MIX_FADING_OUT)
                    volume = (music_volume * (fade_steps - fade_step)) / fade_steps;
                else   /* Fading in */
                    volume = (music_volume * fade_step) / fade_steps;
                music_internal_volume(volume);
            }
            else
            {
                if(music_playing->fading == MIX_FADING_OUT)
                {
                    music_internal_halt();
                    if(music_finished_hook)
                        music_finished_hook();
                    return;
                }
                music_playing->fading = MIX_NO_FADING;
            }
        }

        music_halt_or_loop();
        if(!music_internal_playing())
            return;

        switch(music_playing->type)
        {
            #ifdef CMD_MUSIC
        case MUS_CMD:
            /* The playing is done externally */
            break;
            #endif

            #ifdef WAV_MUSIC
        case MUS_WAV:
            left = WAVStream_PlaySome(music_playing->data.wave, stream, len);
            break;
            #endif

            #ifdef MOD_MUSIC
        case MUS_MOD:
            left = MOD_playAudio(music_playing->data.module, stream, len);
            break;
            #endif

            #ifdef MID_MUSIC
        case MUS_MID:
            switch(mididevice_current)
            {
                #ifdef USE_NATIVE_MIDI
            case MIDI_Native:
                if(native_midi_ok)
                {
                    /* Native midi is handled asynchronously */
                    goto skip;
                }
                break;
                #endif

                #ifdef USE_FLUIDSYNTH_MIDI
            case MIDI_Fluidsynth:
                if(fluidsynth_ok)
                {
                    fluidsynth_playsome(music_playing->data.fluidsynthmidi, stream, len);
                    goto skip;
                }
                break;
                #endif

                #ifdef USE_ADL_MIDI
            case MIDI_ADLMIDI:
                if(adl_midi_ok)
                {
                    left = ADLMIDI_playAudio(music_playing->data.midi_adl, stream, len);
                    goto skip;
                }
                break;
                #endif

                #ifdef USE_OPN2_MIDI
            case MIDI_OPNMIDI:
                if(opn2_midi_ok)
                {
                    left = OPNMIDI_playAudio(music_playing->data.midi_opn, stream, len);
                    goto skip;
                }
                break;
                #endif

                #ifdef USE_TIMIDITY_MIDI
            case MIDI_Timidity:
                if(timidity_ok)
                {
                    int samples = len / samplesize;
                    Timidity_PlaySome(stream, samples);
                    goto skip;
                }
                break;
                #endif
            }
            break;
            #endif

            #if defined(OGG_MUSIC)||defined(FLAC_MUSIC)||defined(MP3_MAD_MUSIC)||defined(MODPLUG_MUSIC)||defined(GME_MUSIC)
        case MUS_OGG:
        case MUS_FLAC:
        case MUS_MP3_MAD:
        case MUS_MODPLUG:
        case MUS_SPC:
            left = music_playing->codec.playAudio(music_playing->music, stream, len);
            break;
            #endif

            #ifdef MP3_MUSIC
        case MUS_MP3:
            left = (len - smpeg.SMPEG_playAudio(music_playing->data.mp3, stream, len));
            break;
            #endif

        default:
            /* Unknown music type?? */
            break;
        }
    }

skip:
    /* Handle seamless music looping */
    if(left > 0 && left < len)
    {
        music_halt_or_loop();
        if(music_internal_playing())
            music_mixer(udata, stream + (len - left), left);
    }
}

/* Initialize the music players with a certain desired audio format */
int open_music(SDL_AudioSpec *mixer)
{
    SDL_memset(available_codecs, 0, sizeof(available_codecs));

    /* available_codecs */
    #ifdef WAV_MUSIC
    if(WAVStream_Init(mixer) == 0)
        add_music_decoder("WAVE");
    #endif

    #ifdef MODPLUG_MUSIC
    if(modplug_init2(&available_codecs[MUS_MODPLUG], mixer) == 0)
        add_music_decoder("MODPLUG");
    #endif

    #ifdef MOD_MUSIC
    if(MOD_init(mixer) == 0)
        add_music_decoder("MIKMOD");
    #endif

    #ifdef GME_MUSIC
    if(GME_init2(&available_codecs[MUS_SPC], mixer) == 0)
        add_music_decoder("GAMEEMU");
    #endif

    #ifdef MID_MUSIC

    #ifdef USE_ADL_MIDI
    if(ADLMIDI_init(mixer) == 0)
    {
        adl_midi_ok = 1;
        add_music_decoder("ADLMIDI");
    }
    #endif

    #ifdef USE_OPN2_MIDI
    if(OPNMIDI_init(mixer) == 0)
    {
        opn2_midi_ok = 1;
        add_music_decoder("OPNMIDI");
    }
    #endif

    #ifdef USE_TIMIDITY_MIDI
    samplesize = mixer->size / mixer->samples;
    if(Timidity_Init(mixer->freq, mixer->format,
                     mixer->channels, mixer->samples) == 0)
    {
        timidity_ok = 1;
        add_music_decoder("TIMIDITY");
    }
    else
        timidity_ok = 0;
    #endif

    #ifdef USE_FLUIDSYNTH_MIDI
    if(fluidsynth_init(mixer) == 0)
    {
        fluidsynth_ok = 1;
        add_music_decoder("FLUIDSYNTH");
    }
    else
        fluidsynth_ok = 0;
    #endif

    #ifdef USE_NATIVE_MIDI
    /* Don't block Native MIDI usage if other MIDI devices are fine to allows dynamic toggling of midi-devices */
    /*
    #ifdef USE_FLUIDSYNTH_MIDI
       native_midi_ok = !fluidsynth_ok;
       if ( native_midi_ok )
    #endif

    #ifdef USE_ADL_MIDI
           native_midi_ok = !adl_midi_ok;
       if ( native_midi_ok )
    #endif

    #ifdef USE_TIMIDITY_MIDI
           native_midi_ok = !timidity_ok;
       if ( !native_midi_ok ) {
           native_midi_ok = (getenv("SDL_NATIVE_MUSIC") != NULL);
       }
       if ( native_midi_ok )
    #endif
    */
    native_midi_ok = native_midi_detect();
    if(native_midi_ok)
        add_music_decoder("NATIVEMIDI");
    #endif

    #endif
    #ifdef OGG_MUSIC
    OGG_init2(&available_codecs[MUS_OGG], mixer);
    add_music_decoder("OGG");
    #endif

    #ifdef FLAC_MUSIC
    FLAC_init2(&available_codecs[MUS_FLAC], mixer);
    add_music_decoder("FLAC");
    #endif

    #if defined(MP3_MUSIC) || defined(MP3_MAD_MUSIC)
    #ifdef MP3_MAD_MUSIC
    MAD_init2(&available_codecs[MUS_MP3_MAD], mixer);
    #endif
    add_music_decoder("MP3");
    #endif

    music_playing = NULL;
    music_stopped = 0;
    Mix_VolumeMusic(SDL_MIX_MAXVOLUME);

    /* Calculate the number of ms for each callback */
    ms_per_step = (int)(((double)mixer->samples * 1000.0) / mixer->freq);

    return(0);
}

/* Portable case-insensitive string compare function */
int MIX_string_equals(const char *str1, const char *str2)
{
    while(*str1 && *str2)
    {
        if(toupper((unsigned char)*str1) !=
           toupper((unsigned char)*str2))
            break;
        ++str1;
        ++str2;
    }
    return (!*str1 && !*str2);
}

static int detect_imf(SDL_RWops *in, Sint64 start)
{
    Uint16 chunksize, buff, i = 42;
    Uint32 sum1 = 0,  sum2 = 0;
    Uint16 word;

    if(!in)
        return 0;

    SDL_RWseek(in, start, RW_SEEK_SET);

    if(SDL_RWread(in, &word, 1, 2) != 2)
    {
        SDL_RWseek(in, start, RW_SEEK_SET);
        return 0;
    }
    chunksize = SDL_SwapLE16(word);
    if((chunksize == 0) || (chunksize & 3))
    {
        SDL_RWseek(in, start, RW_SEEK_SET);
        return 0;
    }

    while(i > 0)
    {
        if(SDL_RWread(in, &word, 1, 2) != 2)
        {
            SDL_RWseek(in, start, RW_SEEK_SET);
            break;
        }
        buff = SDL_SwapLE16(word);
        sum1 += buff;
        if(SDL_RWread(in, &word, 1, 2) != 2)
        {
            SDL_RWseek(in, start, RW_SEEK_SET);
            break;
        }
        buff = SDL_SwapLE16(word);
        sum2 += buff;
        i--;
    }
    SDL_RWseek(in, start, RW_SEEK_SET);
    return (sum1 > sum2);
}

#if defined(MP3_MUSIC) || defined(MP3_MAD_MUSIC)
int SDLMixerX_detect_mp3(Uint8 *magic, SDL_RWops *src, Sint64 start)
{
    /* if first 4 bytes are zeros */
    Uint32 null = 0;
    Uint8  magic2[5];
    unsigned char byte = 0;
    /* Sint64 end = 0; */
    Sint64 notNullPos = 0;

    memcpy(magic2, magic, 5);

    if(strncmp((char *)magic2, "ID3", 3) == 0)
    {
        SDL_RWseek(src, start, RW_SEEK_SET);
        return 1;
    }

    SDL_RWseek(src, start, RW_SEEK_SET);

    /* If first bytes are zero */
    if(memcmp(magic2, &null, 4) != 0)
        goto readHeader;

digMoreBytes:
    {
        /* Find nearest non zero */
        /* Look for FF byte */
        while((SDL_RWread(src, &byte, 1, 1) == 1) &&
              (byte != 0xFF) &&
              (SDL_RWtell(src) < (start + 10240)))
        {}

        /* with offset -1 byte */
        notNullPos = SDL_RWtell(src) - 1;
        SDL_RWseek(src, notNullPos, RW_SEEK_SET);

        /* If file contains only null bytes */
        if(byte != 0xFF)
        {
            /* printf("WRONG BYTE\n"); */
            SDL_RWseek(src, start, RW_SEEK_SET);
            return 0;
        }
        if(SDL_RWread(src, magic2, 1, 4) != 4)
        {
            /* printf("MAGIC WRONG\n"); */
            SDL_RWseek(src, start, RW_SEEK_SET);
            return 0;
        }
    }

readHeader:
    /* Detection code lifted from SMPEG */
    if(
        ((magic2[0] & 0xff) != 0xff) || ((magic2[1] & 0xf0) != 0xf0) || /*  No sync bits */
        ((magic2[2] & 0xf0) == 0x00) || /*  Bitrate is 0 */
        ((magic2[2] & 0xf0) == 0xf0) || /*  Bitrate is 15 */
        ((magic2[2] & 0x0c) == 0x0c) || /*  Frequency is 3 */
        ((magic2[1] & 0x06) == 0x00)   /*  Layer is 4 */
    )
    {
        /* printf("WRONG BITS\n"); */
        goto digMoreBytes;
    }
    SDL_RWseek(src, start, RW_SEEK_SET);
    return 1;
}
#endif


/* MUS_MOD can't be auto-detected. If no other format was detected, MOD is
 * assumed and MUS_MOD will be returned, meaning that the format might not
 * actually be MOD-based.
 *
 * Returns MUS_NONE in case of errors. */
static Mix_MusicType detect_music_type(SDL_RWops *src)
{
    Uint8 magic[5];
    Uint8 moremagic[9];
    Uint8 extramagic[25];
    Uint8 lessmagic[3];

    Sint64 start = SDL_RWtell(src);
    if(SDL_RWread(src, extramagic, 1, 24) != 24)
    {
        Mix_SetError("Couldn't read from RWops");
        return MUS_NONE;
    }
    SDL_RWseek(src, start, RW_SEEK_SET);
    memcpy(lessmagic, extramagic,   2);
    memcpy(magic,     extramagic,   4);
    memcpy(moremagic, extramagic + 4, 8);
    lessmagic[2]  = '\0';
    magic[4]      = '\0';
    moremagic[8]  = '\0';
    extramagic[24] = '\0';

    /* Drop out some known but not supported file types (Archives, etc.) */
    if(strncmp((char *)magic, "PK\x03\x04", 3) == 0)
        return MUS_NONE;
    if(strncmp((char *)extramagic, "\x37\x7A\xBC\xAF\x27\x1C", 6) == 0)
        return MUS_NONE;

    /* WAVE files have the magic four bytes "RIFF"
       AIFF files have the magic 12 bytes "FORM" XXXX "AIFF" */
    if( ((strncmp((char *)magic, "RIFF", 4) == 0) && (strncmp((char *)(moremagic + 4), "WAVE", 4) == 0))
        || ((strncmp((char *)magic, "FORM", 4) == 0) && (strncmp(moremagic + 4, "XDIR", 4) != 0)) ) /*Don't parse XMIDI as AIFF file*/
        return MUS_WAV;

    /* Ogg Vorbis files have the magic four bytes "OggS" */
    if(strncmp((char *)magic, "OggS", 4) == 0)
        return MUS_OGG;

    /* FLAC files have the magic four bytes "fLaC" */
    if(strncmp((char *)magic, "fLaC", 4) == 0)
        return MUS_FLAC;

    /* MIDI files have the magic four bytes "MThd" */
    if(strncmp((char *)magic, "MThd", 4) == 0)
        return MUS_MID;

    #ifdef USE_ADL_MIDI
    if(strncmp((char *)magic, "MUS\x1A", 4) == 0)
        return MUS_ADLMIDI;
    if( (memcmp(extramagic, "FORM", 4) == 0) &&
        (memcmp(extramagic + 8, "XDIR", 4) == 0) )
        return MUS_ADLMIDI;
    if(strncmp((char *)magic, "GMF\x1", 4) == 0)
        return MUS_ADLMIDI;
    if(strncmp((char *)magic, "CTMF", 4) == 0)
        return MUS_ADLMIDI;
    #endif

    /* MIDI files have the magic four bytes "RIFF" */
    if((strncmp((char *)magic, "RIFF", 4) == 0) && (strncmp((char *)(moremagic + 4), "RMID", 4) == 0))
        return MUS_MID;

    /* GME Specific files */
    if(strncmp((char *)magic, "ZXAY", 4) == 0)
        return MUS_SPC;
    if(strncmp((char *)magic, "GBS\x01", 4) == 0)
        return MUS_SPC;
    if(strncmp((char *)magic, "GYMX", 4) == 0)
        return MUS_SPC;
    if(strncmp((char *)magic, "HESM", 4) == 0)
        return MUS_SPC;
    if(strncmp((char *)magic, "KSCC", 4) == 0)
        return MUS_SPC;
    if(strncmp((char *)magic, "KSSX", 4) == 0)
        return MUS_SPC;
    if(strncmp((char *)magic, "NESM", 4) == 0)
        return MUS_SPC;
    if(strncmp((char *)magic, "NSFE", 4) == 0)
        return MUS_SPC;
    if(strncmp((char *)magic, "SAP\x0D", 4) == 0)
        return MUS_SPC;
    if(strncmp((char *)magic, "SNES", 4) == 0)
        return MUS_SPC;
    if(strncmp((char *)magic, "Vgm ", 4) == 0)
        return MUS_SPC;
    if(strncmp((char *)magic, "\x1f\x8b", 2) == 0)
        return MUS_SPC;

    /* Detect some module files */
    if(strncmp((char *)extramagic, "Extended Module", 15) == 0)
        return MUS_MOD;
    if(strncmp((char *)extramagic, "ASYLUM Music Format V", 22) == 0)
        return MUS_MOD;
    if(strncmp((char *)magic, "Extreme", 7) == 0)
        return MUS_MOD;
    if(strncmp((char *)magic, "IMPM", 4) == 0)
        return MUS_MOD;
    if(strncmp((char *)magic, "DBM0", 4) == 0)
        return MUS_MOD;
    if(strncmp((char *)magic, "DDMF", 4) == 0)
        return MUS_MOD;
    /* SMF files have the magic four bytes "RIFF" */
    if((strncmp((char *)magic, "RIFF", 4) == 0) &&
       (strncmp((char *)(extramagic + 8),  "DSMF", 4) == 0) &&
       (strncmp((char *)(extramagic + 12), "SONG", 4) == 0))
        return MUS_MOD;
    if(strncmp((char *)extramagic, "MAS_UTrack_V00", 14) == 0)
        return MUS_MOD;
    if(strncmp((char *)extramagic, "GF1PATCH110", 11) == 0)
        return MUS_MOD;
    if(strncmp((char *)magic, "FAR=", 4) == 0)
        return MUS_MOD;
    if(strncmp((char *)magic, "MTM", 3) == 0)
        return MUS_MOD;
    if(strncmp((char *)magic, "MMD", 3) == 0)
        return MUS_MOD;
    if(strncmp((char *)magic, "PSM\x20", 4) == 0)
        return MUS_MOD;
    if(strncmp((char *)magic, "PTMF", 4) == 0)
        return MUS_MOD;
    if(strncmp((char *)magic, "MT20", 4) == 0)
        return MUS_MOD;
    if(strncmp((char *)magic, "OKTA", 4) == 0)
        return MUS_MOD;
    if(strncmp((char *)magic, "JN", 2) == 0)
        return MUS_MOD;
    if(strncmp((char *)magic, "if", 2) == 0)
        return MUS_MOD;

    #if defined(MP3_MUSIC) || defined(MP3_MAD_MUSIC)
    /* Detect MP3 format [needs scanning of bigger part of the file] */
    if(SDLMixerX_detect_mp3(extramagic, src, start))
        return MUS_MP3;
    #endif

    #ifdef USE_ADL_MIDI
    /* Detect id Software Music Format file */
    if(detect_imf(src, start))
        return MUS_ADLMIDI;
    #endif

    /* Reset position to zero! */
    SDL_RWseek(src, start, RW_SEEK_SET);
    /* Assume MOD format.
     *
     * Apparently there is no way to check if the file is really a MOD,
     * or there are too many formats supported by MikMod/ModPlug, or
     * MikMod/ModPlug does this check by itself. */
    return MUS_MOD;
}

/*
 * Split path and arguments after "|" character.
 */
int split_path_and_params(const char *path, char *args)
{
    int hasArgs = 0;
    int srclen = (signed)strlen(path);
    int i, j;
    for(i = 0; i < srclen; i++)
        if(music_file[i] == '|') hasArgs = 1;
    if(hasArgs == 1)
    {
        /* char trackNum[strlen(path)]; */
        for(i = (signed)strlen(path) - 1; i >= 0; i--)
        {
            args[i] = music_file[i];
            if(music_file[i] == '|')
            {
                if(i == 0)
                {
                    Mix_SetError("Empty filename!");
                    return 0;
                }
                music_file[i] = '\0';
                i++;
                break;
            }
        }
        for(j = 0; i < srclen; i++, j++)
            args[j] = args[i];
        if(j < srclen) args[j] = '\0';
    }
    return 1;
}

void parse_adlmidi_args(char *args)
{
    char arg[100];
    char type = 'x';
    size_t maxlen = strlen(args);
    size_t i, j = 0;
    int value_opened = 0;
    for(i = 0; i < maxlen; i++)
    {
        char c = args[i];
        if(value_opened == 1)
        {
            if((c == ';') || (i == (maxlen - 1)))
            {
                int value;
                arg[j] = '\0';
                value = atoi(arg);
                switch(type)
                {
                    #ifdef USE_ADL_MIDI
                case 'b':
                    ADLMIDI_setBankID(value);
                    break;
                case 't':
                    ADLMIDI_setTremolo(value);
                    break;
                case 'v':
                    ADLMIDI_setVibrato(value);
                    break;
                case 'a':
                    ADLMIDI_setAdLibDrumsMode(value);
                    break;
                case 'm':
                    ADLMIDI_setScaleMod(value);
                    break;
                    #endif

                case 's':
                    MIX_SetMidiDevice(value);
                    break;
                case '\0':
                    break;
                default:
                    break;
                }
                value_opened = 0;
            }
            arg[j++] = c;
        }
        else
        {
            if(c == '\0') return;
            type = c;
            switch(c)
            {
            case 'b':
            case 't':
            case 'v':
            case 'a':
            case 'm':
            case 's':
                value_opened = 1;
                j = 0;
            }
        }
    }
}


/* Load a music file */
Mix_Music *SDLCALLCC Mix_LoadMUS(const char *file)
{
    SDL_RWops *src;
    Mix_Music *music;
    Mix_MusicType type;
    char *ext = NULL;

    if(music_file)
        SDL_free(music_file);
    if(music_args)
        SDL_free(music_args);

    music_file = NULL;
    music_args = NULL;

    if(!file)
    {
        music_file = NULL;
        Mix_SetError("Null filename!");
        return NULL;
    }
    music_file = (char *)SDL_malloc(sizeof(char) * strlen(file) + 1);
    music_args = (char *)SDL_malloc(sizeof(char) * strlen(file) + 1);
    strcpy(music_file, (char *)file);
    music_args[0] = '\0';
    #ifdef _WIN32
    if(music_file)
    {
        int i = 0;
        for(i = 0; music_file[i] != '\0'; i++)
        {
            if(music_file[i] == '\\')
                music_file[i] = '/';
        }
    }
    #endif
    if(split_path_and_params(music_file, music_args) == 0)
        return NULL;

    if(strstr(music_file, "/"))
        music_filename = strrchr(music_file, '/');

    if(strstr(music_filename, "."))
        ext = strrchr(music_filename, '.');

    #ifdef CMD_MUSIC
    if(music_cmd)
    {
        /* Allocate memory for the music structure */
        music = (Mix_Music *)SDL_malloc(sizeof(Mix_Music));
        if(music == NULL)
        {
            Mix_SetError("Out of memory");
            return(NULL);
        }
        music->error = 0;
        music->type = MUS_CMD;
        music->data.cmd = MusicCMD_LoadSong(music_cmd, music_file);
        if(music->data.cmd == NULL)
        {
            SDL_free(music);
            music = NULL;
        }
        return music;
    }
    #endif

    src = SDL_RWFromFile(music_file, "rb");
    if(src == NULL)
    {
        Mix_SetError("Couldn't open '%s'", music_file);
        return NULL;
    }

    /* Use the extension as a first guess on the file type */
    type = MUS_NONE;
    ext = strrchr(music_file, '.');
    /* No need to guard these with #ifdef *_MUSIC stuff,
     * since we simply call Mix_LoadMUSType_RW() later */
    if(ext)
    {
        ++ext; /* skip the dot in the extension */
        /* Detect by file type by extension of that files which are impossible
         * or hard to detect by magic number (For example, some MOD files are may be detected as MP3 files
         * because some of bytes are similar to MP3 Frame header which used to detect MP3 format itself) */
        if(MIX_string_equals(ext, "669") ||
           MIX_string_equals(ext, "APUN") ||
           MIX_string_equals(ext, "GDM") ||
           MIX_string_equals(ext, "MOD") ||
           MIX_string_equals(ext, "MODULE") ||
           MIX_string_equals(ext, "S3M") ||
           MIX_string_equals(ext, "STM") ||
           MIX_string_equals(ext, "UNI"))
            type = MUS_MOD;

        /*else if ( MIX_string_equals(ext, "AY") ||
                    MIX_string_equals(ext, "GBS") ||
                    MIX_string_equals(ext, "GYM") ||
                    MIX_string_equals(ext, "PCE") ||
                    MIX_string_equals(ext, "HES") ||
                    MIX_string_equals(ext, "KSS") ||
                    MIX_string_equals(ext, "NSF") ||
                    MIX_string_equals(ext, "NSFE") ||
                    MIX_string_equals(ext, "SAP") ||
                    MIX_string_equals(ext, "SPC") ||
                    MIX_string_equals(ext, "VGM") ||
                    MIX_string_equals(ext, "VGZ") ) {
            type = MUS_SPC;
        } else if ( MIX_string_equals(ext, "OGG") ) {
            type = MUS_OGG;
        } else if ( MIX_string_equals(ext, "FLAC") ) {
            type = MUS_FLAC;
        } else if( MIX_string_equals(ext, "MPG")  ||
                     MIX_string_equals(ext, "MPEG") ||
                     MIX_string_equals(ext, "MP3")  ||
                     MIX_string_equals(ext, "MAD") ) {
            type = MUS_MP3;
        }*/
    }
    if(type == MUS_NONE)
        type = detect_music_type(src);

    /* We need to know if a specific error occurs; if not, we'll set a
     * generic one, so we clear the current one. */
    SDL_ClearError();
    music = Mix_LoadMUSType_RW(src, type, SDL_TRUE);

    if(music == NULL && Mix_GetError()[0] == '\0')
        Mix_SetError("Unrecognized music format");

    return music;
}

Mix_Music *SDLCALLCC Mix_LoadMUS_RW(SDL_RWops *src, int freesrc)
{
    return Mix_LoadMUSType_RW(src, MUS_NONE, freesrc);
}

Mix_Music *SDLCALLCC Mix_LoadMUS_RW_ARG(SDL_RWops *src, int freesrc, char *args)
{
    if(music_args)
        SDL_free(music_args);
    music_args = (char *)SDL_malloc(sizeof(char) * strlen(args) + 1);
    music_args[0] = '\0';
    strcpy(music_args, args);
    return Mix_LoadMUSType_RW(src, MUS_NONE, freesrc);
}

Mix_Music *SDLCALLCC Mix_LoadMUS_RW_GME(SDL_RWops *src, int freesrc, int trackID)
{
    if(music_args)
        SDL_free(music_args);
    music_args = (char *)SDL_malloc(sizeof(char) * 25);
    music_args[0] = '\0';
    snprintf(music_args, 25, "%i", trackID);
    return Mix_LoadMUSType_RW(src, MUS_NONE, freesrc);
}

Mix_Music *SDLCALLCC Mix_LoadMUSType_RW(SDL_RWops *src, Mix_MusicType type, int freesrc)
{
    Mix_Music *music;
    Sint64 start;

    if(!src)
    {
        Mix_SetError("RWops pointer is NULL");
        return NULL;
    }
    start = SDL_RWtell(src);

    /* If the caller wants auto-detection, figure out what kind of file
     * this is. */
    if(type == MUS_NONE)
    {
        if((type = detect_music_type(src)) == MUS_NONE)
        {
            /* Don't call Mix_SetError() here since detect_music_type()
             * does that. */
            if(freesrc)
                SDL_RWclose(src);
            return NULL;
        }
    }

    /* Allocate memory for the music structure */
    music = (Mix_Music *)SDL_malloc(sizeof(Mix_Music));
    if(music == NULL)
    {
        Mix_SetError("Out of memory");
        if(freesrc)
            SDL_RWclose(src);
        return NULL;
    }
    music->error = 1;

    switch(type)
    {
    #ifdef WAV_MUSIC
    case MUS_WAV:
        music->type = MUS_WAV;
        music->data.wave = WAVStream_LoadSong_RW(src, freesrc);
        if(music->data.wave)
            music->error = 0;
        break;
    #endif

    #ifdef OGG_MUSIC
    case MUS_OGG:
        music->type = MUS_OGG;
        music->codec = available_codecs[MUS_OGG];
        music->music = music->codec.open(src, freesrc);
        if(music->music != NULL)
            music->error = 0;
        break;
    #endif

    #ifdef FLAC_MUSIC
    case MUS_FLAC:
        music->type = MUS_FLAC;
        music->codec = available_codecs[MUS_FLAC];
        music->music = music->codec.open(src, freesrc);
        if(music->music)
            music->error = 0;
        else
            Mix_SetError("Could not initialize FLAC stream.");
        break;
    #endif

    #if defined(MP3_MAD_MUSIC)
    case MUS_MP3:
        music->type = MUS_MP3_MAD;
        music->codec = available_codecs[MUS_MP3_MAD];
        music->music = music->codec.open(src, freesrc);
        if(music->music)
            music->error = 0;
        else
            Mix_SetError("Could not initialize MPEG stream.");
        break;

    #elif defined(MP3_MUSIC)
    case MUS_MP3:
        if(Mix_Init(MIX_INIT_MP3))
        {
            SMPEG_Info info;
            music->type = MUS_MP3;
            music->data.mp3 = smpeg.SMPEG_new_rwops(src, &info, freesrc, 0);
            if(!info.has_audio)
            {
                Mix_SetError("MPEG file does not have any audio stream.");
                smpeg.SMPEG_delete(music->data.mp3);
                /* Deleting the MP3 closed the source if desired */
                freesrc = SDL_FALSE;
            }
            else
            {
                smpeg.SMPEG_actualSpec(music->data.mp3, &used_mixer);
                music->error = 0;
            }
        }
        break;
    #endif

        #ifdef MID_MUSIC

        #ifdef USE_ADL_MIDI
    case MUS_ADLMIDI:
        #endif
    case MUS_MID:
        music->type = MUS_MID;
        if((need_reset_midi == 1) || (lock_midi_args == 0))
        {
            mididevice_next = 0;
            #ifdef USE_ADL_MIDI
            ADLMIDI_setDefaults();
            #endif
            #ifdef USE_OPN2_MIDI
            OPNMIDI_setDefaults();
            #endif
            parse_adlmidi_args(music_args);
        }

        #ifdef USE_ADL_MIDI
        if(type == MUS_ADLMIDI) /* Force ADLMIDI for file formats which are requires it (IMF, MUS, CMF and GMF) */
            mididevice_current = MIDI_ADLMIDI;
        else /* Install next MIDI device */
        #endif
            mididevice_current = mididevice_next;

        switch(mididevice_current)
        {
            #ifdef USE_NATIVE_MIDI
        case MIDI_Native:
            if(native_midi_ok)
            {
                SDL_RWseek(src, start, RW_SEEK_SET);
                music->data.nativemidi = native_midi_loadsong_RW(src, freesrc);
                if(music->data.nativemidi)
                    music->error = 0;
                else
                    Mix_SetError("Native MIDI error: %s", native_midi_error());
                break;
            }
            else
                Mix_SetError("Native MIDI is not ok: %s", native_midi_error());
            break;
            #endif

            #ifdef USE_FLUIDSYNTH_MIDI
        case MIDI_Fluidsynth:
            if(fluidsynth_ok)
            {
                SDL_RWseek(src, start, RW_SEEK_SET);
                music->data.fluidsynthmidi = fluidsynth_loadsong_RW(src, freesrc);
                if(music->data.fluidsynthmidi)
                    music->error = 0;
                break;
            }
            break;
            #endif

            #ifdef USE_ADL_MIDI
        case MIDI_ADLMIDI:
            if(adl_midi_ok)
            {
                SDL_RWseek(src, start, RW_SEEK_SET);
                music->data.midi_adl = ADLMIDI_new_RW(src, freesrc);
                if(music->data.midi_adl)
                    music->error = 0;
                break;
            }
            break;
            #endif

            #ifdef USE_OPN2_MIDI
        case MIDI_OPNMIDI:
            if(opn2_midi_ok)
            {
                SDL_RWseek(src, start, RW_SEEK_SET);
                music->data.midi_opn = OPNMIDI_new_RW(src, freesrc);
                if(music->data.midi_opn)
                    music->error = 0;
                break;
            }
            break;
            #endif

            #ifdef USE_TIMIDITY_MIDI
        case MIDI_Timidity:
            if(timidity_ok)
            {
                SDL_RWseek(src, start, RW_SEEK_SET);
                music->data.midi = Timidity_LoadSong_RW(src, freesrc);
                if(music->data.midi)
                    music->error = 0;
                else
                    Mix_SetError("Timidity error: %s", Timidity_Error());
            }
            else
                Mix_SetError("Timidty is not ok: %s", Timidity_Error());
            break;
            #endif
        }
        break;
        #endif

        #ifdef GME_MUSIC
    case MUS_SPC:
        if(music->error)
        {
            SDL_RWseek(src, start, RW_SEEK_SET);
            music->type = MUS_SPC;
            music->codec = available_codecs[MUS_SPC];
            music->music = music->codec.openEx(src, freesrc, music_args);
            if(music->music)
                music->error = 0;
        }
        break;
        #endif

        #if defined(MODPLUG_MUSIC) || defined(MOD_MUSIC)
    case MUS_MOD:
        #ifdef MODPLUG_MUSIC
        if(music->error)
        {
            SDL_RWseek(src, start, RW_SEEK_SET);
            music->type = MUS_MODPLUG;
            music->codec = available_codecs[MUS_MODPLUG];
            music->music = music->codec.open(src, freesrc);
            if(music->music)
                music->error = 0;
        }
        #endif

        #ifdef MOD_MUSIC
        if(music->error)
        {
            SDL_RWseek(src, start, RW_SEEK_SET);
            music->type = MUS_MOD;
            music->data.module = MOD_new_RW(src, freesrc);
            if(music->data.module)
                music->error = 0;
        }
        #endif
        break;
        #endif

    default:
        Mix_SetError("Unrecognized music format");
        break;
    } /* switch (want) */

    if(music->error)
    {
        SDL_free(music);
        if(freesrc)
            SDL_RWclose(src);
        else
            SDL_RWseek(src, start, RW_SEEK_SET);
        music = NULL;
    }
    return music;
}

/* Free a music chunk previously loaded */
void SDLCALLCC Mix_FreeMusic(Mix_Music *music)
{
    if(music)
    {
        /* Stop the music if it's currently playing */
        Mix_LockAudio();
        if(music == music_playing)
        {
            /* Wait for any fade out to finish */
            while(music->fading == MIX_FADING_OUT)
            {
                Mix_UnlockAudio();
                SDL_Delay(100);
                Mix_LockAudio();
            }
            if(music == music_playing)
                music_internal_halt();
        }
        Mix_UnlockAudio();
        switch(music->type)
        {
            #ifdef CMD_MUSIC
        case MUS_CMD:
            MusicCMD_FreeSong(music->data.cmd);
            break;
            #endif

            #ifdef WAV_MUSIC
        case MUS_WAV:
            WAVStream_FreeSong(music->data.wave);
            break;
            #endif

            #ifdef MOD_MUSIC
        case MUS_MOD:
            MOD_delete(music->data.module);
            break;
            #endif

            #ifdef MID_MUSIC
        case MUS_MID:
            switch(mididevice_current)
            {
                #ifdef USE_NATIVE_MIDI
            case MIDI_Native:
                if(native_midi_ok)
                {
                    native_midi_freesong(music->data.nativemidi);
                    goto skip;
                }
                break;
                #endif

                #ifdef USE_FLUIDSYNTH_MIDI
            case MIDI_Fluidsynth:
                if(fluidsynth_ok)
                {
                    fluidsynth_freesong(music->data.fluidsynthmidi);
                    goto skip;
                }
                break;
                #endif

                #ifdef USE_ADL_MIDI
            case MIDI_ADLMIDI:
                if(adl_midi_ok)
                {
                    ADLMIDI_delete(music->data.midi_adl);
                    goto skip;
                }
                break;
                #endif

                #ifdef USE_OPN2_MIDI
            case MIDI_OPNMIDI:
                if(opn2_midi_ok)
                {
                    OPNMIDI_delete(music->data.midi_opn);
                    goto skip;
                }
                break;
                #endif

                #ifdef USE_TIMIDITY_MIDI
            case MIDI_Timidity:
                if(timidity_ok)
                {
                    Timidity_FreeSong(music->data.midi);
                    goto skip;
                }
                break;
                #endif
            }
            break;
            #endif

            #if defined(OGG_MUSIC)||defined(FLAC_MUSIC)||defined(MP3_MAD_MUSIC)||defined(MODPLUG_MUSIC)||defined(GME_MUSIC)
        case MUS_OGG:
        case MUS_FLAC:
        case MUS_MP3_MAD:
        case MUS_MODPLUG:
        case MUS_SPC:
            music->codec.close(music->music);
            music->music = 0;
            break;
            #endif

            #ifdef MP3_MUSIC
        case MUS_MP3:
            smpeg.SMPEG_delete(music->data.mp3);
            break;
            #endif

        default:
            /* Unknown music type?? */
            break;
        }

skip:
        SDL_free(music);
    }
}

/* Find out the music format of a mixer music, or the currently playing
   music, if 'music' is NULL.
*/
Mix_MusicType SDLCALLCC Mix_GetMusicType(const Mix_Music *music)
{
    Mix_MusicType type = MUS_NONE;

    if(music)
        type = music->type;
    else
    {
        Mix_LockAudio();
        if(music_playing)
            type = music_playing->type;
        Mix_UnlockAudio();
    }
    return(type);
}

/* Get music title from meta-tag if possible */
const char *SDLCALLCC Mix_GetMusicTitle(const Mix_Music *music)
{
    const char *tag = Mix_GetMusicTitleTag(music);
    if(strlen(tag) > 0)
        return tag;
    if(music_filename != NULL)
        return music_filename;
    return "";
}

const char *SDLCALLCC Mix_GetMusicTitleTag(const Mix_Music *music)
{
    if(music)
    {
        switch(music->type)
        {
            #if defined(OGG_MUSIC)||defined(FLAC_MUSIC)||defined(MP3_MAD_MUSIC)||defined(MODPLUG_MUSIC)||defined(GME_MUSIC)
        case MUS_OGG:
        case MUS_FLAC:
        case MUS_MP3_MAD:
        case MUS_MODPLUG:
        case MUS_SPC:
            return music->codec.metaTitle(music->music);
            break;
            #endif

        default:
            break;
        }
    }
    return "";
}

const char *SDLCALLCC Mix_GetMusicArtistTag(const Mix_Music *music)
{
    if(music)
    {
        switch(music->type)
        {
            #if defined(OGG_MUSIC)||defined(FLAC_MUSIC)||defined(MP3_MAD_MUSIC)||defined(MODPLUG_MUSIC)||defined(GME_MUSIC)
        case MUS_OGG:
        case MUS_FLAC:
        case MUS_MP3_MAD:
        case MUS_MODPLUG:
        case MUS_SPC:
            return music->codec.metaArtist(music->music);
            break;
            #endif

        default:
            break;
        }
    }
    return "";
}

const char *SDLCALLCC Mix_GetMusicAlbumTag(const Mix_Music *music)
{
    if(music)
    {
        switch(music->type)
        {
            #if defined(OGG_MUSIC)||defined(FLAC_MUSIC)||defined(MP3_MAD_MUSIC)||defined(MODPLUG_MUSIC)||defined(GME_MUSIC)
        case MUS_OGG:
        case MUS_FLAC:
        case MUS_MP3_MAD:
        case MUS_MODPLUG:
        case MUS_SPC:
            return music->codec.metaAlbum(music->music);
            break;
            #endif

        default:
            break;
        }
    }
    return "";
}

const char *SDLCALLCC Mix_GetMusicCopyrightTag(const Mix_Music *music)
{
    if(music)
    {
        switch(music->type)
        {
            #if defined(OGG_MUSIC)||defined(FLAC_MUSIC)||defined(MP3_MAD_MUSIC)||defined(MODPLUG_MUSIC)||defined(GME_MUSIC)
        case MUS_OGG:
        case MUS_FLAC:
        case MUS_MP3_MAD:
        case MUS_MODPLUG:
        case MUS_SPC:
            return music->codec.metaCopyright(music->music);
            break;
            #endif
        default:
            break;
        }
    }
    return "";
}


/* Play a music chunk.  Returns 0, or -1 if there was an error.
 */
static int music_internal_play(Mix_Music *music, double position)
{
    int retval = 0;

    #if defined(__MACOSX__) && defined(USE_NATIVE_MIDI)
    /* This fixes a bug with native MIDI on Mac OS X, where you
       can't really stop and restart MIDI from the audio callback.
    */
    if(music == music_playing && music->type == MUS_MID && native_midi_ok)
    {
        /* Just a seek suffices to restart playing */
        music_internal_position(position);
        return 0;
    }
    #endif

    /* Note the music we're playing */
    if(music_playing)
        music_internal_halt();
    music_playing = music;

    /* Set the initial volume */
    if(music->type != MUS_MOD)
        music_internal_initialize_volume();

    /* Set up for playback */
    switch(music->type)
    {
        #ifdef CMD_MUSIC
    case MUS_CMD:
        MusicCMD_Start(music->data.cmd);
        break;
        #endif

        #ifdef WAV_MUSIC
    case MUS_WAV:
        WAVStream_Start(music->data.wave);
        break;
        #endif

        #ifdef MOD_MUSIC
    case MUS_MOD:
        MOD_play(music->data.module);
        /* Player_SetVolume() does nothing before Player_Start() */
        music_internal_initialize_volume();
        break;
        #endif

        #ifdef MID_MUSIC
    case MUS_MID:
        switch(mididevice_current)
        {
            #ifdef USE_NATIVE_MIDI
        case MIDI_Native:
            if(native_midi_ok)
            {
                native_midi_start(music->data.nativemidi, music_loops);
                goto skip;
            }
            break;
            #endif

            #ifdef USE_FLUIDSYNTH_MIDI
        case MIDI_Fluidsynth:
            if(fluidsynth_ok)
            {
                fluidsynth_start(music->data.fluidsynthmidi);
                goto skip;
            }
            break;
            #endif

            #ifdef USE_ADL_MIDI
        case MIDI_ADLMIDI:
            if(adl_midi_ok)
            {
                ADLMIDI_setInfiniteLoop(music_playing->data.midi_adl, music_loops < 0 ? 1 : 0);
                ADLMIDI_play(music->data.midi_adl);
                music_internal_initialize_volume();
                goto skip;
            }
            break;
            #endif

            #ifdef USE_OPN2_MIDI
        case MIDI_OPNMIDI:
            if(opn2_midi_ok)
            {
                OPNMIDI_setInfiniteLoop(music_playing->data.midi_opn, music_loops < 0 ? 1 : 0);
                OPNMIDI_play(music->data.midi_opn);
                music_internal_initialize_volume();
                goto skip;
            }
            break;
            #endif

            #ifdef USE_TIMIDITY_MIDI
        case MIDI_Timidity:
            if(timidity_ok)
            {
                Timidity_Start(music->data.midi);
                goto skip;
            }
            break;
            #endif
        }
        break;
        #endif
        #if defined(OGG_MUSIC)||defined(FLAC_MUSIC)||defined(MP3_MAD_MUSIC)||defined(MODPLUG_MUSIC)||defined(GME_MUSIC)
    case MUS_OGG:
    case MUS_FLAC:
    case MUS_MP3_MAD:
    case MUS_MODPLUG:
    case MUS_SPC:
        if((music->type == MUS_MODPLUG)||(music->type == MUS_SPC))
            music_internal_initialize_volume();
        music->codec.setLoops(music->music, music_loops);
        music->codec.play(music->music);
        break;
        #endif

        #ifdef MP3_MUSIC
    case MUS_MP3:
        smpeg.SMPEG_enableaudio(music->data.mp3, 1);
        smpeg.SMPEG_enablevideo(music->data.mp3, 0);
        smpeg.SMPEG_play(music_playing->data.mp3);
        break;
        #endif

    default:
        Mix_SetError("Can't play unknown music type");
        retval = -1;
        break;
    }

skip:
    /* Set the playback position, note any errors if an offset is used */
    if(retval == 0)
    {
        if(position > 0.0)
        {
            if(music_internal_position(position) < 0)
            {
                Mix_SetError("Position not implemented for music type");
                retval = -1;
            }
        }
        else
            music_internal_position(0.0);
    }

    /* If the setup failed, we're not playing any music anymore */
    if(retval < 0)
        music_playing = NULL;
    return(retval);
}
int SDLCALLCC Mix_FadeInMusicPos(Mix_Music *music, int loops, int ms, double position)
{
    int retval;

    if(ms_per_step == 0)
    {
        SDL_SetError("Audio device hasn't been opened");
        return(-1);
    }

    /* Don't play null pointers :-) */
    if(music == NULL)
    {
        Mix_SetError("music parameter was NULL");
        return(-1);
    }

    /* Setup the data */
    if(ms)
        music->fading = MIX_FADING_IN;
    else
        music->fading = MIX_NO_FADING;
    music->fade_step = 0;
    music->fade_steps = ms / ms_per_step;

    /* Play the puppy */
    Mix_LockAudio();
    /* If the current music is fading out, wait for the fade to complete */
    while(music_playing && (music_playing->fading == MIX_FADING_OUT))
    {
        Mix_UnlockAudio();
        SDL_Delay(100);
        Mix_LockAudio();
    }
    music_active = 1;
    if(loops == 1)
    {
        /* Loop is the number of times to play the audio */
        loops = 0;
    }
    music_loops = loops;
    retval = music_internal_play(music, position);
    Mix_UnlockAudio();

    return(retval);
}

int SDLCALLCC Mix_FadeInMusic(Mix_Music *music, int loops, int ms)
{
    return Mix_FadeInMusicPos(music, loops, ms, 0.0);
}

int SDLCALLCC Mix_PlayMusic(Mix_Music *music, int loops)
{
    return Mix_FadeInMusicPos(music, loops, 0, 0.0);
}

/* Set the playing music position */
int music_internal_position(double position)
{
    int retval = 0;

    switch(music_playing->type)
    {
        #ifdef MOD_MUSIC
    case MUS_MOD:
        MOD_jump_to_time(music_playing->data.module, position);
        break;
        #endif

        #if defined(OGG_MUSIC)||defined(FLAC_MUSIC)||defined(MP3_MAD_MUSIC)||defined(MODPLUG_MUSIC)
    case MUS_OGG:
    case MUS_FLAC:
    case MUS_MP3_MAD:
    case MUS_MODPLUG:
        music_playing->codec.jumpToTime(music_playing->music, position);
        break;
        #endif

        #ifdef MP3_MUSIC
    case MUS_MP3:
        smpeg.SMPEG_rewind(music_playing->data.mp3);
        smpeg.SMPEG_play(music_playing->data.mp3);
        if(position > 0.0)
            smpeg.SMPEG_skip(music_playing->data.mp3, (float)position);
        break;
        #endif
    default:
        /* TODO: Implement this for other music backends */
        retval = -1;
        break;
    }
    return(retval);
}
int SDLCALLCC Mix_SetMusicPosition(double position)
{
    int retval;

    Mix_LockAudio();
    if(music_playing)
    {
        retval = music_internal_position(position);
        if(retval < 0)
            Mix_SetError("Position not implemented for music type");
    }
    else
    {
        Mix_SetError("Music isn't playing");
        retval = -1;
    }
    Mix_UnlockAudio();

    return(retval);
}

/* Set the music's initial volume */
static void music_internal_initialize_volume(void)
{
    if(music_playing->fading == MIX_FADING_IN)
        music_internal_volume(0);
    else
        music_internal_volume(music_volume);
}

/* Set the music volume */
static void music_internal_volume(int volume)
{
    switch(music_playing->type)
    {
        #ifdef CMD_MUSIC
    case MUS_CMD:
        MusicCMD_SetVolume(volume);
        break;
        #endif

        #ifdef WAV_MUSIC
    case MUS_WAV:
        WAVStream_SetVolume(music_playing->data.wave, volume);
        break;
        #endif

        #ifdef MOD_MUSIC
    case MUS_MOD:
        MOD_setvolume(music_playing->data.module, volume);
        break;
        #endif

        #ifdef MID_MUSIC
    case MUS_MID:
        switch(mididevice_current)
        {
            #ifdef USE_NATIVE_MIDI
        case MIDI_Native:
            if(native_midi_ok)
            {
                native_midi_setvolume(volume);
                return;
            }
            break;
            #endif

            #ifdef USE_FLUIDSYNTH_MIDI
        case MIDI_Fluidsynth:
            if(fluidsynth_ok)
            {
                fluidsynth_setvolume(music_playing->data.fluidsynthmidi, volume);
                return;
            }
            break;
            #endif

            #ifdef USE_ADL_MIDI
        case MIDI_ADLMIDI:
            if(adl_midi_ok)
            {
                ADLMIDI_setvolume(music_playing->data.midi_adl, volume);
                return;
            }
            break;
            #endif

            #ifdef USE_OPN2_MIDI
        case MIDI_OPNMIDI:
            if(opn2_midi_ok)
            {
                OPNMIDI_setvolume(music_playing->data.midi_opn, volume);
                return;
            }
            break;
            #endif

            #ifdef USE_TIMIDITY_MIDI
        case MIDI_Timidity:
            if(timidity_ok)
            {
                Timidity_SetVolume(volume);
                return;
            }
            break;
            #endif
        }
        break;
        #endif

        #if defined(OGG_MUSIC)||defined(FLAC_MUSIC)||defined(MP3_MAD_MUSIC)||defined(MODPLUG_MUSIC)||defined(GME_MUSIC)
    case MUS_OGG:
    case MUS_FLAC:
    case MUS_MP3_MAD:
    case MUS_MODPLUG:
    case MUS_SPC:
        music_playing->codec.setVolume(music_playing->music, volume);
        break;
        #endif

        #ifdef MP3_MUSIC
    case MUS_MP3:
        smpeg.SMPEG_setvolume(music_playing->data.mp3, (int)(((float)volume / (float)MIX_MAX_VOLUME) * 100.0));
        break;
        #endif

    default:
        /* Unknown music type?? */
        break;
    }
}
int SDLCALLCC Mix_VolumeMusic(int volume)
{
    int prev_volume;

    prev_volume = music_volume;
    if(volume < 0)
        return prev_volume;
    if(volume > SDL_MIX_MAXVOLUME)
        volume = SDL_MIX_MAXVOLUME;
    music_volume = volume;
    Mix_LockAudio();
    if(music_playing)
        music_internal_volume(music_volume);
    Mix_UnlockAudio();
    return(prev_volume);
}

/* Halt playing of music */
static void music_internal_halt(void)
{
    switch(music_playing->type)
    {
        #ifdef CMD_MUSIC
    case MUS_CMD:
        MusicCMD_Stop(music_playing->data.cmd);
        break;
        #endif

        #ifdef WAV_MUSIC
    case MUS_WAV:
        WAVStream_Stop(music_playing->data.wave);
        break;
        #endif

        #ifdef MOD_MUSIC
    case MUS_MOD:
        MOD_stop(music_playing->data.module);
        break;
        #endif

        #ifdef MID_MUSIC
    case MUS_MID:
        switch(mididevice_current)
        {
            #ifdef USE_NATIVE_MIDI
        case MIDI_Native:
            if(native_midi_ok)
            {
                native_midi_stop();
                goto skip;
            }
            break;
            #endif

            #ifdef USE_FLUIDSYNTH_MIDI
        case MIDI_Fluidsynth:
            if(fluidsynth_ok)
            {
                fluidsynth_stop(music_playing->data.fluidsynthmidi);
                goto skip;
            }
            break;
            #endif

            #ifdef USE_ADL_MIDI
        case MIDI_ADLMIDI:
            if(adl_midi_ok)
            {
                ADLMIDI_stop(music_playing->data.midi_adl);
                goto skip;
            }
            break;
            #endif

            #ifdef USE_OPN2_MIDI
        case MIDI_OPNMIDI:
            if(opn2_midi_ok)
            {
                OPNMIDI_stop(music_playing->data.midi_opn);
                goto skip;
            }
            break;
            #endif

            #ifdef USE_TIMIDITY_MIDI
        case MIDI_Timidity:
            if(timidity_ok)
            {
                Timidity_Stop();
                goto skip;
            }
            break;
            #endif
        }
        break;
        #endif

        #if defined(OGG_MUSIC)||defined(FLAC_MUSIC)||defined(MP3_MAD_MUSIC)||defined(MODPLUG_MUSIC)||defined(GME_MUSIC)
    case MUS_OGG:
    case MUS_FLAC:
    case MUS_MP3_MAD:
    case MUS_MODPLUG:
    case MUS_SPC:
        music_playing->codec.stop(music_playing->music);
        break;
        #endif

        #ifdef MP3_MUSIC
    case MUS_MP3:
        smpeg.SMPEG_stop(music_playing->data.mp3);
        break;
        #endif

    default:
        /* Unknown music type?? */
        return;
    }

skip:
    music_playing->fading = MIX_NO_FADING;
    music_playing = NULL;
}
int SDLCALLCC Mix_HaltMusic(void)
{
    Mix_LockAudio();
    if(music_playing)
    {
        music_internal_halt();
        if(music_finished_hook)
            music_finished_hook();
    }
    Mix_UnlockAudio();

    return(0);
}

/* Progressively stop the music */
int SDLCALLCC Mix_FadeOutMusic(int ms)
{
    int retval = 0;

    if(ms_per_step == 0)
    {
        SDL_SetError("Audio device hasn't been opened");
        return 0;
    }

    if(ms <= 0)     /* just halt immediately. */
    {
        Mix_HaltMusic();
        return 1;
    }

    Mix_LockAudio();
    if(music_playing)
    {
        int fade_steps = (ms + ms_per_step - 1) / ms_per_step;
        if(music_playing->fading == MIX_NO_FADING)
            music_playing->fade_step = 0;
        else
        {
            int step;
            int old_fade_steps = music_playing->fade_steps;
            if(music_playing->fading == MIX_FADING_OUT)
                step = music_playing->fade_step;
            else
            {
                step = old_fade_steps
                       - music_playing->fade_step + 1;
            }
            music_playing->fade_step = (step * fade_steps)
                                       / old_fade_steps;
        }
        music_playing->fading = MIX_FADING_OUT;
        music_playing->fade_steps = fade_steps;
        retval = 1;
    }
    Mix_UnlockAudio();

    return(retval);
}

Mix_Fading SDLCALLCC Mix_FadingMusic(void)
{
    Mix_Fading fading = MIX_NO_FADING;

    Mix_LockAudio();
    if(music_playing)
        fading = music_playing->fading;
    Mix_UnlockAudio();

    return(fading);
}

/* Pause/Resume the music stream */
void SDLCALLCC Mix_PauseMusic(void)
{
    music_active = 0;
}

void SDLCALLCC Mix_ResumeMusic(void)
{
    music_active = 1;
}

void SDLCALLCC Mix_RewindMusic(void)
{
    Mix_SetMusicPosition(0.0);
}

int SDLCALLCC Mix_PausedMusic(void)
{
    return (music_active == 0);
}

/* Check the status of the music */
static int music_internal_playing()
{
    int playing = 1;

    if(music_playing == NULL)
        return 0;

    switch(music_playing->type)
    {
        #ifdef CMD_MUSIC
    case MUS_CMD:
        if(!MusicCMD_Active(music_playing->data.cmd))
            playing = 0;
        break;
        #endif

        #ifdef WAV_MUSIC
    case MUS_WAV:
        if(! WAVStream_Active(music_playing->data.wave))
            playing = 0;
        break;
        #endif

        #ifdef MOD_MUSIC
    case MUS_MOD:
        if(! MOD_playing(music_playing->data.module))
            playing = 0;
        break;
        #endif


        #ifdef MID_MUSIC
    case MUS_MID:
        switch(mididevice_current)
        {
            #ifdef USE_NATIVE_MIDI
        case MIDI_Native:
            if(native_midi_ok)
            {
                if(! native_midi_active())
                    playing = 0;
                goto skip;
            }
            break;
            #endif
            #ifdef USE_FLUIDSYNTH_MIDI
        case MIDI_Fluidsynth:
            if(fluidsynth_ok)
            {
                if(! fluidsynth_active(music_playing->data.fluidsynthmidi))
                    playing = 0;
                goto skip;
            }
            break;
            #endif

            #ifdef USE_ADL_MIDI
        case MIDI_ADLMIDI:
            if(adl_midi_ok)
            {
                if(! ADLMIDI_playing(music_playing->data.midi_adl))
                    playing = 0;
                goto skip;
            }
            break;
            #endif

            #ifdef USE_OPN2_MIDI
        case MIDI_OPNMIDI:
            if(opn2_midi_ok)
            {
                if(! OPNMIDI_playing(music_playing->data.midi_opn))
                    playing = 0;
                goto skip;
            }
            break;
            #endif

            #ifdef USE_TIMIDITY_MIDI
        case MIDI_Timidity:
            if(timidity_ok)
            {
                if(! Timidity_Active())
                    playing = 0;
                goto skip;
            }
            break;
            #endif
        }
        break;
        #endif

        #if defined(OGG_MUSIC)||defined(FLAC_MUSIC)||defined(MP3_MAD_MUSIC)||defined(MODPLUG_MUSIC)||defined(GME_MUSIC)
    case MUS_OGG:
    case MUS_FLAC:
    case MUS_MP3_MAD:
    case MUS_MODPLUG:
    case MUS_SPC:
        if(!music_playing->codec.isPlaying(music_playing->music))
            playing = 0;
        break;
        #endif

        #ifdef MP3_MUSIC
    case MUS_MP3:
        if(smpeg.SMPEG_status(music_playing->data.mp3) != SMPEG_PLAYING)
            playing = 0;
        break;
        #endif

    default:
        playing = 0;
        break;
    }

skip:
    return(playing);
}
int SDLCALLCC Mix_PlayingMusic(void)
{
    int playing = 0;

    Mix_LockAudio();
    if(music_playing)
        playing = music_loops || music_internal_playing();
    Mix_UnlockAudio();

    return(playing);
}

/* Set the external music playback command */
int SDLCALLCC Mix_SetMusicCMD(const char *command)
{
    Mix_HaltMusic();
    if(music_cmd)
    {
        SDL_free(music_cmd);
        music_cmd = NULL;
    }
    if(command)
    {
        music_cmd = (char *)SDL_malloc(strlen(command) + 1);
        if(music_cmd == NULL)
            return(-1);
        strcpy(music_cmd, command);
    }
    return(0);
}

int SDLCALLCC Mix_SetSynchroValue(int i)
{
    (void)i;
    /* Not supported by any players at this time */
    return(-1);
}

int SDLCALLCC Mix_GetSynchroValue(void)
{
    /* Not supported by any players at this time */
    return(-1);
}


/* Uninitialize the music players */
void close_music(void)
{
    Mix_HaltMusic();

    #ifdef CMD_MUSIC
    Mix_SetMusicCMD(NULL);
    #endif

    #ifdef MOD_MUSIC
    MOD_exit();
    #endif

    #ifdef MID_MUSIC
        #ifdef USE_ADL_MIDI
        ADLMIDI_exit();
        #endif

        #ifdef USE_OPN2_MIDI
        OPNMIDI_exit();
        #endif

        # ifdef USE_TIMIDITY_MIDI
        Timidity_Close();
        # endif
    #endif

    if(music_file)
        SDL_free(music_file);

    music_file = NULL;
    music_filename = NULL;

    /* rcg06042009 report available decoders at runtime. */
    SDL_free((void *)music_decoders);
    music_decoders = NULL;
    num_decoders = 0;

    ms_per_step = 0;
}

int SDLCALLCC Mix_SetSoundFonts(const char *paths)
{
    #ifdef MID_MUSIC
    if(soundfont_paths)
    {
        SDL_free(soundfont_paths);
        soundfont_paths = NULL;
    }

    if(paths)
    {
        if(!(soundfont_paths = SDL_strdup(paths)))
        {
            Mix_SetError("Insufficient memory to set SoundFonts");
            return 0;
        }
    }
    #endif
    return 1;
}

#ifdef MID_MUSIC
const char *SDLCALLCC Mix_GetSoundFonts(void)
{
    const char *force = getenv("SDL_FORCE_SOUNDFONTS");

    if(!soundfont_paths || (force && force[0] == '1'))
        return getenv("SDL_SOUNDFONTS");
    else
        return soundfont_paths;
}

#if !defined(__MINGW32__) && !defined(__MINGW64__) && !defined(_WIN32)
extern char *strtok_r(char *, const char *, char **);
#endif

int SDLCALLCC Mix_EachSoundFont(int (*function)(const char *, void *), void *data)
{
    char *context, *path = NULL, *paths;
    const char *cpaths = Mix_GetSoundFonts();

    if(!cpaths)
    {
        Mix_SetError("No SoundFonts have been requested");
        return 0;
    }

    if(!(paths = SDL_strdup(cpaths)))
    {
        Mix_SetError("Insufficient memory to iterate over SoundFonts");
        return 0;
    }

    #if defined(__MINGW32__) || defined(__MINGW64__)
    for(path = strtok(paths, ";"); path; path = strtok(NULL, ";"))
    {
    #elif defined(_WIN32)
    for(path = strtok_s(paths, ";", &context); path; path = strtok_s(NULL, ";", &context))
    {
    #else
    for(path = strtok_r(paths, ":;", &context); path; path = strtok_r(NULL, ":;", &context))
    {
    #endif
        if(!function(path, data))
        {
            SDL_free(paths);
            return 0;
        }
    }

    SDL_free(paths);
    return 1;
}
#endif

void SDLCALLCC MIX_Timidity_addToPathList(const char *path)
{
    #ifdef USE_TIMIDITY_MIDI
    Timidity_addToPathList(path);
    #else
    (void *)path;
    #endif
}


/* ADLMIDI Setup functions */

int    SDLCALLCC  MIX_ADLMIDI_getTotalBanks()
{
    return ADLMIDI_getBanksCount();
}

const char *const *SDLCALLCC  MIX_ADLMIDI_getBankNames()
{
    return ADLMIDI_getBankNames();
}

int SDLCALLCC MIX_ADLMIDI_getBankID()
{
    #ifdef USE_ADL_MIDI
    return ADLMIDI_getBankID();
    #else
    Mix_SetError("SDL2 Mixer X built without ADLMIDI support!");
    return -1;
    #endif
}

void SDLCALLCC MIX_ADLMIDI_setBankID(int bnk)
{
    need_reset_midi = 0;
    #ifdef USE_ADL_MIDI
    ADLMIDI_setBankID(bnk);
    #endif
}

int SDLCALLCC MIX_ADLMIDI_getTremolo()
{
    #ifdef USE_ADL_MIDI
    return ADLMIDI_getTremolo();
    #else
    Mix_SetError("SDL2 Mixer X built without ADLMIDI support!");
    return -1;
    #endif
}

void SDLCALLCC MIX_ADLMIDI_setTremolo(int tr)
{
    need_reset_midi = 0;
    #ifdef USE_ADL_MIDI
    ADLMIDI_setTremolo(tr);
    #endif
}

int SDLCALLCC MIX_ADLMIDI_getVibrato()
{
    #ifdef USE_ADL_MIDI
    return ADLMIDI_getVibrato();
    #else
    Mix_SetError("SDL2 Mixer X built without ADLMIDI support!");
    return -1;
    #endif
}

void SDLCALLCC MIX_ADLMIDI_setVibrato(int vib)
{
    need_reset_midi = 0;
    #ifdef USE_ADL_MIDI
    ADLMIDI_setVibrato(vib);
    #endif
}

int SDLCALLCC MIX_ADLMIDI_getScaleMod()
{
    #ifdef USE_ADL_MIDI
    return ADLMIDI_getScaleMod();
    #else
    Mix_SetError("SDL2 Mixer X built without ADLMIDI support!");
    return -1;
    #endif
}

void SDLCALLCC MIX_ADLMIDI_setScaleMod(int sc)
{
    need_reset_midi = 0;
    #ifdef USE_ADL_MIDI
    ADLMIDI_setScaleMod(sc);
    #endif
}

int SDLCALLCC MIX_ADLMIDI_getAdLibMode()
{
    #ifdef USE_ADL_MIDI
    return ADLMIDI_getAdLibDrumsMode();
    #else
    Mix_SetError("SDL2 Mixer X built without ADLMIDI support!");
    return -1;
    #endif
}

void SDLCALLCC MIX_ADLMIDI_setAdLibMode(int sc)
{
    need_reset_midi = 0;
    #ifdef USE_ADL_MIDI
    ADLMIDI_setAdLibDrumsMode(sc);
    #endif
}

void SDLCALLCC MIX_ADLMIDI_setSetDefaults()
{
    need_reset_midi = 1;
    #ifdef USE_ADL_MIDI
    ADLMIDI_setDefaults();
    #endif
}

int SDLCALLCC MIX_ADLMIDI_getLogarithmicVolumes()
{
    #ifdef USE_ADL_MIDI
    return ADLMIDI_getLogarithmicVolumes();
    #else
    Mix_SetError("SDL2 Mixer X built without ADLMIDI support!");
    return -1;
    #endif
}

void SDLCALLCC MIX_ADLMIDI_setLogarithmicVolumes(int lv)
{
    #ifdef USE_ADL_MIDI
    ADLMIDI_setLogarithmicVolumes(lv);
    #endif
}

int SDLCALLCC MIX_ADLMIDI_getVolumeModel()
{
    #ifdef USE_ADL_MIDI
    return ADLMIDI_getVolumeModel();
    #else
    Mix_SetError("SDL2 Mixer X built without ADLMIDI support!");
    return -1;
    #endif
}

void SDLCALLCC MIX_ADLMIDI_setVolumeModel(int vm)
{
    #ifdef USE_ADL_MIDI
    ADLMIDI_setVolumeModel(vm);
    #endif
}


void SDLCALLCC MIX_OPNMIDI_setCustomBankFile(const char *bank_wonp_path)
{
    #ifdef USE_OPN2_MIDI
    OPNMIDI_setCustomBankFile(bank_wonp_path);
    #endif
}


int SDLCALLCC MIX_SetMidiDevice(int device)
{
    switch(device)
    {
        #ifdef MID_MUSIC

        #ifdef USE_ADL_MIDI
    case MIDI_ADLMIDI:
        #endif

        #ifdef USE_OPN2_MIDI
    case MIDI_OPNMIDI:
        #endif

        #ifdef USE_TIMIDITY_MIDI
    case MIDI_Timidity:
        #endif

        #ifdef USE_NATIVE_MIDI
    case MIDI_Native:
        #endif

        #ifdef USE_FLUIDSYNTH_MIDI
    case MIDI_Fluidsynth:
        #endif

        mididevice_next = device;
        need_reset_midi = 0;
        return 0;
        break;
        #endif

    default:
        Mix_SetError("Unknown MIDI Device");
        return -1;
        break;
    }
}

void SDLCALLCC MIX_SetLockMIDIArgs(int lock_midiargs)
{
    lock_midi_args = lock_midiargs;
}

