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
    bool chosing = false;
    KM_Key *targetKey = nullptr;

    PGE_Menu* menu  = nullptr;
    friend class PGE_Menu;
    bool          joystick_mode     = false;
    SDL_Joystick* joystick_device   = nullptr;
};

#endif // PGE_KEYGRAB_MENUITEM_H

