#ifndef LUALEVELENGINE_H
#define LUALEVELENGINE_H

#include "lua_engine.h"

class LevelScene;

class LuaLevelEngine : public LuaEngine
{
private:
    Q_DISABLE_COPY(LuaLevelEngine)
public:
    LuaLevelEngine(LevelScene* scene);
    ~LuaLevelEngine();

    LevelScene* getScene();

protected:
    void onBindAll();
};

#endif // LUALEVELENGINE_H
