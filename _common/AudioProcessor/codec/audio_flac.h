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
    void *m_flac_enc_metadata[2] = {nullptr, nullptr};
    bool m_write = false;

public:
    enum EncodeContainer
    {
        ENCODE_FLAC_NATIVE = 0,
        ENCODE_FLAC_OGG
    };
private:
    EncodeContainer m_container = ENCODE_FLAC_NATIVE;

public:
    MDAudioFLAC(EncodeContainer container = ENCODE_FLAC_NATIVE);
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
