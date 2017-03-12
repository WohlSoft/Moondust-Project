/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef FONT_MANAGER_PRIVATE_H
#define FONT_MANAGER_PRIVATE_H

#include <PGE_File_Formats/ConvertUTF.h>
#include <stdint.h>
#include <string>

inline uint32_t char2int(const char32_t &ch)
{
    return static_cast<uint32_t>(ch - U'0');
}

/* //Currently unused, uncommend when it needed
static int char2int(const char& ch)
{
    return ch - '0';
}
*/

/*
 * Index into the table below with the first byte of a UTF-8 sequence to
 * get the number of trailing bytes that are supposed to follow it.
 * Note that *legal* UTF-8 values can't have 4 or 5-bytes. The table is
 * left as-is for anyone who may want to do such conversion, which was
 * allowed in earlier algorithms.
 */
extern const char trailingBytesForUTF8[256];

/*
 * Magic values subtracted from a buffer value during UTF8 conversion.
 * This table contains as many values as there might be trailing bytes
 * in a UTF-8 sequence.
 */
extern const UTF32 offsetsFromUTF8[6];

/**
 * @brief Get single UTF32 character from firs character of UTF8 string
 * @param str pointer to position on UTF8 string where need to find a character
 * @return UTF32 character
 */
extern char32_t get_utf8_char(const char *str);

/**
 * @brief Converts UTF8 string into UTF32 string
 * @param src Source string in UTF8
 * @return UTF32 string
 */
extern std::u32string std_to_utf32(const std::string &src);

#endif // FONT_MANAGER_PRIVATE_H
