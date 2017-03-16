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
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <vector>
#include <assert.h>
#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "qm_translator.h"
#include "ConvertUTF.h"

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;

typedef signed char sint8;     /* 8 bit signed */
typedef unsigned char uint8;   /* 8 bit unsigned */
typedef short sint16;          /* 16 bit signed */
typedef unsigned short uint16; /* 16 bit unsigned */
typedef int sint32;            /* 32 bit signed */
typedef unsigned int uint32;   /* 32 bit unsigned */

//magic number for the file
static const int MagicLength = 16;
static const unsigned char magic[MagicLength] =
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
#ifdef __linux__
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
    enum { Contexts = 0x2f, Hashes = 0x42, Messages = 0x69, NumerusRules = 0x88, Dependencies = 0x96 };
};

enum Tag { Tag_End = 1, Tag_SourceText16, Tag_Translation, Tag_Context16, Tag_Obsolete1,
           Tag_SourceText, Tag_Context, Tag_Comment, Tag_Obsolete2 };


static unsigned char read8(const unsigned char *data)
{
    return data[0];
}

static unsigned short read16be(const unsigned char *data)
{
    return  ((((unsigned short)data[0])<<8) &0xFF00)  |
            ((((unsigned short)data[1])<<0) &0x00FF);//qFromBigEndian<quint16>(data);
}

static unsigned int read32be(const unsigned char *data)
{
    return   ((((unsigned int)data[0])<<24)&0xFF000000)  |
             ((((unsigned int)data[1])<<16)&0x00FF0000)  |
             ((((unsigned int)data[2])<<8) &0x0000FF00)  |
             ((((unsigned int)data[3])<<0) &0x000000FF);//qFromBigEndian<quint32>(data);
}


static void elfHash_continue(const char *name, uint &h)
{
    const uchar *k;
    uint g;

    k = (const uchar *) name;
    while (*k) {
        h = (h << 4) + *k++;
        if ((g = (h & 0xf0000000)) != 0)
            h ^= g >> 24;
        h &= ~g;
    }
}

static void elfHash_finish(uint &h)
{
    if (!h)
        h = 1;
}

static uint elfHash(const char *name)
{
    uint hash = 0;
    elfHash_continue(name, hash);
    elfHash_finish(hash);
    return hash;
}


static bool match(const uchar *found, uint foundLen, const char *target, uint targetLen)
{
    // catch the case if \a found has a zero-terminating symbol and \a len includes it.
    // (normalize it to be without the zero-terminating symbol)
    if (foundLen > 0 && found[foundLen-1] == '\0')
        --foundLen;
    return ((targetLen == foundLen) && memcmp(found, target, foundLen) == 0);
}


/*
   \internal

   Determines whether \a rules are valid "numerus rules". Test input with this
   function before calling numerusHelper, below.
 */
static bool isValidNumerusRules(const unsigned char *rules, unsigned int rulesSize)
{
    // Disabled computation of maximum numerus return value
    // quint32 numerus = 0;

    if (rulesSize == 0)
        return true;

    unsigned int offset = 0;
    do {
        unsigned char opcode = rules[offset];
        unsigned char op = opcode & Q_OP_MASK;

        if (opcode & 0x80)
            return false; // Bad op

        if (++offset == rulesSize)
            return false; // Missing operand

        // right operand
        ++offset;

        switch (op)
        {
        case Q_EQ:
        case Q_LT:
        case Q_LEQ:
            break;

        case Q_BETWEEN:
            if (offset != rulesSize) {
                // third operand
                ++offset;
                break;
            }
            return false; // Missing operand

        default:
            return false; // Bad op (0)
        }

        // ++numerus;
        if (offset == rulesSize)
            return true;

    } while (((rules[offset] == Q_AND)
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
static unsigned int numerusHelper(int n, const unsigned char *rules, unsigned int rulesSize)
{
    unsigned int result = 0;
    unsigned int i = 0;

    if (rulesSize == 0)
        return 0;

    for (;;) {
        bool orExprTruthValue = false;

        for (;;) {
            bool andExprTruthValue = true;

            for (;;) {
                bool truthValue = true;
                int opcode = rules[i++];

                int leftOperand = n;
                if (opcode & Q_MOD_10) {
                    leftOperand %= 10;
                } else if (opcode & Q_MOD_100) {
                    leftOperand %= 100;
                } else if (opcode & Q_LEAD_1000) {
                    while (leftOperand >= 1000)
                        leftOperand /= 1000;
                }

                int op = opcode & Q_OP_MASK;
                int rightOperand = rules[i++];

                switch (op) {
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
                    int bottom = rightOperand;
                    int top = rules[i++];
                    truthValue = (leftOperand >= bottom && leftOperand <= top);
                }

                if (opcode & Q_NOT)
                    truthValue = !truthValue;

                andExprTruthValue = andExprTruthValue && truthValue;

                if (i == rulesSize || rules[i] != Q_AND)
                    break;
                ++i;
            }

            orExprTruthValue = orExprTruthValue || andExprTruthValue;

            if (i == rulesSize || rules[i] != Q_OR)
                break;
            ++i;
        }

        if (orExprTruthValue)
            return result;

        ++result;

        if (i == rulesSize)
            return result;

        i++; // Q_NEWRULE
    }

    //Q_ASSERT(false);
    assert(false);
    return 0;
}

static std::u16string getMessage(const unsigned char *m, const unsigned char *end, const char *context,
                               const char *sourceText, const char *comment, unsigned int numerus)
{
#ifdef QMTRANSLATPR_DEEP_DEBUG
    printf("-----> Try take message...!\n");
#endif

    const uchar *tn = 0;
    uint tn_length = 0;
    const uint sourceTextLen = uint(strlen(sourceText));
    const uint contextLen = uint(strlen(context));
    const uint commentLen = uint(strlen(comment));

    for (;;) {
        unsigned char tag = 0;
        if (m < end)
            tag = read8(m++);
        switch((Tag)tag)
        {
        case Tag_End:
            goto end;
        case Tag_Translation: {
            int len = read32be(m);
            if (len % 1)
                return std::u16string();
            m += 4;
            if (!numerus--) {
                tn_length = len;
                tn = m;
            }
            m += len;
            break;
        }
        case Tag_Obsolete1:
            m += 4;
            break;
        case Tag_SourceText: {
            uint32 len = read32be(m);
            m += 4;
            if (!match(m, len, sourceText, sourceTextLen))
            {
                #ifdef QMTRANSLATPR_DEEP_DEBUG
                printf("-----> Source text doesn't match!\n");
                #endif
                return std::u16string();
            }
            m += len;
        }
            break;
        case Tag_Context: {
            uint32 len = read32be(m);
            m += 4;
            if (!match(m, len, context, contextLen))
            {
                #ifdef QMTRANSLATPR_DEEP_DEBUG
                printf("-----> Tag gontext doesn't match!\n");
                #endif
                return std::u16string();
            }
            m += len;
        }
            break;
        case Tag_Comment: {
            uint32 len = read32be(m);
            m += 4;
            if (*m && !match(m, len, comment, commentLen))
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
    if (!tn)
    {
        #ifdef QMTRANSLATPR_DEEP_DEBUG
        printf("-----> Empty TN!\n");
        #endif
        return std::u16string();
    }
    #ifdef QMTRANSLATPR_DEEP_DEBUG
    printf("-----> Almost got...!\n");
    #endif

    UTF16*   utf16str = (UTF16*)tn;
    intptr_t utf16str_len = tn_length/2;

#if MACHINE_BYTEORDER == MACHINE_LITTLE_ENDIAN
    std::u16string outStr((char16_t*)utf16str, utf16str_len);
    char16_t*ustr = (char16_t*)outStr.data();
    for(int i=0; i<utf16str_len;i++)
        ustr[i] = ((ustr[i]>>8)&0x00FF) + ((ustr[i]<<8)&0xFF00);
    return outStr;
#else
    return std::u16string((char16_t*)utf16str, utf16str_len);
#endif
}


QmTranslatorX::QmTranslatorX() :
    FileData(nullptr), FileLength(0),
    messageArray(nullptr), offsetArray(nullptr), contextArray(nullptr), numerusRulesArray(nullptr),
    messageLength(0),      offsetLength(0),      contextLength(0),      numerusRulesLength(0)
{}

QmTranslatorX::~QmTranslatorX()
{
    close();
}

std::u16string QmTranslatorX::do_translate(const char *context, const char *sourceText, const char *comment, int n)
{
    if (context == 0)
        context = "";
    if (sourceText == 0)
        sourceText = "";
    if (comment == 0)
        comment = "";

    unsigned int numerus = 0;
    size_t numItems = 0;

    if (!offsetLength)
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
    if (contextLength) {
        #ifdef QMTRANSLATPR_DEEP_DEBUG
        printf("--> Finding contexts...!");
        #endif
        unsigned short hTableSize = read16be(contextArray);
        unsigned int g = elfHash(context) % hTableSize;
        const unsigned char *c = contextArray + 2 + (g << 1);
        unsigned short off = read16be(c);
        c += 2;
        if (off == 0)
        {
            #ifdef QMTRANSLATPR_DEEP_DEBUG
            printf("--> Zero offset...!\n");
            #endif
            return std::u16string();
        }
        c = contextArray + (2 + (hTableSize << 1) + (off << 1));

        const uint contextLen = uint(strlen(context));
        for (;;) {
            unsigned char len = read8(c++);
            if (len == 0)
            {
                #ifdef QMTRANSLATPR_DEEP_DEBUG
                printf("--> Zero length...!\n");
                #endif
                return std::u16string();
            }
            if (match(c, len, context, contextLen))
                break;
            c += len;
        }
    } else {
        #ifdef QMTRANSLATPR_DEEP_DEBUG
        printf("--> Contexts are empty!\n");
        #endif
    }

    numItems = offsetLength / (2 * sizeof(unsigned));
    if (!numItems)
    {
        #ifdef QMTRANSLATPR_DEEP_DEBUG
        printf("--> NO ITEMS!\n");
        #endif
        goto searchDependencies;
    }

    if (n >= 0)
        numerus = numerusHelper(n, numerusRulesArray, numerusRulesLength);

    for (;;) {
        unsigned int h = 0;
        elfHash_continue(sourceText, h);
        elfHash_continue(comment, h);
        elfHash_finish(h);

        const unsigned char *start = offsetArray;
        const unsigned char *end = start + ((numItems-1) << 3);
        while (start <= end) {
            const unsigned char *middle = start + (((end - start) >> 4) << 3);
            unsigned int hash = read32be(middle);
            if (h == hash) {
                start = middle;
                break;
            } else if (hash < h) {
                start = middle + 8;
            } else {
                end = middle - 8;
            }
        }

        if (start <= end) {
            // go back on equal key
            while (start != offsetArray && read32be(start) == read32be(start-8))
                start -= 8;

            while (start < offsetArray + offsetLength) {
                uint32 rh = read32be(start);
                start += 4;
                if (rh != h)
                    break;
                uint32 ro = read32be(start);
                start += 4;
                std::u16string tn = getMessage(messageArray + ro, messageArray + messageLength, context,
                                               sourceText, comment, numerus);
                if(!tn.empty())
                    return tn;
            }
        }
        if (!comment[0])
            break;
        comment = "";
    }

    #ifdef QMTRANSLATPR_DEEP_DEBUG
    printf("--> Nothing found!\n");
    #endif

searchDependencies:
    for (QmTranslatorX *translator : subTranslators) {
        std::u16string tn = translator->do_translate(context, sourceText, comment, n);
        if (!tn.empty())
            return tn;
    }
    return std::u16string();
}

std::string QmTranslatorX::do_translate8(const char *context, const char *sourceText, const char *comment, int n)
{
    std::u16string str = do_translate(context, sourceText, comment, n);
    int utf16len = str.size();
    int utf8bytelen  = str.size()*sizeof(char16_t)+1;
    char* utf8str = (char*)malloc(utf8bytelen);
    memset(utf8str, 0, utf8bytelen);
    if(utf16len>0)
    {
        const UTF16 *pUtf16 = (const UTF16*)str.data();
              UTF8  *pUtf8  = (UTF8*)utf8str;
        ConvertUTF16toUTF8( &pUtf16, pUtf16+utf16len,
                            &pUtf8,  pUtf8+utf8bytelen, lenientConversion );
    }
    std::string outstr(utf8str);
    free(utf8str);
    return outstr;
}

std::u32string QmTranslatorX::do_translate32(const char *context, const char *sourceText, const char *comment, int n)
{
    std::u16string str = do_translate(context, sourceText, comment, n);
    int utf16len = str.size();
    int utf32len = str.size()+1;
    int utf32bytelen = utf32len*sizeof(char32_t);
    char32_t* utf32str = (char32_t*)malloc(utf32bytelen);
    memset(utf32str, 0, utf32bytelen);
    if(utf16len>0)
    {
        const UTF16 *pUtf16 = (const UTF16*)str.data();
              UTF32 *pUtf32  = (UTF32*)utf32str;
        ConvertUTF16toUTF32( &pUtf16, pUtf16+utf16len,
                             &pUtf32, pUtf32+utf32len, lenientConversion );
    }
    std::u32string outstr(utf32str);
    free(utf32str);
    return outstr;
}

bool QmTranslatorX::loadFile(const char *filePath, unsigned char *directory)
{
    unsigned char magicBuffer[MagicLength];
    size_t        fileGotLen = 0;

    #ifndef _WIN32
    FILE* file = fopen(filePath, "rb");
    #else
    wchar_t filePathW[MAX_PATH + 1];
    {
        int utf8len  = strlen(filePath);
        int utf16len = MAX_PATH;
        utf16len = MultiByteToWideChar(CP_UTF8, 0,
                                       filePath,  utf8len,
                                       filePathW, MAX_PATH);
        filePathW[utf16len] = L'\0';
    }
    FILE* file = _wfopen(filePathW, L"rb");
    #endif
    if(!file)
        return false;//err("Can't open file!", 2);

    if( fread(magicBuffer, 1, MagicLength, file) < MagicLength )
    {
        fclose(file);
        return false;//err("ERROR READING MAGIC NUMBER!!!", 3);
    }

    if( memcmp(magicBuffer, magic, MagicLength) )
    {
        fclose(file);
        return false;//err("MAGIC NUMBER DOESN'T CASE!", 4);
    }

    fseek(file, 0L, SEEK_END);
    FileLength = ftell(file);
    fseek(file, 0L, SEEK_SET);

    FileData = (unsigned char*)malloc(FileLength);
    fileGotLen = fread(FileData, 1, FileLength, file);
    fclose(file);
    FileLength = fileGotLen;
    return loadData(FileData, FileLength, directory);
}

bool QmTranslatorX::loadData(unsigned char *data, int len, unsigned char* directory)
{
    std::vector<std::string> dependencies;
    bool ok = true;
    const unsigned char *end = data + len;

    data += MagicLength;
    while (data < end - 4)
    {
        unsigned char tag = read8(data++);
        unsigned int blockLen = read32be(data);
        data += 4;
        if (!tag || !blockLen)
            break;
        if ((unsigned int)(end - data) < blockLen) {
            ok = false;
            break;
        }

        if (tag == QTranslatorEntryTypes::Contexts) {
            contextArray = data;
            contextLength = blockLen;
            #ifdef QMTRANSLATPR_DEEP_DEBUG
            printf("Has contexts array!\n");
            #endif
        } else if (tag == QTranslatorEntryTypes::Hashes) {
            offsetArray = data;
            offsetLength = blockLen;
            #ifdef QMTRANSLATPR_DEEP_DEBUG
            printf("Has hashes! %i\n", offsetLength);
            #endif
        } else if (tag == QTranslatorEntryTypes::Messages) {
            messageArray = data;
            messageLength = blockLen;
            #ifdef QMTRANSLATPR_DEEP_DEBUG
            printf("Has messages! %i\n", messageLength);
            #endif
        } else if (tag == QTranslatorEntryTypes::NumerusRules) {
            numerusRulesArray = data;
            numerusRulesLength = blockLen;
            #ifdef QMTRANSLATPR_DEEP_DEBUG
            printf("Has numerus rules! %i\n", numerusRulesLength);
            #endif
        } else if (tag == QTranslatorEntryTypes::Dependencies) {

            std::string dep;
            while(blockLen != 0)
            {
                unsigned char* begin = data;
                while(*data != '\0') { data++; }
                std::string::size_type gotLen = (data - begin);
                dep = std::string((char*)begin, gotLen);
                if(dep.size() > 0)
                {
                    //List of dependent files
                    dependencies.push_back(dep);
                    #ifdef QMTRANSLATPR_DEEP_DEBUG
                    printf("Dependency: %s\n", dep.c_str());
                    #endif
                }
            }
            #ifdef QMTRANSLATPR_DEEP_DEBUG
            printf("Had deps!\n");
            #endif
        }
        data += blockLen;
    }

    if (dependencies.empty() && (!offsetArray || !messageArray))
        ok = false;
    #ifdef QMTRANSLATPR_DEEP_DEBUG
    printf("Dependencies valid: %i\n", ok);
    #endif

    if (ok && !isValidNumerusRules(numerusRulesArray, numerusRulesLength))
        ok = false;

    #ifdef QMTRANSLATPR_DEEP_DEBUG
    printf("Numerus rules valid: %i\n", ok);
    #endif

    //Process loading of sub-translators
    if (ok) {
        const int dependenciesCount = dependencies.size();
        subTranslators.reserve(dependenciesCount);
        for (int i = 0 ; i < dependenciesCount; ++i) {
            QmTranslatorX *translator = new QmTranslatorX;
            subTranslators.push_back(translator);
            ok = translator->loadFile(dependencies[i].c_str(), directory);
            if (!ok)
                break;
        }

        // In case some dependencies fail to load, unload all the other ones too.
        if (!ok) {
            for(QmTranslatorX* it : subTranslators)
                delete it;
            subTranslators.clear();
        }
    }

    if (!ok) {
        messageArray = 0;
        contextArray = 0;
        offsetArray = 0;
        numerusRulesArray = 0;
        messageLength = 0;
        contextLength = 0;
        offsetLength = 0;
        numerusRulesLength = 0;
        #ifdef QMTRANSLATPR_DEEP_DEBUG
        printf("LOADING FAILED!\n");
        #endif
        return false;
    } else {
        #ifdef QMTRANSLATPR_DEEP_DEBUG
        printf("LOADING PASSED!\n");
        #endif
        return true;
    }
}

bool QmTranslatorX::isEmpty()
{
    return !FileData && !FileLength && !messageArray &&
           !offsetArray && !contextArray && subTranslators.empty();
}

void QmTranslatorX::close()
{
    messageArray = 0;
    contextArray = 0;
    offsetArray = 0;
    numerusRulesArray = 0;
    messageLength = 0;
    contextLength = 0;
    offsetLength = 0;
    numerusRulesLength = 0;

    if(FileData)
        free(FileData);

    FileData = 0;
    FileLength = 0;

    for(QmTranslatorX* it : subTranslators)
        delete it;
    subTranslators.clear();
}
