/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** Copyright (C) 2016 Vitaliy Novichkov <admin@wohlnet.ru>
**
** This file use a part of the QtCore module of the Qt Toolkit,
** ported into pure STL to allow support of qm translations in the non-Qt projects.
**
** To use this code statically linked with the non-GPL projects
** you must have commercial license from the Qt rightholder
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Qt License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <stdio.h>
#include <string>
#include <vector>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#ifdef _WIN32
#include <stdio.h>
#include <windows.h>
#endif

#include "qm_translator.h"
#include "ConvertUTF.h"

typedef uint8_t     uchar;

//magic number for the file
static const int32_t MagicLength = 16;
static const uint8_t magic[MagicLength] =
{
    0x3c, 0xb8, 0x64, 0x18, 0xca, 0xef, 0x9c, 0x95,
    0xcd, 0x21, 0x1c, 0xbf, 0x60, 0xa1, 0xbd, 0xdd
};

/**
 *  \name The two types of endianness
 */
/* @{ */
#define MACHINE_LITTLE_ENDIAN   1234
#define MACHINE_BIG_ENDIAN      4321
/* @} */

#ifndef MACHINE_BYTEORDER
#if defined(__linux__) || defined(__HAIKU__)
#include <endian.h>
#define MACHINE_BYTEORDER  __BYTE_ORDER
#else /* __linux__ */
#if defined(__hppa__) || \
defined(__m68k__) || defined(mc68000) || defined(_M_M68K) || \
(defined(__MIPS__) && defined(__MISPEB__)) || \
defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC) || \
defined(__sparc__)
#define MACHINE_BYTEORDER   MACHINE_BIG_ENDIAN
#else
#define MACHINE_BYTEORDER   MACHINE_LITTLE_ENDIAN
#endif
#endif /* __linux__ */
#endif /* !SDL_BYTEORDER */

enum
{
    Q_EQ          = 0x01,
    Q_LT          = 0x02,
    Q_LEQ         = 0x03,
    Q_BETWEEN     = 0x04,

    Q_NOT         = 0x08,
    Q_MOD_10      = 0x10,
    Q_MOD_100     = 0x20,
    Q_LEAD_1000   = 0x40,

    Q_AND         = 0xFD,
    Q_OR          = 0xFE,
    Q_NEWRULE     = 0xFF,

    Q_OP_MASK     = 0x07,

    Q_NEQ         = Q_NOT | Q_EQ,
    Q_GT          = Q_NOT | Q_LEQ,
    Q_GEQ         = Q_NOT | Q_LT,
    Q_NOT_BETWEEN = Q_NOT | Q_BETWEEN
};

struct QTranslatorEntryTypes
{
    enum
    {
        Contexts     = 0x2f,
        Hashes       = 0x42,
        Messages     = 0x69,
        NumerusRules = 0x88,
        Dependencies = 0x96
    };
};

enum Tag
{
    Tag_End = 1,
    Tag_SourceText16,
    Tag_Translation,
    Tag_Context16,
    Tag_Obsolete1,
    Tag_SourceText,
    Tag_Context,
    Tag_Comment,
    Tag_Obsolete2
};


static uint8_t read8(const uint8_t *data)
{
    return data[0];
}

static uint16_t read16be(const uint8_t *data)
{
    return ((uint16_t(data[0]) << 8) & 0xFF00) |
           ((uint16_t(data[1]) << 0) & 0x00FF);
}

static uint32_t read32be(const uint8_t *data)
{
    return ((uint32_t(data[0]) << 24) & 0xFF000000) |
           ((uint32_t(data[1]) << 16) & 0x00FF0000) |
           ((uint32_t(data[2]) << 8)  & 0x0000FF00) |
           ((uint32_t(data[3]) << 0)  & 0x000000FF);
}


static void elfHash_continue(const char *name, uint32_t &h)
{
    const uchar *k;
    uint32_t g;

    k = reinterpret_cast<const uchar *>(name);
    while(*k)
    {
        h = (h << 4) + *k++;
        if((g = (h & 0xf0000000)) != 0)
            h ^= g >> 24;
        h &= ~g;
    }
}

static void elfHash_finish(uint32_t &h)
{
    if(!h)
        h = 1;
}

static uint32_t elfHash(const char *name)
{
    uint32_t hash = 0;
    elfHash_continue(name, hash);
    elfHash_finish(hash);
    return hash;
}


static bool match(const uchar *found, uint32_t foundLen, const char *target, uint32_t targetLen)
{
    // catch the case if \a found has a zero-terminating symbol and \a len includes it.
    // (normalize it to be without the zero-terminating symbol)
    if(foundLen > 0 && found[foundLen - 1] == '\0')
        --foundLen;
    return ((targetLen == foundLen) && (std::memcmp(found, target, foundLen) == 0));
}


/*
   \internal

   Determines whether \a rules are valid "numerus rules". Test input with this
   function before calling numerusHelper, below.
 */
static bool isValidNumerusRules(const uint8_t *rules, uint32_t rulesSize)
{
    // Disabled computation of maximum numerus return value
    // quint32 numerus = 0;

    if(rulesSize == 0)
        return true;

    uint32_t offset = 0;
    do
    {
        uint8_t opcode = rules[offset];
        uint8_t op = opcode & Q_OP_MASK;

        if(opcode & 0x80)
            return false; // Bad op

        if(++offset == rulesSize)
            return false; // Missing operand

        // right operand
        ++offset;

        switch(op)
        {
        case Q_EQ:
        case Q_LT:
        case Q_LEQ:
            break;

        case Q_BETWEEN:
            if(offset != rulesSize)
            {
                // third operand
                ++offset;
                break;
            }
            return false; // Missing operand

        default:
            return false; // Bad op (0)
        }

        // ++numerus;
        if(offset == rulesSize)
            return true;

    }
    while(((rules[offset] == Q_AND)
           || (rules[offset] == Q_OR)
           || (rules[offset] == Q_NEWRULE))
          && ++offset != rulesSize);

    // Bad op
    return false;
}


/*
   \internal

   This function does no validation of input and assumes it is well-behaved,
   these assumptions can be checked with isValidNumerusRules, above.

   Determines which translation to use based on the value of \a n. The return
   value is an index identifying the translation to be used.

   \a rules is a character array of size \a rulesSize containing bytecode that
   operates on the value of \a n and ultimately determines the result.

   This function has O(1) space and O(rulesSize) time complexity.
 */
static uint32_t numerusHelper(int32_t n, const uint8_t *rules, uint32_t rulesSize)
{
    uint32_t result = 0;
    uint32_t i = 0;

    if(rulesSize == 0)
        return 0;

    for(;;)
    {
        bool orExprTruthValue = false;

        for(;;)
        {
            bool andExprTruthValue = true;

            for(;;)
            {
                bool truthValue = true;
                int32_t opcode = rules[i++];

                int32_t leftOperand = n;
                if(opcode & Q_MOD_10)
                    leftOperand %= 10;
                else if(opcode & Q_MOD_100)
                    leftOperand %= 100;
                else if(opcode & Q_LEAD_1000)
                {
                    while(leftOperand >= 1000)
                        leftOperand /= 1000;
                }

                int32_t op = opcode & Q_OP_MASK;
                int32_t rightOperand = rules[i++];

                switch(op)
                {
                case Q_EQ:
                    truthValue = (leftOperand == rightOperand);
                    break;
                case Q_LT:
                    truthValue = (leftOperand < rightOperand);
                    break;
                case Q_LEQ:
                    truthValue = (leftOperand <= rightOperand);
                    break;
                case Q_BETWEEN:
                    int32_t bottom = rightOperand;
                    int32_t top = rules[i++];
                    truthValue = (leftOperand >= bottom && leftOperand <= top);
                }

                if(opcode & Q_NOT)
                    truthValue = !truthValue;

                andExprTruthValue = andExprTruthValue && truthValue;

                if(i == rulesSize || rules[i] != Q_AND)
                    break;
                ++i;
            }

            orExprTruthValue = orExprTruthValue || andExprTruthValue;

            if(i == rulesSize || rules[i] != Q_OR)
                break;
            ++i;
        }

        if(orExprTruthValue)
            return result;

        ++result;

        if(i == rulesSize)
            return result;

        i++; // Q_NEWRULE
    }

    //Q_ASSERT(false);
    assert(false);
    return 0;
}

static std::u16string getMessage(const uint8_t *m, const uint8_t *end, const char *context,
                                 const char *sourceText, const char *comment, uint32_t numerus)
{
    #ifdef QMTRANSLATPR_DEEP_DEBUG
    printf("-----> Try take message...!\n");
    #endif

    const uchar *tn = 0;
    uint32_t tn_length = 0;
    const uint32_t sourceTextLen = uint32_t(std::strlen(sourceText));
    const uint32_t contextLen = uint32_t(std::strlen(context));
    const uint32_t commentLen = uint32_t(std::strlen(comment));

    for(;;)
    {
        uint8_t tag = 0;
        if(m < end)
            tag = read8(m++);
        switch(tag)
        {
        case Tag_End:
            goto end;
        case Tag_Translation:
        {
            if(m >= (end - 4))
                return std::u16string(u"<qm-error 0>");
            int32_t len = static_cast<int32_t>(read32be(m));
            if(len % 2) //In the Qt here was a bug: byte lenght must be multiple two, but was %1
                return std::u16string(u"<qm-error 1>");
            m += 4;
            if(!numerus--)
            {
                tn_length = static_cast<uint32_t>(len);
                tn = m;
            }
            m += len;
            break;
        }
        case Tag_Obsolete1:
            if(m >= (end - 4))
                return std::u16string(u"<qm-error 2>");
            m += 4;
            break;
        case Tag_SourceText:
        {
            if(m >= (end - 4))
                return std::u16string(u"<qm-error 3>");
            uint32_t len = read32be(m);
            m += 4;
            if((m + len) >= end)
                return std::u16string(u"<qm-error 4>");
            if(!match(m, len, sourceText, sourceTextLen))
            {
                #ifdef QMTRANSLATPR_DEEP_DEBUG
                printf("-----> Source text doesn't match!\n");
                #endif
                return std::u16string();
            }
            m += len;
        }
        break;
        case Tag_Context:
        {
            if(m >= (end - 4))
                return std::u16string(u"<qm-error 5>");
            uint32_t len = read32be(m);
            m += 4;
            if((m + len) >= end)
                return std::u16string(u"<qm-error 6>");
            if(!match(m, len, context, contextLen))
            {
                #ifdef QMTRANSLATPR_DEEP_DEBUG
                printf("-----> Tag gontext doesn't match!\n");
                #endif
                return std::u16string();
            }
            m += len;
        }
        break;
        case Tag_Comment:
        {
            if(m >= (end - 4))
                return std::u16string(u"<qm-error 6>");
            uint32_t len = read32be(m);
            m += 4;
            if((m + len) >= end)
                return std::u16string(u"<qm-error 7>");
            if(*m && !match(m, len, comment, commentLen))
                return std::u16string();
            m += len;
        }
        break;
        default:
            #ifdef QMTRANSLATPR_DEEP_DEBUG
            printf("-----> Wrong tag!\n");
            #endif
            return std::u16string();
        }
    }
end:
    if(!tn)
    {
        #ifdef QMTRANSLATPR_DEEP_DEBUG
        printf("-----> Empty TN!\n");
        #endif
        return std::u16string();
    }
    #ifdef QMTRANSLATPR_DEEP_DEBUG
    printf("-----> Almost got...!\n");
    #endif

    UTF16   *utf16str = reinterpret_cast<UTF16 *>(const_cast<uchar*>(tn));
    size_t  utf16str_len = tn_length / 2;

    #if MACHINE_BYTEORDER == MACHINE_LITTLE_ENDIAN
    std::u16string outStr(reinterpret_cast<char16_t *>(utf16str), utf16str_len);
    char16_t *ustr = &outStr[0];
    for(uint32_t i = 0; i < utf16str_len; i++)
        ustr[i] = ((ustr[i] >> 8) & 0x00FF) + ((ustr[i] << 8) & 0xFF00);
    return outStr;
    #else
    return std::u16string((char16_t *)utf16str, utf16str_len);
    #endif
}


QmTranslatorX::QmTranslatorX() :
    m_fileData(nullptr), m_fileLength(0),
    m_messageArray(nullptr), m_offsetArray(nullptr), m_contextArray(nullptr), m_numerusRulesArray(nullptr),
    m_messageLength(0),      m_offsetLength(0),      m_contextLength(0),      m_numerusRulesLength(0)
{}

QmTranslatorX::~QmTranslatorX()
{
    close();
}

std::u16string QmTranslatorX::do_translate(const char *context, const char *sourceText, const char *comment, int32_t n)
{
    if(context == 0)
        context = "";
    if(sourceText == 0)
        sourceText = "";
    if(comment == 0)
        comment = "";

    uint32_t numerus = 0;
    size_t numItems = 0;

    if(!m_offsetLength)
    {
        #ifdef QMTRANSLATPR_DEEP_DEBUG
        printf("--> ZERO OFFSETS LENGTH!");
        #endif
        goto searchDependencies;
    }

    /*
        Check if the context belongs to this QTranslator. If many
        translators are installed, this step is necessary.
    */
    if(m_contextLength)
    {
        #ifdef QMTRANSLATPR_DEEP_DEBUG
        printf("--> Finding contexts...!");
        #endif
        uint16_t hTableSize = read16be(m_contextArray);
        uint32_t g = elfHash(context) % hTableSize;
        const uint8_t *c = m_contextArray + 2 + (g << 1);
        uint16_t off = read16be(c);
        c += 2;
        if(off == 0)
        {
            #ifdef QMTRANSLATPR_DEEP_DEBUG
            printf("--> Zero offset...!\n");
            #endif
            return std::u16string();
        }
        c = m_contextArray + (2 + (hTableSize << 1) + (off << 1));

        const uint32_t contextLen = uint32_t(std::strlen(context));
        for(;;)
        {
            uint8_t len = read8(c++);
            if(len == 0)
            {
                #ifdef QMTRANSLATPR_DEEP_DEBUG
                printf("--> Zero length...!\n");
                #endif
                return std::u16string();
            }
            if(match(c, len, context, contextLen))
                break;
            c += len;
        }
    }
    else
    {
        #ifdef QMTRANSLATPR_DEEP_DEBUG
        printf("--> Contexts are empty!\n");
        #endif
    }

    numItems = m_offsetLength / (2 * sizeof(unsigned));
    if(!numItems)
    {
        #ifdef QMTRANSLATPR_DEEP_DEBUG
        printf("--> NO ITEMS!\n");
        #endif
        goto searchDependencies;
    }

    if(n >= 0)
        numerus = numerusHelper(n, m_numerusRulesArray, m_numerusRulesLength);

    for(;;)
    {
        uint32_t h = 0;
        elfHash_continue(sourceText, h);
        elfHash_continue(comment, h);
        elfHash_finish(h);

        const uint8_t *start = m_offsetArray;
        const uint8_t *end = start + ((numItems - 1) << 3);
        while(start <= end)
        {
            const uint8_t *middle = start + (((end - start) >> 4) << 3);
            uint32_t hash = read32be(middle);
            if(h == hash)
            {
                start = middle;
                break;
            }
            else if(hash < h)
                start = middle + 8;
            else
                end = middle - 8;
        }

        if(start <= end)
        {
            // go back on equal key
            while(start != m_offsetArray && read32be(start) == read32be(start - 8))
                start -= 8;

            while(start < m_offsetArray + m_offsetLength)
            {
                uint32_t rh = read32be(start);
                start += 4;
                if(rh != h)
                    break;
                uint32_t ro = read32be(start);
                start += 4;
                std::u16string tn = getMessage(m_messageArray + ro, m_messageArray + m_messageLength, context,
                                               sourceText, comment, numerus);
                if(!tn.empty())
                    return tn;
            }
        }
        if(!comment[0])
            break;
        comment = "";
    }

    #ifdef QMTRANSLATPR_DEEP_DEBUG
    printf("--> Nothing found!\n");
    #endif

searchDependencies:
    for(QmTranslatorX *translator : m_subTranslators)
    {
        std::u16string tn = translator->do_translate(context, sourceText, comment, n);
        if(!tn.empty())
            return tn;
    }
    return std::u16string();
}

std::string QmTranslatorX::do_translate8(const char *context, const char *sourceText, const char *comment, int32_t n)
{
    std::u16string str  = do_translate(context, sourceText, comment, n);
    size_t utf16len    = str.size();
    size_t utf8bytelen = str.size() * sizeof(char16_t) + 1;
    char *utf8str = reinterpret_cast<char *>(std::malloc(utf8bytelen));
    std::memset(utf8str, 0, utf8bytelen);
    if(utf16len > 0)
    {
        const UTF16 *pUtf16 = reinterpret_cast<const UTF16 *>(str.data());
        UTF8  *pUtf8  = reinterpret_cast<UTF8 *>(utf8str);
        ConvertUTF16toUTF8(&pUtf16, pUtf16 + utf16len,
                           &pUtf8,  pUtf8 + utf8bytelen, lenientConversion);
    }
    std::string outstr(utf8str);
    std::free(utf8str);
    return outstr;
}

std::u32string QmTranslatorX::do_translate32(const char *context, const char *sourceText, const char *comment, int32_t n)
{
    std::u16string str = do_translate(context, sourceText, comment, n);
    size_t utf16len = str.size();
    size_t utf32len = str.size() + 1;
    size_t utf32bytelen = utf32len * sizeof(char32_t);
    char32_t *utf32str = reinterpret_cast<char32_t *>(std::malloc(utf32bytelen));
    std::memset(utf32str, 0, utf32bytelen);
    if(utf16len > 0)
    {
        const UTF16 *pUtf16 =  reinterpret_cast<const UTF16 *>(str.data());
        UTF32 *pUtf32  = reinterpret_cast<UTF32 *>(utf32str);
        ConvertUTF16toUTF32(&pUtf16, pUtf16 + utf16len,
                            &pUtf32, pUtf32 + utf32len, lenientConversion);
    }
    std::u32string outstr(utf32str);
    std::free(utf32str);
    return outstr;
}

bool QmTranslatorX::loadFile(const char *filePath, uint8_t *directory)
{
    uint8_t magicBuffer[MagicLength];
    size_t  fileGotLen = 0;

    #ifndef _WIN32
    FILE *file = std::fopen(filePath, "rb");
    #else
    wchar_t filePathW[MAX_PATH + 1];
    {
        size_t utf8len  = std::strlen(filePath);
        size_t utf16len = MAX_PATH;
        utf16len = MultiByteToWideChar(CP_UTF8, 0,
                                       filePath,  utf8len,
                                       filePathW, MAX_PATH);
        filePathW[utf16len] = L'\0';
    }
    FILE *file = _wfopen(filePathW, L"rb");
    #endif
    if(!file)
        return false;//err("Can't open file!", 2);

    if(std::fread(magicBuffer, 1, MagicLength, file) < MagicLength)
    {
        std::fclose(file);
        return false;//err("ERROR READING MAGIC NUMBER!!!", 3);
    }

    if(std::memcmp(magicBuffer, magic, MagicLength))
    {
        std::fclose(file);
        return false;//err("MAGIC NUMBER DOESN'T CASE!", 4);
    }

    std::fseek(file, 0L, SEEK_END);

    long fileLenth = std::ftell(file);
    if(fileLenth < 0)
    {
        std::fclose(file);
        return false;//err("FAIL TO SEEK END!", 4);
    }

    m_fileLength = static_cast<size_t>(fileLenth);
    std::fseek(file, 0L, SEEK_SET);

    m_fileData = reinterpret_cast<uint8_t *>(std::malloc(m_fileLength));
    if(!m_fileData)
    {
        std::fclose(file);
        return false;//err("OUT OF MEMORY!", 5);
    }
    fileGotLen = std::fread(m_fileData, 1, m_fileLength, file);
    std::fclose(file);
    m_fileLength = fileGotLen;
    return loadData(m_fileData, m_fileLength, directory);
}

bool QmTranslatorX::loadData(uint8_t *data, size_t len, uint8_t *directory)
{
    std::vector<std::string> dependencies;
    bool ok = true;
    const uint8_t *end = data + len;

    data += MagicLength;
    while(data < end - 4)
    {
        uint8_t  tag = read8(data++);
        uint32_t blockLen = read32be(data);
        data += 4;
        if(!tag || !blockLen)
            break;
        if(uint32_t(end - data) < blockLen)
        {
            ok = false;
            break;
        }

        if(tag == QTranslatorEntryTypes::Contexts)
        {
            m_contextArray = data;
            m_contextLength = blockLen;
            #ifdef QMTRANSLATPR_DEEP_DEBUG
            printf("Has contexts array!\n");
            #endif
        }
        else if(tag == QTranslatorEntryTypes::Hashes)
        {
            m_offsetArray = data;
            m_offsetLength = blockLen;
            #ifdef QMTRANSLATPR_DEEP_DEBUG
            printf("Has hashes! %i\n", offsetLength);
            #endif
        }
        else if(tag == QTranslatorEntryTypes::Messages)
        {
            m_messageArray = data;
            m_messageLength = blockLen;
            #ifdef QMTRANSLATPR_DEEP_DEBUG
            printf("Has messages! %i\n", messageLength);
            #endif
        }
        else if(tag == QTranslatorEntryTypes::NumerusRules)
        {
            m_numerusRulesArray = data;
            m_numerusRulesLength = blockLen;
            #ifdef QMTRANSLATPR_DEEP_DEBUG
            printf("Has numerus rules! %i\n", numerusRulesLength);
            #endif
        }
        else if(tag == QTranslatorEntryTypes::Dependencies)
        {
            std::string dep;
            uint8_t *end   = data + blockLen;
            while(blockLen != 0)
            {
                uint8_t *begin = data;
                while((data != end) && (*data != '\0'))
                    data++;

                std::string::size_type gotLen = std::string::size_type(data - begin);

                //Avoid infinite loop if got len is zero or larger than left bytes block
                if((gotLen == 0) || (gotLen > blockLen))
                {
                    #ifdef QMTRANSLATPR_DEEP_DEBUG
                    printf("Dependencies tag loop: INFINITE LOOP DETECTED!\n");
                    #endif
                    break;
                }

                dep = std::string(reinterpret_cast<char *>(begin), gotLen);
                if(dep.size() > 0)
                {
                    //List of dependent files
                    dependencies.push_back(dep);
                    #ifdef QMTRANSLATPR_DEEP_DEBUG
                    printf("Dependency: %s\n", dep.c_str());
                    #endif
                }
                blockLen -= gotLen;
            }
            #ifdef QMTRANSLATPR_DEEP_DEBUG
            printf("Had deps!\n");
            #endif
        }
        data += blockLen;
    }

    if(dependencies.empty() && (!m_offsetArray || !m_messageArray))
        ok = false;

    #ifdef QMTRANSLATPR_DEEP_DEBUG
    printf("Dependencies valid: %i\n", ok);
    #endif

    if(ok && !isValidNumerusRules(m_numerusRulesArray, m_numerusRulesLength))
        ok = false;

    #ifdef QMTRANSLATPR_DEEP_DEBUG
    printf("Numerus rules valid: %i\n", ok);
    #endif

    //Process loading of sub-translators
    if(ok)
    {
        const size_t dependenciesCount = dependencies.size();
        m_subTranslators.reserve(dependenciesCount);
        for(size_t i = 0 ; i < dependenciesCount; ++i)
        {
            QmTranslatorX *translator = new QmTranslatorX;
            m_subTranslators.push_back(translator);
            ok = translator->loadFile(dependencies[i].c_str(), directory);
            if(!ok)
                break;
        }

        // In case some dependencies fail to load, unload all the other ones too.
        if(!ok)
        {
            for(QmTranslatorX *it : m_subTranslators)
                delete it;
            m_subTranslators.clear();
        }
    }

    if(!ok)
    {
        m_messageArray    = 0;
        m_contextArray    = 0;
        m_offsetArray     = 0;
        m_numerusRulesArray = 0;
        m_messageLength   = 0;
        m_contextLength   = 0;
        m_offsetLength    = 0;
        m_numerusRulesLength = 0;
        #ifdef QMTRANSLATPR_DEEP_DEBUG
        printf("LOADING FAILED!\n");
        #endif
        return false;
    }
    else
    {
        #ifdef QMTRANSLATPR_DEEP_DEBUG
        printf("LOADING PASSED!\n");
        #endif
        return true;
    }
}

bool QmTranslatorX::isEmpty()
{
    return !m_fileData && !m_fileLength && !m_messageArray &&
           !m_offsetArray && !m_contextArray && m_subTranslators.empty();
}

void QmTranslatorX::close()
{
    m_messageArray = 0;
    m_contextArray = 0;
    m_offsetArray = 0;
    m_numerusRulesArray = 0;
    m_messageLength = 0;
    m_contextLength = 0;
    m_offsetLength = 0;
    m_numerusRulesLength = 0;
    if(m_fileData) std::free(m_fileData);
    m_fileData = nullptr;
    m_fileLength = 0;
    for(QmTranslatorX *it : m_subTranslators)
        delete it;
    m_subTranslators.clear();
}
