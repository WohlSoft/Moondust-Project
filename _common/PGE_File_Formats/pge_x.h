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

#ifndef PGE_X_H
#define PGE_X_H

#include "pge_file_lib_globs.h"

typedef PGEPAIR<PGESTRING, PGESTRINGList> PGEXSct;

#ifdef PGE_FILES_QT
class PGEFile: PGE_FILES_INHERED
{
    Q_OBJECT
#else
class PGEFile
{
#endif

public:
    enum PGEX_Item_type
    {
        PGEX_Struct=0,
        PGEX_PlainText
    };


    struct PGEX_Val
    {
        PGESTRING marker;
        PGESTRING value;
    };

    struct PGEX_Item
    {
        PGEX_Item_type type;
        PGELIST<PGEX_Val > values;
    };

    struct PGEX_Entry
    {
        PGESTRING name;
        PGEX_Item_type type;
        PGELIST<PGEX_Item > data;
        PGELIST<PGEX_Entry > subTree;
    };

    #ifdef PGE_FILES_QT
    PGEFile(QObject *parent=NULL);
    PGEFile(PGEFile &pgeFile, QObject *parent=NULL);
    #else
    PGEFile();
    PGEFile(PGEFile &pgeFile);
    #endif
    PGEFile(PGESTRING _rawData);
    void setRawData(PGESTRING _rawData);
    bool buildTreeFromRaw();
    PGESTRING lastError();

    //data tree
    PGELIST<PGEX_Entry > dataTree;

private:
    PGESTRING _lastError;
    PGESTRING rawData;
    PGELIST<PGEXSct > rawDataTree;

    //Static functions
public:
    // /////////////Validators///////////////
    static bool IsSectionTitle(PGESTRING in);//Section Title

    static PGEX_Entry buildTree(PGESTRINGList &src_data, bool *_valid = 0);

    //returns FALSE on valid data
    static bool IsQStr(PGESTRING in);// QUOTED STRING
    static bool IsHex(PGESTRING in);// Hex Encoded String
    static bool IsIntU(PGESTRING in);// UNSIGNED INT
    static bool IsIntS(PGESTRING in);// SIGNED INT
    static bool IsFloat(PGESTRING &in);// FLOAT
    static bool IsBool(PGESTRING in);//BOOL
    static bool IsBoolArray(PGESTRING in);//Boolean array
    static bool IsIntArray(PGESTRING in);//Integer array
    static bool IsStringArray(PGESTRING in);//String array

    //Split string into data values
    static PGELIST<PGESTRINGList> splitDataLine(PGESTRING src_data, bool *valid = 0);

    //PGE Extended File parameter string generators
    static PGESTRING IntS(long input);
    static PGESTRING BoolS(bool input);
    static PGESTRING FloatS(double input);
    static PGESTRING qStrS(PGESTRING input);
    static PGESTRING hStrS(PGESTRING input);
    static PGESTRING strArrayS(PGESTRINGList input);
    static PGESTRING intArrayS(PGELIST<int > input);
    static PGESTRING BoolArrayS(PGELIST<bool > input);

    static PGESTRING X2STR(PGESTRING input);
    static PGESTRINGList X2STRArr(PGESTRING src);
    static PGELIST<bool> X2BollArr(PGESTRING src);

    static PGESTRING escapeStr(PGESTRING input);
    static PGESTRING restoreStr(PGESTRING input);
    static PGESTRING encodeEscape(PGESTRING input);
    static PGESTRING decodeEscape(PGESTRING input);

    static PGESTRING value(PGESTRING marker, PGESTRING data);
    static PGESTRING removeQuotes(PGESTRING str);
};


#endif // PGE_X_H

