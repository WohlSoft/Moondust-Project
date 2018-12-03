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

#include "pge_questionbox.h"

#include <fontman/font_manager.h>
#include <graphics/window.h>

#include <common_features/app_path.h>
#include <audio/pge_audio.h>
#include <settings/global_settings.h>

#include "../scenes/scene_level.h"
#include "../scenes/scene_world.h"

PGE_QuestionBox::PGE_QuestionBox(Scene *_parentScene, std::string _title, msgType _type,
                       PGE_Point boxCenterPos, double _padding, std::string texture)
    : PGE_MenuBoxBase(  _parentScene, PGE_Menu::menuAlignment::HORIZONTAL, 30,
                        _title, _type, boxCenterPos, _padding, texture)
{
    setParentScene(_parentScene);
    PGE_MenuBoxBase::construct(_title, _type, boxCenterPos, _padding, texture);
}

PGE_QuestionBox::PGE_QuestionBox(const PGE_QuestionBox &mb)
    : PGE_MenuBoxBase(mb)
{}

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
