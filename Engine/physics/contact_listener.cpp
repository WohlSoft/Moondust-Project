#include "contact_listener.h"
#include "../physics/base_object.h"

#include <QtDebug>


static int m_numFootContacts = 0;

void PGEContactListener::BeginContact(b2Contact *contact)
{
    b2ContactListener::BeginContact(contact);

//      b2Fixture* fixtureA = contact->GetFixtureA();
//      b2Fixture* fixtureB = contact->GetFixtureB();

//      if(!fixtureA) return;
//      if(!fixtureB) return;

//      //check if one of the fixtures is the platform
//      b2Fixture* platformFixture = NULL;
//      b2Fixture* otherFixture = NULL;

//      PGE_Phys_Object * bodyA = NULL;
//      PGE_Phys_Object * bodyB = NULL;

//      bodyA = static_cast<PGE_Phys_Object *>(fixtureA->GetBody()->GetUserData());
//      bodyB = static_cast<PGE_Phys_Object *>(fixtureB->GetBody()->GetUserData());

//      if(!bodyA) return;
//      if(!bodyB) return;


//      if(bodyA->type == PGE_Phys_Object::LVLBlock)
//            qDebug() << bodyA->collide;


//      if ( bodyA->collide == PGE_Phys_Object::COLLISION_TOP )
//      {
//          platformFixture = fixtureA;
//          otherFixture = fixtureB;
//      }
//      else if ( bodyB->collide == PGE_Phys_Object::COLLISION_TOP )
//      {
//          platformFixture = fixtureB;
//          otherFixture = fixtureA;
//      }

//      bool solid = true;

//      if ( platformFixture )
//      {
//          qDebug() << "Check!";

//          int numPoints = contact->GetManifold()->pointCount;

//          b2WorldManifold worldManifold;
//          contact->GetWorldManifold( &worldManifold );

//          b2Body* platformBody = platformFixture->GetBody();
//          b2Body* otherBody = otherFixture->GetBody();

//          int platformScale = 1;

//          //check if contact points are moving into platform
//          solid = false;
//          for (int i = 0; i < numPoints; i++)
//          {
//              b2Vec2 pointVelPlatform =
//                  platformBody->GetLinearVelocityFromWorldPoint( worldManifold.points[i] );

//              b2Vec2 pointVelOther =
//                  otherBody->GetLinearVelocityFromWorldPoint( worldManifold.points[i] );

//              b2Vec2 relativeVel = platformBody->GetLocalVector( pointVelOther - pointVelPlatform );

//              if ( relativeVel.y < -1 )
//                  solid = true;  //point is moving into platform, leave contact solid

//              else if ( relativeVel.y < 1 )
//              {
//                  //borderline case, moving only slightly out of platform
//                  b2Vec2 contactPointRelativeToPlatform =
//                          platformBody->GetLocalPoint( worldManifold.points[i] );
//                  float platformFaceY = 0.5f * 1 / (float)platformScale;
//                  if ( contactPointRelativeToPlatform.y > platformFaceY - 0.05 )
//                      solid = true;
//              }
//          }
//      }

//      if(bodyA->type == PGE_Phys_Object::LVLBlock)
//            qDebug() << "Solid" << solid;

//      if ( solid )
//      {
//          /*
//          //check if fixture A was the foot
//          void* fixtureUserData = contact->GetFixtureA()->GetUserData();
//          if ( (int)fixtureUserData == 100 ) {
//              m_numFootContacts++;
//          }
//          //check if fixture B was the foot
//          fixtureUserData = contact->GetFixtureB()->GetUserData();
//          if ( (int)fixtureUserData == 100 ) {
//              m_numFootContacts++;
//          }*/
//      }
//      else
//          //no points are moving into platform, contact should not be solid
//          contact->SetEnabled(false);
}

void PGEContactListener::EndContact(b2Contact *contact)
{
    //    //reset the default state of the contact in case it comes back for more
    //    if ( contact->IsEnabled() ) {

    //        //        //check if fixture A was the foot
    //        //        void* fixtureUserData = contact->GetFixtureA()->GetUserData();
    //        //        if ( (int)fixtureUserData == 100 )
    //        //            m_numFootContacts--;
    //        //        //check if fixture B was the foot
    //        //        fixtureUserData = contact->GetFixtureB()->GetUserData();
    //        //        if ( (int)fixtureUserData == 100 )
    //        //            m_numFootContacts--;
    //    }
    //    contact->SetEnabled(true);
}

void PGEContactListener::PreSolve(b2Contact *contact, const b2Manifold *oldManifold)
{
//    const b2Manifold* manifold = contact->GetManifold();

//    b2Fixture* fixtureA = contact->GetFixtureA();
//    b2Fixture* fixtureB = contact->GetFixtureB();

//    b2WorldManifold worldManifold;
//    contact->GetWorldManifold(&worldManifold);

//    b2Body* bodyA = fixtureA->GetBody();
//    b2Body* bodyB = fixtureB->GetBody();
        b2ContactListener::PreSolve(contact, oldManifold);

        b2Fixture* fixtureA = contact->GetFixtureA();
        b2Fixture* fixtureB = contact->GetFixtureB();

        b2Fixture* platformFixture = NULL;
        b2Fixture* otherFixture = NULL;

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
            otherFixture = fixtureB;
            bodyBlock = bodyA;
            bodyChar = bodyB;
        }
        else if ( bodyB->collide == PGE_Phys_Object::COLLISION_TOP )
        {
            platformFixture = fixtureB;
            otherFixture = fixtureA;
            bodyBlock = bodyB;
            bodyChar = bodyA;
        }

        if(platformFixture)
        {
            if (bodyChar->bottom() > bodyBlock->top() )
            {
                contact->SetEnabled(false);
            }
        }
        else
        {   //Collision with any-side
            if ( bodyA->type == PGE_Phys_Object::LVLBlock && bodyA->collide == PGE_Phys_Object::COLLISION_ANY )
            {
                platformFixture = fixtureA;
                otherFixture = fixtureB;
                bodyBlock = bodyA;
                bodyChar = bodyB;
            }
            else
            if (  bodyB->type == PGE_Phys_Object::LVLBlock && bodyB->collide == PGE_Phys_Object::COLLISION_ANY )
            {
                platformFixture = fixtureB;
                otherFixture = fixtureA;
                bodyBlock = bodyB;
                bodyChar = bodyA;
            }

            if(platformFixture)
            {
                if(bodyBlock->isRectangle)
                if( bodyChar->bottom() > bodyBlock->top() && bodyChar->bottom() < bodyBlock->top()+2)
                {
                    bodyChar->_player_moveup = true;
                    contact->SetEnabled(false);
                }
            }
        }



}

void PGEContactListener::PostSolve(b2Contact *contact, const b2ContactImpulse *Impulse)
{
    b2ContactListener::PostSolve(contact, Impulse);

}
