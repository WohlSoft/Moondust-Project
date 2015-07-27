#ifndef PGENET_SERVERUSER_H
#define PGENET_SERVERUSER_H

#include <ConnectionLib/Shared/user/pgenet_user.h>

class PGENET_ServerUser : public PGENET_User
{
public:
    PGENET_ServerUser(const QString username);
};

#endif // PGENET_SERVERUSER_H
