#include "base_object.h"

PGE_Phys_Object::PGE_Phys_Object()
{
    physBody = NULL;
    worldPtr = NULL;
    posX_coefficient = 0.0f;
    posY_coefficient = 0.0f;
    width = 0.0f;
    height = 0.0f;
    z_index = 0.0d;
    isRectangle = true;
    _player_moveup = true;
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

void PGE_Phys_Object::nextFrame() {}

void PGE_Phys_Object::update() {}

void PGE_Phys_Object::render(float x, float y) {Q_UNUSED(x); Q_UNUSED(y);}
