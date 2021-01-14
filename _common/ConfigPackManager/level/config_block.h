/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef CONFIG_BLOCK_H
#define CONFIG_BLOCK_H

#include <PGEString.h>
#include <stdint.h>

class IniProcessing;

struct BlockSetup
{
    /**
     * @brief Read data from the external INI file and fill the config
     * @param setup [_in] Instance of opened INI file processor
     * @param blockImgPath [_in] Folder path where images are stored
     * @param defaultGrid [_in] Default grid size
     * @param merge_with [_in] Another element config to use as source of default values
     * @param error [_out] Output string for error messages writing
     * @return
     */
    bool parse(IniProcessing *setup,
               PGEString blockImgPath,
               uint32_t defaultGrid,
               const BlockSetup *merge_with = nullptr,
               PGEString *error = nullptr);

    //! Element type ID
    uint64_t        id = 0;
    //! Sprite image filename
    PGEString       image_n;
    //! Mask of sprite image filename (for bitmask GIF pair only)
    PGEString       mask_n;

    //! (Optional) In-editor icon for item boxes
    PGEString       icon_n;

    //! Understandible name of element
    PGEString       name;
    //! Alignment grid size
    uint32_t        grid = 32;
    //! Alignment grid offset X in pixels
    int32_t         grid_offset_x = 0;
    //! Alignment grid offset Y in pixels
    int32_t         grid_offset_y = 0;
    //! Filter group name
    PGEString       group = "_NoGroup";
    //! Filter category name
    PGEString       category = "_Other";
    //! Detailed description of element
    PGEString       description = "";

    //! Extra Settings JSON layout file name
    PGEString       extra_settings = "";

    //! Is this a meta-object that should be never shown in game or exported images (screenshots)
    bool            is_meta_object = false;

    //! Is block allowed to have any size?
    bool            sizable = false;
    //! Width of the border of sizable block texture (common)
    int32_t         sizable_border_width = -1;
    //! Width of the border of sizable block texture (left side)
    int32_t         sizable_border_width_left = -1;
    //! Width of the border of sizable block texture (top side)
    int32_t         sizable_border_width_top = -1;
    //! Width of the border of sizable block texture (right side)
    int32_t         sizable_border_width_right = -1;
    //! Width of the border of sizable block texture (bottom side)
    int32_t         sizable_border_width_bottom = -1;

    //! [Gameplay] Define sides are will be harmful for playable character on touching of the block
    int             danger = 0;
    //! [Physics] Sides are collidable to everything
    int             collision = 0;
    //! [Gameplay] This block allows to activate slope sliding (triangular blocks only)
    bool            slopeslide = false;

    /**
     * @brief Physical shape of the block
     */
    enum shapes
    {
        //! Rectangular shape
        SHAPE_rect = 0,
        /*     __
         *    |  |
         *    |  |
         *    |__|
        */

        //! Triangular shape with right and bottom sides. Slope is from top-right to left-bottom.
        SHAPE_triangle_right_bottom = -1,
        /*
         *      /|
         *     / |
         *    /__|
         */

        //! Triangular shape with left and bottom sides. Slope is from top-left to right-bottom.
        SHAPE_triangle_left_bottom = 1,
        /*
         *    |\
         *    | \
         *    |__\
         */

        //! Triangular shape with right and top sides. Slope is from bottom-right to left-top.
        SHAPE_triangle_right_top = -2,
        /*    ___
         *    \  |
         *     \ |
         *      \|
         */

        //! Triangular shape with left and top sides. Slope is from bottom-left to right-top.
        SHAPE_triangle_left_top = 2,
        /*    ___
         *   |  /
         *   | /
         *   |/
         */

        //! [WIP] Custom polygonal shape
        SHAPE_polygonal = 3,
        /*    ___
         *   |   /
         *   |   \
         *   |/\__|
         */

        //! [WIP] Circular shape
        SHAPE_cyrcle = 4
        /*      __
         *    /    \
         *   |      |
         *    \____/
         */
    };
    //! [Physics] Physical shape of the block
    int             phys_shape = SHAPE_rect;
    //! [Gameplay] Is this block a lava?
    bool            lava = false;
    //! [Gameplay] Can this block by destroyed by any attack?
    bool            destroyable = false;
    //! [Gameplay] Can this block by destroyed by explosion?
    bool            destroyable_by_bomb = false;
    //! [Gameplay] Can this block by destroyed by the fire attack?
    bool            destroyable_by_fireball = false;
    //! [Gameplay] Spawn object on destroy
    bool            spawn = false; //split string by "-" in != "0"
    /**
     * @brief [Gameplay] Object type to transform on destroy
     */
    enum spawnes
    {
        //! Don't transform on destroy
        SPAWN_Nothing = 0,
        //! Transform block into NPC
        SPAWN_NPC = 1,
        //! Transform block into another block
        SPAWN_Block = 2,
        //! Transform block into BGO
        SPAWN_BGO = 3
    };
    //! [Gameplay] Transform block into another object on destroy
    int32_t         spawn_obj = SPAWN_Nothing;
    //! [Gameplay] Element ID to transform
    uint64_t        spawn_obj_id = 0;
    //! [Gameplay] Spawn effect on destroy
    uint32_t        effect = 0;
    //! [Gameplay] Is block bouncing
    bool            bounce = false;
    //! [Gameplay] Block can react on bumps and attacks
    bool            bumpable = false;
    //! [Gameplay] Transform block into on attempt to attack it
    uint32_t        transfororm_on_hit_into = 0;

    //Toggable Switch blocks
    //! Is block a switch button (which toggles filter)
    bool            switch_Button = false;
    //! Is block a switch filter block (block which can allow or deny passtrough)
    bool            switch_Block = false;
    //! ID of the switchable blocks group (for example, switch blocks of same color)
    uint32_t        switch_ID = 0;
    //! Transform block into ID on toggling of the switch group
    uint32_t        switch_transform = 0;

    //! Array of integers
    typedef PGEList<int32_t> IntArray;

    //Playable character Switch/Filter blocks
    //! Block which will transform playable character on attempt to attack the block
    bool            plSwitch_Button  = false;
    //! ID of a playable character to transform into
    uint32_t        plSwitch_Button_id = 0; //Target Character ID
    //! Frame sequence to show when playable character ID is equal to ID of this block
    IntArray        plSwitch_frames_true;
    //! Frame sequence to show when playable character ID is different from this block
    IntArray        plSwitch_frames_false;

    //! Filter block that allows passthrough of specific playable character ID
    bool            plFilter_Block = false;   //Target Character ID
    //! ID of a playable character who allowed to passthrough this block
    uint32_t        plFilter_Block_id = 0;//Target Character ID
    //! Frame sequence to show when playable character ID is equal to ID of this block
    IntArray        plFilter_frames_true;
    //! Frame sequence to show when playable character ID is different from this block
    IntArray        plFilter_frames_false;

    //! Base Z-Layer to render the block
    uint32_t        z_layer = 0;
    //! Has element an animated sprite?
    bool            animated = false;
    //! Reversive animation
    bool            animation_rev = false;
    //! Bidirectional animation
    bool            animation_bid = false;
    //! Physical count of frames on the sprite
    uint32_t        frames = 0;
    //! Delay between of animation frames in milliseconds
    uint32_t        framespeed = 0;
    //! Initial frame to show when animation is turned off
    uint32_t        display_frame = 0;
    //!Custom editor specific frame sequence
    IntArray        frame_sequence;

    //! [Automatical value] Hegth of one frame
    uint32_t        frame_h = 0;

    //! Play custom sound on hit
    uint32_t        hit_sound_id = 0;
    //! Play custom sound on destroy
    uint32_t        destroy_sound_id = 0;


    //====Editor defaults====

    //! [In-Editor] Default value for the slippery flag, raw config values
    int64_t default_slippery_raw = -1;
    //! [In-Editor] Force default slippery flag for new adding blocks
    bool    default_slippery = false;
    //! [In-Editor] Default value for the slippery flag
    bool    default_slippery_value = false;

    //! [In-Editor] Default value for the invisible flag, raw config value
    int64_t default_invisible_raw = -1;
    //! [In-Editor] Force default invisible flag for new adding blocks
    bool    default_invisible = false;
    //! [In-Editor] Default value for the invisible flag
    bool    default_invisible_value = false;

    //! [In-Editor] Default value for NPC content, raw config value
    int64_t default_content_raw = -1;
    //! [In-Editor] Force default NPC content for new adding blocks
    bool    default_content = false;
    //! [In-Editor] Default value for NPC content
    int64_t default_content_value = 0;
};


#endif // CONFIG_BLOCK_H
