/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef LUA_LEVELPLAYERSTATE_H
#define LUA_LEVELPLAYERSTATE_H

namespace luabind
{
    struct scope;
}

class LevelScene;
class lua_LevelPlayerState
{
    public:
        lua_LevelPlayerState();
        lua_LevelPlayerState(LevelScene *parent, int playerID);
        lua_LevelPlayerState(const lua_LevelPlayerState &ps);
        int currentHealth();
        unsigned int countPoints();
        unsigned int countStars();
        unsigned int countCoins();
        int countLives();
        void appendPoints(unsigned int newPoints);
        void appendCoins(unsigned int newCoins);
        void appendLives(int newLives);
        void appendStars(unsigned int newStars);
        void setPoints(unsigned int newPoints);
        void setCoins(unsigned int newCoins);
        void setLives(int newLives);
        void setStars(unsigned int newStars);
        void setCharacterID(unsigned long id);
        void setStateID(unsigned long id);
        void setHealth(int health);
        static luabind::scope bindToLua();
    private:
        int m_playerID;
        LevelScene *m_scene;
        int m_health;
        unsigned long m_characterID;
        unsigned long m_stateID;
};

#endif // LUA_LEVELPLAYERSTATE_H
