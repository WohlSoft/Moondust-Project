#ifndef GFXEFFECTS_H
#define GFXEFFECTS_H

#include <QString>
class Scene;

class GfxEffects
{
public:
    GfxEffects(Scene *parent);
    ~GfxEffects();
    enum Loops
    {
        LOOP_ONCE=0,
        LOOP_FOREVER=-1
    };

    void play(long effectID, long x, long y, int loops=LOOP_ONCE, int time=-1 );
    void play(long effectID, long x, long y, std::string luaEvent, int loops=LOOP_ONCE, int time=-1 );

    void update(int ticks);
    void render(float camX, float camY);

private:
    Scene *_s;
};

#endif // GFXEFFECTS_H
