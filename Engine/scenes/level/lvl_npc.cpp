/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "lvl_section.h"

#include "lvl_scene_ptr.h"

LVL_Npc::LVL_Npc()
{
    type = LVLNPC;
    data = FileFormats::dummyLvlNpc();
    animated=false;
    animator_ID=0;
    killed=false;
    isActivated=false;
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
    setSize(setup->width, setup->height);


    int imgOffsetX = (int)round( - ( ( (double)setup->gfx_w - (double)setup->width ) / 2 ) );
    int imgOffsetY = (int)round( - (double)setup->gfx_h + (double)setup->height + (double)setup->gfx_offset_y);
    offset.setSize(imgOffsetX+(-((double)setup->gfx_offset_x)*data.direct), imgOffsetY);
    frameSize.setSize(setup->gfx_w, setup->gfx_h);
    animator.construct(texture, *setup);

    setDefaults();

        b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.fixedRotation=true;
    bodyDef.awake=false;
    bodyDef.allowSleep=true;
    bodyDef.position.Set( PhysUtil::pix2met( data.x+posX_coefficient ),
        PhysUtil::pix2met(data.y + posY_coefficient ) );
    bodyDef.linearDamping = 2.0;
    bodyDef.userData = (void*)dynamic_cast<PGE_Phys_Object *>(this);
    bodyDef.gravityScale = setup->gravity ? 1.0f : 0.f;
    physBody = worldPtr->CreateBody(&bodyDef);

    b2PolygonShape shape;
    shape.SetAsBox(PhysUtil::pix2met(posX_coefficient-0.01), PhysUtil::pix2met(posY_coefficient-0.01) );
    if(setup->block_player)
        collide = COLLISION_ANY;
    else
    if(setup->block_player_top)
        collide = COLLISION_TOP;
    else
        collide = COLLISION_NONE;

    f_npc = physBody->CreateFixture(&shape, 1.0f);
    f_npc->SetSensor( false );
    f_npc->SetDensity(1.0);
    f_npc->SetFriction(0.3f);

    b2EdgeShape edgeShape;
    edgeShape.Set( b2Vec2(PhysUtil::pix2met(-posX_coefficient-0.01), PhysUtil::pix2met(-posY_coefficient-0.01)),
                   b2Vec2(PhysUtil::pix2met(posX_coefficient+0.01), PhysUtil::pix2met(-posY_coefficient-0.01)) );

    if(collide!=COLLISION_TOP)
    {
        edgeShape.m_vertex0.Set( PhysUtil::pix2met(-posX_coefficient-0.01), PhysUtil::pix2met(posY_coefficient+0.01) );
        edgeShape.m_vertex3.Set( PhysUtil::pix2met(posX_coefficient+0.01), PhysUtil::pix2met(posY_coefficient+0.01) );
    }
    else
    {
        edgeShape.m_vertex0.Set( PhysUtil::pix2met(-posX_coefficient), PhysUtil::pix2met(-posY_coefficient) );
        edgeShape.m_vertex3.Set( PhysUtil::pix2met(posX_coefficient), PhysUtil::pix2met(-posY_coefficient) );
    }
    edgeShape.m_hasVertex0 = true;
    edgeShape.m_hasVertex3 = true;
    f_edge = physBody->CreateFixture(&edgeShape, 1.0f);
    f_edge->SetFriction( 0.04f );
    if(collide==COLLISION_NONE)// || collide==COLLISION_TOP)
        f_edge->SetSensor(true);
}

void LVL_Npc::kill()
{
    killed=true;
    sct()->unregisterElement(this);
    LvlSceneP::s->dead_npcs.push_back(this);
}

void LVL_Npc::update(float ticks)
{
    if(killed)
    {
        if(physBody && worldPtr)
        {
          physBody->SetAwake(false);
          physBody->SetType(b2_staticBody);
          f_npc->SetSensor(true);
          f_edge->SetSensor(true);
        }
        return;
    }
    PGE_Phys_Object::update(ticks);
    timeout-=ticks;
    animator.manualTick(ticks);

    if(motionSpeed!=0)
    {
        if(!blocks_left.isEmpty())
            direction=1;
        else
        if(!blocks_right.isEmpty())
            direction=-1;
        physBody->SetLinearVelocity(b2Vec2(PhysUtil::pix2met((motionSpeed)*direction),  physBody->GetLinearVelocity().y));
    }

    LVL_Section *section=sct();
    PGE_RectF sBox = section->sectionRect();

    if(section->isWarp())
    {
        if(posX() < sBox.left()-width-1 )
            physBody->SetTransform(b2Vec2(
                 PhysUtil::pix2met(sBox.right()+posX_coefficient-1),
                 physBody->GetPosition().y), 0.0f);
        else
        if(posX() > sBox.right() + 1 )
            physBody->SetTransform(b2Vec2(
                 PhysUtil::pix2met(sBox.left()-posX_coefficient+1 ),
                 physBody->GetPosition().y), 0.0f
                                   );
    }
}

void LVL_Npc::render(double camX, double camY)
{
    if(killed) return;
    if(!isActivated) return;

    if(physBody)
    {
        if(!physBody->IsAwake())
        {
            physBody->SetAwake(true);
        }
        if(!is_scenery)
        {
            if(physBody->GetType()!=b2_dynamicBody)
                physBody->SetType(b2_dynamicBody);
        }
    }

    AniPos x(0,1);
    if(animated)
    {
        if(is_scenery)
            x=ConfigManager::Animator_NPC[animator_ID].image(direction);
        else
            x=animator.image(direction);
    }

    GlRenderer::renderTexture(&texture, posX()-camX+offset.w(),
                              posY()-camY+offset.h(),
                              frameSize.w(),
                              frameSize.h(),
                              x.first, x.second);
}

void LVL_Npc::setDefaults()
{
    direction=data.direct;

    if(!setup) return;
    motionSpeed = ((!data.nomove)&&(setup->movement)) ? setup->speed : 0.0f;
    is_scenery  = setup->scenery;
}

void LVL_Npc::Activate()
{
    if(!physBody->IsAwake())
    {
        physBody->SetAwake(true);
    }

    if(!is_scenery)
    {
        if(physBody->GetType()!=b2_dynamicBody)
            physBody->SetType(b2_dynamicBody);
        timeout=4000;
    }
    else
        timeout=150;
    isActivated=true;
    animator.start();
}

void LVL_Npc::deActivate()
{
    isActivated=false;
    physBody->SetAwake(false);
    if(!is_scenery)
    {
        setDefaults();
        if(physBody->GetType()!=b2_staticBody)
            physBody->SetType(b2_staticBody);
        setPos(data.x, data.y);
    }
    animator.stop();
}
