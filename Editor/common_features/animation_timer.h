#ifndef ANIMATIONTIMER_H
#define ANIMATIONTIMER_H

#pragma once
#include <QVector>
#include <QTimer>

class TimedAnimation
{
public:
    TimedAnimation();
    virtual ~TimedAnimation() {}
    TimedAnimation(const TimedAnimation&a);
    void processTimer(double ticks);
    double  ticks_left;
    double  frame_delay;
    virtual void resetFrame() {}
    virtual void nextFrame() {}
};

/*!
 * \brief Group animators processor
 */
class AnimationTimer: public QObject
{
    Q_OBJECT

public:
    explicit AnimationTimer(QObject *parent=0);
    ~AnimationTimer() {}
    /*!
     * \brief Adds specific animator into processing list
     * \param animator Pointer to animator object
     */
    void registerAnimation(TimedAnimation* animator);
    /*!
     * \brief Removes all animators from a list
     */
    void clear();
    /*!
     * \brief Starts animation processing
     * \param interval Frame delay (animations which are shorter this interval are will have a skipped frames)
     */
    void start(int interval=32);
    /*!
     * \brief Stops animation processing
     */
    void stop();

public slots:
    /*!
     * \brief Process current animation step
     */
    void processTime();
private:
    QVector<TimedAnimation*> registered_animators;
    QTimer timer;
};


#endif // ANIMATIONTIMER_H
