#include "controllable_object.h"

ControllableObject::ControllableObject()
{
    keys = ResetControlKeys();
}

void ControllableObject::applyControls(controller_keys sentkeys)
{
    keys = sentkeys;
}

void ControllableObject::resetControls()
{
    keys = ResetControlKeys();
}
