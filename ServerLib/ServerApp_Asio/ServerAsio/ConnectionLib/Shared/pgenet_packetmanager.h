#ifndef PGENET_PACKETMANAGER_H
#define PGENET_PACKETMANAGER_H

#include "packetV2/pgepacketregister.h"
#include "pgenet_global.h"

using PGENET_DefPacketRegister = PGENET_PacketRegister<PacketID>;

class PGENET_PacketManager
{
    Q_DISABLE_COPY(PGENET_PacketManager)
public:
    PGENET_PacketManager();

    PGENET_DefPacketRegister* getPacketRegister();
    void registerAllPackets();


protected:
    PGENET_DefPacketRegister m_packetRegister;
};

#endif // PGENET_PACKETMANAGER_H
