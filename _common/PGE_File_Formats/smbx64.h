/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

/*! \file smbx64.h
 *  \brief Contains static class with SMBX64 standard file format fields validation and generation functions
 */

#ifndef SMBX64_H
#define SMBX64_H

#include "pge_file_lib_globs.h"

//SMBX64 standard data
#ifdef PGE_FILES_QT
class SMBX64 : public QObject
{
    Q_OBJECT
#else
class SMBX64
{
#endif

public:
    inline SMBX64() {}

    // /////////////Validators///////////////
    //returns TRUE on wrong data
    static bool uInt(PGESTRING in); // UNSIGNED INT
    static bool sInt(PGESTRING in); // SIGNED INT
    static bool sFloat(PGESTRING &in); // SIGNED FLOAT
    static bool qStr(PGESTRING in); // QUOTED STRING
    static bool wBool(PGESTRING in); //Worded BOOL
    static bool dBool(PGESTRING in); //Worded BOOL

    //Convert from string to internal data
    static bool wBoolR(PGESTRING in);
    //Convert from SMBX64 string to internal
    static PGESTRING StrToStr(PGESTRING in);

    //SMBX64 parameter string generators
    static PGESTRING IntS(long input);
    static PGESTRING BoolS(bool input);
    static PGESTRING qStrS(PGESTRING input);
    static PGESTRING FloatS(float input);
    static PGESTRING qStrS_multiline(PGESTRING input);
};


#endif // SMBX64_H

