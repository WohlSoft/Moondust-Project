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

#include <fluidsynth.h>
#include "midi_seq/mix_midi_seq.h"

#include "audio_midi_fluidsynth.h"

/****************************************************
 *           Real-Time MIDI calls proxies           *
 ****************************************************/

struct MDAudioFluidSynthCallbacks
{
    static void rtNoteOn(void *userdata, uint8_t channel, uint8_t note, uint8_t velocity)
    {
        MDAudioFluidSynth *music = reinterpret_cast<MDAudioFluidSynth*>(userdata);
        fluid_synth_noteon(music->m_synth, channel, note, velocity);
    }

    static void rtNoteOff(void *userdata, uint8_t channel, uint8_t note)
    {
        MDAudioFluidSynth *music = reinterpret_cast<MDAudioFluidSynth*>(userdata);
        fluid_synth_noteoff(music->m_synth, channel, note);
    }

    static void rtNoteAfterTouch(void *userdata, uint8_t channel, uint8_t note, uint8_t atVal)
    {
        MDAudioFluidSynth *music = reinterpret_cast<MDAudioFluidSynth*>(userdata);
        fluid_synth_key_pressure(music->m_synth, channel, note, atVal);
    }

    static void rtChannelAfterTouch(void *userdata, uint8_t channel, uint8_t atVal)
    {
        MDAudioFluidSynth *music = reinterpret_cast<MDAudioFluidSynth*>(userdata);
        fluid_synth_channel_pressure(music->m_synth, channel, atVal);
    }

    static void rtControllerChange(void *userdata, uint8_t channel, uint8_t type, uint8_t value)
    {
        MDAudioFluidSynth *music = reinterpret_cast<MDAudioFluidSynth*>(userdata);
        fluid_synth_cc(music->m_synth, channel, type, value);
    }

    static void rtPatchChange(void *userdata, uint8_t channel, uint8_t patch)
    {
        MDAudioFluidSynth *music = reinterpret_cast<MDAudioFluidSynth*>(userdata);
        fluid_synth_program_change(music->m_synth, channel, patch);
    }

    static void rtPitchBend(void *userdata, uint8_t channel, uint8_t msb, uint8_t lsb)
    {
        MDAudioFluidSynth *music = reinterpret_cast<MDAudioFluidSynth*>(userdata);
        fluid_synth_pitch_bend(music->m_synth, channel, (msb << 7) | lsb);
    }

    static void rtSysEx(void *userdata, const uint8_t *msg, size_t size)
    {
        MDAudioFluidSynth *music = reinterpret_cast<MDAudioFluidSynth*>(userdata);
        fluid_synth_sysex(music->m_synth, (const char*)(msg), (int)(size), NULL, NULL, NULL, 0);
    }

    static void playSynthBuffer(void *userdata, uint8_t *stream, size_t length)
    {
        MDAudioFluidSynth *music = reinterpret_cast<MDAudioFluidSynth*>(userdata);
        int samples = (int)length / (music->sample_size * 2);

        if(music->m_synth_write_ret < 0)
            return;

        music->m_synth_write_ret = music->synth_write(music->m_synth, samples, stream, 0, 2, stream, 1, 2);
    }

    static void all_notes_mute(fluid_synth_t *synth)
    {
        if(!synth)
            return; /* Nothing to do */

        for(int channel = 0; channel < 16; channel++)
            fluid_synth_cc(synth, channel, 123, 0);

        fluid_synth_system_reset(synth);
    }
};

void MDAudioFluidSynth::replace_colon_to_semicolon(std::string &str)
{
    for(char &c : str)
    {
        if(c == '&')
            c = ';';
    }
}

void MDAudioFluidSynth::init_interface()
{
    int in_format = AUDIO_S16SYS;

    if(m_seq)
        return;

    m_seq = (BW_MidiRtInterface*)SDL_malloc(sizeof(BW_MidiRtInterface));
    SDL_memset(m_seq, 0, sizeof(BW_MidiRtInterface));

    /* seq->onDebugMessage             = hooks.onDebugMessage; */
    /* seq->onDebugMessage_userData    = hooks.onDebugMessage_userData; */

    /* MIDI Real-Time calls */
    m_seq->rtUserData = (void *)this;
    m_seq->rt_noteOn  = MDAudioFluidSynthCallbacks::rtNoteOn;
    m_seq->rt_noteOff = MDAudioFluidSynthCallbacks::rtNoteOff;
    m_seq->rt_noteAfterTouch = MDAudioFluidSynthCallbacks::rtNoteAfterTouch;
    m_seq->rt_channelAfterTouch = MDAudioFluidSynthCallbacks::rtChannelAfterTouch;
    m_seq->rt_controllerChange = MDAudioFluidSynthCallbacks::rtControllerChange;
    m_seq->rt_patchChange = MDAudioFluidSynthCallbacks::rtPatchChange;
    m_seq->rt_pitchBend = MDAudioFluidSynthCallbacks::rtPitchBend;
    m_seq->rt_systemExclusive = MDAudioFluidSynthCallbacks::rtSysEx;

    m_seq->onPcmRender = MDAudioFluidSynthCallbacks::playSynthBuffer;
    m_seq->pcmSampleRate = m_spec.m_sample_rate;

    synth_write = fluid_synth_write_s16;
    sample_size = sizeof(Sint16);
    m_seq->pcmFrameSize = 2 * sample_size;
    m_seq->onPcmRender_userData = this;

    if(m_spec.m_sample_format & 0x0020) /* 32 bit. */
    {
        synth_write = fluid_synth_write_float;
        sample_size <<= 1;
        m_seq->pcmFrameSize <<= 1;
        in_format = AUDIO_F32SYS;
    }

    m_spec.m_sample_format = in_format;
}

bool MDAudioFluidSynth::loadSoundFonts()
{
    size_t left = 0, right = 0, num = 0;
    std::string p;
    int ret;

    while(right != std::string::npos)
    {
        right = m_bank_paths.find(';', left);

        if(right == std::string::npos)
            p = m_bank_paths.substr(left);
        else
            p = m_bank_paths.substr(left, right - left);

        if(!p.empty())
        {
            ret = fluid_synth_sfload(m_synth, p.c_str(), 1);
            if(ret != FLUID_FAILED)
                num++;
        }

        if(right != std::string::npos)
            left = right + 1;
    }

    return num > 0;
}


MDAudioFluidSynth::MDAudioFluidSynth() :
    MDAudioFile()
{}

MDAudioFluidSynth::~MDAudioFluidSynth()
{
    MDAudioFluidSynth::close();
}

uint32_t MDAudioFluidSynth::getCodecSpec() const
{
    return SPEC_READ | SPEC_SOURCE_ANY_RATE | SPEC_SOURCE_ANY_FORMAT | SPEC_LOOP_POINTS | SPEC_META_TAGS | SPEC_FIXED_CHANNELS;
}

bool MDAudioFluidSynth::openRead(SDL_RWops *file)
{
    size_t file_size;
    double samplerate; /* as set by the lib. */
    close();

    chorus = m_args.getArgBI("c", 1);
    chorus_nr = m_args.getArgI("cn", 3);
    chorus_level = m_args.getArgF("cl", 2.0f);
    chorus_speed = m_args.getArgF("cs", 0.3f);
    chorus_depth = m_args.getArgF("cd", 8.0f);
    chorus_type = m_args.getArgI("ct", 0);

    reverb = m_args.getArgBI("r", 1);
    reverb_roomsize = m_args.getArgF("rr", 0.2f);
    reverb_damping = m_args.getArgF("rd", 0.0f);
    reverb_width = m_args.getArgF("rw", 0.5f);
    reverb_level = m_args.getArgF("rl", 0.9f);

    polyphony = m_args.getArgI("p", 256);

    tempo = m_args.getArgD("t=", 1.0f);
    gain = m_args.getArgD("g=", 1.0f);

    m_bank_paths = m_args.getArgS("x=", std::string());
    replace_colon_to_semicolon(m_bank_paths);

    m_file = file;

    m_spec.m_channels = 2;
    m_spec.m_sample_rate = m_specWanted.getSampleRate(48000);
    m_spec.m_sample_format = m_specWanted.getSampleFormat(AUDIO_F32SYS);

    init_interface();

    m_settings = new_fluid_settings();
    if(!m_settings)
    {
        m_lastError = "Failed to create FluidSynth settings";
        close();
        return false;
    }

    fluid_settings_setnum(m_settings, "synth.sample-rate", (double)m_spec.m_sample_rate);
    fluid_settings_getnum(m_settings, "synth.sample-rate", &samplerate);
    m_spec.m_sample_rate = samplerate;
    m_seq->pcmSampleRate = samplerate;

    if(m_bank_paths.empty())
    {
        m_lastError = "FluidSynth: Can't use library since no sound fonts provided";
        close();
        return false;
    }

    m_synth = new_fluid_synth(m_settings);

    if(!m_synth)
    {
        m_lastError = "Failed to create FluidSynth synthesizer";
        close();
        return false;
    }

    if(!loadSoundFonts())
    {
        m_lastError = "Failed to load SoundFonts for FluidSynth";
        close();
        return false;
    }

    fluid_synth_set_reverb_on(m_synth, reverb);
    fluid_synth_set_reverb(m_synth, reverb_roomsize, reverb_damping, reverb_width, reverb_level);
    fluid_synth_set_chorus_on(m_synth, chorus);
    fluid_synth_set_chorus(m_synth, chorus_nr, chorus_level, chorus_speed, chorus_depth, chorus_type);
    fluid_synth_set_polyphony(m_synth, polyphony);

    m_player = midi_seq_init_interface(m_seq);
    if(!m_player)
    {
        m_lastError = "Failed to initialize sequencer for FluidSynth";
        close();
        return false;
    }

    file_size = SDL_RWsize(m_file);
    m_in_file.resize(file_size);
    SDL_RWread(m_file, m_in_file.data(), 1, file_size);
    SDL_RWseek(m_file, 0, SEEK_SET);

    if(midi_seq_openData(m_player, m_in_file.data(), m_in_file.size()) < 0)
    {
        m_lastError = "Failed to load song for FluidSynth: ";
        m_lastError += midi_seq_get_error(m_player);
        close();
        return false;
    }

    midi_seq_set_tempo_multiplier(m_player, tempo);
    midi_seq_set_loop_enabled(m_player, 0);
    fluid_synth_set_gain(m_synth, gain);

    m_spec.m_meta_title = midi_seq_meta_title(m_player);
    m_spec.m_meta_copyright = midi_seq_meta_copyright(m_player);

    m_spec.m_loop_start = (midi_seq_loop_start(m_player) * m_spec.m_sample_rate) / tempo;
    m_spec.m_loop_end = (midi_seq_loop_end(m_player) * m_spec.m_sample_rate) / tempo;
    m_spec.m_total_length = (midi_seq_length(m_player) * m_spec.m_sample_rate) / tempo;

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

bool MDAudioFluidSynth::openWrite(SDL_RWops *, const MDAudioFileSpec &)
{
    return false;
}

bool MDAudioFluidSynth::close()
{
    if(m_player)
    {
        midi_seq_free(m_player);
        m_player = nullptr;
    }

    if(m_synth)
    {
        delete_fluid_synth(m_synth);
        m_synth = nullptr;
    }

    if(m_settings)
    {
        delete_fluid_settings(m_settings);
        m_settings = nullptr;
    }

    if(m_seq)
    {
        SDL_free(m_seq);
        m_seq = nullptr;
    }

    m_file = nullptr;
    m_in_file.clear();

    return true;
}

bool MDAudioFluidSynth::readRewind()
{
    midi_seq_rewind(m_player);
    MDAudioFluidSynthCallbacks::all_notes_mute(m_synth);
    fluid_synth_set_gain(m_synth, gain);
    return true;
}

size_t MDAudioFluidSynth::readChunk(uint8_t *out, size_t outSize, bool *)
{
    int got_length = midi_seq_play_buffer(m_player, out, outSize);

    if(m_synth_write_ret < 0)
    {
        m_lastError = "Error generating FluidSynth Audio";
        return MDAudioFile::r_error;
    }

    if(got_length <= 0)
        return 0;

    return got_length;
}

size_t MDAudioFluidSynth::writeChunk(uint8_t *, size_t)
{
    return 0;
}
