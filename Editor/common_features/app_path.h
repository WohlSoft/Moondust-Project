/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef APP_PATH_H
#define APP_PATH_H

#include <QString>

//! Full path to application directory
extern QString ApplicationPath;
//! Full path to application directory (in OS X shows path to executable inside bundle)
extern QString ApplicationPath_x;

/**
 * @brief Manager of the application paths and settings files
 */
class AppPathManager
{
    public:
        /**
         * @brief Initialize path manager
         */
        static void initAppPath();
        /**
         * @brief Returns application settings file
         * @return path to settings INI file
         */
        static QString settingsFile();
        /**
         * @brief Path to user profile application dir
         * @return Path to user profile application dir
         */
        static QString userAppDir();
        /**
         * @brief Process installation (make user directory, save properties to operating system registry)
         */
        static void install();
        /**
         * @brief Is this application portable
         * @return true if this application works in portable mode, false in intergated mode
         */
        static bool isPortable();
        /**
         * @brief Is user profile directory available for writing
         * @return true if user profile directory is available
         */
        static bool userDirIsAvailable();

    private:
        //! Full path to settings INI file
        static QString m_settingsPath;
};

#endif // APP_PATH_H
