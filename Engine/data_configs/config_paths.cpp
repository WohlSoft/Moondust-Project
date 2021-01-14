/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "config_manager.h"
#include "config_manager_private.h"

std::string ConfigManager::bgoPath;
std::string ConfigManager::BGPath;
std::string ConfigManager::blockPath;
std::string ConfigManager::npcPath;
std::string ConfigManager::npcScriptPath;
std::string ConfigManager::effectPath;
std::string ConfigManager::playerLvlPath;
std::string ConfigManager::playerWldPath;
std::string ConfigManager::playerScriptPath;
std::string ConfigManager::playerCalibrationsPath;

std::string ConfigManager::tilePath;
std::string ConfigManager::scenePath;
std::string ConfigManager::pathPath;
std::string ConfigManager::wlvlPath;

std::string ConfigManager::commonGPath;
std::string ConfigManager::scriptPath;


std::string ConfigManager::PathLevelBGO()
{
    return bgoPath;
}

std::string ConfigManager::PathLevelBG()
{
    return BGPath;
}

std::string ConfigManager::PathLevelBlock()
{
    return blockPath;
}

std::string ConfigManager::PathLevelNPC()
{
    return npcPath;
}

std::string ConfigManager::PathLevelNPCScript()
{
    return npcScriptPath;
}

std::string ConfigManager::PathLevelEffect()
{
    return effectPath;
}

std::string ConfigManager::PathLevelPlayable()
{
    return playerLvlPath;
}

std::string ConfigManager::PathCommonGFX()
{
    return commonGPath;
}

std::string ConfigManager::PathScript()
{
    return scriptPath;
}


std::string ConfigManager::PathWorldTiles()
{
    return tilePath;
}

std::string ConfigManager::PathWorldScenery()
{
    return scenePath;
}

std::string ConfigManager::PathWorldPaths()
{
    return pathPath;
}

std::string ConfigManager::PathWorldLevels()
{
    return wlvlPath;
}

std::string ConfigManager::PathWorldPlayable()
{
    return playerWldPath;
}

std::string ConfigManager::PathLevelPlayerScript()
{
    return playerScriptPath;
}

std::string ConfigManager::PathLevelPlayerCalibrations()
{
    return playerCalibrationsPath;
}

std::string ConfigManager::PathMusic()
{
    return dirs.music;
}

std::string ConfigManager::PathSound()
{
    return dirs.sounds;
}


void ConfigManager::refreshPaths()
{
    bgoPath =   dirs.glevel +  "background/";
    BGPath =    dirs.glevel +  "background2/";
    blockPath = dirs.glevel +  "block/";
    npcPath =   dirs.glevel +  "npc/";
    npcScriptPath = config_dirSTD +  "script/npcs/";
    playerScriptPath = config_dirSTD +  "script/player/";
    playerCalibrationsPath = config_dirSTD + "characters/";
    effectPath= dirs.glevel +  "effect/";
    playerLvlPath = dirs.gplayble;
    playerWldPath = dirs.gworld + "player/";

    tilePath =  dirs.gworld +  "tile/";
    scenePath = dirs.gworld +  "scene/";
    pathPath =  dirs.gworld +  "path/";
    wlvlPath =  dirs.gworld +  "level/";

    commonGPath = dirs.gcommon + "/";

    scriptPath = config_dirSTD + "script/";
}
