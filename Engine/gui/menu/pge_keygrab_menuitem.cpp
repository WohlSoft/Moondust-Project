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

#include <graphics/window.h>
#include <graphics/gl_renderer.h>
#include <fontman/font_manager.h>
#include <audio/pge_audio.h>
#include "pge_keygrab_menuitem.h"
#include <controls/controller_joystick.h>

#include "../pge_menu.h"

PGE_KeyGrabMenuItem::PGE_KeyGrabMenuItem() : PGE_Menuitem()
{
    keyValue=NULL;
    type=ITEM_KeyGrab;
    menu=NULL;
    chosing=false;
    joystick_mode=false;
    joystick_key_id     = NULL;
    joystick_key_type   = NULL;
    joystick_device     = NULL;
}

PGE_KeyGrabMenuItem::PGE_KeyGrabMenuItem(const PGE_KeyGrabMenuItem &it) : PGE_Menuitem(it)
{
    this->keyValue = it.keyValue;
    this->menu = it.menu;
    this->type = it.type;
    this->chosing = it.chosing;
    this->joystick_mode=it.joystick_mode;
    this->joystick_key_id=it.joystick_key_id;
    this->joystick_key_type=it.joystick_key_type;
    this->joystick_device = it.joystick_device;
}

PGE_KeyGrabMenuItem::~PGE_KeyGrabMenuItem()
{}

void PGE_KeyGrabMenuItem::processJoystickBind()
{
    if(joystick_mode)
    {
        int jkeyValue=0;
        int keyType=0;
        int keyId=0;
        if(JoystickController::bindJoystickKey(joystick_device, jkeyValue, keyId, keyType) )
        {
            chosing=false;
            *keyValue=jkeyValue;
            *joystick_key_id=keyId;
            *joystick_key_type=keyType;
            PGE_Audio::playSoundByRole(obj_sound_role::MenuDo);
            if(menu) menu->is_keygrab=false;
        }
    }
}

void PGE_KeyGrabMenuItem::grabKey()
{
    chosing=true;
    if(keyValue)
    {
        if(menu)
        {
            menu->is_keygrab=true;
            menu->m_item = this;
        }
    }
}

void PGE_KeyGrabMenuItem::pushKey(int scancode)
{
    if(joystick_mode && (scancode>=0))
        return;
    chosing=false;

    if(keyValue)
    {
        if(scancode>=0)//if -1 - cancel grabbing of key
        {
            *keyValue=scancode;
            PGE_Audio::playSoundByRole(obj_sound_role::MenuDo);
        } else if(scancode==PGE_KEYGRAB_CANCEL)//Cancel key grabbing
        {
            PGE_Audio::playSoundByRole(obj_sound_role::BlockHit);
        }
        else //Remove control key
        {
            if(joystick_mode)
            {
                *joystick_key_id=-1;
                *joystick_key_type=-1;
            }
            *keyValue=-1;
            PGE_Audio::playSoundByRole(obj_sound_role::NpcLavaBurn);
        }
        if(menu) menu->is_keygrab=false;
    }
}

void PGE_KeyGrabMenuItem::render(int x, int y)
{
    PGE_Menuitem::render(x, y);
    if(chosing)
        FontManager::printText(QString("..."), x+valueOffset, y);
    else
    if(keyValue)
    {
        if(joystick_mode)
        {
            if(*joystick_key_type!=-1)
                FontManager::printText(QString("Key=%1 ID=%2 Type=%3")
                                       .arg(*keyValue)
                                       .arg(*joystick_key_id)
                                       .arg(*joystick_key_type)
                                       , x+valueOffset, y, _font_id);
            else
                FontManager::printText(QString("<none>"), x+valueOffset, y, _font_id);
        }
        else
        {
            if(*keyValue!=-1)
                FontManager::printText(QString(SDL_GetScancodeName((SDL_Scancode)*keyValue)), x+valueOffset, y, _font_id);
            else
                FontManager::printText(QString("<none>"), x+valueOffset, y, _font_id);
        }
    }
}
