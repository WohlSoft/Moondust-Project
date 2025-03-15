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

#define MBEDISO_DT_UKNOWN 1
#define MBEDISO_DT_REG 2
#define MBEDISO_DT_DIR 3

struct mbediso_fs;
struct mbediso_directory;

struct mbediso_dirent
{
    uint8_t d_name[334];
    int d_type;
};

struct mbediso_dir
{
    struct mbediso_fs* fs;
    struct mbediso_directory* directory;
    struct mbediso_dirent dirent;
    uint32_t entry_index;
    bool on_heap;
};

struct mbediso_dir* mbediso_opendir(struct mbediso_fs* fs, const char* name);

int mbediso_closedir(struct mbediso_dir* dir);

const struct mbediso_dirent* mbediso_readdir(struct mbediso_dir* dir);

int mbediso_exists(struct mbediso_fs* fs, const char* name);
