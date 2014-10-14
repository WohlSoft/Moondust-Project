#include "scene_level.h"

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include "common_features/app_path.h"
#include "common_features/graphics_funcs.h"

LevelScene::LevelScene()
{
    data = FileFormats::dummyLvlDataArray();
    data.ReadFileValid = false;

    isWarpEntrance=false;


    world=NULL;
}

LevelScene::~LevelScene()
{
    textures_bank.clear();
}

void LevelScene::init()
{

    //Load File

    //Set Entrance  (int entr=0)

    //Load configs

    //Create Animators
    //look for necessary textures and load them into bank

    //Generate texture bank
    textures_bank.push_back( GraphicsHelps::loadTexture(ApplicationPath + "/block-223.bmp") );
    textures_bank.push_back( GraphicsHelps::loadTexture(ApplicationPath + "/bg.bmp") );
    textures_bank.push_back( GraphicsHelps::loadTexture(ApplicationPath + "/bgo.bmp") );


    //InitSections

    //Init Physics
    b2Vec2 gravity(0.0f, 150.0f);
    world = new b2World(gravity);

    //Init Cameras
    PGE_LevelCamera camera;
    camera.setWorld(world);
    camera.init(
                    (float)data.sections[0].size_left,
                    (float)data.sections[0].size_right,
                    (float)PGE_Window::Width, (float)PGE_Window::Height
                );
    cameras.push_back(camera);

    //Init data
    foreach(PlayerPoint p, data.players)
    {
        LVL_Player player;
        player.camera = &(cameras[0]);
        player.worldPtr = world;
        player.setSize(p.w, p.h);
        player.data = &p;
        player.init();
        players.push_back(player);
    }


}



bool LevelScene::loadFile(QString filePath)
{
    QFile file(filePath );
    QFileInfo in_1(filePath);

    if (file.open(QIODevice::ReadOnly))
    {
        if(filePath.endsWith(".lvl", Qt::CaseInsensitive))
            data = FileFormats::ReadLevelFile(file);
        else
            data = FileFormats::ReadExtendedLevelFile(file);
        data.filename = in_1.baseName();
        data.path = in_1.absoluteDir().absolutePath();
    }
    return data.ReadFileValid;
}




bool LevelScene::prepareLevel()
{

    return true;
}


void LevelScene::update()
{

}


void LevelScene::render()
{

}

bool LevelScene::isExit()
{

    return true;
}


int LevelScene::exitType()
{

    return 0;
}
