#ifndef CONTACT_LISTENER_H
#define CONTACT_LISTENER_H

#include <Box2D/Box2D.h>
#include <Box2D/Dynamics/Contacts/b2Contact.h>

class PGEContactListener : public b2ContactListener
{
    public:
      //Эта функция вызывается когда два тела начинают пересекаться
      void BeginContact(b2Contact* contact);

      //Эта когда два тела перестают пересекаться
      void EndContact(b2Contact* contact);


      void PreSolve(b2Contact* contact,const b2Manifold* oldManifold);
      void PostSolve(b2Contact* contact,const b2ContactImpulse* Impulse);
};

#endif // CONTACT_LISTENER_H
