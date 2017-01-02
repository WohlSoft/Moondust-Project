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

void LevelScene::registerElement(PGE_Phys_Object *item)
{
    RPoint lt={item->m_posX_registered, item->m_posY_registered};
    RPoint rb={item->m_posX_registered+item->m_width_registered, item->m_posY_registered+item->m_height_registered};
    if(item->m_width_registered<=0) { rb[0]=item->m_posX_registered+1;}
    if(item->m_height_registered<=0) { rb[1]=item->m_posY_registered+1;}
    tree.Insert(lt, rb, item);
}

void LevelScene::unregisterElement(PGE_Phys_Object *item)
{
    RPoint lt={item->m_posX_registered, item->m_posY_registered};
    RPoint rb={item->m_posX_registered+item->m_width_registered, item->m_posY_registered+item->m_height_registered};
    if(item->m_width_registered<=0) { rb[0]=item->m_posX_registered+1;}
    if(item->m_height_registered<=0) { rb[1]=item->m_posY_registered+1;}
    tree.Remove(lt, rb, item);
}


static bool _TreeSearchCallback(PGE_Phys_Object* item, void* arg)
{
    PGE_RenderList* list = static_cast<PGE_RenderList* >(arg);
    if(list)
    {
        if(item) (*list).push_back(item);
    }
    return true;
}

void LevelScene::queryItems(PGE_RectF &zone, QVector<PGE_Phys_Object *> *resultList)
{
    RPoint lt = { zone.left(),  zone.top() };
    RPoint rb = { zone.right(), zone.bottom() };
    tree.Search(lt, rb, _TreeSearchCallback, (void*)resultList);
}

void LevelScene::queryItems(double x, double y, QVector<PGE_Phys_Object* > *resultList)
{
    PGE_RectF zone = PGE_RectF(x, y, 1, 1);
    queryItems(zone, resultList);
}
