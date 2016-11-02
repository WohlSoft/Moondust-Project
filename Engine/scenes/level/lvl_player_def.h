#ifndef LVL_PLAYERDEF_H
#define LVL_PLAYERDEF_H

#include "../../data_configs/config_manager.h"

class LVL_PlayerDef
{
    public:
        LVL_PlayerDef();
        ~LVL_PlayerDef();
        LVL_PlayerDef(const LVL_PlayerDef &d);
        void setCharacterID(unsigned long _id);
        void setState(unsigned long _state);
        void setPlayerID(int id);
        int playerID();
        unsigned long characterID();
        unsigned long currentState();
        int width();
        int height();
        int heightDuck();

    private:
        int _playerID;
        unsigned long _currentState;
        unsigned long _characterID;
};

#endif // LVL_PLAYERDEF_H
