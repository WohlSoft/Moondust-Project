/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "level/lvl_physenv.h"

#include "level/lvl_warp.h"
#include "level/lvl_section.h"
#include "level/lvl_backgrnd.h"

#include "level/lvl_layer_engine.h"
#include "level/lvl_event_engine.h"
#include "level/lvl_player_switch.h"

#include "level/lvl_z_constants.h"

#include <controls/controller.h>

#include <data_configs/custom_data.h>

#include <script/lua_level_engine.h>
#include <script/bindings/level/classes/luaclass_level_playerstate.h>

#include <PGE_File_Formats/file_formats.h>
#include <QString>
#include <QList>
#include <QVector>

#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_timer.h>

struct transformTask_block
{
    LVL_Block *block;
    unsigned long id;
    int type;
};


class LevelScene : public Scene
{
        friend class LVL_EventEngine;
        friend class PGE_LevelCamera;
        friend class CharacterSwitcherEngine;
        friend class Binding_Level_CommonFuncs;
    public:
        LevelScene();
        ~LevelScene();

        bool init();
        bool init_items();
        static int init_thread(void *self);
        SDL_Thread *initializer_thread;
    private:
        bool isInit;
        bool isInitFinished;
        bool isInitFailed;
    public:

        //Init 1
        bool        loadFile(QString filePath);
        bool        loadFileIP(); //!< Load data via interprocessing

        //Init 2
        bool        setEntrance(unsigned long entr);
    private:
        bool        isWarpEntrance;
        PGE_PointF  cameraStart;
        bool        cameraStartDirected;
        int         cameraStartDirection;

        LevelDoor   startWarp;
        int         NewPlayerID;

    public:
        PlayerPoint getStartLocation(int playerID);

        QHash<unsigned int, LVL_PlayerDef > player_defs;

        bool loadConfigs();

        void onKeyboardPressedSDL(SDL_Keycode sdl_key, Uint16 modifier);
        void onMousePressed(SDL_MouseButtonEvent &mbevent);
        void onMouseMoved(SDL_MouseMotionEvent &mvevent);
        void onMouseReleased(SDL_MouseButtonEvent &mvevent);

        LuaEngine *getLuaEngine();

        void update();
        void processEvents();
        void render();
        int exec();

        void tickAnimations(double ticks);

        QString getLastError();


        int findNearestSection(long x, long y);

        bool isExit();

        //Dummy textures
        PGE_Texture TextureBuffer[3];

        int exitType();

        //Flags
        bool isTimeStopped;

        /****************Level Running State*****************/
        bool isLevelContinues;

        void setExiting(int delay, int reason);

        QString     toAnotherLevel();
        QString     warpToLevelFile;
        unsigned long lastWarpID;

        unsigned long toAnotherEntrance();
        unsigned long warpToArrayID;

        PGE_Point   toWorldXY();
        bool        warpToWorld;
        PGE_Point   warpToWorldXY;

        double      exitLevelDelay;
        int         exitLevelCode;
        /****************Level Running State*****************/

        int numberOfPlayers;

        Controller *player1Controller;
        Controller *player2Controller;

        //Garbage collecting
        void collectGarbageNPCs();
        void collectGarbagePlayers();

        /**************Z-Layers**************/
        long double  zCounter;
        static const LevelZOrderTable zOrder;
        /**************Z-Layers**************/


        /**************LoadScreen**************/
        int loaderSpeed;
        bool IsLoaderWorks;
        void drawLoader();
        void setLoaderAnimation(int speed);
        void stopLoaderAnimation();
        void destroyLoaderTexture();
        static unsigned int nextLoadAniFrame(unsigned int x, void *p);
        void loaderTick();
        bool doLoaderStep;
        void loaderStep();
        SDL_TimerID loader_timer_id;
        /**************LoadScreen**************/

        /*!
         * \brief Get pointer to file data structure used to initialize level
         * \return pointer to the level file data structure
         */
        LevelData *levelData();

        //! Queue of blocks which are requires transformation
        QQueue<transformTask_block > block_transforms;

        /**********************Switch blocks*************************/
        //! Table of registered switchable blocks per SwitchID
        QHash<unsigned int, QList<LVL_Block * > > switch_blocks;

        /*!
         * \brief Toggles switch by SwitchID
         * \param switch_id ID of the switch
         */
        void toggleSwitch(unsigned int switch_id);
        /*!
         * \brief Get state of the SwitchID
         * \param switch_id SwitchID to check state
         * \return true or false state of the SwitchID
         */
        bool lua_switchState(int switch_id);

        //! States of the SwitchID's, Has no effect on switchable blocks, used only to tell Lua scripts which current state of the switches now is
        QList<bool> switch_states;
        /**********************Switch blocks*************************/
        /*************************Character switchers*************************/
        CharacterSwitcherEngine character_switchers;
        /*************************Character switchers*************************/

        /**********************NPC Management*********************/
        QVector<LVL_Npc * > active_npcs;
        QVector<LVL_Npc * > dead_npcs;
        /**********************NPC Management*********************/

        QVector<LVL_Player * > dead_players;

        QVector<LVL_Block * > fading_blocks;

        /*********************Item placing**********************/
        /*********************Initial*placing*******************/
        void placeBlock(LevelBlock blockData);
        void placeBGO(LevelBGO bgoData);
        void placeNPC(LevelNPC npcData);
        void addPlayer(PlayerPoint playerData, bool byWarp = false, int warpType = 0, int warpDirect = 0, bool cannon = false, double cannon_speed = 10.0);
        /*******************************************************/

        /*********************Dynamical*spawn*******************/
        enum NpcSpawnType
        {
            GENERATOR_APPEAR = 0,
            GENERATOR_WARP,
            GENERATOR_PROJECTILE
        };
        enum NpcSpawnDirection
        {
            SPAWN_LEFT = 2,
            SPAWN_RIGHT = 4,
            SPAWN_UP = 1,
            SPAWN_DOWN = 3,
        };
        LVL_Block *spawnBlock(LevelBlock blockData);
        LVL_Bgo   *spawnBGO(LevelBGO bgoData);
        LVL_Npc   *spawnNPC(LevelNPC npcData, NpcSpawnType sp_type, NpcSpawnDirection sp_dir, bool reSpawnable = false);
        /*******************************************************/

        /********************Interprocess Stuff*****************/
        bool       placingMode;
        int        placingMode_item_type;
        LevelBlock placingMode_block;
        LevelBGO   placingMode_bgo;
        LevelNPC   placingMode_npc;
        PGE_PointF placingMode_renderAt;
        PGE_PointF placingMode_renderOffset;
        PGE_PointF placingMode_drawSize;
        /********************Rect***********************/
        bool       placingMode_sizableBlock;
        bool       placingMode_rect_draw;
        PGE_PointF placingMode_drawBegin;
        PGE_PointF placingMode_drawEnd;
        /********************Rect***********************/

        /********************Placing element's texture***********************/
        bool        placingMode_animated;
        int         placingMode_animatorID;
        PGE_Texture placingMode_texture;
        /********************Placing element's texture***********************/

        void       process_InterprocessCommands();

        void       drawPlacingItem();
        void       placeItemByMouse(int x, int y);
        /*******************************************************/

        /*********************Item placing**end*****************/

        void destroyBlock(LVL_Block *&_block);
        void setGameState(EpisodeState *_gameState);

        EventQueue<LevelScene > system_events;

        LVL_Section *getSection(int sct);
        EpisodeState *getGameState();

        bool isVizibleOnScreen(PGE_RectF &rect);
        bool isVizibleOnScreen(PGE_Phys_Object::Momentum &momentum);
        bool isVizibleOnScreen(double x, double y, double w, double h);

    private:
        LevelData data;

        EpisodeState *gameState;
        QString errorMsg;

        bool frameSkip;

        typedef QList<PGE_LevelCamera> LVL_CameraList;
        typedef QList<LVL_Section> LVL_SectionsList;

        LVL_CameraList      cameras;
        LVL_SectionsList    sections;

    public:
        QList<lua_LevelPlayerState>    player_states;

        typedef QVector<LVL_Player * >  LVL_PlayersArray;
        typedef QVector<LVL_Block * >   LVL_BlocksArray;
        typedef QVector<LVL_Bgo * >     LVL_BgosArray;
        typedef QVector<LVL_Npc * >     LVL_NpcsArray;
        typedef QVector<LVL_Warp * >    LVL_WarpsArray;
        typedef QVector<LVL_PhysEnv * > LVL_PhysEnvsArray;

        LVL_LayerEngine     layers;
        LVL_EventEngine     events;

        LVL_PlayersArray    players;
        LVL_BlocksArray     blocks;
        LVL_BgosArray       bgos;
        LVL_NpcsArray       npcs;
        LVL_WarpsArray      warps;
        LVL_PhysEnvsArray   physenvs;

    private:
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
        enum PauseMenuItems_Menu3
        {
            PAUSE_3_Continue = 0,
            PAUSE_3_Replay,
            PAUSE_3_Exit
        };
        int         _pauseMenuID;
        bool        m_isPauseMenu;
        PGE_MenuBox m_pauseMenu;
        bool        _pauseMenu_opened;
        void initPauseMenu1();
        void initPauseMenu2();
        void initPauseMenu3();
        void processPauseMenu();
        /*****************Pause Menu**end**************/

        bool debug_player_jumping;
        bool debug_player_onground;
        int  debug_player_foots;
        int  debug_render_delay;
        int  debug_phys_delay;
        int  debug_event_delay;

    public:
        double globalGravity;
        void processPhysics(double ticks);
        void processAllCollisions();

        typedef PGE_Phys_Object *PhysObjPtr;
    private:
        typedef RTree<PhysObjPtr, double, 2, double > IndexTree;

    public:
        void registerElement(PhysObjPtr item);
        void unregisterElement(PhysObjPtr item);
        typedef double RPoint[2];
        void queryItems(PGE_RectF &zone, QVector<PGE_Phys_Object * > *resultList);
        void queryItems(double x, double y, QVector<PGE_Phys_Object * > *resultList);

        LVL_PlayersArray &getPlayers();
        LVL_NpcsArray &getNpcs();
        LVL_NpcsArray &getActiveNpcs();

        LVL_BlocksArray &getBlocks();
        LVL_BgosArray   &getBGOs();

    private:
        IndexTree tree;

        QList<PGE_Texture > textures_bank;

        LuaLevelEngine luaEngine;
};


#endif // SCENE_LEVEL_H
