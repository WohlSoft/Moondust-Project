/*
 * SMBX64 Playble Character Sprite Calibrator, a free tool for playable srite design
 * This is a part of the Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "../pge_version.h" //Global Project version file

#ifndef EDITOR_VERSION_H
#define EDITOR_VERSION_H

//Version of this program
#define _FILE_VERSION "0.0.9.0"
#define _FILE_RELEASE " pre-alpha"

#define _VF1 0
#define _VF2 0
#define _VF3 8
#define _VF4 1

#define _FILE_DESC "Game Engine"

#define _INTERNAL_NAME "pge_engine"

#ifdef _WIN32
	#define _ORIGINAL_NAME "pge_engine.exe" // for Windows platforms
#else
	#define _ORIGINAL_NAME "pge_engine" // for any other platforms
#endif

//Uncomment this for enable detal logging
//#define DEBUG

#endif
