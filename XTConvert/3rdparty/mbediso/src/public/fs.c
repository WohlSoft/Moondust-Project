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
#include <string.h>

#include "mbediso/fs.h"
#include "internal/io.h"
#include "internal/fs.h"
#include "internal/read.h"

struct mbediso_fs* mbediso_openfs_file(const char* name, bool full_scan)
{
    struct mbediso_fs* fs = malloc(sizeof(struct mbediso_fs));
    if(!fs)
        return NULL;

    if(!mbediso_fs_ctor(fs))
    {
        free(fs);
        return NULL;
    }

    if(!mbediso_fs_init_from_path(fs, name))
    {
        mbediso_fs_dtor(fs);
        free(fs);
        return NULL;
    }

    struct mbediso_io* io = mbediso_fs_reserve_io(fs);
    if(!io)
    {
        mbediso_fs_dtor(fs);
        free(fs);
        return NULL;
    }

    if(mbediso_read_find_joliet_root(fs, io) != 0 || (full_scan && mbediso_fs_full_scan(fs, io) != 0))
    {
        mbediso_fs_release_io(fs, io);
        mbediso_fs_dtor(fs);
        free(fs);
        return NULL;
    }

    mbediso_fs_release_io(fs, io);

    return fs;
}

int mbediso_scanfs(struct mbediso_fs* fs)
{
    if(!fs)
        return -1;

    // already loaded? no problem!
    if(fs->fully_scanned)
        return 0;

    struct mbediso_io* io = mbediso_fs_reserve_io(fs);
    if(!io)
        return -1;

    int ret = mbediso_fs_full_scan(fs, io);

    mbediso_fs_release_io(fs, io);

    return ret;
}

void mbediso_closefs(struct mbediso_fs* fs)
{
    if(!fs)
        return;

    mbediso_fs_dtor(fs);
    free(fs);
}
