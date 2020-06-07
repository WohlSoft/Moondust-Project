#ifndef LunaLoaderPatch_hhhhh
#define LunaLoaderPatch_hhhhh

enum LunaLoaderResult
{
    LUNALOADER_OK = 0,
    LUNALOADER_CREATEPROCESS_FAIL,
    LUNALOADER_PATCH_FAIL
};

LunaLoaderResult LunaLoaderRun(const wchar_t *pathToSMBX, const wchar_t *cmdLine, const wchar_t *workingDir = NULL);

const wchar_t *LunaLoaderGetLastError();

#endif
