/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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
double LevelScene::zCounter=0;

const double LevelScene::Z_backImage = -1000; //Background

//Background-2
const double LevelScene::Z_BGOBack2 = -160; // backround BGO

const double LevelScene::Z_blockSizable = -150; // sizable blocks

//Background-1
const double LevelScene::Z_BGOBack1 = -100; // backround BGO

const double LevelScene::Z_npcBack = -10; // background NPC
const double LevelScene::Z_Block = 1; // standart block
const double LevelScene::Z_npcStd = 30; // standart NPC
const double LevelScene::Z_Player = 35; //player Point

//Foreground-1
const double LevelScene::Z_BGOFore1 = 50; // foreground BGO
const double LevelScene::Z_BlockFore = 100; //LavaBlock
const double LevelScene::Z_npcFore = 150; // foreground NPC
//Foreground-2
const double LevelScene::Z_BGOFore2 = 160; // foreground BGO

const double LevelScene::Z_sys_PhysEnv = 500;
const double LevelScene::Z_sys_door = 700;
const double LevelScene::Z_sys_interspace1 = 1000; // interSection space layer
const double LevelScene::Z_sys_sctBorder = 1020; // section Border

