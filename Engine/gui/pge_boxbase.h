#ifndef PGE_BOXBASE_H
#define PGE_BOXBASE_H

#include "../scenes/scene.h"
#include <QString>

#include <SDL2/SDL_timer.h>

///
/// \brief The PGE_BoxBase class
///
/// This class provides the basis of message boxes, menus, questions, etc.

class PGE_BoxBase
{
public:
    PGE_BoxBase();
    PGE_BoxBase(Scene *_parentScene = 0);
    virtual ~PGE_BoxBase();

    virtual void exec();

    /**************Fader**************/
    float fader_opacity;
    float target_opacity;
    float fade_step;
    int fadeSpeed;
    void setFade(int speed, float target, float step);
    static unsigned int nextOpacity(unsigned int x, void *p);
    void fadeStep();
    SDL_TimerID fader_timer_id;
    /**************Fader**************/

protected:
    Scene * parentScene;
};

#endif // PGE_BOXBASE_H
