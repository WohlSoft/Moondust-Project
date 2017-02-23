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

#include "config_bgo.h"

#include <IniProcessor/ini_processing.h>
#include "../image_size.h"
#include "../../number_limiter.h"

bool BgoSetup::parse(IniProcessing *setup,
                     PGEString bgoImgPath,
                     uint32_t defaultGrid,
                     BgoSetup *merge_with,
                     PGEString *error)
{
    int errCode = PGE_ImageInfo::ERR_OK;
    PGEString section;
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

    section = StdToPGEString(setup->group());
    setup->read("name", name, (merge_with ? merge_with->name : section));

    if(name.size() == 0)
    {
        if(error)
            *error = section + ": item name isn't defined";
        return false;
    }

    setup->read("group", group, (merge_with ? merge_with->group : "_NoGroup"));
    setup->read("category", category, (merge_with ? merge_with->category : category));
    setup->read("grid", grid, (merge_with ? merge_with->grid : defaultGrid));
    setup->read("offset-x", offsetX, (merge_with ? merge_with->offsetX : 0));
    setup->read("offset-y", offsetY, (merge_with ? merge_with->offsetY : 0));
    setup->read("image", image_n, (merge_with ? merge_with->image_n : ""));

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
        setup->readEnum("view", zLayer, (merge_with ? merge_with->zLayer : z_background_1),
                        {
                            {"foreground2", z_foreground_2},
                            {"foreground1", z_foreground_1},
                            {"foreground",  z_foreground_1},
                            {"background",  z_background_1},
                            {"background1", z_background_1},
                            {"background2", z_background_2},
                        });
    }
    zOffset = (long double)setup->value("z-offset", (merge_with ? double(merge_with->zOffset) : 0.0)).toDouble();
    setup->read("climbing", climbing , (merge_with ? merge_with->climbing : false));
    setup->read("animated", animated, (merge_with ? merge_with->animated : false));
    setup->read("frames", frames, (merge_with ? merge_with->frames : 1));
    NumberLimiter::apply(frames, 1u);
    setup->read("frame-speed", framespeed, (merge_with ? merge_with->framespeed : 125));
    NumberLimiter::apply(frame_h, 0u);
    frame_h =   uint(animated ? qRound(qreal(h) / qreal(frames)) : h);
    setup->read("display-frame", display_frame, (merge_with ? merge_with->display_frame : 0));
    NumberLimiter::apply(display_frame, 0u);
    return true;
}
