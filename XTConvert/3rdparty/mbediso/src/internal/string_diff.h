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


/* string for a string diff */
struct mbediso_string_diff
{
    // info for the filename
    unsigned last_effective_entry : 19;
    bool     clip_end : 1;
    unsigned subst_table_offset : 24;
    unsigned subst_begin : 10;
    unsigned subst_end : 10;
};

int mbediso_string_diff_reconstruct(uint8_t* buffer, size_t buffer_size, const uint8_t* stringtable, const void* entries, size_t entry_count, size_t entry_size, size_t top_entry);
int mbediso_string_diff_compact(uint8_t** stringtable, uint32_t* stringtable_size, void* entries, size_t entry_count, size_t entry_size);
