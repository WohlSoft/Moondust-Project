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

#include "luafuncs_core_audio.h"

#include <audio/pge_audio.h>

void Binding_Core_GlobalFuncs_Audio::playSound(size_t soundID)
{
    PGE_Audio::playSound(soundID);
}

void Binding_Core_GlobalFuncs_Audio::playSoundByRole(int role)
{
    PGE_Audio::playSoundByRole(static_cast<obj_sound_role::roles>(role));
}

/***
Sound and music functions
@module AudioFuncs
*/

luabind::scope Binding_Core_GlobalFuncs_Audio::bindToLua()
{
    using namespace luabind;
    return
        namespace_("Audio")[
            /***
            Sound effect functions
            @section SfxFuncs
            */

            /***
            Play sound by ID of registered sound effects in the config pack
            @function Audio.playSound
            @tparam ulong soundID ID of sound registered in the config pack
            */
            def("playSound", &Binding_Core_GlobalFuncs_Audio::playSound),

            /***
            Play sound by embedded role of game engine
            @function Audio.playSoundByRole
            @tparam SoundRoles role Role type of sound
            */
            def("playSoundByRole", &Binding_Core_GlobalFuncs_Audio::playSoundByRole)
            ];
}

void Binding_Core_GlobalFuncs_Audio::bindConstants(lua_State *L)
{
    luabind::object _G = luabind::globals(L);
    if(luabind::type(_G["SoundRoles"]) != LUA_TNIL)
        return;

    /***
    Enums
    @section SfxEnums
    */

    /***
    Built-in sound roles of game engine enumeration
    @enum SoundRoles

    @field Greeting Starts on initial game loading

    @field MenuDo Menu action sound
    @field MenuScroll Menu scrolling sound
    @field MenuMessageBox Message box pop-up sound
    @field MenuPause Pause menu show and hide sound

    @field CameraSwitch Camera mode switch sound

    @field PlayerJump Player jump sound
    @field PlayerStomp Player's stomp sound
    @field PlayerKick Player's kick sound
    @field PlayerShrink Player's shrink sound
    @field PlayerHarm Player's harm sound
    @field PlayerGrow Player's growl sound
    @field PlayerDied Player's death sound
    @field PlayerDropItem Player's item drop sound
    @field PlayerTakeItem Player's item taking sound
    @field PlayerSlide Player's sliding sound
    @field PlayerGrab1 Player's grab sound 1
    @field PlayerGrab2 Player's grab sound 2
    @field PlayerSpring Spring sound
    @field PlayerClimb Player's climbing sound
    @field PlayerTail Player's whip sound
    @field PlayerMagic Player's magic spell sound
    @field PlayerWaterSwim Player's swimming sound

    @field BonusCoin Coin taking sound
    @field Bonus1up Got a new live/attempt sound

    @field WeaponHammer Hammer attack sound
    @field WeaponFire Fire shoot sound
    @field WeaponCannon Cannon shoot sound
    @field WeaponExplosion Explosion sound
    @field WeaponBigFire Big fire shoot sound

    @field NpcLavaBurn Lava birning sound
    @field NpcStoneFall Stone fall sound
    @field NpcHit NPC's standard hit sound
    @field NpcDeath NPC's standard death sound

    @field WarpPipe Pipe enter/exit sound
    @field WarpDoor Door enter/exit sound
    @field WarpTeleport Teleport sound

    @field LevelFailed Failed level passing (all playable characters are dead)
    @field PlayersDead Alias to LevelFailed

    @field LevelCheckPoint Checkpoint save sound

    @field WorldMove Moving on world map sound
    @field WorldDeny Denied move sound
    @field WorldOpenPath Opened path cell sound
    @field WorldEnterLevel Level entering sound

    @field LevelExit01 Level exit of code 1 sound
    @field LevelExit02 Level exit of code 2 sound
    @field LevelExit03 Level exit of code 3 sound
    @field LevelExit04 Level exit of code 4 sound
    @field LevelExit05 Level exit of code 5 sound
    @field LevelExit06 Level exit of code 6 sound
    @field LevelExit07 Level exit of code 7 sound
    @field LevelExit08 Level exit of code 8 sound
    @field LevelExit09 Level exit of code 9 sound
    @field LevelExit10 Level exit of code 10 sound

    @field GameCompleted Game completion theme sound

    @field BlockHit Block hit/beat sound
    @field BlockOpen Open contents of block
    @field BlockSmashed Smashed/broken block sound
    @field BlockSwitch Switch toggle sound
    */
    luabind::object SoundRoles = luabind::newtable(L);

    SoundRoles["Greeting"] = obj_sound_role::Greeting;
    SoundRoles["MenuDo"] = obj_sound_role::MenuDo;
    SoundRoles["MenuScroll"] = obj_sound_role::MenuScroll;
    SoundRoles["MenuMessageBox"] = obj_sound_role::MenuMessageBox;
    SoundRoles["MenuPause"] = obj_sound_role::MenuPause;

    SoundRoles["CameraSwitch"] = obj_sound_role::CameraSwitch;

    SoundRoles["PlayerJump"] = obj_sound_role::PlayerJump;
    SoundRoles["PlayerStomp"] = obj_sound_role::PlayerStomp;
    SoundRoles["PlayerKick"] = obj_sound_role::PlayerKick;
    SoundRoles["PlayerShrink"] = obj_sound_role::PlayerShrink;
    SoundRoles["PlayerHarm"] = obj_sound_role::PlayerHarm;
    SoundRoles["PlayerGrow"] = obj_sound_role::PlayerGrow;
    SoundRoles["PlayerDied"] = obj_sound_role::PlayerDied;
    SoundRoles["PlayerDropItem"] = obj_sound_role::PlayerDropItem;
    SoundRoles["PlayerTakeItem"] = obj_sound_role::PlayerTakeItem;
    SoundRoles["PlayerSlide"] = obj_sound_role::PlayerSlide;
    SoundRoles["PlayerGrab1"] = obj_sound_role::PlayerGrab1;
    SoundRoles["PlayerGrab2"] = obj_sound_role::PlayerGrab2;
    SoundRoles["PlayerSpring"] = obj_sound_role::PlayerSpring;
    SoundRoles["PlayerClimb"] = obj_sound_role::PlayerClimb;
    SoundRoles["PlayerTail"] = obj_sound_role::PlayerTail;
    SoundRoles["PlayerMagic"] = obj_sound_role::PlayerMagic;
    SoundRoles["PlayerWaterSwim"] = obj_sound_role::PlayerWaterSwim;

    SoundRoles["BonusCoin"] = obj_sound_role::BonusCoin;
    SoundRoles["Bonus1up"] = obj_sound_role::Bonus1up;

    SoundRoles["WeaponHammer"] = obj_sound_role::WeaponHammer;
    SoundRoles["WeaponFire"] = obj_sound_role::WeaponFire;
    SoundRoles["WeaponCannon"] = obj_sound_role::WeaponCannon;
    SoundRoles["WeaponExplosion"] = obj_sound_role::WeaponExplosion;
    SoundRoles["WeaponBigFire"] = obj_sound_role::WeaponBigFire;

    SoundRoles["NpcLavaBurn"] = obj_sound_role::NpcLavaBurn;
    SoundRoles["NpcStoneFall"] = obj_sound_role::NpcStoneFall;
    SoundRoles["NpcHit"] = obj_sound_role::NpcHit;
    SoundRoles["NpcDeath"] = obj_sound_role::NpcDeath;

    SoundRoles["WarpPipe"] = obj_sound_role::WarpPipe;
    SoundRoles["WarpDoor"] = obj_sound_role::WarpDoor;
    SoundRoles["WarpTeleport"] = obj_sound_role::WarpTeleport;

    SoundRoles["PlayersDead"] = obj_sound_role::LevelFailed;
    SoundRoles["LevelFailed"] = obj_sound_role::LevelFailed;

    SoundRoles["LevelCheckPoint"] = obj_sound_role::LevelCheckPoint;

    SoundRoles["WorldMove"] = obj_sound_role::WorldMove;
    SoundRoles["WorldDeny"] = obj_sound_role::WorldDeny;
    SoundRoles["WorldOpenPath"] = obj_sound_role::WorldOpenPath;
    SoundRoles["WorldEnterLevel"] = obj_sound_role::WorldEnterLevel;

    SoundRoles["LevelExit01"] = obj_sound_role::LevelExit01;
    SoundRoles["LevelExit02"] = obj_sound_role::LevelExit02;
    SoundRoles["LevelExit03"] = obj_sound_role::LevelExit03;
    SoundRoles["LevelExit04"] = obj_sound_role::LevelExit04;
    SoundRoles["LevelExit05"] = obj_sound_role::LevelExit05;
    SoundRoles["LevelExit06"] = obj_sound_role::LevelExit06;
    SoundRoles["LevelExit07"] = obj_sound_role::LevelExit07;
    SoundRoles["LevelExit08"] = obj_sound_role::LevelExit08;
    SoundRoles["LevelExit09"] = obj_sound_role::LevelExit09;
    SoundRoles["LevelExit10"] = obj_sound_role::LevelExit10;

    SoundRoles["GameCompleted"] = obj_sound_role::GameCompleted;

    SoundRoles["BlockHit"] = obj_sound_role::BlockHit;
    SoundRoles["BlockOpen"] = obj_sound_role::BlockOpen;
    SoundRoles["BlockSmashed"] = obj_sound_role::BlockSmashed;
    SoundRoles["BlockSwitch"] = obj_sound_role::BlockSwitch;

    _G["SoundRoles"] = SoundRoles;
}

