#include "controller_keyboard.h"

KeyboardController::KeyboardController()
{}

void KeyboardController::update(SDL_Event &event)
{
    switch(event.type)
    {
        case SDL_KEYDOWN: // If pressed key
          switch(event.key.keysym.sym)
          {
            case SDLK_z:
                keys.jump=true;
            break;

            case SDLK_x:
                keys.run=true;
            break;

            case SDLK_RIGHT:
                keys.right=true;
            break;

            case SDLK_LEFT:
                keys.left=true;
            break;
          }
        break;

        case SDL_KEYUP:
            switch(event.key.keysym.sym)
            {
                case SDLK_z:
                    keys.jump=false;
                break;

                case SDLK_x:
                    keys.run=false;
                break;

                case SDLK_RIGHT:
                    keys.right=false;
                    break;
                case SDLK_LEFT:
                    keys.left=false;
                    break;
            }
        break;
    }
}
