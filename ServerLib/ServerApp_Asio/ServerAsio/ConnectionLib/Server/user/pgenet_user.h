#ifndef PGENET_USER_H
#define PGENET_USER_H

#include <QObject>

#include <QString>
#include <ConnectionLib/Server/low-level/pgenetll_session.h>

class PGENET_User
{
    Q_DISABLE_COPY(PGENET_User)
public:
    PGENET_User(const QString username);

    QString getUsername() const;

    std::shared_ptr<PGENETLL_Session> getSession() const;
    void setSession(const std::shared_ptr<PGENETLL_Session> &session);

private:
    QString m_username;
    std::shared_ptr<PGENETLL_Session> m_session;
};

#endif // PGENET_USER_H
