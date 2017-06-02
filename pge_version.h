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

#define _VP1 0
#define _VP2 4
#define _VP3 0
#define _VP4 0

#define GEN_VERSION_NUMBER_2(v1,v2)     v1 "." v2
#define GEN_VERSION_NUMBER_3(v1,v2,v3)  v1 "." v2 "." v3
#define GEN_VERSION_NUMBER_4(v1,v2,v3,v4)  v1 "." v2 "." v3 "." v4

#define STR_VALUE_REAL(arg)  #arg
#define STR_VALUE(verf)      STR_VALUE_REAL(verf)
#define _VP1_s STR_VALUE(_VP1)
#define _VP2_s STR_VALUE(_VP2)
#define _VP3_s STR_VALUE(_VP3)
#define _VP4_s STR_VALUE(_VP4)
#if _VP4 == 0
    #if _VP3 == 0
        #define _PROJECT_VERSION_NUM GEN_VERSION_NUMBER_2(_VP1_s, _VP2_s)
    #else
        #define _PROJECT_VERSION_NUM GEN_VERSION_NUMBER_3(_VP1_s, _VP2_s, _VP3_s)
    #endif
#else
    #define _PROJECT_VERSION_NUM GEN_VERSION_NUMBER_4(_VP1_s, _VP2_s, _VP3_s, _VP4_s)
#endif

//Version of whole project
#define _VERSION _PROJECT_VERSION_NUM
#define _RELEASE "" //Developing state (for release this field must be empty)

#define _COPYRIGHT "2014-2017 by Wohlstand"

#define _COMPANY "PGE Team"

#define _PGE_URL "wohlsoft.ru"

#define _PRODUCT_NAME "Platformer Game Engine"

#ifdef _WIN64
	#define OPERATION_SYSTEM "Windows x64"
#elif  _WIN32
	#define OPERATION_SYSTEM "Windows"
#elif defined __APPLE__
	#define OPERATION_SYSTEM "MacOSX"
#elif defined __FreeBSD__
	#define OPERATION_SYSTEM "FreeBSD"
#elif defined __gnu_linux__
	#define OPERATION_SYSTEM "GNU/Linux"
#else
	#define OPERATION_SYSTEM "Other"
#endif

#define _DATE_OF_BUILD __DATE__ " " __TIME__


#endif

