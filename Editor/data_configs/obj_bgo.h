/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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
    obj_bgo();
    bool isValid;
    unsigned long animator_id;
    QPixmap * cur_image;
    unsigned int frame_h; //Hegth of the frame. Calculating automatically

    /*!
     * \brief Quickly copies all properties except images
     * \param Target bgo configuration body
     */
    void copyTo(obj_bgo &bgo);

    unsigned long id;
    QString name;
    QString group;
    QString category;
    unsigned int grid;

    enum ZLayerEnum {
        z_background_2 =-1,
        z_background_1 = 0,
        z_foreground_1 = 1,//Default
        z_foreground_2 = 2
    };
    int zLayer; //-1, 0, 1, 2
    int offsetX;
    int offsetY;
    double zOffset;

    QString image_n;
    QString mask_n;
    QPixmap image;

    bool climbing;
    bool animated;
    unsigned int frames;
    unsigned int framespeed;

    unsigned int display_frame;
};

#endif // OBJ_BGO_H
