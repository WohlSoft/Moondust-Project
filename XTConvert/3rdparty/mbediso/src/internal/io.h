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

struct mbediso_lz4_header;

struct mbediso_io
{
    uint8_t tag;
};

struct mbediso_io* mbediso_io_from_file(FILE* file, struct mbediso_lz4_header* header);
void mbediso_io_close(struct mbediso_io* io);

const uint8_t* mbediso_io_read_sector(struct mbediso_io* io, uint32_t sector);
size_t mbediso_io_read_direct(struct mbediso_io* io, uint8_t* dest, uint64_t offset, size_t bytes);
