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

#include "lvl_layer_engine.h"
#include <data_configs/config_manager.h>
#include "lvl_scene_ptr.h"

LVL_LayerEngine::LVL_LayerEngine() : members()
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
            LvlSceneP::s->launchStaticEffectC(10,
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
            LvlSceneP::s->launchStaticEffectC(10,
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
            LvlSceneP::s->launchStaticEffectC(10,
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

bool LVL_LayerEngine::isEmpty(QString layer)
{
    return members[layer].isEmpty();
}

