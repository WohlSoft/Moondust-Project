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

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "mbediso.h"

#include "internal/util.h"
#include "internal/directory.h"
#include "internal/io.h"
#include "internal/read.h"

bool mbediso_directory_ctor(struct mbediso_directory* dir)
{
    if(!dir)
        return false;

    dir->stringtable = malloc(32);
    if(!dir->stringtable)
        return false;

    dir->stringtable_size = 0;
    dir->stringtable_capacity = 32;

    dir->entries = NULL;
    dir->entry_count = 0;
    dir->entry_capacity = 0;

    dir->utf8_sorted = true;

    return true;
}

void mbediso_directory_dtor(struct mbediso_directory* dir)
{
    if(!dir)
        return;

    free(dir->stringtable);
    free(dir->entries);
}

int mbediso_directory_push(struct mbediso_directory* dir, const struct mbediso_raw_entry* raw_entry)
{
    if(!dir || !raw_entry)
        return -1;

    // make sure entry's name is not overlong (FIXME: make sure filenames are never overlong in source and pass length to this function)
    size_t fn_len = strlen((const char*)raw_entry->name.buffer);
    if(fn_len == 0 || fn_len > 333)
        return -1;

    // make sure no entry is '.' or '..'
    if(fn_len <= 2 && raw_entry->name.buffer[0] == '.' && (fn_len == 1 || raw_entry->name.buffer[1] == '.'))
        return -1;

    // make sure there is capacity for entry
    if(dir->entry_count + 1 > dir->entry_capacity)
    {
        size_t new_capacity = mbediso_util_first_pow2(dir->entry_capacity + 1);
        struct mbediso_dir_entry* new_entries = realloc(dir->entries, new_capacity * sizeof(struct mbediso_dir_entry));
        if(new_entries)
        {
            dir->entries = new_entries;
            dir->entry_capacity = new_capacity;
        }
    }

    if(dir->entry_count + 1 > dir->entry_capacity)
        return -1;

    // make sure there is capacity for name
    if(dir->stringtable_size + fn_len > dir->stringtable_capacity)
    {
        size_t new_capacity = mbediso_util_first_pow2(dir->stringtable_capacity + fn_len);
        uint8_t* new_stringtable = realloc(dir->stringtable, new_capacity);
        if(new_stringtable)
        {
            dir->stringtable = new_stringtable;
            dir->stringtable_capacity = new_capacity;
        }
    }

    if(dir->stringtable_size + fn_len > dir->stringtable_capacity)
        return -1;

    // function must succeed at this point, begin allocating and copying data

    struct mbediso_dir_entry* entry = &dir->entries[dir->entry_count];
    dir->entry_count++;

    // copy information from raw_entry
    entry->l = raw_entry->l;

    // copy name information
    entry->name_frag.last_effective_entry = -1;
    entry->name_frag.subst_table_offset = dir->stringtable_size;
    entry->name_frag.subst_begin = 0;
    entry->name_frag.subst_end = fn_len;
    entry->name_frag.clip_end = true;

    // copy name buffer
    memcpy(dir->stringtable + dir->stringtable_size, raw_entry->name.buffer, fn_len);
    dir->stringtable_size += fn_len;

    return 0;
}

static const struct mbediso_directory* s_sort_dir = NULL;

static int s_directory_entry_cmp_PRECOMPACT(const void* m1, const void* m2)
{
    const struct mbediso_dir_entry* e1 = (const struct mbediso_dir_entry*)m1;
    const struct mbediso_dir_entry* e2 = (const struct mbediso_dir_entry*)m2;

    int len_cmp = 0;
    if(e1->name_frag.subst_end < e2->name_frag.subst_end)
        len_cmp = -1;
    else if(e1->name_frag.subst_end > e2->name_frag.subst_end)
        len_cmp = 1;

    size_t min_len = (len_cmp <= 0) ? e1->name_frag.subst_end : e2->name_frag.subst_end;

    int ret = strncmp((const char*)&s_sort_dir->stringtable[e1->name_frag.subst_table_offset], (const char*)&s_sort_dir->stringtable[e2->name_frag.subst_table_offset], min_len);

    if(ret == 0)
        ret = len_cmp;

    // printf("Compared [%s] [%s], %d\n", (const char*)&s_sort_dir->stringtable[e1->name_frag.subst_table_offset], (const char*)&s_sort_dir->stringtable[e1->name_frag.subst_table_offset], ret);

    return ret;
}

static void s_directory_sort_PRECOMPACT(struct mbediso_directory* dir)
{
    if(dir->entry_count > 0)
    {
        // WARNING: need to deadlock file open with multiple threads in cache mode
        s_sort_dir = dir;
        qsort(dir->entries, dir->entry_count, sizeof(struct mbediso_dir_entry), s_directory_entry_cmp_PRECOMPACT);
        s_sort_dir = NULL;
    }

    dir->utf8_sorted = true;
}

#if 0
static bool mbediso_directory_safe_lookup(const struct mbediso_directory* dir, const char* name, uint32_t name_length, struct mbediso_location* out)
{
    struct mbediso_name temp_filename;

    if(name_length > sizeof(temp_filename.buffer))
        return false;

    // perform binary search on directory's entries
    uint32_t begin = 0;
    uint32_t end = dir->entry_count;

    while(begin != end)
    {
        uint32_t mid = begin + (end - begin) / 2;

        if(mbediso_string_diff_reconstruct(temp_filename.buffer, sizeof(temp_filename.buffer), dir->stringtable, dir->entries, dir->entry_count, sizeof(struct mbediso_dir_entry), mid))
            return false;

        int cmp_ret = strncmp((const char*)temp_filename.buffer, (const char*)name, name_length);

        if(cmp_ret == 0 && (name_length == sizeof(temp_filename.buffer) || temp_filename.buffer[name_length] == '\0'))
        {
            *out = dir->entries[mid].l;
            return true;
        }
        else if(cmp_ret < 0)
            begin = mid + 1;
        else
            end = mid;
    }

    return false;
}
#endif

bool mbediso_directory_lookup(const struct mbediso_directory* dir, const char* _name, uint32_t name_length, struct mbediso_location** out)
{
    const uint8_t* name = (const uint8_t*)_name;

    // perform binary search on directory's entries
    uint32_t begin_ge_end = 0;
    uint32_t begin = 0;
    uint32_t end = dir->entry_count;
    uint32_t end_le_end = 0;

    while(begin < end)
    {
        uint32_t mid = begin + (end - begin) / 2;

        uint32_t cmp_char = begin_ge_end;
        if(end_le_end < cmp_char)
            cmp_char = end_le_end;

        int cmp = 0;
        uint32_t effective_entry = mid;

        while(cmp_char <= name_length)
        {
            uint32_t valid_to = name_length + 1;

            const struct mbediso_string_diff* entry;
            effective_entry = mid;

            // find the entry that sets cmp_char
            do
            {
                entry = (const struct mbediso_string_diff*)(&dir->entries[effective_entry]);

                if(entry->subst_end > cmp_char || entry->clip_end)
                {
                    if(entry->subst_begin <= cmp_char)
                    {
                        if(entry->subst_end < valid_to)
                            valid_to = entry->subst_end;
                        break;
                    }

                    if(entry->subst_begin < valid_to)
                        valid_to = entry->subst_begin;
                }

                // follow effective entry pointers
                effective_entry = entry->last_effective_entry;
            } while(effective_entry < end && valid_to > cmp_char);

            // the string ends before cmp_char
            if(valid_to <= cmp_char)
            {
                // mid is less than if it ends before the requested string
                if(cmp_char < name_length)
                    cmp = -1;
                break;
            }

            bool extends_past = false;
            if(valid_to > name_length)
            {
                extends_past = true;
                valid_to = name_length;
            }

            // do the comparison
            const uint8_t* cmp_string = dir->stringtable - entry->subst_begin + entry->subst_table_offset;
            while(cmp_char < valid_to)
            {
                if(cmp_string[cmp_char] > name[cmp_char])
                {
                    cmp = 1;
                    break;
                }
                else if(cmp_string[cmp_char] < name[cmp_char])
                {
                    cmp = -1;
                    break;
                }

                cmp_char++;
            }

            if(cmp == 0 && extends_past)
                cmp = 1;

            // there was an unequal compare
            if(cmp != 0)
                break;
        }

        if(cmp == 0)
        {
            *out = &dir->entries[mid].l;
            return true;
        }
        else if(cmp < 0)
        {
            begin = mid + 1;
            begin_ge_end = cmp_char;
        }
        else
        {
            end = mid;
            end_le_end = cmp_char;
        }
    }

    return false;
}

static int s_mbediso_directory_finish(struct mbediso_directory* dir)
{
    if(!dir->utf8_sorted)
        s_directory_sort_PRECOMPACT(dir);

    int ret = mbediso_string_diff_compact(&dir->stringtable, &dir->stringtable_size, dir->entries, dir->entry_count, sizeof(struct mbediso_dir_entry));
    if(ret)
        return ret;

    // shrink stringtable to fit
    if(dir->stringtable_size > 0)
    {
        uint8_t* shrunken_stringtable = realloc(dir->stringtable, dir->stringtable_size);
        if(shrunken_stringtable)
        {
            dir->stringtable = shrunken_stringtable;
            dir->stringtable_capacity = dir->stringtable_size;
        }
    }
    else
    {
        free(dir->stringtable);
        dir->stringtable = NULL;
        dir->stringtable_capacity = 0;
    }

    // shrink entry list to fit
    if(dir->entry_count > 0)
    {
        struct mbediso_dir_entry* new_entries = realloc(dir->entries, dir->entry_count * sizeof(struct mbediso_dir_entry));
        if(new_entries)
        {
            dir->entries = new_entries;
            dir->entry_capacity = dir->entry_count;
        }
    }
    else
    {
        free(dir->entries);
        dir->entries = NULL;
        dir->entry_capacity = 0;
    }

    return 0;
}

int mbediso_directory_load(struct mbediso_directory* dir, struct mbediso_io* io, uint32_t sector, uint32_t length)
{
    struct mbediso_raw_entry entry[2];

    uint32_t entry_index = 0;
    uint32_t offset = 0;

    const uint8_t* buffer = NULL;
    bool buffer_dirty = true;

    while(offset < length)
    {
        if(buffer_dirty)
            buffer = mbediso_io_read_sector(io, sector + (offset / 2048));

        if(!buffer)
            return -1;

        struct mbediso_raw_entry* const cur_entry = &entry[entry_index & 1];

        int ret = mbediso_read_dir_entry(cur_entry, buffer + (offset % 2048), 2048 - (offset % 2048));

        if(ret < 33)
        {
            // any cleanup needed?
            return -1;
        }

        buffer_dirty = ((offset % 2048) + ret >= 2048);
        offset += ret;

        if(!buffer_dirty && buffer[offset % 2048] == '\0')
        {
            buffer_dirty = true;
            offset += 2048 - (offset % 2048);
        }

        // skip on partial failure
        if(cur_entry->name.buffer[0] == '\0')
            continue;

        if(entry_index >= 2)
        {
            if(mbediso_directory_push(dir, cur_entry))
            {
                // think about how to cleanly handle full failure... if no resources are owned by stack (ideal), can simply cleanup after failure
                // mbediso_fs_free_directory(fs, dir);
                return -1;
            }
        }

        // printf("%d offset %x, length %x, name %s\n", (int)entry[entry_index & 1].directory, entry[entry_index & 1].sector * 2048, entry[entry_index & 1].length, entry[entry_index & 1].name.buffer);

        if(entry_index > 2)
        {
            const uint8_t* prev_fn = entry[!(entry_index & 1)].name.buffer;
            const uint8_t* this_fn = cur_entry->name.buffer;

            // check sort order
            int sort_order = strncmp((const char*)prev_fn, (const char*)this_fn, sizeof(struct mbediso_name));
            if(sort_order >= 0)
            {
                // printf("Unsorted at [%s] [%s]\n", prev_fn, this_fn);
                dir->utf8_sorted = false;
            }
        }

        entry_index++;
    }

    if(entry_index < 2)
        return -1;

    // finalize the directory
    // old_size += dir->stringtable_size;
    s_mbediso_directory_finish(dir);
    // total_size += dir->stringtable_size;

    return 0;
}

bool mbediso_directory_lookup_unloaded(struct mbediso_io* io, uint32_t sector, uint32_t length, const char* name, uint32_t name_length, struct mbediso_location* out)
{
    struct mbediso_raw_entry entry;

    uint32_t entry_index = 0;
    uint32_t offset = 0;

    const uint8_t* buffer = NULL;
    bool buffer_dirty = true;

    while(offset < length)
    {
        if(buffer_dirty)
            buffer = mbediso_io_read_sector(io, sector + (offset / 2048));

        if(!buffer)
            return false;

        int ret = mbediso_read_dir_entry(&entry, buffer + (offset % 2048), 2048 - (offset % 2048));

        if(ret < 33)
        {
            // any cleanup needed?
            return false;
        }

        buffer_dirty = ((offset % 2048) + ret >= 2048);
        offset += ret;

        if(!buffer_dirty && buffer[offset % 2048] == '\0')
        {
            buffer_dirty = true;
            offset += 2048 - (offset % 2048);
        }

        // skip on partial failure
        if(entry.name.buffer[0] == '\0')
            continue;

        entry_index++;

        if(entry_index < 2)
            continue;

        int cmp_ret = strncmp((const char*)entry.name.buffer, (const char*)name, name_length);

        if(cmp_ret == 0 && (name_length == sizeof(entry.name.buffer) || entry.name.buffer[name_length] == '\0'))
        {
            *out = entry.l;
            return true;
        }
    }

    return false;
}
