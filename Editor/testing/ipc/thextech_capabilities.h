/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2023 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef THEXTECHCAPABILITIES_H
#define THEXTECHCAPABILITIES_H

#include <QString>
#include <QSet>

struct TheXTechCapabilities
{
    //! Was LunaLua was been detected?
    bool loaded = false;
    //! Is detected LunaLua ompatible?
    bool isCompatible = false;
    //! Type of capabilities recognizing
    QString type;
    //! Version of engine
    QString version;
    //! Available IPC protocols
    QSet<QString> ipcProtocols;
    //! Available command line arguments
    QSet<QString> arguments;
    //! Available render APIs
    QSet<QString> renders;
    //! Presenting of special features
    QSet<QString> features;

    //! Clear data
    void clear()
    {
        loaded = false;
        isCompatible = false;
        type.clear();
        version.clear();
        features.clear();
        arguments.clear();
        ipcProtocols.clear();
        renders.clear();
    }
};

/**
 * @brief Detect capabilities of given TheXTech executable
 * @param caps Target capatibilities structure
 * @param path Path to TheXTech executable file
 * @return true when capatibilities has been recognized, false when given file doesn't exist or it's incompatible
 */
extern bool getTheXTechCapabilities(TheXTechCapabilities &caps, const QString &path);

#endif // THEXTECHCAPABILITIES_H
