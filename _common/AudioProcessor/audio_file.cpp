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

#include <SDL2/SDL_stdinc.h>
#include "audio_file.h"

bool MDAudioFile::isLoopTag(const char *tag)
{
    char buf[5];
    SDL_strlcpy(buf, tag, 5);
    return SDL_strcasecmp(buf, "LOOP") == 0;
}

int64_t MDAudioFile::parseTime(char *time, long samplerate_hz)
{
    char *num_start, *p;
    int64_t result;
    char c;
    int val;

    /* Time is directly expressed as a sample position */
    if (SDL_strchr(time, ':') == nullptr)
        return SDL_strtoll(time, nullptr, 10);

    result = 0;
    num_start = time;

    for(p = time; *p != '\0'; ++p)
    {
        if(*p == '.' || *p == ':')
        {
            c = *p; *p = '\0';
            if((val = SDL_atoi(num_start)) < 0)
                return -1;

            result = result * 60 + val;
            num_start = p + 1;
            *p = c;
        }

        if(*p == '.')
        {
            double val_f = SDL_atof(p);

            if(val_f < 0)
                return -1;

            return result * samplerate_hz + (Sint64) (val_f * samplerate_hz);
        }
    }

    if((val = SDL_atoi(num_start)) < 0)
        return -1;

    return (result * 60 + val) * samplerate_hz;
}

static size_t _utf16_byte_len(const char *str)
{
    size_t len = 0;
    const char *cur = str;
    if (!cur)
        return 0;

    while(cur[0] != '\0' || cur[1] != '\0')
    {
        len += 2;
        cur += 2;
    }
    return len;
}

std::string MDAudioFile::parseidiMetaTag(const char *src)
{
    std::string ret;
    size_t src_len = SDL_strlen(src);
    char *dst = nullptr;

    if(src_len >= 3 && (SDL_memcmp(src, "\xEF\xBB\xBF", 3) == 0))
        dst = SDL_strdup(src + 3);
    else if (src_len >= 2 && (SDL_memcmp(src, "\xFF\xFE", 2) == 0))
        dst = SDL_iconv_string("UTF-8", "UCS-2LE", src, _utf16_byte_len(src) + 2);
    else if (src_len >= 2 && (SDL_memcmp(src, "\xFE\xFF", 2) == 0))
        dst = SDL_iconv_string("UTF-8", "UCS-2BE", src, _utf16_byte_len(src) + 2);
    else
        dst = SDL_iconv_string("UTF-8", "ISO-8859-1", src, SDL_strlen(src) + 1);

    if(dst)
        ret = std::string(dst);

    SDL_free(dst);

    return ret;
}

MDAudioFile::MDAudioFile() {}

MDAudioFile::~MDAudioFile()
{}

std::string MDAudioFile::getLastError()
{
    return m_lastError;
}

const MDAudioFileSpec &MDAudioFile::getSpec() const
{
    return m_spec;
}

