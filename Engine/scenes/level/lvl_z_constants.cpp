/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "../scene_level.h"

/**************Z-Layers**************/
const long double LevelScene::Z_backImage =     -1000.0L; //Background

//Background-2
const long double LevelScene::Z_BGOBack2 =      -160.0L; // backround BGO

const long double LevelScene::Z_blockSizable =  -150.0L; // sizable blocks

//Background-1
const long double LevelScene::Z_BGOBack1 =      -100.0L; // backround BGO

const long double LevelScene::Z_npcBack =       -10.0L; // background NPC
const long double LevelScene::Z_Block =         1.0L; // standart block
const long double LevelScene::Z_npcStd =        30.0L; // standart NPC
const long double LevelScene::Z_Player =        35.0L; //player Point

//Foreground-1
const long double LevelScene::Z_BGOFore1 =      50.0L; // foreground BGO
const long double LevelScene::Z_BlockFore =     100.0L; //LavaBlock
const long double LevelScene::Z_npcFore =       150.0L; // foreground NPC
//Foreground-2
const long double LevelScene::Z_BGOFore2 =      160.0L; // foreground BGO

const long double LevelScene::Z_sys_PhysEnv =   500.0L;
const long double LevelScene::Z_sys_door =      700.0L;
const long double LevelScene::Z_sys_interspace1 = 1000.0L; // interSection space layer
const long double LevelScene::Z_sys_sctBorder = 1020.0L; // section Border

