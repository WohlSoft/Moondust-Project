/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2023 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef APP_PATH_H
#define APP_PATH_H

#include <string>

extern std::string  ApplicationPathSTD;

class AppPathManager
{
public:
    static void initAppPath();
    static std::string settingsFileSTD();
    static std::string userAppDirSTD();
    static std::string languagesDir();
    static std::string logsDir();
    static std::string screenshotsDir();
    static std::string gameSaveRootDir();
    static void install();
    static bool isPortable();
    static bool userDirIsAvailable();
#ifdef __EMSCRIPTEN__
    static void syncFs();
#endif

private:
    /**
     * @brief Makes settings path if not exists
     */
    static void initSettingsPath();
    //! Full path to settings INI file
    static std::string m_settingsPath;
    static std::string m_userPath;
};

#endif // APP_PATH_H
