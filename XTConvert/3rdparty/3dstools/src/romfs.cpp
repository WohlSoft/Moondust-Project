#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <map>
#include <list>
#include <algorithm>
#include "types.h"
#include "FileClass.h"
#include "romfs.h"

#define die(msg) do { fputs(msg "\n\n", stderr); return 1; } while(0)
#define safe_call(a) do { int rc = a; if(rc != 0) return rc; } while(0)

// Apparently this is Nintendo's version of "Smallest prime >= the input"
static u32 calcHashTableLen(u32 entryCount)
{
#define D(a) (count % (a) == 0)
	u32 count = entryCount;
	if (count < 3) count = 3;
	else if (count < 19) count |= 1;
	else while (D(2)||D(3)||D(5)||D(7)||D(11)||D(13)||D(17)) count ++;
	return count;
#undef D
}

static u32 calcHash(u32 parent, const romfs_str& str, u32 total)
{
	u32 hash = parent ^ 123456789;
	for (u32 i = 0; i < str.size(); i ++)
	{
		hash = (hash >> 5) | (hash << 27);
		hash ^= str[i];
	}
	return hash % total;
}

RomFS::RomFS() :
	dirHashTable(NULL), fileHashTable(NULL),
	dirOff(0), fileOff(0), fileDataOff(0),
	dirs(), files()
{
	// Create the root
	AddDir(NULL, NULL);
}

RomFS::~RomFS()
{
	if (dirHashTable) free(dirHashTable);
	if (fileHashTable) free(fileHashTable);
}

int RomFS::Build(const char* path)
{
#ifndef WIN32
	safe_call(ScanDir(Root(), path));
#else
	WCHAR buf[OSPATHLEN];
	if (!MultiByteToWideChar(CP_ACP, 0, path, -1, buf, OSPATHLEN))
		die("Cannot convert to Unicode");
	safe_call(ScanDir(Root(), buf));
#endif
	safe_call(CalcHash());
	return 0;
}

static inline u32 offset(romfs_meta_t* m)
{
	return m ? m->offset : (~0);
}

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
static void writeWordArraySlow(FileClass& f, const u32* start, const u32* end)
{
	for (; start < end; start ++)
		f.WriteWord(*start);
}

static void writeHwordArraySlow(FileClass& f, const u16* start, const u16* end)
{
	for (; start < end; start ++)
		f.WriteHword(*start);
}
#endif

int RomFS::WriteToFile(FileClass& f)
{
	u32 counter = 0x28, temp;
	f.WriteWord(counter);
	f.WriteWord(counter);
	temp = dirHashCount*4; f.WriteWord(temp); counter += temp;
	f.WriteWord(counter);
	f.WriteWord(dirOff); counter += dirOff;
	f.WriteWord(counter);
	temp = fileHashCount*4; f.WriteWord(temp); counter += temp;
	f.WriteWord(counter);
	f.WriteWord(fileOff); counter += fileOff;
	f.WriteWord(counter);

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	f.WriteRaw(dirHashTable, dirHashCount*4);
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	writeWordArraySlow(f, dirHashTable, dirHashTable+dirHashCount);
#endif
	for (std::list<romfs_dir_t>::iterator it = dirs.begin(); it != dirs.end(); ++it)
	{
		romfs_dir_t& dir = *it;
		f.WriteWord(offset(dir.parent));
		f.WriteWord(offset(dir.sibling));
		f.WriteWord(offset(dir.firstSubDir));
		f.WriteWord(offset(dir.firstFile));
		f.WriteWord(dir.nextHash);
		f.WriteWord(dir.name.size()*2);
		if (dir.name.size()==0) continue;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		f.WriteRaw(&dir.name.front(), dir.name.size()*2);
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
		writeHwordArraySlow(f, &dir.name.front(), &dir.name.front()+dir.name.size());
#endif
		while (f.Tell() & 3) f.WriteByte(0);
	}

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	f.WriteRaw(fileHashTable, fileHashCount*4);
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	writeWordArraySlow(f, fileHashTable, fileHashTable+fileHashCount);
#endif
	for (std::list<romfs_file_t>::iterator it = files.begin(); it != files.end(); ++it)
	{
		romfs_file_t& file = *it;
		f.WriteWord(offset(file.parent));
		f.WriteWord(offset(file.sibling));
		f.WriteDword(file.dataOff);
		f.WriteDword(file.dataSize);
		f.WriteWord(file.nextHash);
		f.WriteWord(file.name.size()*2);
		if (file.name.size()==0) continue;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		f.WriteRaw(&file.name.front(), file.name.size()*2);
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
		writeHwordArraySlow(f, &file.name.front(), &file.name.front()+file.name.size());
#endif
		while (f.Tell() & 3) f.WriteByte(0);
	}

	for (std::list<romfs_file_t>::iterator it = files.begin(); it != files.end(); ++it)
	{
		romfs_file_t& file = *it;
		f.WriteRaw(file.data, file.dataSize);
		while (f.Tell() & 3) f.WriteByte(0);
	}

	return 0;
}

static u32 osstrlen(const oschar_t* str)
{
	u32 i = 0;
	while (*str++) i++;
	return i;
}

static oschar_t* osstrcpy(oschar_t* dest, const oschar_t* src, u32 size)
{
	u32 i;
	for (i = 0; i < size; i ++)
	{
		dest[i] = src[i];
		if (!dest[i]) break;
	}
	return dest+i;
}

int RomFS::ScanDir(romfs_dir_t& dir, const oschar_t* path)
{
	oschar_t buf[OSPATHLEN];
	oschar_t* pos = osstrcpy(buf, path, OSPATHLEN);
	u32 remSpace = OSPATHLEN - (pos-buf);
	if (remSpace < 3) die("Path too long");
	osstrcpy(pos, OSWILDCARD, remSpace);

#if WIN32
#define _FILENAME  ffd.cFileName
#define _FILESIZE  ((u64)ffd.nFileSizeLow | ((u64)ffd.nFileSizeHigh << 32))
#define _FILEISHID (ffd.dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM))
#define _FILEISDIR (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	WIN32_FIND_DATAW ffd;
	HANDLE hFind = FindFirstFileW(buf, &ffd);
	if (hFind != INVALID_HANDLE_VALUE) do
#else
#define _FILENAME  pent->d_name
#define _FILESIZE  statbuf.st_size
#define _FILEISHID (pent->d_name[0] == '.')
#define _FILEISDIR S_ISDIR(statbuf.st_mode)
	DIR* _dir = opendir(path);
	if (!_dir)
	{
		fprintf(stderr, "Failed to open directory %s!\n", path);
		return 1;
	}
	struct dirent* pent;
	while ((pent = readdir(_dir)) != NULL)
#endif
	{
		if (_FILEISHID)
			continue;

		osstrcpy(pos+1, _FILENAME, remSpace-1);

#ifndef WIN32
		struct stat statbuf;
		if (stat(buf, &statbuf) < 0) die("stat() failed");
#endif

		if (_FILEISDIR)
		{
			if (pos[1]=='.' && (!pos[2] || (pos[2]=='.' && !pos[3])))
				continue;
			romfs_dir_t& child = AddDir(&dir, _FILENAME);
			child.sibling = dir.firstSubDir;
			dir.firstSubDir = &child;
			safe_call(ScanDir(child, buf));
		} else
		{
			romfs_file_t& child = AddFile(&dir, _FILENAME);
			child.sibling = dir.firstFile;
			dir.firstFile = &child;
			child.dataSize = _FILESIZE;
			child.dataOff = fileDataOff;
			fileDataOff += child.dataSize;
			fileDataOff = (fileDataOff + 3) &~ 3;

			child.data = malloc(child.dataSize);
			if (!child.data) die("Out of memory");

#ifdef WIN32
			HANDLE hFile = CreateFileW(buf, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile == INVALID_HANDLE_VALUE) die("Could not open file");

			DWORD bytesRead = 0;
			BOOL rc = ReadFile(hFile, child.data, child.dataSize, &bytesRead, NULL);
			CloseHandle(hFile);
#else
			int fd = open(buf, O_RDONLY);
			if (fd < 0) die("Could not open file");

			bool rc = read(fd, child.data, child.dataSize) == child.dataSize;
			close(fd);
#endif

			if (!rc) die("Could not read file");
		}
	}
#ifdef WIN32
	while (FindNextFileW(hFind, &ffd));
	FindClose(hFind);
#else
	closedir(_dir);
#endif

	return 0;
}

int RomFS::CalcHash(void)
{
	dirHashCount = calcHashTableLen(dirs.size());
	fileHashCount = calcHashTableLen(files.size());
	dirHashTable = (u32*)malloc(dirHashCount*4);
	fileHashTable = (u32*)malloc(fileHashCount*4);
	if (!dirHashTable || !fileHashTable) die("Out of memory!");

	memset(dirHashTable, 0xFF, dirHashCount*4);
	memset(fileHashTable, 0xFF, fileHashCount*4);

	for (std::list<romfs_dir_t>::iterator it = dirs.begin(); it != dirs.end(); ++it)
	{
		romfs_dir_t& dir = *it;
		u32 hash = calcHash(dir.parent->offset, dir.name, dirHashCount);
		dir.nextHash = dirHashTable[hash];
		dirHashTable[hash] = dir.offset;
	}

	for (std::list<romfs_file_t>::iterator it = files.begin(); it != files.end(); ++it)
	{
		romfs_file_t& file = *it;
		u32 hash = calcHash(file.parent->offset, file.name, fileHashCount);
		file.nextHash = fileHashTable[hash];
		fileHashTable[hash] = file.offset;
	}

	return 0;
}

#ifndef WIN32
// Function written by mtheall
static ssize_t decode_utf8(uint32_t *out, const uint8_t *in)
{
	uint8_t code1, code2, code3, code4;

	code1 = *in++;
	if (code1 < 0x80)
	{
		// 1-byte sequence
		*out = code1;
		return 1;
	} else if (code1 < 0xC2)
		return -1;
	else if (code1 < 0xE0)
	{
		// 2-byte sequence
		code2 = *in++;
		if ((code2 & 0xC0) != 0x80)
			return -1;

		*out = (code1 << 6) + code2 - 0x3080;
		return 2;
	} else if (code1 < 0xF0)
	{
		/* 3-byte sequence */
		code2 = *in++;
		if ((code2 & 0xC0) != 0x80)
			return -1;
		if (code1 == 0xE0 && code2 < 0xA0)
			return -1;

		code3 = *in++;
		if ((code3 & 0xC0) != 0x80)
			return -1;

		*out = (code1 << 12) + (code2 << 6) + code3 - 0xE2080;
		return 3;
	} else if(code1 < 0xF5)
	{
		// 4-byte sequence
		code2 = *in++;
		if ((code2 & 0xC0) != 0x80)
			return -1;
		if (code1 == 0xF0 && code2 < 0x90)
			return -1;
		if (code1 == 0xF4 && code2 >= 0x90)
			return -1;

		code3 = *in++;
		if ((code3 & 0xC0) != 0x80)
			return -1;

		code4 = *in++;
		if ((code4 & 0xC0) != 0x80)
			return -1;

		*out = (code1 << 18) + (code2 << 12) + (code3 << 6) + code4 - 0x3C82080;
		return 4;
	}

	return -1;
}
#endif

static void setStr(romfs_str& rstr, const oschar_t* ostr)
{
#ifdef WIN32
	u32 strsize = osstrlen(ostr);
	rstr = romfs_str(ostr, ostr+strsize);
#else
	// We need to convert the UTF-8 input into UTF-16
	rstr.clear();
	for (;;)
	{
		uint32_t code = 0;
		ssize_t units = decode_utf8(&code, (const uint8_t*)ostr);
		if (units == -1)
		{
			rstr.push_back(0xFFFD); // Replacement character
			ostr++;
			continue;
		}
		if (code == 0)
			break;

		// Encode Unicode codepoint as UTF-16
		if (code < 0x10000)
			rstr.push_back(code);
		else if (code < 0x110000)
		{
			rstr.push_back((code >> 10) + 0xD7C0);
			rstr.push_back((code & 0x3FF) + 0xDC00);
		} else
			rstr.push_back(0xFFFD); // Replacement character

		ostr += units;
	}
#endif
}

romfs_dir_t& RomFS::AddDir(romfs_dir_t* parent, const oschar_t* name)
{
	dirs.push_back(romfs_dir_t());
	romfs_dir_t& dir = dirs.back();
	dir.offset = dirOff;
	dir.parent = parent ? parent : &dir;
	if (name)
		setStr(dir.name, name);
	dirOff += 0x18 + dir.name.size()*2;
	dirOff = (dirOff + 3) &~ 3;
	return dir;
}

romfs_file_t& RomFS::AddFile(romfs_dir_t* parent, const oschar_t* name)
{
	files.push_back(romfs_file_t(parent));
	romfs_file_t& file = files.back();
	file.offset = fileOff;
	setStr(file.name, name);
	fileOff += 0x20 + file.name.size()*2;
	fileOff = (fileOff + 3) &~ 3;
	return file;
}
