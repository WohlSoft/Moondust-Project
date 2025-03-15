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

#include <stdint.h>

#define MBEDISO_SEEK_SET 0       /**< Seek from the beginning of data */
#define MBEDISO_SEEK_CUR 1       /**< Seek relative to current read point */
#define MBEDISO_SEEK_END 2       /**< Seek relative to the end of data */

struct mbediso_io;
struct mbediso_fs;

struct mbediso_file
{
    struct mbediso_io* io;
    struct mbediso_fs* fs;
    uint32_t start;
    uint32_t end;
    uint32_t offset;
};

struct mbediso_file* mbediso_fopen(struct mbediso_fs* fs, const char* pathname);

size_t mbediso_fread(struct mbediso_file* file, void* ptr, size_t size, size_t maxnum);

int64_t mbediso_fseek(struct mbediso_file* file, int64_t offset, int whence);

int64_t mbediso_fsize(struct mbediso_file* file);

void mbediso_fclose(struct mbediso_file* file);

struct mbediso_fs* mbediso_file_fs(struct mbediso_file* file);
