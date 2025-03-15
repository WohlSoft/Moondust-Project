/*
 * XTConvert - a tool to package asset packs and episodes for TheXTech game engine.
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

#include "archive.h"
#include "archive_entry.h"

#include "lz4file.h"

namespace XTConvert
{

#define CHUNK_SIZE (16*1024)

static int decompress_file_lz4(FILE* f_in, FILE* f_out)
{
    if(!f_in || !f_out)
        return 1;

    LZ4F_errorCode_t ret = LZ4F_OK_NoError;
    LZ4_readFile_t* lz4fRead;
    void* const buf = malloc(CHUNK_SIZE);
    if (!buf) {
        printf("error: memory allocation failed \n");
    }

    ret = LZ4F_readOpen(&lz4fRead, f_in);
    if (LZ4F_isError(ret)) {
        printf("LZ4F_readOpen error: %s\n", LZ4F_getErrorName(ret));
        free(buf);
        return 1;
    }

    while (1) {
        ret = LZ4F_read(lz4fRead, buf, CHUNK_SIZE);
        if (LZ4F_isError(ret)) {
            printf("LZ4F_read error: %s\n", LZ4F_getErrorName(ret));
            goto out;
        }

        /* nothing to read */
        if (ret == 0) {
            break;
        }

        if(fwrite(buf, 1, ret, f_out) != ret) {
            printf("write error!\n");
            goto out;
        }
    }

out:
    free(buf);
    if (LZ4F_isError(LZ4F_readClose(lz4fRead))) {
        printf("LZ4F_readClose: %s\n", LZ4F_getErrorName(ret));
        return 1;
    }

    if (ret) {
        return 1;
    }

    return 0;
}

static int copy_data(struct archive* ar, struct archive* aw)
{
    int r;
    const void* buff;
    size_t size;
#if ARCHIVE_VERSION_NUMBER >= 3000000
    int64_t offset;
#else
    off_t offset;
#endif

    for (;;) {
        r = archive_read_data_block(ar, &buff, &size, &offset);
        if(r == ARCHIVE_EOF)
            return (ARCHIVE_OK);
        if(r != ARCHIVE_OK)
            return (r);
        r = archive_write_data_block(aw, buff, size, offset);
        if(r != ARCHIVE_OK)
            return (r);
    }
}

static bool extract_archive(const char* dest_dir, const char* data, size_t size)
{
    std::string target = dest_dir;
    if(target.back() != '/' && target.back() != '\\')
        target += '/';

    size_t target_base_size = target.size();

    std::string to_remove;

    struct archive* a = nullptr;
    struct archive* ext = nullptr;
    struct archive_entry* entry = nullptr;

    bool ret = false;

    a = archive_read_new();
    if(!a)
    {
        printf("error 1\n");
        goto cleanup;
    }

    ext = archive_write_disk_new();
    if(!ext)
    {
        printf("error 2\n");
        goto cleanup;
    }

    archive_write_disk_set_options(ext, ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_SECURE_NODOTDOT | ARCHIVE_EXTRACT_SECURE_SYMLINKS);

    archive_read_support_filter_gzip(a);
    archive_read_support_format_tar(a);
    archive_read_support_format_zip(a);
    archive_read_support_format_rar(a);
    archive_read_support_format_7zip(a);
    archive_read_support_format_iso9660(a);

    if(size)
    {
        if(archive_read_open_memory(a, data, size) != ARCHIVE_OK)
        {
            printf("error 3\n");
            goto cleanup;
        }
    }
    else
    {
        const char* use_fn = data;

        // check for LZ4
        FILE* f = fopen(data, "rb");
        char buf[4];
        if(f && fread(buf, 1, 4, f) == 4 && buf[0] == 0x04 && buf[1] == 0x22 && buf[2] == 0x4d && buf[3] == 0x18)
        {
            fseek(f, 0, SEEK_SET);

            target += "_temp.iso";

            FILE* f_out = fopen(target.c_str(), "wb");
            if(f_out && decompress_file_lz4(f, f_out) == 0)
            {
                use_fn = target.c_str();
                to_remove = target;
            }

            if(f_out)
                fclose(f_out);
        }

        if(f)
            fclose(f);

        if(archive_read_open_filename(a, use_fn, 10240) != ARCHIVE_OK)
        {
            printf("error 4 %s\n", archive_error_string(a));
            goto cleanup;
        }
    }

    while(true)
    {
        int r = archive_read_next_header(a, &entry);
        if(r == ARCHIVE_EOF)
            break;

        if(r != ARCHIVE_OK)
        {
            printf("error 5 %s\n", archive_error_string(a));
            goto cleanup;
        }

        target.resize(target_base_size);
        target += archive_entry_pathname(entry);
        archive_entry_set_pathname(entry, target.c_str());
        archive_entry_set_perm(entry, 0755);

        if(archive_write_header(ext, entry) != ARCHIVE_OK)
        {
            printf("error 6 %s\n", archive_error_string(ext));
            goto cleanup;
        }

        if(copy_data(a, ext) != ARCHIVE_OK)
        {
            printf("error 7\n");
            goto cleanup;
        }

        if(archive_write_finish_entry(ext) != ARCHIVE_OK)
        {
            printf("error 8\n");
            goto cleanup;
        }
    }

    ret = true;

cleanup:

    if(a)
    {
        archive_read_close(a);
        archive_read_free(a);
    }

    if(ext)
    {
        archive_write_close(ext);
        archive_write_free(ext);
    }

    if(!to_remove.empty())
        remove(to_remove.c_str());

    return ret;
}

bool extract_archive_file(const char* dest_dir, const char* filename)
{
    return extract_archive(dest_dir, filename, 0);
}

bool extract_archive_data(const char* dest_dir, const uint8_t* data, size_t len)
{
    return extract_archive(dest_dir, (const char*)data, len);
}

} // namespace XTConvert
