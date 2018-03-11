#ifndef SPAWNEFFECTDEF_H
#define SPAWNEFFECTDEF_H

#include <IniProcessor/ini_processing.h>
#include <string>
#include <vector>

#include <luabind/luabind.hpp>
#include <lua_includes/lua.hpp>

class SpawnEffectDef
{
public:
    SpawnEffectDef();
    SpawnEffectDef(const SpawnEffectDef &c);
    void fill(const std::string &prefix, IniProcessing *setup);

    //Call lua-function instead of spawning this effect
    std::string lua_function;

    void lua_setSequence(luabind::object frames);

    unsigned int start_delay;

    unsigned long id;
    double startX;
    double startY;
    int animationLoops;
    int delay;
    int frameDelay;
    std::vector<int> frame_sequence;
    double velocityX;
    double velocityY;
    long double zIndex;
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
