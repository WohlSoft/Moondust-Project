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
    float startX;
    float startY;
    int animationLoops;
    int delay;
    int framespeed;
    QList<int> frame_sequence;
    float velocityX;
    float velocityY;
    float gravity;
    int direction;

    float min_vel_x;
    float min_vel_y;
    float max_vel_x;
    float max_vel_y;
    float decelerate_x;
    float decelerate_y;
};

#endif // SPAWNEFFECTDEF_H
