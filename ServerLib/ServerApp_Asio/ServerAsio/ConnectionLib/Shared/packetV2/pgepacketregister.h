#ifndef PGEPACKETREGISTER_H
#define PGEPACKETREGISTER_H

#include <QObject>
#include <QMutex>
#include <QMutexLocker>
#include <QMetaType>
#include <QMap>

#include <type_traits>
#include <typeinfo>

#include "packets/packet.h"

//To use this class add the friends below.

///
/// \brief This Class register packets
///
template<typename PacketEnum>
class PGENET_PacketRegister
{
private:
    Q_DISABLE_COPY(PGENET_PacketRegister)
public:
    static_assert(std::is_enum<PacketEnum>::value, "PacketEnum must be an Enum!");

    PGENET_PacketRegister() {

    }
private:
    //We don't want this class to be copied as it is a register.

    QMap<PacketEnum, const QMetaObject*> m_registeredPackets;
    QMutex mutex;


public:


    ///
    /// \brief registerPacket Register a packet to the packet register and QMetatype if needed.
    /// \param packetID The id of the packet which the packet-class gets registered.
    /// \return True, if the register process is a success.
    ///
    template<typename T>
    bool registerPacket(PacketEnum packetID){
        QMutexLocker locker(&mutex);

        static_assert(std::is_base_of<Packet, T>::value, "Packets must be derived from the base-class \"Packet\"");
        static_assert(std::is_base_of<QObject, T>::value, "Packets must be derived from the base-class \"QObject\"");

        //If it is already registed then return false
        if(m_registeredPackets.contains(packetID))
            return false;

        if(static_cast<int>(packetID) == 0)
            return false;

        m_registeredPackets[packetID] = &T::staticMetaObject;
        return true;
    }

    ///
    /// \brief createPacketById Creates a new packet-class by packetID.
    /// \param packetID The packetID for the registered class.
    /// \return A new instance of the packet class.
    ///
    Packet* createPacketById(PacketEnum packetID){
        QMutexLocker locker(&mutex);

        //If not registered then return nullptr
        if(!m_registeredPackets.contains(packetID))
            return nullptr;

        Packet* newPacket = qobject_cast<Packet*>(m_registeredPackets[packetID]->newInstance());
        if(!newPacket)
            return nullptr;

        newPacket->setPacketID(static_cast<int>(packetID));
        return newPacket;
    }

};

#endif // PGEPACKETREGISTER_H
