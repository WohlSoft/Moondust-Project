#include "lvl_dtc_player_pos.h"
#include "../lvl_scene_ptr.h"

PlayerPosDetector::PlayerPosDetector(LVL_Npc *parent):
    BasicDetector(parent)
{
    _directedTo=0.0;
    _playersDirection=0;
    _plrptr=NULL;
    _plr_id=0;
    _plr_state=0;
}

PlayerPosDetector::PlayerPosDetector(const PlayerPosDetector &dtc):
    BasicDetector(dtc)
{
    pos = dtc.pos;
    _directedTo=dtc._directedTo;
    _playersDirection=dtc._playersDirection;
    _plrptr=dtc._plrptr;
    _plr_id=dtc._plr_id;
    _plr_state=dtc._plr_state;
}

PlayerPosDetector::~PlayerPosDetector()
{}

void PlayerPosDetector::processDetector()
{
    if(!_parentNPC) return;
    _playersDirection=0;
    pos.setPoint(0.0, 0.0);
    _directedTo=0.0;
    _plrptr=NULL;
    _plr_id=0;
    _plr_state=0;
    float d=0.0f;
    for(int i=0; i<LvlSceneP::s->players.size(); i++)
    {
        LVL_Player *p = LvlSceneP::s->players[i];
        float d1=distance(p);
        if((d1<d)||(i==0))
        {
            d=d1;
            pos = p->posRect.center();
            _playersDirection=p->direction();
            if(_parentNPC->posCenterX() < pos.x())
                _directedTo=1;
            else
            if(_parentNPC->posCenterX() > pos.x())
                _directedTo=-1;
            _plrptr=p;
            _plr_id=p->characterID;
            _plr_state=p->stateID;
        }
    }

}

int PlayerPosDetector::directedTo()
{
    return _directedTo;
}

int PlayerPosDetector::playersDirection()
{
    return _playersDirection;
}

PGE_PointF PlayerPosDetector::position()
{
    return pos;
}

int PlayerPosDetector::playersCharID()
{
    return _plr_id;
}

int PlayerPosDetector::playersStateID()
{
    return _plr_state;
}

LVL_Player *PlayerPosDetector::playerPtr()
{
    return _plrptr;
}

float PlayerPosDetector::distance(LVL_Player *plr)
{
    //distance between player and parent NPC
    return sqrt(pow(_parentNPC->posCenterX()-plr->posCenterX(), 2.0)+
    pow(_parentNPC->posCenterY()-plr->posCenterY(), 2.0));
}



