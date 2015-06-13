#include "lvl_physenv.h"

LVL_PhysEnv::LVL_PhysEnv() : PGE_Phys_Object()
{
    type = LVLPhysEnv;
    env_type = Env_Water;
}

LVL_PhysEnv::~LVL_PhysEnv()
{}

void LVL_PhysEnv::init()
{
    setSize(data.w, data.h);
    setPos(data.x, data.y);
    env_type = data.quicksand ? Env_Quicksand : Env_Water;
    collide = COLLISION_NONE;
}

