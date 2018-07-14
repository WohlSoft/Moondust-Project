/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "config_npc.h"
#include <IniProcessor/ini_processing.h>
#include <cmath>
#include <assert.h>
#include <PGE_File_Formats/npc_filedata.h>
#include <Utils/maths.h>

#include "../image_size.h"
#include "../../number_limiter.h"
#include "../../csv_2_number_array.h"

bool NpcSetup::parse(IniProcessing *setup,
                     PGEString npcImgPath,
                     uint32_t defaultGrid,
                     const NpcSetup *merge_with,
                     PGEString *error)
{
    #define pMerge(param, def) (merge_with ? pgeConstReference(merge_with->param) : pgeConstReference(def))
    #define pMergeMe(param) (merge_with ? pgeConstReference(merge_with->param) : pgeConstReference(param))
    #define pAlias(paramName, destValue) setup->read(paramName, destValue, destValue)

    int errCode = PGE_ImageInfo::ERR_OK;
    PGEString section;
    /*************Buffers*********************/
    uint32_t defGFX_h = 0;
    int combobox_size = 0;

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
            *error = section + " Item name isn't defined";
        return false;
    }

    setup->read("group", group, pMergeMe(group));
    setup->read("category", category, pMergeMe(category));
    setup->read("description", description, pMerge(description, ""));

    setup->read("image", image_n, pMergeMe(image_n));
    if(!merge_with && !PGE_ImageInfo::getImageSize(npcImgPath + image_n, &gfx_w, &gfx_h, &errCode))
    {
        if(error)
        {
            switch(errCode)
            {
            case PGE_ImageInfo::ERR_UNSUPPORTED_FILETYPE:
                *error = "Unsupported or corrupted file format: " + npcImgPath + image_n;
                break;

            case PGE_ImageInfo::ERR_NOT_EXISTS:
                *error = "image file is not exist: " + npcImgPath + image_n;
                break;

            case PGE_ImageInfo::ERR_CANT_OPEN:
                *error = "Can't open image file: " + npcImgPath + image_n;
                break;
            }
        }

        return false;
    }

    assert(merge_with || ((gfx_w > 0) && (gfx_h > 0) && "Width or height of image has zero or negative value!"));
    mask_n = PGE_ImageInfo::getMaskName(image_n);

    setup->read("icon", icon_n, pMerge(icon_n, ""));

    setup->read("algorithm",        algorithm_script,   pMerge(algorithm_script, (section + ".lua")));
    setup->read("default-effect",   effect_1,           pMerge(effect_1, 10u));
    setup->read("shell-effect",     effect_2,           pMerge(effect_2, 10u));

    //Physics
    setup->read("physical-height",  height, pMerge(height, 0));
    NumberLimiter::apply(height, 1u);
    setup->read("physical-width",   width, pMerge(width, 0));
    NumberLimiter::apply(width, 1u);
    setup->read("block-npc",        block_npc, pMerge(block_npc, false));
    setup->read("block-npc-top",    block_npc_top, pMerge(block_npc_top, false));
    setup->read("block-player",     block_player, pMerge(block_player, false));

    setup->read("block-player-top", block_player_top, pMerge(block_player_top, false));
    setup->read("collision-blocks", collision_with_blocks, pMerge(collision_with_blocks, false));
    setup->read("gravity",          gravity, pMerge(gravity, false));
    setup->read("adhesion",         adhesion,   pMerge(adhesion, false));
    setup->read("contact-padding",  contact_padding, pMerge(contact_padding, 0.0));
    setup->read("container",        container, pMerge(container, false));
    setup->read("contents-id",      contents_id, pMerge(contents_id, 0));

    setup->read("container-elastic",            container_elastic, pMerge(container_elastic, false));
    setup->read("container-elastic-border-w",   container_elastic_border_w, pMerge(container_elastic_border_w, 4));
    NumberLimiter::apply(container_elastic_border_w, 0u);
    setup->read("container-show-contents",      container_show_contents, pMerge(container_show_contents, false));
    setup->read("container-content-z-offset",   container_content_z_offset, pMerge(container_content_z_offset, -0.00001));
    setup->read("container-crop-contents",      container_crop_contents, pMerge(container_crop_contents, false));
    setup->read("container-align-contents",     container_align_contents, pMerge(container_align_contents, 0));
    setup->read("no-npc-collisions",            no_npc_collisions, pMerge(no_npc_collisions, false));

    //Graphics
    setup->read("gfx-offset-x",     gfx_offset_x,   pMerge(gfx_offset_x, 0));
    setup->read("gfx-offset-y",     gfx_offset_y,   pMerge(gfx_offset_y, 0));
    setup->read("frame-style",      framestyle,     pMerge(framestyle, 0));
    NumberLimiter::apply(framestyle, 0u, 4u);
    setup->read("frames",       frames,         pMerge(frames, 1));//Real
    pAlias("framecount",        frames);//Alias
    pAlias("frame-count",       frames);//Alias
    NumberLimiter::apply(frames, 1u);
    /****************Calculating of default frame height******************/
    defGFX_h = gfx_h / (frames * static_cast<unsigned int>(std::pow(2.0, static_cast<double>(framestyle))));
    /****************Calculating of default frame height**end*************/
    setup->read("physics-to-gfx",   custom_physics_to_gfx, pMerge(custom_physics_to_gfx, true));
    setup->read("gfx-height",       gfx_h, pMerge(gfx_h, defGFX_h));
    NumberLimiter::apply(gfx_h, 1u);
    setup->read("gfx-width",        gfx_w, pMerge(gfx_w, gfx_w));
    NumberLimiter::apply(gfx_w, 1u);

    setup->read("frame-delay", framespeed, pMerge(framespeed, 125));//Real
    pAlias("frame-speed", framespeed);//Alias
    if(setup->hasKey("framespeed"))
    {
        pAlias("framespeed", framespeed);//Alias
        framespeed = (framespeed * 1000u) / 65u;//Convert 1/65'th into milliseconds
    }
    setup->read("display-frame",            display_frame,  pMerge(display_frame, false));
    setup->read("foreground",               foreground,     pMerge(foreground, false));
    setup->read("background",               background,     pMerge(background, false));
    setup->read("z-offset",                 z_offset,       pMerge(z_offset, 0.0));
    setup->read("animation-directed-direction", ani_directed_direct, pMerge(ani_directed_direct, false));
    setup->read("animation-direction",      ani_direct,     pMerge(ani_direct, false));
    setup->read("animation-bidirectional",  ani_bidir,      pMerge(ani_bidir, false));

    // TODO: Remove those fields completely
    custom_ani_fl = 0;
    custom_ani_el = -1;
    custom_ani_fr = 0;
    custom_ani_er = -1;
    custom_animate = false;
    custom_ani_alg = 0;
    // ====================================

    /*************Build custom animation settings***************/
    if( setup->hasKey("editor-animation-sequence") ||
        setup->hasKey("ani-frames-cmn") ||
        setup->hasKey("editor-animation-sequence-left") ||
        setup->hasKey("ani-frames-left")||
        setup->hasKey("editor-animation-sequence-right") ||
        setup->hasKey("ani-frames-right")
            )
    {
        custom_animate = true;
        custom_ani_alg = 2;
        /*
         * Avoid accidental clean-up made by custom config file
         * by checking existing of "custom-animation-alg" key
         */
        frames_left.clear();
        frames_right.clear();
        IntArray common;
        setup->read("editor-animation-sequence", common, common); // Common frames list
        setup->read("ani-frames-cmn", common, common); // Common frames list
        setup->read("editor-animation-sequence-left", frames_left, common); //left direction
        setup->read("ani-frames-left", frames_left, frames_left); //left direction
        setup->read("editor-animation-sequence-right", frames_right, common); //right direction
        setup->read("ani-frames-right", frames_right, frames_right); //right direction
        if(!common.empty())
        {
            uint32_t framesOffset = ((framestyle > 0)? 1 : 0) * frames;
            for(pge_size_t i = 0; i < frames_right.size(); i++)
                frames_right[i] += framesOffset;
        }

        if(frames_left.size() > 0)
        {
            custom_ani_fl = frames_left.front();
            custom_ani_el = frames_left.back();
        }
        if(frames_right.size() > 0)
        {
            custom_ani_fr = frames_right.front();
            custom_ani_er = frames_right.back();
        }
    }
    /*************Build custom animation settings**end**********/

    /***************GRID And snap*********************************/
    setup->read("grid", grid, pMerge(grid, defaultGrid));
    NumberLimiter::apply(grid, 1u);
    setup->read("grid-attachement-style", grid_attach_style, 0);
    NumberLimiter::apply(grid_attach_style, 0u);

    /***************Calculate the grid offset********************/
    if(width >= grid)
        grid_offset_x = -1 * Maths::iRound(static_cast<double>(width % grid) / 2);
    else
        grid_offset_x = Maths::iRound(static_cast<double>(static_cast<int>(grid) - static_cast<int>(width)) / 2.0);

    if(grid_attach_style == 1)
        grid_offset_x += (grid / 2);

    grid_offset_y = -static_cast<int>(height) % static_cast<int>(grid);
    /***************Calculate the grid offset********************/

    /*************Manual redefinition of the grid offset if not set******************/
    setup->read("grid-offset-x", grid_offset_x, pMergeMe(grid_offset_x));
    setup->read("grid-offset-y", grid_offset_y, pMergeMe(grid_offset_y));
    /*************Manual redefinition of the grid offset if not set******************/

    /***************GRID And snap***end***************************/
    setup->read("have-special", special_option, pMerge(special_option, false));
    setup->read("special-name", special_name,   pMerge(special_name, "Special option value"));
    setup->read("special-type", special_type,   pMerge(special_type, 1));

    setup->read("special-combobox-size", combobox_size, 0);
    if((combobox_size <= 0) && merge_with)
        special_combobox_opts = merge_with->special_combobox_opts;
    else
    {
        /*************Build special value combobox***********/
        special_combobox_opts.clear();

        for(int j = 0; j < combobox_size; j++)
        {
            std::string key = "special-option-" + std::to_string(j);
            std::string value;
            setup->read(key.c_str(), value, "");
            special_combobox_opts.push_back(StdToPGEString(value));
        }
    }

    setup->read("special-spin-min", special_spin_min, pMerge(special_spin_min, 0));
    setup->read("special-spin-max", special_spin_max, pMerge(special_spin_max, 1));
    setup->read("special-spin-value-offset", special_spin_value_offset, pMerge(special_spin_value_offset, 0));

    /**LEGACY, REMOVE THIS LATER!!!***/
    setup->read("have-special-2", special_option_2, pMerge(special_option_2, 0));
    setup->read("special-2-name", special_2_name, pMerge(special_2_name, "Special option value"));

    if(special_option_2)
    {
        setup->read("special-2-npc-spin-required",
                    special_2_npc_spin_required,
                    pMerge(special_2_npc_spin_required, special_2_npc_spin_required));
        setup->read("special-2-npc-box-required",
                    special_2_npc_box_required,
                    pMerge(special_2_npc_box_required, special_2_npc_box_required));
    }

    setup->read("special-2-type", special_2_type, 1);
    setup->read("special-2-combobox-size", combobox_size, 0);
    for(int j = 0; j < combobox_size; j++)
    {
        PGEString value;
        setup->read(("special-2-option-" + std::to_string(j)).c_str(), value, "0");
        special_2_combobox_opts.push_back(value);
    }

    setup->read("special-2-spin-min", special_2_spin_min, 0);
    setup->read("special-2-spin-max", special_2_spin_max, 1);
    setup->read("special-2-spin-value-offset", special_2_spin_value_offset, 0);

    /*************Build special value combobox***end*****/
    setup->read("score",                score,                  pMerge(score, 0));
    setup->read("speed",                speed,                  pMerge(speed, 2.0));
    setup->read("coins",                coins,                  pMerge(coins, 0));
    setup->read("moving",               movement,               pMerge(movement, 1));
    setup->read("activity",             activity,               pMerge(activity, 1));
    setup->read("scenery",              scenery,                pMerge(scenery, 0));
    setup->read("keep-position",        keep_position,          pMerge(keep_position, 0));
    setup->read("shared-animation",     shared_ani,             pMerge(shared_ani, 0));
    setup->read("immortal",             immortal,               pMerge(immortal, 0));
    setup->read("can-be-eaten",         can_be_eaten,           pMerge(can_be_eaten, 0));
    pAlias("yoshicaneat",               can_be_eaten);//Alias. DEPRECATED! Update all config packs and remove this!
    setup->read("takable",              takable,                pMerge(takable, 0));
    setup->read("takable-sound-id",     takable_snd,            pMerge(takable_snd, 0));
    setup->read("grab-side",            grab_side,              pMerge(grab_side, 0));
    setup->read("grab-top",             grab_top,               pMerge(grab_top, 0));
    setup->read("grab-any",             grab_any,               pMerge(grab_any, 0));
    setup->read("default-health",       health,                 pMerge(health, 0));
    setup->read("hurtplayer",           hurt_player,            pMerge(hurt_player, 0));
    setup->read("hurtplayer-on-stomp",  hurt_player_on_stomp,   pMerge(hurt_player_on_stomp, 0));
    setup->read("hurtplayer-on-spinstomp", hurt_player_on_spinstomp, pMerge(hurt_player_on_spinstomp, 0));
    setup->read("hurtnpc",              hurt_npc,               pMerge(hurt_npc, 0));

    //Damage level on attacks types
    setup->read("damage-sensitive-stomp", damage_stomp, pMerge(damage_stomp, 0));
    setup->read("damage-sensitive-spinstomp", damage_spinstomp, pMerge(damage_spinstomp, 0));
    setup->read("damage-sensitive-itemkick", damage_itemkick, pMerge(damage_itemkick, 0));

    //Sound effects
    setup->read("hit-sound-id", hit_sound_id, merge_with ? merge_with->hit_sound_id : 0);
    setup->read("death-sound-id", death_sound_id, merge_with ? merge_with->death_sound_id : 0);

    //Editor features
    setup->read("direction-alt-title",      direct_alt_title,       pMerge(direct_alt_title, ""));
    setup->read("direction-alt-left-field", direct_alt_left,        pMerge(direct_alt_left, ""));
    setup->read("direction-alt-right-field",direct_alt_right,       pMerge(direct_alt_right, ""));
    setup->read("direction-alt-rand-field", direct_alt_rand,        pMerge(direct_alt_rand, ""));
    setup->read("direction-no-rand-field",  direct_disable_random,  pMerge(direct_disable_random, false));

    //Events
    setup->read("deactivate",           deactivation,           pMerge(deactivation, 0));
    setup->read("deactivate-delay",     deactivationDelay,      pMerge(deactivationDelay, 4000));
    NumberLimiter::applyD(deactivationDelay, 4000u, 0u);
    setup->read("deactivate-off-room",  deactivate_off_room,    pMerge(deactivate_off_room, false));
    setup->read("bump-on-stomp",        bump_on_stomp,          pMerge(bump_on_stomp, true));
    setup->read("kill-slside",          kill_slide_slope,       pMerge(kill_slide_slope, 0));
    setup->read("kill-onjump",          kill_on_jump,           pMerge(kill_by_npc, 0));
    setup->read("kill-bynpc",           kill_by_npc,            pMerge(kill_on_pit_fall, 0));
    setup->read("kill-on-pit-fall",     kill_on_pit_fall,       pMerge(kill_by_fireball, false));
    setup->read("kill-fireball",        kill_by_fireball,       pMerge(kill_by_fireball, 0));
    setup->read("kill-iceball",         freeze_by_iceball,      pMerge(freeze_by_iceball, 0));
    setup->read("kill-hammer",          kill_hammer,            pMerge(kill_hammer, 0));
    setup->read("kill-tail",            kill_tail,              pMerge(kill_tail, 0));
    setup->read("kill-spin",            kill_by_spinjump,       pMerge(kill_by_spinjump, 0));
    setup->read("kill-statue",          kill_by_statue,         pMerge(kill_by_statue, 0));
    setup->read("kill-with-mounted",    kill_by_mounted_item,   pMerge(kill_by_mounted_item, 0));
    setup->read("kill-on-eat",          kill_on_eat,            pMerge(kill_on_eat, 0));
    setup->read("cliffturn",            turn_on_cliff_detect,   pMerge(turn_on_cliff_detect, 0));
    setup->read("lava-protection",      lava_protect,           pMerge(lava_protect, false));
    setup->read("is-star",              is_star,                pMerge(is_star, 0));
    setup->read("is-exit",              exit_is,                pMerge(exit_is, false));
    setup->read("exit-direction",       exit_walk_direction,    pMerge(exit_walk_direction, 0));
    setup->read("exit-code",            exit_code,              pMerge(exit_code, 0));
    setup->read("exit-delay",           exit_delay,             pMerge(exit_delay, 0));
    setup->read("exit-sound-id",        exit_snd,               pMerge(exit_snd, 0));
    setup->read("is-climbable",         climbable,              pMerge(climbable, 0));

    //Editor specific flags
    int64_t iTmp;
    setup->read("default-friendly", iTmp, -1);
    default_friendly = (iTmp >= 0);
    default_friendly_value = (iTmp >= 0) ? static_cast<bool>(iTmp) : pMerge(default_friendly_value, false);

    setup->read("default-no-movable", iTmp, -1);
    default_nomovable = (iTmp >= 0);
    default_nomovable_value = (iTmp >= 0) ? static_cast<bool>(iTmp) : pMerge(default_nomovable_value, false);

    setup->read("default-is-boss", iTmp, -1);
    default_boss = (iTmp >= 0);
    default_boss_value = (iTmp >= 0) ? static_cast<bool>(iTmp) : pMerge(default_boss_value, false);

    setup->read("default-special-value", iTmp, -1);
    default_special = (iTmp >= 0);
    default_special_value = (iTmp >= 0) ? iTmp : pMerge(default_special_value, 0);

    #undef pMerge
    #undef pMergeMe
    #undef pAlias
    return true;
}



void NpcSetup::applyNPCtxt(const NPCConfigFile *local, const NpcSetup &global, uint32_t captured_w, uint32_t captured_h)
{
    //*this = global;
    name = (local->en_name) ? local->name : global.name;

    group =  (local->en_group) ? local->group : global.group;
    category =  (local->en_category) ? local->category : global.category;
    description =  (local->en_description) ? local->description : global.description;

    image_n = (local->en_image) ? local->image : global.image_n;
    mask_n = PGE_ImageInfo::getMaskName(image_n);

    algorithm_script =  (local->en_script) ? local->script : global.algorithm_script;
    gfx_offset_x =      (local->en_gfxoffsetx) ? local->gfxoffsetx : global.gfx_offset_x;
    gfx_offset_y =      (local->en_gfxoffsety) ? local->gfxoffsety : global.gfx_offset_y;
    width =             (local->en_width) ? local->width : global.width;
    NumberLimiter::apply(width, 1u);
    height =            (local->en_height) ? local->height : global.height;
    NumberLimiter::apply(height, 1u);
    foreground =        (local->en_foreground) ? local->foreground : global.foreground;
    framespeed =        (local->en_framespeed) ? ((local->framespeed * 1000) / 65) : global.framespeed;
    framestyle =        (local->en_framestyle) ? static_cast<uint32_t>(local->framestyle) : global.framestyle;
    NumberLimiter::apply(framestyle, 0u, 4u);

    //Copy physical size to GFX size
    if((local->en_width) && (custom_physics_to_gfx))
        gfx_w = width;
    else
    {
        if((!local->en_gfxwidth) && (captured_w != 0) && (global.gfx_w != captured_w))
            width = captured_w;
        gfx_w = ((captured_w != 0) ? captured_w : global.gfx_w);
    }

    //Copy physical size to GFX size
    if((local->en_height) && (custom_physics_to_gfx))
        gfx_h = height;
    else
        gfx_h = global.gfx_h;

    if((!local->en_gfxwidth) && (captured_w != 0) && (global.gfx_w != captured_w))
        gfx_w = captured_w;
    else
        gfx_w = (local->en_gfxwidth) ? (local->gfxwidth > 0 ? (local->gfxwidth) : 1) : gfx_w;
    NumberLimiter::apply(gfx_w, 1u);

    gfx_h =     (local->en_gfxheight) ? (local->gfxheight > 0 ? (local->gfxheight) : 1) : gfx_h;
    NumberLimiter::apply(gfx_h, 1u);
    grid =      (local->en_grid) ? local->grid : global.grid;
    NumberLimiter::apply(grid, 1u);

    if(width >= grid)
        grid_offset_x = -1 * Maths::iRound(static_cast<double>((width % grid) / 2));
    else
        grid_offset_x = Maths::iRound(static_cast<double>(static_cast<int32_t>(grid) - static_cast<int32_t>(width)) / 2.0);

    grid_attach_style = (local->en_gridalign) ? static_cast<uint32_t>(local->gridalign) : global.grid_attach_style;

    if(grid_attach_style == 1) grid_offset_x += (grid / 2);

    grid_offset_y = -static_cast<int>(height) % static_cast<int>(grid);
    grid_offset_x += (local->en_gridoffsetx) ? local->gridoffsetx : 0;
    grid_offset_y += (local->en_gridoffsety) ? local->gridoffsety : 0;

    // This check must go before frames will be changed
    bool animation_differs = (local->en_frames && (local->frames != global.frames));

    if((framestyle == 0) && ((local->en_gfxheight) || (local->en_height)) && (!local->en_frames))
    {
        frames = Maths::uRound(static_cast<double>(captured_h) / static_cast<double>(gfx_h));
        custom_animate = false;
    }
    else
        frames = (local->en_frames) ? local->frames : global.frames;
    NumberLimiter::apply(frames, 1u);

    if(animation_differs || (local->en_framestyle && (local->framestyle != global.framestyle)))
    {
        ani_bidir = false; //Disable bidirectional animation
        if(local->en_frames)
            custom_animate = false; //Disable custom animation
    }

    if(custom_animate && animation_differs)
    {
        // Validate custom animation
        for(int &frm : frames_left)
        {
            if((frm < 0) || ((uint32_t)frm > frames))
                custom_animate = false; //Disable custom animation as missmatch of frame IDs
        }
        for(int &frm : frames_right)
        {
            if((frm < 0) || ((uint32_t)frm > frames))
                custom_animate = false; //Disable custom animation as missmatch of frame IDs
        }
    }

    // Convert out of range frames by framestyle into animation with controlable diraction
    if((framestyle > 0) && (static_cast<unsigned int>(gfx_h) * frames >= captured_h))
    {
        framestyle = 0;
        ani_direct = false;
        ani_directed_direct = true;
    }

    if((global.framestyle == 0) && (local->framestyle > 0))
    {
        ani_direct = false;
        ani_directed_direct = false;
    }

    score =                 (local->en_score) ? static_cast<uint32_t>(local->score) : global.score;
    health =                (local->en_health) ? static_cast<uint32_t>(local->health) : global.health;
    block_player =          (local->en_playerblock) ? local->playerblock : global.block_player;
    block_player_top =      (local->en_playerblocktop) ? local->playerblocktop : global.block_player_top;
    block_npc =             (local->en_npcblock) ? local->npcblock : global.block_npc;
    block_npc_top =         (local->en_npcblocktop) ? local->npcblocktop : global.block_npc_top;
    grab_side =             (local->en_grabside) ? local->grabside : global.grab_side;
    grab_top =              (local->en_grabtop) ? local->grabtop : global.grab_top;
    kill_on_jump =          (local->en_jumphurt) ? (!local->jumphurt) : global.kill_on_jump ;
    hurt_player =           (local->en_nohurt) ? !local->nohurt : global.hurt_player;
    collision_with_blocks = (local->en_noblockcollision) ? (!local->noblockcollision) : global.collision_with_blocks;
    turn_on_cliff_detect =  (local->en_cliffturn) ? local->cliffturn : global.turn_on_cliff_detect;
    can_be_eaten =          (local->en_noyoshi) ? (!local->noyoshi) : global.can_be_eaten;
    speed =                 (local->en_speed) ? global.speed * local->speed : global.speed;
    kill_by_fireball =      (local->en_nofireball) ? (!local->nofireball) : global.kill_by_fireball;
    gravity =               (local->en_nogravity) ? (!local->nogravity) : global.gravity;
    freeze_by_iceball =     (local->en_noiceball) ? (!local->noiceball) : global.freeze_by_iceball;
    kill_hammer =           (local->en_nohammer) ? (!local->nohammer) : global.kill_hammer;
    kill_by_npc =           (local->en_noshell) ? (!local->noshell) : global.kill_by_npc;
}
