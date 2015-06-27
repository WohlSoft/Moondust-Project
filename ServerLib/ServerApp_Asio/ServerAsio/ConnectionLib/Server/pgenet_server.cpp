#include "pgenet_server.h"

#include "../Shared/pgenet_global.h"

PGENET_Server::PGENET_Server() :
    m_service(),
    m_llserver(m_service, PGENET_Global::Port)
{

}

void PGENET_Server::start()
{
    m_llserver.startAccepting();
}

