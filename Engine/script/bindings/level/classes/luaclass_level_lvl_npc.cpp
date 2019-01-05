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

#include "luaclass_level_lvl_npc.h"
#include "luaclass_level_lvl_player.h"

#include <script/lua_global.h>

Binding_Level_ClassWrapper_LVL_NPC::Binding_Level_ClassWrapper_LVL_NPC(LevelScene *_parent) : LVL_Npc(_parent)
{
    isLuaNPC = true;
}

Binding_Level_ClassWrapper_LVL_NPC::~Binding_Level_ClassWrapper_LVL_NPC()
{}

/***
Level NPC class, functions, and callback events
@module LevelNpcClass
*/

/***
NPC-AI Controller base class, must be implemented for every NPC which needs more complex logic than engine provides
@type NpcControllerBase
*/

/***
Controller constructor, once called on first initialization of NPC-AI controller.<br>
<b>Required to be defined in an NPC-AI controller class!</b>
@function __init
@tparam BaseNPC npc_obj Reference to actual NPC object

@usage
class 'MyNPC'

function MyNPC:initPros()
    self.timer = 0
    self.state = 0
    -- ...
end

function MyNPC:__init(npc_obj)
    self.npc_obj = npc_obj
    self:initProps()
end

-- ...

return MyNPC
*/


/***
Event callback called on every activation of NPC when it appears on player's camera.
@function onActivated

@usage
class 'MyNPC'

function MyNPC:initPros()
    self.timer = 0
    self.state = 0
    -- ...
end

function MyNPC:onActivated()
    MyNPC:initPros()
end

-- ...

return MyNPC
*/

void Binding_Level_ClassWrapper_LVL_NPC::lua_onActivated()
{
    if(!LuaGlobal::getEngine(mself.ref(*this).state())->shouldShutdown())
        call<void>("onActivated");
}

/***
Event callback calling per every frame
@function onLoop
@tparam double frameDelay Frame delay in milliseconds. Use it for various timing processors.

@usage
class 'MyNPC'

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

function MyNPC:onLoop(frameDelay)
    ticker(frameDelay)
end

return MyNPC
*/
void Binding_Level_ClassWrapper_LVL_NPC::lua_onLoop(double tickTime)
{
    if(!LuaGlobal::getEngine(mself.ref(*this).state())->shouldShutdown())
        call<void>("onLoop", tickTime);
}

void Binding_Level_ClassWrapper_LVL_NPC::lua_onInit()
{
    if(!LuaGlobal::getEngine(mself.ref(*this).state())->shouldShutdown())
        call<void>("onInit");
}

/***
Event callback calling on attempt to kill NPC
@function onKill
@tparam NpcKillEvent killEvent Kill event context. Can be used to identify reasons and gives ability to reject this event.

@usage
class 'MyNPC'

function MyNPC:onKill(killEvent)
    -- NPC is invincable to everything except of lava
    if(killEvent.reason_code ~= BaseNPC.DAMAGE_LAVABURN)then
        killEvent.cancel = true
    end
end

return MyNPC
*/
void Binding_Level_ClassWrapper_LVL_NPC::lua_onKill(KillEvent *killEvent)
{
    if(!LuaGlobal::getEngine(mself.ref(*this).state())->shouldShutdown())
        call<void>("onKill", killEvent);
}

/***
Event callback calling on attempt to hurt NPC
@function onHarm
@tparam NpcHarmEvent harmEvent Harm event context. Can be used to identify reasons and gives ability to reject this event.

@usage
class 'MyNPC'

function MyNPC:onHarm(harmEvent)
    -- Say "Ouch" when NPC was kicked to ass, but don't harm NPC
    if(harmEvent.reason_code == BaseNPC.DAMAGE_BY_KICK)then
        Audio.playSound(5);
        harmEvent.cancel = true
    end
end

return MyNPC
*/
void Binding_Level_ClassWrapper_LVL_NPC::lua_onHarm(HarmEvent *harmEvent)
{
    if(!LuaGlobal::getEngine(mself.ref(*this).state())->shouldShutdown())
        call<void>("onHarm", harmEvent);
}

/***
Event callback calling on attempt to transform NPC into another
@function onTransform
@tparam ulong id Destinition ID to transform this NPC

@usage
class 'MyNPC'

function MyNPC:onTransform(id)
    -- Stop free motion of NPC on attempt to transform it
    self.npc_obj.speedX = 0
end

return MyNPC
*/
void Binding_Level_ClassWrapper_LVL_NPC::lua_onTransform(unsigned long id)
{
    if(!LuaGlobal::getEngine(mself.ref(*this).state())->shouldShutdown())
        call<void>("onTransform", id);
}

luabind::scope Binding_Level_ClassWrapper_LVL_NPC::bindToLua()
{
    using namespace luabind;
    return class_<LVL_Npc,
                  PGE_Phys_Object,
                  detail::null_type,
                  Binding_Level_ClassWrapper_LVL_NPC>("BaseNPC")
            /***
            Damage reasons enumeration (DamageReason)
            @section DamageReason
            */
            .enum_("DamageReason")
            [
                /***
                No reason of damage
                @field BaseNPC.DAMAGE_NOREASON
                */
                value("DAMAGE_NOREASON", LVL_Npc::DamageReason::DAMAGE_NOREASON),
                /***
                NPC was stomped
                @field BaseNPC.DAMAGE_STOMPED
                */
                value("DAMAGE_STOMPED", LVL_Npc::DamageReason::DAMAGE_STOMPED),
                /***
                NPC was kicked
                @field BaseNPC.DAMAGE_BY_KICK
                */
                value("DAMAGE_BY_KICK", LVL_Npc::DamageReason::DAMAGE_BY_KICK),
                /***
                NPC was attacked by player
                @field BaseNPC.DAMAGE_BY_PLAYER_ATTACK
                */
                value("DAMAGE_BY_PLAYER_ATTACK", LVL_Npc::DamageReason::DAMAGE_BY_PLAYER_ATTACK),
                /***
                NPC was taken by player (for example, NPC is a coin, item, weapon, power-up, etc.)
                @field BaseNPC.DAMAGE_TAKEN
                */
                value("DAMAGE_TAKEN", LVL_Npc::DamageReason::DAMAGE_TAKEN),
                /***
                NPC was burn in lava
                @field BaseNPC.DAMAGE_LAVABURN
                */
                value("DAMAGE_LAVABURN", LVL_Npc::DamageReason::DAMAGE_LAVABURN),
                /***
                NPC fell into the pit
                @field BaseNPC.DAMAGE_PITFALL
                */
                value("DAMAGE_PITFALL", LVL_Npc::DamageReason::DAMAGE_PITFALL),
                /***
                Custom reason base, use it as modifier to any custom damage reasons
                @field BaseNPC.DAMAGE_CUSTOM_REASON
                */
                value("DAMAGE_CUSTOM_REASON", LVL_Npc::DamageReason::DAMAGE_CUSTOM_REASON)
            ]

            /***
            Warping side enumeration (WarpingSide)
            @section WarpingSide
            */
            .enum_("WarpingSide")
            [
                /***
                NPC is in warping process and points top direction
                @field BaseNPC.WARP_TOP
                */
                value("WARP_TOP", LVL_Npc::WarpingSide::WARP_TOP),
                /***
                NPC is in warping process and points left direction
                @field BaseNPC.WARP_LEFT
                */
                value("WARP_LEFT", LVL_Npc::WarpingSide::WARP_LEFT),
                /***
                NPC is in warping process and points bottom direction
                @field BaseNPC.WARP_BOTTOM
                */
                value("WARP_BOTTOM", LVL_Npc::WarpingSide::WARP_BOTTOM),
                /***
                NPC is in warping process and points right direction
                @field BaseNPC.WARP_RIGHT
                */
                value("WARP_RIGHT", LVL_Npc::WarpingSide::WARP_RIGHT)
            ]

            /***
            Spawning algorithms enumeration (SpawnType)
            @section SpawnType
            */
            .enum_("SpawnType")
            [
                /***
                Instant appearence with no effects
                @field BaseNPC.SPAWN_APPEAR
                */
                value("SPAWN_APPEAR", 0),
                /***
                Smooth appearence (for example, appearence from under ground)
                @field BaseNPC.SPAWN_WARP
                */
                value("SPAWN_WARP", 1),
                /***
                Projectile shoot, NPC will appear instantly with initial speed and smoke effect
                @field BaseNPC.SPAWN_PROJECTILE
                */
                value("SPAWN_PROJECTILE", 2)
            ]

            /***
            Spawning direction enumeration (SpawnDirection)
            @section SpawnType
            */
            .enum_("SpawnDirection")
            [
                /***
                Spawn NPC with left direction
                @field BaseNPC.SPAWN_LEFT
                */
                value("SPAWN_LEFT", 2),
                /***
                Spawn NPC with right direction
                @field BaseNPC.SPAWN_RIGHT
                */
                value("SPAWN_RIGHT", 4),
                /***
                Spawn NPC with up direction
                @field BaseNPC.SPAWN_UP
                */
                value("SPAWN_UP", 1),
                /***
                Spawn NPC with down direction
                @field BaseNPC.SPAWN_DOWN
                */
                value("SPAWN_DOWN", 3)
            ]

            /***
            Non-Playable Character Object base class, inherited from @{PhysBaseClass.PhysBase}
            @type BaseNPC
            */
            .def(constructor<>())

            //Base-class Events (not same as Controller events, there are calling after controller event will be called)
            .def("onActivated", &LVL_Npc::lua_onActivated, &Binding_Level_ClassWrapper_LVL_NPC::def_lua_onActivated)
            .def("onLoop", &LVL_Npc::lua_onLoop, &Binding_Level_ClassWrapper_LVL_NPC::def_lua_onLoop)
            .def("onInit", &LVL_Npc::lua_onInit, &Binding_Level_ClassWrapper_LVL_NPC::def_lua_onInit)
            .def("onKill", &LVL_Npc::lua_onKill, &Binding_Level_ClassWrapper_LVL_NPC::def_lua_onKill)
            .def("onHarm", &LVL_Npc::lua_onHarm, &Binding_Level_ClassWrapper_LVL_NPC::def_lua_onHarm)
            .def("onTransform", &LVL_Npc::lua_onTransform, &Binding_Level_ClassWrapper_LVL_NPC::def_lua_onTransform)

            //Properties

            /***
            ID of NPC registered in config pack (Read Only)
            @tfield ulong id
            */
            .property("id", &LVL_Npc::getID)
            /***
            Face direction of NPC
            @tfield int direction -1 is faced left, +1 is faced right, 0 is randomly selected drection (when you setting it up)
            */
            .property("direction", &LVL_Npc::direction, &LVL_Npc::setDirection)
            /***
            NPC will don't move horizontally
            @tfield bool not_movable
            */
            .property("not_movable", &LVL_Npc::not_movable, &LVL_Npc::setNot_movable)
            /***
            ID of NPC which contains inside this NPC
            @tfield long contents
            */
            .property("contents", &LVL_Npc::contents, &LVL_Npc::setContents)
            /***
            Special value of NPC from level file entry
            @tfield long special1
            */
            .property("special1", &LVL_Npc::special1, &LVL_Npc::setSpecial1)
            /***
            Special value of NPC from level file entry
            @tfield long special2
            */
            .property("special2", &LVL_Npc::special2, &LVL_Npc::setSpecial2)
            /***
            Is this NPC an important boss (a flag from level file)
            @tfield bool isBoss
            */
            .property("isBoss", &LVL_Npc::isBoss)
            /***
            Can NPC collide blocks?
            @tfield bool collideWithBlocks
            */
            .property("collideWithBlocks", &LVL_Npc::getCollideWithBlocks, &LVL_Npc::setCollideWithBlocks)
            /***
            Can NPC collide playable character?
            @tfield bool collideWithPlayers
            */
            .def_readwrite("collideWithPlayers", &LVL_Npc::enablePlayerCollision)
            /***
            Health level of NPC
            @tfield long health
            */
            .property("health", &LVL_Npc::getHealth, &LVL_Npc::setHealth)
            /***
            Animation frame delay
            @tfield int frameDelay
            */
            .property("frameDelay", &LVL_Npc::lua_frameDelay, &LVL_Npc::lua_setFrameDelay)

            //Parameters

            /***
            Is NPC vulnuable to fire attacks (Read Only)
            @tfield bool killableByFire
            */
            .def_readonly("killableByFire", &LVL_Npc::getKillByFire)
            /***
            Is NPC vulnuable to ice/cold attacks (Read Only)
            @tfield bool killableByIce
            */
            .def_readonly("killableByIce", &LVL_Npc::getKillByIce)
            /***
            Is NPC vulnuable to hammer attacks (Read Only)
            @tfield bool killableByHammer
            */
            .def_readonly("killableByHammer", &LVL_Npc::getKillByHammer)
            /***
            Is NPC is vulnuable from a power jumps (Read Only)
            @tfield bool killableByForcejump
            */
            .def_readonly("killableByForcejump", &LVL_Npc::getKillByForcejump)
            /***
            Is NPC is vulnuable from a statue fell (Read Only)
            @tfield bool killableByStatue
            */
            .def_readonly("killableByStatue", &LVL_Npc::getKillByStatue)
            /***
            Is NPC is vulnuable from attacks by vehicles (Read Only)
            @tfield bool killableByVehicle
            */
            .def_readonly("killableByVehicle", &LVL_Npc::getKillByVehicle)

            //States

            /***
            Is NPC stays on a solid surface (Read Only)
            @tfield bool onGround
            */
            .def_readonly("onGround", &LVL_Npc::onGround)
            /***
            Self-motion speed of NPC in pixels per 1/65 of second (Read Only)
            @tfield double motionSpeed
            */
            .def_readwrite("motionSpeed", &LVL_Npc::motionSpeed)
            /***
            Is NPC stays on an edge of ground where it stays (Read Only)
            @tfield bool onCliff
            */
            .def_readonly("animationIsFinished", &LVL_Npc::lua_animationIsFinished)
            /***
            Is once-playing animation was finished (Read Only)
            @tfield bool animationIsFinished
            */
            .def_readonly("onCliff", &LVL_Npc::onCliff)

            /***
            Transform NPC into another NPC
            @function transformTo
            @tparam ulong id ID of NPC registered in the config pack
            */

            /***
            Transform NPC into another NPC or into the block
            @function transformTo
            @tparam ulong id ID of NPC or Block registered in the config pack
            @tparam int type Type of object to transform NPC: 1 - Another NPC, 2 - Block
            */
            .def("transformTo", &LVL_Npc::transformTo)
            /***
            Original ID of block from which NPC was transformed (Read Only)
            @tfield ulong transformedFromBlockID If 0, NPC was not transformed from block
            */
            .def_readonly("transformedFromBlockID", &LVL_Npc::transformedFromBlockID)
            /***
            Original ID of NPC from which NPC was transformed (Read Only)
            @tfield ulong transformedFromNpcID If 0, NPC was not transformed from another NPC
            */
            .def_readonly("transformedFromNpcID", &LVL_Npc::transformedFromNpcID)
            /***
            Type of spawn algorithm originally produced this NPC (Read Only)
            @tfield SpawnType spawnedGenType
            */
            .def_readonly("spawnedGenType", &LVL_Npc::m_spawnedGeneratorType)
            /***
            Direction of NPC when it was originally spawned (Read Only)
            @tfield SpawnDirection spawnedGenDirection
            */
            .def_readonly("spawnedGenDirection", &LVL_Npc::m_spawnedGeneratorDirection)

            //Functions

            /***
            Sets body type
            @function setBodyType
            @tfield bool isStatic NPC will act as static body like a block
            @tfield bool isSticky NPC will be a child of Layer's group and will take all transforms of it
            */
            .def("setBodyType", &LVL_Npc::setBodyType)

            /***
            Set custom animation sequence for left face direction
            @function setSequenceLeft
            @tparam table frames Array of frame indeces integers
            */
            .def("setSequenceLeft", &LVL_Npc::lua_setSequenceLeft)

            /***
            Set custom animation sequence for right face direction
            @function setSequenceRight
            @tparam table frames Array of frame indeces integers
            */
            .def("setSequenceRight", &LVL_Npc::lua_setSequenceRight)

            /***
            Set custom common animation sequence for all face directions
            @function setSequence
            @tparam table frames Array of frame indeces integers
            */
            .def("setSequence", &LVL_Npc::lua_setSequence)

            /***
            Turn on once animation mode when changed animation sequence will play once after set
            @function setOnceAnimationMode
            @tparam bool enabled If true, once animation mode is turned on
            */
            .def("setOnceAnimationMode", &LVL_Npc::lua_setOnceAnimation)

            /***
            Set custom GFX offset
            @function setGfxOffset
            @tparam double x Horizontal offset
            @tparam double y Vertical offset
             */
            .def("setGfxOffset", &LVL_Npc::lua_setGfxOffset)

            /***
            Set custom horizontal GFX offset
            @function setGfxOffset
            @tparam double x Horizontal offset
             */
            .def("setGfxOffsetX", &LVL_Npc::lua_setGfxOffsetX)

            /***
            Set custom vertical GFX offset
            @function setGfxOffset
            @tparam double y Vertical offset
             */
            .def("setGfxOffsetY", &LVL_Npc::lua_setGfxOffsetY)

            /***
            Install an In-Area detector for relative area and get a referrence to it,
            boundary coordinates of a trap area are relative to center point of NPC
            @function installInAreaDetector
            @tparam double left Left border offset of the trap area
            @tparam double top Top border offset of the trap area
            @tparam double right Right border offset of the trap area
            @tparam double bottom Bottom border offset of the trap area
            @tparam table filters Array of integers are identifying object types needed to detect:<br>
            <ul>
                <li>1 - blocks</li>
                <li>2 - BGOs</li>
                <li>3 - NPCs</li>
                <li>4 - Playable characters</li>
            </ul>
            @treturn InAreaDetector Reference to initialized In-Area detector
            */
            .def("installInAreaDetector", &LVL_Npc::lua_installInAreaDetector)

            /***
            Install a detector of capabilities and state of a nearest playable character ("Player Position" detector)
            @function installPlayerPosDetector
            @treturn PlayerPosDetector Reference to initialized "Player Position" detector
            */
            .def("installPlayerPosDetector", &LVL_Npc::lua_installPlayerPosDetector)

            /***
            Install a detector of contacted objects which will detect any objects contacted this NPC
            @function installContactDetector
            @treturn ContactDetector Reference to initialized contacts detector
            */
            .def("installContactDetector", &LVL_Npc::lua_installContactDetector)

            /***
            Spawn another NPC on same position as this NPC.
            Spawned NPC will be destroyed on off screen timeout
            @function spawnNPC
            @tparam ulong npcID ID of NPC registered in config pack
            @tparam int spawnType Spawn algorithm
            @tparam int direction Spawn direction
            @treturn BaseNPC NPC object of spawned NPC. nil if error has occouped to spawn NPC
            */

            /***
            Spawn another NPC on same position as this NPC.
            Spawned NPC will be destroyed on off screen timeout if <i>reSpawnable</i> flag is false.
            If <i>reSpawnable</i> flag is true, NPC will be deactivated and will be activated on next
            appearence on screen like all other regular NPCs.
            @function spawnNPC
            @tparam ulong npcID ID of NPC registered in config pack
            @tparam int spawnType Spawn algorithm
            @tparam int direction Spawn direction
            @tparam bool reSpawnable NPC will be kept on a level after going off screen. On false, NPC will be destroyed after off screen timeout
            @treturn BaseNPC NPC object of spawned NPC. nil if error has occouped to spawn NPC
            */
            .def("spawnNPC", &LVL_Npc::lua_spawnNPC)

            /***
            Set cut off level of the sprite
            @function setSpriteWarp
            @tparam float depth Warping depth between 1.0 and 0.0. 0 is fully visible, 1.0 is fully invisible
            @tparam WarpingSide direction Direction of "head" which will be shown while body is going into hide
            @tparam bool resizedBody Don't move sprite down to feet, use it when sprite warping is used together with physical body resizing
            */
            .def("setSpriteWarp", &LVL_Npc::setSpriteWarp)

            /***
            Disables sprite cut off and resets it to normal state
            @function resetSpriteWarp
            */
            .def("resetSpriteWarp", &LVL_Npc::resetSpriteWarp)

            /***
            Force activate nearest NPCs are touching this NPC are even not touching player's camera
            @function activateNeighbours
            */
            .def("activateNeighbours", &LVL_Npc::lua_activate_neighbours)

            /***
            Destroy NPC with no effects and no events
            @function unregister
            */
            .def("unregister", &LVL_Npc::unregister)

            /***
            Kill this NPC with damage reason
            @function kill
            @tparam DamageReason damageReason Code of death reason
            */
            .def("kill", (void(LVL_Npc::*)(int))&LVL_Npc::kill)

            /***
            Kill this NPC with damage reason and telling who killed this NPC
            @function kill
            @tparam DamageReason damageReason Code of death reason
            @tparam PhysBaseClass.PhysBase killedBy Reference to person object (playable character or NPC) who killed this NPC
            */
            .def("kill", (void(LVL_Npc::*)(int, PGE_Phys_Object*))&LVL_Npc::kill)

            /***
            Harm this NPC with removing one health point
            @function harm
            */
            .def("harm", (void(LVL_Npc::*)())&LVL_Npc::harm)

            /***
            Harm this NPC with removing given health point
            @function harm
            @tparam int damageLevel Count of health point to remove
            */
            .def("harm", (void(LVL_Npc::*)(int))&LVL_Npc::harm)

            /***
            Harm this NPC with removing given health point and reason of damage
            @function harm
            @tparam int damageLevel Count of health point to remove
            @tparam DamageReason damageReason Code of damage reason
            */
            .def("harm", (void(LVL_Npc::*)(int, int))&LVL_Npc::harm)

            /***
            Harm this NPC with removing given health point, reason of damage and person who damaged this NPC
            @function harm
            @tparam int damageLevel Count of health point to remove
            @tparam DamageReason damageReason Code of damage reason
            @tparam PhysBaseClass.PhysBase harmBy Reference to person object (playable character or NPC) who harmed this NPC
            */
            .def("harm", (void(LVL_Npc::*)(int, int, PGE_Phys_Object*))&LVL_Npc::harm)
            ;
}

luabind::scope Binding_Level_ClassWrapper_LVL_NPC::HarmEvent_bindToLua()
{
    using namespace luabind;
    return
        class_<LVL_Npc::HarmEvent>("NpcHarmEvent")
        /***
        Damaged By enumeration (killedBy)
        @section DamagedBy
        */
        .enum_("killedBy")
        [
            /***
            NPC made damage to itself
            @field NpcHarmEvent.self
            */
            value("self", LVL_Npc::HarmEvent::killedBy::self),
            /***
            NPC was damaged by playable character
            @field NpcHarmEvent.player
            */
            value("player", LVL_Npc::HarmEvent::killedBy::player),
            /***
            NPC was damaged by another NPC
            @field NpcHarmEvent.otherNPC
            */
            value("otherNPC", LVL_Npc::HarmEvent::killedBy::otherNPC)
        ]

        /***
        NPC Harm Event context
        @type NpcHarmEvent
        */
        .def(constructor<>())
        /***
        Reject this event, it will not be executed
        @tfield bool cancel
        */
        .def_readwrite("cancel", &LVL_Npc::HarmEvent::cancel)
        /***
        Damage level caused by this event
        @tfield int damage
        */
        .def_readwrite("damage", &LVL_Npc::HarmEvent::damage)
        /***
        Damage reason code
        @tfield DamageReason reason_code
        */
        .def_readwrite("reason_code", &LVL_Npc::HarmEvent::reason_code)
        /***
        Who damaged this NPC
        @tfield killedBy killed_by
        */
        .def_readwrite("killed_by", &LVL_Npc::HarmEvent::killed_by)
        /***
        Playable character who damaged this NPC or nil if NPC was not damaged by playable character
        @tfield BasePlayer killer_p
        */
        .def_readwrite("killer_p", &LVL_Npc::HarmEvent::killer_p)
        /***
        Another NPC who damaged this NPC or nil if NPC was not damaged by other NPC
        @tfield BaseNPC killer_n
        */
        .def_readwrite("killer_n", &LVL_Npc::HarmEvent::killer_n);
}

luabind::scope Binding_Level_ClassWrapper_LVL_NPC::KillEvent_bindToLua()
{
    using namespace luabind;
    return
        class_<LVL_Npc::KillEvent>("NpcKillEvent")
        /***
        Killed By enumeration (killedBy)
        @section KilledBy
        */
        .enum_("killedBy")
        [
            /***
            NPC made damage to itself
            @field NpcHarmEvent.self
            */
            value("self", LVL_Npc::KillEvent::killedBy::self),
            /***
            NPC was damaged by playable character
            @field NpcHarmEvent.player
            */
            value("player", LVL_Npc::KillEvent::killedBy::player),
            /***
            NPC was damaged by another NPC
            @field NpcHarmEvent.otherNPC
            */
            value("otherNPC", LVL_Npc::KillEvent::killedBy::otherNPC)
        ]

        /***
        NPC Kill Event context
        @type NpcKillEvent
        */
        .def(constructor<>())
        /***
        Reject this event, it will not be executed
        @tfield bool cancel
        */
        .def_readwrite("cancel", &LVL_Npc::KillEvent::cancel)
        /***
        Death reason code
        @tfield DamageReason reason_code
        */
        .def_readwrite("reason_code", &LVL_Npc::KillEvent::reason_code)
        /***
        Who killed this NPC
        @tfield killedBy killed_by
        */
        .def_readwrite("killed_by", &LVL_Npc::KillEvent::killed_by)
        /***
        Playable character who killed this NPC or nil if NPC was not killed by playable character
        @tfield BasePlayer killer_p
        */
        .def_readwrite("killer_p", &LVL_Npc::KillEvent::killer_p)
        /***
        Another NPC who killed this NPC or nil if NPC was not killed by other NPC
        @tfield BaseNPC killer_n
        */
        .def_readwrite("killer_n", &LVL_Npc::KillEvent::killer_n);
}
