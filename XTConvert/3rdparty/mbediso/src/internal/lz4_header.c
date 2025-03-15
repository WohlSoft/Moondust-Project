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

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "internal/lz4_header.h"

static uint32_t s_swap_endian(uint32_t r)
{
    return ((uint8_t)(r >> 24) << 0) + ((uint8_t)(r >> 16) << 8) + ((uint8_t)(r >> 8) << 16) + ((uint8_t)(r >> 0) << 24);
}

static void s_fix_endian(uint32_t* buffer, uint32_t count, bool big_endian)
{
    if(big_endian == (bool)MBEDISO_BIG_ENDIAN)
        return;

    for(size_t i = 0; i < count; i++)
        buffer[i] = s_swap_endian(buffer[i]);
}

struct mbediso_lz4_header* mbediso_lz4_header_load(FILE* file)
{
    if(!file)
        return NULL;

    uint8_t read_buffer[4];

    // check magic numbers
    // LZ4 magic number
    if(fseek(file, 0x00, SEEK_SET)
        || fread(read_buffer, 1, 4, file) != 4
        || read_buffer[0] != 0x04
        || read_buffer[1] != 0x22
        || read_buffer[2] != 0x4d
        || read_buffer[3] != 0x18)
    {
        return NULL;
    }

    // size of mbediso frame
    uint32_t mbediso_inner_frame_length = 0;
    if(fseek(file, 0x0F, SEEK_SET)
        || fread(&mbediso_inner_frame_length, 4, 1, file) != 1)
    {
        return NULL;
    }

    s_fix_endian(&mbediso_inner_frame_length, 1, false);

    // mbediso magic number
    if(fread(read_buffer, 1, 4, file) != 4
        || read_buffer[0] != 'M'
        || read_buffer[1] != 'I'
        || (read_buffer[2] != 'L' && read_buffer[2] != 'B')
        || read_buffer[3] != 'E')
    {
        return NULL;
    }

    bool big_endian = (read_buffer[2] == 'B');

    // file size
    uint32_t file_size = 0;
    if(fread(&file_size, 4, 1, file) != 1)
        return NULL;

    s_fix_endian(&file_size, 1, big_endian);

    // block size
    uint32_t block_size = 0;
    if(fread(&block_size, 4, 1, file) != 1)
        return NULL;

    s_fix_endian(&block_size, 1, big_endian);

    if(block_size > 64 * 1024 || block_size < 2048 || (block_size % 2048) != 0)
        return NULL;

    uint32_t block_count = (file_size + (block_size - 1)) / block_size;

    // check that inner frame is the expected size
    if(mbediso_inner_frame_length != 12 + block_count * 4)
        return NULL;

    // load lookup table from file!
    uint32_t* block_offsets = (uint32_t*)malloc(block_count * 4);
    if(!block_offsets)
        return NULL;

    if(fread(block_offsets, 4, block_count, file) != block_count)
    {
        free(block_offsets);
        return NULL;
    }

    s_fix_endian(block_offsets, block_count, big_endian);

    // allocate, fill, and return the struct
    struct mbediso_lz4_header* header = (struct mbediso_lz4_header*)malloc(sizeof(struct mbediso_lz4_header));
    if(!header)
    {
        free(block_offsets);
        return NULL;
    }

    header->block_size = block_size;
    header->block_count = block_count;
    header->block_offsets = block_offsets;

    return header;
}

void mbediso_lz4_header_free(struct mbediso_lz4_header* header)
{
    if(!header)
        return;

    if(header->block_offsets)
        free(header->block_offsets);

    free(header);
}
