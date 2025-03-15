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

#include "lz4_pack.h"

int main(int argc, char** argv)
{
    if(argc < 2)
        return -1;

    bool want_big_endian = false;
    if(argv[1][0] == '-' && argv[1][1] == 'b' && argv[1][2] == '\0')
    {
        want_big_endian = true;
        argc--;
        if(argc < 2)
            return -1;
        argv[1] = argv[2];
    }

    if(argc != 2)
        return -1;

    const char* infn = argv[1];
    std::string outfn = infn;
    outfn += ".lz4";

    FILE* inf = fopen(infn, "rb");
    FILE* outf = fopen(outfn.c_str(), "wb");

    int ret = !LZ4Pack::compress(outf, inf, 4*1024, want_big_endian);

    fclose(inf);
    fclose(outf);

    return ret;
}
