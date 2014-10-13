#include "controller.h"
#include "controllable_object.h"

Controller::Controller()
{
    keys = noKeys();

}

controller_keys Controller::noKeys()
{
    return ResetControlKeys();
}
