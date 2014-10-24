#ifndef SCENE_LEVEL_H
#define SCENE_LEVEL_H

#include "scene.h"
#include "../graphics/graphics.h"

#include "../common_features/pge_texture.h"
#include "level/lvl_player.h"
#include "level/lvl_block.h"
#include "level/lvl_bgo.h"
#include "../graphics/graphics_lvl_backgrnd.h"

#include "../graphics/window.h"

#include "../controls/controller_keyboard.h"

#include "../data_configs/custom_data.h"

#include <file_formats.h>
#include <Box2D/Box2D.h>
#include <QString>
#include <QVector>

#include <SDL2/SDL_opengl.h>

class LevelScene : public Scene
{
public:
    LevelScene();
    ~LevelScene();

    bool init();

    //Init 1
    bool loadFile(QString filePath);

    //Init 2
    bool setEntrance(int entr);
    bool isWarpEntrance;
    QPoint cameraStart;

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

    bool prepareLevel();

    void update(float step=10);
    void render();


    int findNearSection(long x, long y);

    bool isExit();

    //Dummy textures
    PGE_Texture TextureBuffer[3];

    int exitType();

    //Flags
    bool isPauseMenu;
    bool isTimeStopped;
    bool isLevelContinues;
    int exitLevelDelay;
    int exitLevelCode;
    int numberOfPlayers;

    KeyboardController keyboard1;

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

private:
    LevelData data;

    QVector<PGE_LevelCamera* > cameras;
    QVector<LVL_Player* > players;
    QVector<LVL_Block* > blocks;
    QVector<LVL_Bgo* > bgos;

    QVector<LVL_Background *> backgrounds;


    b2World *world;
    QVector<PGE_Texture > textures_bank;
};

#endif // SCENE_LEVEL_H
