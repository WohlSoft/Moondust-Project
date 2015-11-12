#ifndef SPAWNEFFECTDEF_H
#define SPAWNEFFECTDEF_H

class SpawnEffectDef
{
public:
    SpawnEffectDef();
    long id;
    float startX;
    float startY;
    int animationLoops;
    int delay;
    float velocityX;
    float velocityY;
    float gravity;
    int direction;

    float mix_vel_x;
    float mix_vel_y;
    float max_vel_x;
    float max_vel_y;
    float decelerate_x;
    float decelerate_y;
};

#endif // SPAWNEFFECTDEF_H
