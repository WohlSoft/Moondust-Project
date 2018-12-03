/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "config_bg.h"

#include <IniProcessor/ini_processing.h>
#include <Utils/maths.h>
#include "../image_size.h"
#include "../../number_limiter.h"

#include <sstream>
#include <algorithm>

bool BgSetup::parse(IniProcessing *setup,
                    PGEString bgImgPath,
                    uint32_t /*defaultGrid*/,
                    const BgSetup *merge_with,
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

    setup->read("frames",       frames, pMerge(frames, 1));//Real
    pAlias("framecount",        frames);
    pAlias("frame-count",       frames);
    NumberLimiter::apply(frames, 1u);

    setup->read("frame-delay", framespeed, pMerge(framespeed, 128));
    setup->read("framespeed", framespeed, framespeed);
    NumberLimiter::apply(framespeed, 1u);

    frame_h =   animated ? Maths::uRound(double(h) / double(frames)) : h;
    NumberLimiter::apply(frame_h, 0u);

    setup->read("display-frame", display_frame, pMerge(display_frame, 0));
    NumberLimiter::apply(display_frame, 0u);

    /*
     *  Segmented background
     */
    setup->read("segmented", segmented, pMerge(segmented, false));
    setup->read("segmented-strips", segmented_strips, pMerge(segmented_strips, 1u));
    setup->read("segmented-splits", segmented_splits, pMerge(segmented_splits, PGEList<uint32_t>()));
    setup->read("segmented-speeds", segmented_speeds, pMerge(segmented_speeds, PGEList<double>()));

    // Aliases to obsolete titles
    setup->read("magic", segmented, segmented);
    setup->read("magic-strips", segmented_strips, segmented_strips);
    setup->read("magic-splits", segmented_splits, segmented_splits);
    setup->read("magic-speeds", segmented_speeds, segmented_speeds);
    NumberLimiter::applyD(segmented_strips, 1u, 1u);

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
    setup->read("legacy", use_legacy_bg_engine, false);

    //Required for "nested" config file. Makes no sense in singleton configs
    setup->read("multi-layer-count", multi_layers_count, pMerge(multi_layers_count, 0));
    setup->read("focus", multi_parallax_focus, pMerge(multi_parallax_focus, 200.0l));
    setup->read("multi-layer-parallax-focus", multi_parallax_focus, multi_parallax_focus);

    if(!merge_with)
        layers.clear();

    if(!use_legacy_bg_engine)
    {
        layers.clear();
        setup->endGroup();

        std::vector<std::string> all_layers;

        if((section == "General") || (section == "background2"))
        {
            // Process layers for singetons backgrounds
            all_layers = setup->childGroups();
            std::sort(all_layers.begin(), all_layers.end());//Sort alphabetically
        }
        else
        {
            // Process layers for nested background config
            all_layers.reserve(size_t(multi_layers_count));
            for(uint32_t l = 0; l < multi_layers_count; l++)
            {
                std::ostringstream lr_name_o;
                lr_name_o << PGEStringToStd(section) << "-layer-" << l;
                all_layers.push_back(lr_name_o.str());
            }
        }

        for(std::string &layer_section : all_layers)
        {
            if(StdToPGEString(layer_section) == section)
                continue;//Skip header section

            BgLayer lyr;

            const char *lr_name = layer_section.c_str();
            setup->beginGroup(lr_name);
            {
                setup->read("name",  lyr.name, lr_name);
                setup->read("image", lyr.image, "");
                pAlias("img", lyr.image);

                long double depth = static_cast<long double>(std::numeric_limits<double>::max());
                {
                    int depth_mode = 0;
                    setup->readEnum("depth",
                                    depth_mode,
                                    0, {
                                        {"INFINITE", +1},
                                        {"MAX", +1},
                                        {"MIN", -1}
                                    });
                    switch(depth_mode)
                    {
                    case +1:
                        depth = static_cast<long double>(std::numeric_limits<double>::max());
                        break;
                    case -1:
                        depth = -multi_parallax_focus * 0.99999999999999888977697537484l;
                        break;
                    default:
                        setup->read("depth",    depth, depth);
                        break;
                    }
                }
                setup->read("opacity",  lyr.opacity, 1.0);
                NumberLimiter::applyD(lyr.opacity, 1.0, 0.0, 1.0);//Opacity can be between 0.0 and 1.0
                setup->read("flip-h",  lyr.flip_h, false);
                setup->read("flip-v",  lyr.flip_v, false);
                setup->read("in-scene-draw",  lyr.inscene_draw, false);
                setup->read("in-world-draw",  lyr.inscene_draw, lyr.inscene_draw);
                setup->read("repeat-x", lyr.repeat_x, true);
                setup->read("repeat-y", lyr.repeat_y, false);
                pAlias("repeatx", lyr.repeat_x);
                pAlias("repeaty", lyr.repeat_y);

                IniProcessing::StrEnumMap pMode = {
                    {"scroll", BgLayer::P_MODE_SCROLL},
                    {"fit", BgLayer::P_MODE_FIT},
                    {"fixed", BgLayer::P_MODE_FIXED}
                };
                setup->readEnum("parallax-mode-x",
                                lyr.parallax_mode_x,
                                (uint32_t)BgLayer::P_MODE_SCROLL, pMode);
                setup->readEnum("parallax-mode-y",
                                lyr.parallax_mode_y,
                                (uint32_t)BgLayer::P_MODE_FIT, pMode);

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

                double parallaxX = 1.0, parallaxY = 1.0;
                if(!lyr.inscene_draw)
                {
                    if(lyr.z_index == 0.0l)
                    {
                        parallaxX = 0.0;
                        parallaxY = 0.0;
                    }
                    else
                    {
                        if(lyr.z_index == 0.0l)
                        {
                            lyr.parallax_mode_x = BgLayer::P_MODE_FIXED;
                            lyr.parallax_mode_y = BgLayer::P_MODE_FIXED;
                        }
                        else
                        {
                            long double d = depth / multi_parallax_focus;
                            if(d <= -1.0l)
                            {
                                lyr.parallax_mode_x = BgLayer::P_MODE_FIXED;
                                lyr.parallax_mode_y = BgLayer::P_MODE_FIXED;
                                lyr.opacity = 0.0;//Layer is behind the "camera"
                            } else {
                                d += 1.0l;
                                d = 1.0l / (d * d);
                                parallaxX = parallaxY = static_cast<double>(d);
                            }
                        }
                    }
                }

                setup->read("z-value",   lyr.z_index, -depth);
                setup->read("z-index",   lyr.z_index, lyr.z_index);//< Alias to z-value
                setup->read("z-position",lyr.z_index, lyr.z_index);//< Alias to z-value
                setup->read("priority",  lyr.z_index, lyr.z_index);//< Alias to z-value

                double parallaxX_1 = parallaxX, parallaxY_1 = parallaxY;
                setup->read("parallax-coefficient-x", parallaxX_1, parallaxX == 0.0 ? 0.0 : 1.0 / parallaxX);
                setup->read("parallax-coefficient-y", parallaxY_1, parallaxY == 0.0 ? 0.0 : 1.0 / parallaxY);
                setup->read("parallax-x", lyr.parallax_x, parallaxX_1 == 0.0 ? 0.0 : 1.0 / parallaxX_1);
                setup->read("parallax-y", lyr.parallax_y, parallaxY_1 == 0.0 ? 0.0 : 1.0 / parallaxY_1);
                pAlias("parallaxx", lyr.parallax_x);
                pAlias("parallaxy", lyr.parallax_y);
                NumberLimiter::applyD(lyr.parallax_x, 0.0, 0.0);
                if((lyr.parallax_mode_x == BgLayer::P_MODE_SCROLL) && (lyr.parallax_x == 0.0))
                    lyr.parallax_mode_x = BgLayer::P_MODE_FIXED;

                NumberLimiter::applyD(lyr.parallax_y, 0.0, 0.0);
                if((lyr.parallax_mode_y == BgLayer::P_MODE_SCROLL) && (lyr.parallax_y == 0.0))
                    lyr.parallax_mode_y = BgLayer::P_MODE_FIXED;

                setup->read("offset-x", lyr.offset_x, 0.0);
                setup->read("offset-y", lyr.offset_y, 0.0);
                pAlias("x", lyr.offset_x);
                pAlias("y", lyr.offset_y);

                double summarMarginH = 0.0, summarMarginV = 0.0;
                setup->read("margin-x", summarMarginH, 0.0);
                setup->read("margin-y", summarMarginV, 0.0);
                setup->read("margin-x-right", lyr.margin_x_right, summarMarginH);
                NumberLimiter::applyD(lyr.margin_x_right, 0.0, 0.0);
                setup->read("margin-x-left", lyr.margin_x_left, summarMarginH);
                NumberLimiter::applyD(lyr.margin_x_left, 0.0, 0.0);
                setup->read("margin-y-bottom", lyr.margin_y_bottom, summarMarginV);
                NumberLimiter::applyD(lyr.margin_y_bottom, 0.0, 0.0);
                setup->read("margin-y-top", lyr.margin_y_top, summarMarginV);
                NumberLimiter::applyD(lyr.margin_y_top, 0.0, 0.0);

                setup->read("padding-x", lyr.padding_horizontal, 0.0);
                setup->read("padding-y", lyr.padding_vertical, 0.0);

                setup->read("speed-x", lyr.auto_scrolling_x_speed, 0);
                pAlias("auto-scroll-speed-x", lyr.auto_scrolling_x_speed);
                pAlias("speedx", lyr.auto_scrolling_x_speed);
                lyr.auto_scrolling_x = (lyr.auto_scrolling_x_speed != 0);

                setup->read("speed-y", lyr.auto_scrolling_y_speed, 0);
                pAlias("auto-scroll-speed-y", lyr.auto_scrolling_y_speed);
                pAlias("speedy", lyr.auto_scrolling_y_speed);
                lyr.auto_scrolling_y = (lyr.auto_scrolling_y_speed != 0);

                setup->read("frames", lyr.frames, 1);//Real
                pAlias("framecount",    frames);
                pAlias("frame-count",   frames);
                lyr.animated = (lyr.frames > 1);
                setup->read("frame-delay", lyr.framespeed, 128);
                pAlias("framespeed", lyr.framespeed);
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
#undef pAlias
    return true;
}
