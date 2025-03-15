#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

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

struct RelocEntry
{
	u16 skip, patch;
};

struct SegConv
{
	u32 fileOff, flags, memSize, fileSize, memPos;
};

struct RelocHdr
{
	u32 cAbsolute;
	u32 cRelative;
};

struct SymConv
{
	const char* name;
	u32 addr;
	bool isFunc;

	inline SymConv(const char* n, u32 a, bool i) : name(n), addr(a), isFunc(i) { }
};

class ElfConvert
{
	FileClass fout;
	byte_t* img;
	int platFlags;

	Elf32_Shdr* elfSects;
	int elfSectCount;
	const char* elfSectNames;

	Elf32_Sym* elfSyms;
	int elfSymCount;
	const char* elfSymNames;

	u32 baseAddr, topAddr;

	vector<bool> absRelocMap, relRelocMap;
	vector<RelocEntry> relocData;

	RelocHdr relocHdr[3];

	u8 *codeSeg, *rodataSeg, *dataSeg;
	u32 codeSegSize, rodataSegSize, dataSegSize, bssSize;
	u32 codeSizeAlign, rodataSizeAlign;
	u32 rodataStart, dataStart;

	bool hasExtHeader;
	u32 extHeaderPos;

	int ScanSections();

	int ScanRelocSection(u32 vsect, u32 vsectend, byte_t* sectData, Elf32_Sym* symTab, Elf32_Rel* relTab, int relCount);
	int ScanRelocations();

	void BuildRelocs(vector<bool>& map, int pos, int posEnd, u32& count);

	void SetReloc(u32 address, vector<bool>& map)
	{
		address = (address-baseAddr)/4;
		if (address >= map.size()) return;
		map[address] = true;
	}

	bool HasReloc(u32 address, vector<bool>& map)
	{
		address = (address-baseAddr)/4;
		return map[address];
	}

	bool HasReloc(u32 address)
	{
		return HasReloc(address, absRelocMap) || HasReloc(address, relRelocMap);
	}

public:
	ElfConvert(const char* f, byte_t* i, int x)
		: fout(f, "wb"), img(i), platFlags(x), elfSyms(NULL)
		, absRelocMap(), relRelocMap()
		, relocData()
		, codeSeg(NULL), rodataSeg(NULL), dataSeg(NULL)
		, codeSegSize(0), rodataSegSize(0), dataSegSize(0), bssSize(0)
		, hasExtHeader(false), extHeaderPos(0)
	{
	}
	int Convert();

	void EnableExtHeader() { hasExtHeader = true; }
	int WriteExtHeader(const char* smdhFile, const char* romfsDir);
};

int ElfConvert::ScanRelocSection(u32 vsect, u32 vsectend, byte_t* sectData, Elf32_Sym* symTab, Elf32_Rel* relTab, int relCount)
{
	for (int i = 0; i < relCount; i ++)
	{
		Elf32_Rel* rel = relTab + i;
		u32 relInfo = le_word(rel->r_info);
		int relType = ELF32_R_TYPE(relInfo);
		Elf32_Sym* relSym = symTab + ELF32_R_SYM(relInfo);

		u32 relSymAddr = le_word(relSym->st_value);
		u32 relSrcAddr = le_word(rel->r_offset);
		u32& relSrc = *(u32*)(sectData + relSrcAddr - vsect);

		if (relSrcAddr & 3)
			die("Unaligned relocation!");

		// For some reason this has been observed to happen sometimes in .relARM.exidx
		if (relSrcAddr < vsect || relSrcAddr >= vsectend)
			continue;

		// For some reason this can happen, and we definitely don't want relocations to be processed more than once.
		if (HasReloc(relSrcAddr))
			continue;

		relSrc = le_word(relSrc);
		switch (relType)
		{
			// Notes:
			// R_ARM_TARGET2 is equivalent to R_ARM_REL32
			// R_ARM_PREL31 is an address-relative signed 31-bit offset

			case R_ARM_ABS32:
			case R_ARM_TARGET1:
			{
				// Ignore unbound weak symbols (keep them 0)
				if (ELF32_ST_BIND(relSym->st_info) == STB_WEAK && relSymAddr == 0) break;

				if (relSrc < baseAddr)
				{
					fprintf(stderr, "absolute @ relSrc=%08X\n", relSrc);
					die("Relocation to invalid address!");
				}

				// Add relocation
				relSrc -= baseAddr;
				SetReloc(relSrcAddr, absRelocMap);
				break;
			}

			case R_ARM_REL32:
			case R_ARM_TARGET2:
			case R_ARM_PREL31:
			case R_ARM_TLS_IE32:
			{
				int relocOff = relSrc;

				if (relType == R_ARM_PREL31)
				{
					// "If bit 31 is one: this is a table entry itself (ARM_EXIDX_COMPACT)"
					if (relocOff & BIT(31))
						break;

					// Otherwise, sign extend the offset
					if (relocOff & BIT(30))
						relocOff |= BIT(31);
				}

				relocOff -= (int)relSymAddr - (int)relSrcAddr;

				relSymAddr += relocOff;
				if (relSymAddr < baseAddr || relSymAddr > topAddr)
				{
					printf("relative @ relocOff=%d relSymAddr=%08X relSrcAddr=%08X topAddr=%08X\n", relocOff, relSymAddr, relSrcAddr, topAddr);
					die("Relocation to invalid address!");
				}

				if (
					((relSymAddr < rodataStart) && !(relSrcAddr < rodataStart)) ||
					((relSymAddr >= rodataStart && relSymAddr < dataStart) && !(relSrcAddr >= rodataStart && relSrcAddr < dataStart)) ||
					((relSymAddr >= dataStart   && relSymAddr <= topAddr)  && !(relSrcAddr >= dataStart   && relSrcAddr <= topAddr))
					)
				{
#ifdef DEBUG
					printf("{CrossRelReloc} %c srcAddr=%08X target=%08X relocOff=%d\n", relType == R_ARM_PREL31 ? 'R' : 'N', relSrcAddr, relSymAddr, relocOff);
#endif
					relSrc = relSymAddr - baseAddr; // Convert to absolute address
					if (relType == R_ARM_PREL31)
						relSrc |= 1 << (32-4); // Indicate this is a 31-bit relative offset
					SetReloc(relSrcAddr, relRelocMap); // Add relocation
				}

				break;
			}
		}

		relSrc = le_word(relSrc);
	}
	return 0;
}

int ElfConvert::ScanRelocations()
{
	for (int i = 0; i < elfSectCount; i ++)
	{
		Elf32_Shdr* sect = elfSects + i;
		word_t sectType = le_word(sect->sh_type);
		if (sectType == SHT_RELA)
			die("Unsupported relocation section");
		else if (sectType != SHT_REL)
			continue;

		Elf32_Shdr* targetSect = elfSects + le_word(sect->sh_info);
		if (!(le_word(targetSect->sh_flags) & SHF_ALLOC))
			continue; // Ignore non-loadable sections

		u32 vsect = le_word(targetSect->sh_addr);
		u32 vsectend = vsect + le_word(targetSect->sh_size);
		byte_t* sectData = img + le_word(targetSect->sh_offset);

		Elf32_Sym* symTab = (Elf32_Sym*)(img + le_word(elfSects[le_word(sect->sh_link)].sh_offset));
		Elf32_Rel* relTab = (Elf32_Rel*)(img + le_word(sect->sh_offset));
		int relCount = (int)(le_word(sect->sh_size) / le_word(sect->sh_entsize));

		safe_call(ScanRelocSection(vsect, vsectend, sectData, symTab, relTab, relCount));
	}

	// Scan for interworking thunks that need to be relocated
	for (int i = 0; i < elfSymCount; i ++)
	{
		Elf32_Sym* sym = elfSyms + i;
		const char* symName = (const char*)(elfSymNames + le_word(sym->st_name));
		if (!*symName) continue;
		if (symName[0] != '_' && symName[1] != '_') continue;
		if (strncmp(symName+strlen(symName)-9, "_from_arm", 9) != 0) continue;
		SetReloc(le_word(sym->st_value)+8, absRelocMap);
	}

	// Build relocs
	BuildRelocs(absRelocMap, baseAddr, rodataStart, relocHdr[0].cAbsolute);
	BuildRelocs(relRelocMap, baseAddr, rodataStart, relocHdr[0].cRelative);
	BuildRelocs(absRelocMap, rodataStart, dataStart, relocHdr[1].cAbsolute);
	BuildRelocs(relRelocMap, rodataStart, dataStart, relocHdr[1].cRelative);
	BuildRelocs(absRelocMap, dataStart, topAddr, relocHdr[2].cAbsolute);
	BuildRelocs(relRelocMap, dataStart, topAddr, relocHdr[2].cRelative);

	return 0;
}

void ElfConvert::BuildRelocs(vector<bool>& map, int pos, int posEnd, u32& count)
{
	size_t curs = relocData.size();
	pos    = (pos    - baseAddr) / 4;
	posEnd = (posEnd - baseAddr) / 4;
	for (int i = pos; i < posEnd;)
	{
		RelocEntry reloc;
		u32 rs = 0, rp = 0;
		while ((i < posEnd) && !map[i]) i ++, rs ++;
		while ((i < posEnd) && map[i]) i ++, rp ++;

		// Remove empty trailing relocations
		if (i == posEnd && rs && !rp)
			break;

		// Add excess skip relocations
		for (reloc.skip = 0xFFFF, reloc.patch = 0; rs > 0xFFFF; rs -= 0xFFFF)
			relocData.push_back(reloc);

		// Add excess patch relocations
		for (reloc.skip = rs, reloc.patch = 0xFFFF; rp > 0xFFFF; rp -= 0xFFFF)
		{
			relocData.push_back(reloc);
			rs = reloc.skip = 0;
		}

		// Add remaining relocation
		if (rs || rp)
		{
			reloc.skip = rs;
			reloc.patch = rp;
			relocData.push_back(reloc);
		}
	}
	count = le_word(relocData.size() - curs);
}

int ElfConvert::ScanSections()
{
	for (int i = 0; i < elfSectCount; i ++)
	{
		Elf32_Shdr* sect = elfSects + i;
		//auto sectName = elfSectNames + le_word(sect->sh_name);
		switch (le_word(sect->sh_type))
		{
			case SHT_SYMTAB:
				elfSyms = (Elf32_Sym*) (img + le_word(sect->sh_offset));
				elfSymCount = le_word(sect->sh_size) / sizeof(Elf32_Sym);
				elfSymNames = (const char*)(img + le_word(elfSects[le_word(sect->sh_link)].sh_offset));
				break;
		}
	}

	if (!elfSyms)
		die("ELF has no symbol table!");

	return 0;
}

int ElfConvert::Convert()
{
	if (fout.openerror())
		die("Cannot open output file!");

	Elf32_Ehdr* ehdr = (Elf32_Ehdr*) img;
	if(memcmp(ehdr->e_ident, ELF_MAGIC, 4) != 0)
		die("Invalid ELF file!");
	if(le_hword(ehdr->e_type) != ET_EXEC)
		die("ELF file must be executable! (hdr->e_type should be ET_EXEC)");

	elfSects = (Elf32_Shdr*)(img + le_word(ehdr->e_shoff));
	elfSectCount = (int)le_hword(ehdr->e_shnum);
	elfSectNames = (const char*)(img + le_word(elfSects[le_hword(ehdr->e_shstrndx)].sh_offset));

	Elf32_Phdr* phdr = (Elf32_Phdr*)(img + le_word(ehdr->e_phoff));
	bool hasBaseAddr = false;
	baseAddr = 1, topAddr = 0;

	for (int i = 0; i < le_hword(ehdr->e_phnum); i ++)
	{
		Elf32_Phdr* cur = phdr + i;
		SegConv s;
		s.fileOff = le_word(cur->p_offset);
		s.flags = le_word(cur->p_flags);
		s.memSize = le_word(cur->p_memsz);
		s.fileSize = le_word(cur->p_filesz);
		s.memPos = le_word(cur->p_vaddr);

		if (le_word(cur->p_type) != PT_LOAD) continue;
		if (!s.memSize) continue;

#ifdef DEBUG
		fprintf(stderr, "PHDR[%d]: fOff(%X) memPos(%08X) memSize(%u) fileSize(%u) flags(%08X)\n",
			i, s.fileOff, s.memPos, s.memSize, s.fileSize, s.flags);
#endif

		if (!hasBaseAddr) { hasBaseAddr = true; baseAddr = s.memPos; }
		else if (s.memPos != topAddr) die("Non-contiguous segments!");

		if (s.memSize & 3) die("The segments is not word-aligned!");
		if (s.flags != 6 && s.memSize != s.fileSize) die("Only the data segment can have a BSS!");
		if (s.fileSize & 3) die("The loadable part of the segment is not word-aligned!");

		switch (s.flags)
		{
			case 5: // code
				if (codeSeg) die("Too many code segments");
				if (rodataSeg || dataSeg) die("Code segment must be the first");
				codeSeg = img + s.fileOff;
				codeSegSize = s.memSize;
				break;
			case 4: // rodata
				if (rodataSeg) die("Too many rodata segments");
				if (dataSeg) die("Data segment must be before the code segment");
				rodataSeg = img + s.fileOff;
				rodataSegSize = s.memSize;
				break;
			case 6: // data+bss
				if (dataSeg) die("Too many data segments");
				dataSeg = img + s.fileOff;
				dataSegSize = s.memSize;
				bssSize = s.memSize - s.fileSize;
				break;
			default:
				die("Invalid segment!");
		}

		topAddr = s.memPos + ((s.memSize + 0xFFF) &~ 0xFFF);
	}

	//if (baseAddr != 0)
	//	die("Invalid executable base address!");

	if ((topAddr-baseAddr) >= 0x10000000)
		die("The executable must not be bigger than 256 MiB!");

	if (le_word(ehdr->e_entry) != baseAddr)
		die("Entrypoint should be zero!");

	codeSizeAlign = ((codeSegSize + 0xFFF) &~ 0xFFF);
	rodataSizeAlign = ((rodataSegSize + 0xFFF) &~ 0xFFF);
	rodataStart = baseAddr + codeSizeAlign;
	dataStart = rodataStart + rodataSizeAlign;

	// Create relocation bitmap
	absRelocMap.assign((topAddr - baseAddr) / 4, false);
	relRelocMap.assign((topAddr - baseAddr) / 4, false);

	safe_call(ScanSections());
	safe_call(ScanRelocations());

	// Write header
	fout.WriteWord(0x58534433); // '3DSX'
	fout.WriteHword(8*4 + (hasExtHeader ? 3*4 : 0)); // Header size
	fout.WriteHword(sizeof(RelocHdr)); // Relocation header size
	fout.WriteWord(0); // Version
	fout.WriteWord(0); // Flags

	fout.WriteWord(codeSegSize);
	fout.WriteWord(rodataSegSize);
	fout.WriteWord(dataSegSize);
	fout.WriteWord(bssSize);

	extHeaderPos = fout.Tell();
	if (hasExtHeader)
		for (int i = 0; i < 3; i ++)
			fout.WriteWord(0);

	// Write relocation headers
	for (int i = 0; i < 3; i ++)
		fout.WriteRaw(relocHdr+i, sizeof(RelocHdr));

	// Write segments
	if (codeSeg)   fout.WriteRaw(codeSeg,   codeSegSize);
	if (rodataSeg) fout.WriteRaw(rodataSeg, rodataSegSize);
	if (dataSeg)   fout.WriteRaw(dataSeg,   dataSegSize-bssSize);

	// Write relocations
//	for (auto& reloc : relocData)
//	{
	for(vector<RelocEntry>::iterator it = relocData.begin(); it != relocData.end(); ++it) {
		RelocEntry &reloc = *it;

#ifdef DEBUG
		fprintf(stderr, "RELOC {skip: %d, patch: %d}\n", (int)reloc.skip, (int)reloc.patch);
#endif
		fout.WriteHword(reloc.skip);
		fout.WriteHword(reloc.patch);
	}

	return 0;
}

int ElfConvert::WriteExtHeader(const char* smdhFile, const char* romfsDir)
{
	FILE* f = fopen(smdhFile, "rb");
	if (!f) die("Cannot open SMDH file!");

	fseek(f, 0, SEEK_END);
	u32 smdhSize = ftell(f);
	fseek(f, 0, SEEK_SET);

	u8* buf = (u8*)malloc(smdhSize);
	if (!buf)
	{
		fclose(f);
		die("Out of memory!");
	}

	int rc = fread(buf, smdhSize, 1, f);
	fclose(f);

	if (!rc)
	{
		free(buf);
		die("Cannot read SMDH data!");
	}

	u32 temp = fout.Tell();
	fout.Seek(extHeaderPos, SEEK_SET);
	fout.WriteWord(temp);
	fout.WriteWord(smdhSize);
	if (romfsDir)
	{
		u32 romfsPos = temp + smdhSize;
		romfsPos = (romfsPos + 3) &~ 3;
		fout.WriteWord(romfsPos);
	}

	fout.Seek(temp, SEEK_SET);
	fout.WriteRaw(buf, smdhSize);
	free(buf);

	while (fout.Tell() & 3)
		fout.WriteByte(0);

	if (romfsDir)
	{
		struct stat romfsStat;
		stat(romfsDir, &romfsStat);

		if (S_ISREG(romfsStat.st_mode))
		{
			/* try opening a romfs file */
			FileClass romfsFile(romfsDir, "rb");

			if (!romfsFile.openerror())
			{
				/* get the file size */
				romfsFile.Seek(0, SEEK_END);
				int size = ftell(romfsFile.get_ptr());
				romfsFile.Seek(0, SEEK_SET);

				/* create the buffer */
				uint8_t* file_data = (uint8_t*)malloc(size);

				romfsFile.ReadRaw(file_data, size);
				fout.WriteRaw(file_data, size);

				free(file_data);

				return 0;
			}
			else
			{
				fprintf(stderr, "Failed to open RomFS image %s!\n", romfsDir);
				return 1;
			}
		}

		RomFS romfs;
		safe_call(romfs.Build(romfsDir));
		safe_call(romfs.WriteToFile(fout));
	}

	return 0;
}

struct argInfo
{
	char* outFile;
	char* elfFile;
	char* smdhFile;
	char* romfsDir;
};

int usage(const char* progName)
{
	fprintf(stderr,
		"Usage:\n"
		"    %s input.elf output.3dsx [options]\n\n"
		"Options:\n"
		"    --smdh=input.smdh : Embeds SMDH metadata into the output file.\n"
		"    --romfs=input     : Embeds RomFS from a directory or raw RomFS archive into the output file.\n"
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
		if (arg[0] == '-' && arg[1] == '-')
		{
			arg += 2;
			char* value = strchr(arg, '=');
			if (!value) return usage(argv[0]);
			*value++ = 0;
			if (!*value) return usage(argv[0]);

			if (strcmp(arg, "smdh")==0)
				info.smdhFile = value;
			else if (strcmp(arg, "romfs")==0)
				info.romfsDir = value;
			else
				return usage(argv[0]);
		} else
		{
			switch (status++)
			{
				case 1: info.outFile = arg; break;
				case 0: info.elfFile = arg; break;
				default: return usage(argv[0]);
			}
		}
	}
	return status < 2 ? usage(argv[0]) : 0;
}

int main(int argc, char* argv[])
{
	argInfo args;
	safe_call(parseArgs(args, argc, argv));

	FILE *elf_file = fopen(args.elfFile, "rb");
	if (!elf_file) die("Cannot open input file!");

	fseek(elf_file, 0, SEEK_END);
	size_t elfSize = ftell(elf_file);
	rewind(elf_file);

	byte_t* b = (byte_t*) malloc(elfSize);
	if (!b) { fclose(elf_file); die("Cannot allocate memory!"); }

	fread(b, 1, elfSize, elf_file);
	fclose(elf_file);

	int rc = 0;
	do {
		ElfConvert cnv(args.outFile, b, 0);

		bool hasExtHeader = args.smdhFile || args.romfsDir;
		if (hasExtHeader)
			cnv.EnableExtHeader();

		rc = cnv.Convert();
		if (rc != 0) break;

		if (hasExtHeader)
			rc = cnv.WriteExtHeader(args.smdhFile, args.romfsDir);
	} while(0);
	free(b);

	if (rc != 0)
		remove(args.outFile);

	return rc;
}
