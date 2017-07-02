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

#ifndef CONFIG_BG_H
#define CONFIG_BG_H

#include <PGEString.h>
#include <stdint.h>

class IniProcessing;

struct BgSetup
{
    bool parse(IniProcessing *setup,
               PGEString bgImgPath,
               uint32_t defaultGrid,
               BgSetup *merge_with = nullptr,
               PGEString *error = nullptr);

    enum BgType
    {
        //! Single-row background
        BG_TYPE_SingleRow = 0,
        //! Double-row background. Requiers usage of second image
        BG_TYPE_DoubleRow = 1,
        //! Background will be repeated in both horizontal and vertical directions
        BG_TYPE_Tiled = 2
    };

    enum BgReleatVertical
    {
        BG_REPEAT_V_NR_NoRepeat = 0,
        BG_REPEAT_V_ZR_NoRepeatNoParallax = 1,
        BG_REPEAT_V_RP_RepeatParallax,
        BG_REPEAT_V_RZ_RepeatNoParallax
    };

    enum BgAttach
    {
        BG_ATTACH_TO_BOTTOM = 0,
        BG_ATTACH_TO_TOP = 1
    };

    enum BgAttachSecond
    {
        BG_ATTACH_2_TO_TOP_OF_FIRST = 0,
        BG_ATTACH_2_TO_BOTTOM = 1,
        BG_ATTACH_2_TO_TOP = 2
    };

    typedef PGEList<int32_t> IntArray;

    /*
     *  Generic properties
     */
    //! ID of background
    uint64_t        id = 0ul;
    //! Name of background
    PGEString       name;
    /// Default color to fill background.
    /*!
        "Auto" means fill color will be picked up from left-top pixel of background image or black
     */
    PGEString       fill_color = "auto";

    /*
     *  First image
     */
    //! Filename for first image
    PGEString       image_n;
    //! Icon that will be shown in the backgrounds list
    PGEString       icon_n;
    //! Type of background
    uint32_t        type = BG_TYPE_SingleRow;
    //! Horizontal parallax coefficient
    double          repeat_h = 2.0;
    //! Algorithm of vertical repeat and parallax
    uint32_t        repeat_v = BG_REPEAT_V_NR_NoRepeat;
    //! Attach background to top or bottom of section
    uint32_t        attached = BG_ATTACH_TO_BOTTOM;
    //! Force editor draw background via tiling algorithm
    bool            editing_tiled = false;


    /*
     *  Animation setup
     */
    //! Is background animated
    bool            animated = false;
    //! Count of animation frames of background image
    uint32_t        frames = 1;
    //! Delay between animation frames in milliseconds
    uint32_t        framespeed = 128;
    //! Height of one frame [must be calculated automatically by using real image metrics]
    uint32_t        frame_h = 1;
    //! Default frame to display when animation is disabled
    uint32_t        display_frame = 0;


    /*
     *  Magic background
     */
    /// Turn on "magic" background.
    /*!
        Magic background allow to split background image to horizontal
        stripses are have different parallax coefficients
    */
    bool            magic = false;
    //! Count of stripses
    uint32_t        magic_strips = 1u;
    //! Comma-separated set of Y coordinates of split lines
    PGEString       magic_splits;
    //! Decoded set of Y coordinates of split lines  [decoded from magic_splits]
    PGEList<uint32_t> magic_splits_i;
    //! Comma-separated set of parallax coefficients for every strip line
    PGEString       magic_speeds;
    //! Decoded set of parallax coefficients for every strip line [decoded from magic_speeds]
    PGEList<double> magic_speeds_i;

    /*
     *  Second image
     */
    //! Filename for second background image [Double-row only]
    PGEString       second_image_n;
    //! Parallax coefficient for second image
    double          second_repeat_h = 4.0;
    //! Algorithm of vertical repeat and parallax of second image
    uint32_t        second_repeat_v = BG_REPEAT_V_ZR_NoRepeatNoParallax;
    //! Attach second image of background to top or bottom of section
    uint32_t        second_attached = BG_ATTACH_2_TO_TOP_OF_FIRST;


    /*
     *  Multi-layaring background
     */
    enum MultiLayersParallaxType
    {
        //! Parallax coefficients will be calculated proportionally to position between 0 or min for back and 0 to max for front
        ML_PARALLAX_AUTO = 0,
        //! Parallax coefficients must be defined manually
        ML_PARALLAX_MANUAL = 1
    };

    //! Turn background into multi-layering mode
    bool            multi_layered = false;
    //! Count of layers
    uint32_t        multi_layers_count = 1;
    /// Type of parallax
    /*!
        auto    - parallax coefficient will be calculated relative to distance between 0 to min for backgrounds,
                  and between 0 and max for foregrounds
        manual  - parallax coefficients can be defined
     */
    uint32_t        multi_parallax_type = ML_PARALLAX_AUTO;
    //! Maximal background distance
    long double     multi_parallax_auto_distance_min = -100.0l;
    //! Maximal background distance
    long double     multi_parallax_auto_distance_max = +100.0l;

    struct BgLayer
    {
        //! Name of layer (optionally, can be empty)
        PGEString   name;
        //! Background image of this layer
        PGEString   image;
        //! Z index. <0 - background, >0 - foreground
        long double z_index = -50.0l;
        //! Opacity of the layer
        double      opacity = 1.0;

        //! Flip background image Horizontally
        bool        flip_h = false;
        //! Flip background image Vertically
        bool        flip_v = false;

        //! Draw image as in-scene element
        bool        inscene_draw = false;

        /// Horizontal parallax coefficient. In auto mode will be calculated automatically proportionally to z position
        /*!
            - >1        parallax slower than section scrolling (far sky, city, forest, etc.)
            - =1        statically relative section edge, (for example, a wall in the building)
            - <1 but >0 faster than section scrolling (suggested for a foregrounds like plants, clouds, fog, etc.)
        */
        double      parallax_coefficient_x = 1.0;
        /// Vertical parallax coefficient
        /*!
            - >1        parallax slower than section scrolling
            - =1        statically relative section edge,
            - <1 but >0 faster than section scrolling
        */
        double      parallax_coefficient_y = 1.0;

        enum ParallaxModes
        {
            //! Scroll by coefficient
            P_MODE_SCROLL = 0,
            //! Don't repeat (if image is larger than camera size, otherwise will act as P_MODE_FIXED), allign image by position of camera on background proportionally to position between section edges
            P_MODE_FIT = 1,
            //! Don't move, image will be fixed on camera with no matter position in the section
            P_MODE_FIXED = 2
        };

        enum ReferencePointX
        {
            R_LEFT = 0,
            R_RIGHT = 1,
        };
        enum ReferencePointY
        {
            R_BOTTOM = 0,
            R_TOP = 1,
        };

        //! Horizontal parallax mode
        uint32_t    parallax_mode_x = P_MODE_SCROLL;
        //! Vertical parallax mode
        uint32_t    parallax_mode_y = P_MODE_FIT;

        //! Repeat horizontally
        bool        repeat_x = true;
        //! Repeat vertically
        bool        repeat_y = false;

        //! Horizontal reference point (left or right edge of the section)
        uint32_t    reference_point_x = R_LEFT;
        //! Vertical reference point (left or right edge of the section)
        uint32_t    reference_point_y = R_BOTTOM;

        //! Initial offset X at left/right section edge (dependent on horizontal reference point)
        double      offset_x = 0.0;
        //! Initial offset Y at top/bottop section edge (dependent on vertical reference point)
        double      offset_y = 0.0;
        //! Horizontal padding at right between repeating tiles
        double      padding_x_right       = 0.0;
        //! Horizontal padding at left between repeating tiles
        double      padding_x_left  = 0.0;
        //! Vertical padding at bottom between repeating tiles
        double      padding_y_bottom       = 0.0;
        //! Vertical padding at top between repeating tiles
        double      padding_y_top   = 0.0;

        //! Auto-scroll image (works when releat X is enabled)
        bool        auto_scrolling_x = false;
        //! Auto-scroll speed in pixels per second
        int32_t     auto_scrolling_x_speed = 32;

        //! Auto-scroll image (works when releat Y is enabled)
        bool        auto_scrolling_y = false;
        //! Auto-scroll speed in pixels per second
        int32_t     auto_scrolling_y_speed = 32;

        //! Is this layer animated?
        bool            animated = false;
        //! Count of animation frames of background image
        uint32_t        frames = 1;
        //! Delay between animation frames in milliseconds
        uint32_t        framespeed = 128;
        //! Height of one frame [must be calculated automatically by using real image metrics]
        uint32_t        frame_h = 1;
        //! Default frame to display when animation is disabled
        uint32_t        display_frame = 0;
        //! Custom frame sequence based on frame numbers
        IntArray        frame_sequence;
    };

    PGEList<BgLayer> layers;
};

#endif // CONFIG_BG_H
