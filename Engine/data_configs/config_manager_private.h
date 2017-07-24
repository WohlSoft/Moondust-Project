/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef CONFIG_MANAGER_PRIVATE_H
#define CONFIG_MANAGER_PRIVATE_H

#include <Utils/strings.h>
#include <IniProcessor/ini_processing.h>

namespace ConfigManager
{
    void   checkForImage(std::string &imgPath, std::string root);

    //special paths
    extern std::string imgFile, imgFileM;
    extern std::string tmpstr;
    extern Strings::List tmp;

    extern std::string bgoPath;
    extern std::string BGPath;
    extern std::string blockPath;
    extern std::string npcPath;
    extern std::string npcScriptPath;
    extern std::string effectPath;
    extern std::string playerLvlPath;
    extern std::string playerWldPath;
    extern std::string playerScriptPath;
    extern std::string playerCalibrationsPath;

    extern std::string tilePath;
    extern std::string scenePath;
    extern std::string pathPath;
    extern std::string wlvlPath;

    extern std::string commonGPath;

    extern std::string scriptPath;

    void    refreshPaths();
    bool    loadEngineSettings(); //!< Load engine.ini file
}

#endif // CONFIG_MANAGER_PRIVATE_H
