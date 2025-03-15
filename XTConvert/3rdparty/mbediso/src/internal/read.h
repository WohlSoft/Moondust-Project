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

struct mbediso_fs;
struct mbediso_io;
struct mbediso_raw_entry;

/**
 * \brief function to read a single directory entry
 *
 * \param entry Raw entry object to output filename, location, and directory flags to. In partial failure (unreadable entry), sets filename to an empty string.
 * \param buffer Input buffer (generally a portion of a pre-loaded sector)
 * \param buffer_size Number of bytes that may be legally read from buffer (bytes remaining in sector)
 *
 * \returns On success or partial success, number of bytes of buffer consumed (at least 33). On total failure (the parent directory should be abandoned), returns -1.
 **/
int mbediso_read_dir_entry(struct mbediso_raw_entry* entry, const uint8_t* buffer, int buffer_size);

int mbediso_read_find_joliet_root(struct mbediso_fs* fs, struct mbediso_io* io);
