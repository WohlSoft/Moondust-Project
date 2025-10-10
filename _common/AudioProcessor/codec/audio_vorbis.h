#ifndef AUDIO_VORBIS_H
#define AUDIO_VORBIS_H

#include "../audio_file.h"

struct MDAudioVorbis_private;

class MDAudioVorbis : public MDAudioFile
{
    // internals
    MDAudioVorbis_private *p = nullptr;

    bool m_write = false;

    int set_ov_error(const char *function, int error);

    bool updateSection();
    void writeFlush();

public:
    MDAudioVorbis();
    virtual ~MDAudioVorbis();

    uint32_t getCodecSpec() const override;

    bool openRead(SDL_RWops *file) override;
    bool openWrite(SDL_RWops *file, const MDAudioFileSpec &dstSpec) override;

    bool close() override;

    bool readRewind() override;

    size_t readChunk(uint8_t *out, size_t outSize, bool *spec_changed = nullptr) override;
    size_t writeChunk(uint8_t *in, size_t inSize) override;
};

#endif // AUDIO_VORBIS_H
