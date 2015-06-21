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

#ifndef OBJ_EFFECT_H
#define OBJ_EFFECT_H

#include "../graphics/graphics.h"

#include <QString>
#include <QPixmap>

struct obj_effect
{
    unsigned long id;
        QString image_n;
        QString mask_n;
        //QPixmap image;
        //QPixmap mask;

    /*   OpenGL    */
    bool isInit;
    PGE_Texture * image;
    GLuint textureID;
    long textureArrayId;
    /*   OpenGL    */

    QString name;

    unsigned int frames;
    int framespeed;
    int framestyle;
    int frame_h; //!< Captured frame height
};

#endif // OBJ_EFFECT_H

