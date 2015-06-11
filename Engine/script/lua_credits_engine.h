#ifndef LUACREDITSENGINE_H
#define LUACREDITSENGINE_H

#include "lua_engine.h"

class CreditsScene;

class LuaCreditsEngine : public LuaEngine
{
private:
    Q_DISABLE_COPY(LuaCreditsEngine)
public:
    LuaCreditsEngine(CreditsScene *scene);
    ~LuaCreditsEngine();

    CreditsScene* getScene();

protected:
    void onBindAll();
};

#endif // LUACREDITSENGINE_H
