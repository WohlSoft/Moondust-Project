/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2025 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#include <SDL2/SDL_rwops.h>
#include <SDL2/SDL_audio.h>
#include "audio_flac.h"

#include <FLAC/stream_encoder.h>
#include <FLAC/stream_decoder.h>
#include <FLAC/metadata.h>

#define M_DEC reinterpret_cast<FLAC__StreamDecoder*>(m_flac_dec)

#define M_ENC reinterpret_cast<FLAC__StreamEncoder*>(m_flac_enc)
#define M_ENC_METADATA reinterpret_cast<FLAC__StreamMetadata**>(m_flac_enc_metadata)

struct MDAudioFLAC_callbacks
{
    static FLAC__StreamDecoderReadStatus flac_read_music_cb(
        const FLAC__StreamDecoder *decoder,
        FLAC__byte buffer[],
        size_t *bytes,
        void *client_data
    )
    {
        MDAudioFLAC *data = reinterpret_cast<MDAudioFLAC*>(client_data);
        (void)decoder;

        /* make sure there is something to be reading */
        if(*bytes > 0)
        {
            *bytes = SDL_RWread (data->m_file, buffer, sizeof(FLAC__byte), *bytes);

            if(*bytes == 0) /* error or no data was read (EOF) */
                return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
            else /* data was read, continue */
                return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
        }
        else
            return FLAC__STREAM_DECODER_READ_STATUS_ABORT;
    }

    static FLAC__StreamDecoderSeekStatus flac_seek_music_cb(
        const FLAC__StreamDecoder *decoder,
        FLAC__uint64 absolute_byte_offset,
        void *client_data
    )
    {
        MDAudioFLAC *data = reinterpret_cast<MDAudioFLAC*>(client_data);
        (void)decoder;

        if(SDL_RWseek(data->m_file, (Sint64)absolute_byte_offset, RW_SEEK_SET) < 0)
            return FLAC__STREAM_DECODER_SEEK_STATUS_ERROR;
        else
            return FLAC__STREAM_DECODER_SEEK_STATUS_OK;
    }

    static FLAC__StreamDecoderTellStatus flac_tell_music_cb(
        const FLAC__StreamDecoder *decoder,
        FLAC__uint64 *absolute_byte_offset,
        void *client_data
    )
    {
        MDAudioFLAC *data = reinterpret_cast<MDAudioFLAC*>(client_data);
        Sint64 pos = SDL_RWtell(data->m_file);
        (void)decoder;

        if(pos < 0)
            return FLAC__STREAM_DECODER_TELL_STATUS_ERROR;
        else
        {
            *absolute_byte_offset = (FLAC__uint64)pos;
            return FLAC__STREAM_DECODER_TELL_STATUS_OK;
        }
    }

    static FLAC__StreamDecoderLengthStatus flac_length_music_cb(
        const FLAC__StreamDecoder *decoder,
        FLAC__uint64 *stream_length,
        void *client_data
    )
    {
        MDAudioFLAC *data = reinterpret_cast<MDAudioFLAC*>(client_data);
        Sint64 pos = SDL_RWtell(data->m_file);
        Sint64 length = SDL_RWseek(data->m_file, 0, RW_SEEK_END);
        (void)decoder;

        if(SDL_RWseek(data->m_file, pos, RW_SEEK_SET) != pos || length < 0)
        {
            /* there was an error attempting to return the stream to the original
             * position, or the length was invalid. */
            return FLAC__STREAM_DECODER_LENGTH_STATUS_ERROR;
        }
        else
        {
            *stream_length = (FLAC__uint64)length;
            return FLAC__STREAM_DECODER_LENGTH_STATUS_OK;
        }
    }


    static FLAC__bool flac_eof_music_cb(
        const FLAC__StreamDecoder *decoder,
        void *client_data
    )
    {
        MDAudioFLAC *data = reinterpret_cast<MDAudioFLAC*>(client_data);
        Sint64 pos = SDL_RWtell(data->m_file);
        Sint64 end = SDL_RWseek(data->m_file, 0, RW_SEEK_END);
        (void)decoder;

        /* was the original position equal to the end (a.k.a. the seek didn't move)? */
        if(pos == end)
            return true; /* must be EOF */
        else
        {
            SDL_RWseek(data->m_file, pos, RW_SEEK_SET); /* not EOF, return to the original position */
            return false;
        }
    }

    static FLAC__StreamDecoderWriteStatus flac_write_music_cb(
        const FLAC__StreamDecoder *decoder,
        const FLAC__Frame *frame,
        const FLAC__int32 *const buffer[],
        void *client_data
    )
    {
        MDAudioFLAC *music = reinterpret_cast<MDAudioFLAC*>(client_data);
        Sint16 *data16;
        Sint32 *data32;
        unsigned int i, j, channels;
        int shift_amount = 0, amount;
        (void)decoder;

        switch(music->m_bits_per_sample)
        {
        case 16:
            shift_amount = 0;
            break;
        case 20:
            shift_amount = 4;
            break;
        case 24:
            shift_amount = 8;
            break;
        default:
            music->m_lastError = "FLAC decoder doesn't support bits_per_sample: " + std::to_string(SDL_AUDIO_BITSIZE(music->m_spec.m_sample_format));
            return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
        }

        if (music->m_spec.m_channels == 3)
            /* We'll just drop the centre channel for now */
            channels = 2;
        else
            channels = music->m_spec.m_channels;

        if(music->m_spec.m_sample_format == AUDIO_S32SYS)
        {
            data32 = SDL_stack_alloc(Sint32, (frame->header.blocksize * channels));
            if(!data32)
            {
                music->m_lastError = "Couldn't allocate bytes stack memory: " + std::to_string((int)frame->header.blocksize * channels * sizeof(*data32));
                return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
            }

            if(music->m_spec.m_channels == 3)
            {
                Sint32 *dst = data32;

                for(i = 0; i < frame->header.blocksize; ++i)
                {
                    Sint32 FL = (Sint32)(buffer[0][i] << shift_amount);
                    Sint32 FR = (Sint32)(buffer[1][i] << shift_amount);
                    Sint32 FCmix = (Sint32)(buffer[2][i] << shift_amount) * 0.5f;
                    int sample;

                    sample = (FL + FCmix);
                    if (sample > SDL_MAX_SINT32)
                        *dst = SDL_MAX_SINT32;
                    else if (sample < SDL_MIN_SINT32)
                        *dst = SDL_MIN_SINT32;
                    else
                        *dst = (Sint32)sample;
                    ++dst;

                    sample = (FR + FCmix);
                    if (sample > SDL_MAX_SINT32)
                        *dst = SDL_MAX_SINT32;
                    else if (sample < SDL_MIN_SINT32)
                        *dst = SDL_MIN_SINT32;
                    else
                        *dst = (Sint32)sample;
                    ++dst;
                }
            }
            else
            {
                for(i = 0; i < channels; ++i)
                {
                    Sint32 *dst = data32 + i;
                    for(j = 0; j < frame->header.blocksize; ++j)
                    {
                        *dst = (Sint32)(buffer[i][j] << shift_amount);
                        dst += channels;
                    }
                }
            }

            amount = (int)(frame->header.blocksize * channels * sizeof(*data32));

            if(music->m_io_buffer.size() < (size_t)amount)
                music->m_io_buffer.resize(amount);

            SDL_memcpy(music->m_io_buffer.data(), data32, amount);
            SDL_stack_free(data32);
        }
        else
        {
            data16 = SDL_stack_alloc(Sint16, (frame->header.blocksize * channels));
            if(!data16)
            {
                music->m_lastError = "Couldn't allocate bytes stack memory: " + std::to_string((int)frame->header.blocksize * channels * sizeof(*data16));
                return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
            }

            if(music->m_spec.m_channels == 3)
            {
                Sint16 *dst = data16;

                for(i = 0; i < frame->header.blocksize; ++i)
                {
                    Sint16 FL = (Sint16)(buffer[0][i] >> shift_amount);
                    Sint16 FR = (Sint16)(buffer[1][i] >> shift_amount);
                    Sint16 FCmix = (Sint16)((buffer[2][i] >> shift_amount) * 0.5f);
                    int sample;

                    sample = (FL + FCmix);
                    if (sample > SDL_MAX_SINT16)
                        *dst = SDL_MAX_SINT16;
                    else if (sample < SDL_MIN_SINT16)
                        *dst = SDL_MIN_SINT16;
                    else
                        *dst = (Sint16)sample;
                    ++dst;

                    sample = (FR + FCmix);
                    if (sample > SDL_MAX_SINT16)
                        *dst = SDL_MAX_SINT16;
                    else if (sample < SDL_MIN_SINT16)
                        *dst = SDL_MIN_SINT16;
                    else
                        *dst = (Sint16)sample;
                    ++dst;
                }
            }
            else
            {
                for(i = 0; i < channels; ++i)
                {
                    Sint16 *dst = data16 + i;
                    for(j = 0; j < frame->header.blocksize; ++j)
                    {
                        *dst = (Sint16)(buffer[i][j] >> shift_amount);
                        dst += channels;
                    }
                }
            }

            amount = (int)(frame->header.blocksize * channels * sizeof(*data16));

            if(music->m_io_buffer.size() < (size_t)amount)
                music->m_io_buffer.resize(amount);

            SDL_memcpy(music->m_io_buffer.data(), data16, amount);
            SDL_stack_free(data16);
        }

        music->m_buffer_left = amount;
        music->m_buffer_pos = 0;

        return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
    }

    static void flac_metadata_music_cb(
        const FLAC__StreamDecoder *decoder,
        const FLAC__StreamMetadata *metadata,
        void *client_data
    )
    {
        MDAudioFLAC *music = reinterpret_cast<MDAudioFLAC*>(client_data);
        const FLAC__StreamMetadata_VorbisComment *vc;
        unsigned rate;
        char *param, *argument, *value;
        SDL_bool is_loop_length = SDL_FALSE;

        (void)decoder;

        if(metadata->type == FLAC__METADATA_TYPE_STREAMINFO)
        {
            music->m_spec.m_sample_rate = metadata->data.stream_info.sample_rate;
            music->m_spec.m_channels = metadata->data.stream_info.channels;
            music->m_bits_per_sample = metadata->data.stream_info.bits_per_sample;

            switch(music->m_bits_per_sample)
            {
            case 16:
                music->m_spec.m_sample_format = AUDIO_S16SYS;
                break;
            case 20:
                music->m_spec.m_sample_format = AUDIO_S16SYS;
                break;
            case 24:
                music->m_spec.m_sample_format = AUDIO_S32SYS;
                break;
            default:
                // Fail
                break;
            }

            /*printf("FLAC: Sample rate = %d, channels = %d, bits_per_sample = %d\n", music->sample_rate, music->channels, music->bits_per_sample);*/
        }
        else if(metadata->type == FLAC__METADATA_TYPE_VORBIS_COMMENT)
        {
            FLAC__uint32 i;

            vc  = &metadata->data.vorbis_comment;
            rate = music->m_spec.m_sample_rate;

            for(i = 0; i < vc->num_comments; ++i)
            {
                param = SDL_strdup((const char *) vc->comments[i].entry);
                argument = param;
                value = SDL_strchr(param, '=');

                if(value == NULL)
                    value = param + SDL_strlen(param);
                else
                    *(value++) = '\0';

                /* Want to match LOOP-START, LOOP_START, etc. Remove - or _ from
                 * string if it is present at position 4. */
                if(music->isLoopTag(argument) && ((argument[4] == '_') || (argument[4] == '-')))
                    SDL_memmove(argument + 4, argument + 5, SDL_strlen(argument) - 4);

                if(SDL_strcasecmp(argument, "LOOPSTART") == 0)
                    music->m_spec.m_loop_start = music->parseTime(value, rate);
                else if(SDL_strcasecmp(argument, "LOOPLENGTH") == 0)
                {
                    music->m_spec.m_loop_len = SDL_strtoll(value, NULL, 10);
                    is_loop_length = SDL_TRUE;
                }
                else if(SDL_strcasecmp(argument, "LOOPEND") == 0)
                {
                    music->m_spec.m_loop_end = music->parseTime(value, rate);
                    is_loop_length = SDL_FALSE;
                }
                else if(SDL_strcasecmp(argument, "TITLE") == 0)
                    music->m_spec.m_meta_title = value;
                else if(SDL_strcasecmp(argument, "ARTIST") == 0)
                    music->m_spec.m_meta_artist = value;
                else if(SDL_strcasecmp(argument, "ALBUM") == 0)
                    music->m_spec.m_meta_album = value;
                else if(SDL_strcasecmp(argument, "COPYRIGHT") == 0)
                    music->m_spec.m_meta_copyright = value;

                SDL_free(param);
            }

            if(is_loop_length)
                music->m_spec.m_loop_end = music->m_spec.m_loop_start + music->m_spec.m_loop_len;
            else
                music->m_spec.m_loop_len = music->m_spec.m_loop_end - music->m_spec.m_loop_start;

            /* Ignore invalid loop tag */
            if(music->m_spec.m_loop_start < 0 || music->m_spec.m_loop_len < 0 || music->m_spec.m_loop_end < 0)
            {
                music->m_spec.m_loop_start = 0;
                music->m_spec.m_loop_len = 0;
                music->m_spec.m_loop_end = 0;
            }
        }
    }

    static void flac_error_music_cb(
        const FLAC__StreamDecoder *decoder,
        FLAC__StreamDecoderErrorStatus status,
        void *client_data
    )
    {
        MDAudioFLAC *music = reinterpret_cast<MDAudioFLAC*>(client_data);
        (void)decoder;
        (void)client_data;

        /* print an SDL error based on the error status */
        switch (status) {
        case FLAC__STREAM_DECODER_ERROR_STATUS_LOST_SYNC:
            music->m_lastError = "Error processing the FLAC file [LOST_SYNC].";
            break;
        case FLAC__STREAM_DECODER_ERROR_STATUS_BAD_HEADER:
            music->m_lastError = "Error processing the FLAC file [BAD_HEADER].";
            break;
        case FLAC__STREAM_DECODER_ERROR_STATUS_FRAME_CRC_MISMATCH:
            music->m_lastError = "Error processing the FLAC file [CRC_MISMATCH].";
            break;
        case FLAC__STREAM_DECODER_ERROR_STATUS_UNPARSEABLE_STREAM:
            music->m_lastError = "Error processing the FLAC file [UNPARSEABLE].";
            break;
        default:
            music->m_lastError = "Error processing the FLAC file [UNKNOWN].";
            break;
        }
    }


    static FLAC__StreamEncoderReadStatus flac_encode_read_cb(const FLAC__StreamEncoder *encoder,
                                                             FLAC__byte buffer[],
                                                             size_t *bytes,
                                                             void *client_data)
    {
        MDAudioFLAC *data = reinterpret_cast<MDAudioFLAC*>(client_data);
        (void)encoder;

        /* make sure there is something to be reading */
        if(*bytes > 0)
        {
            *bytes = SDL_RWread (data->m_file, buffer, sizeof(FLAC__byte), *bytes);

            if(*bytes == 0) /* error or no data was read (EOF) */
                return FLAC__STREAM_ENCODER_READ_STATUS_END_OF_STREAM;
            else /* data was read, continue */
                return FLAC__STREAM_ENCODER_READ_STATUS_CONTINUE;
        }
        else
            return FLAC__STREAM_ENCODER_READ_STATUS_ABORT;
    }


    static FLAC__StreamEncoderWriteStatus flac_encode_write_cb(const FLAC__StreamEncoder *encoder,
                                                        const FLAC__byte buffer[], size_t bytes,
                                                        uint32_t samples, uint32_t current_frame,
                                                        void *client_data)
    {
        MDAudioFLAC *data = reinterpret_cast<MDAudioFLAC*>(client_data);
        size_t ret;
        (void)encoder;
        (void)samples;
        (void)current_frame;

        if(bytes > 0)
        {
            ret = SDL_RWwrite(data->m_file, buffer, sizeof(FLAC__byte), bytes);
            if(ret == 0)
            {
                data->m_lastError = "Failed to write: ";
                data->m_lastError += SDL_GetError();
                return FLAC__STREAM_ENCODER_WRITE_STATUS_FATAL_ERROR;
            }
            else
                return FLAC__STREAM_ENCODER_WRITE_STATUS_OK;
        }
        else
            return FLAC__STREAM_ENCODER_WRITE_STATUS_FATAL_ERROR;
    }


    static FLAC__StreamEncoderSeekStatus flac_encode_seek_cb(const FLAC__StreamEncoder *encoder,
                                                      FLAC__uint64 absolute_byte_offset,
                                                      void *client_data)
    {
        MDAudioFLAC *data = reinterpret_cast<MDAudioFLAC*>(client_data);
        (void)encoder;

        if(SDL_RWseek(data->m_file, (Sint64)absolute_byte_offset, RW_SEEK_SET) < 0)
            return FLAC__STREAM_ENCODER_SEEK_STATUS_ERROR;
        else
            return FLAC__STREAM_ENCODER_SEEK_STATUS_OK;
    }

    static FLAC__StreamEncoderTellStatus flac_encode_tell_cb(const FLAC__StreamEncoder *encoder,
                                                              FLAC__uint64 *absolute_byte_offset,
                                                              void *client_data)
    {
        MDAudioFLAC *data = reinterpret_cast<MDAudioFLAC*>(client_data);
        Sint64 pos = SDL_RWtell(data->m_file);
        (void)encoder;

        if(pos < 0)
            return FLAC__STREAM_ENCODER_TELL_STATUS_ERROR;
        else
        {
            *absolute_byte_offset = (FLAC__uint64)pos;
            return FLAC__STREAM_ENCODER_TELL_STATUS_OK;
        }
    }

    static void flac_encode_metadata_cb(const FLAC__StreamEncoder *encoder, const FLAC__StreamMetadata *metadata, void *client_data)
    {
        (void)encoder;
        (void)metadata;
        (void)client_data;
    }
};

MDAudioFLAC::MDAudioFLAC(EncodeContainer container) :
    MDAudioFile(),
    m_container(container)
{}

MDAudioFLAC::~MDAudioFLAC()
{
    MDAudioFLAC::close();
}

uint32_t MDAudioFLAC::getCodecSpec() const
{
    return SPEC_READ | SPEC_WRITE | SPEC_LOOP_POINTS | SPEC_META_TAGS;
}

bool MDAudioFLAC::openRead(SDL_RWops *file)
{
    FLAC__StreamDecoder *dec;
    Uint8 magic[4];
    int was_error = 1;
    bool is_ogg_flac;

    close();

    m_write = false;
    m_file = file;
    m_init_state = 0;

    if(SDL_RWread(m_file, magic, 1, 4) != 4)
    {
        m_lastError = "Couldn't read first 4 bytes of audio data";
        return false;
    }

    SDL_RWseek(m_file, -4, RW_SEEK_CUR);
    is_ogg_flac = (SDL_memcmp(magic, "OggS", 4) == 0);

    m_flac_dec = dec = FLAC__stream_decoder_new();

    if(dec)
    {
        FLAC__StreamDecoderInitStatus ret;
        m_init_state++; /* stage 1! */
        FLAC__stream_decoder_set_metadata_respond(dec, FLAC__METADATA_TYPE_VORBIS_COMMENT);

        if(is_ogg_flac)
        {
            ret = FLAC__stream_decoder_init_ogg_stream(dec,
                MDAudioFLAC_callbacks::flac_read_music_cb, MDAudioFLAC_callbacks::flac_seek_music_cb,
                MDAudioFLAC_callbacks::flac_tell_music_cb, MDAudioFLAC_callbacks::flac_length_music_cb,
                MDAudioFLAC_callbacks::flac_eof_music_cb, MDAudioFLAC_callbacks::flac_write_music_cb,
                MDAudioFLAC_callbacks::flac_metadata_music_cb, MDAudioFLAC_callbacks::flac_error_music_cb,
                this);
        }
        else
        {
            ret = FLAC__stream_decoder_init_stream(dec,
                MDAudioFLAC_callbacks::flac_read_music_cb, MDAudioFLAC_callbacks::flac_seek_music_cb,
                MDAudioFLAC_callbacks::flac_tell_music_cb, MDAudioFLAC_callbacks::flac_length_music_cb,
                MDAudioFLAC_callbacks::flac_eof_music_cb, MDAudioFLAC_callbacks::flac_write_music_cb,
                MDAudioFLAC_callbacks::flac_metadata_music_cb, MDAudioFLAC_callbacks::flac_error_music_cb,
                this);
        }

        if(ret == FLAC__STREAM_DECODER_INIT_STATUS_OK)
        {
            m_init_state++; /* stage 2! */

            if(FLAC__stream_decoder_process_until_end_of_metadata(dec))
                was_error = 0;
            else
                m_lastError = "FLAC__stream_decoder_process_until_end_of_metadata() failed";
        }
        else
            m_lastError = "FLAC__stream_decoder_init_stream() failed";
    }
    else
        m_lastError = "FLAC__stream_decoder_new() failed";

    if(was_error)
    {
        switch(m_init_state)
        {
        case 2:
            FLAC__stream_decoder_finish(dec); /* fallthrough */
        case 1:
            FLAC__stream_decoder_delete(dec); /* fallthrough */
        case 0:
            close();
            break;
        }
        return false;
    }

    m_spec.m_total_length = FLAC__stream_decoder_get_total_samples(dec);

    return true;
}

bool MDAudioFLAC::openWrite(SDL_RWops *file, const MDAudioFileSpec &dstSpec)
{
    FLAC__StreamEncoder *encoder = 0;
    FLAC__StreamEncoderInitStatus init_status = FLAC__STREAM_ENCODER_INIT_STATUS_ENCODER_ERROR;
    FLAC__StreamMetadata *metadata[2];
    FLAC__StreamMetadata_VorbisComment_Entry entry;
    std::string from_num;

    close();

    m_write = true;
    m_file = file;
    m_init_state = 0;

    m_spec = dstSpec;

    if(m_spec.m_sample_format == AUDIO_S16LSB || m_spec.m_sample_format == AUDIO_S16MSB)
    {
        m_spec.m_sample_format = AUDIO_S16SYS; // Accepting only system endianess
        m_bits_per_sample = 16;
    }
    else if(m_spec.m_sample_format == AUDIO_S32LSB || m_spec.m_sample_format == AUDIO_S32MSB)
    {
        m_spec.m_sample_format = AUDIO_S32SYS; // Accepting only system endianess
        m_bits_per_sample = 24;
    }
    else if(m_spec.m_sample_format == AUDIO_F32MSB || m_spec.m_sample_format == AUDIO_F32LSB)
    {
        m_spec.m_sample_format = AUDIO_S32SYS;
        m_bits_per_sample = 24;
    }
    else
    {
        m_spec.m_sample_format = AUDIO_S16SYS;
        m_bits_per_sample = 16;
    }

    m_flac_enc = encoder = FLAC__stream_encoder_new();
    if(!encoder)
    {
        m_lastError = "Can't allocate FLAC encoder";
        close();
        return false;
    }

    ++m_init_state; // 1

    if(!FLAC__stream_encoder_set_verify(encoder, true))
    {
        m_lastError = "Failed FLAC__stream_encoder_set_verify";
        close();
        return false;
    }


    if(m_spec.compression < 0)
        m_spec.compression = 5;
    else if(m_spec.compression > 8)
        m_spec.compression = 8;

    if(!FLAC__stream_encoder_set_compression_level(encoder, m_spec.compression))
    {
        m_lastError = "Failed FLAC__stream_encoder_set_compression_level";
        close();
        return false;
    }

    if(!FLAC__stream_encoder_set_channels(encoder, m_spec.m_channels))
    {
        m_lastError = "Failed FLAC__stream_encoder_set_channels";
        close();
        return false;
    }

    if(!FLAC__stream_encoder_set_bits_per_sample(encoder, m_bits_per_sample))
    {
        m_lastError = "Failed FLAC__stream_encoder_set_bits_per_sample";
        close();
        return false;
    }

    if(!FLAC__stream_encoder_set_sample_rate(encoder, m_spec.m_sample_rate))
    {
        m_lastError = "Failed FLAC__stream_encoder_set_sample_rate";
        close();
        return false;
    }

    if(!FLAC__stream_encoder_set_total_samples_estimate(encoder, m_spec.m_total_length))
    {
        m_lastError = "Failed FLAC__stream_encoder_set_total_samples_estimate";
        close();
        return false;
    }

    m_flac_enc_metadata[0] = metadata[0] = FLAC__metadata_object_new(FLAC__METADATA_TYPE_VORBIS_COMMENT);

    if(!metadata[0])
    {
        m_lastError = "Failed to allocate FLAC metadata 0";
        close();
        return false;
    }

    ++m_init_state; // 2
    m_flac_enc_metadata[1] = metadata[1] = FLAC__metadata_object_new(FLAC__METADATA_TYPE_PADDING);

    if(!metadata[1])
    {
        m_lastError = "Failed to allocate FLAC metadata 1";
        close();
        return false;
    }

    ++m_init_state; // 3

    if(!m_spec.m_meta_title.empty())
    {
        FLAC__metadata_object_vorbiscomment_entry_from_name_value_pair(&entry, "TITLE", m_spec.m_meta_title.c_str());
        FLAC__metadata_object_vorbiscomment_append_comment(metadata[0], entry, /*copy=*/false);
    }

    if(!m_spec.m_meta_artist.empty())
    {
        FLAC__metadata_object_vorbiscomment_entry_from_name_value_pair(&entry, "ARTIST", m_spec.m_meta_artist.c_str());
        FLAC__metadata_object_vorbiscomment_append_comment(metadata[0], entry, /*copy=*/false);
    }

    if(!m_spec.m_meta_album.empty())
    {
        FLAC__metadata_object_vorbiscomment_entry_from_name_value_pair(&entry, "ALBUM", m_spec.m_meta_album.c_str());
        FLAC__metadata_object_vorbiscomment_append_comment(metadata[0], entry, /*copy=*/false);
    }

    if(!m_spec.m_meta_copyright.empty())
    {
        FLAC__metadata_object_vorbiscomment_entry_from_name_value_pair(&entry, "COPYRIGHT", m_spec.m_meta_copyright.c_str());
        FLAC__metadata_object_vorbiscomment_append_comment(metadata[0], entry, /*copy=*/false);
    }

    if(m_spec.m_loop_end > 0 && m_spec.m_loop_end > m_spec.m_loop_start)
    {
        from_num = std::to_string(m_spec.m_loop_start);
        FLAC__metadata_object_vorbiscomment_entry_from_name_value_pair(&entry, "LOOPSTART", from_num.c_str());
        FLAC__metadata_object_vorbiscomment_append_comment(metadata[0], entry, /*copy=*/false);

        from_num = std::to_string(m_spec.m_loop_end);
        FLAC__metadata_object_vorbiscomment_entry_from_name_value_pair(&entry, "LOOPEND", from_num.c_str());
        FLAC__metadata_object_vorbiscomment_append_comment(metadata[0], entry, /*copy=*/false);
    }

    if(!FLAC__stream_encoder_set_metadata(encoder, metadata, 2))
    {
        m_lastError = "Failed to set FLAC metadata";
        close();
        return false;
    }


    if(m_container == ENCODE_FLAC_NATIVE)
    {
        init_status = FLAC__stream_encoder_init_stream(encoder,
                                                       MDAudioFLAC_callbacks::flac_encode_write_cb,
                                                       MDAudioFLAC_callbacks::flac_encode_seek_cb,
                                                       MDAudioFLAC_callbacks::flac_encode_tell_cb,
                                                       MDAudioFLAC_callbacks::flac_encode_metadata_cb,
                                                       this);
    }
    else if(m_container == ENCODE_FLAC_OGG)
    {
        init_status = FLAC__stream_encoder_init_ogg_stream(encoder,
                                                           MDAudioFLAC_callbacks::flac_encode_read_cb,
                                                           MDAudioFLAC_callbacks::flac_encode_write_cb,
                                                           MDAudioFLAC_callbacks::flac_encode_seek_cb,
                                                           MDAudioFLAC_callbacks::flac_encode_tell_cb,
                                                           MDAudioFLAC_callbacks::flac_encode_metadata_cb,
                                                           this);
    }

    if(init_status != FLAC__STREAM_ENCODER_INIT_STATUS_OK)
    {
        m_lastError = "Failed to initialize the FLAC encoder: ";
        m_lastError += FLAC__StreamEncoderInitStatusString[init_status];
        close();
        return false;
    }

    ++m_init_state; // 4

    return true;
}

bool MDAudioFLAC::close()
{
    if(m_write)
    {
        switch(m_init_state)
        {
        case 4:
            FLAC__stream_encoder_finish(M_ENC); /* fallthrough */
        case 3:
            FLAC__metadata_object_delete(M_ENC_METADATA[1]); /* fallthrough */
        case 2:
            FLAC__metadata_object_delete(M_ENC_METADATA[0]); /* fallthrough */
        case 1:
            FLAC__stream_encoder_delete(M_ENC); /* fallthrough */
        case 0:
            m_flac_enc = nullptr;
            m_flac_enc_metadata[0] = nullptr;
            m_flac_enc_metadata[1] = nullptr;
            break;
        }
    }
    else
    {
        switch(m_init_state)
        {
        case 2:
            FLAC__stream_decoder_finish(M_DEC); /* fallthrough */
        case 1:
            FLAC__stream_decoder_delete(M_DEC); /* fallthrough */
        case 0:
            m_flac_dec = nullptr;
            break;
        }
    }

    m_init_state = 0;
    m_io_buffer.clear();
    m_buffer_left = 0;
    m_buffer_pos = 0;

    m_write = false;
    m_file = nullptr;

    return true;
}

bool MDAudioFLAC::readRewind()
{
    if(!FLAC__stream_decoder_seek_absolute(M_DEC, 0))
    {
        if(FLAC__stream_decoder_get_state(M_DEC) == FLAC__STREAM_DECODER_SEEK_ERROR)
            FLAC__stream_decoder_flush(M_DEC);

        m_lastError = "Seeking of FLAC stream failed: libFLAC seek failed.";
        return false;
    }

    FLAC__stream_decoder_reset(M_DEC);

    m_buffer_left = 0;
    m_buffer_pos = 0;

    return true;
}

size_t MDAudioFLAC::readChunk(uint8_t *out, size_t outSize, bool */*spec_changed*/)
{
    size_t out_offset = 0;
    size_t written = 0;

retry:
    if(m_buffer_left == 0)
    {
        if(!FLAC__stream_decoder_process_single(M_DEC))
        {
            m_lastError = "FLAC__stream_decoder_process_single() failed";
            return 0;
        }
    }

    if(m_buffer_left > 0 && outSize > 0)
    {
        if(outSize <= m_buffer_left)
        {
            SDL_memcpy(out + out_offset, m_io_buffer.data() + m_buffer_pos, outSize);
            written += outSize;
            out_offset += outSize;
            m_buffer_left -= outSize;
            m_buffer_pos += outSize;
        }
        else
        {
            SDL_memcpy(out + out_offset, m_io_buffer.data() + m_buffer_pos, m_buffer_left);
            outSize -= m_buffer_left;
            written += m_buffer_left;
            out_offset += m_buffer_left;
            m_buffer_left -= m_buffer_left;
            m_buffer_pos += m_buffer_left;
            goto retry;
        }
    }

    if(written > 0)
        return written;
    else if(FLAC__stream_decoder_get_state(M_DEC) == FLAC__STREAM_DECODER_END_OF_STREAM)
        return 0;

    goto retry;
}

size_t MDAudioFLAC::writeChunk(uint8_t *in, size_t inSize)
{
    size_t sample_shift = 0, in_frame_size, frames_num, samples_num, i;
    FLAC__bool ret;

    if(m_spec.m_sample_format == AUDIO_S32SYS)
    {
        in_frame_size = sizeof(int32_t) * m_spec.m_channels;
        samples_num = inSize / sizeof(int32_t);
    }
    else
    {
        in_frame_size = sizeof(int16_t) * m_spec.m_channels;
        samples_num = inSize / sizeof(int16_t);
    }

    frames_num = inSize / in_frame_size;

    switch(m_bits_per_sample)
    {
    case 16:
        sample_shift = 0;
        break;
    case 24:
        sample_shift = 8;
        break;
    default:
        m_lastError = "Unsupported bits per sample: " + std::to_string(m_bits_per_sample);
        return 0;
    }

    if(m_io_buffer.size() < sizeof(FLAC__int32) * samples_num)
        m_io_buffer.resize(sizeof(FLAC__int32) * samples_num);

    FLAC__int32 *out_buff = reinterpret_cast<FLAC__int32 *>(m_io_buffer.data());

    if(m_spec.m_sample_format == AUDIO_S32SYS)
    {
        FLAC__int32 *cur_out = out_buff;
        int32_t *cur_in = reinterpret_cast<int32_t *>(in);
        for(i = 0; i < samples_num; ++i)
            *(cur_out++) = *(cur_in++) >> sample_shift;
    }
    else
    {
        FLAC__int32 *cur_out = out_buff;
        int16_t *cur_in = reinterpret_cast<int16_t *>(in);
        for(i = 0; i < samples_num; ++i)
            *(cur_out++) = static_cast<int32_t>(*(cur_in++)) << sample_shift;
    }

    ret = FLAC__stream_encoder_process_interleaved(M_ENC, out_buff, frames_num);
    if(!ret)
    {
        m_lastError += "; FLAC state: ";
        m_lastError += FLAC__StreamEncoderStateString[FLAC__stream_encoder_get_state(M_ENC)];
        return 0;
    }

    return inSize;
}
