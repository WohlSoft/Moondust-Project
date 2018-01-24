#include "lvl_dtc_player_pos.h"
#include "../../scene_level.h"

PlayerPosDetector::PlayerPosDetector(LVL_Npc *parent):
    BasicDetector(parent)
{
    m_directedTo = 0.0;
    m_playersDirection = 0;
    m_playerPtr = NULL;
    m_playerId = 0;
    m_playerState = 0;
}

PlayerPosDetector::PlayerPosDetector(const PlayerPosDetector &dtc):
    BasicDetector(dtc)
{
    m_pos = dtc.m_pos;
    m_directedTo = dtc.m_directedTo;
    m_playersDirection = dtc.m_playersDirection;
    m_playerPtr = dtc.m_playerPtr;
    m_playerId = dtc.m_playerId;
    m_playerState = dtc.m_playerState;
}

PlayerPosDetector::~PlayerPosDetector()
{}

void PlayerPosDetector::processDetector()
{
    if(!_parentNPC)
        return;

    double d = 0.0;
    for(size_t i = 0; i < _scene->m_itemsPlayers.size(); i++)
    {
        LVL_Player *p = _scene->m_itemsPlayers[i];
        double d1 = distance(p);
        if((d1 < d) || (i == 0))
        {
            d = d1;
            m_pos.setPoint(p->m_momentum.centerX(), p->m_momentum.centerY());
            m_playersDirection = p->direction();
            if(_parentNPC->posCenterX() < m_pos.x())
                m_directedTo = 1;
            else if(_parentNPC->posCenterX() > m_pos.x())
                m_directedTo = -1;
            m_playerPtr = p;
            m_playerId = p->characterID;
            m_playerState = p->stateID;
        }
    }

}

int PlayerPosDetector::directedTo()
{
    return m_directedTo;
}

int PlayerPosDetector::playersDirection()
{
    return m_playersDirection;
}

PGE_PointF PlayerPosDetector::position()
{
    return m_pos;
}

double PlayerPosDetector::positionX()
{
    return m_pos.x();
}

double PlayerPosDetector::positionY()
{
    return m_pos.y();
}

unsigned long PlayerPosDetector::playersCharID()
{
    return m_playerId;
}

unsigned long PlayerPosDetector::playersStateID()
{
    return m_playerState;
}

LVL_Player *PlayerPosDetector::playerPtr()
{
    return m_playerPtr;
}

double PlayerPosDetector::distance(LVL_Player *plr)
{
    //distance between player and parent NPC
    return sqrt(pow(_parentNPC->posCenterX() - plr->posCenterX(), 2.0) +
                pow(_parentNPC->posCenterY() - plr->posCenterY(), 2.0));
}



