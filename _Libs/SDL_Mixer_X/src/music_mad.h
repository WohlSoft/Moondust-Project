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

#ifdef MP3_MAD_MUSIC

#include "audio_codec.h"

#include "mad.h"
#include <SDL2/SDL_rwops.h>
#include <SDL2/SDL_audio.h>
#include <SDL_mixer_ext/SDL_mixer_ext.h>
#include "resample/my_resample.h"

#define MAD_INPUT_BUFFER_SIZE   (5*8192)
#define MAD_OUTPUT_BUFFER_SIZE  8192

enum
{
    MS_input_eof    = 0x0001,
    MS_input_error  = 0x0001,
    MS_decode_eof   = 0x0002,
    MS_decode_error = 0x0004,
    MS_error_flags  = 0x000f,

    MS_playing      = 0x0100,
    MS_cvt_decoded  = 0x0200
};

typedef struct
{
    SDL_RWops *src;
    /*! Begin position at begin of file, 0 by default, It exists here, because need to don't feed libMAD with ID3s which causes junk at begin! */
    int src_begin_pos;
    int freesrc;
    struct mad_stream stream;
    struct mad_frame frame;
    struct mad_synth synth;
    int frames_read;
    mad_timer_t next_frame_start;
    int volume;
    int status;
    int output_begin, output_end;
    int total_length;
    int sample_rate;
    int sample_position;
    SDL_AudioSpec mixer;
    SDL_AudioCVT cvt;
    int   len_available;
    Uint8 *snd_available;

    unsigned char input_buffer[MAD_INPUT_BUFFER_SIZE + MAD_BUFFER_GUARD];
    unsigned char output_buffer[MAD_OUTPUT_BUFFER_SIZE];

    char *mus_title;
    char *mus_artist;
    char *mus_album;
    char *mus_copyright;
    struct MyResampler resample;
} mad_data;

extern int MAD_init2(AudioCodec* codec, SDL_AudioSpec *mixerfmt);

/*
    Access to those functions is kept because this API is used in
    the chunks loading sub-system

    TODO: Chunks API also must use audio codec thing too,
          and make those functions (and structures above!!!) be static and private
*/

mad_data *mad_openFileRW(SDL_RWops *src, SDL_AudioSpec *mixer, int freesrc);
void    mad_closeFile(void *mp3_mad_p);

void    mad_start(void *mp3_mad_p);
void    mad_stop(void *mp3_mad_p);
int     mad_isPlaying(void *mp3_mad_p);

int     mad_getSamples(void *mp3_mad_p, Uint8 *stream, int len);
void    mad_seek(void *mp3_mad_p, double position);
double  mad_tell(void *mp3_mad_p);
double  mad_total(void *mp3_mad_p);
void    mad_setVolume(void *mp3_mad_p, int volume);

#endif
