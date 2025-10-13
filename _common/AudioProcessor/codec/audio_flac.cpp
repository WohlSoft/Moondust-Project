#include <SDL2/SDL_rwops.h>
#include <SDL2/SDL_audio.h>
#include "audio_flac.h"

#include <FLAC/stream_encoder.h>
#include <FLAC/stream_decoder.h>

#define M_ENC reinterpret_cast<FLAC__StreamEncoder*>(m_flac_enc)
#define M_DEC reinterpret_cast<FLAC__StreamDecoder*>(m_flac_dec)

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
        int channels;
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

};

MDAudioFLAC::MDAudioFLAC() :
    MDAudioFile()
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
    return false;
}

bool MDAudioFLAC::close()
{
    if(m_write)
    {

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
            m_flac_dec = 0;
            break;
        }

        m_init_state = 0;
    }

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

size_t MDAudioFLAC::readChunk(uint8_t *out, size_t outSize, bool *spec_changed)
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
    return false;
}
