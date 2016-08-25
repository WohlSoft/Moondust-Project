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

LVL_LayerEngine::LVL_LayerEngine(LevelScene *_parent) : _scene(_parent), members()
{}

void LVL_LayerEngine::hide(QString layer, bool smoke)
{
    QList<PGE_Phys_Object* > &lyr = members[layer];
    for(int i=0; i<lyr.size(); i++)
    {
        if(!lyr[i]->isVisible()) continue;
        lyr[i]->hide();
        if(smoke)
        {
            _scene->launchStaticEffectC(10,
                                              lyr[i]->posCenterX(),
                                              lyr[i]->posCenterY(),
                                              1, 0, 0, 0, 0);
        }
    }
}

void LVL_LayerEngine::show(QString layer,bool smoke)
{
    QList<PGE_Phys_Object* > &lyr = members[layer];
    for(int i=0; i<lyr.size(); i++)
    {
        if(lyr[i]->isVisible()) continue;
        lyr[i]->show();
        if(smoke)
        {
            _scene->launchStaticEffectC(10,
                                              lyr[i]->posCenterX(),
                                              lyr[i]->posCenterY(),
                                              1, 0, 0, 0, 0);
        }
    }
}

void LVL_LayerEngine::toggle(QString layer, bool smoke)
{
    QList<PGE_Phys_Object* > &lyr = members[layer];
    if(lyr.isEmpty()) return;
    bool show=!lyr.first()->isVisible();

    for(int i=0; i<lyr.size(); i++)
    {
        if(show) lyr[i]->show(); else lyr[i]->hide();
        if(smoke)
        {
            _scene->launchStaticEffectC(10,
                                              lyr[i]->posCenterX(),
                                              lyr[i]->posCenterY(),
                                              1, 0, 0, 0, 0);
        }
    }
}

void LVL_LayerEngine::registerItem(QString layer, PGE_Phys_Object *item)
{
    //Register item in the layer
    QList<PGE_Phys_Object* > &lyr = members[layer];
    if(!lyr.contains(item))
        lyr.push_back(item);
}

void LVL_LayerEngine::removeRegItem(QString layer, PGE_Phys_Object *item)
{
    //Remove item from the layer
    QList<PGE_Phys_Object* > &lyr = members[layer];
    lyr.removeAll(item);
}

void LVL_LayerEngine::installLayerMotion(QString layer, double speedX, double speedY)
{
    if(moving_layers.contains(layer))
    {
        MovingLayer &l = moving_layers[layer];
        l.m_speedX=speedX;
        l.m_speedY=speedY;
    } else {
        if((speedX==0.0)&&(speedY==0.0))
            return;//Don't store zero-speed layers!
        MovingLayer l;
        l.m_speedX=speedX;
        l.m_speedY=speedY;
        l.m_members = &members[layer];
        moving_layers[layer]=l;
    }
}

void LVL_LayerEngine::processMoving(float tickTime)
{
    if(moving_layers.isEmpty()) return;
    QVector<QString> remove_list;
    for(QHash<QString, MovingLayer>::iterator it = moving_layers.begin(); it != moving_layers.end(); it++)
    {
        MovingLayer &l = (*it);
        for(int i=0; i<l.m_members->size(); i++)
        {
            PGE_Phys_Object*obj = (*l.m_members)[i];
            //Don't iterate playable characters
            if(obj->type == PGE_Phys_Object::LVLPlayer)
                continue;

            obj->setSpeed(l.m_speedX, l.m_speedY);
            //Don't iterate activated NPC's!
            if(obj->type == PGE_Phys_Object::LVLNPC)
            {
                LVL_Npc*npc = (LVL_Npc*)obj;
                if(npc->isActivated)
                    continue;
            }
            obj->iterateStep(tickTime);
            obj->_syncPosition();
            if((l.m_speedX==0.0)&&(l.m_speedY==0.0))
                remove_list.push_back(it.key());
        }
    }
    //Remove zero-speed layers
    for(int i=0;i<remove_list.size(); i++)
        moving_layers.remove(remove_list[i]);
}

bool LVL_LayerEngine::isEmpty(QString layer)
{
    return members[layer].isEmpty();
}

