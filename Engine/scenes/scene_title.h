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

#ifndef SCENE_TITLE_H
#define SCENE_TITLE_H

#include <map>
#include <utility>
#include <atomic>
#include "scene.h"
#include <gui/pge_menu.h>
#include <common_features/pge_texture.h>
#include <common_features/simple_animator.h>
#include <common_features/episode_state.h>
#include <common_features/point.h>
#include <controls/controller.h>
#include <SDL2/SDL_thread.h>

#include <script/lua_titlescreen_engine.h>

typedef std::pair<size_t, size_t> menustate;

struct TitleScene_misc_img
{
    int x;
    int y;
    PGE_Texture t;
    SimpleAnimator a;
    int frmH;
};

class TitleScene : public Scene
{
    public:
        TitleScene();
        ~TitleScene();

        bool init();

        void onKeyboardPressed(SDL_Scancode scancode);
        void onKeyboardPressedSDL(SDL_Keycode sdl_key, Uint16 modifier); //!< Triggering when pressed any key on keyboard
        void onMouseMoved(SDL_MouseMotionEvent &mmevent);
        void onMousePressed(SDL_MouseButtonEvent &mbevent);
        void onMouseWheel(SDL_MouseWheelEvent &wheelevent);
        LuaEngine *getLuaEngine();

        void processEvents();

        void update();
        void render();
        void renderMouse();

        void processMenu();

        int exec();
        void resetController();

        PGE_Menu m_menu;
        enum CurrentMenu
        {
            menu_main = 0,
            menu_options,
            menu_playlevel,
            menu_playlevel_wait,
            menu_playepisode,
            menu_playepisode_wait,
            menu_playbattle,
            menu_playbattle_wait,
            menu_opensave,
            menu_tests,
            menu_testboxes,
            menu_controls,
            menu_controls_plr1,
            menu_controls_plr2,
            menu_videosettings,
            menu_player,
            menu_volume,
            menu_dummy_and_big,//leave it!
            //For fetching
            menuFirst = menu_main,
            menuLast = menu_dummy_and_big
        };

        enum menuAnswer
        {
            ANSWER_EXIT = 0,
            ANSWER_PLAYLEVEL,
            ANSWER_PLAYEPISODE,
            ANSWER_PLAYEPISODE_2P,
            ANSWER_PLAYBATTLE,
            ANSWER_CREDITS,
            ANSWER_LOADING,
            ANSWER_GAMEOVER
        };

        int                 m_numOfPlayers      = 1;
        //! play episode
        PlayEpisodeResult   m_result_episode;
        //! Play level/battle
        PlayLevelResult     m_result_level;

        Controller *controller = nullptr;

        /**********************file_finder************************/
        static int findEpisodes(void *);
        static int findLevels(void *);
        static SDL_Thread                                          *m_filefind_thread;
        static std::string                                          m_filefind_folder;
        static std::vector<std::pair<std::string, std::string > >   m_filefind_found_files;
        static std::atomic_bool                                     m_filefind_finished;
        /**********************file_finder************************/

    private:
        int         m_exitCode = 0;//!< Exit code
        CurrentMenu m_currentMenu = menu_main;
        void setMenu(CurrentMenu _menu);
        std::map<CurrentMenu, menustate>    m_menustates;
        std::vector<int>                    m_menuChain;

        PGE_Texture m_backgroundTexture;
        bool        m_backgroundLoaded = false;
        PGEColor    m_backgroundColor;
        std::vector<TitleScene_misc_img > m_imgs;

        PGE_Texture m_cursorTexture;
        bool        m_cursorLoaded = false;
        PGE_Point   m_cursorPos;

        int m_debug_joy_keyval    = 0;
        int m_debug_joy_keyid     = 0;
        int m_debug_joy_keytype   = 0;

        LuaTitleScreenEngine m_luaEngine;
};

#endif // SCENE_TITLE_H
