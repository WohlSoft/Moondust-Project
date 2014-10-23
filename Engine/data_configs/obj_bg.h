#ifndef OBJ_BG_H
#define OBJ_BG_H

#include <QString>
#include "../graphics/graphics.h"

struct obj_BG{

    unsigned long id;
    QString name;

    QString image_n;

    /*   OpenGL    */
    bool isInit;
    PGE_Texture * image;
    GLuint textureID;
    long textureArrayId;
    long animator_ID;
    /*   OpenGL    */

    unsigned int type;//convert from string
    float repeat_h;
    unsigned int repead_v;
    unsigned int attached;
    bool editing_tiled;
    bool animated;
    unsigned int frames;
    unsigned int frame_h; //Hegth of the frame. Calculating automatically

    unsigned int display_frame;

    bool magic;
    unsigned int magic_strips;
    QString magic_splits;
    QString magic_speeds;

    QString second_image_n;
    //QPixmap second_image;

    /*   OpenGL    */
    bool second_isInit;
    PGE_Texture * second_image;
    GLuint second_textureID;
    long second_textureArrayId;
    long second_animator_ID;
    /*   OpenGL    */


    float second_repeat_h;
    unsigned int second_repeat_v;
    unsigned int second_attached;

};

#endif // OBJ_BG_H
