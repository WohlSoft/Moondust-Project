#include "base_object.h"

PGE_Phys_Object::PGE_Phys_Object()
{
    physBody = NULL;
    worldPtr = NULL;
    posX_coefficient = 0.0f;
    posY_coefficient = 0.0f;
    width = 0.0f;
    height = 0.0f;
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

long PGE_Phys_Object::posX()
{
    if(physBody)
        return PhysUtil::met2pix(physBody->GetPosition().x) - posX_coefficient;
    else
        return 0;
}

long PGE_Phys_Object::posY()
{
    if(physBody)
        return PhysUtil::met2pix(physBody->GetPosition().y) - posY_coefficient;
    else
        return 0;
}

void PGE_Phys_Object::setSize(float w, float h)
{
    width = w;
    height = h;
    posX_coefficient = width/2;
    posY_coefficient = height/2;
}

void PGE_Phys_Object::setPos(long x, long y)
{
    physBody->SetTransform(
                b2Vec2(
                 PhysUtil::pix2met( x+posX_coefficient),
                PhysUtil::pix2met( y+posY_coefficient)
                    ), 0.0f);

}

void PGE_Phys_Object::update()
{
}

void PGE_Phys_Object::render()
{
}
