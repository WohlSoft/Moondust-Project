#include <SDL2/SDL_rwops.h>
#include <SDL2/SDL_audio.h>
#include "audio_mp3.h"

#include <mpg123.h>
#include <lame/lame.h>

#include "mp3/mp3utils.h"

#ifdef _MSC_VER
typedef ptrdiff_t MIX_SSIZE_T;
#else
typedef ssize_t   MIX_SSIZE_T;
#endif


const char *MDAudioMP3::mpg_err(mpg123_handle *mpg, int result)
{
    return (mpg && result == MPG123_ERR) ? mpg123_strerror(mpg) : mpg123_plain_strerror(result);
}

MDAudioMP3::MDAudioMP3() :
    MDAudioFile(),
    m_mp3file(new mp3file_t)
{}

MDAudioMP3::~MDAudioMP3()
{
    delete m_mp3file;
}

uint32_t MDAudioMP3::getCodecSpec() const
{
    return SPEC_READ | SPEC_WRITE | SPEC_META_TAGS | SPEC_FIXED_SAMPLE_RATE;
}

static int mpg123_format_to_sdl(int fmt)
{
    switch (fmt)
    {
    case MPG123_ENC_SIGNED_8:
        return AUDIO_S8;
    case MPG123_ENC_UNSIGNED_8:
        return AUDIO_U8;
    case MPG123_ENC_SIGNED_16:
        return AUDIO_S16SYS;
    case MPG123_ENC_UNSIGNED_16:
        return AUDIO_U16SYS;
    case MPG123_ENC_SIGNED_32:
        return AUDIO_S32SYS;
    case MPG123_ENC_FLOAT_32:
        return AUDIO_F32SYS;
    default:
        return -1;
    }
}

static MIX_SSIZE_T rwops_read(void* p, void* dst, size_t n)
{
    return (MIX_SSIZE_T)MP3_RWread((struct mp3file_t *)p, dst, 1, n);
}

static off_t rwops_seek(void* p, off_t offset, int whence)
{
    return (off_t)MP3_RWseek((struct mp3file_t *)p, (Sint64)offset, whence);
}

static void rwops_cleanup(void* p)
{
    (void)p;
    /* do nothing, we will free the file later */
}

bool MDAudioMP3::openRead(SDL_RWops *file)
{
    Mix_MusicMetaTags tags;
    int result, format, channels, encoding;
    long rate;
    const long *rates;
    size_t i, num_rates;

    close();

    m_write = false;
    m_file = file;

    if(MP3_RWinit(m_mp3file, m_file) < 0)
    {
        m_lastError = "Can't open the file";
        close();
        return false;
    }

    meta_tags_init(&tags);
    if(mp3_read_tags(&tags, m_mp3file, SDL_TRUE) < 0)
    {
        m_lastError = "music_mpg123: corrupt mp3 file (bad tags.)";
        close();
        meta_tags_clear(&tags);
        return false;
    }

    m_handle = mpg123_new(0, &result);
    if(result != MPG123_OK)
    {
        m_lastError = "mpg123_new failed";
        close();
        meta_tags_clear(&tags);
        return false;
    }

    result = mpg123_replace_reader_handle(m_handle, rwops_read, rwops_seek, rwops_cleanup);
    if(result != MPG123_OK)
    {
        m_lastError = "mpg123_replace_reader_handle: ";
        m_lastError += mpg_err(m_handle, result);
        close();
        meta_tags_clear(&tags);
        return false;
    }

    result = mpg123_format_none(m_handle);
    if(result != MPG123_OK)
    {
        m_lastError = "mpg123_format_none: ";
        m_lastError += mpg_err(m_handle, result);
        close();
        meta_tags_clear(&tags);
        return false;
    }

    mpg123_rates(&rates, &num_rates);
    for(i = 0; i < num_rates; ++i)
    {
        const int channels = (MPG123_MONO|MPG123_STEREO);
        const int formats = (MPG123_ENC_SIGNED_8 |
                             MPG123_ENC_UNSIGNED_8 |
                             MPG123_ENC_SIGNED_16 |
                             MPG123_ENC_UNSIGNED_16 |
                             MPG123_ENC_SIGNED_32 |
                             MPG123_ENC_FLOAT_32);

        mpg123_format(m_handle, rates[i], channels, formats);
    }

    result = mpg123_open_handle(m_handle, m_mp3file);
    if(result != MPG123_OK)
    {
        m_lastError = "mpg123_open_handle: ";
        m_lastError += mpg_err(m_handle, result);
        close();
        meta_tags_clear(&tags);
        return false;
    }

    result = mpg123_getformat(m_handle, &rate, &channels, &encoding);
    if(result != MPG123_OK)
    {
        m_lastError = "mpg123_getformat: ";
        m_lastError += mpg_err(m_handle, result);
        close();
        meta_tags_clear(&tags);
        return false;
    }

    format = mpg123_format_to_sdl(encoding);
    if(format == -1)
    {
        m_lastError = "encoding has unspported value: ";
        m_lastError += std::to_string(encoding);
        close();
        meta_tags_clear(&tags);
        return false;
    }

    m_spec.m_sample_format = format;
    m_spec.m_channels = channels;
    m_spec.m_sample_rate = rate;

    m_spec.m_total_length = mpg123_length(m_handle);

    m_spec.m_meta_title = meta_tags_get(&tags, MIX_META_TITLE);
    m_spec.m_meta_artist = meta_tags_get(&tags, MIX_META_ARTIST);
    m_spec.m_meta_album = meta_tags_get(&tags, MIX_META_ALBUM);
    m_spec.m_meta_copyright = meta_tags_get(&tags, MIX_META_COPYRIGHT);

    meta_tags_clear(&tags);

    return true;
}

bool MDAudioMP3::openWrite(SDL_RWops *file, const MDAudioFileSpec &dstSpec)
{
    return false;
}

bool MDAudioMP3::close()
{
    if(m_write)
    {

    }
    else if(m_handle)
    {
        mpg123_close(m_handle);
        mpg123_delete(m_handle);
        m_handle = nullptr;
        mpg123_exit();
    }

    return true;
}

bool MDAudioMP3::readRewind()
{
    off_t ret;

    if((ret = mpg123_seek(m_handle, 0, SEEK_SET)) < 0)
    {
        m_lastError = "mpg123_seek: ";
        m_lastError = mpg_err(m_handle, (int)-ret);
        return false;
    }

    return true;
}

size_t MDAudioMP3::readChunk(uint8_t *out, size_t outSize, bool *spec_changed)
{
    int filled, result;
    size_t amount = 0;
    long rate;
    int channels, encoding, format;

    if(spec_changed)
        *spec_changed = false;

retry:
    result = mpg123_read(m_handle, out, outSize, &amount);

    switch (result)
    {
    case MPG123_OK:
        return amount;

    case MPG123_DONE:
        return 0;

    case MPG123_NEW_FORMAT:
        result = mpg123_getformat(m_handle, &rate, &channels, &encoding);
        if(result != MPG123_OK)
        {
            m_lastError = "mpg123_getformat: ";
            m_lastError += mpg_err(m_handle, result);
            return (size_t)~0;
        }

        format = mpg123_format_to_sdl(encoding);
        if(format == -1)
        {
            m_lastError = "encoding has unspported value: ";
            m_lastError += std::to_string(encoding);
            return (size_t)~0;
        }

        m_spec.m_channels = channels;
        m_spec.m_sample_rate = rate;
        m_spec.m_sample_format = format;

        if(spec_changed)
            *spec_changed = true;

        goto retry;

    default:
        m_lastError = "mpg123_read: ";
        m_lastError += mpg_err(m_handle, result);
        return (size_t)~0;
    }

    return 0;
}

size_t MDAudioMP3::writeChunk(uint8_t *in, size_t inSize)
{
    return 0;
}
