/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef OBJ_BGO_H
#define OBJ_BGO_H

#include <QString>
#include <QPixmap>

struct obj_bgo{
    //    [background-1]
    unsigned long id;
    //    name="Smallest bush"		;background name, default="background-%n"
    QString name;
    //    group="scenery"			;Background group, default="All about my pigeon"
    QString group;
    //    category="scenery"		;Background categoty, default="Scenery"
    QString category;
    //    grid=32				; 32 | 16 Default="32"
    unsigned int grid;
    //    view=background			; background2 | background | foreground | foreground2, default="background"
    int view; //-1, 0, 1, 2
    int offsetX;
    int offsetY;
    int zOffset;
    //    image="background-1.gif"	;Image file with level file ; the image mask will be have *m.gif name.
    QString image_n;
    QString mask_n;
    QPixmap image;
    QPixmap mask;
    //    climbing=0			; default = 0
    bool climbing;
    //    animated = 0			; default = 0 - no
    bool animated;
    //    frames = 1			; default = 1
    unsigned int frames;
    //    frame-speed=125			; default = 125 ms, etc. 8 frames per sec
    unsigned int framespeed;
    unsigned int frame_h; //Hegth of the frame. Calculating automatically

    unsigned int display_frame;
};

#endif // OBJ_BGO_H
