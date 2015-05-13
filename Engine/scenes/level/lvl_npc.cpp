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

#include "lvl_npc.h"
#include "../../data_configs/config_manager.h"
#include <graphics/gl_renderer.h>


LVL_Npc::LVL_Npc()
{
    type = LVLNPC;
    data = FileFormats::dummyLvlNpc();
    animated=false;
    animator_ID=0;
    killed=false;
}

LVL_Npc::~LVL_Npc()
{
    if(physBody && worldPtr)
    {
      worldPtr->DestroyBody(physBody);
      physBody->SetUserData(NULL);
      physBody = NULL;
    }
}

void LVL_Npc::init()
{
    if(!worldPtr) return;
    setSize(32, 32);
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.fixedRotation=true;
    bodyDef.awake=false;
    bodyDef.allowSleep=true;
    bodyDef.position.Set( PhysUtil::pix2met( data.x+posX_coefficient ),
        PhysUtil::pix2met(data.y + posY_coefficient ) );
    bodyDef.linearDamping = 2.0;
    bodyDef.userData = (void*)dynamic_cast<PGE_Phys_Object *>(this);
    physBody = worldPtr->CreateBody(&bodyDef);

    b2PolygonShape shape;
    shape.SetAsBox(PhysUtil::pix2met(posX_coefficient), PhysUtil::pix2met(posY_coefficient) );
    collide = COLLISION_ANY;
    b2Fixture * npc = physBody->CreateFixture(&shape, 1.0f);
    npc->SetDensity(1.0);
    npc->SetFriction(0.3f);
    npc->SetSensor(false);
    npc->SetFriction( 0 );
}

void LVL_Npc::kill()
{
    killed=true;
    if(physBody && worldPtr)
    {
      worldPtr->DestroyBody(physBody);
      physBody->SetUserData(NULL);
      physBody = NULL;
    }
}

void LVL_Npc::render(float camX, float camY)
{
    if(killed) return;

    if(physBody)
    {
        if(!physBody->IsAwake())
        {
            physBody->SetAwake(true);
        }
        if(physBody->GetType()!=b2_dynamicBody)
            physBody->SetType(b2_dynamicBody);
    }

    //AniPos x(0,1);

    //if(animated) //Get current animated frame
    //    x = ConfigManager::Animator_BGO[animator_ID].image();
    GlRenderer::renderRect(posX()-camX,
                           posY()-camY,
                           width+1,
                           height+1);
}
