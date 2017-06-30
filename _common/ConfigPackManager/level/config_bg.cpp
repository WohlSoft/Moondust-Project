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

#include "config_bg.h"

#include <IniProcessor/ini_processing.h>
#include <Utils/maths.h>
#include "../image_size.h"
#include "../../number_limiter.h"

#include <sstream>

bool BgSetup::parse(IniProcessing *setup, PGEString bgImgPath, uint32_t /*defaultGrid*/, BgSetup *merge_with, PGEString *error)
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
            *error = "setup IniProcessing is null!";
        return false;
    }

    section = StdToPGEString(setup->group());
    setup->read("name", name, pMerge(name, section));

    if(name.size() == 0)
    {
        if(error)
            *error = section + ": item name isn't defined";
        return false;
    }

    setup->read("fill-color", fill_color, pMerge(fill_color, "auto"));

    /*
     *  First image
     */
    setup->read("image",    image_n, pMerge(image_n, ""));
    if(!merge_with && !PGE_ImageInfo::getImageSize(bgImgPath + image_n, &w, &h, &errCode))
    {
        if(error)
        {
            switch(errCode)
            {
            case PGE_ImageInfo::ERR_UNSUPPORTED_FILETYPE:
                *error = "Unsupported or corrupted file format: " + bgImgPath + image_n;
                break;
            case PGE_ImageInfo::ERR_NOT_EXISTS:
                *error = "image file is not exist: " + bgImgPath + image_n;
                break;
            case PGE_ImageInfo::ERR_CANT_OPEN:
                *error = "Can't open image file: " + bgImgPath + image_n;
                break;
            }
        }
        return false;
    }

    if(!merge_with && ((w == 0) || (h == 0)))
    {
        if(error)
            *error = "Width or height of image has zero or negative value in image " + bgImgPath + image_n;
        return false;
    }

    setup->read("icon", icon_n, pMerge(icon_n, ""));

    setup->readEnum("type", type,
                    pMerge(type, 0),
    {
        {"single-row", BG_TYPE_SingleRow},
        {"double-row", BG_TYPE_DoubleRow},
        {"tiled", BG_TYPE_Tiled}
    });

    setup->read("repeat-h", repeat_h, pMerge(repeat_h, 2.0));
    NumberLimiter::applyD(repeat_h, 1.0, 0.0);

    setup->readEnum("repeat-v", repeat_v,
                    pMerge(repeat_v, (uint32_t)BG_REPEAT_V_NR_NoRepeat),
    {
        {"NR", BG_REPEAT_V_NR_NoRepeat},
        {"ZR", BG_REPEAT_V_ZR_NoRepeatNoParallax},
        {"RP", BG_REPEAT_V_RP_RepeatParallax},
        {"RZ", BG_REPEAT_V_RZ_RepeatNoParallax}
    });

    setup->readEnum("attached", attached,
                    pMerge(attached, (uint32_t)BG_ATTACH_TO_BOTTOM),
    {
        {"bottom", BG_ATTACH_TO_BOTTOM},
        {"top", BG_ATTACH_TO_TOP}
    });

    setup->read("tiled-in-editor", editing_tiled, pMerge(editing_tiled, false));

    /*
     *  Animation setup
     */
    setup->read("animated", animated, pMerge(animated, false));

    setup->read("frames", frames, pMerge(frames, 1));
    NumberLimiter::apply(frames, 1u);

    setup->read("frame-delay", framespeed, pMerge(framespeed, 128));
    setup->read("framespeed", framespeed, framespeed);
    NumberLimiter::apply(framespeed, 1u);

    frame_h =   animated ? Maths::uRound(double(h) / double(frames)) : h;
    NumberLimiter::apply(frame_h, 0u);

    setup->read("display-frame", display_frame, pMerge(display_frame, 0));
    NumberLimiter::apply(display_frame, 0u);

    /*
     *  Magic background
     */
    setup->read("magic", magic, pMerge(magic, false));
    setup->read("magic-strips", magic_strips, pMerge(magic_strips, 1u));
    NumberLimiter::applyD(magic_strips, 1u, 1u);
    setup->read("magic-splits", magic_splits_i, pMerge(magic_splits_i, PGEList<uint32_t>()));
    setup->read("magic-speeds", magic_speeds_i, pMerge(magic_speeds_i, PGEList<double>()));

    /*
     *  Second image
     */
    if(type == BG_TYPE_DoubleRow)
    {
        setup->read("second-image", second_image_n, pMerge(second_image_n, ""));
        setup->read("second-repeat-h", second_repeat_h, pMerge(second_repeat_h, 2.0));
        NumberLimiter::applyD(second_repeat_h, 1.0, 0.0);

        setup->readEnum("second-repeat-v",
                        second_repeat_v,
                        pMerge(second_repeat_v, (uint32_t)BG_REPEAT_V_NR_NoRepeat),
        {
            {"NR", BG_REPEAT_V_NR_NoRepeat},
            {"ZR", BG_REPEAT_V_ZR_NoRepeatNoParallax},
            {"RP", BG_REPEAT_V_RP_RepeatParallax},
            {"RZ", BG_REPEAT_V_RZ_RepeatNoParallax}
        });

        setup->readEnum("second-attached",
                        second_attached,
                        pMerge(second_attached, (uint32_t)BG_ATTACH_2_TO_TOP_OF_FIRST),
        {
            {"overfirst", BG_ATTACH_2_TO_TOP_OF_FIRST},
            {"bottom", BG_ATTACH_2_TO_BOTTOM},
            {"top", BG_ATTACH_2_TO_TOP}
        });
    }

    /*
     *  Multi-layaring background
     */
    setup->read("multi-layer", multi_layered, pMerge(multi_layered, false));
    setup->read("multi-layer-count", multi_layers_count, pMerge(multi_layers_count, 0));

    setup->readEnum("multi-layer-parallax-type",
                    multi_parallax_type,
                    pMerge(multi_parallax_type, (uint32_t)ML_PARALLAX_AUTO),
    {
        {"auto", ML_PARALLAX_AUTO},
        {"0", ML_PARALLAX_AUTO},
        {"manual", ML_PARALLAX_MANUAL},
        {"1", ML_PARALLAX_MANUAL}
    });

    setup->read("multi-layer-parallax-z-min", multi_parallax_auto_distance_min, pMerge(multi_parallax_auto_distance_min, -100.0l));
    setup->read("multi-layer-parallax-z-max", multi_parallax_auto_distance_max, pMerge(multi_parallax_auto_distance_max, -100.0l));

    if(multi_layered && (multi_layers_count > 0))
    {
        setup->endGroup();

        for(uint32_t layer = 0; layer < multi_layers_count; layer++)
        {
            BgLayer lyr;
            std::ostringstream lr_name_o;
            lr_name_o << PGEStringToStd(section) << "-layer-" << layer;
            std::string lr_name_s = lr_name_o.str();
            const char *lr_name = lr_name_s.c_str();

            setup->beginGroup(lr_name);
            {
                setup->read("name", lyr.name, lr_name);
                setup->read("image", lyr.image, "");
                setup->read("z-value",  lyr.z_index, -50.0l);
                setup->read("z-index",  lyr.z_index, lyr.z_index);//< Alias to z-value
                setup->read("priority", lyr.z_index, lyr.z_index);//< Alias to z-value
                setup->read("repeat-x", lyr.repeat_x, true);
                setup->read("repeat-y", lyr.repeat_y, false);
                setup->read("parallax-coefficient-x", lyr.parallax_coefficient_x, 1.0);
                setup->read("parallax-coefficient-y", lyr.parallax_coefficient_y, 1.0);
                setup->readEnum("parallax-mode-x",
                                lyr.parallax_mode_x,
                                (uint32_t)BgLayer::P_MODE_SCROLL,
                {
                    {"scroll", BgLayer::P_MODE_SCROLL},
                    {"fit", BgLayer::P_MODE_FIT},
                    {"fixed", BgLayer::P_MODE_FIXED}
                });
                setup->readEnum("parallax-mode-y",
                                lyr.parallax_mode_y,
                                (uint32_t)BgLayer::P_MODE_FIT,
                {
                    {"scroll", BgLayer::P_MODE_SCROLL},
                    {"fit", BgLayer::P_MODE_FIT},
                    {"fixed", BgLayer::P_MODE_FIXED}
                });
                setup->readEnum("reference-point-x",
                                lyr.reference_point_x,
                                (uint32_t)BgLayer::R_LEFT,
                {
                    {"left", BgLayer::R_LEFT},
                    {"right", BgLayer::R_RIGHT}
                });
                setup->readEnum("reference-point-y",
                                lyr.reference_point_y,
                                (uint32_t)BgLayer::R_BOTTOM,
                {
                    {"bottom", BgLayer::R_BOTTOM},
                    {"top", BgLayer::R_TOP}
                });
                setup->read("offset-x", lyr.offset_x, 0.0);
                setup->read("offset-y", lyr.offset_y, 0.0);
                setup->read("padding-x", lyr.padding_x, 0.0);
                NumberLimiter::applyD(lyr.padding_x, 0.0, 0.0);
                setup->read("padding-y", lyr.padding_y, 0.0);
                NumberLimiter::applyD(lyr.padding_y, 0.0, 0.0);

                setup->read("auto-scroll-x", lyr.auto_scrolling_x, false);
                setup->read("auto-scroll-x-speed", lyr.auto_scrolling_x_speed, 32);
                setup->read("auto-scroll-y", lyr.auto_scrolling_y, false);
                setup->read("auto-scroll-y-speed", lyr.auto_scrolling_y_speed, 32);

                setup->read("animated", lyr.animated, false);
                setup->read("frames", lyr.frames, 1);
                setup->read("frame-delay", lyr.framespeed, 128);
                setup->read("framespeed", lyr.framespeed, lyr.framespeed);
                setup->read("display-frame", lyr.display_frame, 0);
                setup->read("frame-sequence", lyr.frame_sequence);
            }
            setup->endGroup();

            layers.push_back(lyr);
        }
    }
    else
    {
        //Import pre-loaded layers from default config
        if(merge_with)
            layers = merge_with->layers;
    }

#undef pMerge
#undef pMergeMe
    return true;
}
