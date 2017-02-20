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

#include "config_wld_generic.h"

#include <QSettings>
#include "../image_size.h"
#include "../../number_limiter.h"

bool WldGenericSetup::parse(QSettings *setup, QString imgPath, unsigned int defaultGrid, WldGenericSetup *merge_with, QString *error)
{
    int errCode = PGE_ImageInfo::ERR_OK;
    QString section;
    /*************Buffers*********************/
    int w = -1,
        h = -1;

    /*************Buffers*********************/
    if(!setup)
    {
        if(error)
            *error = "setup QSettings is null!";

        return false;
    }

    section     = setup->group();
    group       = setup->value("group", merge_with ? merge_with->group : "_NoGroup").toString();
    category    = setup->value("category", merge_with ? merge_with->category : "_Other").toString();
    image_n     = setup->value("image", (merge_with ? merge_with->image_n : "")).toString();

    if(!merge_with && !PGE_ImageInfo::getImageSize(imgPath + image_n, &w, &h, &errCode))
    {
        if(error)
        {
            switch(errCode)
            {
            case PGE_ImageInfo::ERR_UNSUPPORTED_FILETYPE:
                *error = "Unsupported or corrupted file format: " + imgPath + image_n;
                break;

            case PGE_ImageInfo::ERR_NOT_EXISTS:
                *error = "image file is not exist: " + imgPath + image_n;
                break;

            case PGE_ImageInfo::ERR_CANT_OPEN:
                *error = "Can't open image file: " + imgPath + image_n;
                break;
            }
        }

        return false;
    }

    Q_ASSERT(merge_with || ((w > 0) && (h > 0) && "Width or height of image has zero or negative value!"));
    mask_n = PGE_ImageInfo::getMaskName(image_n);
    grid =            setup->value("grid", merge_with ? merge_with->grid : defaultGrid).toUInt();
    animated =        setup->value("animated", merge_with ? merge_with->animated : 0).toBool();
    frames =          setup->value("frames", merge_with ? merge_with->frames : 1).toUInt();
    NumberLimiter::apply(frames, 1u);
    framespeed =      setup->value("frame-speed", merge_with ? merge_with->framespeed : 175).toUInt();
    NumberLimiter::apply(framespeed, 1u);
    frame_h = uint(animated ? qRound(qreal(h) / qreal(frames)) : h);
    NumberLimiter::apply(frame_h, 0u);

    display_frame =   setup->value("display-frame", merge_with ? merge_with->display_frame : 0).toUInt();

    map3d_vertical =  setup->value("map3d-vertical", merge_with ? merge_with->map3d_vertical : false).toBool();
    //Uncommend when add the IniProcessing which a replacement of QSettings for INI reading
    //map3d_stackables= setup->value("map3d-stackables", merge_with ? merge_with->map3d_stackables).toIntArray();

    //Rows and cols for table-like element sets
    row =             setup->value("row", merge_with ? merge_with->row : 0).toUInt();
    col =             setup->value("col", merge_with ? merge_with->col : 0).toUInt();
    return true;
}
