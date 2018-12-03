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

#ifndef SCENE_LOADING_H
#define SCENE_LOADING_H

#include <vector>
#include <common_features/pge_texture.h>
#include <common_features/simple_animator.h>

#include "scene.h"

class LoadingScene_misc_img
{
    public:
        LoadingScene_misc_img();
        LoadingScene_misc_img(const LoadingScene_misc_img &im);

        int x;
        int y;
        PGE_Texture t;
        SimpleAnimator a;
        int frmH;
};

class LoadingScene : public Scene
{
        friend void loadingSceneLoopStep(void *scene);
    public:
        LoadingScene();
        ~LoadingScene();
        void init();

        void setWaitTime(int time);
        void exitFromScene();

        void onKeyboardPressedSDL(SDL_Keycode sdl_key, Uint16 modifier);
        void onMousePressed(SDL_MouseButtonEvent &mbevent);

        void update();
        void render();
        int exec();

    private:
        float _waitTimer;

        bool m_gfx_frameSkip = false;

        PGE_Texture background;
        PGEColor    bgcolor;
        std::vector<LoadingScene_misc_img > imgs;
};

#endif // SCENE_LOADING_H
