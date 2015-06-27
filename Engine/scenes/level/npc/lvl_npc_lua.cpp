/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
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

#include "../lvl_npc.h"
#include <script/lua_engine.h>

void LVL_Npc::lua_setSequenceLeft(luabind::object frames)
{
    int ltype = luabind::type(frames);
    if(luabind::type(frames) != LUA_TTABLE){
        luaL_error(frames.interpreter(), "setSequenceLeft exptected int-array, got %s", lua_typename(frames.interpreter(), ltype));
        return;
    }
    animator.setSequenceL(luabind_utils::convArrayTo<int>(frames));
}

void LVL_Npc::lua_setSequenceRight(luabind::object frames)
{
    int ltype = luabind::type(frames);
    if(luabind::type(frames) != LUA_TTABLE){
        luaL_error(frames.interpreter(), "setSequenceRight exptected int-array, got %s", lua_typename(frames.interpreter(), ltype));
        return;
    }
    animator.setSequenceR(luabind_utils::convArrayTo<int>(frames));
}

void LVL_Npc::lua_setSequence(luabind::object frames)
{
    int ltype = luabind::type(frames);
    if(luabind::type(frames) != LUA_TTABLE){
        luaL_error(frames.interpreter(), "setSequence exptected int-array, got %s", lua_typename(frames.interpreter(), ltype));
        return;
    }
    animator.setSequence(luabind_utils::convArrayTo<int>(frames));
}

void LVL_Npc::lua_setOnceAnimation(bool en)
{
    animator.setOnceMode(en);
}

bool LVL_Npc::lua_animationIsFinished()
{
    return animator.animationFinished();
}

int LVL_Npc::lua_frameDelay()
{
    return animator.frameSpeed();
}

void LVL_Npc::lua_setFrameDelay(int ms)
{
    animator.setFrameSpeed(ms);
}
