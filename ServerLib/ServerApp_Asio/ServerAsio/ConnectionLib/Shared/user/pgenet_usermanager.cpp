#include "pgenet_usermanager.h"

PGENET_UserManager::PGENET_UserManager()
{

}

PGENET_User *PGENET_UserManager::getUserByName(const QString &name)
{
    QMutexLocker locker(&mutex);

    for(unsigned int i = 0; i < m_regUsers.size(); i++){
        std::unique_ptr<PGENET_User>& nextUser = m_regUsers[i];
        if(name.compare(nextUser->getUsername(), Qt::CaseInsensitive))
            return nextUser.get();
    }

    return nullptr;
}

void PGENET_UserManager::resolveUserOfPacket(Packet *packet)
{

}

