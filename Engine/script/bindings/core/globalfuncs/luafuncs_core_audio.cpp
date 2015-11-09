#include "luafuncs_core_audio.h"

#include <audio/pge_audio.h>

void Binding_Core_GlobalFuncs_Audio::playSound(long soundID)
{
    PGE_Audio::playSound(soundID);
}

void Binding_Core_GlobalFuncs_Audio::playSoundByRole(int role)
{
    PGE_Audio::playSoundByRole(static_cast<obj_sound_role::roles>(role));
}

luabind::scope Binding_Core_GlobalFuncs_Audio::bindToLua()
{
    using namespace luabind;
    return
        namespace_("Audio")[
            def("playSound", &Binding_Core_GlobalFuncs_Audio::playSound),
            def("playSoundByRole", &Binding_Core_GlobalFuncs_Audio::playSoundByRole)
            ];
}

void Binding_Core_GlobalFuncs_Audio::bindConstants(lua_State *L)
{
    luabind::object _G = luabind::globals(L);
    if(luabind::type(_G["SoundRoles"]) != LUA_TNIL)
        return;

    luabind::object SoundRoles = luabind::newtable(L);

    SoundRoles["Greeting"] = obj_sound_role::Greeting;

    SoundRoles["MenuDo"] = obj_sound_role::MenuDo;
    SoundRoles["MenuScroll"] = obj_sound_role::MenuScroll;
    SoundRoles["MenuMessageBox"] = obj_sound_role::MenuMessageBox;
    SoundRoles["MenuPause"] = obj_sound_role::MenuPause;

    SoundRoles["CameraSwitch"] = obj_sound_role::CameraSwitch;

    SoundRoles["PlayerJump"] = obj_sound_role::PlayerJump;
    SoundRoles["PlayerStomp"] = obj_sound_role::PlayerStomp;
    SoundRoles["PlayerShrink"] = obj_sound_role::PlayerShrink;
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

