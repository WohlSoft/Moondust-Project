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

#include "base_object.h"

PGE_Phys_Object::PGE_Phys_Object()
{
    physBody = NULL;
    worldPtr = NULL;
    posX_coefficient = 0.0f;
    posY_coefficient = 0.0f;
    width = 0.0f;
    height = 0.0f;
    z_index = 0.0;
    isRectangle = true;
    _player_moveup = true;
    slippery_surface = false;
    collide = COLLISION_ANY;
}

PGE_Phys_Object::~PGE_Phys_Object()
{
    if((physBody!=NULL) && (worldPtr!=NULL))
    {
        worldPtr->DestroyBody(physBody);
        physBody->SetUserData(NULL);
        physBody = NULL;
    }

}

float PGE_Phys_Object::posX()
{
    if(physBody)
    {//PhysUtil::pixMeter
        return PhysUtil::met2pix(physBody->GetPosition().x) - posX_coefficient;
    }
    else
        return 0;
}

float PGE_Phys_Object::posY()
{
    if(physBody)
    {
//        b2Vec2 offset = b2Vec2(posX_coefficient, posY_coefficient);
//        b2Vec2 position = physBody->GetPosition().cpy().scl(PhysUtil::pixMeter).sub(offset);
//        return position.y;//PhysUtil::met2pix(physBody->GetPosition().x) - posX_coefficient;
        return PhysUtil::met2pix(physBody->GetPosition().y) - posY_coefficient;
    }
    else
        return 0;
}

float PGE_Phys_Object::top()
{
    return posY();
}

float PGE_Phys_Object::bottom()
{
    return posY()+height;
}

float PGE_Phys_Object::left()
{
    return posX();
}

float PGE_Phys_Object::right()
{
    return posX()+width;
}

void PGE_Phys_Object::setSize(float w, float h)
{
    width = w;
    height = h;
    posX_coefficient = width/2.0f;
    posY_coefficient = height/2.0f;
}

void PGE_Phys_Object::setPos(long x, long y)
{
    physBody->SetTransform(
                b2Vec2(
                PhysUtil::pix2met( x+posX_coefficient),
                PhysUtil::pix2met( y+posY_coefficient)
                    ), 0.0f);

}

GLdouble PGE_Phys_Object::zIndex()
{
    return z_index;
}

void PGE_Phys_Object::nextFrame() {}

void PGE_Phys_Object::update() {}

void PGE_Phys_Object::render(float x, float y) {Q_UNUSED(x); Q_UNUSED(y);}


bool operator<(const PGE_Phys_Object &lhs, const PGE_Phys_Object &rhs)
{ return lhs.z_index>rhs.z_index; }


bool operator>(const PGE_Phys_Object &lhs, const PGE_Phys_Object &rhs)
{ return lhs.z_index<rhs.z_index; }



bool CollidablesInRegionQueryCallback::ReportFixture(b2Fixture *fixture) {
    foundBodies.push_back( fixture->GetBody() );
    return true;//keep going to find all fixtures in the query area
}
