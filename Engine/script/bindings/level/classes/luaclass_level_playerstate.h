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
