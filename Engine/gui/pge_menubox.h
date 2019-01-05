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

#ifndef PGE_MENUBOX_H
#define PGE_MENUBOX_H

#include "pge_menuboxbase.h"

class PGE_MenuBox : public PGE_MenuBoxBase
{
public:
    PGE_MenuBox(Scene * _parentScene=NULL, std::string _title="Menu is works!",
               msgType _type=msg_info, PGE_Point boxCenterPos=PGE_Point(-1,-1), double _padding=-1, std::string texture="");
    PGE_MenuBox(const PGE_MenuBox &mb);

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
