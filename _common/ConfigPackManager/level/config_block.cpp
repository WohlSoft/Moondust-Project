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

#include "config_block.h"

#include <IniProcessor/ini_processing.h>
#include "../image_size.h"
#include "../../number_limiter.h"
#include "../../csv_2_number_array.h"

#include <assert.h>

bool BlockSetup::parse(IniProcessing *setup,
                       PGEString blockImgPath,
                       uint32_t defaultGrid,
                       const BlockSetup *merge_with,
                       PGEString *error)
{
    #define pMerge(param, def) (merge_with ? pgeConstReference(merge_with->param) : pgeConstReference(def))
    #define pMergeMe(param) (merge_with ? pgeConstReference(merge_with->param) : pgeConstReference(param))
    #define pAlias(paramName, destValue) setup->read(paramName, destValue, destValue)

    int errCode = PGE_ImageInfo::ERR_OK;
    PGEString   section;
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

    section = StdToPGEString(setup->group());
    setup->read("name", name, pMerge(name, section));

    if(name.size() == 0)
    {
        if(error)
            *error = section + ": Item name isn't defined";
        return false;
    }

    setup->read("group",    group,      pMergeMe(group));
    setup->read("category", category,   pMergeMe(category));
    setup->read("description", description, pMerge(description, ""));
    setup->read("grid",     grid,       pMerge(grid, defaultGrid));

    setup->read("image",    image_n,    pMerge(image_n, ""));
    if(!PGE_ImageInfo::getImageSize(blockImgPath + image_n, &w, &h, &errCode) && !merge_with)
    {
        if(error)
        {
            switch(errCode)
            {
            case PGE_ImageInfo::ERR_UNSUPPORTED_FILETYPE:
                *error = "Unsupported or corrupted file format: " + blockImgPath + image_n;
                break;

            case PGE_ImageInfo::ERR_NOT_EXISTS:
                *error = "image file is not exist: " + blockImgPath + image_n;
                break;

            case PGE_ImageInfo::ERR_CANT_OPEN:
                *error = "Can't open image file: " + blockImgPath + image_n;
                break;
            }
        }

        return false;
    }
    assert(merge_with || ((w > 0) && (h > 0) && "Width or height of image has zero or negative value!"));
    mask_n = PGE_ImageInfo::getMaskName(image_n);

    setup->read("icon", icon_n, pMerge(icon_n, ""));

    setup->read("sizable",                      sizable,                    pMerge(sizable, false));
    setup->read("sizable-border-width",         sizable_border_width,       pMerge(sizable_border_width, -1));
    setup->read("sizable-border-width-left",    sizable_border_width_left,  pMerge(sizable_border_width_left, -1));
    setup->read("sizable-border-width-top",     sizable_border_width_top,   pMerge(sizable_border_width_top, -1));
    setup->read("sizable-border-width-right",   sizable_border_width_right, pMerge(sizable_border_width_right, -1));
    setup->read("sizable-border-width-bottom",  sizable_border_width_bottom,pMerge(sizable_border_width_bottom, -1));

    setup->read("danger",                   danger,                 pMerge(danger, 0));
    setup->read("collision",                collision,              pMerge(collision, 1));
    setup->read("slope-slide",              slopeslide,             pMerge(slopeslide, 0));
    setup->read("shape-type",               phys_shape,             pMerge(phys_shape, 0));
    setup->read("lava",                     lava,                   pMerge(lava, false));
    setup->read("destroyable",              destroyable,            pMerge(destroyable, false));
    setup->read("destroyable-by-bomb",      destroyable_by_bomb,    pMerge(destroyable_by_bomb, false));
    setup->read("destroyable-by-fireball",  destroyable_by_fireball,pMerge(destroyable_by_fireball, false));

    {
        std::string spawnMe;
        setup->read("spawn-on-destroy", spawnMe, "");
        if(!spawnMe.empty())
        {
            spawn = false;
            spawn_obj = 0;
            spawn_obj_id = 0;
            bool r = false;
            std::string left;
            std::string right;
            for(char &c : spawnMe)
            {
                if(r)
                {
                    right.push_back(c);
                }
                else
                {
                    if(c == '-')
                    {
                        r = true;
                        continue;
                    }
                    left.push_back(c);
                }
            }
            IniProcessing::StrEnumMap map =
            {
                {"npc", SPAWN_NPC},
                {"block", SPAWN_Block},
                {"bgo", SPAWN_BGO}
            };
            if(map.find(left) != map.end())
            {
                spawn = true;
                spawn_obj = map[left];
                spawn_obj_id = strtoul(right.c_str(), NULL, 10);
            }
        } else {
            spawn =         pMerge(spawn, 0);
            spawn_obj =     pMerge(spawn_obj, 0);
            spawn_obj_id =  pMerge(spawn_obj_id, 0);
        }
    }

    setup->read("destroy-effect",   effect,  pMerge(effect, 1u));
    setup->read("bounce",           bounce,  pMerge(bounce, false));
    setup->read("bumpable",         bumpable, pMerge(bumpable, false));
    pAlias("hitable",               bumpable);
    setup->read("transform-onhit-into", transfororm_on_hit_into, pMerge(transfororm_on_hit_into, 2u));

    static unsigned int switchID = 0;
    setup->read("switch-button",    switch_Button,      pMerge(switch_Button, false));
    setup->read("switch-block",     switch_Block,       pMerge(switch_Block, false));
    setup->read("switch-id",        switch_ID,          pMerge(switch_ID, switchID++));
    setup->read("switch-transform", switch_transform,   pMerge(switch_transform, 1));

    setup->read("player-switch-button",     plSwitch_Button,    pMerge(plSwitch_Button, false));
    setup->read("player-switch-button-id",  plSwitch_Button_id, pMerge(plSwitch_Button_id, 0));
    plSwitch_frames_true.clear();
    plSwitch_frames_false.clear();
    frame_sequence.clear();

    if(plSwitch_Button)
    {
        setup->read("player-switch-frames-true", plSwitch_frames_true,
                    pMergeMe(plSwitch_frames_true));
        setup->read("player-switch-frames-false", plSwitch_frames_false,
                    pMergeMe(plSwitch_frames_false));
        frame_sequence = plSwitch_frames_false;
    }

    setup->read("player-filter-block",      plFilter_Block,     pMerge(plSwitch_Button, false));
    setup->read("player-filter-block-id",   plFilter_Block_id,  pMerge(plFilter_Block_id, 0u));
    plFilter_frames_true.clear();
    plFilter_frames_false.clear();

    if(plFilter_Block)
    {
        setup->read("player-filter-frames-true", plFilter_frames_true,
                    pMergeMe(plFilter_frames_true));
        setup->read("player-filter-frames-false", plFilter_frames_false,
                    pMergeMe(plFilter_frames_false));
        frame_sequence = plFilter_frames_false;
    }

    {
        IniProcessing::StrEnumMap zLayers = {
            {"background", 0},
            {"background1", 0},
            {"foreground", 1},
            {"foreground1", 1}
        };
        setup->readEnum("z-layer",  z_layer, pMerge(z_layer, 0), zLayers);
        setup->readEnum("view",     z_layer, z_layer, zLayers);//DEPRECATED
    }

    setup->read("animation-reverse",        animation_rev,  pMerge(animation_rev, false)); //Reverse animation
    setup->read("animation-bidirectional",  animation_bid,  pMerge(animation_bid, false)); //Bidirectional animation
    setup->read("frames",                   frames,         pMerge(frames, 1));//Real
    pAlias("framecount",    frames);//Alias
    pAlias("frame-count",   frames);//Alias
    NumberLimiter::apply(frames, 1u);
    animated = (frames > 1u);
    setup->read("frame-delay", framespeed, pMerge(framespeed, 125));//Real
    pAlias("frame-speed", framespeed);//Alias
    if(setup->hasKey("framespeed"))
    {
        pAlias("framespeed",  framespeed);//Alias
        framespeed = (framespeed * 1000u) / 65u;//Convert 1/65'th into milliseconds
    }
    NumberLimiter::apply(framespeed, 1u);
    setup->read("hit-sound-id",             hit_sound_id,   pMerge(hit_sound_id, 0));
    NumberLimiter::apply(hit_sound_id, 0u);
    setup->read("destroy-sound-id",         destroy_sound_id,pMerge(destroy_sound_id, 0));
    NumberLimiter::apply(destroy_sound_id, 0u);
    frame_h = (animated ? (h / frames) : h);

    //Custom frame sequence for regular blocks
    if(!plSwitch_Button && !plFilter_Block)
    {
        setup->read("frame-sequence", frame_sequence, pMergeMe(frame_sequence));
    }

    setup->read("display-frame", display_frame, 0);

    int64_t iTmp;
    setup->read("default-invisible", iTmp, pMerge(default_invisible, -1));
    default_invisible = (iTmp >= 0);
    default_invisible_value = (iTmp >= 0) ? bool(iTmp) : false;

    setup->read("default-slippery", iTmp, pMerge(default_slippery, -1));
    default_slippery = (iTmp >= 0);
    default_slippery_value  = (iTmp >= 0) ? bool(iTmp) : false;

    setup->read("default-npc-content", iTmp, pMerge(default_content, -1));
    default_content =           (iTmp >= 0);
    default_content_value   =   (iTmp >= 0) ? (iTmp < 1000 ? iTmp * -1 : iTmp - 1000) : 0;

    #undef pMerge
    #undef pMergeMe
    #undef pAlias
    return true;
}
