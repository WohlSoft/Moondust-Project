/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef OBJ_PLAYER_H
#define OBJ_PLAYER_H

#include "../graphics/graphics.h"

#include <QString>
#include <QPixmap>
#include <QMap>

/****************Definition of playable character state*******************/
struct obj_player_state
{
    inline void make();//!< Dummy function
    unsigned int width;
    unsigned int height;
    unsigned int height_duck;
            bool allow_duck;

    QString event_script;//!< LUA-Script with events
};

/******************Definition of playable character*********************/
struct obj_player
{
    inline void make();//!< Dummy function
    unsigned long id;
        QString image_n;
        QString mask_n;
        //QPixmap image;
        //QPixmap mask;

    /*   OpenGL    */
    bool isInit;
    PGE_Texture *image;
    GLuint textureID;
    long textureArrayId;
    long animator_ID;
    /*   OpenGL    */

    QString name;
    QMap<int, obj_player_state > states;

    bool allowFloating;
};

#endif // OBJ_PLAYER_H

