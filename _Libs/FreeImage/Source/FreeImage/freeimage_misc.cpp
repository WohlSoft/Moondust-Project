#ifdef _WIN32

#include <windows.h>
#include "FreeImage_misc.h"

void FreeImage_utf8_to_utf16(std::wstring &out, const char *in)
{
    out.resize(strlen(in));
    int newLen = MultiByteToWideChar(CP_UTF8, 0, in, out.size(), &out[0], out.size());
    out.resize(newLen);
}

#endif
