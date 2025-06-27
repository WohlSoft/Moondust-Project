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

#include <string.h>
#include <SDL2/SDL_rwops.h>
#include <SDL2/SDL_audio.h>

#include <adlmidi.h>

#include "audio_midi_adl.h"


MDAudioADLMIDI::MDAudioADLMIDI() :
    MDAudioFile()
{}

MDAudioADLMIDI::~MDAudioADLMIDI()
{
    MDAudioADLMIDI::close();
}

uint32_t MDAudioADLMIDI::getCodecSpec() const
{
    return SPEC_READ | SPEC_SOURCE_ANY_RATE | SPEC_SOURCE_ANY_FORMAT | SPEC_LOOP_POINTS | SPEC_META_TAGS;
}

bool MDAudioADLMIDI::openRead(SDL_RWops *file)
{
    Uint8 *file_dump;
    size_t file_size;
    close();

    m_file = file;

    m_synth = adl_init(ADL_CHIP_SAMPLE_RATE);
    if(!m_synth)
    {
        m_lastError = std::string(adl_errorString());
        return false;
    }

    file_size = SDL_RWsize(m_file);
    file_dump = (Uint8*)SDL_malloc(file_size);
    SDL_RWread(m_file, file_dump, 1, file_size);
    SDL_RWseek(m_file, 0, SEEK_SET);

    adl_setBank(m_synth, 58);

    if(adl_openData(m_synth, file_dump, file_size) < 0)
    {
        SDL_free(file_dump);
        m_lastError = std::string(adl_errorInfo(m_synth));
        close();
        return false;
    }

    SDL_free(file_dump);

    adl_setLoopEnabled(m_synth, false);

    m_spec.m_channels = 2;
    m_spec.m_sample_format = AUDIO_S16SYS;
    m_spec.m_sample_rate = ADL_CHIP_SAMPLE_RATE;
    m_spec.m_total_length = (int64_t)(adl_totalTimeLength(m_synth) * m_spec.m_sample_rate);

    m_spec.m_loop_start = (int64_t)(adl_loopStartTime(m_synth) * m_spec.m_sample_rate);
    m_spec.m_loop_end = (int64_t)(adl_loopEndTime(m_synth) * m_spec.m_sample_rate);

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

bool MDAudioADLMIDI::openWrite(SDL_RWops *, const MDAudioFileSpec &)
{
    return false;
}

bool MDAudioADLMIDI::close()
{
    if(m_file)
    {
        if(m_synth)
        {
            adl_close(m_synth);
            m_synth = nullptr;
        }
        m_file = nullptr;
    }

    m_spec = MDAudioFileSpec();

    return true;
}

bool MDAudioADLMIDI::readRewind()
{
    adl_reset(m_synth);
    adl_positionRewind(m_synth);
    return true;
}

size_t MDAudioADLMIDI::readChunk(uint8_t *out, size_t outSize, bool *spec_changed)
{
    if(spec_changed)
        *spec_changed = false;

    if(adl_atEnd(m_synth))
        return 0;

    return adl_play(m_synth, outSize / sizeof(int16_t), (int16_t*)out) * sizeof(int16_t);
}

size_t MDAudioADLMIDI::writeChunk(uint8_t *, size_t)
{
    return false;
}
