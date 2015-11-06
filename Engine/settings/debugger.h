/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <scenes/scene.h>

class PGE_Debugger
{
public:
    static void executeCommand(Scene* parent);

    static bool cheat_allowed;

    static bool cheat_pagangod;
    static bool cheat_chucknorris;
    static bool cheat_superman;

    static bool cheat_worldfreedom;

    static void resetEverything();
    static void setRestriction(bool denyed);

};

#endif // DEBUGGER_H
