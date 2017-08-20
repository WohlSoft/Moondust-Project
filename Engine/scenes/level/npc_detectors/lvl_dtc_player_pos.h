#ifndef PLAYERPOSDETECTOR_H
#define PLAYERPOSDETECTOR_H

#include "lvl_base_detector.h"
#include <common_features/pointf.h>

class LVL_Player;
class LevelScene;
class PlayerPosDetector : public BasicDetector
{
public:
    PlayerPosDetector(LVL_Npc *parent);
    PlayerPosDetector(const PlayerPosDetector &dtc);
    ~PlayerPosDetector();
    void processDetector();

    int directedTo();       //! where you shold go to be nearer to player
    int playersDirection(); //! self direction of player
    PGE_PointF position();  //! X-Y position of a player's center
    double positionX();
    double positionY();
    unsigned long playersCharID();    //! returns ID of playable character
    unsigned long playersStateID();   //! returns ID of player's state
    LVL_Player *playerPtr(); //! returns pointer to detected platable character

private:
    double distance(LVL_Player *plr);
    int m_directedTo;
    int m_playersDirection;
    PGE_PointF m_pos;
    LVL_Player *m_playerPtr;
    unsigned long m_playerId;
    unsigned long m_playerState;
};

#endif // PLAYERPOSDETECTOR_H
