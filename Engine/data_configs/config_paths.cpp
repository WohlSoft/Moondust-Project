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

#include "config_manager.h"

QString ConfigManager::PathLevelBGO()
{
    return bgoPath;
}

QString ConfigManager::PathLevelBG()
{
    return BGPath;
}

QString ConfigManager::PathLevelBlock()
{
    return blockPath;
}

QString ConfigManager::PathLevelNPC()
{
    return npcPath;
}

QString ConfigManager::PathLevelEffect()
{
    return effectPath;
}

QString ConfigManager::PathLevelPlayable()
{
    return playerLvlPath;
}

QString ConfigManager::PathCommonGFX()
{
    return commonGPath;
}


QString ConfigManager::PathWorldTiles()
{
    return tilePath;
}

QString ConfigManager::PathWorldScenery()
{
    return scenePath;
}

QString ConfigManager::PathWorldPaths()
{
    return pathPath;
}

QString ConfigManager::PathWorldLevels()
{
    return wlvlPath;
}

QString ConfigManager::PathWorldPlayable()
{
    return playerWldPath;
}

QString ConfigManager::PathWorldMusic()
{
    return dirs.music;
}

QString ConfigManager::PathWorldSound()
{
    return dirs.sounds;
}


void ConfigManager::refreshPaths()
{
    bgoPath =   dirs.glevel +  "background/";
    BGPath =    dirs.glevel +  "background2/";
    blockPath = dirs.glevel +  "block/";
    npcPath =   dirs.glevel +  "npc/";
    effectPath= dirs.glevel +  "effect/";
    playerLvlPath = dirs.gplayble;
    playerWldPath = dirs.gworld + "player/";

    tilePath =  dirs.gworld +  "tile/";
    scenePath = dirs.gworld +  "scene/";
    pathPath =  dirs.gworld +  "path/";
    wlvlPath =  dirs.gworld +  "level/";

    commonGPath = dirs.gcommon + "/";
}
