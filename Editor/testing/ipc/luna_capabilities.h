/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef LUNA_CAPABILITIES_H
#define LUNA_CAPABILITIES_H

#include <QString>
#include <QSet>

struct LunaLuaCapabilities
{
    //! Was LunaLua was been detected?
    bool loaded = false;
    //! Is detected LunaLua ompatible?
    bool isCompatible = false;
    //! Type of capabilities recognizing
    QString type;
    //! Presenting of special features
    QSet<QString> features;
    //! Available IPC commands
    QSet<QString> ipcCommands;
    //! Available command line arguments
    QSet<QString> args;

    //! Clear data
    void clear()
    {
        loaded = false;
        isCompatible = false;
        type.clear();
        features.clear();
        args.clear();
        ipcCommands.clear();
    }
};

/**
 * @brief Detect capabilities of given LunaDll.dll library
 * @param caps Target capatibilities structure
 * @param path Path to LunaDll.dll file
 * @return true when capatibilities has been recognized, false when given file doesn't exist or it's incompatible
 */
extern bool getLunaCapabilities(LunaLuaCapabilities &caps, const QString &path);

#endif // LUNA_CAPABILITIES_H
