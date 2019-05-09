/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef SETUP_LOAD_SCREEN_H
#define SETUP_LOAD_SCREEN_H

#include <string>
#include <vector>
#include <graphics/gl_color.h>

class IniProcessing;

struct LoadingScreenAdditionalImage
{
    std::string imgFile;
    bool animated;
    int frames;
    uint32_t frameDelay;
    int x;
    int y;
};

struct LoadingScreenSetup
{
    void init(IniProcessing& engine_ini);
    std::string backgroundImg;
    GlColor backgroundColor;
    uint32_t updateDelay;
    std::string luaFile;
    std::vector<LoadingScreenAdditionalImage > AdditionalImages;
};

#endif // SETUP_LOAD_SCREEN_H

