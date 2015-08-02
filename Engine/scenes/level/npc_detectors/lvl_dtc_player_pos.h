#ifndef PLAYERPOSDETECTOR_H
#define PLAYERPOSDETECTOR_H

#include "lvl_base_detector.h"
#include <common_features/pointf.h>

class LVL_Player;

class PlayerPosDetector : public BasicDetector
{
public:
    PlayerPosDetector(LVL_Npc* parent);
    PlayerPosDetector(const PlayerPosDetector &dtc);
    ~PlayerPosDetector();
    void processDetector();

    int directedTo();       //! where you shold go to be nearer to player
    int playersDirection(); //! self direction of player
    PGE_PointF position();  //! X-Y position of a player's center

private:
    float distance(LVL_Player* plr);
    int _directedTo;
    int _playersDirection;
    PGE_PointF pos;
};

#endif // PLAYERPOSDETECTOR_H
