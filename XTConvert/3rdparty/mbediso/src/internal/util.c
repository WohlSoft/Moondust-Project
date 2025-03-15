/*
 * mbediso - a minimal library to load data from compressed ISO archives
 *
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

#include "internal/util.h"

size_t mbediso_util_first_pow2(size_t capacity)
{
    for(size_t power = 0; power <= 24; ++power)
    {
        if(((size_t)1 << power) >= capacity)
            return ((size_t)1 << power);
    }

    return capacity;
}

int mbediso_util_utf16be_to_utf8(uint8_t* restrict dest, ptrdiff_t capacity, const uint8_t* restrict src, size_t bytes)
{
    if(bytes & 1)
        return -1;

    size_t offset = 0;

    while(offset + 1 < bytes)
    {
        uint32_t codepoint = 0;

        // fast path for ASCII
        if(!src[offset] && src[offset + 1] < 0x80)
        {
            capacity -= 1;
            if(capacity <= 0)
                return -1;

            *(dest++) = src[offset + 1];
            offset += 2;
            continue;
        }

        uint32_t ucs2 = (uint32_t)src[offset] * 256 + (uint32_t)src[offset + 1];
        offset += 2;

        // a low surrogate without a preceding high surrogate is invalid
        if(ucs2 >= 0xDC00 && ucs2 < 0xE000)
            return -1;
        // if high surrogate, also read low surrogate
        else if(ucs2 >= 0xD800 && ucs2 < 0xDC00)
        {
            if(offset + 2 > bytes)
                return -1;

            // store high surrogate
            codepoint = (ucs2 - 0xD800) * 0x0400;

            // read low surrogate
            uint32_t low_surrogate = (uint32_t)src[offset] * 256 + (uint32_t)src[offset + 1];
            offset += 2;

            // add low surrogate if valid
            if(low_surrogate >= 0xDC00 && low_surrogate < 0xE000)
                codepoint += low_surrogate - 0xDC00;
            else
                return -1;
        }
        // otherwise, we have a BMP codepoint
        else
            codepoint = ucs2;

        // output codepoint as UTF8
        if(codepoint < 0x80)
        {
            capacity -= 1;
            if(capacity <= 0)
                return -1;

            *(dest++) = (uint8_t)codepoint;
        }
        else if(codepoint < 0x800)
        {
            capacity -= 2;
            if(capacity <= 0)
                return -1;

            *(dest++) = (uint8_t)(0xC0 | (codepoint / 0x40));
            *(dest++) = (uint8_t)(0x80 | (codepoint & 0x3F));
        }
        else if(codepoint < 0x10000)
        {
            capacity -= 3;
            if(capacity <= 0)
                return -1;

            *(dest++) = (uint8_t)(0xE0 | (codepoint / 0x1000));
            *(dest++) = (uint8_t)(0x80 | ((codepoint / 0x40) & 0x3F));
            *(dest++) = (uint8_t)(0x80 | (codepoint & 0x3F));
        }
        else if(codepoint < 0x110000)
        {
            capacity -= 4;
            if(capacity <= 0)
                return -1;

            *(dest++) = (uint8_t)(0xF0 | (codepoint / 0x40000));
            *(dest++) = (uint8_t)(0x80 | ((codepoint / 0x1000) & 0x3F));
            *(dest++) = (uint8_t)(0x80 | ((codepoint / 0x40) & 0x3F));
            *(dest++) = (uint8_t)(0x80 | (codepoint & 0x3F));
        }
        else
            return -1;
    }

    // add null terminator
    *dest = 0;

    return 0;
}
