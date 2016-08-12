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

#ifndef CONFIG_BGO_H
#define CONFIG_BGO_H

#include <QString>
#include <QList>

class QSettings;

struct BgoSetup
{
    BgoSetup()
    {
        id = 0;
        grid = 32;
    }

    bool parse(QSettings* setup, QString bgoImgPath, int defaultGrid, BgoSetup* merge_with = nullptr, QString *error=nullptr);

    unsigned long id;
    QString name;
    QString group;
    QString category;
    unsigned int grid;

    enum ZLayerEnum
    {
        z_background_2 =-1,
        z_background_1 = 0,
        z_foreground_1 = 1,//Default
        z_foreground_2 = 2
    };
    int zLayer; //-1, 0, 1, 2
    int offsetX;
    int offsetY;
    long double zOffset;

    QString image_n;
    QString mask_n;

    unsigned int frame_h;

    bool climbing;
    bool animated;
    unsigned int frames;
    unsigned int framespeed;

    unsigned int display_frame;
};

#endif // CONFIG_BGO_H
