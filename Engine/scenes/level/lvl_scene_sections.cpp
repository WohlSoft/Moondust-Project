/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "../scene_level.h"

#include <QtDebug>

int LevelScene::findNearSection(long x, long y)
{

    bool found=false;
    int result=0;
    int padding=0;

    while( (!found) && (padding < 1024) )
    {
        for(int i=0; i<data.sections.size(); i++)
        {
            if(
                    (data.sections[i].size_left==0)
                    &&(data.sections[i].size_right==0)
                    &&(data.sections[i].size_top==0)
                    &&(data.sections[i].size_bottom==0) )
                continue;

            if(data.sections[i].size_left-padding > x)
                continue;
            if(data.sections[i].size_right + padding < x)
                continue;
            if(data.sections[i].size_top - padding > y)
                continue;
            if(data.sections[i].size_bottom + padding < y)
                continue;

            found=true;
            result = i;

            break;
        }
        padding+=32;
    }
    return result;
}



LVL_Section *LevelScene::getSection(int sct)
{
    if((sct>=0)&&(sct<sections.size()))
    {
        if(sections[sct].data.id==sct)
            return &sections[sct];
        else
        {
            for(LVL_SectionsList::iterator it=sections.begin(); it!=sections.end(); it++)
                if(it->data.id==sct)
                    return &(*it);
        }
    }
    return NULL;
}

EpisodeState *LevelScene::getGameState()
{
    return gameState;
}




