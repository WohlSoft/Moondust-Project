#ifndef AUDIO_FLAC_H
#define AUDIO_FLAC_H

#include "../audio_file.h"

struct MDAudioFLAC_callbacks;

class MDAudioFLAC : public MDAudioFile
{
    friend struct MDAudioFLAC_callbacks;

    // Decode
    void *m_flac_dec = nullptr;
    uint8_t *m_dec_dst;
    size_t m_dec_size;
    int m_init_state = 0;
    int m_bits_per_sample = 0;
    size_t m_buffer_left = 0;
    size_t m_buffer_pos = 0;

    // Encode
    void *m_flac_enc = nullptr;
    bool m_write = false;

public:
    MDAudioFLAC();
    virtual ~MDAudioFLAC();

    uint32_t getCodecSpec() const override;

    bool openRead(SDL_RWops *file) override;
    bool openWrite(SDL_RWops *file, const MDAudioFileSpec &dstSpec) override;

    bool close() override;

    bool readRewind() override;

    size_t readChunk(uint8_t *out, size_t outSize, bool *spec_changed = nullptr) override;
    size_t writeChunk(uint8_t *in, size_t inSize) override;
};

#endif // AUDIO_FLAC_H
