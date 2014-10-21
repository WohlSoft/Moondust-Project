
#include "lvl_bgo.h"

LVL_Bgo::LVL_Bgo()
{
    type = LVLBGO;
    data = NULL;
    animated=false;
    animator_ID=0;
}

LVL_Bgo::~LVL_Bgo()
{
    if(physBody && worldPtr)
    {
      worldPtr->DestroyBody(physBody);
      physBody->SetUserData(NULL);
      physBody = NULL;
    }
}

//float LVL_Bgo::posX()
//{
//    return data->x;
//}

//float LVL_Bgo::posY()
//{
//    return data->y;
//}

void LVL_Bgo::init()
{
    if(!worldPtr) return;
    setSize(texture->w, texture->h);

    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set( PhysUtil::pix2met( data->x+posX_coefficient ),
        PhysUtil::pix2met(data->y + posY_coefficient ) );
    bodyDef.userData = (void*)dynamic_cast<PGE_Phys_Object *>(this);
    physBody = worldPtr->CreateBody(&bodyDef);

    b2PolygonShape shape;

    shape.SetAsBox(PhysUtil::pix2met(posX_coefficient), PhysUtil::pix2met(posY_coefficient) );

    b2Fixture * bgo = physBody->CreateFixture(&shape, 1.0f);
    bgo->SetSensor(true);
    bgo->SetFriction( 0 );
}
