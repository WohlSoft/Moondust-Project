/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <graphics/window.h>

#include <QtDebug>

PGE_LevelCamera::PGE_LevelCamera()
{
    worldPtr = NULL;
    section = 0;
    isWarp = false;
    RightOnly = false;
    ExitOffscreen = false;
    width=800;
    height=600;
    offset_x=0;
    offset_y=0;
    BackgroundID = 0;
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

    if(pos_x < limitLeft)
        pos_x = limitLeft;

    if((pos_x+width) > limitRight)
        pos_x = limitRight-width;

    if(pos_y < limitTop)
        pos_y = limitTop;

    if((pos_y+height) > limitBottom)
        pos_y = limitBottom-height;

    if(RightOnly)
    {
        if(pos_x>limitLeft)
            limitLeft = pos_x;
    }

    //    sensor->SetTransform(b2Vec2( PhysUtil::pix2met(pos_x+width/2),
    //                                 PhysUtil::pix2met(pos_y+height/2)), 0);

    pos_x = round(pos_x);
    pos_y = round(pos_y);
}

void PGE_LevelCamera::setSize(int w, int h)
{
    width = w;
    height = h;
//    b2PolygonShape* shape = (b2PolygonShape*)(sensor->GetFixtureList()->GetShape());
//    shape->m_vertices[0].Set(-PhysUtil::pix2met(w)/2, -PhysUtil::pix2met(h)/2);
//    shape->m_vertices[1].Set( PhysUtil::pix2met(w)/2, -PhysUtil::pix2met(h)/2);
//    shape->m_vertices[2].Set( PhysUtil::pix2met(w)/2,  PhysUtil::pix2met(h)/2);
    //    shape->m_vertices[3].Set(-PhysUtil::pix2met(w)/2,  PhysUtil::pix2met(h)/2);
}

void PGE_LevelCamera::setOffset(int x, int y)
{
    offset_x=x;
    offset_y=y;
}

//subclass b2QueryCallback
class CollidablesInRegionQueryCallback : public b2QueryCallback
{
public:
  QVector<b2Body*> foundBodies;

  bool ReportFixture(b2Fixture* fixture) {
      foundBodies.push_back( fixture->GetBody() );
      return true;//keep going to find all fixtures in the query area
  }
};

void PGE_LevelCamera::update()
{
    objects_to_render.clear();

    CollidablesInRegionQueryCallback cb = CollidablesInRegionQueryCallback();
    b2AABB aabb;
    aabb.lowerBound.Set(PhysUtil::pix2met(pos_x), PhysUtil::pix2met(pos_y));
    aabb.upperBound.Set(PhysUtil::pix2met(pos_x+width), PhysUtil::pix2met(pos_y+height));
    worldPtr->QueryAABB(&cb, aabb);

    int contacts = 0;

    for(int i=0; i<cb.foundBodies.size();i++)
    {
        contacts++;
        PGE_Phys_Object * visibleBody;

        visibleBody = static_cast<PGE_Phys_Object *>(cb.foundBodies[i]->GetUserData());

        if(visibleBody==NULL)
            continue;

        switch(visibleBody->type)
        {
        case PGE_Phys_Object::LVLBlock:
        case PGE_Phys_Object::LVLBGO:
        case PGE_Phys_Object::LVLNPC:
        case PGE_Phys_Object::LVLPlayer:
        case PGE_Phys_Object::LVLEffect:
            objects_to_render.push_back(visibleBody);
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
        ymin = objects_to_render[sorted]->z_index;
        ymini = sorted;

        for(i = sorted; i < total; i++)
        {
            if( objects_to_render[i]->z_index < ymin )
            {
                ymin = objects_to_render[i]->z_index; ymini = i;
            }
        }
        objects_to_render.swap(ymini, sorted);
        sorted++;
    }
}


void PGE_LevelCamera::changeSectionBorders(long left, long top, long right, long bottom)
{
    s_left = left;
    limitLeft = left;

    s_top = top;
    limitTop = top;

    s_right = right;
    limitRight = right;

    s_bottom = bottom;
    limitBottom = bottom;
}

void PGE_LevelCamera::resetLimits()
{
    limitLeft = s_left;
    limitTop = s_top;
    limitRight = s_right;
    limitBottom = s_bottom;
}

PGE_RenderList PGE_LevelCamera::renderObjects()
{
    return objects_to_render;
}


void PGE_LevelCamera::drawBackground()
{

}

void PGE_LevelCamera::changeSection(LevelSection &sct)
{
    section = &sct;
    BackgroundID = sct.background;
    isWarp = section->IsWarp;
    RightOnly = section->noback;
    ExitOffscreen = section->OffScreenEn;

    changeSectionBorders(sct.size_left, sct.size_top, sct.size_right, sct.size_bottom);
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
