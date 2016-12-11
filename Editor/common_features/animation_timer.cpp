#include "animation_timer.h"
#include <cmath>

AnimationTimer::AnimationTimer(QObject *parent) : QObject(parent)
{
    timer.setTimerType(Qt::PreciseTimer);
    connect(&timer, SIGNAL(timeout()), this, SLOT(processTime()));
}

void AnimationTimer::registerAnimation(TimedAnimation *animator)
{
    if(animator->frame_delay <= 0)
        return;

    registered_animators.push_back(animator);
}

void AnimationTimer::clear()
{
    registered_animators.clear();
}

void AnimationTimer::start(int interval)
{
    timer.setInterval(interval);
    timer.start();
}

void AnimationTimer::stop()
{
    timer.stop();
    TimedAnimation **data = registered_animators.data();
    int count            = registered_animators.size();
    for(int i=0; i<count; i++)
        (*(data++))->resetFrame();
}

void AnimationTimer::processTime()
{
    TimedAnimation **data = registered_animators.data();
    int count            = registered_animators.size();
    double interval = static_cast<double>(timer.interval());

    for(int i=0; i<count; i++)
        (*(data++))->processTimer(interval);
}




TimedAnimation::TimedAnimation() : ticks_left(0.0), frame_delay(0.0) {}

TimedAnimation::TimedAnimation(const TimedAnimation &a) :
    ticks_left(a.ticks_left), frame_delay(a.frame_delay) {}

void TimedAnimation::processTimer(double ticks)
{
    if(frame_delay < 1)
        return; //Idling animation

    ticks_left -= std::fabs(ticks);

    while(ticks_left <= 0.0)
    {
        nextFrame();
        ticks_left += frame_delay;
    }
}
