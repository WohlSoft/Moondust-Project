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

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "mbediso.h"

#include "internal/io.h"
#include "internal/fs.h"
#include "internal/util.h"

int mbediso_read_dir_entry(struct mbediso_raw_entry* entry, const uint8_t* buffer, int buffer_size)
{
    if(buffer_size < 33 || buffer[0] < 33 || buffer[0] > buffer_size)
        return -1;

    // check that name is stored successfully and safely
    // note: be permissive about name_length, in practice allowing up to 111 UTF16 characters
    uint8_t name_length_bytes = buffer[32];

    if(buffer[0] < 33 + name_length_bytes || name_length_bytes > 222)
        return -1;

    // test for all unsupported features
    uint8_t flags = buffer[25];
    uint8_t extended_attr = buffer[1];
    uint8_t unit_size = buffer[26];
    uint8_t interleave_gap = buffer[27];
    uint8_t volume_low = buffer[28];
    uint8_t volume_high = buffer[29];

    if((flags & 0xFC) || extended_attr || unit_size || interleave_gap || volume_low != 1 || volume_high)
    {
        entry->name.buffer[0] = '\0';
        return buffer[0];
    }


    // convert name
    if(name_length_bytes == 1)
    {
        entry->name.buffer[0] = '.';
        entry->name.buffer[1] = (buffer[33]) ? '.' : '\0';
        entry->name.buffer[2] = '\0';
    }
    else if(mbediso_util_utf16be_to_utf8(entry->name.buffer, (ptrdiff_t)sizeof(struct mbediso_name), buffer + 33, name_length_bytes))
    {
        entry->name.buffer[0] = '\0';
        return buffer[0];
    }


    // set directory flag
    entry->l.directory = flags & 0x02;

    // use little-endian copies of sector and length
    entry->l.sector = buffer[ 2] * 0x00000001 + buffer[ 3] * 0x00000100 + buffer[ 4] * 0x00010000 + buffer[ 5] * 0x01000000;
    entry->l.length = buffer[10] * 0x00000001 + buffer[11] * 0x00000100 + buffer[12] * 0x00010000 + buffer[13] * 0x01000000;


    return buffer[0];
}

int mbediso_read_find_joliet_root(struct mbediso_fs* fs, struct mbediso_io* io)
{
    struct mbediso_raw_entry entry;

    uint32_t try_sector = 16;

    // if 255, more than 16 sectors tried, or out of I/O, fail
    const uint8_t* buffer = NULL;

    // find the Joliet sector
    while(true)
    {
        buffer = mbediso_io_read_sector(io, try_sector);

        if(!buffer || buffer[0] == 255)
            return -1;

        if(buffer[0] == 2 // supplementary volume descriptor / enhanced volume descriptor
            && buffer[1] == 'C' && buffer[2] == 'D' && buffer[3] == '0' && buffer[4] == '0' && buffer[5] == '1' // ISO magic numbers
            && buffer[88] == 0x25 && buffer[89] == 0x2F && buffer[90] == 0x45 // UCS-2 level 3 (UTF-16) escape sequence
            && buffer[881] == 1 // supplementary volume descriptor
            )
            break;

        if(try_sector == 31)
            return -1;

        try_sector++;
    }

    // app range, begin 883 end 1395
    // root directory, begin 157 end 190
    int bytes_read = mbediso_read_dir_entry(&entry, &buffer[156], 190 - 156);
    if(bytes_read < 33)
        return -2;

    if(!entry.l.directory)
        return -3;

    fs->root_dir_entry.sector = entry.l.sector;
    fs->root_dir_entry.length = entry.l.length;
    fs->root_dir_entry.directory = true;

    return 0;
}
