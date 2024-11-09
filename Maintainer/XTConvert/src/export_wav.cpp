#include "export_wav.h"

#include <cstring>

#include <SDL2/SDL_mixer_ext.h>

namespace XTConvert
{

namespace ExportWAV
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
    memcpy(&out[0], "RIFF", 4);
    *(uint32_t *)&out[4] = out.size() - 8;
    memcpy(&out[8], "WAVE", 4);
    memcpy(&out[12], "fmt ", 4);
    *(uint32_t *)&out[16] = 16;
    *(uint16_t *)&out[20] = 1;
    *(uint16_t *)&out[22] = 2;
    *(uint32_t *)&out[24] = samplerate;
    *(uint32_t *)&out[28] = samplerate * 4;
    *(uint16_t *)&out[32] = 4;
    *(uint16_t *)&out[34] = 16;
    memcpy(&out[36], "data", 4);
    *(uint32_t *)&out[40] = out.size() - 44;

    memcpy(&out[44], chunk->abuf, chunk->alen);
}

} // namespace ExportWAV

} // namespace XTConvert
