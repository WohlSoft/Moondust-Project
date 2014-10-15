#include "../scene_level.h"
#include "../../data_configs/config_manager.h"


bool LevelScene::setEntrance(int entr)
{

    if(entr<=0)
    {
        isWarpEntrance=false;
        bool found=false;
        for(int i=0;i<data.players.size(); i++)
        {
            if(data.players[i].w==0 && data.players[i].h==0)
                continue; //Skip empty points
            cameraStart.setX(data.players[i].x);
            cameraStart.setY(data.players[i].y);
            found=true;
            break;
        }
        //Find available start points
        return found;
    }
    else
    {
        for(int i=0;i<data.doors.size(); i++)
        {
            if(data.doors[i].array_id==(unsigned int)entr)
            {
                isWarpEntrance = true;
                cameraStart.setX(data.doors[i].ox);
                cameraStart.setY(data.doors[i].oy);
                return true;
            }

        }
    }

    isWarpEntrance=false;

    //Error, sections is not found
    return false;
}

bool LevelScene::loadConfigs()
{
    bool success=true;
    success = ConfigManager::loadLevelBlocks();
    //if(success) success = ...

    return success;
}




