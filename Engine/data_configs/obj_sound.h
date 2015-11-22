#ifndef OBJ_SOUND_H
#define OBJ_SOUND_H

#include <QString>

#include "obj_sound_roles.h"
struct Mix_Chunk;

struct obj_sound_index
{
    obj_sound_index();
    QString path;
    Mix_Chunk *chunk;
    int channel;
    void play(); //!< play sound
};


struct obj_sound
{
    obj_sound();
    unsigned long id;
    QString name;
    QString file;
    QString absPath;
    int channel;
    bool hidden;
};

#endif // OBJ_SOUND_H

