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

#ifndef CONFIG_BLOCK_H
#define CONFIG_BLOCK_H

#include <PGEString.h>
#include <stdint.h>

class IniProcessing;

struct BlockSetup
{
    bool parse(IniProcessing *setup,
               PGEString blockImgPath,
               uint32_t defaultGrid,
               BlockSetup *merge_with = nullptr,
               PGEString *error = nullptr);

    uint64_t        id = 0;
    PGEString       image_n;
    PGEString       mask_n;
    PGEString       name;
    //    grid=32               ; 32 | 16 Default="32"
    unsigned int    grid = 32;
    PGEString       group = "_NoGroup";
    PGEString       category = "_Other";
    bool            sizable = false;
    int             danger = 0;
    int             collision = 0;
    bool            slopeslide = false;
    enum shapes
    {
        SHAPE_rect = 0,
        /*     __
         *    |  |
         *    |  |
         *    |__|
        */

        SHAPE_triangle_right_bottom = -1,
        /*
         *      /|
         *     / |
         *    /__|
         */

        SHAPE_triangle_left_bottom = 1,
        /*
         *    |\
         *    | \
         *    |__\
         */

        SHAPE_triangle_right_top = -2,
        /*    ___
         *    \  |
         *     \ |
         *      \|
         */

        SHAPE_triangle_left_top = 2,
        /*    ___
         *   |  /
         *   | /
         *   |/
         */

        SHAPE_polygonal = 3,
        SHAPE_cyrcle = 4
    };
    int             phys_shape = SHAPE_rect;
    bool            lava = false;
    bool            destroyable = false;
    bool            destroyable_by_bomb = false;
    bool            destroyable_by_fireball = false;
    bool            spawn = false; //split string by "-" in != "0"
    enum spawnes
    {
        SPAWN_Nothing = 0,
        SPAWN_NPC = 1,
        SPAWN_Block = 2,
        SPAWN_BGO = 3
    };
    int32_t         spawn_obj = SPAWN_Nothing;
    uint64_t        spawn_obj_id = 0;
    uint32_t        effect = 0;
    bool            bounce = false;
    bool            hitable = false;
    uint32_t        transfororm_on_hit_into = 0;

    //Toggable Switch blocks
    bool            switch_Button = false;
    bool            switch_Block = false;
    uint32_t        switch_ID = 0;
    uint32_t        switch_transform = 0;

    typedef PGEList<int32_t> IntArray;

    //Playable character Switch/Filter blocks
    bool            plSwitch_Button  = false;
    uint32_t        plSwitch_Button_id = 0; //Target Character ID
    IntArray        plSwitch_frames_true;
    IntArray        plSwitch_frames_false;

    bool            plFilter_Block = false;   //Target Character ID
    uint32_t        plFilter_Block_id = 0;//Target Character ID
    IntArray        plFilter_frames_true;
    IntArray        plFilter_frames_false;

    uint32_t        view = 0;
    bool            animated = false;
    bool            animation_rev = false; //Reverse animation
    bool            animation_bid = false; //Bidirectional animation
    uint32_t        frames = 0;
    uint32_t        framespeed = 0;

    //! Play custom sound on hit
    uint32_t        hit_sound_id = 0;
    //! Play custom sound on destroy
    uint32_t        destroy_sound_id = 0;

    uint32_t        frame_h = 0; //Hegth of the frame. Calculating automatically

    IntArray        frame_sequence;//Custom editor specific frame sequence

    uint32_t        display_frame = 0;

    //Editor defaults
    bool    default_slippery = false; //Slippery flag
    bool    default_slippery_value = false;

    bool    default_invisible = false; //Invisible flag
    bool    default_invisible_value = false;

    bool    default_content = false; //Content value
    int64_t default_content_value = 0;
};


#endif // CONFIG_BLOCK_H
