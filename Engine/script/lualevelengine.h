#ifndef LUALEVELENGINE_H
#define LUALEVELENGINE_H

#include "luaengine.h"

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
private:
    LevelScene* m_scene;
};

#endif // LUALEVELENGINE_H
