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

#ifndef AUDIO_FILE_H
#define AUDIO_FILE_H

#include <stdint.h>
#include <string>

struct SDL_RWops;

struct MDAudioFileSpec
{
    uint64_t m_total_length = 0;
    uint64_t m_loop_start = 0;
    uint64_t m_loop_end = 0;

    int      m_sample_format = 0;
    int      m_sample_rate = 0;

    std::string m_meta_title;
    std::string m_meta_artist;
    std::string m_meta_album;
    std::string m_meta_copyright;
};

class MDAudioFile
{
protected:
    SDL_RWops       *m_file = nullptr;
    uint64_t        m_position = 0;
    MDAudioFileSpec m_spec;
    std::string     m_lastError;

public:
    MDAudioFile();
    virtual ~MDAudioFile();

    virtual std::string getLastError();

    virtual bool openRead(SDL_RWops *file) = 0;
    virtual bool openWrite(SDL_RWops *file, const MDAudioFileSpec &dstSpec) = 0;

    virtual bool close() = 0;

    virtual size_t readChunk(uint8_t *out, size_t outSize) = 0;
    virtual size_t writeChunk(uint8_t *in, size_t inSize) = 0;

    const MDAudioFileSpec &getSpec() const;
};

#endif // AUDIO_FILE_H
