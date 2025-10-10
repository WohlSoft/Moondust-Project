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

#ifndef AUDIO_QOA_H
#define AUDIO_QOA_H

#include <vector>

#include "../audio_file.h"

#ifndef QOA_H
#   define QOA_MAX_CHANNELS 8
#   define QOA_LMS_LEN 4
#   define QOA_FRAME_SIZE(channels, slices) \
        (8 + QOA_LMS_LEN * 4 * channels + 8 * slices * channels)

typedef struct
{
    int history[QOA_LMS_LEN];
    int weights[QOA_LMS_LEN];
} qoa_lms_t;

typedef struct
{
    unsigned int channels;
    unsigned int samplerate;
    unsigned int samples;
    qoa_lms_t lms[QOA_MAX_CHANNELS];
#ifdef QOA_RECORD_TOTAL_ERROR
    double error;
#endif
} qoa_desc;
#endif

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
