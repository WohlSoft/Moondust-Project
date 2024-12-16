/*
 * XTConvert - a tool to package asset packs and episodes for TheXTech game engine.
 * Copyright (c) 2024 ds-sloth
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "export_audio.h"

#include <cstring>

#include <SDL2/SDL_mixer_ext.h>

#define QOA_NO_STDIO
#define QOA_IMPLEMENTATION
#include "qoa.h"

namespace XTConvert
{

namespace ExportAudio
{

// partly based off of ymhm.h by Aaron Giles (BSD-3 clause)
void export_wav(QByteArray& out, const Mix_Chunk* chunk)
{
    int samplerate;

    {
        Uint16 fmt;
        int chn;

        if(Mix_QuerySpec(&samplerate, &fmt, &chn) == 0 || chn != 2 || fmt != AUDIO_S16LSB)
        {
            out.clear();
            return;
        }
    }

    out.resize(44 + chunk->alen);

    // THIS STRONGLY ASSUMES LITTLE ENDIAN BUILD
    memcpy(&out.data()[0], "RIFF", 4);
    *(uint32_t *)&out.data()[4] = out.size() - 8;
    memcpy(&out.data()[8], "WAVE", 4);
    memcpy(&out.data()[12], "fmt ", 4);
    *(uint32_t *)&out.data()[16] = 16;
    *(uint16_t *)&out.data()[20] = 1;
    *(uint16_t *)&out.data()[22] = 2;
    *(uint32_t *)&out.data()[24] = samplerate;
    *(uint32_t *)&out.data()[28] = samplerate * 4;
    *(uint16_t *)&out.data()[32] = 4;
    *(uint16_t *)&out.data()[34] = 16;
    memcpy(&out.data()[36], "data", 4);
    *(uint32_t *)&out.data()[40] = out.size() - 44;

    memcpy(&out.data()[44], chunk->abuf, chunk->alen);
}

void export_qoa(QByteArray& out, const Mix_Chunk* chunk)
{
    int samplerate, chn;

    {
        Uint16 fmt;

        if(Mix_QuerySpec(&samplerate, &fmt, &chn) == 0 || chn > 2 || fmt != AUDIO_S16LSB)
        {
            out.clear();
            return;
        }
    }

    int sample_count = chunk->alen / (2 * chn);
    int16_t* abuf_in = (int16_t*)chunk->abuf;
    int16_t* use_abuf = abuf_in;

    // convert stereo to mono destructively
    if(chn == 2)
    {
        int16_t* abuf_out = (int16_t*)malloc(sample_count * sizeof(int16_t));

        if(!abuf_out)
        {
            out.clear();
            return;
        }

        for(int sample = 0; sample < sample_count; sample++)
        {
            int32_t left_sample = abuf_in[sample * 2 + 0];
            int32_t right_sample = abuf_in[sample * 2 + 1];

            abuf_out[sample] = static_cast<int16_t>((left_sample + right_sample) / 2);
        }

        use_abuf = abuf_out;
        chn = 1;
    }

    qoa_desc desc;
    desc.channels = chn;
    desc.samplerate = samplerate;
    desc.samples = sample_count;

    unsigned int out_len;
    uint8_t* encoded = (uint8_t*)qoa_encode(use_abuf, &desc, &out_len);

    if(use_abuf != abuf_in)
        free(use_abuf);

    if(!encoded)
    {
        out.clear();
        return;
    }

    out.resize(out_len);
    memcpy(&out.data()[0], encoded, out_len);

    QOA_FREE(encoded);
}

} // namespace ExportAudio

} // namespace XTConvert
