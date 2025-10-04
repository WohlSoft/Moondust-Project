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

#define ADLMIDI_DEFAULT_CHIPS_COUNT     4

bool MDAudioADLMIDI::reCreateSynth()
{
    if(!m_synth)
        adl_close(m_synth);

    m_synth = adl_init(m_spec.m_sample_rate);
    if(!m_synth)
    {
        m_lastError = std::string(adl_errorString());
        return false;
    }

    return true;
}

bool MDAudioADLMIDI::reInit()
{
    int err;

    adl_setHVibrato(m_synth, vibrato);
    adl_setHTremolo(m_synth, tremolo);

    if(!custom_bank_path.empty())
        err = adl_openBankFile(m_synth, custom_bank_path.c_str());
    else
        err = adl_setBank(m_synth, bank);

    if(err < 0)
    {
        m_lastError = "ADLMIDI: ";
        m_lastError += adl_errorInfo(m_synth);
        return false;
    }

    adl_switchEmulator(m_synth, (emulator >= 0) ? emulator : ADLMIDI_EMU_DOSBOX );
    adl_setScaleModulators(m_synth, scalemod);
    adl_setVolumeRangeModel(m_synth, volume_model);
    adl_setFullRangeBrightness(m_synth, full_brightness_range);
    adl_setSoftPanEnabled(m_synth, soft_pan);
    adl_setAutoArpeggio(m_synth, auto_arpeggio);
    adl_setChannelAllocMode(m_synth, alloc_mode);
    adl_setNumChips(m_synth, chips_count > 0 ? chips_count : ADLMIDI_DEFAULT_CHIPS_COUNT);

    if(four_op_channels)
        adl_setNumFourOpsChn(m_synth, four_op_channels);

    adl_setTempo(m_synth, tempo);

    return true;
}

bool MDAudioADLMIDI::reOpenFile()
{
    if(adl_openData(m_synth, m_in_file.data(), m_in_file.size()) < 0)
    {
        m_lastError = "ADLMIDI: ";
        m_lastError += adl_errorInfo(m_synth);
        return false;
    }

    adl_setLoopEnabled(m_synth, false);

    return true;
}

MDAudioADLMIDI::MDAudioADLMIDI() :
    MDAudioFile()
{
    m_read_buffer.resize(4096);
}

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
    size_t file_size;
    close();

    // Init settings
    bank = getArgI("b", 58);
    tremolo = getArgI("t", -1);
    vibrato = getArgI("v", -1);
    scalemod = getArgI("m", -1);
    volume_model = getArgI("l", 0);
    alloc_mode = getArgI("o", -1);
    chips_count = getArgI("c", -1);
    four_op_channels = getArgI("f", -1);
    full_brightness_range = getArgI("r", 0);
    soft_pan = getArgI("p", 1);
    emulator = getArgI("e", -1);
    custom_bank_path = getArgS("x=", std::string());
    tempo = getArgD("t=", 1.0);
    gain = getArgF("g=", 2.0f);

    m_file = file;

    m_spec.m_channels = 2;
    m_spec.m_sample_format = m_specWanted.getSampleFormat(AUDIO_S16SYS);
    m_spec.m_sample_rate = m_specWanted.getSampleRate(ADL_CHIP_SAMPLE_RATE);
    m_spec.m_total_length = (int64_t)(adl_totalTimeLength(m_synth) * m_spec.m_sample_rate);

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

    m_spec.m_loop_start = (int64_t)(adl_loopStartTime(m_synth) * m_spec.m_sample_rate);
    m_spec.m_loop_end = (int64_t)(adl_loopEndTime(m_synth) * m_spec.m_sample_rate);

    switch (m_spec.m_sample_format)
    {
    case AUDIO_U8:
        m_sample_format.type = ADLMIDI_SampleType_U8;
        m_sample_format.containerSize = sizeof(Uint8);
        m_sample_format.sampleOffset = sizeof(Uint8) * 2;
        break;
    case AUDIO_S8:
        m_sample_format.type = ADLMIDI_SampleType_S8;
        m_sample_format.containerSize = sizeof(Sint8);
        m_sample_format.sampleOffset = sizeof(Sint8) * 2;
        break;
    case AUDIO_S16LSB:
    case AUDIO_S16MSB:
        m_sample_format.type = ADLMIDI_SampleType_S16;
        m_sample_format.containerSize = sizeof(Sint16);
        m_sample_format.sampleOffset = sizeof(Sint16) * 2;
        m_spec.m_sample_format = AUDIO_S16SYS;
        break;
    case AUDIO_U16LSB:
    case AUDIO_U16MSB:
        m_sample_format.type = ADLMIDI_SampleType_U16;
        m_sample_format.containerSize = sizeof(Uint16);
        m_sample_format.sampleOffset = sizeof(Uint16) * 2;
        m_spec.m_sample_format = AUDIO_U16SYS;
        break;
    case AUDIO_S32LSB:
    case AUDIO_S32MSB:
        m_sample_format.type = ADLMIDI_SampleType_S32;
        m_sample_format.containerSize = sizeof(Sint32);
        m_sample_format.sampleOffset = sizeof(Sint32) * 2;
        m_spec.m_sample_format = AUDIO_S32SYS;
        break;
    case AUDIO_F32LSB:
    case AUDIO_F32MSB:
    default:
        m_sample_format.type = ADLMIDI_SampleType_F32;
        m_sample_format.containerSize = sizeof(float);
        m_sample_format.sampleOffset = sizeof(float) * 2;
        m_spec.m_sample_format = AUDIO_F32SYS;
    }

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

    // To avoid junk data being captured, re-init the synth from the scratch!
    reCreateSynth();
    reInit();
    reOpenFile();

    return true;
}

size_t MDAudioADLMIDI::readChunk(uint8_t *out, size_t outSize, bool *spec_changed)
{
    struct ADLMIDI_AudioFormat sample_format;
    int ret = 0;

    if(spec_changed)
        *spec_changed = false;

    if(adl_atEnd(m_synth))
        return 0;

    if(outSize > m_read_buffer.size())
        m_read_buffer.resize(outSize);

    sample_format.type = (ADLMIDI_SampleType)m_sample_format.type;
    sample_format.sampleOffset = m_sample_format.sampleOffset;
    sample_format.containerSize = m_sample_format.containerSize;

    ret = adl_playFormat(m_synth, outSize / sample_format.containerSize,
                         m_read_buffer.data(),
                         m_read_buffer.data() + sample_format.containerSize,
                         &sample_format);

    copyGained(gain, m_read_buffer.data(), out, outSize);

    return ret * sample_format.containerSize;
}

size_t MDAudioADLMIDI::writeChunk(uint8_t *, size_t)
{
    return false;
}
