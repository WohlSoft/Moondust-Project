#include "files.h"
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>

static std::wstring Str2WStr(const std::string &path)
{
    std::wstring wpath;
    wpath.resize(path.size());
    int newlen = MultiByteToWideChar(CP_UTF8, 0, path.c_str(), path.length(), &wpath[0], path.length());
    wpath.resize(newlen);
    return wpath;
}
#else
#include <unistd.h>
#include <string.h>
#endif

#if defined(__CYGWIN__) || defined(__DJGPP__) || defined(__MINGW32__)
#define IS_PATH_SEPARATOR(c) (((c) == '/') || ((c) == '\\'))
#else
#define IS_PATH_SEPARATOR(c) ((c) == '/')
#endif

static char fi_path_dot[] = ".";
static char fi_path_root[] = "/";

static char *fi_basename(char *s)
{
    char *rv;

    if(!s || !*s)
        return fi_path_dot;

    rv = s + strlen(s) - 1;

    do
    {
        if(IS_PATH_SEPARATOR(*rv))
            return rv + 1;
        --rv;
    }
    while(rv >= s);

    return s;
}

static char *fi_dirname(char *path)
{
    char *p;

    if(path == NULL || *path == '\0')
        return fi_path_dot;

    p = path + strlen(path) - 1;
    while(IS_PATH_SEPARATOR(*p))
    {
        if(p == path)
            return path;
        *p-- = '\0';
    }

    while(p >= path && !IS_PATH_SEPARATOR(*p))
        p--;

    if(p < path)
        return fi_path_dot;

    if(p == path)
        return fi_path_root;

    *p = '\0';
    return path;
}

bool Files::fileExists(const std::string &path)
{
    #ifdef _WIN32
    std::wstring wpath = Str2WStr(path);
    return PathFileExistsW(wpath.c_str()) == TRUE;
    #else
    FILE *ops = fopen(path.c_str(), "rb");
    if(ops)
    {
        fclose(ops);
        return true;
    }
    return false;
    #endif
}

bool Files::deleteFile(const std::string &path)
{
    #ifdef _WIN32
    std::wstring wpath = Str2WStr(path);
    return (DeleteFileW(wpath.c_str()) == TRUE);
    #else
    return ::unlink(path.c_str()) == 0;
    #endif
}

std::string Files::dirname(std::string path)
{
    char *p = strdup(path.c_str());
    char *d = ::fi_dirname(p);
    path = d;
    free(p);
    return path;
}

std::string Files::basename(std::string path)
{
    char *p = strdup(path.c_str());
    char *d = ::fi_basename(p);
    path = d;
    free(p);
    return path;
}

std::string Files::changeSuffix(std::string path, const std::string &suffix)
{
    size_t pos = path.find_last_of('.');// Find dot
    if((path.size() < suffix.size()) || (pos == std::string::npos))
        path.append(suffix);
    else
        path.replace(pos, suffix.size(), suffix);
    return path;
}
