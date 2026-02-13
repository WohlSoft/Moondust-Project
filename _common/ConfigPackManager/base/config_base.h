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

#ifndef CONFIG_BASE_H
#define CONFIG_BASE_H

#include <PGEString.h>
#include <stdint.h>

class IniProcessing;

struct ConfigBaseSetup
{
    ConfigBaseSetup() = default;
    ConfigBaseSetup(const ConfigBaseSetup &o) = default;
    ConfigBaseSetup &operator=(const ConfigBaseSetup &o) = default;

protected:
    /**
     * @brief Read data from the external INI file and fill the config
     * @param setup [_in] Instance of opened INI file processor
     * @param imgPath [_in] Folder path where images are stored
     * @param defaultGrid [_in] Default grid size
     * @param w [_inout] Buffer for obtained width value
     * @param h [_inout] Buffer for obtained height value
     * @param merge_with [_in] Another element config to use as source of default values
     * @param error [_out] Output string for error messages writing
     * @return
     */
    bool parseBase(IniProcessing* setup,
                   PGEString imgPath,
                   uint32_t defaultGrid,
                   uint32_t &w,
                   uint32_t &h,
                   const ConfigBaseSetup* merge_with = nullptr,
                   PGEString *error = nullptr, bool skipImage = false);
public:

    //! Element type ID
    uint64_t    id = 0;
    //! Sprite image filename
    PGEString   image_n;
    //! Mask of sprite image filename (for bitmask GIF pair only)
    PGEString   mask_n;

#ifdef PGE_EDITOR // Editor-only field
    //! (Optional) In-editor icon for item boxes
    PGEString   icon_n;
#endif

    //! Alignment grid size
    uint32_t    grid = 32;
    //! Alignment grid offset X in pixels
    int32_t     grid_offset_x = 0;
    //! Alignment grid offset Y in pixels
    int32_t     grid_offset_y = 0;

#ifdef PGE_EDITOR // Editor-only fields
    //! Understandible name of element
    PGEString   name;
    //! Filter group name
    PGEString   group    = "_NoGroup";
    //! Filter category name
    PGEString   category = "_Other";
    //! Detailed description of element
    PGEString   description;
#endif

    //! Extra Settings JSON layout file name
    PGEString   extra_settings;

    //! Is this a meta-object that should be never shown in game or exported images (screenshots)
    bool        is_meta_object = false;
};

#endif // CONFIG_BASE_H
