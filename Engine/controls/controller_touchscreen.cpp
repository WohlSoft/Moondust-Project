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

#include "controller_touchscreen.h"
#include <graphics/window.h>
#include <common_features/logger.h>
#ifdef __ANDROID__
#include <android/log.h>
#endif

TouchScreenController::TouchScreenController() :
        Controller()
{
    m_touchDevicesCount = SDL_GetNumTouchDevices();
    SDL_GetWindowSize(PGE_Window::window, &m_screenWidth, &m_screenHeight);
}

static void updateKeyValue(bool &key, bool &key_pressed, const Uint8 state)
{
    key_pressed = (static_cast<bool>(state) && !key);
    key = state;
#ifdef __ANDROID__
    __android_log_print(ANDROID_LOG_DEBUG, "TRACKERS", "= Touch key: Pressed=%d, State=%d", (int)key_pressed, (int)key);
#endif
}

static void updateFingerKeyState(TouchScreenController::FingerState &st, controller_keys &keys, int keyCommand, const Uint8 setState)
{
    st.alive = static_cast<bool>(setState);
    switch(keyCommand)
    {
        case Controller::key_left:
            updateKeyValue(keys.left, keys.left_pressed, setState);
            st.heldKey = Controller::key_left;
            break;
        case Controller::key_right:
            updateKeyValue(keys.right, keys.right_pressed, setState);
            st.heldKey = Controller::key_right;
            break;
        case Controller::key_up:
            updateKeyValue(keys.up, keys.up_pressed, setState);
            st.heldKey = Controller::key_up;
            break;
        case Controller::key_down:
            updateKeyValue(keys.down, keys.down_pressed, setState);
            st.heldKey = Controller::key_down;
            break;
        case Controller::key_jump:
            updateKeyValue(keys.jump, keys.jump_pressed, setState);
            st.heldKey = Controller::key_jump;
            break;
        case Controller::key_altjump:
            updateKeyValue(keys.alt_jump, keys.alt_jump_pressed, setState);
            st.heldKey = Controller::key_altjump;
            break;
        case Controller::key_run:
            updateKeyValue(keys.run, keys.run_pressed, setState);
            st.heldKey = Controller::key_run;
            break;
        case Controller::key_altrun:
            updateKeyValue(keys.alt_run, keys.alt_run_pressed, setState);
            st.heldKey = Controller::key_altrun;
            break;
        case Controller::key_drop:
            updateKeyValue(keys.drop, keys.drop_pressed, setState);
            st.heldKey = Controller::key_drop;
            break;
        case Controller::key_start:
            updateKeyValue(keys.start, keys.start_pressed, setState);
            st.heldKey = Controller::key_start;
            break;
        default:
        case -1:
            st.alive = false;
            break;
    }
}

struct KeyPos
{
    float x1;
    float y1;
    float x2;
    float y2;
    Controller::commands cmd;
};

KeyPos touchKeysMap[] =
{
    {7.0f, 483.0f, 134.0f, 604.0f, Controller::key_left},
    {280.0f, 483.0f, 403.0f, 604.0f, Controller::key_right},
    {147.0f, 338.0f, 270.0f, 455.0f, Controller::key_up},
    {147.0f, 483.0f, 270.0f, 604.0f, Controller::key_down},

    {897.0f, 459.0f, 1016.0f, 557.0f, Controller::key_jump},
    {897.0f, 304.0f, 1016.0f, 418.0f, Controller::key_altjump},
    {732.0f, 459.0f, 853.0f, 557.0f, Controller::key_run},
    {732.0f, 304.0f, 853.0f, 418.0f, Controller::key_altrun},

    {897.0f, 142.0f, 1016.0f, 207.0f, Controller::key_drop},
    {439.0f, 7.0f, 636.0f, 66.0f, Controller::key_start}
};

static int findTouchKey(float x, float y)
{
    const size_t touchKeyMapSize = sizeof(touchKeysMap) / sizeof(KeyPos);
    x *= 1024.0f;
    y *= 612.f;
    for(size_t i = 0; i < touchKeyMapSize; i++)
    {
        const KeyPos &p = touchKeysMap[i];
        if(x >= p.x1 && x <= p.x2 && y >= p.y1 && y <= p.y2)
            return p.cmd;
    }
    return -1;
}

void TouchScreenController::update()
{
    if(m_touchDevicesCount == 0)
        return; // Nothing to do

    const SDL_TouchID dev = SDL_GetTouchDevice(0);

    int fingers = SDL_GetNumTouchFingers(dev);

    for(auto it = m_fingers.begin(); it != m_fingers.end(); it++)
    {
        // Mark as "dead"
        it->second.alive = false;
    }

    for(int i = 0; i < fingers; i++)
    {
        SDL_Finger *f = SDL_GetTouchFinger(dev, i);
        if(!f) //Skip a wrong finger
            continue;
        auto found = m_fingers.find(f->id);
        if(found != m_fingers.end())
        {
            int keyCommand = findTouchKey(f->x, f->y);
            if(keyCommand != found->second.heldKey) //Change key if different
            {
                updateFingerKeyState(found->second, keys, found->second.heldKey, 0);
                updateFingerKeyState(found->second, keys, keyCommand, 1);
            }
            else // Keep current key alive
            {
                updateFingerKeyState(found->second, keys, found->second.heldKey, 1);
            }
        }
        else
        {
            // Detect which key is pressed, and press it
            int keyCommand = findTouchKey(f->x, f->y);
            FingerState st;
            updateFingerKeyState(st, keys, keyCommand, 1);
            if(st.alive)
                m_fingers.insert({f->id, st});
        }
#ifdef __ANDROID__
        __android_log_print(ANDROID_LOG_DEBUG, "TRACKERS", "= Finger press: ID=%d, X=%.04f, Y=%.04f, P=%.04f", (int)f->id, f->x, f->y, f->pressure);
#else
        pLogDebug("= Finger press: ID=%d, X=%.04f, Y=%.04f, P=%.04f", f->id, f->x, f->y, f->pressure);
#endif
    }

    for(auto it = m_fingers.begin(); it != m_fingers.end();)
    {
        if(!it->second.alive)
        {
            updateFingerKeyState(it->second, keys, it->second.heldKey, 0);
            it = m_fingers.erase(it);
            continue;
        }
        it++;
    }
}
