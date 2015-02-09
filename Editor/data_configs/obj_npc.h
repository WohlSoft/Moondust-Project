/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef OBJ_NPC_H
#define OBJ_NPC_H

#include <QString>
#include <QPixmap>

// //Defines:// //
//  obj_npc     //
//  npc_Markers //
// //////////// //

struct obj_npc
{
    unsigned long id;   //    [npc-1]
    QString name;       //    name="Goomba"
    QString group;      //    group="Enemy" 		;The sort category
    QString category;   //    category="Enemy"		;The sort category
    QString image_n;    //    image="npc-1.gif"		;NPC Image file
    QString mask_n;
    QPixmap image;
    QPixmap mask;
    int algorithm;              //    algorithm="0"	;NPC's alhoritm. Alhoritm have states and events (onDie, onTail, onCollisionWithFlyBlock...)
    QString algorithm_script;   //    ;If algorithm = 0, will using basic parametric alhorythm.
                                //    ;Else, get alhorythm from list
    unsigned long effect_1;     //    default-effect=2		;Spawn effect ID on jump-die
    unsigned long effect_2;     //    shell-effect=4			;effect on kick by shell or other NPC

    //    ; graphics
    int gfx_offset_x;   //    gfx-offst-x=0
    int gfx_offset_y;   //    gfx-offst-y=2
    int gfx_h;          //    gfx-height-y=32
    int gfx_w;          //    gfx-width-y=32
    bool custom_physics_to_gfx; //The GFX size defining by physics size in the custom configs
    int grid;           //    grid=32
    int grid_offset_x;  //    grid-offset-x=0
    int grid_offset_y;  //    grid-offset-y=0

    int grid_attach_style; //0 - middle, 1 - left side

    int framestyle;     //    frame-style=0	; (0-2) This option in some alhoritmes can be ignored
    unsigned int frames;    //    frames=2
    unsigned int framespeed;//    frame-speed=128
    bool foreground;        //    foreground=0
    bool background;        //    background=0
    bool ani_bidir;         //    animation-bidirectional=0
    bool ani_direct;        //    animation-direction=0

    bool ani_directed_direct; //Animation direction will be gotten from NPC's direction

    //    ; for editor
    bool custom_animate;//custom-animation=0
                        //; this option useful for non-standart algorithmic sprites (for example, bosses)

    int custom_ani_alg; //;custom-animation-alg=0		; Custom animation algorithm
                        //0 simple frame range, 1 - frame Jump; 2 - custom animation sequances
    int custom_ani_fl; //    ;custom-animation-fl=0		; First frame for LEFT
    int custom_ani_el; //    ;custom-animation-el=0		; end frame for LEFT / Jump step
    int custom_ani_fr; //    ;custom-animation-fr=0		; first frame for RIGHT
    int custom_ani_er; //    ;custom-animation-er=0		; end frame for RIGHT / Jump step

    QList<int> frames_left;     //Frame srquence for left
    QList<int> frames_right;    //Frame srquence for right

    bool container;// container=0; NPC can containing inside other NPC (need enable special option type 2)

    unsigned int display_frame;

    bool no_npc_collions;//    ; this option disabling collisions in editor with other NPCs, but with NPC's of same ID collisions will be checked

    bool special_option;  //    ; Special option
                          //    have-special=0; Special NPC's option, what can used by NPC's algorithm
    QString special_name; //    ;special-name="Cheep-cheep"	; 60
    int special_type; //    ;special-type=0; 61 0 combobox, 1 - spin, 2 - npc-id
    QStringList special_combobox_opts;//;special-combobox-size=3; 62 quantity of options
                                      //;special-option-0="Swim"; 63 set title for 0 option combobox
                                      //;special-option-1="Jump"; 64 set title for 1 option combobox
                                      //;special-option-2="Projective"	; 65 set title for 2 option combobox
    int special_spin_min;   //    ;special-spin-min=0		; 66 milimal value of spin
    int special_spin_max;   //    ;special-spin-max=25		; 67 maximal value of spin
    int special_spin_value_offset;  //    have-special-2=0			; Special NPC's option, what can used by NPC's algorithm
    bool special_option_2; //   Second special option
                           //   special-2-npc-spin-required
    QList<long > special_2_npc_spin_required;   //    special-2-npc-box-required
    QList<long > special_2_npc_box_required;

    QString special_2_name; //    ;special-2-name="Cheep-cheep"	; 60
    int special_2_type; //    ;special-2-type=0			; 61 0 combobox, 1 - spin
    QStringList special_2_combobox_opts;    //;special-combobox-size=3		; 62 quantity of options
                                            //;special-option-0="Swim"		; 63 set title for 0 option combobox
                                            //;special-option-1="Jump"		; 64 set title for 1 option combobox
                                            //;special-option-2="Projective"	; 65 set title for 2 option combobox
    int special_2_spin_min; //    ;special-2-spin-min=0		; 66 milimal value of spin
    int special_2_spin_max; //    ;special-2-spin-max=25		; 67 maximal value of spin
    int special_2_spin_value_offset;    //special-2-spin-value-offset

    //;game process
    int score;//    score=2				; Add scores to player (value 0-13)
              //    ; 0, 10, 100, 200, 400, 800, 1000, 2000, 4000, 8000, 1up, 2up, 5up, 3up

    int speed;     //    speed=64			; Default movement speed in px/s
    bool movement; //    moving=1			; NPC simply moving right/left
    bool scenery;  //    scenery=0			; NPC as block
    bool immortal; //    immortal=0			; NPC Can't be destroy
    bool can_be_eaten; //    yoshicaneat=1			; NPC can be eaten by yoshi
    bool takable;  //    takeble=0			; NPC destroyble on contact with player
    bool grab_side;//    grab-side=0			; NPC can be grabbed on side
    bool grab_top; //    grab-top=0			; NPC can be grabbed on top
    bool grab_any; //    grab-any=0			; NPC can be grabbed on any collisions
    int health;    //    default-health=1		; NPC's health value
    bool hurt_player; //    hurtplayer=1			; Hurt player on contact
    bool hurt_npc; //    hurtnpc=0			; Hurt other npc on contact

    //;Editor featured
    QString direct_alt_title;
    QString direct_alt_left;
    QString direct_alt_right;
    bool direct_disable_random;

    bool allow_bubble;  //allow-bubble=1			; Allow packable into the bubble
    bool allow_egg;     //    allow-egg=1			; Allow packable into the egg
    bool allow_lakitu;  //    allow-lakitu=1			; Allow packable into the SMW Lakitu
    bool allow_buried;  //    allow-burred=1			; Allow packable under the herb

//    ; Physics
    unsigned int height; //    ; Size of NPC's body (Collision box)
                         //    fixture-height=32
    unsigned int width; //    fixture-width=32
    bool block_npc;     //    block-npc=1		; NPC is a solid object for NPC's
    bool block_npc_top; //    block-npc-top=0		; on NPC's top can be stay other NPC's
    bool block_player;  //    block-player=0		; NPC is a solid object for player
    bool block_player_top;  //    block-player-top=0	; on NPC's top can be stay player
    bool collision_with_blocks; //    collision-blocks=1	; Enable collisions with blocks
    bool gravity;       //    gravity=1		; Enable gravitation for this NPC
    bool adhesion;      //    adhesion=0		; allows to NPC walking on wall and on celling

//    ;Events
    bool deactivation;  //deactivate=1		; Deactivate on state offscreen > 4 sec ago
    bool kill_slide_slope;  //    kill-slside=1		; Kill on Slope slide
    bool kill_on_jump;  //    kill-onjump=1		; Kill on jump on NPC's head
    bool kill_by_npc;   //    kill-bynpc=1		; Kill by contact with other NPC with hurt-npc
                        //; for example: moving SHELL have "HURT_NPC", and shell kiling ALL NPCs on contact

    bool kill_by_fireball;//    kill-fireball=1		; kill on collision with NPC, marked as "fireball"
    bool freeze_by_iceball;//    kill-iceball=1		; freeze on collision with NPC, marked as "iceball"
    bool kill_hammer;   //    kill-hammer=1		; kill on collision with NPC, marked as "hammer" or "boomerang"
    bool kill_tail;     //    kill-tail=1		; kill on tail attack
    bool kill_by_spinjump;//    kill-spin=1		; kill on spin jump
    bool kill_by_statue;    //    kill-statue=1		; kill on tanooki statue fall
    bool kill_by_mounted_item;  //    kill-with-mounted=1	; kill on jump with mounted items
    bool kill_on_eat;   //    kill-on-eat=1		; Kill on eat, or transform into other
    bool turn_on_cliff_detect;  //    cliffturn=0		; NPC turns on cliff
    bool lava_protect;  //    lava-protection=0	; NPC will not be burn in lava

    bool is_star; //If this marker was set, this NPC will be markered as "star"
                  //Quantity placed NPC's with marker "star" will be save in LVL-file

    //Editor defaults
    bool default_friendly;
    bool default_friendly_value;

    bool default_nomovable;
    bool default_nomovable_value;

    bool default_boss;
    bool default_boss_value;

    bool default_special;
    long default_special_value;
};



struct npc_Markers
{
    //    ;Defines for SMBX64
    unsigned long bubble;   //    bubble=283	; NPC-Container for packed in bubble
    unsigned long egg;      //    egg=96		; NPC-Container for packed in egg
    unsigned long lakitu;   //    lakitu=284	; NPC-Container for spawn by lakitu
    unsigned long buried;   //    burred=91	; NPC-Container for packed in herb

    unsigned long ice_cube; //    icecube=263	; NPC-Container for frozen NPCs

    //    ;markers
    unsigned long iceball;  //    iceball=265
    unsigned long fireball; //    fireball=13
    unsigned long hammer;   //    hammer=171
    unsigned long boomerang;//    boomerang=292
    unsigned long coin_in_block; //    coin-in-block=10
};


#endif // OBJ_NPC_H
