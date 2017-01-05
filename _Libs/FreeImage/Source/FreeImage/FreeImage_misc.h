#ifndef MISC_H
#define MISC_H

#ifdef _WIN32
#include <string>
void FreeImage_utf8_to_utf16(std::wstring &out, const char *in);
#endif

#endif // MISC_H
