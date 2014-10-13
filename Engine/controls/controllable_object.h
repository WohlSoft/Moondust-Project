#ifndef CONTROLLABLE_OBJECT_H
#define CONTROLLABLE_OBJECT_H

#include <vector>
#include "control_keys.h"

class ControllableObject
{
public:
    ControllableObject();
    controller_keys keys;
    void applyControls(controller_keys sentkeys);
    void resetControls();

};

#endif // CONTROLLABLE_OBJECT_H
