#include "pgenet_packetmanager.h"

#include "packetV2/packets/ClientToServer/packetuserauth.h"

#include "packetV2/packets/Both/packetmessage.h"

PGENET_PacketManager::PGENET_PacketManager()
{
    registerAllPackets();
}


PGENET_DefPacketRegister *PGENET_PacketManager::getPacketRegister()
{
    return &m_packetRegister;
}


//#define reg_packet(packetName) m_packetRegister.registerPacket<packetName>(PacketID::PGENET_packetName)
void PGENET_PacketManager::registerAllPackets()
{
    //reg_packet(PacketUserAuth);
    //reg_packet(PacketMessage);
    m_packetRegister.registerPacket<PacketUserAuth>(PacketID::PGENET_PacketUserAuth);
    m_packetRegister.registerPacket<PacketMessage>(PacketID::PGENET_PacketMessage);
}


