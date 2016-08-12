/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
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
    WldGenericSetup()
    {
        id = 0;
        grid = 0;
        animated = false;
        frames = 0;
        display_frame = 0;
        frame_h = 0;
    }
    bool parse(QSettings* setup, QString imgPath, int defaultGrid, WldGenericSetup* merge_with = nullptr, QString *error=nullptr);

    unsigned long id;
        QString image_n;
        QString mask_n;

    unsigned int grid;
    QString group;
    QString category;
    bool animated;
    unsigned int frames;
    unsigned int framespeed; // Default 128 ms
    unsigned int display_frame;
    unsigned int frame_h; //Hegth of the frame. Calculating automatically

    /**Used only by terrain and path tiles **/
    unsigned int row;
    unsigned int col;
    /****************************************/
};

#endif // CONFIG_WLD_GENERIC_H
