#ifndef OBJ_MUSIC_H
#define OBJ_MUSIC_H

#include <QString>

struct obj_music
{
    obj_music();
    unsigned long id;
    QString name;
    QString file;
    QString absPath;
};

#endif // OBJ_MUSIC_H
