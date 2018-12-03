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

#ifndef SCENE_CONFIGSELECT_H
#define SCENE_CONFIGSELECT_H

#include <scenes/scene.h>
#include <controls/controller.h>
#include <gui/pge_menu.h>
#include <graphics/gl_color.h>
#include <Utils/vptrlist.h>

class ConfigSelectScene : public Scene
{
    friend void configSelectSceneLoopStep(void *scene);
public:
    ConfigSelectScene();
    ~ConfigSelectScene();

    std::string currentConfig;
    std::string currentConfigPath;
    std::string themePack;

    bool    hasConfigPacks();
    std::string isPreLoaded(std::string openConfig="");

    void onKeyboardPressedSDL(SDL_Keycode sdl_key, Uint16 modifier); //!< Triggering when pressed any key on keyboard
    void onMouseMoved(SDL_MouseMotionEvent &mmevent);
    void onMousePressed(SDL_MouseButtonEvent &mbevent);
    void onMouseWheel(SDL_MouseWheelEvent &wheelevent);

    void update();
    void render();
    void renderMouse();
    int32_t  exec();

    void processMenu();

    void processEvents();

    Controller* controller;

    void setLabel(const std::string &label);

private:
    struct ConfigPackEntry
    {
        PGE_Texture image;
        std::string fullname;
        std::string description;
        std::string key;
        std::string path;
    };
    int32_t ret;//!< Exit code
    bool m_gfx_frameSkip = false;
    std::string m_label;
    std::string m_waterMark;
    PGE_Rect m_waterMarkRect;
    GlColor  m_waterMarkColor;
    uint32_t m_waterMarkFontSize;
    PGE_Point mousePos;
    VPtrList<ConfigPackEntry> m_availablePacks;
    PGE_Menu menu;
    PGE_Texture cursor;
    PGEColor    bgcolor;
};

#endif // SCENE_CONFIGSELECT_H
