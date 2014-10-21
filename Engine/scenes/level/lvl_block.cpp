#include "lvl_block.h"

LVL_Block::LVL_Block()
{
    type = LVLBlock;
    data = NULL;
    animated=false;
    sizable=false;
    animator_ID=0;
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

    switch(setup->phys_shape)
    {
    //triangles
    case 1: //up-left
        {
          b2Vec2 vertices[3];
          //Left-top
          vertices[0].Set(PhysUtil::pix2met(-posX_coefficient),  PhysUtil::pix2met(-posY_coefficient));
          //right-bottom
          vertices[1].Set(PhysUtil::pix2met(posX_coefficient),  PhysUtil::pix2met(posY_coefficient));
          //left-bottom
          vertices[2].Set(PhysUtil::pix2met(-posX_coefficient), PhysUtil::pix2met(posY_coefficient));
          shape.Set(vertices, 3);
          break;
        }
    case -1: // Up-right
        {
          b2Vec2 vertices[3];
          //Left-bottom
          vertices[0].Set(PhysUtil::pix2met(-posX_coefficient),  PhysUtil::pix2met(posY_coefficient));
          //right-top
          vertices[1].Set(PhysUtil::pix2met(posX_coefficient),  PhysUtil::pix2met(-posY_coefficient));
          //right-bottom
          vertices[2].Set(PhysUtil::pix2met(posX_coefficient), PhysUtil::pix2met(posY_coefficient));
          shape.Set(vertices, 3);
          break;
        }
    case 2: //right
        {
          b2Vec2 vertices[3];
          //Left-top
          vertices[0].Set(PhysUtil::pix2met(-posX_coefficient),  PhysUtil::pix2met(-posY_coefficient));
          //right-top
          vertices[1].Set(PhysUtil::pix2met(posX_coefficient),  PhysUtil::pix2met(-posY_coefficient));
          //left-bottom
          vertices[2].Set(PhysUtil::pix2met(-posX_coefficient), PhysUtil::pix2met(posY_coefficient));
          shape.Set(vertices, 3);
          break;
        }
    case -2: //left
        {
          b2Vec2 vertices[3];
          //Left-top
          vertices[0].Set(PhysUtil::pix2met(-posX_coefficient),  PhysUtil::pix2met(-posY_coefficient));
          //right-top
          vertices[1].Set(PhysUtil::pix2met(posX_coefficient),  PhysUtil::pix2met(-posY_coefficient));
          //right-bottom
          vertices[2].Set(PhysUtil::pix2met(posX_coefficient), PhysUtil::pix2met(posY_coefficient));
          shape.Set(vertices, 3);
          break;
        }
    case 3:// map shape from texture
        //Is not implemented, create the box:

    //rectangle box
    default: //Box shape
        shape.SetAsBox(PhysUtil::pix2met(posX_coefficient), PhysUtil::pix2met(posY_coefficient) );
    }

    b2Fixture * block = physBody->CreateFixture(&shape, 1.0f);

    if((setup->sizable) || (setup->collision==0) || (setup->collision==2))
        block->SetSensor(true);


    block->SetFriction(data->slippery? 0.04f : 0.25f );

}
