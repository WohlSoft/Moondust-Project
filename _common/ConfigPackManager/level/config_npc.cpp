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
#include <QSettings>
#include <cmath>
#include <tgmath.h>
#include <PGE_File_Formats/npc_filedata.h>

#include "../image_size.h"
#include "../../number_limiter.h"
#include "../../csv_2_number_array.h"

bool NpcSetup::parse(QSettings *setup, QString npcImgPath, unsigned int defaultGrid, NpcSetup *merge_with, QString *error)
{
    int errCode = PGE_ImageInfo::ERR_OK;
    QString section;
    /*************Buffers*********************/
    int defGFX_h = 0;
    int combobox_size = 0;

    /*************Buffers*********************/
    if(!setup)
    {
        if(error)
            *error = "setup QSettings is null!";

        return false;
    }

    section = setup->group();
    name =         setup->value("name", merge_with ? merge_with->name : section).toString();

    if(name.isEmpty())
    {
        if(error)
            *error = QString("%1 Item name isn't defined").arg(section.toUpper());

        return false;
    }

    group =        setup->value("group", merge_with ? merge_with->group : "_NoGroup").toString();
    category =     setup->value("category", merge_with ? merge_with->category : "_Other").toString();
    image_n =      setup->value("image", merge_with ? merge_with->image_n : "").toString();

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

    mask_n = PGE_ImageInfo::getMaskName(image_n);
    algorithm_script = setup->value("algorithm", merge_with ? merge_with->algorithm_script : QString("%1.lua").arg(section)).toString();
    effect_1 =         setup->value("default-effect", merge_with ? merge_with->effect_1 : 10u).toUInt();
    effect_2 =         setup->value("shell-effect", merge_with ? merge_with->effect_2 : 10u).toUInt();
    //Physics
    height =               setup->value("physical-height", merge_with ? merge_with->height : 0).toUInt();
    NumberLimiter::apply(height, 1u);
    width =                setup->value("physical-width", merge_with ? merge_with->width : 0).toUInt();
    NumberLimiter::apply(width, 1u);
    block_npc =            setup->value("block-npc", merge_with ? merge_with->block_npc : false).toBool();
    block_npc_top =        setup->value("block-npc-top", merge_with ? merge_with->block_npc_top : false).toBool();
    block_player =         setup->value("block-player", merge_with ? merge_with->block_player : false).toBool();
    block_player_top =     setup->value("block-player-top", merge_with ? merge_with->block_player_top : false).toBool();
    collision_with_blocks = setup->value("collision-blocks", merge_with ? merge_with->collision_with_blocks : false).toBool();
    gravity =              setup->value("gravity", merge_with ? merge_with->gravity : false).toBool();
    adhesion =             setup->value("adhesion", merge_with ? merge_with->adhesion : false).toBool();
    contact_padding =      setup->value("contact-padding", merge_with ? merge_with->contact_padding : 0.0).toDouble();
    container =            setup->value("container", merge_with ? merge_with->container : false).toBool();
    contents_id =                setup->value("contents-id", merge_with ? merge_with->contents_id : 0).toUInt();
    container_elastic =          setup->value("container-elastic", merge_with ? merge_with->container_elastic : false).toBool();
    container_elastic_border_w = setup->value("container-elastic-border-w", merge_with ? merge_with->container_elastic_border_w : 4).toInt();
    NumberLimiter::apply(container_elastic_border_w, 0);
    container_show_contents    = setup->value("container-show-contents", merge_with ? merge_with->container_show_contents : false).toBool();
    container_content_z_offset = setup->value("container-content-z-offset", merge_with ? merge_with->container_content_z_offset : -0.00001f).toFloat();
    container_crop_contents    = setup->value("container-crop-contents", merge_with ? merge_with->container_crop_contents : false).toBool();
    container_align_contents   = setup->value("container-align-contents", merge_with ? merge_with->container_align_contents : 0).toInt();
    no_npc_collisions =      setup->value("no-npc-collisions", merge_with ? merge_with->no_npc_collisions : false).toBool();
    //Graphics
    gfx_offset_x = setup->value("gfx-offset-x", merge_with ? merge_with->gfx_offset_x : 0).toInt();
    gfx_offset_y = setup->value("gfx-offset-y", merge_with ? merge_with->gfx_offset_y : 0).toInt();
    framestyle = setup->value("frame-style", merge_with ? merge_with->framestyle : 0).toInt();
    NumberLimiter::apply(framestyle, 0, 4);
    frames = setup->value("frames", merge_with ? merge_with->frames : 1).toUInt();
    NumberLimiter::apply(frames, 1u);
    /****************Calculating of default frame height******************/
    defGFX_h = gfx_h / static_cast<int>( frames * static_cast<unsigned int>(pow(2.0, static_cast<double>(framestyle))));
    /****************Calculating of default frame height**end*************/
    custom_physics_to_gfx = setup->value("physics-to-gfx", merge_with ? merge_with->custom_physics_to_gfx : true).toBool();
    gfx_h =                setup->value("gfx-height", merge_with ? merge_with->gfx_h : defGFX_h).toInt();
    NumberLimiter::apply(gfx_h, 1);
    gfx_w =                setup->value("gfx-width", merge_with ? merge_with->gfx_w : gfx_w).toInt();
    NumberLimiter::apply(gfx_w, 1);
    framespeed =           setup->value("frame-speed", merge_with ? merge_with->framespeed : 128).toUInt();
    display_frame =        setup->value("display-frame", merge_with ? merge_with->display_frame : false).toUInt();
    foreground =           setup->value("foreground", merge_with ? merge_with->foreground : false).toBool();
    background =           setup->value("background", merge_with ? merge_with->background : false).toBool();
    z_offset =             setup->value("z-offset", merge_with ? merge_with->z_offset : 0.0).toDouble();
    ani_directed_direct =  setup->value("animation-directed-direction", merge_with ? merge_with->ani_directed_direct : false).toBool();
    ani_direct =           setup->value("animation-direction", merge_with ? merge_with->ani_direct : false).toBool();
    ani_bidir =            setup->value("animation-bidirectional", merge_with ? merge_with->ani_bidir : false).toBool();
    custom_animate =   setup->value("custom-animation", merge_with ? merge_with->custom_animate : 0).toBool();
    custom_ani_alg =   setup->value("custom-animation-alg", merge_with ? merge_with->custom_ani_alg : 0).toInt();
    custom_ani_fl =    setup->value("custom-animation-fl", merge_with ? merge_with->custom_ani_fl : 0).toInt();
    custom_ani_el =    setup->value("custom-animation-el", merge_with ? merge_with->custom_ani_el : -1).toInt();
    custom_ani_fr =    setup->value("custom-animation-fr", merge_with ? merge_with->custom_ani_fr : 0).toInt();
    custom_ani_er =    setup->value("custom-animation-er", merge_with ? merge_with->custom_ani_er : -1).toInt();
    /*************Build custom animation settings***************/
    frames_left.clear();
    frames_right.clear();

    if(custom_ani_alg == 2)
    {
        QString common = setup->value("ani-frames-cmn", 0).toString(); // Common frames list
        QString tmp;
        tmp = setup->value("ani-frames-left", common).toString().remove(' '); //left direction
        CSV2NumArray(tmp, frames_left,  0);
        tmp = setup->value("ani-frames-right", common).toString().remove(' '); //right direction
        CSV2NumArray(tmp, frames_right, 0);
    }

    /*************Build custom animation settings**end**********/
    /***************GRID And snap*********************************/
    grid = setup->value("grid", merge_with ? merge_with->grid : defaultGrid).toUInt();
    NumberLimiter::apply(grid, 1u);
    grid_attach_style = setup->value("grid-attachement-style", 0).toInt();
    NumberLimiter::apply(grid_attach_style, 0);

    /***************Calculate the grid offset********************/
    if(width >= grid)
        grid_offset_x = -1 * qRound(static_cast<qreal>(width % grid) / 2);
    else
        grid_offset_x = qRound(static_cast<qreal>(static_cast<int>(grid) - static_cast<int>(width)) / 2.0);

    if(grid_attach_style == 1)
        grid_offset_x += (grid / 2);

    grid_offset_y = -static_cast<int>(height) % static_cast<int>(grid);
    /***************Calculate the grid offset********************/
    /*************Manual redefinition of the grid offset if not set******************/
    grid_offset_x = setup->value("grid-offset-x", merge_with ? merge_with->grid_offset_x : grid_offset_x).toInt();
    grid_offset_y = setup->value("grid-offset-y", merge_with ? merge_with->grid_offset_y : grid_offset_y).toInt();
    /*************Manual redefinition of the grid offset if not set******************/
    /***************GRID And snap***end***************************/
    special_option =   setup->value("have-special", merge_with ? merge_with->special_option : false).toBool();
    special_name =     setup->value("special-name", merge_with ? merge_with->special_name : "Special option value").toString();
    special_type =     setup->value("special-type", merge_with ? merge_with->special_type : 1).toInt();
    combobox_size =    setup->value("special-combobox-size", 0).toInt();

    if((combobox_size <= 0) && merge_with)
        special_combobox_opts = merge_with->special_combobox_opts;
    else
    {
        /*************Build special value combobox***********/
        special_combobox_opts.clear();

        for(int j = 0; j < combobox_size; j++)
        {
            special_combobox_opts.push_back(
                setup->value(QString("special-option-%1").arg(j), 0).toString()
            );
        }
    }

    special_spin_min = setup->value("special-spin-min", merge_with ? merge_with->special_spin_min : 0).toInt();
    special_spin_max = setup->value("special-spin-max", merge_with ? merge_with->special_spin_max : 1).toInt();
    special_spin_value_offset = setup->value("special-spin-value-offset", merge_with ? merge_with->special_spin_value_offset : 0).toInt();
    /**LEGACY, REMOVE THIS LAYER!!!***/
    special_option_2 = setup->value("have-special-2", merge_with ? merge_with->special_option_2 : 0).toBool();
    special_2_name = setup->value("special-2-name", merge_with ? merge_with->special_2_name : "Special option value").toString();

    if(special_option_2)
    {
        QString tmp1 = setup->value("special-2-npc-spin-required", "").toString().remove(' ');
        CSV2NumArray(tmp1, special_2_npc_spin_required, 0l);
        QString tmp2 = setup->value("special-2-npc-box-required", "").toString().remove(' ');
        CSV2NumArray(tmp2, special_2_npc_box_required, 0l);
    }

    special_2_type = setup->value("special-2-type", 1).toInt();
    combobox_size  = setup->value("special-2-combobox-size", 0).toInt();

    for(int j = 0; j < combobox_size; j++)
    {
        special_2_combobox_opts.push_back(
            setup->value(QString("special-2-option-%1").arg(j), 0).toString()
        );
    }

    special_2_spin_min = setup->value("special-2-spin-min", 0).toInt();
    special_2_spin_max = setup->value("special-2-spin-max", 1).toInt();
    special_2_spin_value_offset = setup->value("special-2-spin-value-offset", 0).toInt();
    /*************Build special value combobox***end*****/
    score =        setup->value("score", merge_with ? merge_with->score : 0).toInt();
    speed =        setup->value("speed", merge_with ? merge_with->speed : 2.0).toDouble();
    coins =        setup->value("coins", merge_with ? merge_with->coins : 0).toInt();
    movement =     setup->value("moving", merge_with ? merge_with->movement : 1).toBool();
    activity =     setup->value("activity", merge_with ? merge_with->activity : 1).toBool();
    scenery =      setup->value("scenery", merge_with ? merge_with->scenery : 0).toBool();
    keep_position = setup->value("keep-position", merge_with ? merge_with->keep_position : 0).toBool();
    shared_ani =   setup->value("shared-animation", merge_with ? merge_with->shared_ani : 0).toBool();
    immortal =     setup->value("immortal", merge_with ? merge_with->immortal : 0).toBool();
    can_be_eaten = setup->value("can-be-eaten", merge_with ? merge_with->can_be_eaten : 0).toBool();
    //! LEGACY! Update all config packs and remove this!
    can_be_eaten = setup->value("yoshicaneat", can_be_eaten).toBool();
    takable =      setup->value("takable", merge_with ? merge_with->takable : 0).toBool();
    takable_snd =  setup->value("takable-sound-id", merge_with ? merge_with->takable_snd : 0).toInt();
    grab_side =    setup->value("grab-side", merge_with ? merge_with->grab_side : 0).toBool();
    grab_top =     setup->value("grab-top", merge_with ? merge_with->grab_top : 0).toBool();
    grab_any =     setup->value("grab-any", merge_with ? merge_with->grab_any : 0).toBool();
    health =       setup->value("default-health", merge_with ? merge_with->health : 0).toInt();
    hurt_player =  setup->value("hurtplayer", merge_with ? merge_with->hurt_player : 0).toBool();
    hurt_player_on_stomp =  setup->value("hurtplayer-on-stomp", merge_with ? merge_with->hurt_player_on_stomp : 0).toBool();
    hurt_player_on_spinstomp =  setup->value("hurtplayer-on-spinstomp", merge_with ? merge_with->hurt_player_on_spinstomp : 0).toBool();
    hurt_npc =     setup->value("hurtnpc", merge_with ? merge_with->hurt_npc : 0).toBool();
    //Damage level on attacks types
    damage_stomp = setup->value("damage-sensitive-stomp", merge_with ? merge_with->damage_stomp : 0).toInt();
    damage_spinstomp = setup->value("damage-sensitive-spinstomp", merge_with ? merge_with->damage_spinstomp : 0).toInt();
    damage_itemkick = setup->value("damage-sensitive-itemkick", merge_with ? merge_with->damage_itemkick : 0).toInt();
    //Sound effects
    hit_sound_id = setup->value("hit-sound-id", merge_with ? merge_with->hit_sound_id : 0).toInt();
    death_sound_id = setup->value("death-sound-id", merge_with ? merge_with->death_sound_id : 0).toInt();
    //Editor features
    direct_alt_title =      setup->value("direction-alt-title", "").toString();
    direct_alt_left =       setup->value("direction-alt-left-field", "").toString();
    direct_alt_right =      setup->value("direction-alt-right-field", "").toString();
    direct_disable_random = setup->value("direction-no-rand-field", merge_with ? merge_with->direct_disable_random : false).toBool();
    //Events
    deactivation =         setup->value("deactivate", merge_with ? merge_with->deactivation : 0).toBool();
    deactivationDelay =    setup->value("deactivate-delay", merge_with ? merge_with->deactivationDelay : 4000).toInt();
    NumberLimiter::applyD(deactivationDelay, 4000, 0);
    deactivate_off_room =  setup->value("deactivate-off-room", merge_with ? merge_with->deactivate_off_room : false).toBool();
    bump_on_stomp =        setup->value("bump-on-stomp", merge_with ? merge_with->bump_on_stomp : true).toBool();
    kill_slide_slope =     setup->value("kill-slside", merge_with ? merge_with->kill_slide_slope : 0).toBool();
    kill_on_jump =         setup->value("kill-onjump", merge_with ? merge_with->kill_by_npc : 0).toBool();
    kill_by_npc =          setup->value("kill-bynpc", merge_with ? merge_with->kill_on_pit_fall : 0).toBool();
    kill_on_pit_fall =     setup->value("kill-on-pit-fall", merge_with ? merge_with->kill_by_fireball : false).toBool();
    kill_by_fireball =     setup->value("kill-fireball", merge_with ? merge_with->kill_by_fireball : 0).toBool();
    freeze_by_iceball =    setup->value("kill-iceball", merge_with ? merge_with->freeze_by_iceball : 0).toBool();
    kill_hammer =          setup->value("kill-hammer", merge_with ? merge_with->kill_hammer : 0).toBool();
    kill_tail =            setup->value("kill-tail", merge_with ? merge_with->kill_tail : 0).toBool();
    kill_by_spinjump =     setup->value("kill-spin", merge_with ? merge_with->kill_by_spinjump : 0).toBool();
    kill_by_statue =       setup->value("kill-statue", merge_with ? merge_with->kill_by_statue : 0).toBool();
    kill_by_mounted_item = setup->value("kill-with-mounted", merge_with ? merge_with->kill_by_mounted_item : 0).toBool();
    kill_on_eat =          setup->value("kill-on-eat", merge_with ? merge_with->kill_on_eat : 0).toBool();
    turn_on_cliff_detect = setup->value("cliffturn", merge_with ? merge_with->turn_on_cliff_detect : 0).toBool();
    lava_protect =         setup->value("lava-protection", merge_with ? merge_with->lava_protect : false).toBool();
    is_star =              setup->value("is-star", merge_with ? merge_with->is_star : 0).toBool();
    exit_is =              setup->value("is-exit", merge_with ? merge_with->exit_is : false).toBool();
    exit_walk_direction =  setup->value("exit-direction", merge_with ? merge_with->exit_walk_direction : 0).toInt();
    exit_code       =      setup->value("exit-code", merge_with ? merge_with->exit_code : 0).toInt();
    exit_delay      =      setup->value("exit-delay", merge_with ? merge_with->exit_delay : 0).toInt();
    exit_snd        =      setup->value("exit-sound-id", merge_with ? merge_with->exit_snd : 0).toInt();
    climbable       =      setup->value("is-climbable", merge_with ? merge_with->climbable : 0).toBool();
    //Editor specific flags
    long iTmp;
    iTmp =      setup->value("default-friendly", -1).toInt();
    default_friendly = (iTmp >= 0);
    default_friendly_value = (iTmp >= 0) ? static_cast<bool>(iTmp) : (merge_with ? merge_with->default_friendly_value : false);
    iTmp =      setup->value("default-no-movable", -1).toInt();
    default_nomovable = (iTmp >= 0);
    default_nomovable_value = (iTmp >= 0) ? static_cast<bool>(iTmp) : (merge_with ? merge_with->default_nomovable_value : false);
    iTmp =      setup->value("default-is-boss", -1).toInt();
    default_boss = (iTmp >= 0);
    default_boss_value = (iTmp >= 0) ? static_cast<bool>(iTmp) : (merge_with ? merge_with->default_boss_value : false);
    iTmp =      setup->value("default-special-value", -1).toInt();
    default_special = (iTmp >= 0);
    default_special_value = (iTmp >= 0) ? iTmp : (merge_with ? merge_with->default_special_value : false);
    return true;
}



void NpcSetup::applyNPCtxt(const NPCConfigFile *local, const NpcSetup &global, const QSize &captured)
{
    //*this = global;
    name = (local->en_name) ? local->name : global.name;
    image_n = (local->en_image) ? local->image : global.image_n;
    mask_n = PGE_ImageInfo::getMaskName(image_n);
    algorithm_script = (local->en_script) ? local->script : global.algorithm_script;
    gfx_offset_x = (local->en_gfxoffsetx) ? local->gfxoffsetx : global.gfx_offset_x;
    gfx_offset_y = (local->en_gfxoffsety) ? local->gfxoffsety : global.gfx_offset_y;
    width = (local->en_width) ? local->width : global.width;
    height = (local->en_height) ? local->height : global.height;
    foreground = (local->en_foreground) ? local->foreground : global.foreground;
    framespeed = (local->en_framespeed) ? static_cast<unsigned int>(qRound(static_cast<qreal>(global.framespeed) / (8.0 / static_cast<qreal>(local->framespeed)))) : global.framespeed;
    framestyle = (local->en_framestyle) ? static_cast<int>(local->framestyle) : global.framestyle;

    //Copy physical size to GFX size
    if((local->en_width) && (custom_physics_to_gfx))
        gfx_w = static_cast<int>(width);
    else
    {
        if((!local->en_gfxwidth) && (captured.width() != 0) && (global.gfx_w != captured.width()))
            width = static_cast<unsigned int>(captured.width());

        gfx_w = ((captured.width() != 0) ? captured.width() : global.gfx_w);
    }

    //Copy physical size to GFX size
    if((local->en_height) && (custom_physics_to_gfx))
        gfx_h = static_cast<int>(height);
    else
        gfx_h = global.gfx_h;

    if((!local->en_gfxwidth) && (captured.width() != 0) && (global.gfx_w != captured.width()))
        gfx_w = captured.width();
    else
        gfx_w = (local->en_gfxwidth) ? (local->gfxwidth > 0 ? static_cast<int>(local->gfxwidth) : 1) : gfx_w;

    gfx_h = (local->en_gfxheight) ? (local->gfxheight > 0 ? static_cast<int>(local->gfxheight) : 1) : gfx_h;
    grid = (local->en_grid) ? local->grid : global.grid;

    if(width >= grid)
        grid_offset_x = -1 * qRound(static_cast<qreal>((width % grid) / 2));
    else
        grid_offset_x = qRound(static_cast<qreal>(static_cast<int>(grid) - static_cast<int>(width)) / 2);

    grid_attach_style = (local->en_grid_align) ? static_cast<int>(local->grid_align) : global.grid_attach_style;

    if(grid_attach_style == 1) grid_offset_x += (grid / 2);

    grid_offset_y = -static_cast<int>(height) % static_cast<int>(grid);
    grid_offset_x += (local->en_grid_offset_x) ? local->grid_offset_x : 0;
    grid_offset_y += (local->en_grid_offset_y) ? local->grid_offset_y : 0;

    if((framestyle == 0) && ((local->en_gfxheight) || (local->en_height)) && (!local->en_frames))
    {
        frames = static_cast<unsigned int>(qRound(static_cast<qreal>(captured.height()) / static_cast<qreal>(gfx_h)));
        //merged.custom_animate = false;
    }
    else
        frames = (local->en_frames) ? local->frames : global.frames;

    if((local->en_frames) || (local->en_framestyle))
    {
        ani_bidir = false; //Disable bidirectional animation

        if((local->en_frames)) custom_animate = false; //Disable custom animation
    }

    // Convert out of range frames by framestyle into animation with controlable diraction
    if((framestyle > 0) && (static_cast<unsigned int>(gfx_h) * frames >= static_cast<unsigned int>(captured.height())))
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

    score = (local->en_score) ? static_cast<int>(local->score) : global.score;
    health = (local->en_health) ? static_cast<int>(local->health) : global.health;
    block_player = (local->en_playerblock) ? local->playerblock : global.block_player;
    block_player_top = (local->en_playerblocktop) ? local->playerblocktop : global.block_player_top;
    block_npc = (local->en_npcblock) ? local->npcblock : global.block_npc;
    block_npc_top = (local->en_npcblocktop) ? local->npcblocktop : global.block_npc_top;
    grab_side = (local->en_grabside) ? local->grabside : global.grab_side;
    grab_top = (local->en_grabtop) ? local->grabtop : global.grab_top;
    kill_on_jump = (local->en_jumphurt) ? (!local->jumphurt) : global.kill_on_jump ;
    hurt_player = (local->en_nohurt) ? !local->nohurt : global.hurt_player;
    collision_with_blocks = (local->en_noblockcollision) ? (!local->noblockcollision) : global.collision_with_blocks;
    turn_on_cliff_detect = (local->en_cliffturn) ? local->cliffturn : global.turn_on_cliff_detect;
    can_be_eaten = (local->en_noyoshi) ? (!local->noyoshi) : global.can_be_eaten;
    speed = (local->en_speed) ? global.speed * local->speed : global.speed;
    kill_by_fireball = (local->en_nofireball) ? (!local->nofireball) : global.kill_by_fireball;
    gravity = (local->en_nogravity) ? (!local->nogravity) : global.gravity;
    freeze_by_iceball = (local->en_noiceball) ? (!local->noiceball) : global.freeze_by_iceball;
    kill_hammer = (local->en_nohammer) ? (!local->nohammer) : global.kill_hammer;
    kill_by_npc = (local->en_noshell) ? (!local->noshell) : global.kill_by_npc;
}
