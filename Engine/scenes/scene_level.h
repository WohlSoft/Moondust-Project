#ifndef SCENE_LEVEL_H
#define SCENE_LEVEL_H

#include "scene.h"
#include "../graphics/graphics.h"
#include "level/lvl_player.h"
#include "../graphics/window.h"

#include <file_formats.h>
#include <Box2D/Box2D.h>
#include <QString>

class LevelScene : public Scene
{
public:
    LevelScene();
    ~LevelScene();

    void init();

    //Init 1
    bool loadFile(QString filePath);

    //Init 2
    bool setEntrance(int entr);
    bool isWarpEntrance;
    QPoint cameraStart;

    //Init 3 -> load Configs

    //Init 4 -> build animators

    //Init 5 -> Init sections

    //Init 6 -> InitPhysics

    //Init 7 -> InitCameras

    //Init 8 -> Init Players

    //Init 9 -> Init blocks

    //Init 10 -> Init BGOs

    //Init 11 -> Init NPCs

    bool prepareLevel();

    void update();
    void render();


    int findNearSection(long x, long y);

    bool isExit();

    //Dummy textures
    PGE_Texture TextureBuffer[3];

    int exitType();

private:
    LevelData data;

    std::vector<PGE_LevelCamera > cameras;
    std::vector<LVL_Player > players;


    b2World *world;
    std::vector<PGE_Texture > textures_bank;
};

#endif // SCENE_LEVEL_H
