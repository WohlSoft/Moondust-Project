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

#include "contact_listener.h"
#include "../physics/base_object.h"

#include "../scenes/level/lvl_player.h"
#include "../scenes/level/lvl_block.h"
#include "../scenes/level/lvl_bgo.h"
#include "../scenes/level/lvl_physenv.h"
#include "../scenes/level/lvl_warp.h"

#include <cstdint>

#include <QtDebug>

void PGEContactListener::BeginContact(b2Contact *contact)
{
    b2ContactListener::BeginContact(contact);

    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();

    b2Fixture* platformFixture = NULL;
    //b2Fixture* otherFixture = NULL;

    PGE_Phys_Object * bodyA = NULL;
    PGE_Phys_Object * bodyB = NULL;

    PGE_Phys_Object * bodyBlock = NULL;
    PGE_Phys_Object * bodyChar = NULL;

    bodyA = static_cast<PGE_Phys_Object *>(fixtureA->GetBody()->GetUserData());
    bodyB = static_cast<PGE_Phys_Object *>(fixtureB->GetBody()->GetUserData());

    if(!bodyA) return;
    if(!bodyB) return;

    /***********************************Warp & Player***********************************/
    if ( bodyA->type == PGE_Phys_Object::LVLWarp && bodyB->type == PGE_Phys_Object::LVLPlayer )
    {
        platformFixture = fixtureA;
        //otherFixture = fixtureB;
        bodyBlock = bodyA;
        bodyChar = bodyB;
    }
    else if ( bodyB->type == PGE_Phys_Object::LVLWarp && bodyA->type == PGE_Phys_Object::LVLPlayer )
    {
        platformFixture = fixtureB;
        //otherFixture = fixtureA;
        bodyBlock = bodyB;
        bodyChar = bodyA;
    }
    else
    {
        platformFixture=NULL;
    }

    if(platformFixture)
    {
        LVL_Player *chr=dynamic_cast<LVL_Player *>(bodyChar);
        if(!chr) return;
        //qDebug() <<"Contacted With Warp";
        chr->contactedWarp = dynamic_cast<LVL_Warp *>(bodyBlock);
        chr->contactedWithWarp=true;
        chr->warpsTouched++;
    }

    /***********************************PhysEnvironment & Player***********************************/
    if ( bodyA->type == PGE_Phys_Object::LVLPhysEnv && bodyB->type == PGE_Phys_Object::LVLPlayer )
    {
        platformFixture = fixtureA;
        //otherFixture = fixtureB;
        bodyBlock = bodyA;
        bodyChar = bodyB;
    }
    else if ( bodyB->type == PGE_Phys_Object::LVLPhysEnv && bodyA->type == PGE_Phys_Object::LVLPlayer )
    {
        platformFixture = fixtureB;
        //otherFixture = fixtureA;
        bodyBlock = bodyB;
        bodyChar = bodyA;
    }
    else
    {
        platformFixture=NULL;
    }

    if(platformFixture)
    {
        LVL_PhysEnv *env=dynamic_cast<LVL_PhysEnv *>(bodyBlock);
        LVL_Player *chr=dynamic_cast<LVL_Player *>(bodyChar);
        if(!chr) return;
        if(!env) return;
        chr->environments_map[(intptr_t)bodyBlock]
                = env->env_type;
    }

    /***********************************BGO & Player***********************************/
    if ( bodyA->type == PGE_Phys_Object::LVLBGO && bodyB->type == PGE_Phys_Object::LVLPlayer )
    {
        platformFixture = fixtureA;
        //otherFixture = fixtureB;
        bodyBlock = bodyA;
        bodyChar = bodyB;
    }
    else if ( bodyB->type == PGE_Phys_Object::LVLBGO && bodyA->type == PGE_Phys_Object::LVLPlayer )
    {
        platformFixture = fixtureB;
        //otherFixture = fixtureA;
        bodyBlock = bodyB;
        bodyChar = bodyA;
    }
    else
    {
        platformFixture=NULL;
    }

    if(platformFixture)
    {
        LVL_Bgo *bgo=dynamic_cast<LVL_Bgo *>(bodyBlock);
        LVL_Player *chr=dynamic_cast<LVL_Player *>(bodyChar);
        if(!chr) return;
        if(!bgo) return;
        if(bgo->setup->climbing)
            chr->climbable_map[(intptr_t)bodyBlock]=1;
    }


    /***********************************Block & Player***********************************/
    if ( (bodyA->type == PGE_Phys_Object::LVLBlock) && (bodyB->type == PGE_Phys_Object::LVLPlayer) )
    {
        platformFixture = fixtureA;
        //otherFixture = fixtureB;
        bodyBlock = bodyA;
        bodyChar = bodyB;
    }
    else if ( (bodyB->type == PGE_Phys_Object::LVLBlock) && (bodyA->type == PGE_Phys_Object::LVLPlayer) )
    {
        platformFixture = fixtureB;
        //otherFixture = fixtureA;
        bodyBlock = bodyB;
        bodyChar = bodyA;
    }
    else
    {
        platformFixture=NULL;
    }

    if(platformFixture)
    {
        if(bodyBlock->collide==PGE_Phys_Object::COLLISION_ANY)
        {
            LVL_Block *blk=dynamic_cast<LVL_Block *>(bodyBlock);
            LVL_Player *chr=dynamic_cast<LVL_Player *>(bodyChar);
            if(!chr) return;
            if(!blk) return;

            if(blk->destroyed)
            {
                    contact->SetEnabled(false);
                    return;
            }

            if(  ( (chr->bottom()<=blk->top()-0.1) ||
                    ((chr->bottom() >= blk->top())&&
                    (chr->bottom()<=blk->top()+2)) )
                 //Uncommend this code piece to disallow wall climbing (now wal climbing is needed without NPC's :P)
              /*   &&( !( (bodyChar->left()>=bodyBlock->right()-2) || (bodyChar->right() <= bodyBlock->left()+2) ) )*/
                 &&(!blk->isHidden)
                  )
            {
                chr->foot_contacts_map[(intptr_t)bodyBlock] = 1;
                chr->onGround=(!chr->foot_contacts_map.isEmpty());
                if(chr->keys.down)
                    chr->climbing=false;
                if(bodyBlock->slippery_surface)
                    chr->foot_sl_contacts_map[(intptr_t)bodyBlock] = 1;
            }

            if(bodyChar->top() >= bodyBlock->bottom() && bodyChar->top() <= bodyBlock->bottom()+3
                    && (bodyChar->physBody->GetLinearVelocity().y < -0.01) )
            {
                chr->jumpForce=0;

                if(blk->setup->hitable)
                {
                    chr->bump();
                }
                blk->hit();
            }

            if(blk->destroyed)
            {
                    chr->bump();
                    contact->SetEnabled(false);
                    return;
            }

            if(blk->isHidden)
            {
                contact->SetEnabled(false);
                return;
            }

            if(bodyBlock->isRectangle)
            {
                if( bodyChar->bottom() <= bodyBlock->top() && bodyChar->bottom() <= bodyBlock->top()+3 )
                {
                    chr->onGround=true;

                    if(blk->setup->bounce)
                    {
                        chr->bump(true);
                        blk->hit(LVL_Block::down);
                    }
                }
                else
                if( (bodyChar->bottom() > bodyBlock->top()) &&
                        (bodyChar->bottom() < bodyBlock->top()+2)
                       && (fabs(bodyChar->physBody->GetLinearVelocity().x)>0))
                {
                    bodyChar->_player_moveup = true;
                    contact->SetEnabled(false);
                }
            }
            else
            {
                chr->onGround=true;
                chr->foot_contacts_map[(intptr_t)bodyBlock]=1;
            }
        }
    }

}

void PGEContactListener::EndContact(b2Contact *contact)
{
    b2ContactListener::EndContact(contact);

    b2ContactListener::BeginContact(contact);

    b2Fixture* fixtureA = contact->GetFixtureA();
    b2Fixture* fixtureB = contact->GetFixtureB();

    b2Fixture* platformFixture = NULL;
    //b2Fixture* otherFixture = NULL;

    PGE_Phys_Object * bodyA = NULL;
    PGE_Phys_Object * bodyB = NULL;

    PGE_Phys_Object * bodyBlock = NULL;
    PGE_Phys_Object * bodyChar = NULL;

    bodyA = static_cast<PGE_Phys_Object *>(fixtureA->GetBody()->GetUserData());
    bodyB = static_cast<PGE_Phys_Object *>(fixtureB->GetBody()->GetUserData());

    if(!bodyA) return;
    if(!bodyB) return;

    Q_UNUSED(bodyBlock);

    /***********************************Warp & Player***********************************/
    if ( (bodyA->type == PGE_Phys_Object::LVLWarp) && (bodyB->type == PGE_Phys_Object::LVLPlayer) )
    {
        platformFixture = fixtureA;
        //otherFixture = fixtureB;
        //bodyBlock = bodyA;
        bodyChar = bodyB;
    }
    else if ( (bodyB->type == PGE_Phys_Object::LVLWarp) && (bodyA->type == PGE_Phys_Object::LVLPlayer) )
    {
        platformFixture = fixtureB;
        //otherFixture = fixtureA;
        //bodyBlock = bodyB;
        bodyChar = bodyA;
    }
    else
    {
        platformFixture=NULL;
    }

    if(platformFixture)
    {
        LVL_Player *chr=dynamic_cast<LVL_Player *>(bodyChar);
        if(!chr) return;
        //qDebug() <<"Contact With Warp end";
        chr->warpsTouched--;
        if(chr->warpsTouched==0)
        {
            chr->contactedWithWarp=false;
            chr->contactedWarp = NULL;
        }
    }
    /***********************************Physical Environment zone & Player***********************************/
    if ( (bodyA->type == PGE_Phys_Object::LVLPhysEnv) && (bodyB->type == PGE_Phys_Object::LVLPlayer) )
    {
        platformFixture = fixtureA;
        //otherFixture = fixtureB;
        bodyBlock = bodyA;
        bodyChar = bodyB;
    }
    else if ( (bodyB->type == PGE_Phys_Object::LVLPhysEnv) && (bodyA->type == PGE_Phys_Object::LVLPlayer) )
    {
        platformFixture = fixtureB;
        //otherFixture = fixtureA;
        bodyBlock = bodyB;
        bodyChar = bodyA;
    }
    else
    {
        platformFixture=NULL;
    }

    if(platformFixture)
    {
        LVL_Player *chr=dynamic_cast<LVL_Player *>(bodyChar);
        if(!chr) return;
        if(chr->environments_map.contains((intptr_t)bodyBlock))
        {
            chr->environments_map.remove((intptr_t)bodyBlock);
        }
    }

    /***********************************BGO & Player***********************************/
    if ( (bodyA->type == PGE_Phys_Object::LVLBGO) && (bodyB->type == PGE_Phys_Object::LVLPlayer) )
    {
        platformFixture = fixtureA;
        //otherFixture = fixtureB;
        bodyBlock = bodyA;
        bodyChar = bodyB;
    }
    else if ( (bodyB->type == PGE_Phys_Object::LVLBGO) && (bodyA->type == PGE_Phys_Object::LVLPlayer) )
    {
        platformFixture = fixtureB;
        //otherFixture = fixtureA;
        bodyBlock = bodyB;
        bodyChar = bodyA;
    }
    else
    {
        platformFixture=NULL;
    }

    if(platformFixture)
    {
        LVL_Player *chr=dynamic_cast<LVL_Player *>(bodyChar);
        if(!chr) return;
        if(chr->climbable_map.contains((intptr_t)bodyBlock))
        {
            chr->climbable_map.remove((intptr_t)bodyBlock);
            chr->climbing =
                    ((!chr->climbable_map.isEmpty()) &&
                        (chr->climbing));
        }
    }

    /***********************************Block & Player***********************************/
    if ( (bodyA->type == PGE_Phys_Object::LVLBlock) && (bodyB->type == PGE_Phys_Object::LVLPlayer) )
    {
        platformFixture = fixtureA;
        //otherFixture = fixtureB;
        bodyBlock = bodyA;
        bodyChar = bodyB;
    }
    else if ( (bodyB->type == PGE_Phys_Object::LVLBlock) && (bodyA->type == PGE_Phys_Object::LVLPlayer) )
    {
        platformFixture = fixtureB;
        //otherFixture = fixtureA;
        bodyBlock = bodyB;
        bodyChar = bodyA;
    }
    else
    {
        platformFixture=NULL;
    }

    if(platformFixture)
    {
        LVL_Player *chr=dynamic_cast<LVL_Player *>(bodyChar);
        if(!chr) return;
        if(chr->foot_contacts_map.contains((intptr_t)bodyBlock))
        {
            chr->foot_contacts_map.remove((intptr_t)bodyBlock);
            chr->onGround  =
                    (!chr->foot_contacts_map.isEmpty());
        }

        if(chr->foot_sl_contacts_map.contains((intptr_t)bodyBlock))
        {
            chr->foot_sl_contacts_map.remove((intptr_t)bodyBlock);
        }
    }

}

void PGEContactListener::PreSolve(b2Contact *contact, const b2Manifold *oldManifold)
{
    b2ContactListener::PreSolve(contact, oldManifold);

        b2Fixture* fixtureA = contact->GetFixtureA();
        b2Fixture* fixtureB = contact->GetFixtureB();

        b2Fixture* platformFixture = NULL;
        //b2Fixture* otherFixture = NULL;

        PGE_Phys_Object * bodyA = NULL;
        PGE_Phys_Object * bodyB = NULL;

        PGE_Phys_Object * bodyBlock = NULL;
        PGE_Phys_Object * bodyChar = NULL;

        bodyA = static_cast<PGE_Phys_Object *>(fixtureA->GetBody()->GetUserData());
        bodyB = static_cast<PGE_Phys_Object *>(fixtureB->GetBody()->GetUserData());

        if(!bodyA) return;
        if(!bodyB) return;

        if ( bodyA->type == PGE_Phys_Object::LVLBlock && bodyB->type == PGE_Phys_Object::LVLPlayer )
        {
            platformFixture = fixtureA;
            //otherFixture = fixtureB;
            bodyBlock = bodyA;
            bodyChar = bodyB;
        }
        else if ( bodyB->type == PGE_Phys_Object::LVLBlock && bodyA->type == PGE_Phys_Object::LVLPlayer )
        {
            platformFixture = fixtureB;
            //otherFixture = fixtureA;
            bodyBlock = bodyB;
            bodyChar = bodyA;
        }
        else
        {
            platformFixture=NULL;
        }


        if(platformFixture)
        {
            LVL_Player *chr=dynamic_cast<LVL_Player *>(bodyChar);
            if(!chr) return;

            if(bodyBlock->type == PGE_Phys_Object::LVLBlock)
            {
                LVL_Block *blk=dynamic_cast<LVL_Block *>(bodyBlock);
                if(!blk) return;
                if(blk->destroyed)
                {
                    contact->SetEnabled(false);
                    return;
                }

                if(blk->setup->lava)
                {
                    chr->kill(LVL_Player::DEAD_burn);
                }
            }

            /*************************No collisions check*****************************/
            if ( bodyBlock->collide == PGE_Phys_Object::COLLISION_NONE )
            {
                contact->SetEnabled(false);
                return;
            }
            else
            /*************************Top collisions check*****************************/
            if( bodyBlock->collide == PGE_Phys_Object::COLLISION_TOP )
            {
                if(
                        (
                            (bodyChar->physBody->GetLinearVelocity().y > 0.1)
                            &&
                            (bodyChar->bottom() < bodyBlock->top()+1)
                            &&
                            (
                                 (bodyChar->left()<bodyBlock->right()-1 ) &&
                                 (bodyChar->right()>bodyBlock->left()+1 )
                             )
                         )
                        ||
                        (bodyChar->bottom() < bodyBlock->top())
                        )
                {
                    contact->SetEnabled(true);
                    chr->foot_contacts_map[(intptr_t)bodyBlock] = 1;
                    chr->onGround  =
                            (!chr->foot_contacts_map.isEmpty());

                    if(chr->keys.down)
                        chr->climbing=false;
                    if(bodyBlock->slippery_surface)
                        chr->foot_sl_contacts_map[(intptr_t)bodyBlock] = 1;

                }
                else //if( (bodyChar->bottom() > bodyBlock->top()+2) )
                {
                    contact->SetEnabled(false);
                }
                return;
            }
            else
            if( bodyBlock->collide == PGE_Phys_Object::COLLISION_ANY )
            /***********************Collision with any-side************************/
            {
                if(bodyBlock->type == PGE_Phys_Object::LVLBlock)
                {
                    LVL_Block *blk=dynamic_cast<LVL_Block *>(bodyBlock);
                    if(!blk) return;
                    if(blk->destroyed)
                    {
                            contact->SetEnabled(false);
                            return;
                    }

                    if(blk->isHidden)
                    {
                        contact->SetEnabled(false);
                        return;
                    }
                }

                if( (bodyChar->left()>bodyBlock->right()) || (bodyChar->right() < bodyBlock->left()) )
                    return;

                if(
                    ( (bodyChar->bottom() <= bodyBlock->top()-0.1)
                        ||
                      (
                            (bodyChar->bottom() >= bodyBlock->top())&&
                         (bodyChar->bottom()<=bodyBlock->top()+2)
                      )
                      )
                   )
                {
                    chr->foot_contacts_map[(intptr_t)bodyBlock]=1;
                    chr->onGround  =
                            (!chr->foot_contacts_map.isEmpty());
                    if(chr->keys.down)
                        chr->climbing=false;

                    if(bodyBlock->slippery_surface)
                        chr->foot_sl_contacts_map[(intptr_t)bodyBlock] = 1;
                }

                return;
            }

        }

}

void PGEContactListener::PostSolve(b2Contact *contact, const b2ContactImpulse *Impulse)
{
    b2ContactListener::PostSolve(contact, Impulse);

}
