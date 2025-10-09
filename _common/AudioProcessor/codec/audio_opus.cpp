#include <SDL2/SDL_rwops.h>
#include <SDL2/SDL_audio.h>
#include "audio_opus.h"

#include <opus/opusfile.h>

int MDAudioOpus::set_op_error(const char *function, int error)
{
#define HANDLE_ERROR_CASE(X) case X: m_lastError = std::string(function) + ": " + std::string(#X); break;
    switch(error)
    {
    HANDLE_ERROR_CASE(OP_FALSE)
    HANDLE_ERROR_CASE(OP_EOF)
    HANDLE_ERROR_CASE(OP_HOLE)
    HANDLE_ERROR_CASE(OP_EREAD)
    HANDLE_ERROR_CASE(OP_EFAULT)
    HANDLE_ERROR_CASE(OP_EIMPL)
    HANDLE_ERROR_CASE(OP_EINVAL)
    HANDLE_ERROR_CASE(OP_ENOTFORMAT)
    HANDLE_ERROR_CASE(OP_EBADHEADER)
    HANDLE_ERROR_CASE(OP_EVERSION)
    HANDLE_ERROR_CASE(OP_ENOTAUDIO)
    HANDLE_ERROR_CASE(OP_EBADPACKET)
    HANDLE_ERROR_CASE(OP_EBADLINK)
    HANDLE_ERROR_CASE(OP_ENOSEEK)
    HANDLE_ERROR_CASE(OP_EBADTIMESTAMP)
    default:
        m_lastError = std::string(function) + ": unknown error " + std::to_string(error);
        break;
    }
    return -1;
}

bool MDAudioOpus::updateSection()
{
    const OpusHead *op_info;

    op_info = op_head(m_of, -1);
    if(!op_info)
    {
        m_lastError = "op_head returned NULL";
        return false;
    }

    if(m_op_info && op_info->channel_count == m_op_info->channel_count)
        return true;

    m_op_info = op_info;

    m_spec.m_sample_rate = 48000;
    m_spec.m_channels = m_op_info->channel_count;

    return true;
}

void MDAudioOpus::writeFlush()
{

}

MDAudioOpus::MDAudioOpus() :
    MDAudioFile()
{}

MDAudioOpus::~MDAudioOpus()
{
    MDAudioOpus::close();
}

uint32_t MDAudioOpus::getCodecSpec() const
{
    return SPEC_READ | SPEC_WRITE | SPEC_LOOP_POINTS | SPEC_META_TAGS | SPEC_FIXED_SAMPLE_RATE;
}

static int sdl_read_func(void *datasource, unsigned char *ptr, int size)
{
    return (int)SDL_RWread((SDL_RWops*)datasource, ptr, 1, (size_t)size);
}

static int sdl_seek_func(void *datasource, opus_int64 offset, int whence)
{
    return (SDL_RWseek((SDL_RWops*)datasource, offset, whence) < 0)? -1 : 0;
}

static opus_int64 sdl_tell_func(void *datasource)
{
    return SDL_RWtell((SDL_RWops*)datasource);
}

bool MDAudioOpus::openRead(SDL_RWops *file)
{
    OpusFileCallbacks callbacks;
    const OpusTags* tags;
    int err = 0, ci;
    SDL_bool is_loop_length = SDL_FALSE;

    close();

    m_write = false;
    m_file = file;

    m_section = -1;
    m_of = nullptr;
    m_op_info = nullptr;

    SDL_zero(callbacks);
    callbacks.read = sdl_read_func;
    callbacks.seek = sdl_seek_func;
    callbacks.tell = sdl_tell_func;

    m_of = op_open_callbacks(file, &callbacks, NULL, 0, &err);
    if(!m_of)
    {
        /*  set_op_error("op_open_callbacks", err);*/
        m_lastError = "Not an Opus audio stream";
        close();
        return false;
    }

    if(!op_seekable(m_of))
    {
        m_lastError = "Opus stream not seekable";
        close();
        return false;
    }

    if(!updateSection())
    {
        close();
        return false;
    }

    m_spec.m_sample_format = AUDIO_S16SYS;
    m_spec.m_sample_rate = 48000;

    tags = op_tags(m_of, -1);
    if(tags != NULL)
    {
        for(ci = 0; ci < tags->comments; ci++)
        {
            char *param = SDL_strdup(tags->user_comments[ci]);
            char *argument = param;
            char *value = SDL_strchr(param, '=');

            if(value == NULL)
                value = param + SDL_strlen(param);
            else
                *(value++) = '\0';

            /* Want to match LOOP-START, LOOP_START, etc. Remove - or _ from
             * string if it is present at position 4. */
            if(isLoopTag(argument) && ((argument[4] == '_') || (argument[4] == '-')))
                SDL_memmove(argument + 4, argument + 5, SDL_strlen(argument) - 4);

            if(SDL_strcasecmp(argument, "LOOPSTART") == 0)
                m_spec.m_loop_start = parseTime(value, 48000);
            else if(SDL_strcasecmp(argument, "LOOPLENGTH") == 0)
            {
                m_spec.m_loop_len = SDL_strtoll(value, NULL, 10);
                is_loop_length = SDL_TRUE;
            }
            else if(SDL_strcasecmp(argument, "LOOPEND") == 0)
            {
                m_spec.m_loop_end = parseTime(value, 48000);
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
            m_spec.m_loop_end = m_spec.m_loop_start + m_spec.m_loop_len;
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

    m_spec.m_total_length = op_pcm_total(m_of, -1);

    return readRewind();
}

bool MDAudioOpus::openWrite(SDL_RWops *file, const MDAudioFileSpec &dstSpec)
{
    return false;
}

bool MDAudioOpus::close()
{
    if(m_write)
    {

    }
    else
    {
        op_free(m_of);
        m_of = nullptr;
        m_op_info = nullptr;
        m_file = nullptr;
    }

    m_spec = MDAudioFileSpec();

    return true;
}

bool MDAudioOpus::readRewind()
{
    if(m_write)
    {
        m_lastError = "Function readRewind called when write is open.";
        return false; // This function works for read only!
    }

    int result = op_pcm_seek(m_of, (ogg_int64_t)0);

    if(result < 0)
    {
        set_op_error("op_pcm_seek", result);
        return false;
    }

    return true;
}

size_t MDAudioOpus::readChunk(uint8_t *out, size_t outSize, bool *spec_changed)
{
    int filled, samples, section;

    if(spec_changed)
        *spec_changed = false;

    section = m_section;

    samples = op_read(m_of, (opus_int16 *)out, outSize / (int)sizeof(opus_int16), &section);
    if(samples < 0)
    {
        set_op_error("op_read", samples);
        return 0;
    }

    if(section != m_section)
    {
        m_section = section;

        if(spec_changed)
            *spec_changed = true;

        if(!updateSection())
            return 0;
    }

    return samples * sizeof(opus_int16) * m_spec.m_channels;
}

size_t MDAudioOpus::writeChunk(uint8_t *in, size_t inSize)
{
    return 0;
}
