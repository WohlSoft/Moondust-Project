#include "lvl_dtc_inarea.h"
#include "../lvl_scene_ptr.h"

InAreaDetector::InAreaDetector(LVL_Npc *parent, PGE_RectF range, int filters)
    : BasicDetector(parent)
{
    _filters=filters;
    _srcTrapZone=range;
    _trapZone=range;
    _contacts=0;
}

InAreaDetector::InAreaDetector(const InAreaDetector &dtc) :
    BasicDetector(dtc)
{
    _filters=dtc._filters;
    _srcTrapZone=dtc._srcTrapZone;
    _trapZone=dtc._trapZone;
    _contacts=dtc._detected;
    detectedPLR=dtc.detectedPLR;
    detectedBLK=dtc.detectedBLK;
    detectedBGO=dtc.detectedBGO;
    detectedNPC=dtc.detectedNPC;
}

InAreaDetector::~InAreaDetector()
{}

void InAreaDetector::processDetector()
{
    _contacts = 0;
    _detected=false;
    _trapZone=_srcTrapZone;
    _trapZone.setPos(
                _parentNPC->posCenterX()+_srcTrapZone.left(),
                _parentNPC->posCenterY()+_srcTrapZone.top()
                );

    detectedPLR.clear();
    detectedBLK.clear();
    detectedBGO.clear();
    detectedNPC.clear();

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
            case PGE_Phys_Object::LVLPlayer: if((_filters&F_PLAYER)==0) continue;
                {
                    LVL_Player* p= static_cast<LVL_Player*>(visibleBody);
                    if(!p) continue;
                    detectedPLR[p->data.id]=1;
                    break;
                }
            case PGE_Phys_Object::LVLBGO:    if((_filters&F_BGO)==0) continue;
                {
                    LVL_Bgo* b= static_cast<LVL_Bgo*>(visibleBody);
                    if(!b) continue;
                    detectedBGO[b->data.id]=1;
                    break;
                }
            case PGE_Phys_Object::LVLNPC:    if((_filters&F_NPC)==0) continue;
                {
                    LVL_Npc* n= static_cast<LVL_Npc*>(visibleBody);
                    if(!n) continue;
                    detectedNPC[n->data.id]=1;
                    break;
                }
            case PGE_Phys_Object::LVLBlock:  if((_filters&F_BLOCK)==0) continue;
                {
                    LVL_Block* s= static_cast<LVL_Block*>(visibleBody);
                    if(!s) continue;
                    detectedBLK[s->data.id]=1;
                    break;
                }
            default:continue;
        }
        _detected=true;
        _contacts++;
    }
}

bool InAreaDetector::detected(long type, long ID)
{
    if(!_detected) return false;
    switch(type)
    {
        case 1: return detectedBLK.contains(ID); break;
        case 2: return detectedBGO.contains(ID); break;
        case 3: return detectedNPC.contains(ID); break;
        case 4: return detectedPLR.contains(ID); break;
        default:break;
    }
    return false;
}

int InAreaDetector::contacts()
{
    return _contacts;
}


