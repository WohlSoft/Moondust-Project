#ifndef PLAYERINAREADETECTOR_H
#define PLAYERINAREADETECTOR_H

#include "lvl_base_detector.h"
#include <common_features/rectf.h>

class PlayerInAreaDetector : public BasicDetector
{
public:
    PlayerInAreaDetector(LVL_Npc * parent, PGE_RectF range);
    PlayerInAreaDetector(const PlayerInAreaDetector &dtc);
    ~PlayerInAreaDetector();
    void processDetector();
    //bool detected() //! from base class!
    int contacts();   //! number of detected players

private:
    int _contacts;
    PGE_RectF _srcTrapZone;
    PGE_RectF _trapZone;
};

#endif // PLAYERINAREADETECTOR_H
