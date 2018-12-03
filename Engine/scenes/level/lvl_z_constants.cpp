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

#include "lvl_z_constants.h"
#include "../scene_level.h"

LevelZOrderTable::LevelZOrderTable() :
    backImage(-1000.0L),    // Background

    //Background-2
    bgoBack2(-95.0),        // backround BGO
    blockBack2(-90.0),      // sizable blocks

    //Background-1
    bgoBack1(-85.0),        // backround BGO
    //-80.0 Special         BGO (i.e. locks) All special BGOs like locks.
    npcBack(-75.0),         // background NPC
    //-70.0 Held NPCs       NPCs which are held by the player.
    blockBack1(-65.0),      // standart block
    //-60.0 Effects         Some effects, which are on the background: doors, pressed p-switches, some other stuff
    //-55.0 NPC             NPCs like: coins, car, herb, wood rocket, boss fire
    //-50.0 NPC             NPCs like ice blocks.
    npcStd(-45.0),          // standart NPC
    //-40.0 Chat Icon       The chat icon, which is displayed, when you can talk with an NPC.
    //-35.0 Player Mounts   The player mounts: Catnips, Car, Sack
    //-30.0 Player Mounts?  Maybe is the same as -35.0
    player(-25.0),          // playable character

    //Foreground-1
    bgoFront1(-20.0),       // foreground BGO
    npcFront(-15.0),        // foreground NPC
    blockFront1(-10.0),     // foreground blocks
    //-5.0  Effect          All foreground Effects.

    //Foreground-2
    bgoFront2(0.0),         // foreground BGO (over everything)
    //+1.0  Image Draw      Render commands like Graphics.drawImage.
    //+2.0  Placed Sprites  Placed sprite by Autocode or LunaLua's Graphics.placeSprite.
    //+3.0  Text Draw       Text which is drawn by Autocode or LunaLua's Text.print.
    //+5.0  HUD             All elements of the HUD.

    debugFront(500.0L)     // debug render stuff
{}

const LevelZOrderTable LevelScene::zOrder;
