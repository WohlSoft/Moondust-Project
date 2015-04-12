#ifndef PGESOCKETDEFINES_H
#define PGESOCKETDEFINES_H

#include <QtGlobal>

//This is the network ID.
//For every change you should increment it.
extern const qlonglong PGENetworkID = 0;

//These are the packets which should be implemented on server side and client side
enum class PGEPackets{
    PACKET_HANDSHAKE
};


#endif // PGESOCKETDEFINES_H
