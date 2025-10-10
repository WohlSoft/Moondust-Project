#ifndef AUDIO_MP3_H
#define AUDIO_MP3_H

#include "../audio_file.h"

struct mp3file_t;
struct mpg123_handle_struct;
typedef struct mpg123_handle_struct mpg123_handle;

struct lame_global_struct;
typedef struct lame_global_struct lame_global_flags;
typedef lame_global_flags *lame_t;

class MDAudioMP3 : public MDAudioFile
{
    // Decoder
    struct mp3file_t *m_mp3file = nullptr;
    mpg123_handle *m_handle = nullptr;

    // Encoder
    lame_t m_lame = nullptr;

    bool m_write = false;
    size_t m_written = 0;

    const char* mpg_err(mpg123_handle* mpg, int result);

public:
    MDAudioMP3();
    virtual ~MDAudioMP3() override;

    uint32_t getCodecSpec() const override;

    bool openRead(SDL_RWops *file) override;
    bool openWrite(SDL_RWops *file, const MDAudioFileSpec &dstSpec) override;

    bool close() override;

    bool readRewind() override;

    size_t readChunk(uint8_t *out, size_t outSize, bool *spec_changed = nullptr) override;
    size_t writeChunk(uint8_t *in, size_t inSize) override;
};

#endif // AUDIO_MP3_H
