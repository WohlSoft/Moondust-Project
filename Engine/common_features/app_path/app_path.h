/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2026 Vitaly Novichkov <admin@wohlnet.ru>
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

class EnginePathMan
{
public:

    /*!
     * \brief Initialise all paths
     */
    static void initAppPath();

    /*!
     * \brief Get the path to the game settings file
     * \return Path to the game settings file
     */
    static std::string settingsFileSTD();

    /*!
     * \brief Get the path to the writable user directory
     * \return Path to ther writable user directory, always ends with a slash
     */
    static std::string userAppDirSTD();

    /**
     * @brief Default read-only path to hold general data files for Moondust aplications
     * @return Path to read-only data path
     *
     * There are files such as translations, helper resources, and other stuff. On some platforms it might match to the application path.
     */
    static std::string dataDir();

    /*!
     * \brief Get the path to engine languages directory
     * \return Path to ther engine languages directory, always ends with a slash
     */
    static std::string languagesDir();

    /*!
     * \brief Get the path to the logs output directory
     * \return Path to ther logs output directory, always ends with a slash
     */
    static std::string logsDir();

    /*!
     * \brief Get the path to the screenshots output directory
     * \return Path to ther screenshots output directory, always ends with a slash
     */
    static std::string screenshotsDir();

    /*!
     * \brief Get the path to the game saves directory
     * \return Path to ther game saves directory, always ends with a slash
     */
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
    static void initDataPath();
    static void initIsPortable();

    static bool m_isPortable;
    //! Full path to settings INI file
    static std::string m_settingsPath;
    static std::string m_userPath;
    //! Path to read-only resources directory (on UNIX-like systems it might appear around the /usr/share/moondust-project/ directory)
    static std::string m_dataPath;
};

#endif // APP_PATH_H
