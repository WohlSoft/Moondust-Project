#ifndef CONTROLLER_KEYBOARD_H
#define CONTROLLER_KEYBOARD_H

#include "controller.h"
#include <SDL2/SDL.h>

class KeyboardController : public Controller
{
public:
    KeyboardController();
    void update(SDL_Event &event);

};

#endif // CONTROLLER_KEYBOARD_H
