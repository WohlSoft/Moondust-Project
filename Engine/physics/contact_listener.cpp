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
#include "../scenes/level/lvl_warp.h"

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

    if(platformFixture)
    {
        qDebug() <<"Contacted With Warp";
        dynamic_cast<LVL_Player *>(bodyChar)->contactedWarp = dynamic_cast<LVL_Warp *>(bodyBlock);
        dynamic_cast<LVL_Player *>(bodyChar)->contactedWithWarp=true;
        dynamic_cast<LVL_Player *>(bodyChar)->warpsTouched++;
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

    //PGE_Phys_Object * bodyBlock = NULL;
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
        //bodyBlock = bodyA;
        bodyChar = bodyB;
    }
    else if ( bodyB->type == PGE_Phys_Object::LVLWarp && bodyA->type == PGE_Phys_Object::LVLPlayer )
    {
        platformFixture = fixtureB;
        //otherFixture = fixtureA;
        //bodyBlock = bodyB;
        bodyChar = bodyA;
    }

    if(platformFixture)
    {
        qDebug() <<"Contact With Warp end";
        dynamic_cast<LVL_Player *>(bodyChar)->warpsTouched--;
        if(dynamic_cast<LVL_Player *>(bodyChar)->warpsTouched==0)
        {
            dynamic_cast<LVL_Player *>(bodyChar)->contactedWithWarp=false;
            dynamic_cast<LVL_Player *>(bodyChar)->contactedWarp = NULL;
        }
    }

}

void PGEContactListener::PreSolve(b2Contact *contact, const b2Manifold *oldManifold)
{
    b2ContactListener::PreSolve(contact, oldManifold);

        b2Fixture* fixtureA = contact->GetFixtureA();
        b2Fixture* fixtureB = contact->GetFixtureB();


        /****************** Top collision************************/
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

        //Top collisions check
        if ( bodyA->collide == PGE_Phys_Object::COLLISION_TOP )
        {
            platformFixture = fixtureA;
            //otherFixture = fixtureB;
            bodyBlock = bodyA;
            bodyChar = bodyB;
        }
        else if ( bodyB->collide == PGE_Phys_Object::COLLISION_TOP )
        {
            platformFixture = fixtureB;
            //otherFixture = fixtureA;
            bodyBlock = bodyB;
            bodyChar = bodyA;
        }

        if(platformFixture)
        {
            if(bodyChar->physBody->GetLinearVelocity().y>10 && bodyChar->bottom() < bodyBlock->top()+10)
            {
                contact->SetEnabled(true);
            }
            else if( (bodyChar->bottom() > bodyBlock->top()+2) )
            {
                contact->SetEnabled(false);
            }
        }
        else
        {   //Collision with any-side
            if ( bodyA->type == PGE_Phys_Object::LVLBlock && bodyA->collide == PGE_Phys_Object::COLLISION_ANY )
            {
                platformFixture = fixtureA;
                //otherFixture = fixtureB;
                bodyBlock = bodyA;
                bodyChar = bodyB;
            }
            else
            if (  bodyB->type == PGE_Phys_Object::LVLBlock && bodyB->collide == PGE_Phys_Object::COLLISION_ANY )
            {
                platformFixture = fixtureB;
                //otherFixture = fixtureA;
                bodyBlock = bodyB;
                bodyChar = bodyA;
            }

            if(platformFixture)
            {
                if(dynamic_cast<LVL_Block *>(bodyBlock)->destroyed)
                {
                        contact->SetEnabled(false);
                        return;
                }

                if(bodyChar->top() >= bodyBlock->bottom() && bodyChar->top() <= bodyBlock->bottom()+3
                        && bodyChar->physBody->GetLinearVelocity().y < -0.01 )
                {
                    if(dynamic_cast<LVL_Block *>(bodyBlock)->setup->hitable)
                    {
                        dynamic_cast<LVL_Player *>(bodyChar)->bump();
                    }
                    dynamic_cast<LVL_Block *>(bodyBlock)->hit();
                }

                if(dynamic_cast<LVL_Block *>(bodyBlock)->destroyed)
                {
                        dynamic_cast<LVL_Player *>(bodyChar)->bump();
                        contact->SetEnabled(false);
                        return;
                }

                if(dynamic_cast<LVL_Block *>(bodyBlock)->isHidden)
                {
                    contact->SetEnabled(false);
                    return;
                }

                if(bodyBlock->isRectangle)
                {
                    if( bodyChar->bottom() <= bodyBlock->top() && bodyChar->bottom() <= bodyBlock->top()+3 )
                    {
                        if(dynamic_cast<LVL_Block *>(bodyBlock)->setup->bounce)
                        {
                            dynamic_cast<LVL_Player *>(bodyChar)->bump(true);
                            dynamic_cast<LVL_Block *>(bodyBlock)->hit(LVL_Block::down);
                        }
                    }
                    else
                    if( bodyChar->bottom() > bodyBlock->top() && bodyChar->bottom() < bodyBlock->top()+2 )
                    {
                        bodyChar->_player_moveup = true;
                        contact->SetEnabled(false);
                    }
                }
            }
        }

}

void PGEContactListener::PostSolve(b2Contact *contact, const b2ContactImpulse *Impulse)
{
    b2ContactListener::PostSolve(contact, Impulse);

}
