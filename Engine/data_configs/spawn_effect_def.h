#ifndef SPAWNEFFECTDEF_H
#define SPAWNEFFECTDEF_H

#include <QString>
#include <QSettings>
#include <QList>

#include <luabind/luabind.hpp>
#include <lua_inclues/lua.hpp>

class SpawnEffectDef
{
public:
    SpawnEffectDef();
    SpawnEffectDef(const SpawnEffectDef &c);
    void fill(QString prefix, QSettings *setup);

    //Call lua-function instead of spawning this effect
    QString lua_function;

    void lua_setSequence(luabind::object frames);

    unsigned int start_delay;

    long id;
    double startX;
    double startY;
    int animationLoops;
    int delay;
    int framespeed;
    QList<int> frame_sequence;
    double velocityX;
    double velocityY;
    double zIndex;
    double gravity;
    int direction;

    double min_vel_x;
    double min_vel_y;
    double max_vel_x;
    double max_vel_y;
    double decelerate_x;
    double decelerate_y;
};

#endif // SPAWNEFFECTDEF_H
