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
#include "level/lvl_player.h"
#include "level/lvl_block.h"
#include "level/lvl_bgo.h"

#include "level/lvl_warp.h"


#include "../graphics/graphics_lvl_backgrnd.h"

#include "../graphics/window.h"

#include "../controls/controller_keyboard.h"

#include "../data_configs/custom_data.h"

#include <file_formats.h>
#include <Box2D/Box2D.h>
#include <QString>
#include <QVector>

#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_timer.h>

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
    LevelDoors startWarp;
    int NewPlayerID;

    //Init 3 -> load Configs
    bool loadConfigs();

    //Init 4 -> build animators

    //Init 5 -> Init sections

    //Init 6 -> InitPhysics

    //Init 7 -> InitCameras

    //Init 8 -> Init Players

    //Init 9 -> Init blocks

    //Init 10 -> Init BGOs

    //Init 11 -> Init NPCs

    void update(float step=10);
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
    enum exitLevelCodes
    {
        EXIT_MenuExit=-3,
        EXIT_Error=-2,
        EXIT_PlayerDeath=-1,
        EXIT_Closed=0,
        EXIT_Card=1,
        EXIT_Ball=2,
        EXIT_OffScreen=3,
        EXIT_Key=4,
        EXIT_Crystal=5,
        EXIT_Warp=6,
        EXIT_Star=7,
        EXIT_Tape=8
    };

    bool doExit;

    void checkPlayers();
    void setExiting(int delay, int reason);

    QString toAnotherLevel();
    QString warpToLevelFile;

    int toAnotherEntrance();
    int warpToArrayID;

    int exitLevelDelay;
    int exitLevelCode;
    /****************Level Running State*****************/

    int numberOfPlayers;

    KeyboardController keyboard1;


    /**************Z-Layers**************/
    double Z_backImage; //Background

    //Background-2
    double Z_BGOBack2; // backround BGO

    double Z_blockSizable; // sizable blocks

    //Background-1
    double Z_BGOBack1; // backround BGO

    double Z_npcBack; // background NPC
    double Z_Block; // standart block
    double Z_npcStd; // standart NPC
    double Z_Player; //player Point

    //Foreground-1
    double Z_BGOFore1; // foreground BGO
    double Z_BlockFore; //LavaBlock
    double Z_npcFore; // foreground NPC
    //Foreground-2
    double Z_BGOFore2; // foreground BGO

    double Z_sys_PhysEnv;
    double Z_sys_door;
    double Z_sys_interspace1; // interSection space layer
    double Z_sys_sctBorder; // section Border
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

    /*********************Item placing**********************/
    void placeBlock(LevelBlock blockData);
    void placeBGO(LevelBGO bgoData);

    void addPlayer(PlayerPoint playerData, bool byWarp=false);
    /*********************Item placing**********************/

    void destroyBlock(LVL_Block * _block);

private:
    LevelData data;

    QVector<PGE_LevelCamera* > cameras;
    QVector<LVL_Player* > players;
    QVector<LVL_Block* > blocks;
    QVector<LVL_Bgo* > bgos;
    QVector<LVL_Warp* > warps;

    QVector<LVL_Background *> backgrounds;

    QString errorMsg;


    b2World *world;
    QVector<PGE_Texture > textures_bank;
};

#endif // SCENE_LEVEL_H
