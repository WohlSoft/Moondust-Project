/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2025 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef OBJ_SOUND_ROLES_H
#define OBJ_SOUND_ROLES_H

///
/// \brief The obj_sound_role struct
///
/// Sound roles usually using by engine internally
/// Each sound role associating with one of sound ID's (some roles can have same sound ID)
///
struct obj_sound_role
{
    // Note to the devs: Please updating Binding_Core_GlobalFuncs_Audio::bindConstants
    // When updating this enum!
    enum roles
    {
        Greeting=0,

        MenuDo=0,
        MenuScroll,
        MenuMessageBox,
        MenuPause,

        CameraSwitch,

        PlayerJump,  //Hey, i'm flying in air! :D
        PlayerStomp, //Pha!
        PlayerKick,
        PlayerShrink,//Ouch!
        PlayerHarm, //Ouch!
        PlayerGrow,  //from small to big
        PlayerDied,  //player died 2
        PlayerDropItem, //Drop reserve item
        PlayerTakeItem, //Take item
        PlayerSlide, //skid
        PlayerGrab1,
        PlayerGrab2,
        PlayerSpring,
        PlayerClimb,
        PlayerTail,
        PlayerMagic,
        PlayerWaterSwim,

        BonusCoin,
        Bonus1up,

        WeaponHammer,
        WeaponFire, // fireball / iceball
        WeaponCannon, // generators / big bullet
        WeaponExplosion,
        WeaponBigFire,

        NpcLavaBurn,
        NpcStoneFall,
        NpcHit,
        NpcDeath,

        WarpPipe,
        WarpDoor,
        WarpTeleport,//use racoon sound!

        LevelFailed, //player died 1

        LevelCheckPoint,

        WorldMove,
        WorldDeny,      //Not allowed movement
        WorldOpenPath,  //Open paths
        WorldEnterLevel,//Enter into level

        LevelExit01,  // reserved
        LevelExit02,  // card roulette
        LevelExit03,  // dungeon ball
        LevelExit04,  // reserved
        LevelExit05,  // key hole
        LevelExit06,  // crystal spshare
        LevelExit07,  // reserved
        LevelExit08,  // tape
        LevelExit09,  // reserved
        LevelExit10,  // reserved

        GameCompleted,

        BlockHit,
        BlockOpen,    //block with NPC
        BlockSmashed,
        BlockSwitch,

        __begin=Greeting,
        __end=BlockSwitch
    };
};

#endif // OBJ_SOUND_ROLES_H

