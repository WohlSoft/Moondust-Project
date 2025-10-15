#include <SDL2/SDL_rwops.h>
#include <SDL2/SDL_audio.h>
#include <gme/gme.h>

#include "audio_gme.h"


bool MDAudioGME::initialize_from_track_info()
{
    gme_info_t *mus_info;
    bool has_loop_length = true;
    const char *err;

    err = gme_track_info(m_synth, &mus_info, m_track_number);
    if(err != 0)
    {
        m_lastError = "GME init track error: ";
        m_lastError += err;
        return false;
    }

    // FIXME: on GME side, implement the sample-accurate values retrieving
    m_track_length = mus_info->length;
    m_intro_length = mus_info->intro_length;
    m_loop_length = mus_info->loop_length;

    m_has_track_length = true;
    if(m_track_length <= 0 )
    {
        m_track_length = (int)(2.5 * 60 * 1000);
        m_has_track_length = false;
    }

    if(m_intro_length < 0)
        m_intro_length = 0;

    if(m_loop_length <= 0)
    {
        if(m_track_length > 0)
            m_loop_length = m_track_length;
        else
            m_loop_length = (int)(2.5 * 60 * 1000);

        has_loop_length = false;
    }

    if(!m_has_track_length && has_loop_length)
    {
        m_track_length = m_intro_length + m_loop_length;
        m_has_track_length = true;
    }

    m_spec.m_meta_title = mus_info->song;
    m_spec.m_meta_artist = mus_info->author;
    m_spec.m_meta_album = mus_info->game;
    m_spec.m_meta_copyright = mus_info->copyright;
    gme_free_info(mus_info);

    m_spec.m_total_length = ((m_track_length / 1000.0) * m_spec.m_sample_rate) / tempo;
    m_spec.m_loop_start = ((m_intro_length / 1000.0) * m_spec.m_sample_rate) / tempo;
    m_spec.m_loop_end = ((m_track_length / 1000.0) * m_spec.m_sample_rate) / tempo;

    if(m_spec.m_loop_start >= 0 && m_spec.m_loop_end >= 0)
    {
        m_spec.m_loop_len = m_spec.m_loop_end - m_spec.m_loop_start;
    }
    else
    {
        m_spec.m_loop_start = 0;
        m_spec.m_loop_end = 0;
        m_spec.m_loop_len = 0;
    }

    return true;
}

MDAudioGME::MDAudioGME() :
    MDAudioFile()
{
    m_io_buffer.resize(4096);
}

MDAudioGME::~MDAudioGME()
{
    MDAudioGME::close();
}

uint32_t MDAudioGME::getCodecSpec() const
{
    return SPEC_READ | SPEC_SOURCE_ANY_RATE | SPEC_LOOP_POINTS | SPEC_META_TAGS | SPEC_FIXED_CHANNELS;
}

bool MDAudioGME::openRead(SDL_RWops *file)
{
    size_t file_size;
    const char *err;

    close();

    m_file = file;
    pos = 0;

    m_track_number = m_args.getTrackNumber();

    m_echo_disabled = m_args.getArgBI("e", 0);
    m_echo_const = m_args.getArgBI("c", 0);

    tempo = m_args.getArgD("t=", 1.0);
    gain = m_args.getArgF("g=", 1.0f);

    m_spec.m_channels = 2;
    m_spec.m_sample_format = AUDIO_S16SYS;
    m_spec.m_sample_rate = m_specWanted.getSampleRate(48000);

    file_size = SDL_RWsize(m_file);
    m_in_file.resize(file_size);
    SDL_RWread(m_file, m_in_file.data(), 1, file_size);
    SDL_RWseek(m_file, 0, SEEK_SET);

    err = gme_open_data(m_in_file.data(), (long)m_in_file.size(), &m_synth, m_spec.m_sample_rate);
    if(err)
    {
        m_lastError = "GME open file error: ";
        m_lastError += err;
        close();
        return false;
    }

    if(m_track_number < 0 || m_track_number >= gme_track_count(m_synth))
        m_track_number = gme_track_count(m_synth) - 1;

#if (GME_VERSION >= 0x000603)
    gme_set_autoload_playback_limit(m_synth, 0);
#endif

#if (GME_VERSION >= 0x000604)
    gme_disable_echo(m_synth, m_echo_disabled);
#endif

    err = gme_start_track(m_synth, m_track_number);
    if(err)
    {
        m_lastError = "GME track start error: ";
        m_lastError += err;
        close();
        return false;
    }

    gme_set_tempo(m_synth, tempo);

    if(!initialize_from_track_info())
    {
        close();
        return false;
    }

    return true;
}

bool MDAudioGME::openWrite(SDL_RWops *, const MDAudioFileSpec &)
{
    return false;
}

bool MDAudioGME::close()
{
    if(m_synth)
    {
        gme_delete(m_synth);
        m_synth = nullptr;
    }

    m_file = nullptr;
    m_in_file.clear();
    m_io_buffer.clear();

    return true;
}

bool MDAudioGME::readRewind()
{
    gme_set_fade(m_synth, m_intro_length + m_loop_length);
    gme_seek(m_synth, 0);
    pos = 0;
    return true;
}

size_t MDAudioGME::readChunk(uint8_t *out, size_t outSize, bool *)
{
    const char *err = NULL;

    if(gme_track_ended(m_synth) || pos > (size_t)m_spec.m_total_length)
        return 0;

    if(outSize > m_io_buffer.size())
        m_io_buffer.resize(outSize);

    err = gme_play(m_synth, (int)(outSize / 2), (short*)m_io_buffer.data());
    if(err)
    {
        m_lastError = "GME playback error: ";
        m_lastError += err;
        return MDAudioFile::r_error;
    }

    pos += (outSize / 4);

    copyGained(gain, m_io_buffer.data(), out, outSize);

    return outSize;
}

size_t MDAudioGME::writeChunk(uint8_t *, size_t)
{
    return 0;
}

