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
#ifndef OBJ_BG_H
#define OBJ_BG_H

#include <QString>
#include <QPixmap>
#include <stdint.h>

struct obj_BG
{
    bool isValid = false;
    uint64_t  animator_id = 0;
    QPixmap * cur_image = nullptr;
    QPixmap * cur_image_second = nullptr;

    /*!
     * \brief Quickly copies all properties except images
     * \param Target bgo configuration body
     */
    void copyTo(obj_BG &bg);

    uint64_t id = 0;
    QString name;

    QString image_n;

    QPixmap image;
    uint32_t type = 0;//convert from string
    double  repeat_h = 0.0;
    uint32_t repead_v = 0;
    uint32_t attached = 0;
    bool editing_tiled = false;
    bool animated = false;
    uint32_t frames = 1;
    uint32_t framespeed = 128;
    uint32_t frame_h = 0; //Hegth of the frame. Calculating automatically

    uint32_t display_frame = 0;

    bool magic = false;
    uint32_t magic_strips = 1;
    QString magic_splits;
    QString magic_speeds;

    QString second_image_n;
    QPixmap second_image;

    double second_repeat_h = 0.0;
    uint32_t second_repeat_v = 0;
    uint32_t second_attached = 0;
};

#endif // OBJ_BG_H

