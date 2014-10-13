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
    if(physBody && worldPtr)
        worldPtr->DestroyBody(physBody);

}

long PGE_Phys_Object::posX()
{
    if(physBody)
        return physBody->GetPosition().x - posX_coefficient;
    else
        return 0;
}

long PGE_Phys_Object::posY()
{
    if(physBody)
        return physBody->GetPosition().y - posY_coefficient;
    else
        return 0;
}

long PGE_Phys_Object::setSize(float w, float h)
{
    width = w;
    height = h;
    posX_coefficient = width/2;
    posY_coefficient = height/2;

}

void PGE_Phys_Object::update()
{
}

void PGE_Phys_Object::render()
{
}
