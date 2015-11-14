#ifndef LUALEVELENGINE_H
#define LUALEVELENGINE_H

#include "lua_engine.h"

class LevelScene;
class LVL_Player;
class LVL_Npc;

class LuaLevelEngine : public LuaEngine
{
private:
    Q_DISABLE_COPY(LuaLevelEngine)
public:
    LuaLevelEngine(LevelScene* scene);
    ~LuaLevelEngine();

    LVL_Player* createLuaPlayer();
    LVL_Npc* createLuaNpc(unsigned int id);
    void destoryLuaNpc(LVL_Npc* npc);
    void destoryLuaPlayer(LVL_Player *plr);

    void loadNPCClass(int id, const QString& path);

    LevelScene* getScene();

    QString getNpcBaseClassPath() const;
    void setNpcBaseClassPath(const QString &npcBaseClassPath);

    QString getPlayerBaseClassPath() const;
    void setPlayerBaseClassPath(const QString &playerBaseClassPath);

protected:
    QString m_npcBaseClassPath;
    QString m_playerBaseClassPath;

    void onBindAll();
};

#endif // LUALEVELENGINE_H
