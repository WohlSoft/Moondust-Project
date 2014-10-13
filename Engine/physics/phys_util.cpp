#include "phys_util.h"


float PhysUtil::pixMeter = 10.0f; // Pixels per meter;


float PhysUtil::met2pix(float met)
{
    return met * pixMeter;
}

float PhysUtil::pix2met(float pix)
{
    return pix / pixMeter;
}
