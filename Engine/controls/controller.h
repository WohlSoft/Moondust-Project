#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <vector>
#include "control_keys.h"
#include "controllable_object.h"

class Controller
{
public:
    Controller();
    enum commands
    {
        key_start=0,
        key_left,
        key_right,
        key_up,
        key_down,
        key_run,
        key_jump,
        key_altrun,
        key_altjump,
        key_drop
    };
    static controller_keys noKeys();

    void resetControls();
    void sendControls();

    controller_keys keys;
    std::vector<ControllableObject* > objects;
};

#endif // CONTROLLER_H
