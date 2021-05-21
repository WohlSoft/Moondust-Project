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
    SpawnEffectDef() = default;
    SpawnEffectDef(const SpawnEffectDef &c);
    SpawnEffectDef &operator=(const SpawnEffectDef &c);
    void fill(const std::string &prefix, IniProcessing *setup);

    //Call lua-function instead of spawning this effect
    std::string lua_function;

    void lua_setSequence(luabind::object frames);

    unsigned int start_delay = 0;

    unsigned long id = 0;
    double startX = 0.0;
    double startY = 0.0;
    int animationLoops = 1;
    int delay =0 ;
    int frameDelay = 0;
    std::vector<int> frame_sequence;
    double velocityX = 0.0;
    double velocityY = 0.0;
    long double zIndex = -5.0L;
    double gravity = 0.0;
    int direction = 1;

    double min_vel_x = 0.0;
    double min_vel_y = 0.0;
    double max_vel_x = 0.0;
    double max_vel_y = 0.0;
    double decelerate_x = 0.0;
    double decelerate_y = 0.0;
};

#endif // SPAWNEFFECTDEF_H
