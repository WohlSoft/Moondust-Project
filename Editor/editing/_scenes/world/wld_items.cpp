/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <common_features/grid.h>
#include <common_features/mainwinconnect.h>
#include <editing/edit_world/world_edit.h>

#include "wld_scene.h"
#include "items/item_point.h"


void WldScene::setPoint(QPoint p)
{
     selectedPoint = p;
     selectedPointNotUsed=false;

     if(!pointAnimation)
     {
        pointAnimation = new SimpleAnimator(pointImg, true, 4, 64);
        pointAnimation->start();
     }

     if(!pointTarget)
     {
         pointTarget = new ItemPoint;
         ((ItemPoint*)pointTarget)->gridSize = pConfigs->default_grid;
         addItem(pointTarget);
         ((ItemPoint*)pointTarget)->setScenePoint(this);
         pointTarget->setData(0, "POINT");
         pointTarget->setZValue(6000);
     }
     pointTarget->setPos(QPointF(p));

}

void WldScene::unserPointSelector()
{
     if(pointTarget) delete pointTarget;
        pointTarget = NULL;
     if(pointAnimation)
     {
         pointAnimation->stop();
         delete pointAnimation;
     }
        pointAnimation = NULL;
}
