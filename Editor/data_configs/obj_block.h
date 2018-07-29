/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
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
#ifndef OBJ_BLOCK_H
#define OBJ_BLOCK_H

#include <QString>
#include <QPixmap>
#include <QList>
#include <ConfigPackManager/level/config_block.h>

struct obj_block
{
    obj_block();
    bool isValid;
    unsigned long animator_id;
    QPixmap *cur_image;
    QPixmap image;

    /*!
     * \brief Quickly copies all properties except images
     * \param Target bgo configuration body
     */
    void copyTo(obj_block &block);

    BlockSetup setup;
};


#endif // OBJ_BLOCK_H
