#ifndef PLAYERINAREADETECTOR_H
#define PLAYERINAREADETECTOR_H

#include "lvl_base_detector.h"
#include <common_features/rectf.h>
#include <QHash>

class InAreaDetector : public BasicDetector
{
public:
    enum Filters
    {
        F_BLOCK=  1,
        F_BGO=    2,
        F_NPC=    4,
        F_PLAYER= 8
    };
    InAreaDetector(LVL_Npc * parent, PGE_RectF range, int filters);
    InAreaDetector(const InAreaDetector &dtc);
    ~InAreaDetector();
    void processDetector();
    bool detected();
    bool detected(long type, long ID);
    int contacts();   //! number of detected items

private:
    QHash<int, int > detectedPLR;
    QHash<int, int > detectedBLK;
    QHash<int, int > detectedBGO;
    QHash<int, int > detectedNPC;
    int _filters;
    int _contacts;
    PGE_RectF _srcTrapZone;
    PGE_RectF _trapZone;
};

#endif // PLAYERINAREADETECTOR_H
