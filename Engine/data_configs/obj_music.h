#ifndef OBJ_MUSIC_H
#define OBJ_MUSIC_H

#include <string>

struct obj_music
{
    obj_music();
    unsigned long id;
    std::string name;
    std::string file;
    std::string absPath;
};

#endif // OBJ_MUSIC_H
