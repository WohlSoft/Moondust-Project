#include "pgeclient.h"

PGEClient::PGEClient(QObject *parent) :
    PGEConnection(parent)
{}

PGEClient::PGEClient(const QMap<PGEPackets, QMetaObject *> &toRegisterPackets, QObject *parent) :
    PGEConnection(toRegisterPackets, parent)
{}
