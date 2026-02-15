/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2026 Vitaly Novichkov <admin@wohlnet.ru>
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

#pragma once
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
namespace AppPathManager
{

/**
 * @brief Initialize path manager
 */
void initAppPath(const char *argv0);
/**
 * @brief Returns application settings file
 * @return path to settings INI file
 */
QString settingsFile();
/**
 * @brief Returns generic folder for saving of various settings files
 * @return path to settings folder
 */
QString settingsPath();
/**
 * @brief Path to user profile application dir
 * @return Path to user profile application dir
 */
QString userAppDir();

/**
 * @brief Default read-only path to hold general data files for Moondust aplications
 * @return Path to read-only data path
 *
 * There are files such as translations, helper resources, and other stuff. On some platforms it might match to the application path.
 */
QString dataDir();

/**
 * @brief Directory that contains utility executables used by the devkit directly
 * @return Path to the libexec directory
 */
QString libExecDir();

/**
 * @brief Path to UI translation files
 * @return Path to UI translation files
 */
QString languagesDir();
/**
 * @brief Path to logs directory
 * @return Path to logs directory
 */
QString logsDir();
/**
 * @brief Process installation (make user directory, save properties to operating system registry)
 */
void install();
/**
 * @brief Is this application portable
 * @return true if this application works in portable mode, false in intergated mode
 */
bool isPortable();
/**
 * @brief Is user profile directory available for writing
 * @return true if user profile directory is available
 */
bool userDirIsAvailable();

}

#endif // APP_PATH_H
