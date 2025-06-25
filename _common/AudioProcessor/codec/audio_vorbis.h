#ifndef AUDIO_VORBIS_H
#define AUDIO_VORBIS_H

#include "../audio_file.h"
#include <vorbis/vorbisfile.h>
#include <vorbis/vorbisenc.h>

class MDAudioVorbis : public MDAudioFile
{
    // Vorbis Read
    OggVorbis_File m_vf;
    vorbis_info m_vi;
    int m_section = -1;

    // Vorbis Write
    ogg_stream_state m_os;    //!< take physical pages, weld into a logical stream of packets
    ogg_page         m_og;    //!< one Ogg bitstream page.  Vorbis packets are inside
    ogg_packet       m_op;    //!< one raw packet of data for decode
    vorbis_comment   m_vc;    //!< struct that stores all the user comments
    vorbis_dsp_state m_vd;    //!< central working state for the packet->PCM decoder
    vorbis_block     m_vb;    //!< local working space for packet->PCM decode

    bool m_write = false;

    int set_ov_error(const char *function, int error);

    bool updateSection();
    void writeFlush();

public:
    MDAudioVorbis();
    virtual ~MDAudioVorbis();

    uint32_t getCodecSpec() const;

    bool openRead(SDL_RWops *file) override;
    bool openWrite(SDL_RWops *file, const MDAudioFileSpec &dstSpec) override;

    bool close() override;

    bool readRewind() override;

    size_t readChunk(uint8_t *out, size_t outSize, bool *spec_changed = nullptr) override;
    size_t writeChunk(uint8_t *in, size_t inSize) override;
};

#endif // AUDIO_VORBIS_H
