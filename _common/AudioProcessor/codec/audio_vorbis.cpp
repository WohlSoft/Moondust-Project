#include <string.h>
#include <time.h>
#include <SDL2/SDL_rwops.h>
#include <SDL2/SDL_audio.h>
#include "audio_vorbis.h"

#include <vorbis/vorbisfile.h>
#include <vorbis/vorbisenc.h>

struct MDAudioVorbis_private
{
    // Vorbis Read
    OggVorbis_File m_vf;
    vorbis_info m_vi;
    int m_section = -1;

    // Vorbis Write
    ogg_stream_state m_os;    //!< take physical pages, weld into a logical stream of packets
    ogg_page         m_og;    //!< one Ogg bitstream page.  Vorbis packets are inside
    ogg_packet       m_op;    //!< one raw packet of data for decode
    vorbis_comment   m_vc;    //!< struct that stores all the user comments
    vorbis_dsp_state m_vd;    //!< central working state for the packet->PCM decoder
    vorbis_block     m_vb;    //!< local working space for packet->PCM decode
};

int MDAudioVorbis::set_ov_error(const char *function, int error)
{
#define HANDLE_ERROR_CASE(X) case X: m_lastError = std::string(function) + ": " + std::string(#X); break;
    switch(error)
    {
        HANDLE_ERROR_CASE(OV_FALSE)
        HANDLE_ERROR_CASE(OV_EOF)
        HANDLE_ERROR_CASE(OV_HOLE)
        HANDLE_ERROR_CASE(OV_EREAD)
        HANDLE_ERROR_CASE(OV_EFAULT)
        HANDLE_ERROR_CASE(OV_EIMPL)
        HANDLE_ERROR_CASE(OV_EINVAL)
        HANDLE_ERROR_CASE(OV_ENOTVORBIS)
        HANDLE_ERROR_CASE(OV_EBADHEADER)
        HANDLE_ERROR_CASE(OV_EVERSION)
        HANDLE_ERROR_CASE(OV_ENOTAUDIO)
        HANDLE_ERROR_CASE(OV_EBADPACKET)
        HANDLE_ERROR_CASE(OV_EBADLINK)
        HANDLE_ERROR_CASE(OV_ENOSEEK)
    default:
        m_lastError = std::string(function) + ": unknown error " + std::to_string(error);
        break;
    }
    return -1;
}

bool MDAudioVorbis::updateSection()
{
    vorbis_info *vi;

    vi = ov_info(&p->m_vf, -1);
    if(!vi)
    {
        m_lastError = "ov_info returned NULL";
        return false;
    }

    if(vi->channels == p->m_vi.channels && vi->rate == p->m_vi.rate)
        return true;

    SDL_memcpy(&p->m_vi, vi, sizeof(*vi));

    m_spec.m_sample_rate = p->m_vi.rate;
    m_spec.m_channels = p->m_vi.channels;

    return true;
}

void MDAudioVorbis::writeFlush()
{
    int eos = 0;

    while(vorbis_analysis_blockout(&p->m_vd, &p->m_vb) == 1)
    {
        /* analysis, assume we want to use bitrate management */
        vorbis_analysis(&p->m_vb, nullptr);
        vorbis_bitrate_addblock(&p->m_vb);

        while(vorbis_bitrate_flushpacket(&p->m_vd, &p->m_op))
        {
            /* weld the packet into the bitstream */
            ogg_stream_packetin(&p->m_os, &p->m_op);

            /* write out pages (if any) */
            while(!eos)
            {
                int result = ogg_stream_pageout(&p->m_os, &p->m_og);
                if(result == 0)
                    break;

                SDL_RWwrite(m_file, p->m_og.header, 1, p->m_og.header_len);
                SDL_RWwrite(m_file, p->m_og.body, 1, p->m_og.body_len);

                /* this could be set above, but for illustrative purposes, I do
                   it here (to show that vorbis does know where the stream ends) */
                if(ogg_page_eos(&p->m_og))
                    eos = 1;
            }
        }
    }
}

MDAudioVorbis::MDAudioVorbis() :
    MDAudioFile(),
    p(new MDAudioVorbis_private)
{}

MDAudioVorbis::~MDAudioVorbis()
{
    MDAudioVorbis::close();
    delete p;
}

uint32_t MDAudioVorbis::getCodecSpec() const
{
    return SPEC_READ | SPEC_WRITE | SPEC_LOOP_POINTS | SPEC_META_TAGS;
}

static size_t sdl_read_func(void *ptr, size_t size, size_t nmemb, void *datasource)
{
    return SDL_RWread((SDL_RWops*)datasource, ptr, size, nmemb);
}

static int sdl_seek_func(void *datasource, ogg_int64_t offset, int whence)
{
    return (SDL_RWseek((SDL_RWops*)datasource, offset, whence) < 0)? -1 : 0;
}

static long sdl_tell_func(void *datasource)
{
    return (long)SDL_RWtell((SDL_RWops*)datasource);
}

static int sdl_close_func(void *datasource)
{
    (void)datasource;
    return 0;
}

bool MDAudioVorbis::openRead(SDL_RWops *file)
{
    ov_callbacks callbacks;
    vorbis_comment *vc = nullptr;
    SDL_bool is_loop_length = SDL_FALSE;

    close();

    m_write = false;
    m_file = file;

    p->m_section = -1;
    memset(&p->m_vf, 0, sizeof(p->m_vf));
    memset(&p->m_vi, 0, sizeof(p->m_vi));

    callbacks.read_func = sdl_read_func;
    callbacks.seek_func = sdl_seek_func;
    callbacks.close_func = sdl_close_func;
    callbacks.tell_func = sdl_tell_func;

    if(ov_open_callbacks(m_file, &p->m_vf, nullptr, 0, callbacks) < 0)
    {
        m_lastError = "Not an Ogg Vorbis audio stream";
        close();
        return false;
    }

    if(!updateSection())
    {
        close();
        return false;
    }

    m_spec.m_sample_format = m_specWanted.getSampleFormat(AUDIO_F32SYS);

    if(m_spec.m_sample_format != AUDIO_S16SYS && m_spec.m_sample_format != AUDIO_F32SYS)
        m_spec.m_sample_format = AUDIO_F32SYS;

    vc = ov_comment(&p->m_vf, -1);
    if(vc != nullptr)
    {
        for(int i = 0; i < vc->comments; i++)
        {
            char *param = SDL_strdup(vc->user_comments[i]);
            char *argument = param;
            char *value = SDL_strchr(param, '=');

            if(value == nullptr)
                value = param + SDL_strlen(param);
            else
                *(value++) = '\0';

            /* Want to match LOOP-START, LOOP_START, etc. Remove - or _ from
             * string if it is present at position 4. */
            if(isLoopTag(argument) && ((argument[4] == '_') || (argument[4] == '-')))
                SDL_memmove(argument + 4, argument + 5, SDL_strlen(argument) - 4);

            if(SDL_strcasecmp(argument, "LOOPSTART") == 0)
                m_spec.m_loop_start = parseTime(value, m_spec.m_sample_rate);
            else if(SDL_strcasecmp(argument, "LOOPLENGTH") == 0)
            {
                m_spec.m_loop_len = SDL_strtoll(value, NULL, 10);
                is_loop_length = SDL_TRUE;
            }
            else if(SDL_strcasecmp(argument, "LOOPEND") == 0)
            {
                m_spec.m_loop_end = parseTime(value, m_spec.m_sample_rate);
                is_loop_length = SDL_FALSE;
            }
            else if(SDL_strcasecmp(argument, "TITLE") == 0)
                m_spec.m_meta_title = std::string(value);
            else if(SDL_strcasecmp(argument, "ARTIST") == 0)
                m_spec.m_meta_artist = std::string(value);
            else if(SDL_strcasecmp(argument, "ALBUM") == 0)
                m_spec.m_meta_album = std::string(value);
            else if(SDL_strcasecmp(argument, "COPYRIGHT") == 0)
                m_spec.m_meta_copyright = std::string(value);

            SDL_free(param);
        }

        if(is_loop_length)
            m_spec.m_loop_end =m_spec.m_loop_start + m_spec.m_loop_len;
        else
            m_spec.m_loop_len = m_spec.m_loop_end - m_spec.m_loop_start;

        /* Ignore invalid loop tag */
        if(m_spec.m_loop_start < 0 || m_spec.m_loop_len < 0 || m_spec.m_loop_end < 0)
        {
            m_spec.m_loop_start = 0;
            m_spec.m_loop_len = 0;
            m_spec.m_loop_end = 0;
        }
    }

    m_spec.m_total_length = ov_pcm_total(&p->m_vf, -1);

    return readRewind();
}

bool MDAudioVorbis::openWrite(SDL_RWops *file, const MDAudioFileSpec &dstSpec)
{
    std::string from_num;
    int ret, eos = 0;

    close();

    m_write = true;
    m_file = file;

    m_spec = dstSpec;

    vorbis_info_init(&p->m_vi);

#ifdef OGG_USE_TREMOR
    m_spec.m_sample_format = AUDIO_S16SYS;
#else
    if(m_spec.m_sample_format != AUDIO_S16SYS && m_spec.m_sample_format != AUDIO_F32SYS)
        m_spec.m_sample_format = AUDIO_F32SYS;
#endif

    if(m_spec.vbr)
        ret = vorbis_encode_init_vbr(&p->m_vi, m_spec.m_channels, m_spec.m_sample_rate, m_spec.quality / 10.0);
    else
        ret = vorbis_encode_init(&p->m_vi, m_spec.m_channels, m_spec.m_sample_rate, -1, m_spec.bitrate, -1);

    if(ret)
    {
        set_ov_error("vorbis_encode_init(_vbr)", ret);
        close();
        return false;
    }

    vorbis_comment_init(&p->m_vc);
    vorbis_comment_add_tag(&p->m_vc, "ENCODER", "Moondust Audio Converter");

    if(!m_spec.m_meta_title.empty())
        vorbis_comment_add_tag(&p->m_vc, "TITLE", m_spec.m_meta_title.c_str());

    if(!m_spec.m_meta_artist.empty())
        vorbis_comment_add_tag(&p->m_vc, "ARTIST", m_spec.m_meta_artist.c_str());

    if(!m_spec.m_meta_album.empty())
        vorbis_comment_add_tag(&p->m_vc, "ALBUM", m_spec.m_meta_album.c_str());

    if(!m_spec.m_meta_copyright.empty())
        vorbis_comment_add_tag(&p->m_vc, "COPYRIGHT", m_spec.m_meta_copyright.c_str());

    if(m_spec.m_loop_end > 0 && m_spec.m_loop_end > m_spec.m_loop_start)
    {
        from_num = std::to_string(m_spec.m_loop_start);
        vorbis_comment_add_tag(&p->m_vc, "LOOPSTART", from_num.c_str());

        from_num = std::to_string(m_spec.m_loop_end);
        vorbis_comment_add_tag(&p->m_vc, "LOOPEND", from_num.c_str());
    }

    vorbis_analysis_init(&p->m_vd, &p->m_vi);
    vorbis_block_init(&p->m_vd, &p->m_vb);

    srand(time(nullptr));
    ogg_stream_init(&p->m_os, rand());


    ogg_packet header;
    ogg_packet header_comm;
    ogg_packet header_code;

    vorbis_analysis_headerout(&p->m_vd, &p->m_vc, &header, &header_comm, &header_code);
    ogg_stream_packetin(&p->m_os, &header); /* automatically placed in its own page */
    ogg_stream_packetin(&p->m_os, &header_comm);
    ogg_stream_packetin(&p->m_os, &header_code);

    /* This ensures the actual
     * audio data will start on a new page, as per spec
     */
    while(!eos)
    {
        int result = ogg_stream_flush(&p->m_os, &p->m_og);
        if(result == 0)
            break;

        SDL_RWwrite(m_file, p->m_og.header, 1, p->m_og.header_len);
        SDL_RWwrite(m_file, p->m_og.body, 1, p->m_og.body_len);
    }

    return true;
}

bool MDAudioVorbis::close()
{
    if(m_write)
    {
        if(m_file)
        {
            vorbis_analysis_wrote(&p->m_vd, 0);
            writeFlush();

            ogg_stream_clear(&p->m_os);
            vorbis_block_clear(&p->m_vb);
            vorbis_dsp_clear(&p->m_vd);
            vorbis_comment_clear(&p->m_vc);
            vorbis_info_clear(&p->m_vi);
        }

        m_file = nullptr;
    }
    else if(m_file)
    {
        ov_clear(&p->m_vf);
        m_file = nullptr;
    }

    m_spec = MDAudioFileSpec();

    return true;
}

bool MDAudioVorbis::readRewind()
{
    if(m_write)
    {
        m_lastError = "Function readRewind called when write is open.";
        return false; // This function works for read only!
    }

    int result;

#ifdef OGG_USE_TREMOR
    result = ov_time_seek(&p->m_vf, 0);
#else
    result = ov_time_seek(&p->m_vf, 0.0);
#endif

    if(result)
    {
        set_ov_error("ov_time_seek", result);
        return false;
    }

    return true;
}

size_t MDAudioVorbis::readChunk(uint8_t *out, size_t outSize, bool *spec_changed)
{
    int amount = 0, cur_section = p->m_section;
#ifndef OGG_USE_TREMOR
    float **in_buffer;
    long in_buffer_samples;
#endif

    if(spec_changed)
        *spec_changed = false;

#ifdef OGG_USE_TREMOR
    amount = (int)ov_read(&p->m_vf, (char*)out, outSize, &cur_section);
#else
    if(m_spec.m_sample_format == AUDIO_F32SYS)
    {
        int sample_size = sizeof(float) * m_spec.m_channels;

        in_buffer_samples = ov_read_float(&p->m_vf, &in_buffer, outSize / sample_size, &cur_section);

        if(in_buffer_samples < 0)
            amount = (int)in_buffer_samples;
        else
        {
            float *out_s = (float*)out;

            for(int i = 0; i < in_buffer_samples; ++i)
            {
                for(int c = 0; c < m_spec.m_channels; ++c)
                    *(out_s++) = in_buffer[c][i];
            }

            amount = (int)(in_buffer_samples * sample_size);
        }
    }
    else
        amount = (int)ov_read(&p->m_vf, (char*)out, outSize, SDL_BYTEORDER == SDL_BIG_ENDIAN, 2, 1, &cur_section);
#endif

    if(amount < 0)
    {
        set_ov_error("ov_read", amount);
        return ~(size_t)0;
    }

    if(cur_section != p->m_section)
    {
        p->m_section = cur_section;

        if(spec_changed)
            *spec_changed = true;

        if(!updateSection())
            return 0;
    }

    return amount;
}

size_t MDAudioVorbis::writeChunk(uint8_t *in, size_t inSize)
{
    long i;
    size_t written = 0;
    int sample_size;
    /* expose the buffer to submit data */

    float **buffer = vorbis_analysis_buffer(&p->m_vd, inSize);

    /* uninterleave samples */
    if(m_spec.m_sample_format == AUDIO_F32SYS)
    {
        float *in_s = (float*)in;
        sample_size = m_spec.m_channels * sizeof(float);

        for(i = 0; i < inSize / sample_size; i++)
        {
            for(int c = 0; c < m_spec.m_channels; ++c)
                buffer[c][i] = *(in_s++);
        }
    }
    else
    {
        int16_t *in_s = (int16_t*)in;
        sample_size = m_spec.m_channels * sizeof(int16_t);

        for(i = 0; i < inSize / sample_size; i++)
        {
            for(int c = 0; c < m_spec.m_channels; ++c)
                buffer[c][i] = *(in_s++) / 32768.f;
        }
    }

    written += i * sample_size;
    vorbis_analysis_wrote(&p->m_vd, i);

    writeFlush();

    return written;
}
