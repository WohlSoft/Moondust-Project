#include "packetuserauth.h"

#include "../../../pgenet_global.h"

#include <QDataStream>

PacketUserAuth::PacketUserAuth(QObject* parent) : Packet(parent), m_username(""), m_networkVersionNumber(PGENET_Global::NetworkVersion)
{}




