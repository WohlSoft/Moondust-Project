#include "files.h"
#include <SDL2/SDL_rwops.h>

bool Files::fileExists(const std::string &path)
{
    SDL_RWops *ops = SDL_RWFromFile(path.c_str(), "rb");
    if(ops)
    {
        SDL_RWclose(ops);
        return true;
    }
    return false;
}
