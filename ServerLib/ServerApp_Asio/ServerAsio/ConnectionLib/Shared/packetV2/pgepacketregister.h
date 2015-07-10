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
        int regId = QMetaType::type(typeid(Packet).name());
        if(regId == QMetaType::UnknownType){
            regId = qRegisterMetaType<Packet>(typeid(Packet).name());
        }
    }
private:
    //We don't want this class to be copied as it is a register.

    QMap<PacketEnum, int> m_registeredPackets;
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

        //If it is already registed then return false
        if(m_registeredPackets.contains(packetID))
            return false;

        if(static_cast<int>(packetID) == 0)
            return false;

        //Be sure that T is registered as meta-type
        int regId = QMetaType::type(typeid(T).name());
        if(regId == QMetaType::UnknownType){
            regId = qRegisterMetaType<T>(typeid(T).name());
        }


        m_registeredPackets[packetID] = regId;
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

        return reinterpret_cast<Packet*>(QMetaType::create(m_registeredPackets[packetID]));
    }

};

#endif // PGEPACKETREGISTER_H
