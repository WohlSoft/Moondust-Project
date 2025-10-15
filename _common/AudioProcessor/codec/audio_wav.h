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
