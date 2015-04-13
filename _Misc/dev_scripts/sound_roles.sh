#!/bin/bash

sRole()
{
a=$1
	#echo "main_sound_table[obj_sound_role::$1]=soundset.value(\"${a,,}\", 0).toInt();"
	echo "${a,,}=0"
}

echo "[sound-roles]"

sRole MenuDo
sRole MenuScroll
sRole MenuMessageBox
sRole MenuPause

sRole CameraSwitch

sRole PlayerJump
sRole PlayerStomp
sRole PlayerShrink
sRole PlayerGrow
sRole PlayerDied
sRole PlayerDropItem
sRole PlayerTakeItem
sRole PlayerSlide
sRole PlayerGrab1
sRole PlayerGrab2
sRole PlayerSpring
sRole PlayerClimb
sRole PlayerTail
sRole PlayerMagic

sRole BonusCoin
sRole Bonus1up

sRole WeaponHammer
sRole WeaponFire
sRole WeaponCannon
sRole WeaponExplosion
sRole WeaponBigFire

sRole NpcLavaBurn
sRole NpcStoneFall

sRole WarpPipe
sRole WarpDoor
sRole WarpTeleport

sRole LevelFailed

sRole LevelCheckPoint

sRole WorldMove
sRole WorldDeny
sRole WorldOpenPath
sRole WorldEnterLevel

sRole LevelExit01
sRole LevelExit02
sRole LevelExit03
sRole LevelExit04
sRole LevelExit05
sRole LevelExit06
sRole LevelExit07
sRole LevelExit08
sRole LevelExit09
sRole LevelExit10

sRole GameCompleted

sRole BlockHit
sRole BlockOpen
sRole BlockSmashed
sRole BlockSwitch

read -n 1

