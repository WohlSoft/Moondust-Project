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

#include "internal/directory.h"

struct mbediso_lz4_header;

struct mbediso_fs
{
    /* Will soon be more flexible: path to give fopen when creating a new io. If non-null, owned by the mbediso_fs object. */
    char* archive_path;
    struct mbediso_lz4_header* lz4_header;

    /* total memory usage and budget of the filesystem */
    uint32_t mem_usage;
    uint32_t mem_capacity;

    /* nulled if directories own their own stringtables */
    uint8_t* stringtable;
    uint32_t stringtable_size;
    uint32_t stringtable_capacity;

    /* stores either all directories, or the currently loaded directories (owned by the pathcache) */
    struct mbediso_directory* directories;
    uint32_t directory_count;
    uint32_t directory_capacity;

    /* tracks the allocated and used IO instances */
    struct mbediso_io** io_pool;
    uint32_t io_pool_used;
    uint32_t io_pool_size;
    uint32_t io_pool_capacity;

    struct mbediso_location root_dir_entry;
    bool fully_scanned;

    /* fixme: add a directory free list */

    /* stores references to directories by path; may own these references */
    // struct mbediso_pathcache_entry** pathcache_list;
    // uint32_t pathcache_list_size;
    // uint32_t pathcache_list_capacity;
};


bool mbediso_fs_ctor(struct mbediso_fs* fs);
void mbediso_fs_dtor(struct mbediso_fs* fs);

bool mbediso_fs_init_from_path(struct mbediso_fs* fs, const char* path);

uint32_t mbediso_fs_alloc_directory(struct mbediso_fs* fs);
void mbediso_fs_free_directory(struct mbediso_fs* fs, uint32_t dir_index);

bool mbediso_fs_lookup(struct mbediso_fs* fs, const char* path, struct mbediso_location* out);

struct mbediso_io* mbediso_fs_reserve_io(struct mbediso_fs* fs);
void mbediso_fs_release_io(struct mbediso_fs* fs, struct mbediso_io* io);

int mbediso_fs_full_scan(struct mbediso_fs* fs, struct mbediso_io* io);
