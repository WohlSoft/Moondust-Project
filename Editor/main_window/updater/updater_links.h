/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2025 Vitaly Novichkov <admin@wohlnet.ru>
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

#pragma once
#ifndef UPDATER_LINKS_H
#define UPDATER_LINKS_H

#define STABLE_LINK "http://wohlsoft.ru/PGE/#links"
#define DEVEL_LINK "http://wohlsoft.ru/docs/_laboratory/"

//#define STABLE_CHECK "https://raw.githubusercontent.com/Wohlhabend-Networks/PlatGEnWohl/master/_Misc/versions/editor_stable.txt"
//#define DEVEL_CHECK "https://raw.githubusercontent.com/Wohlhabend-Networks/PlatGEnWohl/master/_Misc/versions/editor_dev.txt"
//#define STABLE_CHECK "http://wohlsoft.ru/docs/_laboratory/_versions/editor_stable_date.txt"

#ifdef _WIN32
#define STABLE_CHECK "http://wohlsoft.ru/docs/_laboratory/_Builds/_versions/build_date_stable_win32.txt"
#elif __gnu_linux__
#define STABLE_CHECK "http://wohlsoft.ru/docs/_laboratory/_Builds/_versions/build_date_stable_linux.txt"
#elif __APPLE__
#define STABLE_CHECK "http://wohlsoft.ru/docs/_laboratory/_Builds/_versions/build_date_stable_osx.txt"
#else
#define STABLE_CHECK "http://wohlsoft.ru/docs/_laboratory/_Builds/_versions/build_date_stable_linux.txt"
#endif


#ifdef _WIN32
#define DEVEL_CHECK "http://wohlsoft.ru/docs/_laboratory/_Builds/_versions/build_date_dev_win32.txt"
#elif __gnu_linux__
#define DEVEL_CHECK "http://wohlsoft.ru/docs/_laboratory/_Builds/_versions/build_date_dev_linux.txt"
#elif __APPLE__
#define DEVEL_CHECK "http://wohlsoft.ru/docs/_laboratory/_Builds/_versions/build_date_dev_osx.txt"
#else
#define DEVEL_CHECK "http://wohlsoft.ru/docs/_laboratory/_Builds/_versions/build_date_dev_linux.txt"
#endif


#endif // UPDATER_LINKS_H

