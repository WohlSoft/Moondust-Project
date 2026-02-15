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
#ifndef PGE_APP_PATH_PRIVATE_H
#define PGE_APP_PATH_PRIVATE_H

#include <QString>

#define SHARE_DATA_DIR "moondust-project"

// Legacy user directory
#if defined(__ANDROID__) || defined(__APPLE__) || defined(__HAIKU__)
#   define UserDirName "/PGE Project"
#else
#   define UserDirName "/.PGE_Project"
#endif

namespace AppPathManagerPrivate
{
void initAppPathPrivate(const char *argv0);

/**
 * @brief Makes settings path if not exists
 */
void initSettingsPath();

/**
 * @brief Detects the proper read-only data path location
 */
void initDataPath();

/**
 * @brief Sets the languages directory path
 */
void initLangsDir();

/**
 * @brief Sets the misc. executables for IPC
 */
void initLibExecPath();

/**
 * @brief Sets the logs output directory
 */
void initLogsDir();

/**
 * @brief Checks is this the portable package or not
 */
void initIsPortable();

extern bool m_isPortable;

//! Full path to settings INI file
extern QString m_userPath;
//! Path to read-only resources directory (on UNIX-like systems it might appear around the /usr/share/moondust-project/ directory)
extern QString m_dataPath;
extern QString m_libExecPath;

extern QString m_settingsPath;
extern QString m_languagesPath;
extern QString m_logsPath;

}

#endif // PGE_APP_PATH_PRIVATE_H
