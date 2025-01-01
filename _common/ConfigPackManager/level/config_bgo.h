/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2025 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef CONFIG_BGO_H
#define CONFIG_BGO_H

#include <PGEString.h>
#include <stdint.h>

class IniProcessing;

struct BgoSetup
{
    /**
     * @brief Read data from the external INI file and fill the config
     * @param setup [_in] Instance of opened INI file processor
     * @param bgoImgPath [_in] Folder path where images are stored
     * @param defaultGrid [_in] Default grid size
     * @param merge_with [_in] Another element config to use as source of default values
     * @param error [_out] Output string for error messages writing
     * @return
     */
    bool parse(IniProcessing* setup,
               PGEString bgoImgPath,
               uint32_t defaultGrid,
               const BgoSetup* merge_with = nullptr,
               PGEString *error = nullptr);

    //! Element type ID
    uint64_t    id = 0;
    //! Sprite image filename
    PGEString   image_n;
    //! Mask of sprite image filename (for bitmask GIF pair only)
    PGEString   mask_n;

    //! (Optional) In-editor icon for item boxes
    PGEString   icon_n;

    //! Alignment grid size
    uint32_t    grid = 32;
    //! Alignment grid offset X in pixels
    int32_t     grid_offset_x = 0;
    //! Alignment grid offset Y in pixels
    int32_t     grid_offset_y = 0;

    //! Understandible name of element
    PGEString   name;
    //! Filter group name
    PGEString   group = "_NoGroup";
    //! Filter category name
    PGEString   category = "_Other";
    //! Detailed description of element
    PGEString   description = "";

    //! Extra Settings JSON layout file name
    PGEString   extra_settings = "";

    //! Is this a meta-object that should be never shown in game or exported images (screenshots)
    bool        is_meta_object = false;

    /**
     * @brief Base Z-Layer type
     */
    enum ZLayerEnum
    {
        //! Use "Background-2" Z-Layer as a base
        z_background_2 =-1,
        //! Use "Background-1" Z-Layer as a base
        z_background_1 = 0,
        //! Use "Foreground-1" Z-Layer as a base
        z_foreground_1 = 1,//Default
        //! Use "Foreground-2" Z-Layer as a base
        z_foreground_2 = 2
    };
    //! Base Z-Layer type
    int32_t     zLayer = 0; //-1, 0, 1, 2
    //! Relative Z-Offset at the value of the base Z-Layer
    long double zOffset = 0.0l;
    //! Force using of absolute Z-Value without of referencing to the base Z-Layer
    bool        zValueOverride = false;
    //! Absolute Z-Value
    long double zValue = 0.0l;

    //! [Gameplay] Climbable surface for the playable character or NPCs are can climb
    bool        climbing = false;
    //! Has element an animated sprite?
    bool        animated = false;
    //! Phyiscal count of animation frames on the sprite
    uint32_t    frames = 1;
    //! Delay between of animation frames in milliseconds
    uint32_t    framespeed = 128;
    //! Initial frame to show when animation is turned off
    uint32_t    display_frame = 0;
    //! Custom animation sequence
    PGEList<int32_t> frame_sequence;

    //! [Automatical value] Hegth of one frame
    uint32_t    frame_h = 0;
};

#endif // CONFIG_BGO_H
