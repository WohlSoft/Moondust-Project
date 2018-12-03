/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "pge_menubox.h"

PGE_MenuBox::PGE_MenuBox(Scene *_parentScene, std::string _title, msgType _type,
                       PGE_Point boxCenterPos, double _padding, std::string texture)
    : PGE_MenuBoxBase(_parentScene,
                      PGE_Menu::menuAlignment::VERTICLE, 0,
                      _title, _type, boxCenterPos, _padding, texture)
{
    setParentScene(_parentScene);
    PGE_MenuBoxBase::construct(_title, _type, boxCenterPos, _padding, texture);
}

PGE_MenuBox::PGE_MenuBox(const PGE_MenuBox &mb)
    : PGE_MenuBoxBase(mb)
{}

PGE_MenuBox::~PGE_MenuBox()
{}

void PGE_MenuBox::onUpButton()
{
    _menu.selectUp();
}

void PGE_MenuBox::onDownButton()
{
    _menu.selectDown();
}

void PGE_MenuBox::onLeftButton()
{
    _menu.selectLeft();
}

void PGE_MenuBox::onRightButton()
{
    _menu.selectRight();
}

void PGE_MenuBox::onJumpButton()
{
    _menu.acceptItem();
}

void PGE_MenuBox::onAltJumpButton()
{
    _menu.acceptItem();
}

void PGE_MenuBox::onRunButton()
{
    _menu.rejectItem();
}

void PGE_MenuBox::onAltRunButton()
{
    _menu.rejectItem();
}

void PGE_MenuBox::onStartButton()
{
    _menu.acceptItem();
}

void PGE_MenuBox::onDropButton()
{
    _menu.rejectItem();
}
