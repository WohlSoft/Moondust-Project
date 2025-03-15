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

#pragma once

#include <stdio.h>
#include <stdint.h>

#define MBEDISO_IO_TAG_UNC 1
#define MBEDISO_IO_TAG_LZ4 2

struct mbediso_io_unc
{
    uint8_t tag;

    FILE* file;

    uint64_t filepos;

    // must be 2048
    uint8_t* buffer;
};

struct mbediso_io_lz4
{
    uint8_t tag;

    FILE* file;
    struct mbediso_lz4_header* header;

    uint32_t file_pos;

    uint32_t file_buffer_pos;
    uint32_t file_buffer_length;
    uint32_t file_buffer_capacity;

    uint32_t buffer_logical_pos;
    uint32_t buffer_length;

    // must be larger than (block_size + 4), resized as needed
    uint8_t* file_buffer;
    // equals block_size, which must be larger than 2048
    uint8_t* decompression_buffer;

    // this one can be accessed by the rest of the program
    const uint8_t* public_buffer;
};

