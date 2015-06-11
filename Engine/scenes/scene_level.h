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

#ifndef SCENE_LEVEL_H
#define SCENE_LEVEL_H

#include "scene.h"
#include <graphics/graphics.h>
#include <graphics/window.h>

#include <common_features/pge_texture.h>
#include <common_features/episode_state.h>
#include <common_features/event_queue.h>
#include <common_features/point.h>

#include <common_features/RTree/RTree.h>

#include <gui/pge_menubox.h>

#include "level/lvl_player.h"
#include "level/lvl_player_def.h"

#include "level/lvl_block.h"
#include "level/lvl_bgo.h"
#include "level/lvl_npc.h"

#include "level/lvl_effect.h"

#include "level/lvl_physenv.h"

#include "level/lvl_warp.h"
#include "level/lvl_section.h"
#include "level/lvl_backgrnd.h"

#include <controls/controller.h>

#include <data_configs/custom_data.h>

#include <script/lua_level_engine.h>

#include <PGE_File_Formats/file_formats.h>
#include <Box2D/Box2D.h>
#include <QString>
#include <QList>
#include <QVector>

#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_timer.h>

struct transformTask_block
{
    LVL_Block *block;
    int id;
    int type;
};


class LevelScene : public Scene
{
public:
    LevelScene();
    ~LevelScene();

    bool init();

    //Init 1
    bool        loadFile(QString filePath);
    bool        loadFileIP(); //!< Load data via interprocessing

    //Init 2
    bool        setEntrance(int entr);
private:
    bool        isWarpEntrance;
    PGE_Point   cameraStart;
    bool        cameraStartDirected;
    int         cameraStartDirection;

    LevelDoor   startWarp;
    int         NewPlayerID;

public:
    PlayerPoint getStartLocation(int playerID);

    QList<LVL_PlayerDef > player_defs;

    bool loadConfigs();

    void onKeyboardPressedSDL(SDL_Keycode sdl_key, Uint16 modifier);
    LuaEngine* getLuaEngine();

    void update();
    void processEvents();
    void render();
    int exec();

    void tickAnimations(float ticks);

    QString getLastError();


    int findNearSection(long x, long y);

    bool isExit();

    //Dummy textures
    PGE_Texture TextureBuffer[3];

    int exitType();

    //Flags
    bool isTimeStopped;

    /****************Level Running State*****************/
    bool isLevelContinues;

    void checkPlayers();
    void setExiting(int delay, int reason);

    QString     toAnotherLevel();
    QString     warpToLevelFile;
    long        lastWarpID;

    int         toAnotherEntrance();
    int         warpToArrayID;

    PGE_Point   toWorldXY();
    bool        warpToWorld;
    PGE_Point   warpToWorldXY;

    float       exitLevelDelay;
    int         exitLevelCode;
    /****************Level Running State*****************/

    int numberOfPlayers;

    Controller* player1Controller;
    Controller* player2Controller;


    /**************Z-Layers**************/
    static double zCounter;

    static const double Z_backImage; //Background

    //Background-2
    static const double Z_BGOBack2; // backround BGO

    static const double Z_blockSizable; // sizable blocks

    //Background-1
    static const double Z_BGOBack1; // backround BGO

    static const double Z_npcBack; // background NPC
    static const double Z_Block; // standart block
    static const double Z_npcStd; // standart NPC
    static const double Z_Player; //player Point

    //Foreground-1
    static const double Z_BGOFore1; // foreground BGO
    static const double Z_BlockFore; //LavaBlock
    static const double Z_npcFore; // foreground NPC
    //Foreground-2
    static const double Z_BGOFore2; // foreground BGO

    static const double Z_sys_PhysEnv;
    static const double Z_sys_door;
    static const double Z_sys_interspace1; // interSection space layer
    static const double Z_sys_sctBorder; // section Border
    /**************Z-Layers**************/


    /**************LoadScreen**************/
    int loaderSpeed;
    bool IsLoaderWorks;
    void drawLoader();
    void setLoaderAnimation(int speed);
    void stopLoaderAnimation();
    static unsigned int nextLoadAniFrame(unsigned int x, void *p);
    void loaderTick();
    bool doLoaderStep;
    void loaderStep();
    SDL_TimerID loader_timer_id;
    /**************LoadScreen**************/

    LevelData *levelData();

    QQueue<transformTask_block > block_transforms;

    QMap<int, QList<LVL_Block* > > switch_blocks;
    void toggleSwitch(int switch_id);

    QVector<LVL_Npc* > active_npcs;
    QVector<LVL_Npc* > dead_npcs;

    QVector<LVL_Block* > fading_blocks;

    /*********************Item placing**********************/
    void placeBlock(LevelBlock blockData);
    void placeBGO(LevelBGO bgoData);
    void placeNPC(LevelNPC npcData);

    void addPlayer(PlayerPoint playerData, bool byWarp=false, int warpType=0, int warpDirect=0);
    /*********************Item placing**********************/

    void destroyBlock(LVL_Block *&_block);
    void setGameState(EpisodeState *_gameState);

    EventQueue<LevelScene > system_events;
    LVL_Section *getSection(int sct);

private:
    bool isInit;

    LevelData data;

    EpisodeState *gameState;
    QString errorMsg;

    bool frameSkip;

    typedef QList<PGE_LevelCamera> LVL_CameraList;
    typedef QList<LVL_Section> LVL_SectionsList;

    LVL_CameraList      cameras;
    LVL_SectionsList    sections;


    /*  Effects engine   */
    typedef QList<Scene_Effect>    SceneEffectsArray;
    SceneEffectsArray  WorkingEffects;
    void launchStaticEffect(long effectID, float startX, float startY, int animationLoops, int delay, float velocityX, float velocityY, float gravity);
    void processEffects(float ticks);
    /*  Effects engine   */



    typedef QVector<LVL_Player* >  LVL_PlayersArray;
    typedef QVector<LVL_Block* >   LVL_BlocksArray;
    typedef QVector<LVL_Bgo* >     LVL_BgosArray;
    typedef QVector<LVL_Npc* >     LVL_NpcsArray;
    typedef QVector<LVL_Warp* >    LVL_WarpsArray;
    typedef QVector<LVL_PhysEnv* > LVL_PhysEnvsArray;

    LVL_PlayersArray    players;
    LVL_BlocksArray     blocks;
    LVL_BgosArray       bgos;
    LVL_NpcsArray       npcs;
    LVL_WarpsArray      warps;
    LVL_PhysEnvsArray   physenvs;

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

    bool debug_player_jumping;
    bool debug_player_onground;
    int  debug_player_foots;
    int  debug_render_delay;
    int  debug_phys_delay;
    int  debug_event_delay;

public:
    void registerElement(PGE_Phys_Object* item);
    void unregisterElement(PGE_Phys_Object* item);
    typedef double RPoint[2];
    void queryItems(PGE_RectF zone, QVector<PGE_Phys_Object* > *resultList);
    void queryItems(double x, double y, QVector<PGE_Phys_Object* > *resultList);
private:
    typedef RTree<PGE_Phys_Object*, double, 2, double > IndexTree;
    IndexTree tree;

    b2World *world;
    QList<PGE_Texture > textures_bank;

    LuaLevelEngine luaEngine;
};


#endif // SCENE_LEVEL_H
