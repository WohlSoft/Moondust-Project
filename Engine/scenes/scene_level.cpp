#include "scene_level.h"

#include <QFile>
#include <QDir>
#include <QFileInfo>

LevelScene::LevelScene()
{
    data = FileFormats::dummyLvlDataArray();
    data.ReadFileValid = false;

    world=NULL;
}

void LevelScene::init()
{


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
