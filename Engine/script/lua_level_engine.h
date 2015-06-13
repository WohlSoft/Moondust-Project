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
    LVL_Npc* createLuaNpc();

    void loadNPCClass(int id, const QString& path);

    LevelScene* getScene();

protected:
    void onBindAll();
};

#endif // LUALEVELENGINE_H
