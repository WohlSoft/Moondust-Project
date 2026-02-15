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

#include <QDir>

#include "../pge_app_path.h"
#include "pge_app_path_private.h"


void AppPathManagerPrivate::initAppPathPrivate(const char *argv0)
{
    ApplicationPath = QFileInfo(QString::fromUtf8(argv0)).absoluteDir().absolutePath();
}

void AppPathManagerPrivate::initDataPath()
{
    m_dataPath = ApplicationPath;
}

void AppPathManagerPrivate::initLibExecPath()
{
    m_libExecPath = m_dataPath;
}

void AppPathManagerPrivate::initLangsDir()
{
    m_languagesPath = m_dataPath + "/languages";
}

void AppPathManagerPrivate::initLogsDir()
{
    m_logsPath = m_userPath + "/logs";
}
