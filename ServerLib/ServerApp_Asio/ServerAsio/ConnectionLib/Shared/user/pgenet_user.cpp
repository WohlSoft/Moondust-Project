#include "pgenet_user.h"

PGENET_User::PGENET_User(const QString username) : m_username(username)
{}

QString PGENET_User::getUsername() const
{
    return m_username;
}
std::shared_ptr<PGENETLL_Session> PGENET_User::getSession() const
{
    return m_session;
}

void PGENET_User::setSession(const std::shared_ptr<PGENETLL_Session> &session)
{
    m_session = session;
}



