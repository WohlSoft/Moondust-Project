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

#ifndef WINE_SETUP_CFG_H
#define WINE_SETUP_CFG_H

#include <QString>

struct WineSetupData
{
    // User-controllable setup
    bool useCustom = false;
    bool useCustomEnv = false;
    QString wineRoot;

    bool useWinePrefix = false;
    QString winePrefix;

    bool enableWineDebug = false;

    // Auto-computed values
    QString metaWineExec;
    QString metaWinePathExec;
    QString metaWine64Exec;
    QString metaWineBinDir;
    QString metaWineLibDir;
    QString metaWineLib64Dir;
    QString metaWineDllDir;

};

#endif // WINE_SETUP_CFG_H
