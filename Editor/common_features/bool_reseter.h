/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2025 Vitaly Novichkov <admin@wohlnet.ru>
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
#ifndef BOOLRESETER_H
#define BOOLRESETER_H
#include <QAtomicInteger>
///
/// \brief The BoolReseter class
///
/// Smart flag reseting class. Like to smart pointers, but especially for boolean flags.
/// For example, you can use it to reset a lock when you finished some operation
///
class BoolReseter
{
public:
    BoolReseter(bool* _var, bool _to=false);
    BoolReseter(QAtomicInteger<bool>* _var, bool _to=false);
    ~BoolReseter();
    bool *var;
    QAtomicInteger<bool>* atomic_var;
    bool to;
};

#endif // BOOLRESETER_H
