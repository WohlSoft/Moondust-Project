#include "gfx_effects.h"
#include "../scene.h"
#include <graphics/graphics.h>
#include <data_configs/config_manager.h>

GfxEffects::GfxEffects(Scene *parent)
{
    _s = parent;
}

GfxEffects::~GfxEffects()
{}

///
/// \brief GfxEffects::play Launching the static effect on the X and Y of the scene
/// \param effectID ID of effect
/// \param x  X position on the scene
/// \param y  Y position on the scene
/// \param loops  How much animation loops should be played (1 or 0 - once, -1 - forever)
/// \param time   Timeout of effect in milliseconds
///
void GfxEffects::play(long effectID, long x, long y, int loops, int time)
{
    Q_UNUSED(effectID);
    Q_UNUSED(x);
    Q_UNUSED(y);
    Q_UNUSED(loops);
    Q_UNUSED(time);
}

void GfxEffects::play(long effectID, long x, long y, std::string luaEvent, int loops, int time)
{
    Q_UNUSED(effectID);
    Q_UNUSED(x);
    Q_UNUSED(y);
    Q_UNUSED(luaEvent);
    Q_UNUSED(loops);
    Q_UNUSED(time);
}


void GfxEffects::update(int ticks)
{
    Q_UNUSED(ticks);

}

void GfxEffects::render(float camX, float camY)
{
    Q_UNUSED(camX);
    Q_UNUSED(camY);
}
