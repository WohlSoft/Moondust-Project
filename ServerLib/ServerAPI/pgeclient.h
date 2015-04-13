#ifndef PGECLIENT_H
#define PGECLIENT_H

#include "pgeconnection.h"

class PGEClient : public PGEConnection
{
    Q_OBJECT
public:
    explicit PGEClient(QObject *parent = 0);
    explicit PGEClient(const QMap<PGEPackets, QMetaObject*> &toRegisterPackets, QObject *parent = 0);
signals:

public slots:

};

#endif // PGECLIENT_H
