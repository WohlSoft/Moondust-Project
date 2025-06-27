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

#include "audio_processor.h"
#include "codec/audio_vorbis.h"
#include "codec/audio_qoa.h"

#include <SDL2/SDL.h>


bool MoondustAudioProcessor::init_cvt_stream()
{
    const auto &source = m_in_file->getSpec();
    const auto &obtained = m_out_file->getSpec();

    if(m_cvt_stream)
        SDL_FreeAudioStream(m_cvt_stream);

    m_cvt_stream = SDL_NewAudioStream(source.m_sample_format, source.m_channels, source.m_sample_rate,
                                      obtained.m_sample_format, obtained.m_channels, obtained.m_sample_rate);
    if(!m_cvt_stream)
    {
        m_lastError = "Failed to initialize SDL Audio Stream: " + std::string(SDL_GetError());
        close();
        return false;
    }

    size_t sample_in = (SDL_AUDIO_BITSIZE(source.m_sample_format) / 8) * source.m_channels;
    size_t sample_out = (SDL_AUDIO_BITSIZE(obtained.m_sample_format) / 8) * obtained.m_channels;

    m_in_buffer.resize(MD_AUDIO_CHUNK_SIZE * sample_in);
    m_out_buffer.resize(MD_AUDIO_CHUNK_SIZE * sample_out);

    m_numChunks = (source.m_total_length * sample_in) / (double)MD_AUDIO_CHUNK_SIZE;

    if(m_cutAtLoopEnd && source.m_loop_end > 0)
        m_numChunks = ((source.m_loop_end * sample_in) + sample_in) / (double)MD_AUDIO_CHUNK_SIZE;

    m_curChunk = 0;

    return true;
}

MoondustAudioProcessor::MoondustAudioProcessor()
{}

MoondustAudioProcessor::~MoondustAudioProcessor()
{
    close();
}

std::string MoondustAudioProcessor::getLastError() const
{
    return m_lastError;
}

const MDAudioFileSpec &MoondustAudioProcessor::getInSpec() const
{
    return m_in_file->getSpec();
}

bool MoondustAudioProcessor::openInFile(const std::string &file, int *detectedFormat)
{
    Uint8 magic[100];
    Uint8 submagic[4];

    if(m_rw_in)
        SDL_RWclose(m_rw_in);

    m_done = false;
    m_cuttedAtLoop = false;
    m_stat_write = 0;
    m_stat_read = 0;
    m_curChunk = 0.0;

    m_rw_in = SDL_RWFromFile(file.c_str(), "rb");
    if(!m_rw_in)
    {
        m_lastError = "Failed to open input file by SDL_RWFromFile for read.";
        m_in_file.reset(nullptr);
        return false;
    }

    SDL_memset(magic, 0, 100);
    if(SDL_RWread(m_rw_in, magic, 1, 99) < 24)
    {
        m_lastError = "Couldn't read any first 24 bytes of audio data";
        m_in_file.reset(nullptr);
        SDL_RWclose(m_rw_in);
        m_rw_in = nullptr;
        return false;
    }
    SDL_RWseek(m_rw_in, 0, RW_SEEK_SET);

    m_in_filePath = file;
    m_in_file.reset(nullptr);

    if(SDL_memcmp(magic, "OggS", 4) == 0)
    {
        SDL_RWseek(m_rw_in, 28, RW_SEEK_CUR);
        SDL_RWread(m_rw_in, magic, 1, 8);
        SDL_RWseek(m_rw_in,-36, RW_SEEK_CUR);

        if (SDL_memcmp(magic, "OpusHead", 8) == 0)
        {
            // m_in_file.reset(new MDAudioOpus);
        }
        else if (magic[0] == 0x7F && SDL_memcmp(magic + 1, "FLAC", 4) == 0)
        {
            // m_in_file.reset(new MDAudioFLAC);
        }
        else
        {
            m_in_file.reset(new MDAudioVorbis);
        }
    }
    else if (SDL_memcmp(magic, "qoaf", 4) == 0)
        m_in_file.reset(new MDAudioQOA);
    else if (SDL_memcmp(magic, "XQOA", 4) == 0)
        m_in_file.reset(new MDAudioQOA(true));

    SDL_RWseek(m_rw_in, 0, RW_SEEK_SET);

    if(!m_in_file.get())
    {
        m_lastError = "Unknown or unsupported file format";
        SDL_RWclose(m_rw_in);
        m_rw_in = nullptr;
        return false;
    }

    if(!m_in_file->openRead(m_rw_in))
    {
        m_lastError = m_in_file->getLastError();
        m_in_file.reset(nullptr);
        SDL_RWclose(m_rw_in);
        m_rw_in = nullptr;
        return false;
    }

    const auto &source = m_in_file->getSpec();
    size_t sample_in = (SDL_AUDIO_BITSIZE(source.m_sample_format) / 8) * source.m_channels;
    m_numChunks = (source.m_total_length * sample_in) / MD_AUDIO_CHUNK_SIZE;

    if(m_cutAtLoopEnd && source.m_loop_end > 0)
        m_numChunks = ((source.m_loop_end * sample_in) + sample_in) / (double)MD_AUDIO_CHUNK_SIZE;

    if(detectedFormat)
        *detectedFormat = FORMAT_OGG_VORBIS;

    return true;
}

bool MoondustAudioProcessor::openOutFile(const std::string &file, int dstFormat, const MDAudioFileSpec &dstSpec)
{
    if(!m_in_file.get())
        return false;

    switch(dstFormat)
    {
    case FORMAT_OGG_VORBIS:
        m_out_file.reset(new MDAudioVorbis);
        break;
    case FORMAT_QOA:
        m_out_file.reset(new MDAudioQOA);
        break;
    case FORMAT_XQOA:
        m_out_file.reset(new MDAudioQOA(true));
        break;
    default:
        m_lastError = "Incorrect or unsupported destination format";
        return false;
    }

    if(m_rw_out)
        SDL_RWclose(m_rw_out);

    m_rw_out = SDL_RWFromFile(file.c_str(), "wb");
    if(!m_rw_out)
    {
        m_lastError = "Failed to open output file by SDL_RWFromFile for write.";
        m_out_file.reset(nullptr);
        return false;
    }

    m_out_filePath = file;

    if(!m_out_file->openWrite(m_rw_out, dstSpec))
    {
        m_lastError = m_out_file->getLastError();
        m_out_file.reset(nullptr);
        SDL_RWclose(m_rw_in);
        m_rw_in = nullptr;
        return false;
    }

    if(!init_cvt_stream())
    {
        m_out_file.reset(nullptr);
        SDL_RWclose(m_rw_in);
        m_rw_in = nullptr;
        return false;
    }

    m_stat_read = 0;
    m_stat_write = 0;

    return true;
}

void MoondustAudioProcessor::close()
{
    if(m_out_file.get())
    {
        m_out_file->close();
        m_out_file.reset(nullptr);
        SDL_RWclose(m_rw_out);
        m_rw_out = nullptr;
    }

    if(m_in_file.get())
    {
        m_in_file->close();
        m_in_file.reset(nullptr);
        SDL_RWclose(m_rw_in);
        m_rw_in = nullptr;
    }

    if(m_cvt_stream)
    {
        SDL_FreeAudioStream(m_cvt_stream);
        m_cvt_stream = nullptr;
    }

    m_in_filePath.clear();
    m_out_filePath.clear();
    m_done = false;
    m_cutAtLoopEnd = false;
    m_numChunks = 0.0;
    m_curChunk = 0.0;
}

void MoondustAudioProcessor::setCutAtLoopEnd(bool en)
{
    m_cutAtLoopEnd = en;
}

uint32_t MoondustAudioProcessor::numChunks() const
{
    return (int)round(m_numChunks);
}

uint32_t MoondustAudioProcessor::curChunk() const
{
    return (int)round(m_curChunk);
}

int64_t MoondustAudioProcessor::getBytesReadStat() const
{
    return m_stat_read;
}

int64_t MoondustAudioProcessor::getBytesWrittenStat() const
{
    return m_stat_write;
}

int64_t MoondustAudioProcessor::getSamplesReadStat() const
{
    if(!m_in_file.get())
        return 0;

    const auto &source = m_in_file->getSpec();
    size_t sample_in = (SDL_AUDIO_BITSIZE(source.m_sample_format) / 8) * source.m_channels;

    return m_stat_read / sample_in;
}

int64_t MoondustAudioProcessor::getSamplesWrittenStat() const
{
    if(!m_out_file.get())
        return 0;

    const auto &obtained = m_out_file->getSpec();
    size_t sample_out = (SDL_AUDIO_BITSIZE(obtained.m_sample_format) / 8) * obtained.m_channels;

    return m_stat_write / sample_out;
}

bool MoondustAudioProcessor::rewindRead()
{
    m_done = false;
    m_cuttedAtLoop = false;
    m_stat_write = 0;
    m_stat_read = 0;
    m_curChunk = 0.0;

    if(!init_cvt_stream())
        return false;

    return m_in_file->readRewind();
}

bool MoondustAudioProcessor::runChunk(bool dry)
{
    int filled = 0, amount = 0, written = 0, attempts = 10;
    bool spec_changed = false;

    do
    {
        filled = SDL_AudioStreamGet(m_cvt_stream, m_out_buffer.data(), m_out_buffer.size());
        if(filled != 0)
        {
            if(filled < 0)
                return false;
            break;
        }

        if(!m_cuttedAtLoop)
            amount = m_in_file->readChunk(m_in_buffer.data(), m_in_buffer.size(), &spec_changed);

        if(amount == ~(size_t)0)
            return false;

        if(spec_changed) // Re-Init streamer
        {
            if(!init_cvt_stream())
                return false;
        }

        if(amount > 0)
        {
            if(m_cutAtLoopEnd)
            {
                const auto &source = m_in_file->getSpec();
                size_t sample_in = (SDL_AUDIO_BITSIZE(source.m_sample_format) / 8) * source.m_channels;

                if(source.m_loop_end > 0 && m_stat_read + amount > (source.m_loop_end * sample_in))
                {
                    amount = m_stat_read + amount - (source.m_loop_end * sample_in);
                    m_cuttedAtLoop = true;
                }
            }

            m_stat_read += amount;
            m_curChunk += amount / (double)MD_AUDIO_CHUNK_SIZE;
            if(SDL_AudioStreamPut(m_cvt_stream, m_in_buffer.data(), amount) < 0)
                return false;
        }
        else
        {
            SDL_AudioStreamFlush(m_cvt_stream);
            attempts--;
            if(attempts < 0)
            {
                m_done = true;
                return true;
            }
        }

    } while(filled == 0);

    if(dry)
        written = filled;
    else if(m_out_file.get())
        written = m_out_file->writeChunk(m_out_buffer.data(), filled);

    if(written == 0)
        return false;

    m_stat_write += written;

    return true;
}

bool MoondustAudioProcessor::done() const
{
    return m_done;
}
