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

#include <cstdio>
#include <string>
#include <limits>
#include "lz4.h"
#include "lz4hc.h"
#define XXH_NAMESPACE LZ4_
#include "xxhash.h"

#include "lz4_pack.h"

static void write_uint32_le(uint8_t* dest, uint32_t value)
{
    dest[0] = (uint8_t)(value >>  0);
    dest[1] = (uint8_t)(value >>  8);
    dest[2] = (uint8_t)(value >> 16);
    dest[3] = (uint8_t)(value >> 24);
}

static void write_uint32_be(uint8_t* dest, uint32_t value)
{
    dest[0] = (uint8_t)(value >> 24);
    dest[1] = (uint8_t)(value >> 16);
    dest[2] = (uint8_t)(value >>  8);
    dest[3] = (uint8_t)(value >>  0);
}

static inline void write_uint32(uint8_t* dest, uint32_t value, bool big_endian)
{
    if(big_endian)
        write_uint32_be(dest, value);
    else
        write_uint32_le(dest, value);
}

bool LZ4Pack::compress(FILE* outf, FILE* inf, size_t block_size, bool big_endian)
{
    if(!outf || !inf)
        return false;

    if(block_size > 64*1024)
        return false;

    size_t out_block_max = LZ4_compressBound(block_size);

    fseek(inf, 0, SEEK_END);
    size_t inf_size = ftell(inf);
    fseek(inf, 0, SEEK_SET);

    if(inf_size > std::numeric_limits<uint32_t>::max())
        return false;

    // an lz4 frame endmark
    uint32_t endmark = 0;

    // the fake header
    const char* fake_header = "\x04\x22\x4d\x18" // magic number
        "\x60" // FLG, version 1 with independent blocks
        "\x40" // BD, max 64KB
        "\x82" // checksum, so DON'T CHANGE OTHER FIELDS
        ;

    XXH32_hash_t xxh = XXH32(fake_header + 4, 2, 0);
    if((uint8_t)(xxh >> 8) != (uint8_t)fake_header[6])
        return false;

    // the real header
    const char* real_header = "\x04\x22\x4d\x18" // magic number
        "\x64" // FLG, version 1 with independent blocks and content checksum at end of frame
        "\x40" // BD, max 64KB
        "\xa7" // checksum, so DON'T CHANGE OTHER FIELDS
        ;

    xxh = XXH32(real_header + 4, 2, 0);
    if((uint8_t)(xxh >> 8) != (uint8_t)real_header[6])
        return false;

    size_t block_count = (inf_size + (block_size - 1)) / block_size;
    uint8_t mbediso_frame_header[20];
    uint32_t mbediso_frame_length = sizeof(mbediso_frame_header) + block_count * 4;
    uint32_t mbediso_frame_inner_length = mbediso_frame_length - 8;

    // lz4 magic number for skippable frame
    mbediso_frame_header[0] = 0x50;
    mbediso_frame_header[1] = 0x2a;
    mbediso_frame_header[2] = 0x4d;
    mbediso_frame_header[3] = 0x18;
    // lz4 little-endian frame size
    write_uint32_le(&mbediso_frame_header[4], mbediso_frame_inner_length);
    // mbediso magic number
    mbediso_frame_header[8] = 'M';
    mbediso_frame_header[9] = 'I';
    mbediso_frame_header[10] = (big_endian) ? 'B' : 'L';
    mbediso_frame_header[11] = 'E';
    // mbediso file size
    write_uint32(&mbediso_frame_header[12], inf_size, big_endian);
    // mbediso block size
    write_uint32(&mbediso_frame_header[16], block_size, big_endian);

    uint8_t* mbediso_block_offsets = (uint8_t*)malloc(block_count * 4);
    if(!mbediso_block_offsets)
        return false;

    // initialize hash state
    auto hash_state = XXH32_createState();
    if(!hash_state)
    {
        free(mbediso_block_offsets);
        return false;
    }
    XXH32_reset(hash_state, 0);

    // WRITE ALL HEADERS TO FILE!!
    fseek(outf, 0, SEEK_SET);

    fwrite(fake_header, 1, 7, outf);
    fwrite(&endmark, 4, 1, outf);

    fwrite(mbediso_frame_header, 1, sizeof(mbediso_frame_header), outf);
    auto block_offset_table_cursor = ftell(outf);
    fseek(outf, block_count * 4, SEEK_CUR);

    fwrite(real_header, 1, 7, outf);

    // WRITE ALL BLOCKS TO FILE!
    bool success = false;
    char* in_block = (char*)malloc(block_size);
    char* out_block = (char*)malloc(out_block_max);
    if(in_block && out_block)
    {
        size_t block_index = 0;
        size_t bytes_left = inf_size;

        while(bytes_left > 0)
        {
            size_t to_compress = bytes_left;
            if(to_compress > block_size)
                to_compress = block_size;

            if(fread(in_block, 1, to_compress, inf) != to_compress)
                break;

            XXH32_update(hash_state, in_block, to_compress);

            auto _to_write = LZ4_compress_HC(in_block, out_block, to_compress, out_block_max, LZ4HC_CLEVEL_MAX);
            if(_to_write <= 0)
                break;

            size_t to_write = (size_t)_to_write;

            uint32_t block_dest = (uint32_t)ftell(outf);
            write_uint32(&mbediso_block_offsets[block_index * 4], block_dest, big_endian);

            uint8_t block_header[4];

            // fall back to uncompressed data if worse than 50% compression ratio
            if(to_write > to_compress * 5 / 10)
            {
                write_uint32_le(block_header, to_compress | 0x80000000);

                if(fwrite(block_header, 1, 4, outf) != 4 || fwrite(in_block, 1, to_compress, outf) != to_compress)
                    break;
            }
            else
            {
                write_uint32_le(block_header, to_write);

                if(fwrite(block_header, 1, 4, outf) != 4 || fwrite(out_block, 1, to_write, outf) != to_write)
                    break;
            }

            block_index++;
            bytes_left -= to_compress;
        }

        if(bytes_left == 0)
            success = true;
    }

    free(in_block);
    free(out_block);


    // FINALIZE REAL FRAME
    // digest hash state
    uint32_t checksum = (uint32_t)XXH32_digest(hash_state);
    uint8_t checksum_bytes[4];
    write_uint32_le(checksum_bytes, checksum);
    // write endmark
    fwrite(&endmark, 4, 1, outf);
    // content checksum
    fwrite(checksum_bytes, 1, 4, outf);

    // WRITE BLOCK OFFSET TABLE
    fseek(outf, block_offset_table_cursor, SEEK_SET);
    fwrite(mbediso_block_offsets, 1, block_count * 4, outf);

    // finalize state
    free(mbediso_block_offsets);
    XXH32_freeState(hash_state);


    return success;
}
