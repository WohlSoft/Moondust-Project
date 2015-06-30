#ifndef PGENET_USERMANAGER_H
#define PGENET_USERMANAGER_H

#include <QObject>
#include <ConnectionLib/Shared/packetV2/packets/packet.h>
#include "pgenet_user.h"


#include <vector>
#include <memory>

#include <QMutex>
#include <QMutexLocker>

class PGENET_UserManager
{
    Q_DISABLE_COPY(PGENET_UserManager)
private:
    QMutex mutex;
public:
    PGENET_UserManager();

    PGENET_User* getUserByName(const QString& name);
    void resolveUserOfPacket(Packet* packet);


private:
    std::vector<std::unique_ptr<PGENET_User> > m_regUsers;
};

#endif // PGENET_USERMANAGER_H
