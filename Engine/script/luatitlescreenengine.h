#ifndef LUATITLESCREENENGINE_H
#define LUATITLESCREENENGINE_H

#include "luaengine.h"

class TitleScene;



class LuaTitleScreenEngine : public LuaEngine
{
private:
    Q_DISABLE_COPY(LuaTitleScreenEngine)
public:
    LuaTitleScreenEngine(TitleScene* scene);
    ~LuaTitleScreenEngine();

    TitleScene* getScene();

protected:
    void onBindAll();


private:
    TitleScene* m_scene;
};

#endif // LUATITLESCREENENGINE_H
