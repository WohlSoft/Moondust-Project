#include "lvl_player_def.h"


LVL_PlayerDef::LVL_PlayerDef()
{
    _characterID=-1;
    _currentState=0;
}

LVL_PlayerDef::~LVL_PlayerDef()
{}

void LVL_PlayerDef::setCharacterID(int _id)
{
    if(ConfigManager::playable_characters.contains(_id))
        _characterID=_id;
    else
        _characterID=-1;
}

void LVL_PlayerDef::setState(int _state)
{
    if(_characterID>=0)
    {
        if(ConfigManager::playable_characters[_characterID].states.contains(_state))
            _currentState=_state;
        else
            _currentState=0;
    }
    else
        _currentState=0;
}

int LVL_PlayerDef::characterID()
{
    return _characterID;
}

int LVL_PlayerDef::currentState()
{
    return _currentState;
}

int LVL_PlayerDef::width()
{
    if(_characterID>=0)
        return ConfigManager::playable_characters[_characterID].states[_currentState].width;
    else
        return -1;
}

int LVL_PlayerDef::height()
{
    if(_characterID>=0)
        return ConfigManager::playable_characters[_characterID].states[_currentState].height;
    else
        return -1;
}

int LVL_PlayerDef::heightDuck()
{
    if(_characterID>=0)
        return ConfigManager::playable_characters[_characterID].states[_currentState].duck_height;
    else
        return -1;
}

