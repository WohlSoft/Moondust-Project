#ifndef PGENET_USER_H
#define PGENET_USER_H

#include <QObject>

#include <QString>


class PGENET_User
{
    Q_DISABLE_COPY(PGENET_User)
public:
    PGENET_User();

    QString getUsername() const;
private:
    QString username;
};

#endif // PGENET_USER_H
