#ifndef OBJ_SOUND_H
#define OBJ_SOUND_H

#include <QString>

#include "obj_sound_roles.h"
struct Mix_Chunk;

struct obj_sound_index
{
    obj_sound_index();
    std::string path;
    bool need_reload;
    Mix_Chunk *chunk;
    int channel;
    void setPath(std::string _path);
    void play(); //!< play sound
};


struct obj_sound
{
    obj_sound();
    unsigned long id;
    std::string name;
    std::string file;
    std::string absPath;
    int channel;
    bool hidden;
};

#endif // OBJ_SOUND_H

