#include "spawn_effect_def.h"
#include <QStringList>

#include <script/lua_engine.h>

SpawnEffectDef::SpawnEffectDef()
{
    id=0;
    start_delay=0;
    startX=0.0f;
    startY=0.0f;
    animationLoops=1;
    delay = 0;
    framespeed=64;
    velocityX=0.0;
    velocityY=0.0;
    zIndex = -5.0;
    gravity=0.0f;
    direction=1;

    min_vel_x=0.0;
    min_vel_y=0.0;
    max_vel_x=0.0;
    max_vel_y=0.0;
    decelerate_x=0.0;
    decelerate_y=0.0;
}

SpawnEffectDef::SpawnEffectDef(const SpawnEffectDef &c)
{
    id = c.id;
    start_delay = c.start_delay;
    startX = c.startX;
    startY = c.startY;
    animationLoops = c.animationLoops;

    delay = c.delay;

    framespeed = c.framespeed;
    frame_sequence = c.frame_sequence;
    velocityX = c.velocityX;
    velocityY = c.velocityY;
    zIndex    = c.zIndex;
    gravity = c.gravity;

    direction = c.direction;

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

    lua_function = setup->value(prefix+"-effect-function", "").toString();

    id = setup->value(prefix+"-effect-id", 0).toInt();
    start_delay = setup->value(prefix+"-effect-start-delay", 0).toUInt();
    startX = setup->value(prefix+"-effect-start-x", 0.0).toDouble();
    startY = setup->value(prefix+"-effect-start-y", 0.0).toDouble();
    animationLoops = setup->value(prefix+"-effect-animation-loops", 1).toInt();
    delay = setup->value(prefix+"-effect-delay", 0).toInt();
    framespeed = setup->value(prefix+"-effect-framespeed", 0).toInt();
    velocityX = setup->value(prefix+"-effect-velocity-x", 0.0).toDouble();
    velocityY = setup->value(prefix+"-effect-velocity-y", 0.0).toDouble();
    zIndex  = setup->value(prefix+"-effect-z-index", -5.0).toDouble();
    gravity = setup->value(prefix+"-effect-gravity", 0.0).toDouble();
    direction = setup->value(prefix+"-effect-direction", 1).toInt();
    min_vel_x = setup->value(prefix+"-effect-min-vel-x", 0.0).toDouble();
    min_vel_y = setup->value(prefix+"-effect-min-vel-y", 0.0).toDouble();
    max_vel_x = setup->value(prefix+"-effect-max-vel-x", 0.0).toDouble();
    max_vel_y = setup->value(prefix+"-effect-max-vel-y", 0.0).toDouble();
    decelerate_x = setup->value(prefix+"-effect-decelerate-x", 0.0).toDouble();
    decelerate_y = setup->value(prefix+"-effect-decelerate-y", 0.0).toDouble();

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

void SpawnEffectDef::lua_setSequence(luabind::adl::object frames)
{
    int ltype = luabind::type(frames);
    if(luabind::type(frames) != LUA_TTABLE){
        luaL_error(frames.interpreter(),
                   "setSequence exptected int-array, got %s",
                   lua_typename(frames.interpreter(), ltype));
        return;
    }
    frame_sequence = luabind_utils::convArrayTo<int>(frames);
}
