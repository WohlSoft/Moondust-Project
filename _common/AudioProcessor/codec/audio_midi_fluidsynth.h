#ifndef AUDIO_MIDI_FLUIDSYNTH_H
#define AUDIO_MIDI_FLUIDSYNTH_H

#include "../audio_file.h"

typedef struct BW_MidiRtInterface BW_MidiRtInterface;
typedef struct _fluid_synth_t fluid_synth_t;
typedef struct _fluid_hashtable_t fluid_settings_t;

class MDAudioFluidSynth : public MDAudioFile
{
    fluid_synth_t *m_synth = nullptr;
    fluid_settings_t *m_settings = nullptr;
    BW_MidiRtInterface *m_seq = nullptr;
    void *m_player = nullptr;

    int (*synth_write)(fluid_synth_t*, int, void*, int, int, void*, int, int);
    int m_synth_write_ret = 0;
    int sample_size = 0;

    friend struct MDAudioFluidSynthCallbacks;

    int     chorus = 1;
    int     chorus_nr = 3;
    float   chorus_level  = 2.0f;
    float   chorus_speed = 0.3f;
    float   chorus_depth = 8.0f;
    int     chorus_type = 0;

    int     reverb = 1;
    float   reverb_roomsize = 0.2f;
    float   reverb_damping = 0.0f;
    float   reverb_width = 0.5f;
    float   reverb_level = 0.9f;

    int     polyphony = 256;

    double  tempo = 1.0f;
    float   gain = 1.0f;

    std::string m_bank_paths;

    std::vector<uint8_t> m_in_file;

    static void replace_colon_to_semicolon(std::string &str);
    void init_interface();

    bool loadSoundFonts();

public:
    MDAudioFluidSynth();
    virtual ~MDAudioFluidSynth();

    uint32_t getCodecSpec() const override;

    bool openRead(SDL_RWops *file) override;
    bool openWrite(SDL_RWops *file, const MDAudioFileSpec &dstSpec) override;

    bool close() override;

    bool readRewind() override;

    size_t readChunk(uint8_t *out, size_t outSize, bool *spec_changed = nullptr) override;
    size_t writeChunk(uint8_t *in, size_t inSize) override;
};

#endif // AUDIO_MIDI_FLUIDSYNTH_H
