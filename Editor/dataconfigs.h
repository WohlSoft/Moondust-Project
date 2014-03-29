#ifndef DATACONFIGS_H
#define DATACONFIGS_H
#include <QVector>
#include <QPixmap>
#include <QBitmap>

struct obj_bgo{
    /*
    [background-1]
    name="Smallest bush"		;background name, default="background-%n"
    type="scenery"			;Background type, default="Scenery"
    grid=32				; 32 | 16 Default="32"
    view=background			; background | foreground, default="background"
    image="background-1.gif"	;Image file with level file ; the image mask will be have *m.gif name.
    climbing=0			; default = 0
    animated = 0			; default = 0 - no
    frames = 1			; default = 1
    frame-speed=125			; default = 125 ms, etc. 8 frames per sec
    */
    unsigned long id;
    QString name;
    QString type;
    unsigned int grid;
    unsigned int view;
    QBitmap mask;
    QPixmap image;
    bool climbing;
    bool animated;
    unsigned int frames;
    unsigned int framespeed;
};

class dataconfigs
{
public:
    dataconfigs();
    void loadconfigs();
    QVector<obj_bgo > main_bgo;
};

#endif // DATACONFIGS_H
