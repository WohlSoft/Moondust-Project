/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2026 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <IniProcessor/ini_processing.h>
#include "../image_size.h"

#include "config_base.h"


bool ConfigBaseSetup::parseBase(IniProcessing *setup,
                                PGEString imgPath,
                                uint32_t defaultGrid,
                                uint32_t &w,
                                uint32_t &h,
                                const ConfigBaseSetup *merge_with,
                                PGEString *error,
                                bool skipImage)
{
#define pMerge(param, def) (merge_with ? pgeConstReference(merge_with->param) : pgeConstReference(def))
#define pMergeMe(param) (merge_with ? pgeConstReference(merge_with->param) : pgeConstReference(param))
#define pAlias(paramName, destValue) setup->read(paramName, destValue, destValue)
    int errCode = PGE_ImageInfo::ERR_OK;
    PGEString section;

    if(!setup)
    {
        if(error)
            *error = "setup IniProcessing is null!";

        return false;
    }

    section     = StdToPGEString(setup->group());

#ifdef PGE_EDITOR // Editor-only fields
    setup->read("name",     name,       pMerge(name, section));

    if(name.size() == 0)
    {
        if(error)
            *error = section + ": item name isn't defined";
        return false;
    }

    setup->read("group",    group,      pMergeMe(group));
    setup->read("category", category,   pMergeMe(category));
    setup->read("description", description, pMerge(description, ""));
#endif

    setup->read("grid", grid, pMerge(grid, defaultGrid));
    setup->read("grid-offset-x", grid_offset_x, pMerge(grid_offset_x, 0));
    setup->read("grid-offset-y", grid_offset_y, pMerge(grid_offset_y, 0));

    setup->read("extra-settings", extra_settings, pMerge(extra_settings, ""));
    setup->read("is-meta-object", is_meta_object, pMerge(is_meta_object, false));
    pAlias("hide-on-exported-images", is_meta_object);//Alias

    if(!skipImage)
    {
        setup->read("image",    image_n,    pMergeMe(image_n));
#ifdef PGE_EDITOR // alternative image for Editor
        pAlias("editor-image", image_n);
#endif

#ifdef PGE_EDITOR // Editor-only field
        setup->read("icon", icon_n, pMerge(icon_n, ""));
#endif

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

        if(!merge_with && ((w == 0) || (h == 0)))
        {
            if(error)
                *error = "Width or height of image has zero or negative value in image " + imgPath + image_n;
            return false;
        }

        mask_n = PGE_ImageInfo::getMaskName(image_n);
    }

#undef pMerge
#undef pMergeMe
#undef pAlias
    return true;
}
