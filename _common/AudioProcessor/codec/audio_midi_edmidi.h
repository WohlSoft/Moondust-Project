#ifndef AUDIO_MIDI_EDMIDI_H
#define AUDIO_MIDI_EDMIDI_H

#include "../audio_file.h"

struct EDMIDIPlayer;

class MDAudioEDMIDI : public MDAudioFile
{
    struct EDMIDIPlayer *m_synth = nullptr;

    int mods_num = 2;
    double tempo = 1.0f;
    float gain = 2.0f;

    bool reCreateSynth();
    bool reInit();
    bool reOpenFile();

    struct AudioFormat_t
    {
        /*! type of sample */
        int type;
        /*! size in bytes of the storage type */
        unsigned containerSize;
        /*! distance in bytes between consecutive samples */
        unsigned sampleOffset;
    } m_sample_format;

    std::vector<uint8_t> m_in_file;

public:
    MDAudioEDMIDI();
    virtual ~MDAudioEDMIDI();

    uint32_t getCodecSpec() const override;

    bool openRead(SDL_RWops *file) override;
    bool openWrite(SDL_RWops *file, const MDAudioFileSpec &dstSpec) override;

    bool close() override;

    bool readRewind() override;

    size_t readChunk(uint8_t *out, size_t outSize, bool *spec_changed = nullptr) override;
    size_t writeChunk(uint8_t *in, size_t inSize) override;
};

#endif // AUDIO_MIDI_EDMIDI_H
