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
#include <graphics/gl_renderer.h>

#include <QtDebug>

#include "lvl_scene_ptr.h"

PGE_LevelCamera::PGE_LevelCamera()
{
    width=800;
    height=600;
    offset_x=0;
    offset_y=0;
    pos_x=0;
    pos_y=0;
    section = 0;
    cur_section = NULL;
    fader.setNull();
}

PGE_LevelCamera::PGE_LevelCamera(const PGE_LevelCamera &cam)
{
    width=cam.width;
    height=cam.height;
    offset_x=cam.offset_x;
    offset_y=cam.offset_y;
    pos_x=cam.pos_x;
    pos_y=cam.pos_y;

    objects_to_render = cam.objects_to_render;

    section = cam.section;
    cur_section = cam.cur_section;

    fader = cam.fader;
}

PGE_LevelCamera::~PGE_LevelCamera()
{}

void PGE_LevelCamera::init(float x, float y, float w, float h)
{
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

void PGE_LevelCamera::update(float ticks)
{
    objects_to_render.clear();

    if(!cur_section) return;
//    CollidablesInRegionQueryCallback cb = CollidablesInRegionQueryCallback();
//    b2AABB aabb;
//    aabb.lowerBound.Set(PhysUtil::pix2met(pos_x), PhysUtil::pix2met(pos_y));
//    aabb.upperBound.Set(PhysUtil::pix2met(pos_x+width), PhysUtil::pix2met(pos_y+height));
//    worldPtr->QueryAABB(&cb, aabb);
    fader.tickFader(ticks);
    LvlSceneP::s->queryItems(PGE_RectF(pos_x, pos_y, width, height), &objects_to_render);

    int contacts = 0;
    for(int i=0; i<objects_to_render.size();i++)
    {
        contacts++;
        PGE_Phys_Object * visibleBody = objects_to_render[i];
        bool renderable=false;
        switch(visibleBody->type)
        {
            case PGE_Phys_Object::LVLBlock:
            case PGE_Phys_Object::LVLBGO:
            case PGE_Phys_Object::LVLNPC:
            case PGE_Phys_Object::LVLPlayer:
            case PGE_Phys_Object::LVLEffect:
                renderable=true;
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

        if(!renderable)
        {
            objects_to_render.removeAt(i); i--;
        }
    }

    //Sort array
    sortElements();
}

void PGE_LevelCamera::sortElements()
{
    if(objects_to_render.size()<=1) return; //Nothing to sort!
    QStack<int> beg;
    QStack<int> end;
    PGE_Phys_Object* piv;
    int i=0, L, R, swapv;
    beg.push_back(0);
    end.push_back(objects_to_render.size());
    while (i>=0)
    {
        L=beg[i]; R=end[i]-1;
        if (L<R)
        {
            piv=objects_to_render[L];
            while (L<R)
            {
                while ((objects_to_render[R]->zIndex()>=piv->zIndex()) && (L<R)) R--;
                if (L<R) objects_to_render[L++]=objects_to_render[R];

                while ((objects_to_render[L]->zIndex()<=piv->zIndex()) && (L<R)) L++;
                if (L<R) objects_to_render[R--]=objects_to_render[L];
            }
            objects_to_render[L]=piv; beg.push_back(L+1); end.push_back(end[i]); end[i++]=(L);
            if((end[i]-beg[i]) > (end[i-1]-beg[i-1]))
            {
                swapv=beg[i]; beg[i]=beg[i-1]; beg[i-1]=swapv;
                swapv=end[i]; end[i]=end[i-1]; end[i-1]=swapv;
            }
        }
        else
        {
            i--;
            beg.pop_back();
            end.pop_back();
        }
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

void PGE_LevelCamera::drawForeground()
{
    if(!fader.isNull())
    {
        GlRenderer::renderRect(0, 0, width, height, 0.0f, 0.0f, 0.0f, fader.fadeRatio());
    }
}


void PGE_LevelCamera::changeSection(LVL_Section *sct)
{
    if(!sct) return;

    cur_section=sct;
    section = &sct->data;
    cur_section->playMusic();//Play current section music
    cur_section->initBG();   //Init background if not initialized
}
