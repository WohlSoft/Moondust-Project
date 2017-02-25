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

#pragma once
#ifndef CONFIG_NPC_H
#define CONFIG_NPC_H

#include <PGEString.h>
#include <stdint.h>
#include <QSize>

class IniProcessing;
struct NPCConfigFile;

struct NpcSetup
{
    NpcSetup()
    {
        id = 0;
        gfx_offset_x = 0;
        gfx_offset_y = 0;
        gfx_w = 32;
        gfx_h = 32;
        grid = 32;
        grid_offset_x = 0;
        grid_offset_y = 0;
        grid_attach_style = 0;
        framestyle = 0;
        frames = 1;
        framespeed = 80;
        width = 32;
        height = 32;
        z_offset = 0.0;
    }

    /**
     * @brief Parse opened NPC config INI section
     * @param setup pointer to initialized INI manager with opened NPC section
     * @param npcImgPath path to NPC images folder
     * @param [in] merge_with Merge settings with one other
     * @param [out] error Error string
     * @return true if config parsed successfully, false on erros
     */
    bool parse(IniProcessing *setup,
               PGEString npcImgPath,
               uint32_t defaultGrid,
               NpcSetup* merge_with = nullptr,
               PGEString *error=nullptr);

    /**
     * @brief Merges SMBX64-NPC.txt structure with default global NPC-configuration of specific NPC-ID and returns merged configuration
     * @param local SMBX64-NPC.txt structure readen from a file
     * @param global Default NPC configuration for specific NPC-ID
     */
    void applyNPCtxt(const NPCConfigFile *local, const NpcSetup &global, uint32_t captured_w = 0, uint32_t captured_h = 0);

    uint64_t          id = 0;     //    [npc-1]
    PGEString         name;       //    name="Goomba"
    PGEString         group = "_NoGroup";      //    group="Enemy" 		;The sort category
    PGEString         category = "_Other";   //    category="Enemy"		;The sort category
    PGEString         image_n;    //    image="npc-1.gif"		;NPC Image file
    PGEString         mask_n;
    PGEString         algorithm_script;   //    ; Filename of the lua algorithm
    //!Name of global level function to process NPC death effect
    PGEString         effect_function;
    //!LEGACY: effect on stomp death reason
    uint32_t          effect_1 = 0;     //default-effect=2		;Spawn effect ID on jump-die
    //!LEGACY: effect on other death reasons except lava burn
    uint32_t          effect_2 = 0;     //shell-effect=4        ;effect on kick by shell or other NPC

    //    ; graphics
    //! Offset X of sprite relative to center<->center position (dependent to direction, may be multipled to -1)
    int32_t           gfx_offset_x = 0;   //    gfx-offst-x=0
    //! Offset Y of sprite relative to bottom<->bottom position of NPC
    int32_t           gfx_offset_y = 0;   //    gfx-offst-y=2
    //! Height of frame (if not defined, will be calculated from count of the frames)
    uint32_t          gfx_h = 0;          //    gfx-height-y=32
    //! Width of frame (if not defined, will be copied from actual sprite image width)
    uint32_t          gfx_w = 0;          //    gfx-width-y=32
    //! Allow automatic mapping of the customized hitbox metrics to frame size
    bool              custom_physics_to_gfx = false; //The GFX size defining by physics size in the custom configs
    //! Size of alignment grid while placing mode
    uint32_t          grid = 32;          //    grid=32
    //! Offset X relative aligned position
    int32_t           grid_offset_x = 0;  //    grid-offset-x=0
    //! Offset Y relative aligned position
    int32_t           grid_offset_y = 0;  //    grid-offset-y=0
    //! 0 - Align NPC X position relative it's center to center of the grid cell, 1 - to edge of grid cell
    uint32_t          grid_attach_style = 0; //0 - to center, 1 - to edge

    //! Style of frame sets
    uint32_t          framestyle = 0;   //    frame-style=0	; (0-2) This option in some alhoritmes can be ignored
    //! Total count of frames (Total frames on the sprite formula is: Ntf = Fn/(2^Fs) )
    uint32_t          frames = 0;    //    frames=2
    //! Delay between frames in milli-seconds
    uint32_t          framespeed = 128;//    frame-speed=128
    //! Render NPC's frame on NPC-Foreground layer (if both foreground and background are false - render on regular NPC render layer)
    bool              foreground = false;        //    foreground=0
    //! Render NPC's frame on NPC-Background layer
    bool              background = false;        //    background=0
    //! Relative Z-Offset at current render layer
    double          z_offset = 0.0;
    //! Bidirectional animation
    bool            ani_bidir = false;         //    animation-bidirectional=0
    //! Initial direction of animation sequence (0 - forward, 1 - backward)
    bool            ani_direct = false;        //    animation-direction=0
    //! Direction of animation sequence will be dependent from physical face direction
    bool            ani_directed_direct = false; //Animation direction will be gotten from NPC's direction

    //    ; for editor
    bool            custom_animate = false;//custom-animation=0
                        //; this option useful for non-standart algorithmic sprites (for example, bosses)

    int32_t         custom_ani_alg = 0; //;custom-animation-alg=0		; Custom animation algorithm
                        //0 simple frame range, 1 - frame Jump; 2 - custom animation sequances
    int32_t         custom_ani_fl = 0; //    ;custom-animation-fl=0		; First frame for LEFT
    int32_t         custom_ani_el = 0; //    ;custom-animation-el=0		; end frame for LEFT / Jump step
    int32_t         custom_ani_fr = 0; //    ;custom-animation-fr=0		; first frame for RIGHT
    int32_t         custom_ani_er = 0; //    ;custom-animation-er=0		; end frame for RIGHT / Jump step

    typedef PGEList<int32_t> IntArray;
    typedef PGEList<int64_t> LongArray;

    IntArray        frames_left;     //Frame srquence for left
    IntArray        frames_right;    //Frame srquence for right

    bool            container = false;// container=0; NPC can containing inside other NPC (need enable special option type 2)
    /*PGE Engine side things*/
    uint32_t        contents_id = 0; //contents-id=0; ID of contained NPC
    bool            container_elastic = false; //Elastic sprite of container (like sizable block)
    uint32_t        container_elastic_border_w = 0; //Width of border to draw elastic container
    bool            container_show_contents = false; //Show contents of container
    double          container_content_z_offset = 0.0; //Render target sprite with next offset
    bool            container_crop_contents = false;    //Crop contents GFX size to the size of an item
    uint32_t        container_align_contents = 0;    //align contents to center-0 or top-1
    /************************/

    uint32_t        display_frame = 0;

    bool            no_npc_collisions = false;//    ; this option disabling collisions in editor with other NPCs, but with NPC's of same ID collisions will be checked

    bool            special_option = false;  //    ; Special option
                                     //    have-special=0; Special NPC's option, what can used by NPC's algorithm
    PGEString       special_name; //    ;special-name="Cheep-cheep"	; 60
    uint32_t        special_type; //    ;special-type=0; 61 0 combobox, 1 - spin, 2 - npc-id
    PGEStringList   special_combobox_opts;//;special-combobox-size=3; 62 quantity of options
                                      //;special-option-0="Swim"; 63 set title for 0 option combobox
                                      //;special-option-1="Jump"; 64 set title for 1 option combobox
                                      //;special-option-2="Projective"	; 65 set title for 2 option combobox
    int32_t         special_spin_min = 0;   //    ;special-spin-min=0		; 66 milimal value of spin
    int32_t         special_spin_max = 0;   //    ;special-spin-max=25		; 67 maximal value of spin
    int32_t         special_spin_value_offset = 0;  //    have-special-2=0			; Special NPC's option, what can used by NPC's algorithm
    bool            special_option_2 = false; //   Second special option
                           //   special-2-npc-spin-required
    LongArray       special_2_npc_spin_required;   //    special-2-npc-box-required
    LongArray       special_2_npc_box_required;

    PGEString       special_2_name; //    ;special-2-name="Cheep-cheep"	; 60
    int32_t         special_2_type = 0; //    ;special-2-type=0			; 61 0 combobox, 1 - spin
    PGEStringList   special_2_combobox_opts;    //;special-combobox-size=3		; 62 quantity of options
                                            //;special-option-0="Swim"		; 63 set title for 0 option combobox
                                            //;special-option-1="Jump"		; 64 set title for 1 option combobox
                                            //;special-option-2="Projective"	; 65 set title for 2 option combobox
    int32_t         special_2_spin_min = 0; //    ;special-2-spin-min=0		; 66 milimal value of spin
    int32_t         special_2_spin_max = 0; //    ;special-2-spin-max=25		; 67 maximal value of spin
    int32_t         special_2_spin_value_offset = 0;    //special-2-spin-value-offset

    //;game process
    uint32_t        score = 0;  //    score=2				; Add scores to player (value 0-13)
                            //    ; 0, 10, 100, 200, 400, 800, 1000, 2000, 4000, 8000, 1up, 2up, 5up, 3up
    int32_t         coins = 0; //Add number of coins

    double          speed = 0.0;     //    speed=64			; Default movement speed in px/s
    bool            movement = false; //    moving=1			; NPC simply moving right/left
    bool            scenery = false;  //    scenery=0			; NPC as block
    bool            immortal = false; //    immortal=0			; NPC Can't be destroy
    bool            keep_position = false;  //    scenery=0			; Keep NPC's position on despawn
    bool            activity = false;  //   activity=1         ; NPC has "Activated" event, doing regular loops, etc.
                               //                        If flag is false, NPC doing job while on screen only and keeps position
    bool            shared_ani = false;//   shared-animation   ; All NPC's of same ID will do same animation
    bool            can_be_eaten = false; //    yoshicaneat=1			; NPC can be eaten by yoshi
    bool            takable = false;  //    takeble=0			; NPC destroyble on contact with player
    uint32_t        takable_snd = 0; //Play sound-id on take
    bool            grab_side = false;//    grab-side=0			; NPC can be grabbed on side
    bool            grab_top = false; //    grab-top=0			; NPC can be grabbed on top
    bool            grab_any = false; //    grab-any=0			; NPC can be grabbed on any collisions
    uint32_t        health = 1;    //    default-health=1		; NPC's health value
    bool            hurt_player = false; //    hurtplayer=1			; Hurt player on contact
    bool            hurt_player_on_stomp = false;//    Hurt player on attempt to apply a stomp attack
    bool            hurt_player_on_spinstomp = false;//Hurt player on attempt to apply a spin-stomp attack
    bool            hurt_npc = false; //    hurtnpc=0			; Hurt other npc on contact
    int32_t         damage_stomp = 0;//     Damage sensitivity from stomp attak
    int32_t         damage_spinstomp = 0;// Damage sensitivity from spin-stomp attak
    int32_t         damage_itemkick = 0;//  Damage sensitivity from kick by thrown item (other NPC) attak

    //Hardcoded sound effects
    uint32_t        hit_sound_id = 0;
    uint32_t        death_sound_id = 0;

    //;Editor featured
    PGEString       direct_alt_title;
    PGEString       direct_alt_left;
    PGEString       direct_alt_right;
    bool            direct_disable_random = false;

//    ; Physics
    uint32_t        height = 0; //    ; Size of NPC's body (Collision box)
    uint32_t        width = 0;
    bool            block_npc = false;     //    block-npc=1		; NPC is a solid object for NPC's
    bool            block_npc_top = false; //    block-npc-top=0		; on NPC's top can be stay other NPC's
    bool            block_player = false;  //    block-player=0		; NPC is a solid object for player
    bool            block_player_top = false;  //    block-player-top=0	; on NPC's top can be stay player
    bool            collision_with_blocks = false; //    collision-blocks=1	; Enable collisions with blocks
    bool            gravity = false;       //    gravity=1		; Enable gravitation for this NPC
    bool            adhesion = false;      //    adhesion=0		; allows to NPC walking on wall and on celling
    double          contact_padding = 0.0;//  negative value will increase "contactable zone" (non-collidable contacts only)

//    ;Events
    bool            deactivation = true;  //deactivate=1		; Deactivate on state offscreen > 4 sec ago
    uint32_t        deactivationDelay = 4000;
    bool            deactivate_off_room = false; //Decativate NPC outed of section box
    bool            bump_on_stomp = false;  //    Bumps playable character on stomp attak of this NPC
    bool            kill_slide_slope = false;  //    kill-slside=1		; Kill on Slope slide
    bool            kill_on_jump = false;  //    kill-onjump=1		; Kill on jump on NPC's head
    bool            kill_by_npc = false;   //    kill-bynpc=1		; Kill by contact with other NPC with hurt-npc
                        //; for example: moving SHELL have "HURT_NPC", and shell kiling ALL NPCs on contact
    bool            kill_on_pit_fall = false; // NPC will die on falling into pit

    bool            kill_by_fireball = false;//    kill-fireball=1		; kill on collision with NPC, marked as "fireball"
    bool            freeze_by_iceball = false;//    kill-iceball=1		; freeze on collision with NPC, marked as "iceball"
    bool            kill_hammer = false;   //    kill-hammer=1		; kill on collision with NPC, marked as "hammer" or "boomerang"
    bool            kill_tail = false;     //    kill-tail=1		; kill on tail attack
    bool            kill_by_spinjump = false;//    kill-spin=1		; kill on spin jump
    bool            kill_by_statue = false;    //    kill-statue=1		; kill on tanooki statue fall
    bool            kill_by_mounted_item = false;  //    kill-with-mounted=1	; kill on jump with mounted items
    bool            kill_on_eat = false;   //    kill-on-eat=1		; Kill on eat, or transform into other
    bool            turn_on_cliff_detect = false;  //    cliffturn=0		; NPC turns on cliff
    bool            lava_protect = false;  //    lava-protection=0	; NPC will not be burn in lava

    bool            is_star = false; //If this marker was set, this NPC will be markered as "star"
                             //Quantity placed NPC's with marker "star" will be save in LVL-file

    bool            exit_is = false;       //This NPC is an exit
    int32_t         exit_walk_direction = false;   //Direction to walk offscreen (if 0 - player will don't walk
    int32_t         exit_code = false;             //Trigger exit code
    int32_t         exit_delay = false;            //Time to wait after triggering of exit
    int32_t         exit_snd = false;              //Custom exit sound if code more than 10

    bool            climbable = false;             //this is a climbable NPC

    //Editor defaults
    bool            default_friendly = false;
    bool            default_friendly_value = false;

    bool            default_nomovable = false;
    bool            default_nomovable_value = false;

    bool            default_boss = false;
    bool            default_boss_value = false;

    bool            default_special = false;
    int64_t         default_special_value = 0;
};

#endif //CONFIG_NPC_H
