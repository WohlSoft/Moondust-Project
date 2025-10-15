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

#include <emu_de_midi.h>

#include "audio_midi_edmidi.h"

bool MDAudioEDMIDI::reCreateSynth()
{
    if(!m_synth)
        edmidi_close(m_synth);

    m_synth = edmidi_initEx(m_spec.m_sample_rate, mods_num);
    if(!m_synth)
    {
        m_lastError = std::string(edmidi_errorString());
        return false;
    }

    return true;
}

bool MDAudioEDMIDI::reInit()
{
    edmidi_setTempo(m_synth, tempo);
    return true;
}

bool MDAudioEDMIDI::reOpenFile()
{
    if(edmidi_openData(m_synth, m_in_file.data(), m_in_file.size()) < 0)
    {
        m_lastError = "EDMIDI: ";
        m_lastError += edmidi_errorInfo(m_synth);
        return false;
    }

    edmidi_setLoopEnabled(m_synth, false);

    return true;
}

MDAudioEDMIDI::MDAudioEDMIDI() :
    MDAudioFile()
{
    m_io_buffer.resize(4096);
}

MDAudioEDMIDI::~MDAudioEDMIDI()
{
    MDAudioEDMIDI::close();
}

uint32_t MDAudioEDMIDI::getCodecSpec() const
{
    return SPEC_READ | SPEC_SOURCE_ANY_RATE | SPEC_SOURCE_ANY_FORMAT | SPEC_LOOP_POINTS | SPEC_META_TAGS | SPEC_FIXED_CHANNELS;
}

bool MDAudioEDMIDI::openRead(SDL_RWops *file)
{
    size_t file_size;
    close();

    // Init settings
    mods_num = m_args.getArgI("m", 2);
    tempo = m_args.getArgD("t=", 1.0);
    gain = m_args.getArgF("g=", 2.0f);

    m_file = file;

    m_spec.m_channels = 2;
    m_spec.m_sample_format = m_specWanted.getSampleFormat(AUDIO_F32SYS);
    m_spec.m_sample_rate = m_specWanted.getSampleRate(44100);

    if(!reCreateSynth())
    {
        close();
        return false;
    }

    file_size = SDL_RWsize(m_file);
    m_in_file.resize(file_size);
    SDL_RWread(m_file, m_in_file.data(), 1, file_size);
    SDL_RWseek(m_file, 0, SEEK_SET);

    if(!reInit())
    {
        close();
        return false;
    }

    if(!reOpenFile())
    {
        close();
        return false;
    }

    switch(m_spec.m_sample_format)
    {
    case AUDIO_U8:
        m_sample_format.type = EDMIDI_SampleType_U8;
        m_sample_format.containerSize = sizeof(Uint8);
        m_sample_format.sampleOffset = sizeof(Uint8) * 2;
        break;
    case AUDIO_S8:
        m_sample_format.type = EDMIDI_SampleType_S8;
        m_sample_format.containerSize = sizeof(Sint8);
        m_sample_format.sampleOffset = sizeof(Sint8) * 2;
        break;
    case AUDIO_S16LSB:
    case AUDIO_S16MSB:
        m_sample_format.type = EDMIDI_SampleType_S16;
        m_sample_format.containerSize = sizeof(Sint16);
        m_sample_format.sampleOffset = sizeof(Sint16) * 2;
        m_spec.m_sample_format = AUDIO_S16SYS;
        break;
    case AUDIO_U16LSB:
    case AUDIO_U16MSB:
        m_sample_format.type = EDMIDI_SampleType_U16;
        m_sample_format.containerSize = sizeof(Uint16);
        m_sample_format.sampleOffset = sizeof(Uint16) * 2;
        m_spec.m_sample_format = AUDIO_U16SYS;
        break;
    case AUDIO_S32LSB:
    case AUDIO_S32MSB:
        m_sample_format.type = EDMIDI_SampleType_S32;
        m_sample_format.containerSize = sizeof(Sint32);
        m_sample_format.sampleOffset = sizeof(Sint32) * 2;
        m_spec.m_sample_format = AUDIO_S32SYS;
        break;
    case AUDIO_F32LSB:
    case AUDIO_F32MSB:
    default:
        m_sample_format.type = EDMIDI_SampleType_F32;
        m_sample_format.containerSize = sizeof(float);
        m_sample_format.sampleOffset = sizeof(float) * 2;
        m_spec.m_sample_format = AUDIO_F32SYS;
    }

    m_spec.m_loop_start = (int64_t)(edmidi_loopStartTime(m_synth) * m_spec.m_sample_rate) / tempo;
    m_spec.m_loop_end = (int64_t)(edmidi_loopEndTime(m_synth) * m_spec.m_sample_rate) / tempo;
    m_spec.m_total_length = (edmidi_totalTimeLength(m_synth) * m_spec.m_sample_rate) / tempo;

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

    m_spec.m_meta_title = edmidi_metaMusicTitle(m_synth);
    m_spec.m_meta_copyright = edmidi_metaMusicCopyright(m_synth);

    return true;
}

bool MDAudioEDMIDI::openWrite(SDL_RWops *, const MDAudioFileSpec &)
{
    return false;
}

bool MDAudioEDMIDI::close()
{
    if(m_file)
    {
        if(m_synth)
        {
            edmidi_close(m_synth);
            m_synth = nullptr;
        }
        m_file = nullptr;
    }

    m_spec = MDAudioFileSpec();

    return true;
}

bool MDAudioEDMIDI::readRewind()
{
    edmidi_reset(m_synth);
    edmidi_positionRewind(m_synth);

    // To avoid junk data being captured, re-init the synth from the scratch!
    reCreateSynth();
    reInit();
    reOpenFile();

    return true;
}

size_t MDAudioEDMIDI::readChunk(uint8_t *out, size_t outSize, bool *spec_changed)
{
    struct EDMIDI_AudioFormat sample_format;
    int ret = 0;

    if(spec_changed)
        *spec_changed = false;

    if(edmidi_atEnd(m_synth))
        return 0;

    if(outSize > m_io_buffer.size())
        m_io_buffer.resize(outSize);

    sample_format.type = (EDMIDI_SampleType)m_sample_format.type;
    sample_format.sampleOffset = m_sample_format.sampleOffset;
    sample_format.containerSize = m_sample_format.containerSize;

    ret = edmidi_playFormat(m_synth, outSize / sample_format.containerSize,
                            m_io_buffer.data(),
                            m_io_buffer.data() + sample_format.containerSize,
                            &sample_format);

    copyGained(gain, m_io_buffer.data(), out, outSize);

    return ret * sample_format.containerSize;
}

size_t MDAudioEDMIDI::writeChunk(uint8_t *, size_t)
{
    return 0;
}
