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
#include <functional>
#include "lvl_subtree.h"

void LevelScene::registerElement(LevelScene::PhysObjPtr item)
{
    //RPoint lt={item->m_posX_registered, item->m_posY_registered};
    //RPoint rb={item->m_posX_registered+item->m_width_registered, item->m_posY_registered+item->m_height_registered};
    //if(item->m_width_registered<=0) { rb[0]=item->m_posX_registered+1;}
    //if(item->m_height_registered<=0) { rb[1]=item->m_posY_registered+1;}
    //m_tree.Insert(lt, rb, item);
    m_qtree.insert(item);
}

void LevelScene::updateElement(LevelScene::PhysObjPtr item)
{
    m_qtree.update(item);
}

void LevelScene::unregisterElement(LevelScene::PhysObjPtr item)
{
    //RPoint lt={item->m_posX_registered, item->m_posY_registered};
    //RPoint rb={item->m_posX_registered+item->m_width_registered, item->m_posY_registered+item->m_height_registered};
    //if(item->m_width_registered<=0) { rb[0]=item->m_posX_registered+1;}
    //if(item->m_height_registered<=0) { rb[1]=item->m_posY_registered+1;}
    //m_tree.Remove(lt, rb, item);
    m_qtree.remove(item);
}

struct _TreeSearchData
{
    std::function<bool(PGE_Phys_Object*)> *validator;
    PGE_RenderList* list;
    PGE_RectF *zone;
};

static bool _TreeSearchCallback(PGE_Phys_Object* item, void* arg)
{
    _TreeSearchData *d = static_cast<_TreeSearchData*>(arg);
    if(d && d->list)
    {
        if(item)
        {
            if(item->type == PGE_Phys_Object::LVLSubTree)
            {
                LVL_SubTree *stree = dynamic_cast<LVL_SubTree*>(item);
                if(stree)
                {
                    PGE_RectF newRect = *d->zone;
                    newRect.setPos(newRect.x() + stree->m_offsetX, newRect.y() + stree->m_offsetY);
                    _TreeSearchData dd{d->validator, d->list, &newRect};
                    stree->query(newRect, _TreeSearchCallback, (void*)&dd);
                }
                return true;
            }
            if(!d->validator || (*(d->validator))(item))
            {
                if(item->m_parent)
                {
                    LVL_SubTree *st = dynamic_cast<LVL_SubTree *>(item->m_parent);
                    if(st)
                    {
                        item->m_momentum = item->m_momentum_relative;
                        item->m_momentum.x -= st->m_offsetX;
                        item->m_momentum.y -= st->m_offsetY;
                        item->m_momentum.oldx -= st->m_offsetXold;
                        item->m_momentum.oldy -= st->m_offsetYold;
                        item->m_momentum.velX = st->speedX();
                        item->m_momentum.velXsrc = st->speedX();
                        item->m_momentum.velY = st->speedY();
                    }
                }
                d->list->push_back(item);
            }
        }
    }
    return true;
}

void LevelScene::queryItems(PGE_RectF &zone, std::vector<PGE_Phys_Object *> *resultList, std::function<bool(PGE_Phys_Object*)> *validator)
{
    _TreeSearchData d{validator, resultList, &zone};
    //RPoint lt = { zone.left(),  zone.top() };
    //RPoint rb = { zone.right(), zone.bottom() };
    //m_tree.Search(lt, rb, _TreeSearchCallback, (void*)&d);
    m_qtree.query(zone, _TreeSearchCallback, (void*)&d);
}

void LevelScene::queryItems(double x, double y, std::vector<PGE_Phys_Object* > *resultList, std::function<bool(PGE_Phys_Object*)> *validator)
{
    PGE_RectF zone = PGE_RectF(x, y, 1, 1);
    queryItems(zone, resultList, validator);
}
