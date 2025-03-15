#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "FileClass.h"
#include "3dsx.h"

typedef struct
{
	void* segPtrs[3]; // code, rodata & data
	u32 segAddrs[3];
	u32 segSizes[3];
} _3DSX_LoadInfo;

static inline u32 TranslateAddr(u32 addr, _3DSX_LoadInfo* d, u32* offsets)
{
	if (addr < offsets[0])
		return d->segAddrs[0] + addr;
	if (addr < offsets[1])
		return d->segAddrs[1] + addr - offsets[0];
	return d->segAddrs[2] + addr - offsets[1];
}

int Dump3DSX(FILE* f, u32 baseAddr, FILE* fout)
{
	u32 i, j, k, m;

	_3DSX_Header hdr;
	if (fread(&hdr, sizeof(hdr), 1, f) != 1)
		return 2;

	// Endian swap!
#define ESWAP(_field, _type) \
	hdr._field = le_##_type(hdr._field)
	ESWAP(magic, word);
	ESWAP(headerSize, hword);
	ESWAP(relocHdrSize, hword);
	ESWAP(formatVer, word);
	ESWAP(flags, word);
	ESWAP(codeSegSize, word);
	ESWAP(rodataSegSize, word);
	ESWAP(dataSegSize, word);
	ESWAP(bssSize, word);
#undef ESWAP

	if (hdr.magic != _3DSX_MAGIC)
		return 3;

	_3DSX_LoadInfo d;
	d.segSizes[0] = (hdr.codeSegSize+0xFFF) &~ 0xFFF;
	d.segSizes[1] = (hdr.rodataSegSize+0xFFF) &~ 0xFFF;
	d.segSizes[2] = (hdr.dataSegSize+0xFFF) &~ 0xFFF;
	u32 offsets[2] = { d.segSizes[0], d.segSizes[0] + d.segSizes[1] };
	u32 dataLoadSize = (hdr.dataSegSize-hdr.bssSize+0xFFF) &~ 0xFFF;
	u32 bssLoadSize = d.segSizes[2] - dataLoadSize;
	u32 nRelocTables = hdr.relocHdrSize/4;
	void* allMem = malloc(d.segSizes[0]+d.segSizes[1]+d.segSizes[2]+4*3*nRelocTables);
	if (!allMem)
		return 3;
	d.segAddrs[0] = baseAddr;
	d.segAddrs[1] = d.segAddrs[0] + d.segSizes[0];
	d.segAddrs[2] = d.segAddrs[1] + d.segSizes[1];
	d.segPtrs[0] = (char*)allMem;
	d.segPtrs[1] = (char*)d.segPtrs[0] + d.segSizes[0];
	d.segPtrs[2] = (char*)d.segPtrs[1] + d.segSizes[1];

	// Skip header for future compatibility.
	fseek(f, hdr.headerSize, SEEK_SET);

	// Read the relocation headers
	u32* relocs = (u32*)((char*)d.segPtrs[2] + hdr.dataSegSize);

	for (i = 0; i < 3; i ++)
		if (fread(&relocs[i*nRelocTables], nRelocTables*4, 1, f) != 1)
			return 4;

	// Read the segments
	if (fread(d.segPtrs[0], hdr.codeSegSize, 1, f) != 1) return 5;
	if (fread(d.segPtrs[1], hdr.rodataSegSize, 1, f) != 1) return 5;
	if (fread(d.segPtrs[2], hdr.dataSegSize - hdr.bssSize, 1, f) != 1) return 5;

	// BSS clear
	memset((char*)d.segPtrs[2] + hdr.dataSegSize - hdr.bssSize, 0, hdr.bssSize);

	// Relocate the segments
	for (i = 0; i < 3; i ++)
	{
		for (j = 0; j < nRelocTables; j ++)
		{
			u32 nRelocs = le_word(relocs[i*nRelocTables+j]);
			if (j >= 2)
			{
				// We are not using this table - ignore it
				fseek(f, nRelocs*sizeof(_3DSX_Reloc), SEEK_CUR);
				continue;
			}

#define RELOCBUFSIZE 512
			static _3DSX_Reloc relocTbl[RELOCBUFSIZE];

			u32* pos = (u32*)d.segPtrs[i];
			u32* endPos = pos + (d.segSizes[i]/4);

			while (nRelocs)
			{
				u32 toDo = nRelocs > RELOCBUFSIZE ? RELOCBUFSIZE : nRelocs;
				nRelocs -= toDo;

				if (fread(relocTbl, toDo*sizeof(_3DSX_Reloc), 1, f) != 1)
					return 6;

				for (k = 0; k < toDo && pos < endPos; k ++)
				{
					//printf("(t=%d,skip=%u,patch=%u)\n", j, (u32)relocTbl[k].skip, (u32)relocTbl[k].patch);
					pos += le_hword(relocTbl[k].skip);
					u32 num_patches = le_hword(relocTbl[k].patch);
					for (m = 0; m < num_patches && pos < endPos; m ++)
					{
						u32 inAddr = baseAddr + ((char*)pos-(char*)allMem);
						u32 origData = le_word(*pos);
						u32 subType = origData >> (32-4);
						u32 addr = TranslateAddr(origData &~ 0xF0000000, &d, offsets);
						//printf("Patching %08X <-- rel(%08X,%d,%u) (%08X)\n", baseAddr+inAddr, addr, j, subType, le_word(*pos));
						switch (j)
						{
							case 0:
							{
								if (subType != 0)
									return 7;
								*pos = le_word(addr);
								break;
							}
							case 1:
							{
								u32 data = addr - inAddr;
								switch (subType)
								{
									case 0: *pos = le_word(data);            break; // 32-bit signed offset
									case 1: *pos = le_word(data &~ BIT(31)); break; // 31-bit signed offset
									default: return 8;
								}
								break;
							}
						}
						pos++;
					}
				}
			}
		}
	}

	// Write the data
	if (fwrite(allMem, d.segSizes[0] + d.segSizes[1] + dataLoadSize, 1, fout) != 1)
		return 9;
	free(allMem);

	printf("CODE:   %u pages\n", d.segSizes[0] / 0x1000);
	printf("RODATA: %u pages\n", d.segSizes[1] / 0x1000);
	printf("DATA:   %u pages\n", dataLoadSize / 0x1000);
	printf("BSS:    %u pages\n", bssLoadSize / 0x1000);

	return 0; // Success.
}

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		fprintf(stderr, "Usage:\n\t%s [inputFile] [outputFile]\n", argv[0]);
		return 1;
	}

	FILE* fin = fopen(argv[1], "rb");
	if (!fin) { printf("Cannot open input file!\n"); return 1; }

	FILE* fout = fopen(argv[2], "wb");
	if (!fout) { fclose(fin); printf("Cannot open output file!\n"); return 1; }

	int rc = Dump3DSX(fin, 0x00100000, fout);
	fclose(fin);
	fclose(fout);
	if (rc != 0)
	{
		remove(argv[2]);
		switch (rc)
		{
			case 2: printf("Cannot read header!\n"); break;
			case 3: printf("Invalid header!\n"); break;
			case 4: printf("Cannot read relocation headers!\n"); break;
			case 5: printf("Cannot read segment data!\n"); break;
			case 6: printf("Cannot read relocation table!\n"); break;
			case 7: printf("Cannot write segment data!\n"); break;
		}
	}

	return rc;
}
