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

#ifndef PGE_MENUBOX_H
#define PGE_MENUBOX_H

#include "pge_menuboxbase.h"

class PGE_MenuBox : public PGE_MenuBoxBase
{
public:
    PGE_MenuBox(Scene * _parentScene=NULL, QString _title="Menu is works!",
               msgType _type=msg_info, PGE_Point boxCenterPos=PGE_Point(-1,-1), float _padding=-1, QString texture="");
    PGE_MenuBox(const PGE_MenuBox &mb);

    void construct(QString _title="Menu is works!",
                    msgType _type=msg_info, PGE_Point pos=PGE_Point(-1,-1), float _padding=-1, QString texture="");

    ~PGE_MenuBox();

    void onUpButton();
    void onDownButton();
    void onLeftButton();
    void onRightButton();
    void onJumpButton();
    void onAltJumpButton();
    void onRunButton();
    void onAltRunButton();
    void onStartButton();
    void onDropButton();

};

#endif // PGE_MENUBOX_H
