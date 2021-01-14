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
#ifndef OBJ_BGO_H
#define OBJ_BGO_H

#include <QPixmap>
#include <ConfigPackManager/level/config_bgo.h>

struct obj_bgo
{
    bool isValid = false;
    unsigned long animator_id = 0;
    QPixmap  *cur_image = nullptr;
    QPixmap  *cur_icon = nullptr;
    QPixmap image;
    QPixmap icon;

    /*!
     * \brief Quickly copies all properties except images
     * \param Target bgo configuration body
     */
    void copyTo(obj_bgo &bgo);

    BgoSetup setup;
};

#endif // OBJ_BGO_H
