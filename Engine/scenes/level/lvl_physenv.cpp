#include "lvl_physenv.h"

LVL_PhysEnv::LVL_PhysEnv()
{
    type = LVLPhysEnv;
    env_type = Env_Water;
}

LVL_PhysEnv::~LVL_PhysEnv()
{
    if(physBody && worldPtr)
    {
      worldPtr->DestroyBody(physBody);
      physBody->SetUserData(NULL);
      physBody = NULL;
    }
}

void LVL_PhysEnv::init()
{
    if(!worldPtr) return;
    setSize(data.w, data.h);

    env_type = data.quicksand ? Env_Quicksand : Env_Water;

    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set( PhysUtil::pix2met( data.x+posX_coefficient ),
        PhysUtil::pix2met(data.y + posY_coefficient ) );
    bodyDef.userData = (void*)dynamic_cast<PGE_Phys_Object *>(this);
    physBody = worldPtr->CreateBody(&bodyDef);

    b2PolygonShape shape;

    shape.SetAsBox(PhysUtil::pix2met(posX_coefficient), PhysUtil::pix2met(posY_coefficient) );

    b2Fixture * bgo = physBody->CreateFixture(&shape, 1.0f);
    bgo->SetSensor(true);
    bgo->SetFriction( 0 );

    setPos(data.x, data.y);
    collide = COLLISION_NONE;
}

