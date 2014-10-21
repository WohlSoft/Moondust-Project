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

void Controller::sendControls()
{
    foreach(ControllableObject* x, objects)
        x->applyControls(keys);

}

void Controller::update() {}

void Controller::registerInControl(ControllableObject *obj)
{
    if(!objects.contains(obj))
        objects.push_back(obj);
}

void Controller::removeFromControl(ControllableObject *obj)
{
    if(objects.contains(obj))
        objects.remove(objects.indexOf(obj));
}
