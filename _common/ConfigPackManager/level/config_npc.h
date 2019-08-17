/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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

#pragma once
#ifndef CONFIG_NPC_H
#define CONFIG_NPC_H

#include <PGEString.h>
#include <stdint.h>

class IniProcessing;
struct NPCConfigFile;

struct NpcSetup
{
    /**
     * @brief Parse opened NPC config INI section
     * @param setup [_in] pointer to initialized INI manager with opened NPC section
     * @param npcImgPath [_in] path to NPC images folder
     * @param merge_with [_in] Merge settings with one other
     * @param error [_out] Error string
     * @return true if config parsed successfully, false on erros
     */
    bool parse(IniProcessing *setup,
               PGEString npcImgPath,
               uint32_t defaultGrid,
               const NpcSetup* merge_with = nullptr,
               PGEString *error=nullptr);

    /**
     * @brief Merges SMBX64-NPC.txt structure with default global NPC-configuration of specific NPC-ID and returns merged configuration
     * @param local SMBX64-NPC.txt structure readen from a file
     * @param global Default NPC configuration for specific NPC-ID
     */
    void applyNPCtxt(const NPCConfigFile *local,
                     const NpcSetup &global,
                     uint32_t captured_w = 0,
                     uint32_t captured_h = 0);

    //! Element type ID
    uint64_t        id = 0;
    //! Understandible name of element
    PGEString       name;
    //! Filter group name
    PGEString       group = "_NoGroup";
    //! Filter category name
    PGEString       category = "_Other";
    //! Detailed description of element
    PGEString       description = "";
    //! Main sprite image file
    PGEString       image_n;
    //! (Optional) Main sprite mask image file
    PGEString       mask_n;
    //! (Optional) In-editor icon for item boxes
    PGEString       icon_n;
    //! Filename of the NPC AI Lua controller script
    PGEString       algorithm_script;
    //!Name of global level function to process NPC death effect
    PGEString       effect_function;
    //!LEGACY: effect on stomp death reason
    uint32_t        effect_1 = 0;
    //!LEGACY: effect on other death reasons except lava burn
    uint32_t        effect_2 = 0;

    //    ; graphics
    //! Offset X of sprite relative to center<->center position (dependent to direction, may be multipled to -1)
    int32_t         gfx_offset_x = 0;
    //! Offset Y of sprite relative to bottom<->bottom position of NPC
    int32_t         gfx_offset_y = 0;
    //! Height of frame (if not defined, will be calculated from count of the frames)
    uint32_t        gfx_h = 32;
    //! Width of frame (if not defined, will be copied from actual sprite image width)
    uint32_t        gfx_w = 32;
    //! Allow automatic mapping of the customized hitbox metrics to frame size
    bool            custom_physics_to_gfx = false;
    //! Size of alignment grid while placing mode
    uint32_t        grid = 32;          //    grid=32
    //! Offset X relative aligned position
    int32_t         grid_offset_x = 0;  //    grid-offset-x=0
    //! Offset Y relative aligned position
    int32_t         grid_offset_y = 0;  //    grid-offset-y=0
    //! 0 - Align NPCX position relative it's center to center of the grid cell, 1 - to edge of grid cell
    uint32_t        grid_attach_style = 0; //0 - to center, 1 - to edge

    //! Style of frame sets
    uint32_t        framestyle = 0;   //    frame-style=0	; (0-2) This option in some alhoritmes can be ignored
    //! Total count of frames (Total frames on the sprite formula is: Ntf = Fn/(2^Fs) )
    uint32_t        frames = 1;    //    frames=2
    //! Delay between frames in milli-seconds
    uint32_t        framespeed = 80;//    frame-speed=128
    //! Render NPC's frame on NPC-Foreground layer (if both foreground and background are false - render on regular NPC render layer)
    bool            foreground = false;        //    foreground=0
    //! Render NPC's frame on NPC-Background layer
    bool            background = false;        //    background=0
    //! Relative Z-Offset at current render layer
    double          z_offset = 0.0;
    //! Bidirectional animation
    bool            ani_bidir = false;         //    animation-bidirectional=0
    //! Initial direction of animation sequence (0 - forward, 1 - backward)
    bool            ani_direct = false;        //    animation-direction=0
    //! Direction of animation sequence will be dependent from physical face direction
    bool            ani_directed_direct = false; //Animation direction will be gotten from NPC's direction

    //! [In-Editor] Turn on custom animation algorithm
    bool            custom_animate = false;//custom-animation=0
                        //; this option useful for non-standart algorithmic sprites (for example, bosses)

    //! [In-Editor] Custom animation algorithm type
    int32_t         custom_ani_alg = 0;
                        //0 simple frame range, 1 - frame Jump; 2 - custom animation sequances
    //! [In-Editor] Custom animation first frame for LEFT face direction
    int32_t         custom_ani_fl = 0; //    ;custom-animation-fl=0		; First frame for LEFT
    //! [In-Editor] Custom animation last frame for LEFT face direction (or Jump Step)
    int32_t         custom_ani_el = 0;
    //! [In-Editor] Custom animation first frame for RIGHT face direction
    int32_t         custom_ani_fr = 0;
    //! [In-Editor] Custom animation last frame for RIGHT face direction (or Jump Step)
    int32_t         custom_ani_er = 0;

    //! Array of 32-bit integers
    typedef PGEList<int32_t> IntArray;
    //! Array of 64-bit integers
    typedef PGEList<int64_t> LongArray;

    //! Custom animation sequence for left face direction
    IntArray        frames_left;     //Frame srquence for left
    //! Custom animation sequence for right face direction
    IntArray        frames_right;    //Frame srquence for right

    //! NPC can containing inside other NPC (need enable special option type 2)
    bool            container = false;

    /*PGE Engine side things*/
    //! ID of contained NPC
    uint32_t        contents_id = 0;
    //! Elastic sprite of container (like sizable block)
    bool            container_elastic = false;
    //! Width of border to draw elastic container
    uint32_t        container_elastic_border_w = 0;
    //! Show contents of container
    bool            container_show_contents = false;
    //! Render target sprite with next offset
    double          container_content_z_offset = 0.0;
    //! Crop contents GFX size to the size of an item
    bool            container_crop_contents = false;
    //! align contents to center-0 or top-1
    uint32_t        container_align_contents = 0;
    /************************/

    //! Initial frame to show when animation is turned off
    uint32_t        display_frame = 0;

    //! [In-Editor] Disable collisions in editor with other NPCs, but with NPC's of same ID collisions will be checked
    bool            no_npc_collisions = false;

    //! [In-Editor] Has special option
    bool            special_option = false;
    //! Title of the Special Option
    PGEString       special_name;
    //! Type of the special option: 0 combobox, 1 - integer spinbox, 2 - npc-id
    uint32_t        special_type = 0;
    //! Special option combobox entries
    PGEStringList   special_combobox_opts;
    //! Special option Spinbox: minimal value
    int32_t         special_spin_min = 0;
    //! Special option Spinbox: maximal value
    int32_t         special_spin_max = 0;
    //! Special option Spinbox: spining value step
    int32_t         special_spin_value_offset = 0;

    //! Extra Settings JSON layout file name
    PGEString       extra_settings = "";

    //;game process
    //! [Gameplay] Scores code are will be added to player on beating this NPC
    uint32_t        score = 0;
    //! [Gameplay] Coins count are will be added to player on beating this NPC
    int32_t         coins = 0;

    //! Motion speed in 1/65 pixels per second
    double          speed = 0.0;
    //! Let NPC simply moving right/left
    bool            movement = false;
    //! Make NPC be statical body as blocks
    bool            scenery = false;
    //! NPC can't be destroyed
    bool            immortal = false;
    //! NPC will keep it's position after deactivation and will not be returned back on re-activation
    bool            keep_position = false;
    //! NPC supports active processing and loops. When flag is off, NPC will be deactivated immediately on screen off.
    bool            activity = false;
    //! NPC will use same animation instance with all NPCs of this type
    bool            shared_ani = false;
    //! [Gameplay] NPC can be "eaten"
    bool            can_be_eaten = false;
    //! [Gameplay] NPC will be destroyed on contact with a playable character
    bool            takable = false;
    //! [Gameplay] Play sound on destroy when takable mode is turned on
    uint32_t        takable_snd = 0;
    //! [Gameplay] NPC can be grabbed from a side
    bool            grab_side = false;
    //! [Gameplay] NPC can be grabbed from the top
    bool            grab_top = false;
    //! [Gameplay] NPC can be grabbed from any contacted side or place
    bool            grab_any = false;
    //! Initial NPC health value
    uint32_t        health = 1;
    //! [Gameplay] Hurt player on contact
    bool            hurt_player = false;
    //! [Gameplay] Hurt player on attempt to stomp this NPC
    bool            hurt_player_on_stomp = false;
    //! [Gameplay] Hurt player on attempt to apply a spin-stomp attack
    bool            hurt_player_on_spinstomp = false;
    //! [Gameplay] Hurt another npc on contact
    bool            hurt_npc = false;
    //! [Gameplay] Damage sensitivity from stomp attak
    int32_t         damage_stomp = 0;
    //! [Gameplay] Damage sensitivity from spin-stomp attak
    int32_t         damage_spinstomp = 0;
    //! [Gameplay] Damage sensitivity from kick by thrown item (other NPC) attak
    int32_t         damage_itemkick = 0;

    //Hardcoded sound effects
    //! Play sound on damage receive
    uint32_t        hit_sound_id = 0;
    //! Play sound od NPC death
    uint32_t        death_sound_id = 0;

    //;Editor featured
    //! [In-Editor] Alternative title for "Face direction" field
    PGEString       direct_alt_title;
    //! [In-Editor] Alternative title for "Left face direction" value
    PGEString       direct_alt_left;
    //! [In-Editor] Alternative title for "Right face direction" value
    PGEString       direct_alt_right;
    //! [In-Editor] Alternative title for "Random face direction" value
    PGEString       direct_alt_rand;
    //! [In-Editor] Disable support for "Random" face direction value
    bool            direct_disable_random = false;
    //! [In-Editor] Default value for direction field
    int32_t         direct_default_value = -1;

//    ; Physics
    //! Physical height of NPC hitbox
    uint32_t        height = 32; //    ; Size of NPC's body (Collision box)
    //! Physical width of NPC hitbox
    uint32_t        width = 32;
    //! NPC is a solid object for other NPCs from sides
    bool            block_npc = false;
    //! Other NPCs can stay on the top of this NPC
    bool            block_npc_top = false;
    //! NPC is a solid object for playable characters from sides
    bool            block_player = false;
    //! Playable character can stay on the top of this NPC
    bool            block_player_top = false;
    //! NPC can collide with blocks
    bool            collision_with_blocks = false;
    //! NPC is affected by the gravity
    bool            gravity = false;
    //! NPC allowed to walk on walls and ceilings
    bool            adhesion = false;
    //! Contact zone padding. Negative value will increase "contactable zone" (non-collidable contacts only)
    double          contact_padding = 0.0;

//    ;Events
    //! Enable self-deactivation on specified condition
    bool            deactivation = true;
    //! Deactivation timeout in milliseconds.
    uint32_t        deactivationDelay = 4000;
    //! Deactivation of NPC that outed off the section box
    bool            deactivate_off_room = false;
    //! [Gameplay] Bump playalbe character which have stomped this NPC
    bool            bump_on_stomp = false;
    //! [Gameplay] NPC can be killed by sliding playable character
    bool            kill_slide_slope = false;
    //! [Gameplay] NPC can be stomped
    bool            kill_on_jump = false;
    //! [Gameplay] NPC can be killed by another NPC that has enabled harming of other NPCs
    bool            kill_by_npc = false;
    //! [Gameplay] NPC will die on falling into a pit
    bool            kill_on_pit_fall = false;

    //! [Gameplay] NPC can be killed by the fire attack
    bool            kill_by_fireball = false;
    //! [Gameplay] NPC can be frozen by the ice attack
    bool            freeze_by_iceball = false;
    //! [Gameplay] NPC can be killed by the hammer attack
    bool            kill_hammer = false;
    //! [Gameplay] NPC can be whipped by the whip attack
    bool            kill_tail = false;
    //! [Gameplay] NPC can be killed by the spining stomp attack
    bool            kill_by_spinjump = false;
    //! [Gameplay] NPC can be stomped by the hard stomp attack
    bool            kill_by_statue = false;
    //! [Gameplay] NPC can be killed by players on a vehicle
    bool            kill_by_mounted_item = false;
    //! [Gameplay] NPC will die when it will be eaten
    bool            kill_on_eat = false;
    //! NPC will turn facing direction on reaching the edge of a floor
    bool            turn_on_cliff_detect = false;
    //! [Gameplay] NPC will never burn in the lava
    bool            lava_protect = false;

    //! [Gameplay, In-Editor] NPC is the SMBX64 Star object
    bool            is_star = false; //If this marker was set, this NPC will be markered as "star"
                             //Quantity placed NPC's with marker "star" will be save in LVL-file
    //! Trigger level exit on killing / taking this NPC
    bool            exit_is = false;       //This NPC is an exit
    //! On level exit: Direction to auto-walk playable character when NPC was taken / killed. 0 - don't walk.
    int32_t         exit_walk_direction = 0;
    //! On level exit: Exit code will be returned into the world map
    int32_t         exit_code = 0;
    //! On level exit: Waiting delay in milliseconds until level exiting will happen
    int32_t         exit_delay = 0;
    //! On level exit: Play sound ID
    int32_t         exit_snd = 0;

    //! NPC is able to climb on climbable surfaces (provided by some BGOs or NPCs)
    bool            climbable = false;

    //Editor defaults
    //! [In-Editor] Force default friendly flag value for new-placing elements
    bool            default_friendly = false;
    //! [In-Editor] Default friendly flag value
    bool            default_friendly_value = false;

    //! [In-Editor] Force default non-movable flag value for new-placing elements
    bool            default_nomovable = false;
    //! [In-Editor] Default non-movable flag value
    bool            default_nomovable_value = false;

    //! [In-Editor] Force default "Is Boss" flag value for new-placing elements
    bool            default_boss = false;
    //! [In-Editor] Default "Is Boss" flag value
    bool            default_boss_value = false;

    //! [In-Editor] Force default special value value for new-placing elements
    bool            default_special = false;
    //! [In-Editor] Default special value value
    int64_t         default_special_value = 0;
};

#endif //CONFIG_NPC_H
