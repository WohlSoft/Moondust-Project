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
#include <ConfigPackManager/world/config_wld_generic.h>

////////////////////World map items///////////////////////////
struct obj_wld_generic
{
    obj_wld_generic()
    {
        isValid     = false;
        animator_id = 0;
        cur_image   = nullptr;
        m_itemType  = 0;
    }

    bool isValid;
    unsigned long animator_id;
    QPixmap *cur_image;
    QPixmap image;
    int m_itemType;

    /*!
     * \brief Quickly copies all properties except images
     * \param Target bgo configuration body
     */
    void copyTo(obj_wld_generic &item)
    {
        /* for internal usage */
        item.isValid         = isValid;
        item.animator_id     = animator_id;
        item.cur_image       = cur_image;
        if(cur_image==nullptr)
            item.cur_image   = &image;

        item.setup = setup;
    }

    WldGenericSetup setup;
};

typedef obj_wld_generic obj_w_tile;
typedef obj_wld_generic obj_w_scenery;
typedef obj_wld_generic obj_w_path;
typedef obj_wld_generic obj_w_level;

//Markers
struct wld_levels_Markers
{
    unsigned long path;
    unsigned long bigpath;
};

#endif // OBJ_WLD_ITEMS_H
