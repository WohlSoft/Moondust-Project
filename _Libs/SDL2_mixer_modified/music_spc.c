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
struct resample_state spc_resampler_state;
FILE * debug=0;


/* Initialize the MOD player, with the given mixer settings
   This function returns 0, or -1 if there was an error.
 */

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

    debug=fopen("SDL_Mix_debug.txt", "w");

    snes_spc = spc_new();
    spc_filter = spc_filter_new();
    if ( !snes_spc || !spc_filter )
    {
        Mix_SetError("SNES_SPC: Out of memory");
        return -1;
    }

    spc_t_sample_rate = mixerfmt->freq;
    if(debug!=0) fprintf(debug, "init resampler\n");

    mad_resample_init(&spc_resampler_state, 32000, 44100);

    current_output_channels = mixerfmt->channels;
    current_output_format = mixerfmt->format;

    music_swap8 = 0;
    music_swap16 = 0;
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
            //*mikmod.md_mode = DMODE_16BITS;
        }
        break;

        default: {
            Mix_SetError("Unknown hardware audio format");
            return -1;
        }
    }

    /*
    switch (mixerfmt->format) {

        case AUDIO_U8:
        case AUDIO_S8: {
            if ( mixerfmt->format == AUDIO_S8 ) {
                music_swap8 = 1;
            }
            *mikmod.md_mode = 0;
        }
        break;

        case AUDIO_S16LSB:
        case AUDIO_S16MSB: {
        }
        break;

        default: {
            //Mix_SetError("Unknown hardware audio format");
            //return -1;
        }
    }
    current_output_channels = mixerfmt->channels;
    current_output_format = mixerfmt->format;

    if ( mixerfmt->channels > 1 ) {
        if ( mixerfmt->channels > MAX_OUTPUT_CHANNELS ) {
            Mix_SetError("Hardware uses more channels than mixerfmt");
            return -1;
        }
        *mikmod.md_mode |= DMODE_STEREO;
    }

    *mikmod.md_mixfreq = mixerfmt->freq;
    *mikmod.md_device  = 0;
    *mikmod.md_volume  = 96;
    *mikmod.md_musicvolume = 128;
    *mikmod.md_sndfxvolume = 128;
    *mikmod.md_pansep  = 128;
    *mikmod.md_reverb  = 0;
    *mikmod.md_mode    |= DMODE_HQMIXER|DMODE_SOFT_MUSIC|DMODE_SURROUND;

    list = mikmod.MikSPC_InfoDriver();
    if ( list )
      mikmod.MikSPC_free(list);
    else
      mikmod.MikSPC_RegisterDriver(mikmod.drv_nos);

    list = mikmod.MikSPC_InfoLoader();
    if ( list )
      mikmod.MikSPC_free(list);
    else
      mikmod.MikSPC_RegisterAllLoaders();

    if ( mikmod.MikSPC_Init(NULL) ) {
        Mix_SetError("%s", mikmod.MikSPC_strerror(*mikmod.MikSPC_errno));
        return -1;
    }
    */

    if(debug!=0) fprintf(debug, "SPC Initialized\n");
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

    if(debug!=0) fprintf(debug, "SPC Closed\n");
}

/* Set the volume for a MOD stream */
void SPC_setvolume(SNES_SPC *music, int volume)
{
    if(debug!=0) fprintf(debug, "SPC Set volume\n");
    //mikmod.Player_SetVolume((SWORD)volume);
    //spc_set
}

unsigned char* load_xxxxfile( const char* path, long* size_out )
{
    size_t size;
    unsigned char* data;

    FILE* in = fopen( path, "rb" );
    if ( !in ) return NULL;

    fseek( in, 0, SEEK_END );
    size = ftell( in );
    if ( size_out )
        *size_out = size;
    rewind( in );

    data = (unsigned char*) malloc( size );
    if ( !data ) return NULL;

    if ( fread( data, 1, size, in ) < size ) return NULL;
    fclose( in );

    return data;
}

SNES_SPC *SnesSPC_LoadSongRW(SDL_RWops *src, int maxchan)
{
    if(debug!=0) fprintf(debug, "SPC Load song\n");
    if(src != NULL) {

        /* Seek to 0 bytes from the end of the file */
        Sint64 length = SDL_RWseek(src, 0, RW_SEEK_END);
        SDL_RWseek(src, 0, RW_SEEK_SET);
        if (length < 0)
        {
            Mix_SetError("SNES_SPC: wrong file\n");
            return NULL;
        }

        //sf-28.spc
        //char bytes[length];
        void *bytes;
        long spcsize;
        //SDL_RWread(src, &bytes, sizeof(char), length);
        //if(debug!=0) fprintf(debug, "SNES_SPC: load bytes\n\n");

        bytes = load_xxxxfile("nutz-10.spc", &spcsize);
        char *err = (char*)spc_load_spc( snes_spc, bytes, spcsize );
        //if(debug!=0) fprintf(debug, "SPC loaded, lengiht %I64d, error code\n", spcsize);
        free(bytes);
        return snes_spc;
    }
    return NULL;
}

/* Load a MOD stream from an SDL_RWops object */
SNES_SPC *SPC_new_RW(SDL_RWops *src, int freesrc)
{
    SNES_SPC *spcMusic;

    if(debug!=0) fprintf(debug, "SPC New RW\n");

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
        return NULL;
    }

    spc_is_playing=-1;

    spcMusic = SnesSPC_LoadSongRW(src, 64);
    if (!spcMusic)
    {
        Mix_SetError("SNES_SPC: Can't load file");
        if(debug!=0) fprintf(debug, "SPC can't load file\n");
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
void SPC_play(SNES_SPC *music)
{
    if(debug!=0) fprintf(debug, "SPC play command\n");
    //mikmod.Player_Start(music);
    music=snes_spc;
    spc_is_playing=1;
}

/* Return non-zero if a stream is currently playing */
int SPC_playing(SNES_SPC *music)
{
    if(debug!=0) fprintf(debug, "SPC Playing\n");
    return spc_is_playing;
}


/* Play some of a stream previously started with SPC_play() */
int SPC_playAudio(SNES_SPC *music, Uint8 *stream, int len)
{
    if(debug!=0) fprintf(debug, "SPC paly audio\n");

    if(music==NULL)
        music=snes_spc;

    if(snes_spc==NULL) return 1;
    if(debug!=0) fprintf(debug, "SPC not null\n");

    if(spc_is_playing==-1) return 1;

    if(debug!=0) fprintf(debug, "SPC playing--------\n");
    if(debug!=0) fprintf(debug, "SPC current_output_channels %i\n", current_output_channels);

    short buf[len];
    short nbuf[len];
    Uint8 dstt[len];
    spc_play( snes_spc, len, buf);
    int i;

    int smps = current_output_channels * (spc_sample_count(snes_spc)/8);
    mad_resample_block(&spc_resampler_state, len/(smps), buf, nbuf);

    for(i=0; i<len;i++)
        dstt[i]=(Uint8)nbuf[i];
    memcpy(stream, dstt, len);
    if(debug!=0) fprintf(debug, "SPC grabbed len %i\n", len);

//    if ( music_swap8 )
//    {
//        Uint8 *dst;
//        int i;

//        dst = stream;
//        for ( i=len; i; --i ) {
//            *dst++ ^= 0x80;
//        }
//    }
//    if ( music_swap16 )
//    {
//        Uint8 *dst, tmp;
//        int i;

//        dst = stream;
//        for ( i=(len/2); i; --i ) {
//            tmp = dst[0];
//            dst[0] = dst[1];
//            dst[1] = tmp;
//            dst += 2;
//        }
//    }
    return 0;
}

/* Stop playback of a stream previously started with SPC_play() */
void SPC_stop(SNES_SPC *music)
{
    spc_is_playing=-1;
    //mikmod.Player_Stop();
}

/* Close the given MOD stream */
void SPC_delete(SNES_SPC *music)
{
    SPC_exit();
}

/* Jump (seek) to a given position (time is in seconds) */
void SPC_jump_to_time(SNES_SPC *music, double time)
{
    //mikmod.Player_SetPosition((UWORD)time);
}


#endif
