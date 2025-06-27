#ifndef AUDIO_QOA_H
#define AUDIO_QOA_H

#include <vector>

#include "../audio_file.h"
#include "qoa/qoa.h"

class MDAudioQOA : public MDAudioFile
{
    bool m_write = false;
    const bool m_encodeXQOA;

    // Decoder
    qoa_desc info;
    uint32_t first_frame_pos = 0;
    uint32_t sample_pos = 0;
    uint32_t sample_data_pos = 0;
    uint32_t sample_data_len = 0;
    std::vector<uint8_t> m_decode_buffer;
    std::vector<uint8_t> m_sample_buffer;
    uint32_t qoa_file_begin = 0;
    uint32_t qoa_file_size = 0;

    // Encoder
    uint32_t written_bytes = 0;
    uint32_t written_samples = 0;
    std::vector<uint8_t> m_encode_buffer;

    uint32_t decode_frame();
    uint32_t encode_frame();

public:
    MDAudioQOA(bool encodeXQOA = false);
    virtual ~MDAudioQOA();

    uint32_t getCodecSpec() const override;

    bool openRead(SDL_RWops *file) override;
    bool openWrite(SDL_RWops *file, const MDAudioFileSpec &dstSpec) override;

    bool close() override;

    bool readRewind() override;

    size_t readChunk(uint8_t *out, size_t outSize, bool *spec_changed = nullptr) override;
    size_t writeChunk(uint8_t *in, size_t inSize) override;
};

#endif // AUDIO_QOA_H
