/*
INI Processor - a small library which allows you parsing INI-files

Copyright (c) 2017 Vitaliy Novichkov <admin@wohlnet.ru>

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

*/

//#define USE_FILE_MAPPER

/* Stop parsing on first error (default is to keep parsing). */
//#define INI_STOP_ON_FIRST_ERROR

#include "ini_processing.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <clocale>
#include <sstream>
#include <assert.h>
#ifdef _WIN32
#include <windows.h>
#endif

#ifdef USE_FILE_MAPPER
/*****Replace this with right path to file mapper class*****/
#include "../fileMapper/file_mapper.h"
#endif

static const unsigned char utfbom[3] = {0xEF, 0xBB, 0xBF};

enum { Space = 0x01, Special = 0x02, INIParamEq = 0x04 };

static const unsigned char charTraits[256] =
{
    // Space: '\t', '\n', '\r', ' '
    // Special: '\n', '\r', '"', ';', '=', '\\'
    // INIParamEq: ':', '='

    0, 0, 0, 0, 0, 0, 0, 0, 0, Space, Space | Special, 0, 0, Space | Special,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Space, 0, Special,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, INIParamEq,
    Special, 0, Special | INIParamEq, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, Special, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

#if 0//for speed comparison who faster - macro or inline function. Seems speeds are same
#define IS_SPACE(c) (charTraits[static_cast<unsigned char>(c)] & Space)
#define IS_SPECIAL(c) (charTraits[static_cast<unsigned char>(c)] & Special)
#define IS_INIEQUAL(c) (charTraits[static_cast<unsigned char>(c)] & INIParamEq)
#else
inline unsigned char IS_SPACE(char &c)
{
    return (charTraits[static_cast<unsigned char>(c)] & Space);
}
inline unsigned char IS_SPECIAL(char &c)
{
    return (charTraits[static_cast<unsigned char>(c)] & Special);
}
inline unsigned char IS_INIEQUAL(char &c)
{
    return (charTraits[static_cast<unsigned char>(c)] & INIParamEq);
}
#endif

/* Strip whitespace chars off end of given string, in place. Return s. */
inline char *rstrip(char *s)
{
    char *p = s + strlen(s);

    while(p > s && IS_SPACE(*--p))
        *p = '\0';

    return s;
}

/* Return pointer to first non-whitespace char in given string. */
inline char *lskip(char *s)
{
    while(*s && IS_SPACE(*s))
        s++;

    return reinterpret_cast<char *>(s);
}

inline char *lrtrim(char *s)
{
    while(*s && IS_SPACE(*s))
        s++;

    char *p = s + strlen(s);

    while(p > s && IS_SPACE(*--p))
        *p = '\0';

    return s;
}

/* Return pointer to first char c or ';' comment in given string, or pointer to
   null at end of string if neither found. ';' must be prefixed by a whitespace
   character to register as a comment. */
inline char *find_char_or_comment(char *s, char c)
{
    unsigned char was_whitespace = 0;

    while(*s && *s != c && !(was_whitespace && *s == ';'))
    {
        was_whitespace = IS_SPACE(*s);
        s++;
    }

    return s;
}

inline char *find_inieq_or_comment(char *s)
{
    unsigned char was_whitespace = 0;

    while(*s && (!IS_INIEQUAL(*s)) && !(was_whitespace && *s == ';'))
    {
        was_whitespace = IS_SPACE(*s);
        s++;
    }

    return s;
}

inline char *removeQuotes(char *begin, char *end)
{
    if((*begin == '\0') || (begin == end))
        return begin;

    if((*begin == '"') && (begin + 1 != end))
        begin++;
    else
        return begin;

    if(*(end - 1) == '"')
        *(end - 1) = '\0';

    return begin;
}

//Remove comment line from a tail of value
inline void skipcomment(char *value)
{
    unsigned char quoteDepth = 0;

    while(*value)
    {
        if(quoteDepth > 0)
        {
            if(*value == '\\')
                continue;

            if(*value == '"')
                --quoteDepth;
        }
        else if(*value == '"')
            ++quoteDepth;

        if((quoteDepth == 0) && (*value == ';'))
        {
            *value = '\0';
            break;
        }

        value++;
    }
}

inline bool memfgets(char *&line, char *data, char *&pos, char *end)
{
    line = pos;

    while(pos != end)
    {
        if(*pos == '\n')
        {
            if((pos > data) && (*(pos - 1) == '\r'))
                *((pos++) - 1) = '\0';//Support CRLF too
            else
                *(pos++) = '\0';

            break;
        }

        ++pos;
    }

    return (pos != line);
    //EOF is a moment when position wasn't changed.
    //If do check "pos != end", will be an inability to read last line.
    //this logic allows detect true EOF when line is really eof
}

/* See documentation in header file. */
bool IniProcessing::parseHelper(char *data, size_t size)
{
    char *section = nullptr;
    #if defined(INI_ALLOW_MULTILINE)
    char *prev_name = nullptr;
    #endif
    char *start;
    char *end;
    char *name;
    char *value;
    int lineno = 0;
    int error = 0;
    char *line;
    char *pos_end = data + size;
    char *pos_cur = data;
    params::IniKeys *recentKeys = nullptr;

    /* Scan through file line by line */
    //while (fgets(line, INI_MAX_LINE, file) != NULL)
    while(memfgets(line, data, pos_cur, pos_end))
    {
        lineno++;
        start = line;

        if((lineno == 1) && (size >= 3) && (memcmp(start, utfbom, 3) == 0))
            start += 3;

        start = lrtrim(start);

        if(!*start)//if empty line - skip it away!
            continue;

        switch(*start)
        {
        case ';':
        case '#':
            //if (*start == ';' || *start == '#') {
            //    /* Per Python ConfigParser, allow '#' comments at start of line */
            //}
            continue;

        case '[':
        {
            /* A "[section]" line */
            end = find_char_or_comment(start + 1, ']');

            if(*end == ']')
            {
                *end = '\0';
                section = start + 1;
                //#if defined(INI_ALLOW_MULTILINE)
                //                prev_name = nullptr;
                //#endif
                recentKeys = &m_params.iniData[section];
            }
            else if(!error)
            {
                /* No ']' found on section line */
                m_params.errorCode = ERR_SECTION_SYNTAX;
                error = lineno;
            }
        }
        break;

        default:
        {
            /* Not a comment, must be a name[=:]value pair */
            end = find_inieq_or_comment(start);

            if(IS_INIEQUAL(*end))
            {
                *end = '\0';
                name = rstrip(start);
                value = lskip(end + 1);
                end = find_char_or_comment(value, '\0');

                if(*end == ';')
                    *end = '\0';

                rstrip(value);
                {
                    char *v = value;
                    skipcomment(v);
                    v = rstrip(v);

                    if(!recentKeys)
                        recentKeys = &m_params.iniData["General"];

                    #ifdef INIDEBUG
                    printf("-> [%s]; %s = %s\n", section, name, v);
                    #endif
                    (*recentKeys)[name] = removeQuotes(v, v + strlen(v));
                }
            }
            else if(!error)
            {
                /* No '=' or ':' found on name[=:]value line */
                m_params.errorCode = ERR_KEY_SYNTAX;
                error = lineno;
            }

            break;
        }
        }//switch(*start)

        #if defined(INI_STOP_ON_FIRST_ERROR)

        if(error)
            break;

        #endif
    }

    m_params.lineWithError = error;
    return (error == 0);
}

/* See documentation in header file. */
bool IniProcessing::parseFile(const char *filename)
{
    bool valid = true;
    char *tmp = nullptr;
    #ifdef USE_FILE_MAPPER
    //By mystical reasons, reading whole file form fread() is faster than mapper :-P
    PGE_FileMapper file(filename);

    if(!file.data)
    {
        m_params.errorCode = ERR_NOFILE;
        return -1;
    }

    tmp = reinterpret_cast<char *>(malloc(static_cast<size_t>(file.size + 1)));

    if(!tmp)
    {
        m_params.errorCode = ERR_NO_MEMORY;
        return false;
    }

    memcpy(tmp, file.data, static_cast<size_t>(file.size));
    *(tmp + file.size) = '\0';//null terminate last line
    valid = ini_parse_file(tmp, static_cast<size_t>(file.size));
    #else
    #ifdef _WIN32
    //Convert UTF8 file path into UTF16 to support non-ASCII paths on Windows
    std::wstring dest;
    dest.resize(std::strlen(filename));
    int newSize = MultiByteToWideChar(CP_UTF8,
                                      0,
                                      filename,
                                      dest.size(),
                                      (wchar_t *)dest.c_str(),
                                      dest.size());
    dest.resize(newSize);
    FILE *cFile = _wfopen(dest.c_str(), L"rb");
    #else
    FILE *cFile = fopen(filename, "rb");
    #endif

    if(!cFile)
    {
        m_params.errorCode = ERR_NOFILE;
        return false;
    }

    fseek(cFile, 0, SEEK_END);
    ssize_t size = static_cast<ssize_t>(ftell(cFile));
    if(size < 0)
    {
        m_params.errorCode = ERR_KEY_SYNTAX;
        fclose(cFile);
        return false;
    }
    fseek(cFile, 0, SEEK_SET);
    tmp = reinterpret_cast<char *>(malloc(static_cast<size_t>(size + 1)));
    if(!tmp)
    {
        fclose(cFile);
        m_params.errorCode = ERR_NO_MEMORY;
        return false;
    }

    if(fread(tmp, 1, static_cast<size_t>(size), cFile) != static_cast<size_t>(size))
        valid = false;

    fclose(cFile);
    if(valid)
    {
        *(tmp + size) = '\0';//null terminate last line
        try
        {
            valid = parseHelper(tmp, size);
        }
        catch(...)
        {
            valid = false;
            m_params.errorCode = ERR_SECTION_SYNTAX;
        }
    }
    #endif

    free(tmp);
    return valid;
}

bool IniProcessing::parseMemory(char *mem, size_t size)
{
    bool valid = true;
    char *tmp = nullptr;
    tmp = reinterpret_cast<char *>(malloc(size + 1));

    if(!tmp)
    {
        m_params.errorCode = ERR_NO_MEMORY;
        return false;
    }

    memcpy(tmp, mem, static_cast<size_t>(size));
    *(tmp + size) = '\0';//null terminate last line
    valid = parseHelper(tmp, size);
    free(tmp);
    return valid;
}

IniProcessing::IniProcessing() :
    m_params{"", false, -1, ERR_OK, false, params::IniSections(), nullptr, ""}
{}

IniProcessing::IniProcessing(const std::string &iniFileName, int) :
    m_params{iniFileName, false, -1, ERR_OK, false, params::IniSections(), nullptr, ""}
{
    open(iniFileName);
}

IniProcessing::IniProcessing(char *memory, size_t size):
    m_params{"", false, -1, ERR_OK, false, params::IniSections(), nullptr, ""}
{
    openMem(memory, size);
}

IniProcessing::IniProcessing(const IniProcessing &ip) :
    m_params(ip.m_params)
{}

bool IniProcessing::open(const std::string &iniFileName)
{
    std::setlocale(LC_NUMERIC, "C");

    if(!iniFileName.empty())
    {
        close();
        m_params.errorCode = ERR_OK;
        m_params.filePath  = iniFileName;
        bool res = parseFile(m_params.filePath.c_str());
        #ifdef INIDEBUG

        if(res)
            printf("\n==========WOOHOO!!!==============\n\n");
        else
            printf("\n==========OOOUCH!!!==============\n\n");

        #endif
        m_params.opened = res;
        return res;
    }

    m_params.errorCode = ERR_NOFILE;
    return false;
}

bool IniProcessing::openMem(char *memory, size_t size)
{
    std::setlocale(LC_NUMERIC, "C");

    if((memory != nullptr) && (size > 0))
    {
        close();
        m_params.errorCode = ERR_OK;
        m_params.filePath.clear();
        bool res = parseMemory(memory, size);
        m_params.opened = res;
        return res;
    }

    m_params.errorCode = ERR_NOFILE;
    return false;
}

void IniProcessing::close()
{
    m_params.errorCode = ERR_OK;
    m_params.iniData.clear();
    m_params.opened = false;
    m_params.lineWithError = -1;
}

IniProcessing::ErrCode IniProcessing::lastError()
{
    return m_params.errorCode;
}

int IniProcessing::lineWithError()
{
    return m_params.lineWithError;
}

bool IniProcessing::isOpened()
{
    return m_params.opened;
}

bool IniProcessing::beginGroup(const std::string &groupName)
{
    if(!m_params.opened)
        return false;

    params::IniSections::iterator e = m_params.iniData.find(groupName);

    if(e == m_params.iniData.end())
        return false;

    params::IniKeys &k = e->second;
    m_params.currentGroup = &k;
    m_params.currentGroupName = groupName;
    return true;
}

bool IniProcessing::contains(const std::string &groupName)
{
    if(!m_params.opened)
        return false;

    params::IniSections::iterator e = m_params.iniData.find(groupName);
    return (e != m_params.iniData.end());
}

std::string IniProcessing::group()
{
    return m_params.currentGroupName;
}

bool IniProcessing::hasKey(const std::string &keyName)
{
    if(!m_params.opened)
        return false;

    if(!m_params.currentGroup)
        return false;

    params::IniKeys::iterator e = m_params.currentGroup->find(keyName);
    return (e != m_params.currentGroup->end());
}

void IniProcessing::endGroup()
{
    m_params.currentGroup = nullptr;
    m_params.currentGroupName.clear();
}

void IniProcessing::read(const char *key, bool &dest, bool defVal)
{
    bool ok = false;
    params::IniKeys::iterator e = readHelper(key, ok);

    if(!ok)
    {
        dest = defVal;
        return;
    }

    std::string &k = e->second;
    size_t i = 0;
    size_t ss = std::min(static_cast<size_t>(4ul), k.size());
    char buff[4] = {0, 0, 0, 0};
    const char *pbufi = k.c_str();
    char *pbuff = buff;

    for(; i < ss; i++)
        (*pbuff++) = static_cast<char>(std::tolower(*pbufi++));

    if(ss < 4)
    {
        if(ss == 0)
        {
            dest = false;
            return;
        }

        if(ss == 1)
        {
            dest = (buff[0] == '1');
            return;
        }

        try
        {
            long num = std::strtol(buff, 0, 0);
            dest = num != 0l;
            return;
        }
        catch(...)
        {
            dest = (std::memcmp(buff, "yes", 3) == 0) ||
                   (std::memcmp(buff, "on", 2) == 0);
            return;
        }
    }

    if(std::memcmp(buff, "true", 4) == 0)
    {
        dest = true;
        return;
    }

    try
    {
        long num = std::strtol(buff, 0, 0);
        dest = num != 0l;
        return;
    }
    catch(...)
    {
        dest = false;
        return;
    }
}

void IniProcessing::read(const char *key, unsigned char &dest, unsigned char defVal)
{
    bool ok = false;
    params::IniKeys::iterator e = readHelper(key, ok);

    if(!ok)
    {
        dest = defVal;
        return;
    }

    std::string &k = e->second;

    if(k.size() >= 1)
        dest = static_cast<unsigned char>(k[0]);
    else
        dest = defVal;
}

void IniProcessing::read(const char *key, char &dest, char defVal)
{
    bool ok = false;
    params::IniKeys::iterator e = readHelper(key, ok);

    if(!ok)
    {
        dest = defVal;
        return;
    }

    std::string &k = e->second;

    if(k.size() >= 1)
        dest = k[0];
    else
        dest = defVal;
}

void IniProcessing::read(const char *key, unsigned short &dest, unsigned short defVal)
{
    bool ok = false;
    params::IniKeys::iterator e = readHelper(key, ok);

    if(!ok)
    {
        dest = defVal;
        return;
    }

    dest = static_cast<unsigned short>(std::strtoul(e->second.c_str(), nullptr, 0));
}

void IniProcessing::read(const char *key, short &dest, short defVal)
{
    bool ok = false;
    params::IniKeys::iterator e = readHelper(key, ok);

    if(!ok)
    {
        dest = defVal;
        return;
    }

    dest = static_cast<short>(std::strtol(e->second.c_str(), nullptr, 0));
}

void IniProcessing::read(const char *key, unsigned int &dest, unsigned int defVal)
{
    bool ok = false;
    params::IniKeys::iterator e = readHelper(key, ok);

    if(!ok)
    {
        dest = defVal;
        return;
    }

    dest = static_cast<unsigned int>(std::strtoul(e->second.c_str(), nullptr, 0));
}

void IniProcessing::read(const char *key, int &dest, int defVal)
{
    bool ok = false;
    params::IniKeys::iterator e = readHelper(key, ok);

    if(!ok)
    {
        dest = defVal;
        return;
    }

    dest = static_cast<int>(std::strtol(e->second.c_str(), nullptr, 0));
}

void IniProcessing::read(const char *key, unsigned long &dest, unsigned long defVal)
{
    bool ok = false;
    params::IniKeys::iterator e = readHelper(key, ok);

    if(!ok)
    {
        dest = defVal;
        return;
    }

    dest = std::strtoul(e->second.c_str(), nullptr, 0);
}

void IniProcessing::read(const char *key, long &dest, long defVal)
{
    bool ok = false;
    params::IniKeys::iterator e = readHelper(key, ok);

    if(!ok)
    {
        dest = defVal;
        return;
    }

    dest = std::strtol(e->second.c_str(), nullptr, 0);
}

void IniProcessing::read(const char *key, unsigned long long &dest, unsigned long long defVal)
{
    bool ok = false;
    params::IniKeys::iterator e = readHelper(key, ok);

    if(!ok)
    {
        dest = defVal;
        return;
    }

    dest = std::strtoull(e->second.c_str(), nullptr, 0);
}

void IniProcessing::read(const char *key, long long &dest, long long defVal)
{
    bool ok = false;
    params::IniKeys::iterator e = readHelper(key, ok);

    if(!ok)
    {
        dest = defVal;
        return;
    }

    dest = std::strtoll(e->second.c_str(), nullptr, 0);
}

void IniProcessing::read(const char *key, float &dest, float defVal)
{
    bool ok = false;
    params::IniKeys::iterator e = readHelper(key, ok);

    if(!ok)
    {
        dest = defVal;
        return;
    }

    dest = std::strtof(e->second.c_str(), nullptr);
}

void IniProcessing::read(const char *key, double &dest, double defVal)
{
    bool ok = false;
    params::IniKeys::iterator e = readHelper(key, ok);

    if(!ok)
    {
        dest = defVal;
        return;
    }

    dest = std::strtod(e->second.c_str(), nullptr);
}

void IniProcessing::read(const char *key, std::string &dest, const std::string &defVal)
{
    bool ok = false;
    params::IniKeys::iterator e = readHelper(key, ok);

    if(!ok)
    {
        dest = defVal;
        return;
    }

    dest = e->second;
}

#ifdef INI_PROCESSING_ALLOW_QT_TYPES
void IniProcessing::read(const char *key, QString &dest, const QString &defVal)
{
    bool ok = false;
    params::IniKeys::iterator e = readHelper(key, ok);

    if(!ok)
    {
        dest = defVal;
        return;
    }

    dest = QString::fromStdString(e->second);
}
#endif

template<class TList>
inline void StrToNumVectorHelper(const std::string &source, TList &dest, const typename TList::value_type &def)
{
    typedef typename TList::value_type T;

    if(!source.empty())
    {
        std::stringstream ss(source);
        std::string item;
        while(std::getline(ss, item, ','))
        {
            try
            {
                if(std::is_same<T, int>::value ||
                   std::is_same<T, long>::value ||
                   std::is_same<T, short>::value)
                    dest.push_back(static_cast<T>(std::strtol(item.c_str(), NULL, 0)));
                else if(std::is_same<T, unsigned int>::value ||
                        std::is_same<T, unsigned long>::value ||
                        std::is_same<T, unsigned short>::value)
                    dest.push_back(static_cast<T>(std::strtoul(item.c_str(), NULL, 0)));
                else if(std::is_same<T, float>::value)
                    dest.push_back(std::strtof(item.c_str(), NULL));
                else
                    dest.push_back(std::strtod(item.c_str(), NULL));
            }
            catch(...)
            {
                dest.pop_back();
            }
        }

        if(dest.empty())
            dest.push_back(def);
    }
    else
        dest.push_back(def);
}

template<class TList, typename T>
void readNumArrHelper(IniProcessing *self, const char *key, TList &dest, const TList &defVal)
{
    bool ok = false;
    IniProcessing::params::IniKeys::iterator e = self->readHelper(key, ok);

    if(!ok)
    {
        dest = defVal;
        return;
    }

    StrToNumVectorHelper(e->second, dest, static_cast<T>(0));
}

void IniProcessing::read(const char *key, std::vector<unsigned short> &dest, const std::vector<unsigned short> &defVal)
{
    readNumArrHelper<std::vector<unsigned short>, unsigned short>(this, key, dest, defVal);
}
void IniProcessing::read(const char *key, std::vector<short> &dest, const std::vector<short> &defVal)
{
    readNumArrHelper<std::vector<short>, short>(this, key, dest, defVal);
}
void IniProcessing::read(const char *key, std::vector<unsigned int> &dest, const std::vector<unsigned int> &defVal)
{
    readNumArrHelper<std::vector<unsigned int>, unsigned int>(this, key, dest, defVal);
}
void IniProcessing::read(const char *key, std::vector<int> &dest, const std::vector<int> &defVal)
{
    readNumArrHelper<std::vector<int>, int>(this, key, dest, defVal);
}
void IniProcessing::read(const char *key, std::vector<unsigned long> &dest, const std::vector<unsigned long> &defVal)
{
    readNumArrHelper<std::vector<unsigned long>, unsigned long>(this, key, dest, defVal);
}
void IniProcessing::read(const char *key, std::vector<long> &dest, const std::vector<long> &defVal)
{
    readNumArrHelper<std::vector<long>, long>(this, key, dest, defVal);
}
void IniProcessing::read(const char *key, std::vector<unsigned long long> &dest, const std::vector<unsigned long long> &defVal)
{
    readNumArrHelper<std::vector<unsigned long long>, unsigned long long>(this, key, dest, defVal);
}
void IniProcessing::read(const char *key, std::vector<long long> &dest, const std::vector<long long> &defVal)
{
    readNumArrHelper<std::vector<long long>, long long>(this, key, dest, defVal);
}
void IniProcessing::read(const char *key, std::vector<float> &dest, const std::vector<float> &defVal)
{
    readNumArrHelper<std::vector<float>, float>(this, key, dest, defVal);
}
void IniProcessing::read(const char *key, std::vector<double> &dest, const std::vector<double> &defVal)
{
    readNumArrHelper<std::vector<double>, double>(this, key, dest, defVal);
}

#ifdef INI_PROCESSING_ALLOW_QT_TYPES
void IniProcessing::read(const char *key, QList<short> &dest, const QList<short> &defVal)
{
    readNumArrHelper<QList<short>, short>(this, key, dest, defVal);
}

void IniProcessing::read(const char *key, QList<unsigned short> &dest, const QList<unsigned short> &defVal)
{
    readNumArrHelper<QList<unsigned short>, unsigned short>(this, key, dest, defVal);
}

void IniProcessing::read(const char *key, QList<int> &dest, const QList<int> &defVal)
{
    readNumArrHelper<QList<int>, int>(this, key, dest, defVal);
}
void IniProcessing::read(const char *key, QList<unsigned int> &dest, const QList<unsigned int> &defVal)
{
    readNumArrHelper<QList<unsigned int>, unsigned int>(this, key, dest, defVal);
}
void IniProcessing::read(const char *key, QList<long> &dest, const QList<long> &defVal)
{
    readNumArrHelper<QList<long>, long>(this, key, dest, defVal);
}
void IniProcessing::read(const char *key, QList<unsigned long> &dest, const QList<unsigned long> &defVal)
{
    readNumArrHelper<QList<unsigned long>, unsigned long>(this, key, dest, defVal);
}
void IniProcessing::read(const char *key, QList<long long> &dest, const QList<long long> &defVal)
{
    readNumArrHelper<QList<long long>, long long>(this, key, dest, defVal);
}
void IniProcessing::read(const char *key, QList<unsigned long long> &dest, const QList<unsigned long long> &defVal)
{
    readNumArrHelper<QList<unsigned long long>, unsigned long long>(this, key, dest, defVal);
}
void IniProcessing::read(const char *key, QList<float> &dest, const QList<float> &defVal)
{
    readNumArrHelper<QList<float>, float>(this, key, dest, defVal);
}
void IniProcessing::read(const char *key, QList<double> &dest, const QList<double> &defVal)
{
    readNumArrHelper<QList<double>, double>(this, key, dest, defVal);
}

void IniProcessing::read(const char *key, QVector<short> &dest, const QVector<short> &defVal)
{
    readNumArrHelper<QVector<short>, short>(this, key, dest, defVal);
}
void IniProcessing::read(const char *key, QVector<unsigned short> &dest, const QVector<unsigned short> &defVal)
{
    readNumArrHelper<QVector<unsigned short>, unsigned short>(this, key, dest, defVal);
}
void IniProcessing::read(const char *key, QVector<int> &dest, const QVector<int> &defVal)
{
    readNumArrHelper<QVector<int>, int>(this, key, dest, defVal);
}
void IniProcessing::read(const char *key, QVector<unsigned int> &dest, const QVector<unsigned int> &defVal)
{
    readNumArrHelper<QVector<unsigned int>, unsigned int>(this, key, dest, defVal);
}
void IniProcessing::read(const char *key, QVector<long> &dest, const QVector<long> &defVal)
{
    readNumArrHelper<QVector<long>, long>(this, key, dest, defVal);
}
void IniProcessing::read(const char *key, QVector<unsigned long> &dest, const QVector<unsigned long> &defVal)
{
    readNumArrHelper<QVector<unsigned long>, unsigned long>(this, key, dest, defVal);
}
void IniProcessing::read(const char *key, QVector<long long> &dest, const QVector<long long> &defVal)
{
    readNumArrHelper<QVector<long long>, long long>(this, key, dest, defVal);
}
void IniProcessing::read(const char *key, QVector<unsigned long long> &dest, const QVector<unsigned long long> &defVal)
{
    readNumArrHelper<QVector<unsigned long long>, unsigned long long>(this, key, dest, defVal);
}
void IniProcessing::read(const char *key, QVector<float> &dest, const QVector<float> &defVal)
{
    readNumArrHelper<QVector<float>, float>(this, key, dest, defVal);
}
void IniProcessing::read(const char *key, QVector<double> &dest, const QVector<double> &defVal)
{
    readNumArrHelper<QVector<double>, double>(this, key, dest, defVal);
}
#endif


IniProcessingVariant IniProcessing::value(const char *key, const IniProcessingVariant &defVal)
{
    bool ok = false;
    params::IniKeys::iterator e = readHelper(key, ok);

    if(!ok)
        return defVal;

    std::string &k = e->second;
    return IniProcessingVariant(&k);
}
