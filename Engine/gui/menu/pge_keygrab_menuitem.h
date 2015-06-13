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

#ifndef PGE_KEYGRAB_MENUITEM_H
#define PGE_KEYGRAB_MENUITEM_H

#include "_pge_menuitem.h"
#include <SDL2/SDL_joystick.h>
#include <controls/controller_key_map.h>

#define PGE_KEYGRAB_CANCEL -1
#define PGE_KEYGRAB_REMOVE_KEY -2

class PGE_KeyGrabMenuItem : public PGE_Menuitem
{
public:
    PGE_KeyGrabMenuItem();
    PGE_KeyGrabMenuItem(const PGE_KeyGrabMenuItem &it);
    ~PGE_KeyGrabMenuItem();
    void processJoystickBind();
    void grabKey();
    void pushKey(int scancode);
    void render(int x, int y);

private:
    bool chosing;
    KM_Key *targetKey;

    PGE_Menu* menu;
    friend class PGE_Menu;
    bool          joystick_mode;
    SDL_Joystick* joystick_device;
};

#endif // PGE_KEYGRAB_MENUITEM_H

