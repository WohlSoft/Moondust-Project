/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "lvl_layer_engine.h"
#include <data_configs/config_manager.h>
#include "../scene_level.h"

LVL_LayerEngine::LVL_LayerEngine(LevelScene *_parent) :
    m_scene(_parent),
    m_layers()
{
    // Automatically initialize special layers
    {
        Layer &db = m_layers[DESTROYED_LAYER_NAME];
        db.m_layerType = Layer::T_DESTROYED_BLOCKS;
        db.m_vizible = false;
    }
    {
        Layer &db = m_layers[SPAWNED_LAYER_NAME];
        db.m_layerType = Layer::T_SPAWNED_NPCs;
        db.m_vizible = true;
    }
}

void LVL_LayerEngine::spawnSmokeAt(double x, double y)
{
    SpawnEffectDef smoke = ConfigManager::g_setup_effects.m_smoke;
    smoke.startX = x;
    smoke.startY = y;
    m_scene->launchEffect(smoke, true);
}

void LVL_LayerEngine::hide(QString layer, bool smoke)
{
    Layer &lyr = m_layers[layer];

    for(Layer::Members::iterator it = lyr.m_members.begin();
        it != lyr.m_members.end();
        it++ )
    {
        PGE_Phys_Object*body = it->second;
        if(!body->isVisible())
            continue;
        body->hide();
        if(smoke)
            spawnSmokeAt(body->posCenterX(), body->posCenterY());
    }

    if(lyr.m_layerType == Layer::T_REGULAR)
        lyr.m_vizible = false;
}

void LVL_LayerEngine::show(QString layer, bool smoke)
{
    Layer &lyr = m_layers[layer];

    if(lyr.m_layerType == Layer::T_DESTROYED_BLOCKS)
    {
        //Restore all destroyed and modified blocks into their initial states
        for(Layer::Members::iterator it = lyr.m_members.begin();
            it != lyr.m_members.end();
            it++ )
        {
            LVL_Block* blk = dynamic_cast<LVL_Block*>(it->second);
            if(!blk)
                continue;
            //lyr.m_members.remove(intptr_t(blk));
            bool wasInviz = !blk->isVisible();
            blk->setVisible(true);
            blk->init(true);//Force re-initialize block from initial data
            if(smoke && wasInviz)
                spawnSmokeAt(blk->posCenterX(), blk->posCenterY());
        }

        //Clear layer from objects
        lyr.m_members.clear();
    }
    else
    {
        for(Layer::Members::iterator it = lyr.m_members.begin();
            it != lyr.m_members.end();
            it++ )
        {
            PGE_Phys_Object*body = it->second;
            if(body->isVisible())
                continue;
            body->show();
            if(smoke)
                spawnSmokeAt(body->posCenterX(), body->posCenterY());
        }
    }

    if(lyr.m_layerType == Layer::T_REGULAR)
        lyr.m_vizible = true;
}

void LVL_LayerEngine::toggle(QString layer, bool smoke)
{
    Layer &lyr = m_layers[layer];
    bool viz = !lyr.m_vizible;

    if(viz && (lyr.m_layerType == Layer::T_DESTROYED_BLOCKS))
    {
        show(layer);
        return;
    }
    else
    if(!viz && (lyr.m_layerType == Layer::T_SPAWNED_NPCs))
    {
        hide(layer);
        return;
    }

    for(Layer::Members::iterator it = lyr.m_members.begin(); it != lyr.m_members.end(); it++ )
    {
        PGE_Phys_Object*body = it->second;
        body->setVisible(viz);
        if(smoke)
            spawnSmokeAt(body->posCenterX(), body->posCenterY());
    }

    if(lyr.m_layerType == Layer::T_REGULAR)
        lyr.m_vizible = viz;
}

void LVL_LayerEngine::registerItem(QString layer, PGE_Phys_Object *item)
{
    //Register item in the layer
    Layer &lyr = m_layers[layer];
    lyr.m_members[reinterpret_cast<intptr_t>(item)] = item;
    if(lyr.m_layerType == Layer::T_REGULAR)
        item->setVisible(lyr.m_vizible);
}

void LVL_LayerEngine::removeRegItem(QString layer, PGE_Phys_Object *item)
{
    //Remove item from the layer
    Layer &lyr = m_layers[layer];
    lyr.m_members.erase(reinterpret_cast<intptr_t>(item));
}

void LVL_LayerEngine::installLayerMotion(QString layer, double speedX, double speedY)
{
    if(m_movingLayers.contains(layer))
    {
        MovingLayer &l = m_movingLayers[layer];
        l.m_speedX=speedX;
        l.m_speedY=speedY;
    }
    else
    {
        if((speedX == 0.0) && (speedY == 0.0))
            return;//Don't store zero-speed layers!
        if(!m_layers.contains(layer))
            return;
        Layer &lyr = m_layers[layer];
        MovingLayer l;
        l.m_speedX=speedX;
        l.m_speedY=speedY;
        l.m_members = &lyr.m_members;
        m_movingLayers[layer]=l;
    }
}

void LVL_LayerEngine::processMoving(double tickTime)
{
    if(m_movingLayers.isEmpty())
        return;
    QVector<QString> remove_list;
    for(QHash<QString, MovingLayer>::iterator layerIt = m_movingLayers.begin();
        layerIt != m_movingLayers.end();
        layerIt++)
    {
        MovingLayer &l = (*layerIt);
        for(Layer::Members::iterator it = l.m_members->begin(); it != l.m_members->end(); it++ )
        {
            PGE_Phys_Object* obj = it->second;
            //Don't iterate playable characters
            if(obj->type == PGE_Phys_Object::LVLPlayer)
                continue;

            obj->setSpeed(l.m_speedX, l.m_speedY);

            //Don't iterate activated NPC's!
            if(obj->type == PGE_Phys_Object::LVLNPC)
            {
                LVL_Npc*npc = (LVL_Npc*)obj;
                if( npc->isActivated /* &&
                   !npc->isGenerator &&
                   !npc->is_scenery*/ )
                    continue;
            }
            obj->iterateStep(tickTime, true);
            if( (l.m_speedX == 0.0) && (l.m_speedY == 0.0) )
            {
                if(obj->m_bodytype == PGE_physBody::Body_STATIC)
                {
                    //obj->m_momentum.y = obj->m_momentum.y < 0 ? floor(obj->m_momentum.y) : ceil(obj->m_momentum.y);
                    obj->m_momentum.saveOld();
                }
            }
            obj->_syncPosition();
        }
        if( (l.m_speedX==0.0) && (l.m_speedY==0.0) )
        {
            remove_list.push_back(layerIt.key());
        }
    }
    //Remove zero-speed layers
    for(int i=0;i<remove_list.size(); i++)
        m_movingLayers.remove(remove_list[i]);
}

bool LVL_LayerEngine::isEmpty(QString layer)
{
    return !m_layers.contains(layer) || m_layers[layer].m_members.empty();
}

void LVL_LayerEngine::clear()
{
    m_movingLayers.clear();
    m_layers.clear();
}

