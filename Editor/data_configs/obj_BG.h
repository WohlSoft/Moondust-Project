/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <ConfigPackManager/level/config_bg.h>

struct obj_BG
{
    bool isValid = false;
    uint64_t  animator_id = 0;
    QPixmap * cur_image = nullptr;
    QPixmap * cur_image_second = nullptr;
    QPixmap image;
    QPixmap second_image;
    QColor  fill_color;

    /*!
     * \brief Quickly copies all properties except images
     * \param Target bgo configuration body
     */
    void copyTo(obj_BG &bg);

    BgSetup setup;
};

#endif // OBJ_BG_H

