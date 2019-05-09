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

#ifndef SPAWNEFFECTDEF_H
#define SPAWNEFFECTDEF_H

#include <IniProcessor/ini_processing.h>
#include <string>
#include <vector>

#include <luabind/luabind.hpp>
#include <lua_includes/lua.hpp>

class SpawnEffectDef
{
public:
    SpawnEffectDef();
    SpawnEffectDef(const SpawnEffectDef &c);
    void fill(const std::string &prefix, IniProcessing *setup);

    //Call lua-function instead of spawning this effect
    std::string lua_function;

    void lua_setSequence(luabind::object frames);

    unsigned int start_delay;

    unsigned long id;
    double startX;
    double startY;
    int animationLoops;
    int delay;
    int frameDelay;
    std::vector<int> frame_sequence;
    double velocityX;
    double velocityY;
    long double zIndex;
    double gravity;
    int direction;

    double min_vel_x;
    double min_vel_y;
    double max_vel_x;
    double max_vel_y;
    double decelerate_x;
    double decelerate_y;
};

#endif // SPAWNEFFECTDEF_H
