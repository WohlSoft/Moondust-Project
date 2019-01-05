/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#ifndef CONTROLLER_KEY_MAP_H
#define CONTROLLER_KEY_MAP_H

struct KM_Key
{
    inline KM_Key()
    {
        val=-1;
        id=-1;
        type=-1;
    }
    int val;
    int id;
    int type;
};

struct KeyMap
{
    KM_Key start;
    KM_Key drop;

    KM_Key left;
    KM_Key right;
    KM_Key up;
    KM_Key down;

    KM_Key jump;
    KM_Key jump_alt;

    KM_Key run;
    KM_Key run_alt;
};

struct KeyMapJoyCtrls
{
    enum CtrlTypes
    {
        NoControl=-1,
        JoyAxis=0,
        JoyBallX,
        JoyBallY,
        JoyHat,
        JoyButton
    };
    inline KeyMapJoyCtrls()
    {
        start=-1; drop=-1;
        left=-1; right=-1; up=-1; down=-1;
        jump=-1; jump_alt=-1; run=-1; run_alt=-1;
    }
    int start;

    int left;
    int right;
    int up;
    int down;

    int run;
    int jump;
    int run_alt;
    int jump_alt;

    int drop;
};

#endif // CONTROLLER_KEY_MAP_H

