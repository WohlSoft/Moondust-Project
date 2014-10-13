#ifndef SCENE_LEVEL_H
#define SCENE_LEVEL_H

#include "scene.h"
#include <file_formats.h>
#include <Box2D/Box2D.h>
#include <QString>

class LevelScene : public Scene
{
public:
    LevelScene();
    void init();
    bool loadFile(QString filePath);
    bool prepareLevel();

    void update();
    void render();

    bool isExit();
    int exitType();

private:
    LevelData data;
    b2World *world;
};

#endif // SCENE_LEVEL_H
