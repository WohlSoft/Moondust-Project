/*
 * SMBX64 Playble Character Sprite Calibrator, a free tool for playable srite design
 * This is a part of the Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#include "../pge_version.h" //Global Project version file

#ifndef ENGINE_VERSION_H
#define ENGINE_VERSION_H

#ifdef GIT_VERSION
#define V_BUILD_VER GIT_VERSION
#else
#define V_BUILD_VER "<empty>"
#endif

#ifdef GIT_BRANCH
#define V_BUILD_BRANCH GIT_BRANCH
#else
#define V_BUILD_BRANCH "<unknown>"
#endif

#ifdef ENGINE_VERSION_1
#   define V_VF1 ENGINE_VERSION_1
#else
#   define V_VF1 0
#endif

#ifdef ENGINE_VERSION_2
#   define V_VF2 ENGINE_VERSION_2
#else
#   define V_VF2 0
#endif

#ifdef ENGINE_VERSION_3
#   define V_VF3 ENGINE_VERSION_3
#else
#   define V_VF3 0
#endif

#ifdef ENGINE_VERSION_4
#   define V_VF4 ENGINE_VERSION_4
#else
#   define V_VF4 0
#endif

#ifdef ENGINE_VERSION_REL
#   define V_FILE_RELEASE STR_VALUE(ENGINE_VERSION_REL)
#else
#   define V_FILE_RELEASE "-unk" //"-alpha","-beta","-dev", or "" aka "release"
#endif

#define V_VF1_s STR_VALUE(V_VF1)
#define V_VF2_s STR_VALUE(V_VF2)
#define V_VF3_s STR_VALUE(V_VF3)
#define V_VF4_s STR_VALUE(V_VF4)
#if V_VF4 == 0
    #if V_VF3 == 0
        #define V_FILE_VERSION_NUM GEN_VERSION_NUMBER_2(V_VF1_s, V_VF2_s)
    #else
        #define V_FILE_VERSION_NUM GEN_VERSION_NUMBER_3(V_VF1_s, V_VF2_s, V_VF3_s)
    #endif
#else
    #define V_FILE_VERSION_NUM GEN_VERSION_NUMBER_4(V_VF1_s, V_VF2_s, V_VF3_s, V_VF4_s)
#endif

#define V_FILE_VERSION V_FILE_VERSION_NUM
//Version of this program
#define _LATEST_STABLE V_FILE_VERSION_NUM V_FILE_RELEASE

#define V_FILE_DESC "Game Engine"

#define V_INTERNAL_NAME "pge_engine"

#ifdef _WIN32
	#define V_ORIGINAL_NAME "pge_engine.exe" // for Windows platforms
#else
	#define V_ORIGINAL_NAME "pge_engine" // for any other platforms
#endif

//Uncomment this for enable detal logging
//#define DEBUG

#endif
