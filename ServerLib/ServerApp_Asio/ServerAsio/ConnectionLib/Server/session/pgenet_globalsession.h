#ifndef PGENET_GLOBALSESSION_H
#define PGENET_GLOBALSESSION_H

#include "pgenet_session.h"
#include <ConnectionLib/Shared/user/pgenet_usermanager.h>
#include <memory>

class PGENET_GlobalSession : public PGENET_Session
{
    Q_DISABLE_COPY(PGENET_GlobalSession)
public:
    PGENET_GlobalSession(PGENET_UserManager* userManager);
private:
    PGENET_UserManager* m_userManager;
};

#endif // PGENET_GLOBALSESSION_H
