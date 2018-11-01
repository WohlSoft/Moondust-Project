/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GLOBAL_PGE_VERSION
#define GLOBAL_PGE_VERSION

#ifdef PGE_TOOLCHAIN_VERSION_1
#   define V_VP1 PGE_TOOLCHAIN_VERSION_1
#else
#   define V_VP1 0
#endif

#ifdef PGE_TOOLCHAIN_VERSION_2
#   define V_VP2 PGE_TOOLCHAIN_VERSION_2
#else
#   define V_VP2 0
#endif

#ifdef PGE_TOOLCHAIN_VERSION_3
#   define V_VP3 PGE_TOOLCHAIN_VERSION_3
#else
#   define V_VP3 0
#endif

#ifdef PGE_TOOLCHAIN_VERSION_4
#   define V_VP4 PGE_TOOLCHAIN_VERSION_4
#else
#   define V_VP4 0
#endif

#ifdef PGE_CONFIG_PACK_API_VERSION
#   define V_CP_API PGE_CONFIG_PACK_API_VERSION
#else
#   define V_CP_API 0
#endif

#ifdef PGE_TOOLCHAIN_VERSION_REL
#   define V_TOOLCHAIN_RELEASE PGE_TOOLCHAIN_VERSION_REL
#else
#   define V_TOOLCHAIN_RELEASE "-unk" //"-alpha","-beta","-dev", or "" aka "release"
#endif

#define GEN_VERSION_NUMBER_2(v1,v2)     v1 "." v2
#define GEN_VERSION_NUMBER_3(v1,v2,v3)  v1 "." v2 "." v3
#define GEN_VERSION_NUMBER_4(v1,v2,v3,v4)  v1 "." v2 "." v3 "." v4

#define STR_VALUE_REAL(arg)  #arg
#define STR_VALUE(verf)      STR_VALUE_REAL(verf)
#define V_VP1_s STR_VALUE(V_VP1)
#define V_VP2_s STR_VALUE(V_VP2)
#define V_VP3_s STR_VALUE(V_VP3)
#define V_VP4_s STR_VALUE(V_VP4)
#if V_VP4 == 0
    #if V_VP3 == 0
        #define V_PROJECT_VERSION_NUM GEN_VERSION_NUMBER_2(V_VP1_s, V_VP2_s)
    #else
        #define V_PROJECT_VERSION_NUM GEN_VERSION_NUMBER_3(V_VP1_s, V_VP2_s, V_VP3_s)
    #endif
#else
    #define V_PROJECT_VERSION_NUM GEN_VERSION_NUMBER_4(V_VP1_s, V_VP2_s, V_VP3_s, V_VP4_s)
#endif

//Version of whole project
#define V_VERSION V_PROJECT_VERSION_NUM
#define V_RELEASE "" //Developing state (for release this field must be empty)

#define V_COPYRIGHT "2014-2018 by Wohlstand"

#define V_COMPANY "PGE Team"

#define V_PGE_URL "wohlsoft.ru"

#define V_PRODUCT_NAME "Platformer Game Engine"

#if defined(_WIN64)
    #define OPERATION_SYSTEM "Windows x64"
#elif defined(_WIN32)
    #define OPERATION_SYSTEM "Windows"
#elif defined __APPLE__
    #define OPERATION_SYSTEM "macOS"
#elif defined __FreeBSD__
    #define OPERATION_SYSTEM "FreeBSD"
#elif defined __gnu_linux__
    #define OPERATION_SYSTEM "GNU/Linux"
#elif defined __HAIKU__
    #define OPERATION_SYSTEM "Haiku"
#elif defined __EMSCRIPTEN__
    #define OPERATION_SYSTEM "Emscripten"
#else
    #define OPERATION_SYSTEM "Other"
#endif


#if defined(_X86_)||defined(__i386__)||defined(__i486__)||defined(__i586__)||defined(__i686__)||defined(_M_IX86)
    #define FILE_CPU "x86 (32-bit)"
#elif defined(__x86_64__)||defined(__amd64__)||defined(_WIN64)||defined(_M_X64)||defined(_M_AMD64)
    #define FILE_CPU "x86_64 (64-bit)"
#elif defined(__arm__)||defined(_M_ARM)||defined(_M_ARMT)
    #ifdef __aarch64__
        #define FILE_CPU "ARM (64-bit)"
    #else
        #define FILE_CPU "ARM (32-bit)"
    #endif
#elif defined(__mips__)||defined(__mips)||defined(__MIPS__)
    #define FILE_CPU "MIPS"
#elif defined(__powerpc__)||defined(_M_PPC)||defined(__POWERPC__)
    #if defined(__powerpc64__)||defined(__ppc64__)||defined(__PPC64__)
        #define FILE_CPU "PowerPC (64-bit)"
    #else
        #define FILE_CPU "PowerPC (32-bit)"
    #endif
#else
    #define FILE_CPU "unknown"
#endif

#define V_DATE_OF_BUILD __DATE__ " " __TIME__

#endif

