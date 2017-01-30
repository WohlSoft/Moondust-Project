/* define to align allocated memory on 32-byte boundaries */
#define FLAC__ALIGN_MALLOC_DATA 1

#ifdef _WIN32
#undef  NO_OLDNAMES
#undef _NO_OLDNAMES
//#define HAVE_FSEEKO
#endif

/* define if building without ASM optimizations
   (on some files there are seems crashful, therefore disabling it) */
#define FLAC__NO_ASM

/* define if building for ia32/i386 */
/* #undef FLAC__CPU_IA32 */

/* define if building for PowerPC */
/* #undef FLAC__CPU_PPC */

/* define if building for PowerPC with SPE ABI */
/* #undef FLAC__CPU_PPC_SPE */

/* define if building for SPARC */
/* #undef FLAC__CPU_SPARC */

/* define if building for x86_64 */
/* #define FLAC__CPU_X86_64 1 */

/* define if you have docbook-to-man or docbook2man */
/* #undef FLAC__HAS_DOCBOOK_TO_MAN */

/* define if you are compiling for x86 and have the NASM assembler */
/* #undef FLAC__HAS_NASM */

/* define if you have the ogg library */
#define FLAC__HAS_OGG 1

/* Set to 1 if <x86intrin.h> is available. */
/* #define FLAC__HAS_X86INTRIN */

/* define to disable use of assembly code */
/* #undef FLAC__NO_ASM */

/* define if your operating system supports SSE instructions */
/* #define FLAC__SSE_OS 1 */

/* define if building for Darwin / MacOS X */
/* #undef FLAC__SYS_DARWIN */

/* define if building for Linux */
/* #undef FLAC__SYS_LINUX */

/* lround support */
#define HAVE_LROUND 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Version number of package */
#define VERSION "1.3.1"

/* Target processor is big endian. */
#define WORDS_BIGENDIAN 0

/* Number of bits in a file offset, on hosts where this is settable. */
/* #undef _FILE_OFFSET_BITS */

/* Define to 1 to make fseeko visible on some hosts (e.g. glibc 2.2). */
/* #undef _LARGEFILE_SOURCE */

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */

/* Define to 1 if on MINIX. */
/* #undef _MINIX */

/* Define to 2 if the system does not provide POSIX.1 features except with
   this defined. */
/* #undef _POSIX_1_SOURCE */

/* Define to 1 if you need to in order for `stat' and other things to work. */
/* #undef _POSIX_SOURCE */

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
/* #undef inline */
#endif

/* Define to __typeof__ if your compiler spells it that way. */
/* #undef typeof */
