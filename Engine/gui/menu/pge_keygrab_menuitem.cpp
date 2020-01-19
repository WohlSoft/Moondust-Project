/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <graphics/window.h>
#include <graphics/gl_renderer.h>
#include <fontman/font_manager.h>
#include <audio/pge_audio.h>
#include "pge_keygrab_menuitem.h"
#include <controls/controller_joystick.h>
#include <common_features/fmt_format_ne.h>

#include "../pge_menu.h"

PGE_KeyGrabMenuItem::PGE_KeyGrabMenuItem() :
    PGE_Menuitem()
{
    type = ITEM_KeyGrab;
}

PGE_KeyGrabMenuItem::PGE_KeyGrabMenuItem(const PGE_KeyGrabMenuItem &it) : PGE_Menuitem(it)
{
    this->targetKey = it.targetKey;
    this->menu = it.menu;
    this->type = it.type;
    this->chosing = it.chosing;
    this->joystick_mode = it.joystick_mode;
    this->joystick_device = it.joystick_device;
}

PGE_KeyGrabMenuItem::~PGE_KeyGrabMenuItem()
{}

void PGE_KeyGrabMenuItem::processJoystickBind()
{
    if(joystick_mode)
    {
        KM_Key jkey;
        jkey.val = 0;
        jkey.type = 0;
        jkey.id = 0;
        if(JoystickController::bindJoystickKey(joystick_device, jkey))
        {
            chosing = false;
            *targetKey = jkey;
            PGE_Audio::playSoundByRole(obj_sound_role::MenuDo);
            if(menu) menu->m_is_keygrab = false;
        }
    }
}

void PGE_KeyGrabMenuItem::grabKey()
{
    chosing = true;
    if(targetKey && menu)
    {
        menu->m_is_keygrab = true;
        menu->m_item = this;
    }
}

void PGE_KeyGrabMenuItem::pushKey(int scancode)
{
    //The first inital "Enter" - press by the keyboard should be ignored.
    if(menu && menu->isKeygrabViaKey())
    {
        menu->setKeygrabViaKey(false);
        return;
    }

    if(joystick_mode && (scancode >= 0))
        return;

    chosing = false;

    if(targetKey)
    {
        if(scancode >= 0) //if -1 - cancel grabbing of key
        {
            targetKey->val = scancode;
            PGE_Audio::playSoundByRole(obj_sound_role::MenuDo);
        }
        else if(scancode == PGE_KEYGRAB_CANCEL)//Cancel key grabbing
            PGE_Audio::playSoundByRole(obj_sound_role::BlockHit);
        else //Remove control key
        {
            if(joystick_mode)
            {
                targetKey->id = -1;
                targetKey->type = -1;
            }
            targetKey->val = -1;
            PGE_Audio::playSoundByRole(obj_sound_role::NpcLavaBurn);
        }

        if(menu)
            menu->m_is_keygrab = false;
    }
}

void PGE_KeyGrabMenuItem::render(int x, int y)
{
    PGE_Menuitem::render(x, y);
    float colorLevel = m_enabled ? 1.0 : 0.5;
    if(chosing)
        FontManager::printText("...", x + valueOffset, y, _font_id, colorLevel, colorLevel, colorLevel);
    else if(targetKey)
    {
        if(joystick_mode)
        {
            if(targetKey->type != -1)
                FontManager::printText(fmt::format_ne("Key={0} ID={1} Type={2}",
                                                   targetKey->val,
                                                   targetKey->id,
                                                   targetKey->type), x + valueOffset, y, _font_id, colorLevel, colorLevel, colorLevel);
            else
                FontManager::printText("<none>", x + valueOffset, y, _font_id, colorLevel, colorLevel, colorLevel);
        }
        else
        {
            if(targetKey->val != -1)
                FontManager::printText(SDL_GetScancodeName((SDL_Scancode)targetKey->val), x + valueOffset, y, _font_id, colorLevel, colorLevel, colorLevel);
            else
                FontManager::printText("<none>", x + valueOffset, y, _font_id, colorLevel, colorLevel, colorLevel);
        }
    }
}
