#include "../scene_level.h"


int LevelScene::findNearSection(long x, long y)
{

    bool found=false;
    int result=0;
    int padding=-64;

    while( (!found) && (padding < 1024) )
        for(int i=0; i<data.sections.size(); i++)
        {
            padding+=64;

            if(
                    (data.sections[i].size_left==0)
                    &&(data.sections[i].size_right==0)
                    &&(data.sections[i].size_top==0)
                    &&(data.sections[i].size_bottom==0) )
                continue;

            if(data.sections[i].size_left-padding > x)
                continue;
            if(data.sections[i].size_right+padding < x)
                continue;
            if(data.sections[i].size_top-padding > y)
                continue;
            if(data.sections[i].size_bottom+padding < y)
                continue;

            found=true;
            result=i;
            break;
        }

    return result;
}

