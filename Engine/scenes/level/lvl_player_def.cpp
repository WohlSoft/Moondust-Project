#include "lvl_player_def.h"


LVL_PlayerDef::LVL_PlayerDef()
{
    _characterID=-1;
    _currentState=0;
    _playerID=1;
}

LVL_PlayerDef::~LVL_PlayerDef()
{}

LVL_PlayerDef::LVL_PlayerDef(const LVL_PlayerDef &d)
{
     _playerID=d._playerID;
     _characterID=d._characterID;
    _currentState=d._currentState;
}

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
            _currentState=1;
    }
    else
        _currentState=1;
}

void LVL_PlayerDef::setPlayerID(int id)
{
    _playerID=id;
}

int LVL_PlayerDef::playerID()
{
    return _playerID;
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

