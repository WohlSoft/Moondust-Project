/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2015 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
    PGEColor Color_upper;
    PGEColor Color_lower;
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
    QList<int > magic_splits_i;
    QString magic_speeds;
    QList<double > magic_speeds_i;

    QString second_image_n;
    //QPixmap second_image;

    /*   OpenGL    */
    bool second_isInit;
    PGE_Texture * second_image;
    GLuint second_textureID;
    long second_textureArrayId;
    long second_animator_ID;
    PGEColor second_Color_upper;
    PGEColor second_Color_lower;
    /*   OpenGL    */


    float second_repeat_h;
    unsigned int second_repeat_v;
    unsigned int second_attached;

};

#endif // OBJ_BG_H
