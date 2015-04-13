#ifndef OBJ_SOUND_H
#define OBJ_SOUND_H

#include <QString>

#include "obj_sound_roles.h"

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

