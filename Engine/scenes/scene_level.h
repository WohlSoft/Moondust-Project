/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
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
#include "level/lvl_quad_tree.h"

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

#include <Utils/elapsed_timer.h>

#include <PGE_File_Formats/file_formats.h>
#include <vector>
#include <deque>
#include <unordered_map>
#include <unordered_set>

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
        friend void  levelSceneLoopStep(void *scene);
    public:
        LevelScene();
        ~LevelScene();

        bool init();
        bool init_items();
        static int init_thread(void *self);
        SDL_Thread *m_initializer_thread = nullptr;
    private:
        bool m_isInit = false;
        bool m_isInitFinished = false;
        bool m_isInitFailed = false;
    public:
        //Init 1
        bool        loadFile(std::string filePath);
        bool        loadFileIP(); //!< Load data via interprocessing

        //Init 2
        bool        setEntrance(unsigned long entr);
    private:
        bool        m_isWarpEntrance;
        PGE_PointF  m_cameraStart;
        bool        m_cameraStartDirected;
        int         m_cameraStartDirection;

        LevelDoor   m_warpInitial;
        int         m_newPlayerID;

    public:
        PlayerPoint getStartLocation(int playerID);

        typedef std::unordered_map<uint32_t, LVL_PlayerDef > PlayerDefMap;
        PlayerDefMap player_defs;

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

        std::string getLastError();

        int findNearestSection(long x, long y);

        bool isExit();

        //Dummy textures
        PGE_Texture m_textureBuffer[3];

        int exitType();

        //Flags
        bool m_isTimeStopped;

        /****************Level Running State*****************/
        bool m_isLevelContinues;

        void setExiting(int delay, int reason);

        std::string     toAnotherLevel();
        std::string     m_warpToLevelFile;
        unsigned long   m_lastWarpID;

        unsigned long toAnotherEntrance();
        unsigned long m_warpToArrayID;

        PGE_Point   toWorldXY();
        bool        m_warpToWorld;
        PGE_Point   m_warpToWorldXY;

        double      m_exitLevelDelay;
        int         m_exitLevelCode;
        /****************Level Running State*****************/

        int m_numberOfPlayers = 1;

        Controller *m_player1Controller = nullptr;
        Controller *m_player2Controller = nullptr;

        //Garbage collecting
        void collectGarbageNPCs();
        void collectGarbagePlayers();
        void collectGarbageBlocks();

        /**************Z-Layers**************/
        long double  m_zCounter = 0.0l;
        static const LevelZOrderTable zOrder;
        /**************Z-Layers**************/


        /**************LoadScreen**************/
        int     m_loaderSpeed = 32;
        bool    m_loaderIsWorks = false;
        void    drawLoader();
        void    setLoaderAnimation(int speed);
        void    stopLoaderAnimation();
        void    destroyLoaderTexture();
        static  unsigned int nextLoadAniFrame(unsigned int x, void *p);
        void    loaderTick();
        bool    m_loaderDoStep = false;
        void    loaderStep();
        SDL_TimerID m_loader_timer_id = 0;
        /**************LoadScreen**************/

        /*!
         * \brief Get pointer to file data structure used to initialize level
         * \return pointer to the level file data structure
         */
        LevelData *levelData();

        //! Queue of blocks which are requires transformation
        std::deque<transformTask_block > m_blockTransforms;

        /**********************Switch blocks*************************/
        typedef std::vector<LVL_Block *>                    BlocksList;
        typedef std::unordered_map<uint32_t, BlocksList >   SwitchBlocksMap;

        //! Table of registered switchable blocks per SwitchID
        SwitchBlocksMap     m_switchBlocks;

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
        bool lua_switchState(uint32_t switch_id);

        //! States of the SwitchID's, Has no effect on switchable blocks, used only to tell Lua scripts which current state of the switches now is
        std::vector<bool>       m_switchStates;
        /**********************Switch blocks*************************/
        /*************************Character switchers*************************/
        CharacterSwitcherEngine m_characterSwitchers;
        /*************************Character switchers*************************/

        /**********************NPC Management*********************/
        typedef std::unordered_set<LVL_Npc * > LVL_NpcActiveSet;
        //! List of activated NPCs
        LVL_NpcActiveSet m_npcActive;
        //! List of dead NPCs
        std::vector<LVL_Npc * > m_npcDead;
        /**********************NPC Management*end*****************/
        //! List of dead playable character
        std::vector<LVL_Player * >  m_playersDead;
        //! Blocks are have fading animation in process
        std::vector<LVL_Block * >   m_blocksInFade;
        typedef std::unordered_set<LVL_Block*> LVL_BlocksSet;
        //! Blocks are pending to be completely deleted
        std::vector<LVL_Block * >   m_blocksToDelete;
        //! List of destroyed blocks
        LVL_BlocksSet m_blocksDestroyed;
        void restoreDestroyedBlocks(bool smoke = true);
        /*********************Item placing**********************/
        /*********************Initial*placing*******************/
        void placeBlock(LevelBlock& blockData);
        void placeBGO(LevelBGO &bgoData);
        void placeNPC(LevelNPC &npcData);
        bool addPlayer(PlayerPoint playerData, bool byWarp = false, int warpType = 0, int warpDirect = 0, bool cannon = false, double cannon_speed = 10.0);
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
            SPAWN_LEFT  = 2,
            SPAWN_RIGHT = 4,
            SPAWN_UP    = 1,
            SPAWN_DOWN  = 3,
        };
        LVL_Block *spawnBlock(const LevelBlock &blockData);
        LVL_Bgo   *spawnBGO(const LevelBGO &bgoData);
        LVL_Npc   *spawnNPC(const LevelNPC &npcData, NpcSpawnType sp_type, NpcSpawnDirection sp_dir, bool reSpawnable = false);
        /*******************************************************/

        /********************Interprocess Stuff*****************/
        bool       m_placingMode;
        int        m_placingMode_item_type;
        LevelBlock m_placingMode_block;
        LevelBGO   m_placingMode_bgo;
        LevelNPC   m_placingMode_npc;
        PGE_PointF m_placingMode_renderAt;
        PGE_PointF m_placingMode_renderOffset;
        PGE_PointF m_placingMode_drawSize;
        /********************Rect***********************/
        bool       m_placingMode_sizableBlock;
        bool       m_placingMode_rect_draw;
        PGE_PointF m_placingMode_drawBegin;
        PGE_PointF m_placingMode_drawEnd;
        /********************Rect***********************/

        /********************Placing element's texture***********************/
        bool        m_placingMode_animated;
        int         m_placingMode_animatorID;
        PGE_Texture m_placingMode_texture;
        /********************Placing element's texture***********************/

        void        process_InterprocessCommands();

        void        drawPlacingItem();
        void        placeItemByMouse(int x, int y);
        /*******************************************************/

        /*********************Item placing**end*****************/

        void setGameState(EpisodeState *_gameState);

        EventQueue<LevelScene > m_systemEvents;

        LVL_Section     *getSection(int sct);
        EpisodeState    *getGameState();

        bool isVizibleOnScreen(PGE_RectF &rect);
        bool isVizibleOnScreen(PGE_Phys_Object::Momentum &momentum);
        bool isVizibleOnScreen(double x, double y, double w, double h);

    private:
        LevelData m_data;

        EpisodeState    *m_gameState = nullptr;
        std::string     m_errorMsg;

        bool            m_frameSkip;

        typedef VPtrList<PGE_LevelCamera>   LVL_CameraList;
        typedef VPtrList<LVL_Section>       LVL_SectionsList;

    public:
        bool m_blinkStateFlag = false;

    private:
        LVL_CameraList      m_cameras;
        LVL_SectionsList    m_sections;


    public:
        std::vector<lua_LevelPlayerState>   m_playerStates;

        typedef std::vector<LVL_Player * >  LVL_PlayersArray;
        typedef std::unordered_set<LVL_Block * >   LVL_BlocksArray;
        typedef std::vector<LVL_Block * >   LVL_BlocksVector;
        typedef std::unordered_set<LVL_Bgo * >     LVL_BgosArray;
        typedef std::unordered_set<LVL_Npc * >     LVL_NpcsArray;
        typedef std::unordered_set<LVL_Warp * >    LVL_WarpsArray;
        typedef std::unordered_set<LVL_PhysEnv * > LVL_PhysEnvsArray;

        LVL_LayerEngine     m_layers;
        LVL_EventEngine     m_events;

        LVL_PlayersArray    m_itemsPlayers;
        LVL_BlocksArray     m_itemsBlocks;
        LVL_BgosArray       m_itemsBgo;
        LVL_NpcsArray       m_itemsNpc;
        LVL_WarpsArray      m_itemsWarps;
        LVL_PhysEnvsArray   m_itemsPhysEnvs;

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

        int         m_pauseMenuID;
        bool        m_isPauseMenu;
        PGE_MenuBox m_pauseMenu;
        bool        m_pauseMenu_opened;
        void initPauseMenu1();
        void initPauseMenu2();
        void initPauseMenu3();
        void processPauseMenu();
        /*****************Pause Menu**end**************/

        bool m_debug_player_jumping = false;
        bool m_debug_player_onground= false;
        int  m_debug_player_foots   = 0;
        int  m_debug_render_delay   = 0;
        int  m_debug_phys_delay     = 0;
        int  m_debug_event_delay    = 0;

        ElapsedTimer debug_TimeReal;
        int          debug_TimeCounted  = 0;
        bool m_debug_slowTimeMode       = false;
        bool m_debug_oneStepMode        = false;
        bool m_debug_oneStepMode_doStep = false;

    public:
        double m_globalGravity = 1.0;
        void processPhysics(double ticks);
        void processAllCollisions();

        typedef PGE_Phys_Object *PhysObjPtr;
    private:
        typedef RTree<PhysObjPtr, double, 2, double > IndexTree;
        typedef LvlQuadTree IndexTree4;

    public:
        void registerElement(PhysObjPtr item);
        void updateElement(PhysObjPtr item);
        void unregisterElement(PhysObjPtr item);
        typedef double RPoint[2];
        void queryItems(PGE_RectF &zone,
                        std::vector<PGE_Phys_Object * > *resultList,
                        std::function<bool(PGE_Phys_Object*)> *validator = nullptr);
        void queryItems(double x, double y,
                        std::vector<PGE_Phys_Object *> *resultList,
                        std::function<bool(PGE_Phys_Object*)> *validator = nullptr);

        LVL_PlayersArray &getPlayers();
        LVL_NpcsArray &getNpcs();
        LVL_NpcActiveSet &getActiveNpcs();

        LVL_BlocksArray &getBlocks();
        LVL_BgosArray   &getBGOs();

    private:
        IndexTree                   m_tree;
        IndexTree4                  m_qtree;
        std::vector<PGE_Texture >   m_texturesBank;

        LuaLevelEngine              m_luaEngine;
};


#endif // SCENE_LEVEL_H
