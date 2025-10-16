/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2025 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#pragma once
#ifndef AUDIO_FFMPEG_H
#define AUDIO_FFMPEG_H

#include "../audio_file.h"

struct MDAudioFFMPEG_private;
typedef struct AVPacket AVPacket;

class MDAudioFFMPEG : public MDAudioFile
{
    MDAudioFFMPEG_private *p = nullptr;
    size_t m_buffer_left = 0;
    size_t m_buffer_pos = 0;

    bool updateStream();

    bool m_write = false;

    static int _rw_read_buffer(void *opaque, uint8_t *buf, int buf_size);
    static int64_t _rw_seek(void *opaque, int64_t offset, int whence);

    int decode_packet(const AVPacket *pkt, bool *got_some);

public:
    enum EncodeTarget
    {
        ENCODE_UNDEFINED = 0,
        ENCODE_WMAv1,
        ENCODE_WMAv2,
        ENCODE_WAV_MULAW,
        ENCODE_WAV_ULAW,
        ENCODE_WAV_ADPCM_MS,
        ENCODE_WAV_ADPCM_IMA,
        ENCODE_AIFF
    };
private:
    const EncodeTarget m_target;

public:
    MDAudioFFMPEG(EncodeTarget encode = ENCODE_UNDEFINED);
    virtual ~MDAudioFFMPEG();

    uint32_t getCodecSpec() const override;

    bool openRead(SDL_RWops *file) override;
    bool openWrite(SDL_RWops *file, const MDAudioFileSpec &dstSpec) override;

    bool close() override;

    bool readRewind() override;

    size_t readChunk(uint8_t *out, size_t outSize, bool *spec_changed = nullptr) override;
    size_t writeChunk(uint8_t *in, size_t inSize) override;
};

#endif // AUDIO_FFMPEG_H
