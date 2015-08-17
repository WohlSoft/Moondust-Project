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

#include "pge_menubox.h"

PGE_MenuBox::PGE_MenuBox(Scene *_parentScene, QString _title, msgType _type,
                       PGE_Point boxCenterPos, float _padding, QString texture)
    : PGE_MenuBoxBase(_parentScene)
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
