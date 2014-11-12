#include "pge_boxbase.h"

PGE_BoxBase::PGE_BoxBase()
{
    parentScene = 0;

    fader_opacity = 0.0f;

    fade_step = 0.02f;
    target_opacity=0.0f;
    fadeSpeed=10;
}

PGE_BoxBase::PGE_BoxBase(Scene *_parentScene)
{
    parentScene = _parentScene;
}

PGE_BoxBase::~PGE_BoxBase()
{

}

void PGE_BoxBase::exec()
{
    if(parentScene)
        parentScene->render();
}



/**************************Fader*******************************/
void PGE_BoxBase::setFade(int speed, float target, float step)
{
    fade_step = fabs(step);
    target_opacity = target;
    fadeSpeed = speed;
    fader_timer_id = SDL_AddTimer(speed, &PGE_BoxBase::nextOpacity, this);
}

unsigned int PGE_BoxBase::nextOpacity(unsigned int x, void *p)
{
    Q_UNUSED(x);
    PGE_BoxBase *self = reinterpret_cast<PGE_BoxBase *>(p);
    self->fadeStep();
    return 0;
}

void PGE_BoxBase::fadeStep()
{
    if(fader_opacity < target_opacity)
        fader_opacity+=fade_step;
    else
        fader_opacity-=fade_step;

    if(fader_opacity>=1.0 || fader_opacity<=0.0)
        SDL_RemoveTimer(fader_timer_id);
    else
        fader_timer_id = SDL_AddTimer(fadeSpeed, &PGE_BoxBase::nextOpacity, this);
}
/**************************Fader**end**************************/
