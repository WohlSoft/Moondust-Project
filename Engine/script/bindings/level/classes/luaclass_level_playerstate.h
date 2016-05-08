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
    lua_LevelPlayerState(const lua_LevelPlayerState&ps);
    unsigned int currentHealth();
    unsigned int countPoints();
    unsigned int countStars();
    unsigned int countCoins();
    unsigned int countLives();
    void appendPoints(unsigned int newPoints);
    void appendCoins(unsigned int newCoins);
    void appendLives(unsigned int newLives);
    void appendStars(unsigned int newStars);
    void setPoints(unsigned int newPoints);
    void setCoins(unsigned int newCoins);
    void setLives(unsigned int newLives);
    void setStars(unsigned int newStars);
    void setCharacterID(int id);
    void setStateID(int id);
    void setHealth(int health);
    static luabind::scope bindToLua();
private:
    int m_playerID;
    LevelScene *m_scene;
    int m_health;
    int m_characterID;
    int m_stateID;
};

#endif // LUA_LEVELPLAYERSTATE_H
