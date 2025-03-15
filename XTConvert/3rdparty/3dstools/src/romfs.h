#pragma once
#ifdef WIN32
#include <windows.h>
typedef WCHAR oschar_t;
#define OSPATHLEN MAX_PATH
#define OSWILDCARD L"\\*"
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
typedef char oschar_t;
#define OSPATHLEN PATH_MAX
#define OSWILDCARD "/"
#endif

typedef std::vector<u16> romfs_str;

struct romfs_meta_t
{
	romfs_str name;
	u32 offset;
	u32 nextHash;

	romfs_meta_t() : name(), offset(0), nextHash(~0) { }
};

struct romfs_file_t; // Forward declaration

struct romfs_dir_t : public romfs_meta_t
{
	romfs_dir_t *parent;
	romfs_dir_t *sibling;
	romfs_dir_t *firstSubDir;
	romfs_file_t *firstFile;

	romfs_dir_t() : romfs_meta_t(), parent(NULL), sibling(NULL), firstSubDir(NULL), firstFile(NULL) { }
};

struct romfs_file_t : public romfs_meta_t
{
	romfs_dir_t *parent;
	romfs_file_t *sibling;
	u64 dataOff, dataSize;
	void* data;

	romfs_file_t(romfs_dir_t* parent) : romfs_meta_t(), parent(parent), sibling(NULL), dataOff(0), dataSize(0), data(NULL) { }
	~romfs_file_t() { if (data) free(data); }
};

class RomFS
{
	u32 *dirHashTable, *fileHashTable;
	u32 dirHashCount, fileHashCount;

	u32 dirOff, fileOff;
	u64 fileDataOff;

	std::list<romfs_dir_t> dirs;
	std::list<romfs_file_t> files;

	romfs_dir_t& AddDir(romfs_dir_t* parent, const oschar_t* name);
	romfs_file_t& AddFile(romfs_dir_t* parent, const oschar_t* name);

	romfs_dir_t& Root() { return dirs.front(); }

	int ScanDir(romfs_dir_t& dir, const oschar_t* path);
	int CalcHash(void);

public:
	RomFS();
	~RomFS();
	int Build(const char* path);
	int WriteToFile(FileClass& f);
};
