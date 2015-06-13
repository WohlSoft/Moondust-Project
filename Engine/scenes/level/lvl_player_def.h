#ifndef LVL_PLAYERDEF_H
#define LVL_PLAYERDEF_H

#include "../../data_configs/config_manager.h"

class LVL_PlayerDef
{
public:
    LVL_PlayerDef();
    ~LVL_PlayerDef();
    LVL_PlayerDef(const LVL_PlayerDef &d);
    void setCharacterID(int _id);
    void setState(int _state);
    void setPlayerID(int id);
    int playerID();
    int characterID();
    int currentState();
    int width();
    int height();
    int heightDuck();

private:
    int _playerID;
    int _currentState;
    int _characterID;
};

#endif // LVL_PLAYERDEF_H
