/* SoX Resampler Library      Copyright (c) 2007-13 robs@users.sourceforge.net
 * Licence for this file: LGPL v2.1                  See LICENCE for details. */

#if !defined soxr_config_included
#define soxr_config_included

/**
 *  \name The two types of endianness
 */
/* @{ */
#define SDL_LIL_ENDIAN  1234
#define SDL_BIG_ENDIAN  4321
/* @} */

#ifndef SDL_BYTEORDER           /* Not defined in SDL_config.h? */
#ifdef __linux__
#include <endian.h>
#define SDL_BYTEORDER  __BYTE_ORDER
#else /* __linux__ */
#if defined(__hppa__) || \
    defined(__m68k__) || defined(mc68000) || defined(_M_M68K) || \
    (defined(__MIPS__) && defined(__MISPEB__)) || \
    defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC) || \
    defined(__sparc__)
#define WORDS_BIGENDIAN       1
#else
#define WORDS_BIGENDIAN       0
#endif
#endif /* __linux__ */
#endif /* !SDL_BYTEORDER */

#define HAVE_SINGLE_PRECISION 1
#define HAVE_DOUBLE_PRECISION 1
#define HAVE_AVFFT          0
#define HAVE_SIMD           1
#define HAVE_FENV_H         0
#define HAVE_LRINT          0

#include <limits.h>

#undef bool
#undef false
#undef true
#define bool int
#define false 0
#define true 1

#undef int16_t
#undef int32_t
#undef int64_t
#undef uint32_t
#undef uint64_t
#define int16_t short
#if LONG_MAX > 2147483647L
  #define int32_t int
  #define int64_t long
#elif LONG_MAX < 2147483647L
#error this library requires that 'long int' has at least 32-bits
#else
  #define int32_t long
  #if defined _MSC_VER
    #define int64_t __int64
  #else
    #define int64_t long long
  #endif
#endif
#define uint32_t unsigned int32_t
#define uint64_t unsigned int64_t

#endif
