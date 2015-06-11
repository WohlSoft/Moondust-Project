#ifndef LUAWORLDENGINE_H
#define LUAWORLDENGINE_H

#include "lua_engine.h"

class WorldScene;

class LuaWorldEngine : public LuaEngine
{
private:
    Q_DISABLE_COPY(LuaWorldEngine)
public:
    LuaWorldEngine(WorldScene* scene);
    ~LuaWorldEngine();

    WorldScene* getScene();

protected:
    void onBindAll();

};

#endif // LUAWORLDENGINE_H
