
#include "luaclass_level_playerstate.h"
#include <scenes/scene_level.h>

lua_LevelPlayerState::lua_LevelPlayerState()
{
    m_scene = NULL;
    m_playerID = 0;
    m_health = 1;
    m_characterID = 0;
    m_stateID = 0;
}

lua_LevelPlayerState::lua_LevelPlayerState(LevelScene *parent, int playerID)
{
    m_scene = parent;
    m_playerID = playerID;
    m_health = 0;
    m_characterID = 0;
    m_stateID = 0;
}

lua_LevelPlayerState::lua_LevelPlayerState(const lua_LevelPlayerState &ps)
{
    m_scene = ps.m_scene;
    m_playerID = ps.m_playerID;
    m_health = ps.m_health;
    m_characterID = ps.m_characterID;
    m_stateID = ps.m_stateID;
}

unsigned int lua_LevelPlayerState::currentHealth()
{
    return m_health;
}

unsigned int lua_LevelPlayerState::countPoints()
{
    return m_scene->getGameState()->game_state.points;
}

unsigned int lua_LevelPlayerState::countStars()
{
    return m_scene->getGameState()->game_state.totalStars;
}

unsigned int lua_LevelPlayerState::countCoins()
{
    return m_scene->getGameState()->game_state.coins;
}

unsigned int lua_LevelPlayerState::countLives()
{
    return m_scene->getGameState()->game_state.lives;
}

void lua_LevelPlayerState::appendPoints(unsigned int newPoints)
{
    m_scene->getGameState()->game_state.points += newPoints;
}

void lua_LevelPlayerState::appendCoins(unsigned int newCoins)
{
    m_scene->getGameState()->game_state.coins += newCoins;
}

void lua_LevelPlayerState::appendLives(unsigned int newLives)
{
    m_scene->getGameState()->game_state.lives += newLives;
}

void lua_LevelPlayerState::appendStars(unsigned int newStars)
{
    m_scene->getGameState()->game_state.totalStars += newStars;
}

void lua_LevelPlayerState::setPoints(unsigned int newPoints)
{
    m_scene->getGameState()->game_state.points = newPoints;
}

void lua_LevelPlayerState::setCoins(unsigned int newCoins)
{
    m_scene->getGameState()->game_state.coins = newCoins;
}

void lua_LevelPlayerState::setLives(unsigned int newLives)
{
    m_scene->getGameState()->game_state.lives = newLives;
}

void lua_LevelPlayerState::setStars(unsigned int newStars)
{
    m_scene->getGameState()->game_state.totalStars = newStars;
}

void lua_LevelPlayerState::setCharacterID(int id)
{
    m_characterID = id;
}

void lua_LevelPlayerState::setStateID(int id)
{
    m_stateID = id;
}

void lua_LevelPlayerState::setHealth(int health)
{
    m_health=health;
}

luabind::scope lua_LevelPlayerState::bindToLua()
{
    using namespace luabind;
    return class_<lua_LevelPlayerState>("LevelPlayerState")
            .def_readonly("points", &lua_LevelPlayerState::countPoints)
            .def_readonly("coins",  &lua_LevelPlayerState::countCoins)
            .def_readonly("lives",  &lua_LevelPlayerState::countLives)
            .def_readonly("stars",  &lua_LevelPlayerState::countStars)
            .def_readonly("leeks",  &lua_LevelPlayerState::countStars)
            .def_readonly("health", &lua_LevelPlayerState::m_health)
            .def_readonly("characterID", &lua_LevelPlayerState::m_characterID)
            .def_readonly("stateID", &lua_LevelPlayerState::m_stateID)
            .def("addPoints", &lua_LevelPlayerState::appendPoints)
            .def("addCoins",  &lua_LevelPlayerState::appendCoins)
            .def("addStars",  &lua_LevelPlayerState::appendStars)
            .def("addLeeks",  &lua_LevelPlayerState::appendStars)
            .def("addLives",  &lua_LevelPlayerState::appendLives)
            .def("setPoints", &lua_LevelPlayerState::setPoints)
            .def("setCoins",  &lua_LevelPlayerState::setCoins)
            .def("setStars",  &lua_LevelPlayerState::setStars)
            .def("setLeeks",  &lua_LevelPlayerState::setStars)
            .def("setLives",  &lua_LevelPlayerState::setLives)
            .def_readonly("playerID",  &lua_LevelPlayerState::m_playerID)
            ;
}
