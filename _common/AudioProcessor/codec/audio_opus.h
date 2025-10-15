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
#ifndef AUDIO_OPUS_H
#define AUDIO_OPUS_H

#include "../audio_file.h"

typedef struct OggOpusFile      OggOpusFile;
typedef struct OpusHead         OpusHead;
typedef struct OggOpusEnc       OggOpusEnc;
typedef struct OggOpusComments  OggOpusComments;

class MDAudioOpus : public MDAudioFile
{
    // Opus Read
    OggOpusFile *m_of = nullptr;
    const OpusHead *m_op_info = nullptr;
    int m_section = -1;

    // Opus write
    OggOpusEnc *m_enc = nullptr;
    OggOpusComments *m_out_comments = nullptr;
    size_t m_written = 0;

    bool m_write = false;

    int set_op_error(const char *function, int error);

    bool updateSection();
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
