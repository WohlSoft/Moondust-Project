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

#include "config_bgo.h"

#include <QSettings>
#include "../image_size.h"
#include "../../number_limiter.h"

bool BgoSetup::parse(QSettings *setup, QString bgoImgPath, unsigned int defaultGrid, BgoSetup *merge_with, QString *error)
{
    int errCode = PGE_ImageInfo::ERR_OK;
    QString section;
    /*************Buffers*********************/
    QString tmpStr;
    int w = -1,
        h = -1;

    /*************Buffers*********************/
    if(!setup)
    {
        if(error)
            *error = "setup QSettings is null!";

        return false;
    }

    section = setup->group();
    name =       setup->value("name", (merge_with ? merge_with->name : section)).toString();

    if(name.isEmpty())
    {
        if(error)
            *error = QString("%1 Item name isn't defined").arg(section.toUpper());

        return false;
    }

    group =      setup->value("group", (merge_with ? merge_with->group : "_NoGroup")).toString();
    category =   setup->value("category", (merge_with ? merge_with->category : "_Other")).toString();
    grid =       setup->value("grid", (merge_with ? merge_with->grid : defaultGrid)).toUInt();
    offsetX =    setup->value("offset-x", (merge_with ? merge_with->offsetX : 0)).toInt();
    offsetY =    setup->value("offset-y", (merge_with ? merge_with->offsetY : 0)).toInt();
    image_n =           setup->value("image", (merge_with ? merge_with->image_n : "")).toString();

    if(!merge_with && !PGE_ImageInfo::getImageSize(bgoImgPath + image_n, &w, &h, &errCode))
    {
        if(error)
        {
            switch(errCode)
            {
            case PGE_ImageInfo::ERR_UNSUPPORTED_FILETYPE:
                *error = "Unsupported or corrupted file format: " + bgoImgPath + image_n;
                break;

            case PGE_ImageInfo::ERR_NOT_EXISTS:
                *error = "image file is not exist: " + bgoImgPath + image_n;
                break;

            case PGE_ImageInfo::ERR_CANT_OPEN:
                *error = "Can't open image file: " + bgoImgPath + image_n;
                break;
            }
        }

        return false;
    }

    Q_ASSERT(merge_with || ((w > 0) && (h > 0) && "Width or height of image has zero or negative value!"));
    mask_n = PGE_ImageInfo::getMaskName(image_n);
    {
        tmpStr = setup->value("view", "background").toString();

        if(tmpStr == "foreground2")
            zLayer = z_foreground_2;
        else if(tmpStr == "foreground")
            zLayer = z_foreground_1;
        else if(tmpStr == "background")
            zLayer = (merge_with ? merge_with->zLayer : z_background_1);
        else if(tmpStr == "background2")
            zLayer = z_background_2;
        else
            zLayer = z_background_1;
    }
    zOffset = (long double)setup->value("z-offset", (merge_with ? double(merge_with->zOffset) : 0.0)).toDouble();
    climbing = (setup->value("climbing", (merge_with ? merge_with->climbing : false)).toBool());
    animated = (setup->value("animated", (merge_with ? merge_with->animated : false)).toBool());
    frames =        setup->value("frames", (merge_with ? merge_with->frames : 1)).toUInt();
    NumberLimiter::apply(frames, 1u);
    framespeed =    setup->value("frame-speed", (merge_with ? merge_with->framespeed : 125)).toUInt();
    NumberLimiter::apply(frame_h, 0u);
    frame_h =   uint(animated ? qRound(qreal(h) / qreal(frames)) : h);
    display_frame = setup->value("display-frame", (merge_with ? merge_with->display_frame : 0)).toUInt();
    NumberLimiter::apply(display_frame, 0u);
    return true;
}
