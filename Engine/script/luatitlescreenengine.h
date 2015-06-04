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
protected:
    void onReportError(const QString& errMsg);
};

#endif // LUATITLESCREENENGINE_H
