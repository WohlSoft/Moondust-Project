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

#include "pge_questionbox.h"

#include <fontman/font_manager.h>
#include <graphics/window.h>

#include <common_features/app_path.h>
#include <audio/pge_audio.h>
#include <settings/global_settings.h>

#include "../scenes/scene_level.h"
#include "../scenes/scene_world.h"

#include <QFontMetrics>
#include <QMessageBox>
#include <QApplication>
#include <QTranslator>


PGE_QuestionBox::PGE_QuestionBox(Scene *_parentScene, QString _title, msgType _type,
                       PGE_Point boxCenterPos, float _padding, QString texture)
    : PGE_MenuBoxBase(_parentScene, PGE_Menu::menuAlignment::HORIZONTAL, 30)
{
    setParentScene(_parentScene);
    construct(_title,_type, boxCenterPos, _padding, texture);
}

PGE_QuestionBox::PGE_QuestionBox(const PGE_QuestionBox &mb)
    : PGE_MenuBoxBase(mb)
{}


void PGE_QuestionBox::construct(QString _title, PGE_MenuBox::msgType _type,
                            PGE_Point pos, float _padding, QString texture)
{
    PGE_MenuBoxBase::construct(_title, _type, pos, _padding, texture);
}

PGE_QuestionBox::~PGE_QuestionBox()
{}

void PGE_QuestionBox::onLeftButton()
{
    _menu.selectUp();
}

void PGE_QuestionBox::onRightButton()
{
    _menu.selectDown();
}

void PGE_QuestionBox::onJumpButton()
{
    _menu.acceptItem();
}

void PGE_QuestionBox::onAltJumpButton()
{
    _menu.acceptItem();
}

void PGE_QuestionBox::onRunButton()
{
    _menu.rejectItem();
}

void PGE_QuestionBox::onAltRunButton()
{
    _menu.rejectItem();
}

void PGE_QuestionBox::onStartButton()
{
    _menu.acceptItem();
}

void PGE_QuestionBox::onDropButton()
{
    _menu.rejectItem();
}
