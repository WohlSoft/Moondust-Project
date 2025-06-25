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
#ifndef AUDIO_PROCESSOR_H
#define AUDIO_PROCESSOR_H

#include <string>
#include <vector>
#include <memory>
#include "audio_format.h"
#include "audio_file.h"

struct SDL_RWops;
struct _SDL_AudioStream;
typedef struct _SDL_AudioStream SDL_AudioStream;

#define MD_AUDIO_CHUNK_SIZE 4096

class MoondustAudioProcessor
{
    std::string m_lastError;
    std::vector<uint8_t> m_in_buffer;
    std::vector<uint8_t> m_out_buffer;

    SDL_RWops *m_rw_in = nullptr;
    SDL_RWops *m_rw_out = nullptr;
    std::unique_ptr<MDAudioFile> m_in_file;
    std::unique_ptr<MDAudioFile> m_out_file;

    SDL_AudioStream *m_cvt_stream = nullptr;
    bool init_cvt_stream();

    double m_curChunk = 0;
    double m_numChunks = 0;

    bool m_done = false;

    std::string m_in_filePath;
    std::string m_out_filePath;

    int64_t m_stat_read = 0;
    int64_t m_stat_write = 0;

public:
    MoondustAudioProcessor();
    ~MoondustAudioProcessor();

    std::string getLastError() const;

    const MDAudioFileSpec &getInSpec() const;

    bool openInFile(const std::string &file, int *detectedFormat = nullptr);
    bool openOutFile(const std::string &file, int dstFormat, const MDAudioFileSpec &dstSpec);

    void close();

    uint32_t numChunks() const;
    uint32_t curChunk() const;

    int64_t getBytesReadStat() const;
    int64_t getBytesWrittenStat() const;

    int64_t getSamplesReadStat() const;
    int64_t getSamplesWrittenStat() const;

    bool rewindRead();

    bool runChunk(bool dry = false);

    bool done() const;

};

#endif // AUDIO_PROCESSOR_H
