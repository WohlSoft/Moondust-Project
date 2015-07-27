#include "pgenet_usermanager.h"

PGENET_UserManager::PGENET_UserManager()
{}

PGENET_ServerUser *PGENET_UserManager::getUserByName(const QString &name)
{
    QMutexLocker locker(&mutex);

    for(unsigned int i = 0; i < m_regUsers.size(); i++){
        std::unique_ptr<PGENET_ServerUser>& nextUser = m_regUsers[i];
        if(name.compare(nextUser->getUsername(), Qt::CaseInsensitive))
            return nextUser.get();
    }

    return nullptr;
}

void PGENET_UserManager::registerUser(const QString &name, std::shared_ptr<PGENETLL_Session> sessionObj)
{
    std::unique_ptr<PGENET_ServerUser> newUser(new PGENET_ServerUser(name));
    newUser->setSession(sessionObj);
    m_regUsers.push_back(std::move(newUser));
}

std::function<void (std::shared_ptr<PGENETLL_Session>)> PGENET_UserManager::getNewIncomingConnectionHandler()
{
    return [this](std::shared_ptr<PGENETLL_Session> newIncomingSession){this->newIncomingConnection(newIncomingSession);};
}

void PGENET_UserManager::newIncomingConnection(std::shared_ptr<PGENETLL_Session> newUnindentifiedSession)
{
    m_unindentifiedUsers.push_back(newUnindentifiedSession);
}
