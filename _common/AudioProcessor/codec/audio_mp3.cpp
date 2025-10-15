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
    const int allowed_rate[9] =
    {
        8000,
        11025,
        12000,
        16000,
        22050,
        24000,
        32000,
        44100,
        48000
    };
    int ret;

    close();

    m_write = true;
    m_file = file;
    m_written = 0;

    m_spec = dstSpec;

    m_io_buffer.resize(8192);

    switch(m_spec.m_sample_format)
    {
    case AUDIO_S16SYS:
    case AUDIO_S32SYS:
    case AUDIO_F32SYS:
        // Allowed
        break;
    default:
        m_spec.m_sample_format = AUDIO_S16SYS;
        break;
    }

    if(m_spec.m_channels > 2)
        m_spec.m_channels = 2; // MP3 supports only mono or stereo

    if(m_spec.m_sample_rate <= 8000)
        m_spec.m_sample_rate = 8000;
    else if(m_spec.m_sample_rate >= 48000)
        m_spec.m_sample_rate = 48000;
    else for(size_t i = 0; i < 8; ++i)
    {
        int s1 = allowed_rate[i], s2 = allowed_rate[i + 1];
        if(m_spec.m_sample_rate > s1 && m_spec.m_sample_rate <= s2)
        {
            // Set the nearest sample rate value to the desired
            m_spec.m_sample_rate = ((m_spec.m_sample_rate - s1) < ((s2 - s1) / 2)) ? s1 : s2;
            break;
        }
    }

    m_lame = lame_init();

    lame_set_in_samplerate(m_lame, m_spec.m_sample_rate);
    lame_set_num_channels(m_lame, m_spec.m_channels);
    lame_set_mode(m_lame, m_spec.m_channels == 2 ? STEREO : MONO);

    lame_set_VBR(m_lame, m_spec.vbr ? vbr_default : vbr_off);

    if(m_spec.vbr)
    {
        if(m_spec.quality > 9)
            m_spec.quality = 9;
        lame_set_VBR_q(m_lame, (9 - m_spec.quality));
    }
    else
        lame_set_brate(m_lame, m_spec.bitrate / 1000);

    if(!m_spec.m_meta_title.empty() || !m_spec.m_meta_artist.empty() || !m_spec.m_meta_album.empty() || !m_spec.m_meta_copyright.empty())
    {
        id3tag_init(m_lame);
        id3tag_add_v2(m_lame);

        if(!m_spec.m_meta_title.empty())
            id3tag_set_title(m_lame, m_spec.m_meta_title.c_str());

        if(!m_spec.m_meta_artist.empty())
            id3tag_set_artist(m_lame, m_spec.m_meta_artist.c_str());

        if(!m_spec.m_meta_album.empty())
            id3tag_set_album(m_lame, m_spec.m_meta_album.c_str());

        if(!m_spec.m_meta_copyright.empty())
            id3tag_set_comment(m_lame, m_spec.m_meta_copyright.c_str());

        lame_set_write_id3tag_automatic(m_lame, 1);
    }

    ret = lame_init_params(m_lame);
    if(ret < 0)
    {
        m_lastError = "Failed to initialize LAME: " + std::to_string(ret);
        close();
        return false;
    }

    return true;
}

bool MDAudioMP3::close()
{
    if(m_write)
    {
        if(m_lame)
        {
            if(m_written > 0)
            {
                int ret = lame_encode_flush(m_lame, m_io_buffer.data(), m_io_buffer.size());
                SDL_RWwrite(m_file, m_io_buffer.data(), 1, ret);
            }

            lame_close(m_lame);
        }
        m_lame = nullptr;
        m_write = false;
    }
    else if(m_handle)
    {
        mpg123_close(m_handle);
        mpg123_delete(m_handle);
        m_handle = nullptr;
        mpg123_exit();
    }

    m_file = nullptr;

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
    int result;
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
            return MDAudioFile::r_error;
        }

        format = mpg123_format_to_sdl(encoding);
        if(format == -1)
        {
            m_lastError = "encoding has unspported value: ";
            m_lastError += std::to_string(encoding);
            return MDAudioFile::r_error;
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
        return MDAudioFile::r_error;
    }

    return 0;
}

size_t MDAudioMP3::writeChunk(uint8_t *in, size_t inSize)
{
    int ret, frame_size;

    switch(m_spec.m_sample_format)
    {
    case AUDIO_S16SYS:
        frame_size = sizeof(int16_t) * m_spec.m_channels;

        if(m_spec.m_channels == 1)
            ret = lame_encode_buffer(m_lame, (short*)in, nullptr, inSize / frame_size, m_io_buffer.data(), m_io_buffer.size());
        else
            ret = lame_encode_buffer_interleaved(m_lame, (short*)in, inSize / frame_size, m_io_buffer.data(), m_io_buffer.size());
        break;

    case AUDIO_S32SYS:
        frame_size = sizeof(int) * m_spec.m_channels;

        if(m_spec.m_channels == 1)
            ret = lame_encode_buffer_int(m_lame, (int*)in, nullptr, inSize / frame_size, m_io_buffer.data(), m_io_buffer.size());
        else
            ret = lame_encode_buffer_interleaved_int(m_lame, (int*)in, inSize / frame_size, m_io_buffer.data(), m_io_buffer.size());
        break;

    case AUDIO_F32SYS:
        frame_size = sizeof(float) * m_spec.m_channels;

        if(m_spec.m_channels == 1)
            ret = lame_encode_buffer_ieee_float(m_lame, (float*)in, nullptr, inSize / frame_size, m_io_buffer.data(), m_io_buffer.size());
        else
            ret = lame_encode_buffer_interleaved_ieee_float(m_lame, (float*)in, inSize / frame_size, m_io_buffer.data(), m_io_buffer.size());
        break;

    default:
        m_lastError = "Unsupported write sample format";
        return 0;
    }

    if(ret)
    {
        size_t written = SDL_RWwrite(m_file, m_io_buffer.data(), 1, ret);
        if(written < (size_t)ret)
        {
            m_lastError = "Failed to write the stream: ";
            m_lastError += SDL_GetError();
            return 0;
        }

        m_written += written;
    }

    return inSize;
}
