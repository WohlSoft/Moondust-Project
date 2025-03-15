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

#include "internal/string_diff.h"

int mbediso_string_diff_reconstruct(uint8_t* buffer, size_t buffer_size, const uint8_t* stringtable, const void* entries, size_t entry_count, size_t entry_size, size_t top_entry)
{
    size_t clip_at = -1;
    size_t last_effective_entry = top_entry;
    size_t string_size = 0;

    while(last_effective_entry < entry_count)
    {
        const struct mbediso_string_diff* entry = (const struct mbediso_string_diff*)((const uint8_t*)entries + (entry_size * last_effective_entry));

        size_t end_effective = clip_at;
        if(end_effective > entry->subst_end)
            end_effective = entry->subst_end;

        // fill with zero bytes if needed
        if(end_effective + 1 > string_size)
        {
            if(end_effective + 1 > buffer_size)
                return -1;

            memset(buffer + string_size, 0, end_effective + 1 - string_size);
            string_size = end_effective + 1;
        }

        // replace any uninitialized characters
        for(size_t i = entry->subst_begin; i < end_effective; ++i)
        {
            if(buffer[i] == 0)
                buffer[i] = stringtable[i - entry->subst_begin + entry->subst_table_offset];
        }

        // update the clipping location
        if(entry->clip_end)
            clip_at = end_effective;

        // follow effective entry pointers
        last_effective_entry = entry->last_effective_entry;
    }

    return 0;
}

int mbediso_string_diff_compact(uint8_t** stringtable, uint32_t* stringtable_size, void* entries, size_t entry_count, size_t entry_size)
{
    if(entry_count == 0)
        return 0;

    // for now this is okay, it'll become very dangerous soon, remember that stringtable is not necessarily in its original order
    uint8_t* new_stringtable = malloc(*stringtable_size);
    uint32_t new_stringtable_size = 0;

    if(!new_stringtable)
        return -1;

    struct mbediso_string_diff* last_entry = (struct mbediso_string_diff*)(entries);
    if(!last_entry->clip_end || last_entry->subst_begin != 0)
    {
        free(new_stringtable);
        return -1;
    }

    size_t last_diff_begin = 0;
    size_t last_diff_end = last_entry->subst_end;
    bool last_clip_end = true;

    for(size_t e = 1; e < entry_count; ++e)
    {
        struct mbediso_string_diff* entry = (struct mbediso_string_diff*)((uint8_t*)entries + (entry_size * e));
        if(!entry->clip_end || entry->subst_begin != 0)
        {
            free(new_stringtable);
            return -1;
        }

        // find range of differences
        size_t diff_begin = 0;
        size_t diff_end = 0;
        bool clip_end = false;

        for(size_t i = 0; i < entry->subst_end && i < last_entry->subst_end; i++)
        {
            if((*stringtable)[entry->subst_table_offset + i] == (*stringtable)[last_entry->subst_table_offset + i])
                continue;

            if(diff_end == 0)
                diff_begin = i;

            diff_end = i + 1;
        }

        // count the ending as a difference
        if(entry->subst_end < last_entry->subst_end)
        {
            if(diff_end == 0)
                diff_begin = entry->subst_end;

            diff_end = entry->subst_end;
            clip_end = true;
        }
        else if(entry->subst_end > last_entry->subst_end)
        {
            if(diff_end == 0)
                diff_begin = last_entry->subst_end;

            diff_end = entry->subst_end;
            clip_end = true;
        }
        else
        {
            // caught an identical entry!!
            if(diff_end == 0)
            {
                free(new_stringtable);
                return -1;
            }
            // ends at same time as entry
            else if(diff_end == entry->subst_end)
                clip_end = true;
        }

        // now that last entry has been checked, apply the clipping to it
        const uint8_t* src = *stringtable + last_entry->subst_table_offset;

        last_entry->subst_table_offset = new_stringtable_size;
        last_entry->subst_begin = last_diff_begin;
        last_entry->subst_end = last_diff_end;
        last_entry->clip_end = last_clip_end;

        for(size_t i = last_entry->subst_begin; i < last_entry->subst_end; i++)
            new_stringtable[new_stringtable_size++] = src[i];

        // save the just-detected values in the previous entry now
        last_entry = entry;
        last_diff_begin = diff_begin;
        last_diff_end = diff_end;
        last_clip_end = clip_end;

        // now update the last effective entry for THIS entry (safe because it won't be used during the comparison)
        if(diff_begin == 0 && clip_end)
            entry->last_effective_entry = -1;
        else
            entry->last_effective_entry = e - 1;

        size_t clip_at = entry->subst_end;

        while(entry->last_effective_entry < entry_count)
        {
            struct mbediso_string_diff* o_entry = (struct mbediso_string_diff*)((uint8_t*)entries + (entry_size * entry->last_effective_entry));

            size_t o_entry_eff_end = o_entry->subst_end;
            if(o_entry_eff_end > clip_at)
                o_entry_eff_end = clip_at;

            if(o_entry->subst_begin < diff_begin || o_entry_eff_end > diff_end)
            {
                // choose the entry
                break;
            }

            if(o_entry->clip_end)
                clip_at = o_entry_eff_end;

            entry->last_effective_entry = o_entry->last_effective_entry;
        }
    }

    // apply the clipping to the very last entry
    const uint8_t* src = *stringtable + last_entry->subst_table_offset;

    last_entry->subst_table_offset = new_stringtable_size;
    last_entry->subst_begin = last_diff_begin;
    last_entry->subst_end = last_diff_end;
    last_entry->clip_end = last_clip_end;

    for(size_t i = last_entry->subst_begin; i < last_entry->subst_end; i++)
        new_stringtable[new_stringtable_size++] = src[i];

    free(*stringtable);
    *stringtable = new_stringtable;
    *stringtable_size = new_stringtable_size;

    return 0;
}
