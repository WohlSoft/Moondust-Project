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

#include <luabind/luabind.hpp>
#include <luabind/function.hpp>
#include <luabind/class.hpp>
#include <luabind/detail/primitives.hpp> // for null_type
#include <luabind/detail/call_function.hpp>
#include <luabind/operator.hpp>

#include "luaclass_level_playerstate.h"

#include "luaclass_level_lvl_player.h"
#include <script/lua_global.h>

Binding_Level_ClassWrapper_LVL_Player::Binding_Level_ClassWrapper_LVL_Player() : LVL_Player()
{
    m_isLuaPlayer = true;
}

Binding_Level_ClassWrapper_LVL_Player::~Binding_Level_ClassWrapper_LVL_Player()
{}

/***
Level Playable Character class, functions, and callback events
@module LevelPlayerClass
*/

/***
Playble Character Controller base class, must be implemented for every Playable Character which needs more complex logic than engine provides
@type PlayerControllerBase
*/

/***
Controller constructor, once called on first initialization of Playable Character Controller.<br>
<b>Required to be defined in an Playable Character controller class!</b>
@function __init
@tparam BasePlayer player_obj Reference to actual NPC object

@usage
class 'MyPlayer'

function MyPlayer:initPros()
    self.timer = 0
    self.state = 0
    -- ...
end

function MyPlayer:__init(player_obj)
    self.player_obj = player_obj
    self:initProps()
end

-- ...

return MyPlayer
*/
void Binding_Level_ClassWrapper_LVL_Player::lua_onInit()
{
    if(!LuaGlobal::getEngine(mself.ref(*this).state())->shouldShutdown())
        call<void>("onInit");
}


/***
Event callback calling per every frame
@function onLoop
@tparam double frameDelay Frame delay in milliseconds. Use it for various timing processors.

@usage
class 'MyPlayer'

-- A timer value
local timer = 0

-- Play sound every one second
function ticker(frameDelay)
    -- Iterate a timer
    timer = timer + frameDelay
    -- Check if timer reaches one whole second
    if(timer >= 1000)then
        -- Play a tick sound
        Audio.playSoundByRole(SoundRoles.MenuScroll)
        -- Reset timer to zero without lost of timing accuracy
        timer = timer - 1000
    end
end

function MyPlayer:onLoop(frameDelay)
    ticker(frameDelay)
end

return MyPlayer
*/
void Binding_Level_ClassWrapper_LVL_Player::lua_onLoop(double tickTime)
{
    if(!LuaGlobal::getEngine(mself.ref(*this).state())->shouldShutdown())
        call<void>("onLoop", tickTime);
}

/***
Event callback calling on attempt to hurt Playable Character
@function onHarm
@tparam PlayerHarmEvent harmEvent Harm event context. Can be used to identify reasons and gives ability to reject this event.

@usage
class 'MyPlayer'

function MyPlayer:onHarm(harmEvent)
    -- Say "Ouch" when Playable Character was kicked to ass, but don't harm it
    if(harmEvent.do_harm == true)then
        Audio.playSound(5);
        harmEvent.do_harm = false
    end
end

return MyPlayer
*/

void Binding_Level_ClassWrapper_LVL_Player::lua_onHarm(LVL_Player_harm_event *harmevent)
{
    if(!LuaGlobal::getEngine(mself.ref(*this).state())->shouldShutdown())
        call<void>("onHarm", harmevent);
}

/***
Event callback calling on transform of playable character into another or on state change
@function onTransform
@tparam ulong characterID ID of destinition playable character
@tparam ulong stateID ID of destinition playable character's state

@usage
class 'MyPlayer'

function MyPlayer:onTransform(characterID, stateID)
    -- Do something here...
end

return MyPlayer
*/
void Binding_Level_ClassWrapper_LVL_Player::lua_onTransform(unsigned long character, unsigned long state)
{
    if(!LuaGlobal::getEngine(mself.ref(*this).state())->shouldShutdown())
        call<void>("onTransform", character, state);
}

/***
Event callback calling on attempt of player to take an NPC
@function onTakeNpc
@tparam LevelNpcClass.BaseNPC npc reference to NPC which was taken by player

@usage
class 'MyPlayer'

function MyPlayer:onTakeNpc(npc)
    -- Do something here...
end

return MyPlayer
*/
void Binding_Level_ClassWrapper_LVL_Player::lua_onTakeNpc(LVL_Npc *npc)
{
    if(!LuaGlobal::getEngine(mself.ref(*this).state())->shouldShutdown())
        call<void>("onTakeNpc", npc);
}

/***
Event callback calling on attempt of player to kill an NPC
@function onKillNpc
@tparam LevelNpcClass.BaseNPC npc reference to NPC which was killed by player

@usage
class 'MyPlayer'

function MyPlayer:onKillNpc(npc)
    -- Do something here...
end

return MyPlayer
*/
void Binding_Level_ClassWrapper_LVL_Player::lua_onKillNpc(LVL_Npc *npc)
{
    if(!LuaGlobal::getEngine(mself.ref(*this).state())->shouldShutdown())
        call<void>("onKillNpc", npc);
}

/***
Event callback calling on key pressing on playable character's controller device
@function onKeyPressed
@tparam GlobalConstants.PLAYER_KEY_CODES keyType Key type pressed on controller device which is attached to this playable character

@usage
class 'MyPlayer'

function MyPlayer:onKeyPressed(keyType)
    -- Do something here...
end

return MyPlayer
*/
void Binding_Level_ClassWrapper_LVL_Player::lua_onKeyPressed(ControllableObject::KeyType ktype)
{
    if(!LuaGlobal::getEngine(mself.ref(*this).state())->shouldShutdown())
        call<void>("onKeyPressed", ktype);
}

/***
Event callback calling on key releasing on playable character's controller device
@function onKeyReleased
@tparam GlobalConstants.PLAYER_KEY_CODES keyType Key type of released on controller device which is attached to this playable character

@usage
class 'MyPlayer'

function MyPlayer:onKeyReleased(keyType)
    -- Do something here...
end

return MyPlayer
*/
void Binding_Level_ClassWrapper_LVL_Player::lua_onKeyReleased(ControllableObject::KeyType ktype)
{
    if(!LuaGlobal::getEngine(mself.ref(*this).state())->shouldShutdown())
        call<void>("onKeyReleased", ktype);
}

luabind::scope Binding_Level_ClassWrapper_LVL_Player::bindToLua()
{
    using namespace luabind;
    using namespace luabind::detail;
    return
        class_<LVL_Player, PGE_Phys_Object, null_type, Binding_Level_ClassWrapper_LVL_Player>("BasePlayer")
        /***
        Playable Character Object base class, inherited from @{PhysBaseClass.PhysBase}
        @type BasePlayer
        */
        .def(constructor<>())
        /***
        Current playable character ID (Read Only)
        @tfield ulong characterID
        */
        .def_readonly("characterID", &LVL_Player::characterID)
        /***
        Current playable character's state ID (Read Only)
        @tfield ulong stateID
        */
        .def_readonly("stateID", &LVL_Player::stateID)
        /***
        Current playable character's face direction (Read Only)
        @tfield int direction -1 - Player is faced to left, +1 - Player is faced to right
        */
        .property("direction", &LVL_Player::direction)
        /***
        Is playable character stays on solid ground (Read Only)
        @tfield bool onGround
        */
        .def_readonly("onGround", &LVL_Player::onGround)
        /***
        Is playable character ducking (Read Only)
        @tfield bool isDucking
        */
        .def_readonly("isDucking", &LVL_Player::m_ducking)

        /***
        Global state information of a player
        @tfield LevelCommon.LevelPlayerState globalState
        */
        .property("globalState",  &LVL_Player::m_global_state)

        /***
        Playable character's health level
        @tfield int health
        */
        .property("health", &LVL_Player::getHealth, &LVL_Player::setHealth)

        .def("onInit", &LVL_Player::lua_onInit, &Binding_Level_ClassWrapper_LVL_Player::def_lua_onInit)
        .def("onLoop", &LVL_Player::lua_onLoop, &Binding_Level_ClassWrapper_LVL_Player::def_lua_onLoop)
        .def("onHarm", &LVL_Player::lua_onHarm, &Binding_Level_ClassWrapper_LVL_Player::def_lua_onHarm)
        .def("onTransform", &LVL_Player::lua_onTransform, &Binding_Level_ClassWrapper_LVL_Player::def_lua_onTransform)
        .def("onTakeNpc", &LVL_Player::lua_onTakeNpc, &Binding_Level_ClassWrapper_LVL_Player::def_lua_onTakeNpc)
        .def("onKillNpc", &LVL_Player::lua_onKillNpc, &Binding_Level_ClassWrapper_LVL_Player::def_lua_onKillNpc)
        .def("onKeyPressed", &LVL_Player::lua_onKeyPressed, &Binding_Level_ClassWrapper_LVL_Player::def_lua_onKeyPressed)
        .def("onKeyReleased", &LVL_Player::lua_onKeyReleased, &Binding_Level_ClassWrapper_LVL_Player::def_lua_onKeyReleased)

        .def("spawnNPC", &LVL_Player::lua_spawnNPC)
        /***
        Get pressed state of a specific key
        @function getKeyState
        @tparam GlobalConstants.PLAYER_KEY_CODES keyType Code of key code
        @treturn bool true if key is pressed, false is released
        */
        .def("getKeyState",  &LVL_Player::lua_getKeyState)

        /***
        Change currently animation sequence
        @function setAnimation
        @tparam int animationID Index of animation sequence in the calibration file
        @tparam int frameDelay Delay of one animation frame in milliseconds
        */
        .def("setAnimation", &LVL_Player::lua_setAnimation)

        /***
        Play specified animation sequence once and switch previous animation back
        @function playAnimationOnce
        @tparam int animationID Index of animation sequence in the calibration file
        @tparam int frameDelay Delay of one animation frame in milliseconds
        @tparam bool fixedSpeed Forbid framedelay change while animation is playing
        @tparam bool locked Forbid animation switch while sequence is playing
        @tparam int skipLastFrames Count of last frames to skip until end animation
        */
        .def("playAnimationOnce", &LVL_Player::lua_playAnimationOnce)

        /***
        Bump playable character vertically to down
        @function bump
        */

        /***
        Bump playable character vertically
        @function bump
        @tparam bool upDirection Bump playable character up, if false - to down
        */

        /***
        Bump playable character vertically with custom bounce speed
        @function bump
        @tparam bool upDirection Bump playable character up, if false - to down
        @tparam double bounceSpeed Initial flying speed
        */

        /***
        Bump playable character vertically with custom bounce speed
        @function bump
        @tparam bool upDirection Bump playable character up, if false - to down
        @tparam double bounceSpeed Initial flying speed
        @tparam int timeToJump Allow playable character jump higher in specified time delay, doesnt affects down directed bounce
        */
        .def("bump", &LVL_Player::bump)

        /***
        Change playable character's state/skin
        @function setState
        @tparam ulong stateID ID of state to toggle
        */
        .def("setState", &LVL_Player::setState)

        /***
        Change playable character
        @function setCharacter
        @tparam ulong characterID ID of playable character to toggle
        */
        .def("setCharacter", &LVL_Player::setCharacterID)

        /***
        Set temporary invincability to playable character
        @function setInvincible
        @tparam bool state true to turn invincibility on, false - turn it off
        @tparam double delay Delay of invincibility effect in milliseconds
        */

        /***
        Set temporary invincability to playable character with blinking
        @function setInvincible
        @tparam bool state true to turn invincibility on, false - turn it off
        @tparam double delay Delay of invincibility effect in milliseconds
        @tparam bool enableSpriteBlinking Blink sprite of playable character while invincibility is active
        */
        .def("setInvincible", &LVL_Player::setInvincible)

        /***
        Attack type (AttackType)
        @section AttackType
        */
        .enum_("AttackType")
        [
            /***
            Regular hit attack
            @field BasePlayer.AttackType_Hit
            */
            value("AttackType_Hit", LVL_Player::AttackType::AttackType_Hit),
            /***
            Force-destroy found objects, used by "Chunk Norris" cheat attack
            @field BasePlayer.AttackType_ForceDestroy
            */
            value("AttackType_ForceDestroy", LVL_Player::AttackType::AttackType_ForceDestroy),
            /***
            User defined types (base value, greater values than AttackType_User are also user values!)
            @field BasePlayer.AttackType_User
            */
            value("AttackType_User", LVL_Player::AttackType::AttackType_User)
        ]
        /***
        Attack objects in the given area
        @function attackArea
        @tparam double left Left position relative to player's center
        @tparam double top Top position relative to player's bottom
        @tparam double right Right position relative to player's center
        @tparam double bottom Bottom position relative to player's bottom
        @tparam int action Attack action (see AttackType enum)
        @tparam int action Attack type
        @tparam table filters Array of integers are identifying object types needed to attack:<br>
        <ul>
            <li>1 - blocks</li>
            <li>2 - BGOs</li>
            <li>3 - NPCs</li>
            <li>4 - Playable characters</li>
        </ul>
        */
        .def("attackArea", &LVL_Player::lua_attackArea)
        ;
}

luabind::scope Binding_Level_ClassWrapper_LVL_Player::HarmEvent_bindToLua()
{
    using namespace luabind;
    return
        class_<LVL_Player_harm_event>("PlayerHarmEvent")
        /***
        Playable Character Harm Event context
        @type PlayerHarmEvent
        */
        .def(constructor<>())
        /***
        Grand this event, it will be executed if this field is true, set it to false to reject this event
        @tfield bool do_harm
        */
        .def_readwrite("do_harm", &LVL_Player_harm_event::doHarm)
        /***
        Damage level caused by this event
        @tfield int damage
        */
        .def_readwrite("damage", &LVL_Player_harm_event::doHarm_damage);
}
