#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <map>
#include <list>
#include <algorithm>
#include "types.h"
#include "elf.h"
#include "FileClass.h"
#include "romfs.h"

#include "libromfs3ds.h"

#define safe_call(a) do { int rc = a; if(rc != 0) return rc; } while(0)

struct argInfo
{
    char* outFile;
    char* romfsDir;
};

int LibRomFS3DS::MkRomfs(const char* out_file, const char* romfs_dir)
{
    RomFS romfs;
    safe_call(romfs.Build(romfs_dir));

    FileClass fout(out_file, "wb");
    safe_call(romfs.WriteToFile(fout));

    return 0;
}
