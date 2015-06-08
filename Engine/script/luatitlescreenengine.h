#ifndef LUATITLESCREENENGINE_H
#define LUATITLESCREENENGINE_H

#include "luaengine.h"

class LuaTitleScreenEngine : public LuaEngine
{
private:
    Q_DISABLE_COPY(LuaTitleScreenEngine)
public:
    LuaTitleScreenEngine();
    ~LuaTitleScreenEngine();
};

#endif // LUATITLESCREENENGINE_H
