#include "lvl_dtc_player_inarea.h"
#include "../lvl_scene_ptr.h"

PlayerInAreaDetector::PlayerInAreaDetector(LVL_Npc *parent, PGE_RectF range)
    : BasicDetector(parent)
{
    _srcTrapZone=range;
    _trapZone=range;
    _contacts=0;
}

PlayerInAreaDetector::PlayerInAreaDetector(const PlayerInAreaDetector &dtc) :
    BasicDetector(dtc)
{
    _srcTrapZone=dtc._srcTrapZone;
    _trapZone=dtc._trapZone;
    _contacts=dtc._detected;
}

PlayerInAreaDetector::~PlayerInAreaDetector()
{}

void PlayerInAreaDetector::processDetector()
{
    _contacts = 0;
    _detected=false;
    _trapZone=_srcTrapZone;
    _trapZone.setPos(
                _parentNPC->posCenterX()+_srcTrapZone.left(),
                _parentNPC->posCenterY()+_srcTrapZone.top()
                );

    QVector<PGE_Phys_Object*> bodies;
    LvlSceneP::s->queryItems(_trapZone, &bodies);

    for(PGE_RenderList::iterator it=bodies.begin();it!=bodies.end(); it++ )
    {
        PGE_Phys_Object*visibleBody=*it;
        if(visibleBody==_parentNPC) continue;
        if(visibleBody==NULL)
            continue;
        if(!visibleBody->isVisible()) //Don't whip elements from hidden layers!
            continue;
        switch(visibleBody->type)
        {
            case PGE_Phys_Object::LVLPlayer:
                _detected=true;
                _contacts++;
                default:break;
        }
    }
}

int PlayerInAreaDetector::contacts()
{
    return _contacts;
}


