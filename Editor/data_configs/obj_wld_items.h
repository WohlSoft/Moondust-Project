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

#ifndef OBJ_WLD_ITEMS_H
#define OBJ_WLD_ITEMS_H

#include <QString>
#include <QPixmap>

////////////////////World map items///////////////////////////
struct obj_w_tile{
    unsigned long id;
        QString image_n;
        QString mask_n;
        QPixmap image;
        QPixmap mask;
        unsigned long row;
        unsigned long col;

    unsigned int grid;
    QString group;
    QString category;
    bool animated;
    unsigned int frames;
    unsigned int framespeed; // Default 128 ms
    unsigned int frame_h; //Hegth of the frame. Calculating automatically
    unsigned int display_frame;
};

struct obj_w_scenery{
    unsigned long id;
        QString image_n;
        QString mask_n;
        QPixmap image;
        QPixmap mask;
    unsigned int grid;
    QString group;
    QString category;
    bool animated;
    unsigned int frames;
    unsigned int framespeed; // Default 128 ms
    unsigned int frame_h; //Hegth of the frame. Calculating automatically
    unsigned int display_frame;
};

struct obj_w_level{
    unsigned long id;
        QString image_n;
        QString mask_n;
        QPixmap image;
        QPixmap mask;
    unsigned int grid;
    QString group;
    QString category;
    bool animated;
    unsigned int frames;
    unsigned int framespeed; // Default 128 ms
    unsigned int frame_h; //Hegth of the frame. Calculating automatically
    unsigned int display_frame;
};

struct obj_w_path{
    unsigned long id;
        QString image_n;
        QString mask_n;
        QPixmap image;
        QPixmap mask;
        unsigned long row;
        unsigned long col;
    unsigned int grid;
    QString group;
    QString category;
    bool animated;
    unsigned int frames;
    unsigned int framespeed; // Default 128 ms
    unsigned int frame_h; //Hegth of the frame. Calculating automatically
    unsigned int display_frame;
};

//Markers
struct wld_levels_Markers
{
    unsigned long path;
    unsigned long bigpath;
};

#endif // OBJ_WLD_ITEMS_H
