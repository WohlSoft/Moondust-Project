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

#include "lvl_camera.h"
#include "lvl_backgrnd.h"
#include "lvl_section.h"

#include <graphics/window.h>

#include <data_configs/config_manager.h>
#include <audio/SdlMusPlayer.h>

#include <QtDebug>

#include "lvl_scene_ptr.h"

PGE_LevelCamera::PGE_LevelCamera()
{
    worldPtr = NULL;
    section = 0;
    width=800;
    height=600;
    offset_x=0;
    offset_y=0;
    BackgroundID = 0;
    cur_section = NULL;
}

PGE_LevelCamera::~PGE_LevelCamera()
{
    qDebug() << "Destroy camera";
}

void PGE_LevelCamera::setWorld(b2World *wld)
{
    worldPtr = wld;
}


void PGE_LevelCamera::init(float x, float y, float w, float h)
{
    if(!worldPtr) return;

    pos_x = x;
    pos_y = y;
    width = w;
    height = h;
}


int PGE_LevelCamera::w()
{
    return width;
}

int PGE_LevelCamera::h()
{
    return height;
}

qreal PGE_LevelCamera::posX()
{
    return pos_x;
}

qreal PGE_LevelCamera::posY()
{
    return pos_y;
}

void PGE_LevelCamera::setPos(float x, float y)
{
    pos_x = x;
    pos_y = y;

    if(!cur_section) return;

    if(pos_x < cur_section->limitBox.left())
        pos_x = cur_section->limitBox.left();

    if((pos_x+width) > cur_section->limitBox.right())
        pos_x = cur_section->limitBox.right()-width;

    if(pos_y < cur_section->limitBox.top())
        pos_y = cur_section->limitBox.top();

    if((pos_y+height) > cur_section->limitBox.bottom())
        pos_y = cur_section->limitBox.bottom()-height;

    if(cur_section->RightOnly())
    {
        if(pos_x>cur_section->limitBox.left())
            cur_section->limitBox.setLeft(pos_x);
    }

    pos_x = round(pos_x);
    pos_y = round(pos_y);
}

void PGE_LevelCamera::setSize(int w, int h)
{
    width = w;
    height = h;
}

void PGE_LevelCamera::setOffset(int x, int y)
{
    offset_x=x;
    offset_y=y;
}

void PGE_LevelCamera::update()
{
    objects_to_render.clear();

    if(!cur_section) return;
//    CollidablesInRegionQueryCallback cb = CollidablesInRegionQueryCallback();
//    b2AABB aabb;
//    aabb.lowerBound.Set(PhysUtil::pix2met(pos_x), PhysUtil::pix2met(pos_y));
//    aabb.upperBound.Set(PhysUtil::pix2met(pos_x+width), PhysUtil::pix2met(pos_y+height));
//    worldPtr->QueryAABB(&cb, aabb);
    R_itemList foundItems;
    cur_section->queryItems(PGE_RectF(pos_x, pos_y, width, height), &foundItems);

    int contacts = 0;
    for(int i=0; i<foundItems.size();i++)
    {
        contacts++;
        PGE_Phys_Object * visibleBody = foundItems[i];
        //visibleBody = static_cast<PGE_Phys_Object *>(cb.foundBodies[i]->GetUserData());
        //if(visibleBody==NULL)
        //    continue;
        switch(visibleBody->type)
        {
        case PGE_Phys_Object::LVLBlock:
        case PGE_Phys_Object::LVLBGO:
        case PGE_Phys_Object::LVLNPC:
        case PGE_Phys_Object::LVLPlayer:
        case PGE_Phys_Object::LVLEffect:
            objects_to_render.push_back(visibleBody);
        }

        if(visibleBody->type==PGE_Phys_Object::LVLNPC)
        {
            LVL_Npc *npc = dynamic_cast<LVL_Npc*>(visibleBody);
            if(npc)
            {
                if(!npc->isActivated)
                {
                    npc->Activate();
                    npc->timeout=4000;
                    LvlSceneP::s->active_npcs.push_back(npc);
                }
                else
                {
                    npc->timeout=4000;
                }
            }
        }
    }

    //Sort array
    int total = objects_to_render.size();
    long i;
    double ymin;
    long ymini;
    long sorted = 0;

    while(sorted < objects_to_render.size())
    {
        ymin = objects_to_render[sorted]->zIndex();
        ymini = sorted;

        for(i = sorted; i < total; i++)
        {
            if( objects_to_render[i]->zIndex() < ymin )
            {
                ymin = objects_to_render[i]->zIndex(); ymini = i;
            }
        }
        objects_to_render.swap(ymini, sorted);
        sorted++;
    }
}


void PGE_LevelCamera::changeSectionBorders(long left, long top, long right, long bottom)
{
    if(cur_section)
        cur_section->changeSectionBorders(left, top, right, bottom);
}

void PGE_LevelCamera::resetLimits()
{
    if(cur_section)
        cur_section->resetLimits();
}

PGE_RenderList &PGE_LevelCamera::renderObjects()
{
    return objects_to_render;
}


void PGE_LevelCamera::drawBackground()
{
    if(cur_section)
    {
        cur_section->renderBG(posX(), posY(), width, height);
    }
}

void PGE_LevelCamera::changeSection(LVL_Section *sct)
{
    if(!sct) return;

    cur_section=sct;
    section = &sct->data;
    BackgroundID = sct->getBgId();
    cur_section->playMusic();
    cur_section->setBG(BackgroundID);
}

void PGE_LevelCamera::setMusicRoot(QString dir)
{
    musicRootDir=dir;
    if(!musicRootDir.endsWith('/'))
        musicRootDir.append('/');
}

/**************************Fader*******************************/

void PGE_LevelCamera::setFade(int speed, float target, float step)
{
    fade_step = fabs(step);
    target_opacity = target;
    fadeSpeed = speed;
    fader_timer_id = SDL_AddTimer(speed, &PGE_LevelCamera::nextOpacity, this);
}

unsigned int PGE_LevelCamera::nextOpacity(unsigned int x, void *p)
{
    Q_UNUSED(x);
    PGE_LevelCamera *self = reinterpret_cast<PGE_LevelCamera *>(p);
    self->fadeStep();
    return 0;
}

void PGE_LevelCamera::fadeStep()
{
    if(fader_opacity < target_opacity)
        fader_opacity+=fade_step;
    else
        fader_opacity-=fade_step;

    if(fader_opacity>=1.0 || fader_opacity<=0.0)
        SDL_RemoveTimer(fader_timer_id);
    else
        fader_timer_id = SDL_AddTimer(fadeSpeed, &PGE_LevelCamera::nextOpacity, this);
}

/**************************Fader**end**************************/
