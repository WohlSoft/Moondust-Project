/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

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

int lua_LevelPlayerState::currentHealth()
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

int lua_LevelPlayerState::countLives()
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

void lua_LevelPlayerState::appendLives(int newLives)
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

void lua_LevelPlayerState::setLives(int newLives)
{
    m_scene->getGameState()->game_state.lives = newLives;
}

void lua_LevelPlayerState::setStars(unsigned int newStars)
{
    m_scene->getGameState()->game_state.totalStars = newStars;
}

void lua_LevelPlayerState::setCharacterID(unsigned long id)
{
    m_characterID = id;
}

void lua_LevelPlayerState::setStateID(unsigned long id)
{
    m_stateID = id;
}

void lua_LevelPlayerState::setHealth(int health)
{
    m_health = health;
}

/***
Level specific functions and classes
@module LevelCommon
*/

luabind::scope lua_LevelPlayerState::bindToLua()
{
    using namespace luabind;
    /***
    Playable character state information (Level only)
    @type LevelPlayerState
    */
    return class_<lua_LevelPlayerState>("LevelPlayerState")
            /***
            Points counter (Read Only)
            @tfield uint points
            */
           .def_readonly("points", &lua_LevelPlayerState::countPoints)
            /***
            Coins counter (Read Only)
            @tfield uint coins
            */
           .def_readonly("coins",  &lua_LevelPlayerState::countCoins)
            /***
            Lives/Attempts counter (Read Only)
            @tfield int lives
            */
           .def_readonly("lives",  &lua_LevelPlayerState::countLives)
            /***
            Lives/Attempts counter, alias to @{lives} (Read Only)
            @tfield int attempts
            */
           .def_readonly("attempts",  &lua_LevelPlayerState::countLives)
            /***
            Count of stars/leeks (Read Only)
            @tfield uint leeks
            */
           .def_readonly("leeks",  &lua_LevelPlayerState::countStars)
            /***
            Count of stars/leeks, alias to @{leeks} (Read Only)
            @tfield uint stars
            */
           .def_readonly("stars",  &lua_LevelPlayerState::countStars)

            /***
            Health level of player (Read Only)
            @tfield int health
            */
           .def_readonly("health", &lua_LevelPlayerState::m_health)
            /***
            ID of playable character (Read Only)
            @tfield ulong characterID
            */
           .def_readonly("characterID", &lua_LevelPlayerState::m_characterID)
            /***
            ID of playable character's state/suit/form/shape (Read Only)
            @tfield ulong stateID
            */
           .def_readonly("stateID", &lua_LevelPlayerState::m_stateID)
            /***
            Add points to player
            @function addPoints
            @tparam uint points Count of points to append
            */
           .def("addPoints", &lua_LevelPlayerState::appendPoints)
            /***
            Add cois to player
            @function addCoins
            @tparam uint cois Count of cois to append
            */
           .def("addCoins",  &lua_LevelPlayerState::appendCoins)
            /***
            Add leeks/stars to player
            @function addLeeks
            @tparam uint leeks Count of leeks to append
            */
           .def("addLeeks",  &lua_LevelPlayerState::appendStars)
            /***
            Add leeks/stars to player, alias to @{addLeeks}
            @function addStars
            @tparam uint stars Count of Stars to append
            */
           .def("addStars",  &lua_LevelPlayerState::appendStars)
            /***
            Add or removes lives of player
            @function addLives
            @tparam int lives Count of lives to add (positive) or remove (negative)
            */
           .def("addLives",  &lua_LevelPlayerState::appendLives)
            /***
            Add or removes attempts of player, alias to @{addLives}
            @function addAttempts
            @tparam int attempts Count of attempts to add (positive) or remove (negative)
            */
           .def("addAttempts",  &lua_LevelPlayerState::appendLives)
            /***
            Set absolute value of points
            @function setPoints
            @tparam uint points Count of points to set
            */
           .def("setPoints", &lua_LevelPlayerState::setPoints)
            /***
            Set absolute value of coins
            @function setCoins
            @tparam uint points Count of coins to set
            */
           .def("setCoins",  &lua_LevelPlayerState::setCoins)
            /***
            Set absolute value of leeks
            @function setLeeks
            @tparam uint points Count of leeks to set
            */
           .def("setLeeks",  &lua_LevelPlayerState::setStars)
            /***
            Set absolute value of stars, alias to @{setLeeks}
            @function setStars
            @tparam uint points Count of stars to set
            */
           .def("setStars",  &lua_LevelPlayerState::setStars)
            /***
            Set absolute value of lives
            @function setLives
            @tparam uint points Count of lives to set
            */
           .def("setLives",  &lua_LevelPlayerState::setLives)
            /***
            Set absolute value of attempts, alias to @{setLives}
            @function setAttempts
            @tparam uint points Count of attempts to set
            */
           .def("setAttempts",  &lua_LevelPlayerState::setLives)
            /***
            ID of player (Read Only)
            @tfield int playerID
            */
           .def_readonly("playerID",  &lua_LevelPlayerState::m_playerID)
           ;
}
