/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef CONFIG_WLD_GENERIC_H
#define CONFIG_WLD_GENERIC_H

#include <QString>
#include <QList>

class QSettings;

struct WldGenericSetup
{
    bool parse(QSettings* setup,
               QString imgPath,
               unsigned int defaultGrid,
               WldGenericSetup* merge_with = nullptr,
               QString *error = nullptr);

    unsigned long id = 0;
        QString image_n;
        QString mask_n;

    unsigned int grid = 0;
    QString group;
    QString category;
    bool animated = false;
    unsigned int frames = 0;
    unsigned int framespeed = 0; // Default 128 ms
    unsigned int display_frame = 0;
    unsigned int frame_h = 0; //Hegth of the frame. Calculating automatically

    bool                map3d_vertical = false;
    QList<unsigned int> map3d_stackables;

    /**Used only by terrain and path tiles **/
    unsigned int row = 0;
    unsigned int col = 0;
    /****************************************/
};

#endif // CONFIG_WLD_GENERIC_H
