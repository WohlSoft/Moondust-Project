/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <QBitmap>

struct obj_npc
{
//    [npc-1]
    unsigned long id;
//    name="Goomba"
    QString name;
//    group="Enemy" 		;The sort category
    QString group;
//    category="Enemy"		;The sort category
    QString category;
//    image="npc-1.gif"		;NPC Image file
    QString image_n;
    QString mask_n;
    QPixmap image;
    QPixmap mask;
//    algorithm="0"			;NPC's alhorytm. Alhoritm have states and events (onDie, onTail, onCollisionWithFlyBlock...)
    int algorithm;
    //    ;If algorithm = 0, will using basic parametric alhorythm.
//    ;Else, get alhorythm from list
//    default-effect=2		;Spawn effect ID on jump-die
    unsigned long effect_1;
//    shell-effect=4			;effect on kick by shell or other NPC
    unsigned long effect_2;

//    ; graphics
    int gfx_offset_x;
//    gfx-offst-x=0
    int gfx_offset_y;
//    gfx-offst-y=2
    int gfx_h;
//    gfx-height-y=32
    int gfx_w;
//    gfx-width-y=32
    int grid;
//    grid=32
//    grid-offset-x=0
    int grid_offset_x;
//    grid-offset-y=0
    int grid_offset_y;

    int grid_attach_style; //0 - middle, 1 - left side

//    frame-style=0	; (0-2) This option in some alhoritmes can be ignored
    int framestyle;
//    frames=2
    unsigned int frames;
//    frame-speed=128
    unsigned int framespeed;
//    foreground=0
    bool foreground;
//    background=0
    bool background;
//    animation-bidirectional=0
    bool ani_bidir;
//    animation-direction=0
    bool ani_direct;

    bool ani_directed_direct; //Animation direction will be gotten from NPC's direction
//    ; for editor
//    custom-animation=0
    bool custom_animate;
//    ; this option useful for non-standart algorithmic sprites (for example, bosses)

//    ;custom-animation-alg=0		; Custom animation algorithm
    // 0 simple frame range, 1 - frame Jump
    int custom_ani_alg;
//    ;custom-animation-fl=0		; First frame for LEFT
    int custom_ani_fl;
//    ;custom-animation-el=0		; end frame for LEFT / Jump step
    int custom_ani_el;
//    ;custom-animation-fr=0		; first frame for RIGHT
    int custom_ani_fr;
//    ;custom-animation-er=0		; end frame for RIGHT / Jump step
    int custom_ani_er;

//    container=0			; NPC can containing inside other NPC (need enable special option type 2)
    bool container;

    bool no_npc_collions;
//    ; this option disabling collisions in editor with other NPCs, but with NPC's of same ID collisions will be checked

//    ; Special option
//    have-special=0			; Special NPC's option, what can used by NPC's algorithm
    bool special_option;
//    ;special-name="Cheep-cheep"	; 60

    QString special_name;
//    ;special-type=0			; 61 0 combobox, 1 - spin, 2 - npc-id
    int special_type;
//    ;special-combobox-size=3		; 62 quantity of options
    QStringList special_combobox_opts;
//    ;special-option-0="Swim"		; 63 set title for 0 option combobox
//    ;special-option-1="Jump"		; 64 set title for 1 option combobox
//    ;special-option-2="Projective"	; 65 set title for 2 option combobox
    int special_spin_min;
//    ;special-spin-min=0		; 66 milimal value of spin
    int special_spin_max;
//    ;special-spin-max=25		; 67 maximal value of spin
    int special_spin_value_offset;

//    have-special-2=0			; Special NPC's option, what can used by NPC's algorithm
    bool special_option_2; //Second special option
//    special-2-npc-spin-required
    QList<long > special_2_npc_spin_required;
//    special-2-npc-box-required
    QList<long > special_2_npc_box_required;

//    ;special-2-name="Cheep-cheep"	; 60
    QString special_2_name;
//    ;special-2-type=0			; 61 0 combobox, 1 - spin
    int special_2_type;
//    ;special-combobox-size=3		; 62 quantity of options
    QStringList special_2_combobox_opts;
//    ;special-option-0="Swim"		; 63 set title for 0 option combobox
//    ;special-option-1="Jump"		; 64 set title for 1 option combobox
//    ;special-option-2="Projective"	; 65 set title for 2 option combobox
    int special_2_spin_min;
//    ;special-2-spin-min=0		; 66 milimal value of spin
    int special_2_spin_max;
//    ;special-2-spin-max=25		; 67 maximal value of spin
    int special_2_spin_value_offset;
    //special-2-spin-value-offset

//    ;game process
//    score=2				; Add scores to player (value 0-13)
//    ; 0, 10, 100, 200, 400, 800, 1000, 2000, 4000, 8000, 1up, 2up, 5up, 3up
    int score;
//    speed=64			; Default movement speed in px/s
    int speed;
//    moving=1			; NPC simply moving right/left

    bool movement;
//    scenery=0			; NPC as block
    bool scenery;
//    immortal=0			; NPC Can't be destroy
    bool immortal;
//    yoshicaneat=1			; NPC can be eaten by yoshi
    bool can_be_eaten;
//    takeble=0			; NPC destroyble on contact with player
    bool takable;
//    grab-side=0			; NPC can be grabbed on side
    bool grab_side;
//    grab-top=0			; NPC can be grabbed on top
    bool grab_top;
//    grab-any=0			; NPC can be grabbed on any collisions
    bool grab_any;
//    default-health=1		; NPC's health value
    int health;
//    hurtplayer=1			; Hurt player on contact
    bool hurt_player;
//    hurtnpc=0			; Hurt other npc on contact
    bool hurt_npc;

//    ;Editor featured
    QString direct_alt_title;
    QString direct_alt_left;
    QString direct_alt_right;
    bool direct_disable_random;

//    allow-bubble=1			; Allow packable into the bubble
    bool allow_bubble;
//    allow-egg=1			; Allow packable into the egg
    bool allow_egg;
//    allow-lakitu=1			; Allow packable into the SMW Lakitu
    bool allow_lakitu;
//    allow-burred=1			; Allow packable under the herb
    bool allow_buried;


//    ; Physics
//    ; Size of NPC's body (Collision box)
//    fixture-height=32
    unsigned int height;
//    fixture-width=32
    unsigned int width;
//    block-npc=1		; NPC is a solid object for NPC's
    bool block_npc;
//    block-npc-top=0		; on NPC's top can be stay other NPC's
    bool block_npc_top;
//    block-player=0		; NPC is a solid object for player
    bool block_player;
//    block-player-top=0	; on NPC's top can be stay player
    bool block_player_top;
//    collision-blocks=1	; Enable collisions with blocks
    bool collision_with_blocks;
//    gravity=1		; Enable gravitation for this NPC
    bool gravity;
//    adhesion=0		; allows to NPC walking on wall and on celling
    bool adhesion;

//    ;Events
//    deactivate=1		; Deactivate on state offscreen > 4 sec ago
    bool deactivation;
//    kill-slside=1		; Kill on Slope slide
    bool kill_slide_slope;
//    kill-onjump=1		; Kill on jump on NPC's head
    bool kill_on_jump;
//    kill-bynpc=1		; Kill by contact with other NPC with hurt-npc
//    ; for example: moving SHELL have "HURT_NPC", and shell kiling ALL NPCs on contact
    bool kill_by_npc;
//    kill-fireball=1		; kill on collision with NPC, marked as "fireball"
    bool kill_by_fireball;
//    kill-iceball=1		; freeze on collision with NPC, marked as "iceball"
    bool freeze_by_iceball;
//    kill-hammer=1		; kill on collision with NPC, marked as "hammer" or "boomerang"
    bool kill_hammer;
//    kill-tail=1		; kill on tail attack
    bool kill_tail;
//    kill-spin=1		; kill on spin jump
    bool kill_by_spinjump;
//    kill-statue=1		; kill on tanooki statue fall
    bool kill_by_statue;
//    kill-with-mounted=1	; kill on jump with mounted items
    bool kill_by_mounted_item;
//    kill-on-eat=1		; Kill on eat, or transform into other
    bool kill_on_eat;
//    cliffturn=0		; NPC turns on cliff
    bool turn_on_cliff_detect;
//    lava-protection=0	; NPC will not be burn in lava
    bool lava_protect;

    bool is_star; //If this marker was set, this NPC will be markered as "star"
    //Quantity placed NPC's with marker "star" will be save in LVL-file

};

#endif // OBJ_NPC_H
