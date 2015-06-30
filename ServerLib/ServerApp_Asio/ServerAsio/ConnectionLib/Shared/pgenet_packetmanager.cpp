#include "pgenet_packetmanager.h"

PGENET_PacketManager::PGENET_PacketManager()
{

}
PGENET_DefPacketRegister *PGENET_PacketManager::getPacketRegister()
{
    return &m_packetRegister;
}


