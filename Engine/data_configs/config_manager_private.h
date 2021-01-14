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
