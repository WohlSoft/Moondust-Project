#ifndef AUDIO_WAV_H
#define AUDIO_WAV_H

#include "../audio_file.h"

struct waveEncodeMeta;

class MDAudioWAV : public MDAudioFile
{
    void *m_wav = nullptr;
    bool m_cvt_needed = false;

    waveEncodeMeta *m_meta = nullptr;

    bool m_write = false;
    std::vector<double> m_double_cvt;

    void initChPermute();
    void reorderChannels(uint8_t *buff, size_t outSize);

public:
    enum EncodeTarget
    {
        ENCODE_DEFAULT = 0,
        // ENCODE_ALAW,
        // ENCODE_MULAW,
        // ENCODE_MS_ADPCM,
        // ENCODE_IMA_ADPCM,
        ENCODE_FLOAT64,
    };

    enum EncodeContainer
    {
        CONTAINER_RIFF = 0,
        CONTAINER_AIFF
    };

private:
    EncodeTarget m_target;
    EncodeContainer m_container;

public:
    MDAudioWAV(EncodeContainer container = CONTAINER_RIFF, EncodeTarget target = ENCODE_DEFAULT);
    virtual ~MDAudioWAV();

    uint32_t getCodecSpec() const override;

    bool openRead(SDL_RWops *file) override;
    bool openWrite(SDL_RWops *file, const MDAudioFileSpec &dstSpec) override;

    bool close() override;

    bool readRewind() override;

    size_t readChunk(uint8_t *out, size_t outSize, bool *spec_changed = nullptr) override;
    size_t writeChunk(uint8_t *in, size_t inSize) override;
};

#endif // AUDIO_WAV_H
