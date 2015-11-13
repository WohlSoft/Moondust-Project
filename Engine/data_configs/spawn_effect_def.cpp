#include "spawn_effect_def.h"
#include <QStringList>

SpawnEffectDef::SpawnEffectDef()
{
    id=0;
    startX=0.0f;
    startY=0.0f;
    animationLoops=1;
    delay=0;
    framespeed=64;
    velocityX=0.0f;
    velocityY=0.0f;
    gravity=0.0f;
    direction=1;

    min_vel_x=0.0f;
    min_vel_y=0.0f;
    max_vel_x=0.0f;
    max_vel_y=0.0f;
    decelerate_x=0.0f;
    decelerate_y=0.0f;
}

SpawnEffectDef::SpawnEffectDef(const SpawnEffectDef &c)
{
    id=c.id;
    startX=c.startX;
    startY=c.startY;
    animationLoops=c.animationLoops;
    delay=c.delay;
    framespeed=c.framespeed;
    velocityX=c.velocityX;
    velocityY=c.velocityY;
    gravity=c.gravity;
    direction=c.direction;

    min_vel_x=c.min_vel_x;
    min_vel_y=c.min_vel_y;
    max_vel_x=c.max_vel_x;
    max_vel_y=c.max_vel_y;
    decelerate_x=c.decelerate_x;
    decelerate_y=c.decelerate_y;
}

void SpawnEffectDef::fill(QString prefix, QSettings *setup)
{
    if(!setup) return;

    id = setup->value(prefix+"-effect-id", 0).toInt();
    startX = setup->value(prefix+"-effect-start-x", 0.0f).toFloat();
    startY = setup->value(prefix+"-effect-start-y", 0.0f).toFloat();
    animationLoops = setup->value(prefix+"-effect-animation-loops", 1).toInt();
    delay = setup->value(prefix+"-effect-delay", 0).toInt();
    framespeed = setup->value(prefix+"-effect-framespeed", 0).toInt();
    velocityX = setup->value(prefix+"-effect-velocity-x", 0.0f).toFloat();
    velocityY = setup->value(prefix+"-effect-velocity-y", 0.0f).toFloat();
    gravity = setup->value(prefix+"-effect-gravity", 0.0f).toFloat();
    direction = setup->value(prefix+"-effect-direction", 1).toInt();
    min_vel_x = setup->value(prefix+"-effect-min-vel-x", 0.0f).toFloat();
    min_vel_y = setup->value(prefix+"-effect-min-vel-y", 0.0f).toFloat();
    max_vel_x = setup->value(prefix+"-effect-max-vel-x", 0.0f).toFloat();
    max_vel_y = setup->value(prefix+"-effect-max-vel-y", 0.0f).toFloat();
    max_vel_x = setup->value(prefix+"-effect-max-vel-x", 0.0f).toFloat();
    decelerate_x = setup->value(prefix+"-effect-decelerate-x", 0.0f).toFloat();
    decelerate_y = setup->value(prefix+"-effect-decelerate-y", 0.0f).toFloat();

    frame_sequence.clear();
    QString frame_sequence_str=setup->value(prefix+"-effect-frame-sequence", "").toString();
    if(!frame_sequence_str.isEmpty())
    {
        bool ok;
        QStringList fr=frame_sequence_str.remove(" ").split(",", QString::SkipEmptyParts);
        foreach(QString f, fr)
        {
            frame_sequence.push_back(f.toInt(&ok));
            if(!ok) frame_sequence.pop_back();
        }
    }
}

