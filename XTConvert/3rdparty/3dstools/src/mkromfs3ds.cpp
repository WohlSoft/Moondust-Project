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

using std::vector;
using std::map;

#define die(msg) do { fputs(msg "\n\n", stderr); return 1; } while(0)
#define safe_call(a) do { int rc = a; if(rc != 0) return rc; } while(0)

struct argInfo
{
	char* outFile;
	char* romfsDir;
};

int usage(const char* progName)
{
	fprintf(stderr,
		"Usage:\n"
		"    %s input_dir output.romfs\n\n"
		, progName);
	return 1;
}

int parseArgs(argInfo& info, int argc, char* argv[])
{
	memset(&info, 0, sizeof(info));

	int status = 0;
	for (int i = 1; i < argc; i ++)
	{
		char* arg = argv[i];
		switch (status++)
		{
			case 1: info.outFile = arg; break;
			case 0: info.romfsDir = arg; break;
			default: return usage(argv[0]);
		}
	}
	return status < 2 ? usage(argv[0]) : 0;
}

int main(int argc, char* argv[])
{
	argInfo args;
	safe_call(parseArgs(args, argc, argv));

	RomFS romfs;
	safe_call(romfs.Build(args.romfsDir));
	FileClass fout(args.outFile, "wb");
	safe_call(romfs.WriteToFile(fout));

	return 0;
}
