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

#pragma once
#ifndef OBJ_WLD_ITEMS_H
#define OBJ_WLD_ITEMS_H

#include <QString>
#include <QPixmap>

////////////////////World map items///////////////////////////
struct obj_w_tile
{
    obj_w_tile();
    bool isValid;
    unsigned long animator_id;
    QPixmap * cur_image;
    unsigned int frame_h; //Hegth of the frame. Calculating automatically

    /*!
     * \brief Quickly copies all properties except images
     * \param Target bgo configuration body
     */
    void copyTo(obj_w_tile &tile);

    unsigned long id;
        QString image_n;
        QString mask_n;
        QPixmap image;
        unsigned long row;
        unsigned long col;
    unsigned int grid;
    QString group;
    QString category;
    bool animated;
    unsigned int frames;
    unsigned int framespeed; // Default 128 ms
    unsigned int display_frame;
};

struct obj_w_scenery
{
    obj_w_scenery();
    bool isValid;
    unsigned long animator_id;
    QPixmap * cur_image;
    unsigned int frame_h; //Hegth of the frame. Calculating automatically

    /*!
     * \brief Quickly copies all properties except images
     * \param Target bgo configuration body
     */
    void copyTo(obj_w_scenery &scenery);

    unsigned long id;
        QString image_n;
        QString mask_n;
        QPixmap image;
    unsigned int grid;
    QString group;
    QString category;
    bool animated;
    unsigned int frames;
    unsigned int framespeed; // Default 128 ms
    unsigned int display_frame;
};

struct obj_w_level
{
    obj_w_level();
    bool isValid;
    unsigned long animator_id;
    QPixmap * cur_image;
    unsigned int frame_h; //Hegth of the frame. Calculating automatically

    /*!
     * \brief Quickly copies all properties except images
     * \param Target bgo configuration body
     */
    void copyTo(obj_w_level &level);

    unsigned long id;
        QString image_n;
        QString mask_n;
        QPixmap image;
    unsigned int grid;
    QString group;
    QString category;
    bool animated;
    unsigned int frames;
    unsigned int framespeed; // Default 128 ms
    unsigned int display_frame;
};

struct obj_w_path
{
    obj_w_path();
    bool isValid;
    unsigned long animator_id;
    QPixmap * cur_image;
    unsigned int frame_h; //Hegth of the frame. Calculating automatically

    /*!
     * \brief Quickly copies all properties except images
     * \param Target bgo configuration body
     */
    void copyTo(obj_w_path &wpath);

    unsigned long id;
        QString image_n;
        QString mask_n;
        QPixmap image;
        unsigned long row;
        unsigned long col;
    unsigned int grid;
    QString group;
    QString category;
    bool animated;
    unsigned int frames;
    unsigned int framespeed; // Default 128 ms
    unsigned int display_frame;
};

//Markers
struct wld_levels_Markers
{
    unsigned long path;
    unsigned long bigpath;
};

#endif // OBJ_WLD_ITEMS_H
