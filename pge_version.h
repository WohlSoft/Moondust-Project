/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

//Version of whole project
#define _VERSION "0.2.2"
#define _RELEASE " Alpha" //Developing state (for release this field must be empty)

#define _VP1 0
#define _VP2 2
#define _VP3 0
#define _VP4 2


#define _COPYRIGHT "2014 by Wohlstand"

#define _COMPANY "Wohlhabend Network System"

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
