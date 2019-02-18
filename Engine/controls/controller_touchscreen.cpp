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
#   include <jni.h>
#   if 1
#       undef JNIEXPORT
#       undef JNICALL
#       define JNIEXPORT extern "C"
#       define JNICALL
#   endif
#endif

TouchScreenController::FingerState::FingerState()
{
    SDL_memset(&heldKey, 0, sizeof(heldKey));
    SDL_memset(&heldKeyPrev, 0, sizeof(heldKeyPrev));
}

TouchScreenController::FingerState::FingerState(const FingerState &fs)
{
    alive = fs.alive;
    SDL_memcpy(&heldKey, &fs.heldKey, sizeof(heldKey));
    SDL_memcpy(&heldKeyPrev, &fs.heldKeyPrev, sizeof(heldKeyPrev));
}

TouchScreenController::FingerState &TouchScreenController::FingerState::operator=(const FingerState &fs)
{
    alive = fs.alive;
    SDL_memcpy(&heldKey, &fs.heldKey, sizeof(heldKey));
    SDL_memcpy(&heldKeyPrev, &fs.heldKeyPrev, sizeof(heldKeyPrev));
    return *this;
}

static struct TouchKeyMap
{
    struct KeyPos
    {
        float x1;
        float y1;
        float x2;
        float y2;
        Controller::commands cmd;
    };

    //! Width of canvas area
    float touchCanvasWidth = 1024.0f;
    //! Height of canvas area
    float touchCanvasHeight = 600.0f;

    //! List of key hit boxes
    KeyPos touchKeysMap[Controller::key_END] =
    {
        /* Note that order of keys must match the Controller::commands enum!!! */
        {331.0f, 537.0f, 482.0f,  587.0f, Controller::key_start},

        {1.0f,   328.0f, 91.0f,   498.0f, Controller::key_left},
        {171.0f, 328.0f, 261.0f,  498.0f, Controller::key_right},
        {1.0f,  328.0f, 261.0f,  418.0f, Controller::key_up},
        {1.0f,  498.0f, 261.0f,  588.0f, Controller::key_down},

        {807.0f, 431.0f, 914.0f,  522.0f, Controller::key_run},
        {898.0f, 396.0f, 1005.0f, 487.0f, Controller::key_jump},
        {807.0f, 325.0f, 914.0f,  416.0f, Controller::key_altrun},
        {898.0f, 290.0f, 1005.0f, 381.0f, Controller::key_altjump},

        {542.0f, 537.0f, 693.0f,  587.0f, Controller::key_drop},
    };

    TouchKeyMap()
    {
        for(int it = Controller::key_BEGIN; it < Controller::key_END; it++)
            touchKeysMap[it].cmd = static_cast<Controller::commands>(it);
    }

    /**
     * \brief Change size of virtual canvas
     * @param width Width of touchable canvas
     * @param height Height of touchable canvas
     */
    void setCanvasSize(float width, float height)
    {
        touchCanvasWidth = width;
        touchCanvasHeight = height;
    }

    /**
     * \brief Change hitbox of key
     * @param cmd Command
     * @param left Left side position
     * @param top Top side position
     * @param right Right side position
     * @param bottom Bottom side position
     */
    void setKeyPos(Controller::commands cmd, float left, float top, float right, float bottom)
    {
        if(cmd < 0 || cmd >= Controller::key_END)
            return;

        SDL_assert_release(touchKeysMap[cmd].cmd == cmd);

        auto &key = touchKeysMap[cmd];
        key.x1 = left;
        key.y1 = top;
        key.x2 = right;
        key.y2 = bottom;
    }

    /**
     * \brief Find keys are touched by one finger
     * @param x X position of finger
     * @param y Y position of finger
     * @param keys Destination array to write captured keys
     * @return Count of keys are got touched
     */
    int findTouchKeys(float x, float y, TouchScreenController::FingerState &fs)
    {
        const size_t touchKeyMapSize = sizeof(touchKeysMap) / sizeof(KeyPos);
        int count = 0;
        x *= touchCanvasWidth;
        y *= touchCanvasHeight;

        for(const auto &p : touchKeysMap)
        {
            fs.heldKey[p.cmd] = false;
            if(x >= p.x1 && x <= p.x2 && y >= p.y1 && y <= p.y2)
            {
                fs.heldKey[p.cmd] = true;
                count++;
            }
        }

        return count;
    }

} g_touchKeyMap;


#ifdef __ANDROID__

JNIEXPORT void JNICALL
Java_ru_wohlsoft_moondust_moondustActivity_setKeyPos(JNIEnv *env, jclass type,
        jint cmd, jfloat left, jfloat top, jfloat right, jfloat bottom)
{
    (void)env;
    (void)type;
    g_touchKeyMap.setKeyPos(static_cast<Controller::commands>(cmd), left, top, right, bottom);
}

JNIEXPORT void JNICALL
Java_ru_wohlsoft_moondust_moondustActivity_setCanvasSize(JNIEnv *env, jclass type,
        jfloat width, jfloat height)
{

    (void)env;
    (void)type;
    g_touchKeyMap.setCanvasSize(width, height);
}

#endif


TouchScreenController::TouchScreenController() :
        Controller()
{
    m_touchDevicesCount = SDL_GetNumTouchDevices();
    SDL_GetWindowSize(PGE_Window::window, &m_screenWidth, &m_screenHeight);
}

static void updateKeyValue(bool &key, bool &key_pressed, bool state)
{
    key_pressed = (state && !key);
    key = state;
    D_pLogDebug("= Touch key: Pressed=%d, State=%d", (int)key_pressed, (int)key);
}

static void updateFingerKeyState(TouchScreenController::FingerState &st,
        controller_keys &keys, int keyCommand, bool setState)
{
    st.alive = (setState != 0);
    if(keyCommand >= static_cast<int>(Controller::key_BEGIN) && keyCommand < static_cast<int>(Controller::key_END))
    {
        switch(keyCommand)
        {
            case Controller::key_left:
                updateKeyValue(keys.left, keys.left_pressed, setState);
                break;
            case Controller::key_right:
                updateKeyValue(keys.right, keys.right_pressed, setState);
                break;
            case Controller::key_up:
                updateKeyValue(keys.up, keys.up_pressed, setState);
                break;
            case Controller::key_down:
                updateKeyValue(keys.down, keys.down_pressed, setState);
                break;
            case Controller::key_jump:
                updateKeyValue(keys.jump, keys.jump_pressed, setState);
                break;
            case Controller::key_altjump:
                updateKeyValue(keys.alt_jump, keys.alt_jump_pressed, setState);
                break;
            case Controller::key_run:
                updateKeyValue(keys.run, keys.run_pressed, setState);
                break;
            case Controller::key_altrun:
                updateKeyValue(keys.alt_run, keys.alt_run_pressed, setState);
                break;
            case Controller::key_drop:
                updateKeyValue(keys.drop, keys.drop_pressed, setState);
                break;
            case Controller::key_start:
                updateKeyValue(keys.start, keys.start_pressed, setState);
                break;
            default:
                break;
        }
    }
    else
    {
        st.alive = false;
    }
}


void TouchScreenController::update()
{
    if(m_touchDevicesCount == 0)
        return; // Nothing to do

    const SDL_TouchID dev = SDL_GetTouchDevice(0);

    int fingers = SDL_GetNumTouchFingers(dev);

    for(auto &m_finger : m_fingers)
    {
        // Mark as "dead"
        m_finger.second.alive = false;
    }

    for(int i = 0; i < fingers; i++)
    {
        SDL_Finger *f = SDL_GetTouchFinger(dev, i);
        if(!f || (f->id < 0)) //Skip a wrong finger
            continue;

        SDL_FingerID finger_id = f->id;
        float finger_x = f->x, finger_y = f->y, finger_pressure = f->pressure;

        auto found = m_fingers.find(finger_id);
        if(found != m_fingers.end())
        {
            FingerState &fs = found->second;
            int keysCount = g_touchKeyMap.findTouchKeys(finger_x, finger_y, fs);
            for(int key = 0; key < Controller::key_END; key++)
            {
                if(fs.heldKeyPrev[key] && !fs.heldKey[key]) // set key off
                {
                    updateFingerKeyState(found->second, keys, key, false);
                    fs.heldKeyPrev[key] = fs.heldKey[key];
                }
                else if(fs.heldKey[key]) // set key on and keep alive
                {
                    updateFingerKeyState(found->second, keys, key, true);
                    fs.heldKeyPrev[key] = fs.heldKey[key];
                }
            }
            fs.alive = (keysCount > 0);
        }
        else
        {
            // Detect which key is pressed, and press it
            FingerState st;
            int keysCount = g_touchKeyMap.findTouchKeys(finger_x, finger_y, st);
            for(int key = 0; key < Controller::key_END; key++)
            {
                if(st.heldKey[key]) // set key on
                {
                    updateFingerKeyState(found->second, keys, key, true);
                    st.heldKeyPrev[key] = st.heldKey[key];
                }
            }
            st.alive = (keysCount > 0);
            if(st.alive)
                m_fingers.insert({finger_id, st});
        }

        D_pLogDebug("= Finger press: ID=%d, X=%.04f, Y=%.04f, P=%.04f",
                static_cast<int>(finger_id), finger_x, finger_y, finger_pressure);

    }

    for(auto it = m_fingers.begin(); it != m_fingers.end();)
    {
        if(!it->second.alive)
        {
            for(int key = 0; key < Controller::key_END; key++)
                updateFingerKeyState(it->second, keys, key, false);
            it = m_fingers.erase(it);
            continue;
        }
        it++;
    }

    Controller::update();
}
