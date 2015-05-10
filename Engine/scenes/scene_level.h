/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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
#include "../graphics/graphics.h"

#include "../common_features/pge_texture.h"

#include <common_features/episode_state.h>
#include <common_features/event_queue.h>

#include "level/lvl_player.h"
#include "level/lvl_player_def.h"

#include "level/lvl_block.h"
#include "level/lvl_bgo.h"
#include "level/lvl_npc.h"

#include "level/lvl_physenv.h"

#include "level/lvl_warp.h"
#include "level/lvl_backgrnd.h"

#include "../graphics/window.h"

#include "../controls/controller_keyboard.h"

#include "../data_configs/custom_data.h"

#include <PGE_File_Formats/file_formats.h>
#include <Box2D/Box2D.h>
#include <QString>
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
    bool isInit;

    //Init 1
    bool loadFile(QString filePath);
    bool loadFileIP(); //!< Load data via interprocessing

    //Init 2
    bool setEntrance(int entr);
    bool isWarpEntrance;
    QPoint cameraStart;
    bool   cameraStartDirected;
    int    cameraStartDirection;
    LevelDoor startWarp;
    int NewPlayerID;

    QList<LVL_PlayerDef > player_defs;

    bool loadConfigs();

    void update();
    void render();
    int exec();

    QString getLastError();


    int findNearSection(long x, long y);

    bool isExit();

    //Dummy textures
    PGE_Texture TextureBuffer[3];

    int exitType();

    //Flags
    bool isPauseMenu;
    bool isTimeStopped;

    /****************Level Running State*****************/
    bool isLevelContinues;
    bool doExit;

    void checkPlayers();
    void setExiting(int delay, int reason);

    QString toAnotherLevel();
    QString warpToLevelFile;
    long lastWarpID;

    int toAnotherEntrance();
    int warpToArrayID;

    QPoint toWorldXY();
    bool warpToWorld;
    QPoint warpToWorldXY;

    int exitLevelDelay;
    int exitLevelCode;
    /****************Level Running State*****************/

    int numberOfPlayers;

    KeyboardController keyboard1;


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


    QVector<LVL_Background *> * bgList();
    LevelData *levelData();

    QQueue<transformTask_block > block_transfors;

    QMap<int, QList<LVL_Block* > > switch_blocks;
    void toggleSwitch(int switch_id);

    /*********************Item placing**********************/
    void placeBlock(LevelBlock blockData);
    void placeBGO(LevelBGO bgoData);
    void placeNPC(LevelNPC npcData);

    void addPlayer(PlayerPoint playerData, bool byWarp=false, int warpType=0, int warpDirect=0);
    /*********************Item placing**********************/

    void destroyBlock(LVL_Block * _block);
    void setGameState(EpisodeState *_gameState);

    EventQueue<LevelScene > system_events;

private:
    LevelData data;

    EpisodeState *gameState;

    QVector<PGE_LevelCamera* > cameras;
    QVector<LVL_Player* > players;
    QVector<LVL_Block* > blocks;
    QVector<LVL_Bgo* > bgos;
    QVector<LVL_Npc* > npcs;
    QVector<LVL_Warp* > warps;
    QVector<LVL_PhysEnv* > physenvs;


    QVector<LVL_Background *> backgrounds;

    QString errorMsg;


    b2World *world;
    QVector<PGE_Texture > textures_bank;
};

#endif // SCENE_LEVEL_H
