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

int main(int argc, char** argv)
{
    const char* fn = (argc > 1) ? argv[1] : NULL;
    if(!fn)
        return 1;

    struct mbediso_fs* fs = mbediso_openfs_file(fn, false);

    if(!fs)
    {
        printf("Failed to load structure\n");
        return 1;
    }

    char req_fn[1024];

    req_fn[0] = '\0';

    while(scanf(" %1023[^\n]", req_fn) == 1)
    {
        if(strcmp(req_fn, "<scan>") == 0)
        {
            mbediso_scanfs(fs);
            continue;
        }

        struct mbediso_file* f = mbediso_fopen(fs, req_fn);
        if(f)
        {
            printf("Found file with length %lld\n", (long long)mbediso_fsize(f));
            mbediso_fclose(f);
            goto cont;
        }

        struct mbediso_dir* dir = mbediso_opendir(fs, req_fn);
        if(dir)
        {
            printf("Directory listing:\n");
            for(const struct mbediso_dirent* p = mbediso_readdir(dir); p != NULL; p = mbediso_readdir(dir))
                printf("  %d name [%s]\n", (int)(p->d_type == MBEDISO_DT_DIR), (const char*)p->d_name);

            mbediso_closedir(dir);
            goto cont;
        }

        printf("Could not find.\n");

cont:
        req_fn[0] = '\0';
    }

    mbediso_closefs(fs);
    fs = NULL;

    return 0;
}
