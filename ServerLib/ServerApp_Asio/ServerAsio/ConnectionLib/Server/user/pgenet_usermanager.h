#ifndef PGENET_USERMANAGER_H
#define PGENET_USERMANAGER_H

#include <QObject>
#include <ConnectionLib/Shared/packetV2/packets/packet.h>
#include <ConnectionLib/Server/low-level/pgenetll_session.h>
#include "pgenet_user.h"


#include <vector>
#include <memory>

#include <QMutex>
#include <QMutexLocker>
#include <functional>

class PGENET_UserManager
{
    Q_DISABLE_COPY(PGENET_UserManager)
private:
    QMutex mutex;
public:
    PGENET_UserManager();
    PGENET_User* getUserByName(const QString& name);

    void registerUser(const QString& name, std::shared_ptr<PGENETLL_Session> sessionObj);
    std::function<void(std::shared_ptr<PGENETLL_Session>)> getNewIncomingConnectionHandler();

private:
    void newIncomingConnection(std::shared_ptr<PGENETLL_Session> newUnindentifiedSession);

    std::vector<std::shared_ptr<PGENETLL_Session> > m_unindentifiedUsers;
    std::vector<std::unique_ptr<PGENET_User> > m_regUsers;
};

#endif // PGENET_USERMANAGER_H
