#include "pge_boxbase.h"

#undef main
#include <SDL2/SDL_opengl.h>
#undef main


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
    else
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
    }
}



/**************************Fader*******************************/
void PGE_BoxBase::setFade(int speed, float target, float step)
{
    fade_step = fabs(step);
    target_opacity = target;
    fadeSpeed = speed;

    fader_timer_id = SDL_AddTimer(speed, &PGE_BoxBase::nextOpacity, this);
    if(!fader_timer_id) fader_opacity = target_opacity;
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

    if(fader_opacity>=1.0f || fader_opacity<=0.0f)
        SDL_RemoveTimer(fader_timer_id);
    else
        {
            fader_timer_id = SDL_AddTimer(fadeSpeed, &PGE_BoxBase::nextOpacity, this);
            if(!fader_timer_id) fader_opacity = target_opacity;
        }

    if(fader_opacity>1.0f) fader_opacity = 1.0f;
    else
    if(fader_opacity<0.0f) fader_opacity = 0.0f;
}
/**************************Fader**end**************************/
