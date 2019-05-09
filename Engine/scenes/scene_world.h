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

#ifndef SCENE_WORLD_H
#define SCENE_WORLD_H

#include "scene.h"
#include <controls/controller.h>
#include <PGE_File_Formats/wld_filedata.h>
#include <common_features/pge_texture.h>
#include <common_features/episode_state.h>
#include <common_features/rect.h>
#include <common_features/point.h>
#include <common_features/point_mover.h>
#include <common_features/event_queue.h>
#include <data_configs/config_manager.h>
#include <gui/pge_menubox.h>

#include "world/wld_tilebox.h"
#include "world/wld_player_portrait.h"
#include "world/wld_pathopener.h"

#include <script/lua_world_engine.h>

struct WorldScene_misc_img
{
    int x;
    int y;
    PGE_Texture t;
    SimpleAnimator a;
    int frmH;
};

class WorldScene final : public Scene
{
    friend class PGE_BoxBase;
    friend class PGE_MsgBox;
    friend class PGE_TextInputBox;
    friend class PGE_MenuBoxBase;
    friend class WldPathOpener;
public:
    WorldScene();
    ~WorldScene() override;

    bool init();
    bool initPlayableCharacter(int playerId, bool isInit = false);
    bool loadConfigs();

    void onKeyboardPressedSDL(SDL_Keycode sdl_key, Uint16 modifier) override;
    LuaEngine *getLuaEngine() override;

    void processEvents() override;
    void update() override;
    void render() override;
    int  exec() override;

    bool isVizibleOnScreen(PGE_RectF &rect) override;

    void tickAnimations(double ticks);

    bool isExit();
    void setExiting(int delay, int reason);

    bool loadFile(std::string filePath);

    std::string getLastError();
    void setGameState(EpisodeState *_state);

private:
    bool            m_isInit;

    Controller     *m_player1Controller = nullptr;
    controller_keys m_controls_1;

    bool            m_frameSkip;

    EpisodeState   *m_gameState;
    std::string     m_errorMsg;

    WorldMapSetup   m_commonSetup;
    WorldData       m_data;

    bool            m_worldIsContinues;
    bool            m_lockControls;

    PGE_Rect        m_viewportRect;
    PGE_Fader       m_viewportFader;
    PointMover      m_viewportCameraMover;

    int             m_exitWorldDelay;
    int             m_exitWorldCode;

    int                         m_numOfPlayers;
    std::vector<PlayerState >   m_players;

    PGE_Texture                 m_backgroundTexture;
    std::vector<PGE_Texture >   m_texturesBank;

    enum WalkDirections
    {
        Walk_Idle = 0,
        Walk_Left,
        Walk_Right,
        Walk_Up,
        Walk_Down
    };
    struct MapWalker
    {
        //! X Position of the playable character
        double          posX;
        //! Y Position of the playable character
        double          posY;
        //! Walk direction of the playable character
        int             walkDirection = 0;
        //! Walk direction of the playable character
        int             prevWalkDirection = 0;
        //! Calculated movement step dependent to physical step
        double          moveSpeed;
        //! Speps counterm, used to correct inter-cell position
        double          moveStepsCount;
        //! Playable character setup
        obj_player      setup;
        //! Texture of the map walker playable character
        PGE_Texture     texture;
        //! Height of using image
        double          img_h;
        //! Simple animator
        SimpleAnimator  ani;
        double          offsetX;
        double          offsetY;
        void            refreshDirection(bool restorePrev = false);
    } m_mapWalker;

    void    doMoveStep(double &posVal);
    void    setDir(int dr);

    void        playMusic(unsigned long musicID, std::string customMusicFile, bool fade = false, int fadeLen = 300);
    void        stopMusic(bool fade = false, int fadeLen = 300);
    bool        m_playStopSnd;
    bool        m_playDenySnd;

    std::string m_currentMusicFile;

    void       jump();
    bool       m_jumpTo;
    PGE_PointF m_jumpToXY;

    /************Printable stuff****************/
    struct Counters
    {
        long        health = 0;
        long        lives = 0;
        uint32_t    stars = 0;
        long        points = 0;
        long        coins = 0;
    } m_counters;

    std::string m_levelTitle;
    /*******************************************/
    bool    m_allowedLeft   = false;
    bool    m_allowedUp     = false;
    bool    m_allowedRight  = false;
    bool    m_allowedDown   = false;

    void        updateAvailablePaths();//!< Checks paths by sides arround player and sets walking permission
    void        updateCenter();

    static void fetchSideNodes(bool &side, std::vector<WorldNode *> &nodes, long cx, long cy);
    void        initElementsVisibility();
    void        saveElementsVisibility();

    bool            m_pathOpeningInProcess = false;
    WldPathOpener   m_pathOpener;

    std::vector<WorldScene_misc_img > m_imgs;
    std::vector<WorldScene_Portrait > m_portraits;

    TileBox                         m_indexTable;
    VPtrList<WldTerrainItem >       m_itemsTerrain;
    VPtrList<WldSceneryItem >       m_itemsSceneries;
    VPtrList<WldPathItem >          m_itemsPaths;
    VPtrList<WldLevelItem >         m_itemsLevels;
    VPtrList<WldMusicBoxItem >      m_itemsMusicBoxes;
    EventQueue<WorldScene >         m_events;

    std::vector<WorldNode*>         m_itemsToRender;


    /*****************Pause Menu*******************/
    enum PauseMenuItems_Menu1
    {
        PAUSE_Continue = 0,
        PAUSE_SaveCont,
        PAUSE_SaveQuit,
        PAUSE_Exit
    };
    enum PauseMenuItems_Menu2
    {
        PAUSE_2_Continue = 0,
        PAUSE_2_Exit
    };
    struct PauseMenu
    {
        int         menuId = 0;
        bool        isShown = false;
        PGE_MenuBox menu;
        bool        isOpened = false;
        void initPauseMenu1(WorldScene *parent);
        void initPauseMenu2(WorldScene *parent);
    } m_pauseMenu;

    void processPauseMenu();
    void processPauseMenuSwitchCharacter();
    /*****************Pause Menu**end**************/

    int     debug_render_delay;
    int     debug_phys_delay;
    int     debug_event_delay;
    int     debug_total_delay;

    LuaWorldEngine luaEngine;
};

#endif // SCENE_WORLD_H
