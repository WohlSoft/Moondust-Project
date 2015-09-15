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

#ifndef SCENE_WORLD_H
#define SCENE_WORLD_H

#include <QString>
#include <QList>
#include "scene.h"
#include <controls/controller.h>
#include <PGE_File_Formats/wld_filedata.h>
#include <common_features/pge_texture.h>
#include <common_features/episode_state.h>
#include <common_features/rect.h>
#include <common_features/point.h>
#include <common_features/event_queue.h>
#include <data_configs/config_manager.h>
#include <gui/pge_menubox.h>

#include "world/wld_tilebox.h"
#include "world/wld_player_portrait.h"

#include <script/lua_world_engine.h>

struct WorldScene_misc_img
{
    int x;
    int y;
    PGE_Texture t;
    SimpleAnimator a;
    int frmH;
};

class WorldScene : public Scene
{
    friend class PGE_MsgBox;
    friend class PGE_TextInputBox;
    friend class PGE_MenuBoxBase;
public:
    WorldScene();
    ~WorldScene();

    bool init();
    bool loadConfigs();

    void onKeyboardPressedSDL(SDL_Keycode sdl_key, Uint16 modifier);
    LuaEngine* getLuaEngine();

    void processEvents();
    void update();
    void render();
    int  exec();

    void tickAnimations(float ticks);

    bool isExit();
    void setExiting(int delay, int reason);

    bool loadFile(QString filePath);

    QString getLastError();
    void setGameState(EpisodeState *_state);

private:
    bool isInit;

    Controller *    player1Controller;
    controller_keys controls_1;

    bool frameSkip;

    EpisodeState *  gameState;
    QString         errorMsg;

    WorldMapSetup   common_setup;
    WorldData       data;

    bool     worldIsContinues;
    bool     lock_controls;

    PGE_Rect viewportRect;

    int     exitWorldDelay;
    int     exitWorldCode;

    int                 numOfPlayers;
    QList<PlayerState > players;

    PGE_Texture         backgroundTex;
    QList<PGE_Texture > textures_bank;

    enum WalkDirections{
        Walk_Idle=0,
        Walk_Left,
        Walk_Right,
        Walk_Up,
        Walk_Down
    };
    double  posX;
    double  posY;
    int     walk_direction;
    float   move_speed;//!< Calculated movement step dependent to physical step
    float   move_steps_count;//!< Speps counterm, used to correct inter-cell position
    void    doMoveStep(double &posVal);
    void    setDir(int dr);

    obj_player     mapwalker_setup;
    PGE_Texture    mapwalker_texture;
    float          mapwalker_img_h;
    SimpleAnimator mapwalker_ani;
    int            mapwalker_offset_x;
    int            mapwalker_offset_y;
    void           mapwalker_refreshDirection();


    void    playMusic(long musicID, QString customMusicFile, bool fade=false, int fadeLen=300);
    void    stopMusic(bool fade=false, int fadeLen=300);
    bool    _playStopSnd;
    bool    _playDenySnd;

    QString currentMusicFile;

    void      jump();
    bool      jumpTo;
    PGE_Point jumpToXY;

    /************Printable stuff****************/
    long      health;
    long      lives;
    long      stars;
    long      points;
    long      coins;
    QString   levelTitle;
    /*******************************************/
    bool    ignore_paths;
    bool    allow_left;
    bool    allow_up;
    bool    allow_right;
    bool    allow_down;
    void    updateAvailablePaths();//!< Checks paths by sides arround player and sets walking permission
    void    updateCenter();
    void    fetchSideNodes(bool &side, QVector<WorldNode *> &nodes, float cx, float cy);

    QVector<WorldScene_misc_img > imgs;
    QVector<WorldScene_Portrait > portraits;

    TileBox _indexTable;
    QList<WldTileItem >     wld_tiles;
    QList<WldSceneryItem >  wld_sceneries;
    QList<WldPathItem >     wld_paths;
    QList<WldLevelItem >    wld_levels;
    QList<WldMusicBoxItem > wld_musicboxes;
    EventQueue<WorldScene > wld_events;

    QList<WorldNode >       wldItems;
    QVector<WorldNode * >   _itemsToRender;


    /*****************Pause Menu*******************/
    enum PauseMenuItems_Menu1
    {
        PAUSE_Continue=0,
        PAUSE_SaveCont,
        PAUSE_SaveQuit,
        PAUSE_Exit
    };
    enum PauseMenuItems_Menu2
    {
        PAUSE_2_Continue=0,
        PAUSE_2_Exit
    };
    int         _pauseMenuID;
    bool        isPauseMenu;
    PGE_MenuBox _pauseMenu;
    bool        _pauseMenu_opened;
    void initPauseMenu1();
    void initPauseMenu2();
    void processPauseMenu();
    /*****************Pause Menu**end**************/

    int     debug_render_delay;
    int     debug_phys_delay;
    int     debug_event_delay;
    int     debug_total_delay;

    LuaWorldEngine luaEngine;
};

#endif // SCENE_WORLD_H
