/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef CONFIG_BG_H
#define CONFIG_BG_H

#include <PGEString.h>
#include <stdint.h>

class IniProcessing;

struct BgSetup
{
    /**
     * @brief Read data from the external INI file and fill the config
     * @param setup [_in] Instance of opened INI file processor
     * @param bgImgPath [_in] Folder path where images are stored
     * @param defaultGrid [_in] Default grid size
     * @param merge_with [_in] Another element config to use as source of default values
     * @param error [_out] Output string for error messages writing
     * @return
     */
    bool parse(IniProcessing *setup,
               PGEString bgImgPath,
               uint32_t defaultGrid,
               const BgSetup *merge_with = nullptr,
               PGEString *error = nullptr);

    /**
     * @brief Legacy BG: background type
     */
    enum BgType
    {
        //! Single-row background
        BG_TYPE_SingleRow = 0,
        //! Double-row background. Requiers usage of second image
        BG_TYPE_DoubleRow = 1,
        //! Background will be repeated in both horizontal and vertical directions
        BG_TYPE_Tiled = 2
    };

    /**
     * @brief Legacy BG: Vertical repeating algorithm
     */
    enum BgReleatVertical
    {
        //! Don't repeat vertically, turn on parallax
        BG_REPEAT_V_NR_NoRepeat = 0,
        //! Don't repeat vertically, turn off parallax
        BG_REPEAT_V_ZR_NoRepeatNoParallax = 1,
        //! Repeat vertically with parallax
        BG_REPEAT_V_RP_RepeatParallax,
        //! Repeat vertically without parallax
        BG_REPEAT_V_RZ_RepeatNoParallax
    };

    /**
     * @brief Legacy BG: Vertical side attach
     */
    enum BgAttach
    {
        //! Attach background picture to bottom of the section
        BG_ATTACH_TO_BOTTOM = 0,
        //! Attach background picture to top of the section
        BG_ATTACH_TO_TOP = 1
    };

    /**
     * @brief Legacy BG: Second image vertical side attach (double-row only)
     */
    enum BgAttachSecond
    {
        //! Attach second background image to top of the first image
        BG_ATTACH_2_TO_TOP_OF_FIRST = 0,
        //! Attach second background image to bottom of the section
        BG_ATTACH_2_TO_BOTTOM = 1,
        //! Attach second background image to top of the section
        BG_ATTACH_2_TO_TOP = 2
    };

    //! Integer array
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
     *  First image (Single-row, Tiled and Double-row background)
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
     *  Animation setup (Single-row and first row of double-row background only)
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
     *  Segmented background
     */
    /// Turn on "segmented" background.
    /*!
        Segmented background allows to split background image to horizontal
        stripses are have different parallax coefficients
    */
    bool            segmented = false;
    //! Count of stripses
    uint32_t        segmented_strips = 1u;
    //! Decoded set of Y coordinates of split lines
    PGEList<uint32_t> segmented_splits;
    //! Decoded set of parallax coefficients for every strip line (1/speed)
    PGEList<double> segmented_speeds;

    /*
     *  Second image (Doble-row backgrounds only)
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
    //! Turn on legacy background engine without multilayer support. Has exception: it is not inheritable in custom configs.
    bool            use_legacy_bg_engine = false;
    //! Count of layers
    uint32_t        multi_layers_count = 1;
    //! Focus value
    long double     multi_parallax_focus = 200.0l;

    struct BgLayer
    {
        //! Name of layer (optionally, can be empty)
        PGEString   name;
        //! Background image of this layer
        PGEString   image;
        //! Z index. <0 - background, >0 - foreground
        long double z_index = -100.0l;
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
            - =0        fixed position on the camera with no matter position in the section
            - <1        parallax slower than section scrolling
            - =1        statically relative section edge,
            - >1        faster than section scrolling (suggested for a foregrounds like plants, clouds, fog, etc.)
        */
        double      parallax_x = 1.0;
        /// Vertical parallax coefficient
        /*!
            - =0        fixed position on the camera with no matter position in the section
            - <1        parallax slower than section scrolling
            - =1        statically relative section edge,
            - >1        faster than section scrolling
        */
        double      parallax_y = 1.0;

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
            R_TOP = 0,
            R_BOTTOM = 1,
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

        //! Horizontal margin at right of every tile
        double      margin_x_right       = 0.0;
        //! Horizontal margin at left of every tile
        double      margin_x_left  = 0.0;
        //! Vertical margin at bottom of every tile
        double      margin_y_bottom       = 0.0;
        //! Vertical margin at top of every tile
        double      margin_y_top   = 0.0;

        //! Horizontal padding between repeating tiles
        double      padding_horizontal   = 0.0;
        //! Vertical padding between repeating tiles
        double      padding_vertical   = 0.0;

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

    //! Multi-layer background layers
    PGEList<BgLayer> layers;
};

#endif // CONFIG_BG_H
