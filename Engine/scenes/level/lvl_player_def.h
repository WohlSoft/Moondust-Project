#ifndef LVL_PLAYERDEF_H
#define LVL_PLAYERDEF_H

#include "../../data_configs/config_manager.h"

class LVL_PlayerDef
{
public:
    LVL_PlayerDef();
    ~LVL_PlayerDef();
    void setCharacterID(int _id);
    void setState(int _state);
    int characterID();
    int currentState();
    int width();
    int height();
    int heightDuck();

private:
    int _currentState;
    int _characterID;
};

#endif // LVL_PLAYERDEF_H
