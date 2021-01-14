/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef RENDER_PLATFORM_SUPPORT_H
#define RENDER_PLATFORM_SUPPORT_H

#if !defined(__ANDROID__) && !defined(__EMSCRIPTEN__)
#define RENDER_SUPORT_OPENGL2
#endif

#if !defined(__ANDROID__) && !defined(__APPLE__) && !defined(__EMSCRIPTEN__)
#define RENDER_SUPORT_OPENGL3
#endif

#endif//RENDER_PLATFORM_SUPPORT_H
