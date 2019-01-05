/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
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
                            const WldGenericSetup *merge_with,
                            PGEString *error)
{
    #define pMerge(param, def) (merge_with ? pgeConstReference(merge_with->param) : pgeConstReference(def))
    #define pMergeMe(param) (merge_with ? pgeConstReference(merge_with->param) : pgeConstReference(param))
    #define pAlias(paramName, destValue) setup->read(paramName, destValue, destValue)

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
    setup->read("name",     name,       pMerge(name, ""));
    setup->read("group",    group,      pMergeMe(group));
    setup->read("category", category,   pMergeMe(category));
    setup->read("description", description, pMerge(description, ""));
    setup->read("extra-settings", extra_settings, pMerge(extra_settings, ""));

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
            default:
                break;
            }
        }

        return false;
    }
    assert(merge_with || ((w > 0) && (h > 0) && "Width or height of image has zero or negative value!"));
    mask_n  =    PGE_ImageInfo::getMaskName(image_n);

    setup->read("icon", icon_n, pMerge(icon_n, ""));

    setup->read("grid",         grid,       pMerge(grid, defaultGrid));
    setup->read("frames",       frames,     pMerge(frames, 1u));//Real
    pAlias("framecount",        frames);//Alias
    pAlias("frame-count",       frames);//Alias
    NumberLimiter::apply(frames, uint32_t(1u));
    animated = (frames > 1u);
    setup->read("frame-delay", framespeed, pMerge(framespeed, 125));//Real
    pAlias("frame-speed", framespeed);//Alias
    if(setup->hasKey("framespeed"))
    {
        pAlias("framespeed",  framespeed);//Alias
        framespeed = (framespeed * 1000u) / 65u;//Convert 1/65'th into milliseconds
    }
    NumberLimiter::apply(framespeed, uint32_t(1u));

    frame_sequence.clear();
    setup->read("frame-sequence", frame_sequence, pMergeMe(frame_sequence));

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
    #undef pAlias
    return true;
}
