#include "file_open.h"
/*
 * Forces to look for unicode paths on Windows
 */
#ifdef _WIN32
#include <windows.h>

static size_t utf8len(const char *s)
{
    size_t len = 0;
    while(*s)
        len += (*(s++)&0xC0)!=0x80;
    return len;
}

FILE *_lua_fopen(const char *filename, const char *mode)
{
    int fn_len=utf8len(filename);
    int fn_len_s=strlen(filename);
    if(fn_len==0) return NULL;

    int m_len=utf8len(mode);
    int m_len_s=strlen(mode);
    if(m_len==0) return NULL;

    wchar_t *path = (wchar_t*) LocalAlloc(LMEM_ZEROINIT, sizeof(wchar_t) * fn_len_s);
    wchar_t *wmode = (wchar_t*) LocalAlloc(LMEM_ZEROINIT, sizeof(wchar_t) *m_len_s);
    MultiByteToWideChar(CP_UTF8, 0, filename, fn_len_s, path, fn_len_s);
    MultiByteToWideChar(CP_UTF8, 0, mode, m_len_s, wmode, m_len_s);
    FILE *f = _wfopen(path, wmode);
    LocalFree(path);
    LocalFree(wmode);
    return f;
}

FILE *_lua_freopen(const char *filename, const char *mode, FILE * oldfile)
{
    int fn_len=utf8len(filename);
    int fn_len_s=strlen(filename);
    if(fn_len==0) return NULL;

    int m_len=utf8len(filename);
    int m_len_s=strlen(filename);
    if(m_len==0) return NULL;

    wchar_t *path = (wchar_t*) LocalAlloc(LMEM_ZEROINIT, sizeof(wchar_t) * fn_len_s);
    wchar_t *wmode = (wchar_t*) LocalAlloc(LMEM_ZEROINIT, sizeof(wchar_t) *m_len_s);

    MultiByteToWideChar(CP_UTF8, 0, filename, fn_len_s, path, fn_len_s);
    MultiByteToWideChar(CP_UTF8, 0, mode, m_len_s, wmode, m_len_s);
    FILE *f = _wfreopen(path, wmode, oldfile);
    LocalFree(path);
    LocalFree(wmode);
    return f;
}

#endif
