#include "lvl_block.h"

LVL_Block::LVL_Block()
{
    type = LVLBlock;
    data = NULL;
}

LVL_Block::~LVL_Block()
{
    if(physBody && worldPtr)
    {
      worldPtr->DestroyBody(physBody);
      physBody->SetUserData(NULL);
      physBody = NULL;
    }
}

//float LVL_Block::posX()
//{
//    return data->x;
//}

//float LVL_Block::posY()
//{
//    return data->y;
//}

void LVL_Block::init()
{
    if(!worldPtr) return;
    setSize(data->w, data->h);

    slippery = data->slippery;

    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.position.Set( PhysUtil::pix2met( data->x+posX_coefficient ),
        PhysUtil::pix2met(data->y + posY_coefficient ) );
    bodyDef.userData = (void*)dynamic_cast<PGE_Phys_Object *>(this);
    physBody = worldPtr->CreateBody(&bodyDef);
    b2PolygonShape shape;
    shape.SetAsBox(PhysUtil::pix2met(posX_coefficient), PhysUtil::pix2met(posY_coefficient) );
    b2Fixture * block = physBody->CreateFixture(&shape, 1.0f);
    block->SetFriction(data->slippery? 0.04f : 0.25f );

}
