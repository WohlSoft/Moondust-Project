#ifndef AUDIO_GME_H
#define AUDIO_GME_H

#include "../audio_file.h"

typedef struct Music_Emu Music_Emu;

class MDAudioGME : public MDAudioFile
{
    Music_Emu *m_synth = nullptr;
    int m_track_number = 0;
    int m_echo_disabled = 0;
    int m_echo_const = 0;
    bool m_has_track_length = false;
    int m_track_length = 0;
    int m_intro_length = 0;
    int m_loop_length = 0;

    double tempo = 1.0f;
    float gain = 1.0f;

    size_t pos = 0;

    std::vector<uint8_t> m_in_file;

    bool initialize_from_track_info();

public:
    MDAudioGME();
    virtual ~MDAudioGME();

    uint32_t getCodecSpec() const override;

    bool openRead(SDL_RWops *file) override;
    bool openWrite(SDL_RWops *file, const MDAudioFileSpec &dstSpec) override;

    bool close() override;

    bool readRewind() override;

    size_t readChunk(uint8_t *out, size_t outSize, bool *spec_changed = nullptr) override;
    size_t writeChunk(uint8_t *in, size_t inSize) override;
};

#endif // AUDIO_GME_H
