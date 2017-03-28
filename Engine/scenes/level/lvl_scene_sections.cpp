/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <cmath>

int LevelScene::findNearestSection(long x, long y)
{
    long lessDistance = 0;
    int  result = 0;

    //Try to find intersecting section first
    for(size_t i = 0; i < m_data.sections.size(); i++)
    {
        LevelSection &s = m_data.sections[i];

        if((x >= s.size_left) && (x <= s.size_right) &&
           (y >= s.size_top)  && (y <= s.size_bottom))
            return i;
    }

    //Find section by nearest center or corner
    for(size_t i = 0; i < m_data.sections.size(); i++)
    {
        LevelSection &s = m_data.sections[i];
        long centerX = s.size_left + std::abs(s.size_left - s.size_right) / 2;
        long centerY = s.size_top  + std::abs(s.size_top  - s.size_bottom) / 2;
        //Find distance to center
        long distanceC = std::sqrt(std::pow(centerX - x, 2) + std::pow(centerY - y, 2));

        auto addDistance = [&](long f)
        {
            long distanceCorner = f;
            if(distanceC > distanceCorner)
                distanceC = distanceCorner;
        };

        //Find distance to left-top corner
        addDistance(std::sqrt(std::pow(s.size_left - x, 2) + std::pow(s.size_top - y, 2)));
        //Find distance to right-top corner
        addDistance(std::sqrt(std::pow(s.size_right - x, 2) + std::pow(s.size_top - y, 2)));
        //Find distance to right-bottom corner
        addDistance(std::sqrt(std::pow(s.size_right - x, 2) + std::pow(s.size_bottom - y, 2)));
        //Find distance to left-bottom corner
        addDistance(std::sqrt(std::pow(s.size_left - x, 2) + std::pow(s.size_bottom - y, 2)));

        //Find distance to nearest vertical edge
        if((x >= s.size_left) && (x <= s.size_right))
        {
            addDistance(std::abs(s.size_top - y));
            addDistance(std::abs(s.size_bottom - y));
        }

        //Find distance to nearest horizontal edge
        if((y >= s.size_top) && (y <= s.size_bottom))
        {
            addDistance(std::abs(s.size_left - x));
            addDistance(std::abs(s.size_right - x));
        }

        if(i == 0)
            lessDistance = distanceC;
        else if(distanceC < lessDistance)
        {
            lessDistance = distanceC;
            result = i;
        }
    }

    return result;
}



LVL_Section *LevelScene::getSection(int sct)
{
    if((sct >= 0) && (sct < int32_t(m_sections.size())))
    {
        if(m_sections[sct].data.id == sct)
            return &m_sections[sct];
        else
        {
            for(LVL_SectionsList::iterator it = m_sections.begin(); it != m_sections.end(); it++)
                if(it->data.id == sct)
                    return &(*it);
        }
    }

    return nullptr;
}

EpisodeState *LevelScene::getGameState()
{
    return m_gameState;
}


