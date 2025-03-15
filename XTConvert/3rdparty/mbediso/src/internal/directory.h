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
#include <stdbool.h>

#include "internal/string_diff.h"

struct mbediso_io;

/* struct for a single filename */
struct mbediso_name
{
    uint8_t buffer[334];
};

/* struct for the location portion of a directory entry */
/* if directory is set and length is zero, then the directory is loaded into the directory array and sector is an index into that array */
struct mbediso_location
{
    uint32_t sector;
    uint32_t length;
    bool directory;
};

/* struct for a raw directory entry */
struct mbediso_raw_entry
{
    struct mbediso_name name;
    struct mbediso_location l;
};

/* struct for a directory entry suitable for long-term storage */
struct mbediso_dir_entry
{
    struct mbediso_string_diff name_frag;
    struct mbediso_location l;
};

/* represents the contents of a single directory */
struct mbediso_directory
{
    /* if null, the directory strings have been permanently consolidated to the global stringtable */
    uint8_t* stringtable;
    uint32_t stringtable_size;
    uint32_t stringtable_capacity;

    struct mbediso_dir_entry* entries;
    uint32_t entry_count;
    uint32_t entry_capacity;

    /* tracks whether the directory is utf8-sorted */
    bool utf8_sorted;
};

bool mbediso_directory_ctor(struct mbediso_directory* dir);
void mbediso_directory_dtor(struct mbediso_directory* dir);

int mbediso_directory_push(struct mbediso_directory* dir, const struct mbediso_raw_entry* entry);
bool mbediso_directory_lookup(const struct mbediso_directory* dir, const char* name, uint32_t name_length, struct mbediso_location** out);

/* load a directory's entries from the filesystem and prepare the directory for use */
int mbediso_directory_load(struct mbediso_directory* dir, struct mbediso_io* io, uint32_t sector, uint32_t length);

/* fill the provided directory entry with the found directory item, for a directory which may not be loaded */
bool mbediso_directory_lookup_unloaded(struct mbediso_io* io, uint32_t sector, uint32_t length, const char* name, uint32_t name_length, struct mbediso_location* out);
