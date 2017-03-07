#ifndef OBJ_SOUND_H
#define OBJ_SOUND_H

#include <string>

#include "obj_sound_roles.h"
struct Mix_Chunk;

struct obj_sound_index
{
    std::string path;
    bool need_reload = false;
    Mix_Chunk *chunk = nullptr;
    int channel = -1;
    void setPath(std::string _path);
    void play(); //!< play sound
};


struct obj_sound
{
    unsigned long id = 0;
    std::string name;
    std::string file;
    std::string absPath;
    int channel = -1;
    bool hidden = false;
};

#endif // OBJ_SOUND_H

