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

#include <IniProcessor/ini_processing.h>
#include <Utils/maths.h>
#include "../image_size.h"
#include "../../number_limiter.h"

#include <assert.h>

bool WldGenericSetup::parse(IniProcessing *setup,
                            PGEString imgPath,
                            uint32_t defaultGrid,
                            WldGenericSetup *merge_with,
                            PGEString *error)
{
    #define pMerge(param, def) (merge_with ? (merge_with->param) : (def))
    #define pMergeMe(param) (merge_with ? (merge_with->param) : (param))

    int errCode = PGE_ImageInfo::ERR_OK;
    PGEString section;
    /*************Buffers*********************/
    uint32_t    w = 0,
                h = 0;

    /*************Buffers*********************/
    if(!setup)
    {
        if(error)
            *error = "setup QSettings is null!";

        return false;
    }

    section     = StdToPGEString(setup->group());
    setup->read("group",    group,      pMergeMe(group));
    setup->read("category", category,   pMergeMe(category));
    setup->read("image",    image_n,    pMerge(image_n, ""));

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

    assert(merge_with || ((w > 0) && (h > 0) && "Width or height of image has zero or negative value!"));
    mask_n  =    PGE_ImageInfo::getMaskName(image_n);
    setup->read("grid",         grid,       pMerge(grid, defaultGrid));
    setup->read("animated",     animated,   pMerge(animated, 0));
    setup->read("frames",       frames,     pMerge(frames, 1));
    NumberLimiter::apply(frames, uint32_t(1u));
    setup->read("frame-speed",  framespeed, pMerge(framespeed, 175));
    NumberLimiter::apply(framespeed, uint32_t(1u));
    frame_h = animated ? Maths::uRound(double(h) / double(frames)) : h;
    NumberLimiter::apply(frame_h, uint32_t(0u));

    setup->read("display-frame",    display_frame,      pMerge(display_frame, 0));
    setup->read("map3d-vertical",   map3d_vertical,     pMerge(map3d_vertical, false));
    setup->read("map3d-stackables", map3d_stackables,   pMerge(map3d_stackables, map3d_stackables));

    //Rows and cols for table-like element sets
    setup->read("row", row, pMerge(row, 0));
    setup->read("col", col, pMerge(col, 0));

    #undef pMerge
    #undef pMergeMe
    return true;
}
