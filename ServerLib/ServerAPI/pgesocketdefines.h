#ifndef PGESOCKETDEFINES_H
#define PGESOCKETDEFINES_H

#include <QtGlobal>

//This is the network ID.
//For every change you should increment it.
static const qlonglong PGENetworkVersion = 0;
static const quint16 PGENetworkPort = 64100;

//These are the packets which should be implemented on server side and client side
enum class PGEPackets{
    PACKET_UNDEFINED,
    PACKET_HANDSHAKE,
    PACKET_HANDSHAKE_ACCEPTED
};


#endif // PGESOCKETDEFINES_H
