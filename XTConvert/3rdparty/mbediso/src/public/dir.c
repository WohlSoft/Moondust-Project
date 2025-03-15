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

#include "mbediso/dir.h"
#include "internal/io.h"
#include "internal/fs.h"
#include "internal/directory.h"

struct mbediso_dir* mbediso_opendir(struct mbediso_fs* fs, const char* name)
{
    struct mbediso_location loc;
    if(!mbediso_fs_lookup(fs, name, &loc))
        return NULL;

    if(!loc.directory)
        return NULL;

    if(loc.length == 0 && loc.sector >= fs->directory_count)
        return NULL;

    struct mbediso_dir* dir = malloc(sizeof(struct mbediso_dir));
    if(!dir)
        return NULL;

    // preloaded directory
    if(loc.length == 0 && loc.sector < fs->directory_count)
    {
        dir->directory = &fs->directories[loc.sector];
        dir->on_heap = false;
    }
    // need to load
    else
    {
        dir->directory = malloc(sizeof(struct mbediso_directory));
        dir->on_heap = true;

        struct mbediso_io* io = NULL;

        if(!dir->directory
            || !mbediso_directory_ctor(dir->directory)
            || (io = mbediso_fs_reserve_io(fs)) == NULL
            || mbediso_directory_load(dir->directory, io, loc.sector, loc.length) != 0
        )
        {
            mbediso_fs_release_io(fs, io);
            mbediso_closedir(dir);
            return NULL;
        }

        // loaded successfully, just need to clean up here
        mbediso_fs_release_io(fs, io);
    }

    dir->fs = fs;
    dir->entry_index = 0;
    return dir;
}

int mbediso_closedir(struct mbediso_dir* dir)
{
    if(!dir)
        return 0;

    if(dir->on_heap)
    {
        mbediso_directory_dtor(dir->directory);
        free(dir->directory);
    }

    free(dir);
    return 0;
}

const struct mbediso_dirent* mbediso_readdir(struct mbediso_dir* dir)
{
    if(dir->entry_index >= dir->directory->entry_count)
        return NULL;

    const struct mbediso_dir_entry* entry = &dir->directory->entries[dir->entry_index];

    dir->dirent.d_type = entry->l.directory ? MBEDISO_DT_DIR : MBEDISO_DT_REG;

    // update the entry name
    const struct mbediso_string_diff* diff = &entry->name_frag;

    if(diff->subst_end >= sizeof(dir->dirent.d_name))
    {
        // this should be unreachable
        return NULL;
    }

    const char* diff_str = (const char*)(dir->directory->stringtable + diff->subst_table_offset);
    for(unsigned i = diff->subst_begin; i < diff->subst_end; ++i)
        dir->dirent.d_name[i] = *(diff_str++);

    if(diff->clip_end)
        dir->dirent.d_name[diff->subst_end] = '\0';

    dir->entry_index++;
    return &dir->dirent;
}

int mbediso_exists(struct mbediso_fs* fs, const char* name)
{
    struct mbediso_location loc;
    if(!mbediso_fs_lookup(fs, name, &loc))
        return 0;

    if(loc.directory)
        return MBEDISO_DT_DIR;
    else
        return MBEDISO_DT_REG;
}
