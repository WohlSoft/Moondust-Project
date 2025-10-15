#include <string.h>
#include <SDL2/SDL_rwops.h>
#include <SDL2/SDL_audio.h>

#include <opnmidi.h>
#include "OPNMIDI/gm_opn_bank.h"

#include "audio_midi_opn.h"

#define OPNMIDI_DEFAULT_CHIPS_COUNT     6

bool MDAudioOPNMIDI::reCreateSynth()
{
    if(!m_synth)
        opn2_close(m_synth);

    m_synth = opn2_init(m_spec.m_sample_rate);
    if(!m_synth)
    {
        m_lastError = std::string(opn2_errorString());
        return false;
    }

    return true;
}

bool MDAudioOPNMIDI::reInit()
{
    int err;

    if(!custom_bank_path.empty())
        err = opn2_openBankFile(m_synth, custom_bank_path.c_str());
    else
        err = opn2_openBankData(m_synth, g_gm_opn2_bank, sizeof(g_gm_opn2_bank));

    if(err < 0)
    {
        m_lastError = "OPNMIDI: ";
        m_lastError += opn2_errorInfo(m_synth);
        return false;
    }

    opn2_switchEmulator(m_synth, (emulator >= 0) ? emulator : OPNMIDI_EMU_MAME);
    opn2_setVolumeRangeModel(m_synth, volume_model);
    opn2_setFullRangeBrightness(m_synth, full_brightness_range);
    opn2_setSoftPanEnabled(m_synth, soft_pan);
    opn2_setAutoArpeggio(m_synth, auto_arpeggio);
    opn2_setChannelAllocMode(m_synth, alloc_mode);
    opn2_setNumChips(m_synth, chips_count > 0 ? chips_count : OPNMIDI_DEFAULT_CHIPS_COUNT);

    opn2_setTempo(m_synth, tempo);

    return true;
}

bool MDAudioOPNMIDI::reOpenFile()
{
    if(opn2_openData(m_synth, m_in_file.data(), m_in_file.size()) < 0)
    {
        m_lastError = "OPNMIDI: ";
        m_lastError += opn2_errorInfo(m_synth);
        return false;
    }

    opn2_setLoopEnabled(m_synth, false);

    return true;
}

MDAudioOPNMIDI::MDAudioOPNMIDI() :
    MDAudioFile()
{
    m_io_buffer.resize(4096);
}

MDAudioOPNMIDI::~MDAudioOPNMIDI()
{
    MDAudioOPNMIDI::close();
}

uint32_t MDAudioOPNMIDI::getCodecSpec() const
{
    return SPEC_READ | SPEC_SOURCE_ANY_RATE | SPEC_SOURCE_ANY_FORMAT | SPEC_LOOP_POINTS | SPEC_META_TAGS | SPEC_FIXED_CHANNELS;
}

bool MDAudioOPNMIDI::openRead(SDL_RWops *file)
{
    size_t file_size;
    close();

    // Init settings
    volume_model = m_args.getArgI("v", 0);
    volume_model = m_args.getArgI("l", volume_model); // alias
    alloc_mode = m_args.getArgI("o", -1);
    chips_count = m_args.getArgI("c", -1);
    full_brightness_range = m_args.getArgBI("r", 0);
    auto_arpeggio = m_args.getArgBI("j", 0);
    soft_pan = m_args.getArgBI("p", 1);
    emulator = m_args.getArgI("e", -1);
    custom_bank_path = m_args.getArgS("x=", std::string());
    tempo = m_args.getArgD("t=", 1.0);
    gain = m_args.getArgF("g=", 2.0f);

    m_file = file;

    m_spec.m_channels = 2;
    m_spec.m_sample_format = m_specWanted.getSampleFormat(AUDIO_F32SYS);
    m_spec.m_sample_rate = m_specWanted.getSampleRate(48000);

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

    switch (m_spec.m_sample_format)
    {
    case AUDIO_U8:
        m_sample_format.type = OPNMIDI_SampleType_U8;
        m_sample_format.containerSize = sizeof(Uint8);
        m_sample_format.sampleOffset = sizeof(Uint8) * 2;
        break;
    case AUDIO_S8:
        m_sample_format.type = OPNMIDI_SampleType_S8;
        m_sample_format.containerSize = sizeof(Sint8);
        m_sample_format.sampleOffset = sizeof(Sint8) * 2;
        break;
    case AUDIO_S16LSB:
    case AUDIO_S16MSB:
        m_sample_format.type = OPNMIDI_SampleType_S16;
        m_sample_format.containerSize = sizeof(Sint16);
        m_sample_format.sampleOffset = sizeof(Sint16) * 2;
        m_spec.m_sample_format = AUDIO_S16SYS;
        break;
    case AUDIO_U16LSB:
    case AUDIO_U16MSB:
        m_sample_format.type = OPNMIDI_SampleType_U16;
        m_sample_format.containerSize = sizeof(Uint16);
        m_sample_format.sampleOffset = sizeof(Uint16) * 2;
        m_spec.m_sample_format = AUDIO_U16SYS;
        break;
    case AUDIO_S32LSB:
    case AUDIO_S32MSB:
        m_sample_format.type = OPNMIDI_SampleType_S32;
        m_sample_format.containerSize = sizeof(Sint32);
        m_sample_format.sampleOffset = sizeof(Sint32) * 2;
        m_spec.m_sample_format = AUDIO_S32SYS;
        break;
    case AUDIO_F32LSB:
    case AUDIO_F32MSB:
    default:
        m_sample_format.type = OPNMIDI_SampleType_F32;
        m_sample_format.containerSize = sizeof(float);
        m_sample_format.sampleOffset = sizeof(float) * 2;
        m_spec.m_sample_format = AUDIO_F32SYS;
    }

    m_spec.m_loop_start = (int64_t)(opn2_loopStartTime(m_synth) * m_spec.m_sample_rate) / tempo;
    m_spec.m_loop_end = (int64_t)(opn2_loopEndTime(m_synth) * m_spec.m_sample_rate) / tempo;
    m_spec.m_total_length = (opn2_totalTimeLength(m_synth) * m_spec.m_sample_rate) / tempo;

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

    m_spec.m_meta_title = opn2_metaMusicTitle(m_synth);
    m_spec.m_meta_copyright = opn2_metaMusicCopyright(m_synth);

    return true;
}

bool MDAudioOPNMIDI::openWrite(SDL_RWops */*file*/, const MDAudioFileSpec &/*dstSpec*/)
{
    return false;
}

bool MDAudioOPNMIDI::close()
{
    if(m_file)
    {
        if(m_synth)
        {
            opn2_close(m_synth);
            m_synth = nullptr;
        }
        m_file = nullptr;
    }

    m_spec = MDAudioFileSpec();

    return true;
}

bool MDAudioOPNMIDI::readRewind()
{
    opn2_reset(m_synth);
    opn2_positionRewind(m_synth);

    // To avoid junk data being captured, re-init the synth from the scratch!
    reCreateSynth();
    reInit();
    reOpenFile();

    return true;
}

size_t MDAudioOPNMIDI::readChunk(uint8_t *out, size_t outSize, bool *spec_changed)
{
    struct OPNMIDI_AudioFormat sample_format;
    int ret = 0;

    if(spec_changed)
        *spec_changed = false;

    if(opn2_atEnd(m_synth))
        return 0;

    if(outSize > m_io_buffer.size())
        m_io_buffer.resize(outSize);

    sample_format.type = (OPNMIDI_SampleType)m_sample_format.type;
    sample_format.sampleOffset = m_sample_format.sampleOffset;
    sample_format.containerSize = m_sample_format.containerSize;

    ret = opn2_playFormat(m_synth, outSize / sample_format.containerSize,
                          m_io_buffer.data(),
                          m_io_buffer.data() + sample_format.containerSize,
                          &sample_format);

    copyGained(gain, m_io_buffer.data(), out, outSize);

    return ret * sample_format.containerSize;
}

size_t MDAudioOPNMIDI::writeChunk(uint8_t */*in*/, size_t /*inSize*/)
{
    return 0;
}
