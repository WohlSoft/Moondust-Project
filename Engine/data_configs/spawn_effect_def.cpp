/*
 * Moondust, a free game engine for platform game making
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

#include "spawn_effect_def.h"
#include <vector>

#include <script/lua_engine.h>

SpawnEffectDef::SpawnEffectDef()
{
    id = 0;
    start_delay = 0;
    startX = 0.0;
    startY = 0.0;
    animationLoops = 1;
    delay = 0;
    frameDelay = 64;
    velocityX = 0.0;
    velocityY = 0.0;
    zIndex = -5.0L;
    gravity = 0.0;
    direction = 1;
    min_vel_x = 0.0;
    min_vel_y = 0.0;
    max_vel_x = 0.0;
    max_vel_y = 0.0;
    decelerate_x = 0.0;
    decelerate_y = 0.0;
}

SpawnEffectDef::SpawnEffectDef(const SpawnEffectDef &c)
{
    id = c.id;
    start_delay = c.start_delay;
    startX = c.startX;
    startY = c.startY;
    animationLoops = c.animationLoops;
    delay = c.delay;
    frameDelay = c.frameDelay;
    frame_sequence = c.frame_sequence;
    velocityX = c.velocityX;
    velocityY = c.velocityY;
    zIndex    = c.zIndex;
    gravity = c.gravity;
    direction = c.direction;
    min_vel_x = c.min_vel_x;
    min_vel_y = c.min_vel_y;
    max_vel_x = c.max_vel_x;
    max_vel_y = c.max_vel_y;
    decelerate_x = c.decelerate_x;
    decelerate_y = c.decelerate_y;
}

void SpawnEffectDef::fill(const std::string &prefix, IniProcessing *setup)
{
    if(!setup)
        return;

    setup->read((prefix + "-effect-function").c_str(), lua_function, "");
    setup->read((prefix + "-effect-id").c_str(), id, 0);
    setup->read((prefix + "-effect-start-delay").c_str(), start_delay, 0);
    setup->read((prefix + "-effect-start-x").c_str(), startX, 0.0);
    setup->read((prefix + "-effect-start-y").c_str(), startY, 0.0);
    setup->read((prefix + "-effect-animation-loops").c_str(), animationLoops, 1);
    setup->read((prefix + "-effect-delay").c_str(), delay, 0);
    setup->read((prefix + "-effect-frame-delay").c_str(), frameDelay, 0);
    setup->read((prefix + "-effect-framespeed").c_str(), frameDelay, frameDelay);
    setup->read((prefix + "-effect-velocity-x").c_str(), velocityX, 0.0);
    setup->read((prefix + "-effect-velocity-y").c_str(), velocityY, 0.0);
    setup->read((prefix + "-effect-z-index").c_str(), zIndex, -5.0l);
    setup->read((prefix + "-effect-gravity").c_str(), gravity, 0.0);
    setup->read((prefix + "-effect-direction").c_str(), direction, 1);
    setup->read((prefix + "-effect-min-vel-x").c_str(), min_vel_x, 0.0);
    setup->read((prefix + "-effect-min-vel-y").c_str(), min_vel_y, 0.0);
    setup->read((prefix + "-effect-max-vel-x").c_str(), max_vel_x, 0.0);
    setup->read((prefix + "-effect-max-vel-y").c_str(), max_vel_y, 0.0);
    setup->read((prefix + "-effect-decelerate-x").c_str(), decelerate_x, 0.0);
    setup->read((prefix + "-effect-decelerate-y").c_str(), decelerate_y, 0.0);
    setup->read((prefix + "-effect-frame-sequence").c_str(), frame_sequence, std::vector<int>());
}

void SpawnEffectDef::lua_setSequence(luabind::adl::object frames)
{
    int ltype = luabind::type(frames);

    if(luabind::type(frames) != LUA_TTABLE)
    {
        luaL_error(frames.interpreter(),
                   "setSequence exptected int-array, got %s",
                   lua_typename(frames.interpreter(), ltype));
        return;
    }

    frame_sequence = luabind_utils::convArrayTo<int>(frames);
}
