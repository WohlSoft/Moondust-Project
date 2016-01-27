#ifndef CHARSETCONVERT_H
#define CHARSETCONVERT_H

#define SI_NO_MBSTOWCS_NULL

#define SI_Case     SI_GenericCase
#define SI_NoCase   SI_GenericNoCase

#include <wchar.h>
#include "ConvertUTF.h"
#include <cstdlib>

/**
 * Converts UTF-8 to a wchar_t (or equivalent) using the Unicode reference
 * library functions. This can be used on all platforms.
 */
template<class SI_CHAR>
class SI_ConvertW {
    bool m_bStoreIsUtf8;
protected:
    SI_ConvertW() { }
public:
    SI_ConvertW(bool a_bStoreIsUtf8) : m_bStoreIsUtf8(a_bStoreIsUtf8) { }

    /* copy and assignment */
    SI_ConvertW(const SI_ConvertW & rhs) { operator=(rhs); }
    SI_ConvertW & operator=(const SI_ConvertW & rhs) {
        m_bStoreIsUtf8 = rhs.m_bStoreIsUtf8;
        return *this;
    }

    static size_t utf8len(const char *s)
    {
        size_t len = 0;
        while(*s)
            len += (*(s++)&0xC0)!=0x80;
        return len;
    }

    /** Calculate the number of SI_CHAR required for converting the input
     * from the storage format. The storage format is always UTF-8 or MBCS.
     *
     * @param a_pInputData  Data in storage format to be converted to SI_CHAR.
     * @param a_uInputDataLen Length of storage format data in bytes. This
     *                      must be the actual length of the data, including
     *                      NULL byte if NULL terminated string is required.
     * @return              Number of SI_CHAR required by the string when
     *                      converted. If there are embedded NULL bytes in the
     *                      input data, only the string up and not including
     *                      the NULL byte will be converted.
     * @return              -1 cast to size_t on a conversion error.
     */
    size_t SizeFromStore(
        const char *    a_pInputData,
        size_t          a_uInputDataLen)
    {
        //SI_ASSERT(a_uInputDataLen != (size_t) -1);

        if (m_bStoreIsUtf8) {
            return utf8len(a_pInputData);
        }

#if defined(SI_NO_MBSTOWCS_NULL) || (!defined(_MSC_VER) && !defined(_linux))
        // fall back processing for platforms that don't support a NULL dest to mbstowcs
        // worst case scenario is 1:1, this will be a sufficient buffer size
        (void)a_pInputData;
        return a_uInputDataLen;
#else
        // get the actual required buffer size
        return mbstowcs(NULL, a_pInputData, a_uInputDataLen);
#endif
    }

    /** Convert the input string from the storage format to SI_CHAR.
     * The storage format is always UTF-8 or MBCS.
     *
     * @param a_pInputData  Data in storage format to be converted to SI_CHAR.
     * @param a_uInputDataLen Length of storage format data in bytes. This
     *                       must be the actual length of the data, including
     *                       NULL byte if NULL terminated string is required.
     * @param a_pOutputData Pointer to the output buffer to received the
     *                       converted data.
     * @param a_uOutputDataSize Size of the output buffer in SI_CHAR.
     * @return              true if all of the input data was successfully
     *                       converted.
     */
    bool ConvertFromStore(
        const char *    a_pInputData,
        size_t          a_uInputDataLen,
        SI_CHAR *       a_pOutputData,
        size_t          a_uOutputDataSize)
    {
        if (m_bStoreIsUtf8) {
            // This uses the Unicode reference implementation to do the
            // conversion from UTF-8 to wchar_t. The required files are
            // ConvertUTF.h and ConvertUTF.c which should be included in
            // the distribution but are publically available from unicode.org
            // at http://www.unicode.org/Public/PROGRAMS/CVTUTF/
            ConversionResult retval;
            const UTF8 * pUtf8 = (const UTF8 *) a_pInputData;
            if (sizeof(SI_CHAR) == sizeof(UTF32)) {
                UTF32 * pUtf32 = (UTF32 *) a_pOutputData;
                retval = ConvertUTF8toUTF32(
                    &pUtf8, pUtf8 + a_uInputDataLen,
                    &pUtf32, pUtf32 + a_uOutputDataSize,
                    lenientConversion);
            }
            else if (sizeof(SI_CHAR) == sizeof(UTF16)) {
                UTF16 * pUtf16 = (UTF16 *) a_pOutputData;
                retval = ConvertUTF8toUTF16(
                    &pUtf8, pUtf8 + a_uInputDataLen,
                    &pUtf16, pUtf16 + a_uOutputDataSize,
                    lenientConversion);
            }
            return retval == conversionOK;
        }

        // convert to wchar_t
        size_t retval = mbstowcs(a_pOutputData,
            a_pInputData, a_uOutputDataSize);
        return retval != (size_t)(-1);
    }

    /** Calculate the number of char required by the storage format of this
     * data. The storage format is always UTF-8 or MBCS.
     *
     * @param a_pInputData  NULL terminated string to calculate the number of
     *                       bytes required to be converted to storage format.
     * @return              Number of bytes required by the string when
     *                       converted to storage format. This size always
     *                       includes space for the terminating NULL character.
     * @return              -1 cast to size_t on a conversion error.
     */
    size_t SizeToStore(
        const SI_CHAR * a_pInputData)
    {
        if (m_bStoreIsUtf8) {
            // worst case scenario for wchar_t to UTF-8 is 1 wchar_t -> 6 char
            size_t uLen = 0;
            while (a_pInputData[uLen])
            {
                ++uLen;
            }
            return (6 * uLen) + 1;
        }
        else {
            size_t uLen = wcstombs(NULL, a_pInputData, 0);
            if (uLen == (size_t)(-1)) {
                return uLen;
            }
            return uLen + 1; // include NULL terminator
        }
    }

    /** Convert the input string to the storage format of this data.
     * The storage format is always UTF-8 or MBCS.
     *
     * @param a_pInputData  NULL terminated source string to convert. All of
     *                       the data will be converted including the
     *                       terminating NULL character.
     * @param a_pOutputData Pointer to the buffer to receive the converted
     *                       string.
     * @param a_uOutputDataSize Size of the output buffer in char.
     * @return              true if all of the input data, including the
     *                       terminating NULL character was successfully
     *                       converted.
     */
    bool ConvertToStore(
        const SI_CHAR * a_pInputData,
        char *          a_pOutputData,
        size_t          a_uOutputDataSize
        )
    {
        if (m_bStoreIsUtf8) {
            // calc input string length (SI_CHAR type and size independent)
            size_t uInputLen = 0;
            while (a_pInputData[uInputLen]) {
                ++uInputLen;
            }
            ++uInputLen; // include the NULL char

            // This uses the Unicode reference implementation to do the
            // conversion from wchar_t to UTF-8. The required files are
            // ConvertUTF.h and ConvertUTF.c which should be included in
            // the distribution but are publically available from unicode.org
            // at http://www.unicode.org/Public/PROGRAMS/CVTUTF/
            ConversionResult retval;
            UTF8 * pUtf8 = (UTF8 *) a_pOutputData;
            if (sizeof(SI_CHAR) == sizeof(UTF32)) {
                const UTF32 * pUtf32 = (const UTF32 *) a_pInputData;
                retval = ConvertUTF32toUTF8(
                    &pUtf32, pUtf32 + uInputLen,
                    &pUtf8, pUtf8 + a_uOutputDataSize,
                    lenientConversion);
            }
            else if (sizeof(SI_CHAR) == sizeof(UTF16)) {
                const UTF16 * pUtf16 = (const UTF16 *) a_pInputData;
                retval = ConvertUTF16toUTF8(
                    &pUtf16, pUtf16 + uInputLen,
                    &pUtf8, pUtf8 + a_uOutputDataSize,
                    lenientConversion);
            }
            return retval == conversionOK;
        }
        else {
            size_t retval = wcstombs(a_pOutputData,
                a_pInputData, a_uOutputDataSize);
            return retval != (size_t) -1;
        }
    }
};

#endif // CHARSETCONVERT_H

