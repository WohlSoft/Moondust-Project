/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#include "../scene_level.h"
#include <functional>
#include "lvl_subtree.h"

void PGE_Phys_Object::TreeMapMember::addToScene(bool keepAbsPos)
{
    if(!m_is_registered)
    {
        LVL_SubTree *st = nullptr;
        if(m_self->m_parent && ((st = dynamic_cast<LVL_SubTree *>(m_self->m_parent)) != nullptr))
        {
            Momentum m = m_self->m_momentum;
            if(keepAbsPos)
            {
                m.x += st->m_offsetX;
                m.y += st->m_offsetY;
                m.oldx += st->m_offsetXold;
                m.oldy += st->m_offsetYold;
            }
            m_self->m_momentum_relative = m;
            m_posX_registered   = m.x;
            m_posY_registered   = m.y;
            m_width_registered  = m.w;
            m_height_registered = m.h;
            st->registerElement(m_self);
        }
        else
        {
            m_self->m_momentum_relative = m_self->m_momentum;
            m_self->m_scene->registerElement(m_self);
        }
    }
    m_is_registered = true;
}

void PGE_Phys_Object::TreeMapMember::updatePos()
{
    LVL_SubTree *st = nullptr;
    if(m_self->m_parent && ((st = dynamic_cast<LVL_SubTree *>(m_self->m_parent)) != nullptr))
    {
        Momentum m = m_self->m_momentum_relative;
        //m.x += st->m_offsetX;
        //m.y += st->m_offsetY;
        //m.oldx += st->m_offsetXold;
        //m.oldy += st->m_offsetYold;
        //m_self->m_momentum_relative = m;
        if(m_is_registered)
            st->unregisterElement(m_self);
        else
            m_is_registered = true;
        m_posX_registered   = m.x;
        m_posY_registered   = m.y;
        st->registerElement(m_self);
    }
    else
    {
        if(!m_is_registered)
        {
            m_self->m_scene->registerElement(m_self);
            m_is_registered = true;
        }
        else
            m_self->m_scene->updateElement(m_self);
    }
}

void PGE_Phys_Object::TreeMapMember::updatePosAndSize()
{
    LVL_SubTree *st = nullptr;
    if(m_self->m_parent && ((st = dynamic_cast<LVL_SubTree *>(m_self->m_parent)) != nullptr))
    {
        Momentum m = m_self->m_momentum_relative;
        //m.x += st->m_offsetX;
        //m.y += st->m_offsetY;
        //m.oldx += st->m_offsetXold;
        //m.oldy += st->m_offsetYold;
        //m_self->m_momentum_relative = m;
        if((!m_is_registered) ||
           (m_posX_registered != m.x) ||
           (m_posY_registered != m.y) ||
           (m_width_registered != m.w) ||
           (m_height_registered != m.h) )
        {
            if(m_is_registered)
                st->unregisterElement(m_self);
            else
                m_is_registered = true;
            m_posX_registered   = m.x;
            m_posY_registered   = m.y;
            m_width_registered  = m.w;
            m_height_registered = m.h;
            st->registerElement(m_self);
        }
    }
    else
    {
        if(!m_is_registered)
        {
            m_self->m_scene->registerElement(m_self);
            m_is_registered = true;
        }
        else
            m_self->m_scene->updateElement(m_self);
    }
}

void PGE_Phys_Object::TreeMapMember::updateSize()
{
    LVL_SubTree *st = nullptr;
    if(m_self->m_parent && ((st = dynamic_cast<LVL_SubTree *>(m_self->m_parent)) != nullptr))
    {
        Momentum &m = m_self->m_momentum;
        if(m_is_registered)
            st->unregisterElement(m_self);
        else
            m_is_registered = true;
        m_width_registered  = m.w;
        m_height_registered = m.h;
        st->registerElement(m_self);
    }
    else
    {
        if(!m_is_registered)
        {
            m_self->m_scene->registerElement(m_self);
            m_is_registered = true;
        }
        else
            m_self->m_scene->updateElement(m_self);
    }
}

void PGE_Phys_Object::TreeMapMember::delFromScene()
{
    if(m_is_registered)
    {
        LVL_SubTree *st = nullptr;
        if(m_self->m_parent && ((st = dynamic_cast<LVL_SubTree *>(m_self->m_parent)) != nullptr))
            st->unregisterElement(m_self);
        else
            m_self->m_scene->unregisterElement(m_self);
    }
    m_is_registered = false;
}

void LevelScene::registerElement(LevelScene::PhysObjPtr item)
{
    m_qtree.insert(item);
}

void LevelScene::updateElement(LevelScene::PhysObjPtr item)
{
    m_qtree.update(item);
}

void LevelScene::unregisterElement(LevelScene::PhysObjPtr item)
{
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
                        item->m_momentum.velX =    st->speedXsum();
                        item->m_momentum.velXsrc = st->speedX();
                        item->m_momentum.velY =    st->speedY();
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
    m_qtree.query(zone, _TreeSearchCallback, (void*)&d);
}

void LevelScene::queryItems(double x, double y, std::vector<PGE_Phys_Object* > *resultList, std::function<bool(PGE_Phys_Object*)> *validator)
{
    PGE_RectF zone = PGE_RectF(x, y, 1, 1);
    queryItems(zone, resultList, validator);
}
