#ifndef AUDIO_OPUS_H
#define AUDIO_OPUS_H

#include "../audio_file.h"

class MDAudioOpus : public MDAudioFile
{
    bool m_write = false;

    int set_op_error(const char *function, int error);

    bool updateSection();
    void writeFlush();

public:
    MDAudioOpus();
    virtual ~MDAudioOpus();

    uint32_t getCodecSpec() const override;

    bool openRead(SDL_RWops *file) override;
    bool openWrite(SDL_RWops *file, const MDAudioFileSpec &dstSpec) override;

    bool close() override;

    bool readRewind() override;

    size_t readChunk(uint8_t *out, size_t outSize, bool *spec_changed = nullptr) override;
    size_t writeChunk(uint8_t *in, size_t inSize) override;
};

#endif // AUDIO_OPUS_H
