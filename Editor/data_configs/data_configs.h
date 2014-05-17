/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#ifndef DATACONFIGS_H
#define DATACONFIGS_H
#include <QVector>
#include <QPixmap>
#include <QBitmap>

#include <QtWidgets>
#include <QSettings>
#include <QProgressDialog>
#include "../common_features/logger.h"

#include <QDebug>

struct DataFolders
{
    QString worlds;

    QString music;
    QString sounds;

    QString glevel;
    QString gworld;
    QString gplayble;

    QString gcustom;
};

struct obj_bgo{
    /*
    [background-1]
    name="Smallest bush"		;background name, default="background-%n"
    type="scenery"			;Background type, default="Scenery"
    grid=32				; 32 | 16 Default="32"
    view=background			; background | foreground, default="background"
    image="background-1.gif"	;Image file with level file ; the image mask will be have *m.gif name.
    climbing=0			; default = 0
    animated = 0			; default = 0 - no
    frames = 1			; default = 1
    frame-speed=125			; default = 125 ms, etc. 8 frames per sec
    */
    unsigned long id;
    QString name;
    QString type;
    unsigned int grid;
    unsigned int view;
    int offsetX;
    int offsetY;
    int zOffset;
    QString image_n;
    QString mask_n;
    QPixmap image;
    QBitmap mask;
    bool climbing;
    bool animated;
    unsigned int frames;
    unsigned int framespeed;
};

struct obj_block{
    unsigned long id;
        QString image_n;
        QString mask_n;
        QPixmap image;
        QBitmap mask;
    QString name;
    QString type;
    bool sizable;
    int danger;
    int collision;
    bool slopeslide;
    int fixture;
    bool lava;
    bool destruct;
    bool dest_bomb;
    bool dest_fire;
    bool spawn; //split string by "-" in != "0"
        int spawn_obj; // 1 - NPC, 2 - block, 3 - BGO
        unsigned long spawn_obj_id;
    unsigned long effect;
    bool bounce;
    bool hitable;
    bool onhit;
    unsigned long onhit_block;
    unsigned long algorithm;

    unsigned int view;
    bool animated;
    unsigned int frames;
    int framespeed;
};

struct obj_BG{

    unsigned long id;
    QString name;

    QString image_n;

    QPixmap image;
    unsigned int type;//convert from string
    float repeat_h;
    unsigned int repead_v;
    unsigned int attached;
    bool editing_tiled;
    bool animated;
    unsigned int frames;

    bool magic;
    unsigned int magic_strips;
    QString magic_splits;
    QString magic_speeds;

    QString second_image_n;
    QPixmap second_image;

    float second_repeat_h;
    unsigned int second_repeat_v;
    unsigned int second_attached;

};

struct obj_npc
{
//    [npc-1]
    unsigned long id;
//    name="Goomba"
    QString name;
//    category="Enemy"		;The sort category
    QString category;
//    image="npc-1.gif"		;NPC Image file
    QString image_n;
    QString mask_n;
    QPixmap image;
    QBitmap mask;
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
//    frame-style=0	; (0-2) This option in some alhoritmes can be ignored
    int framestyle;
//    frames=2
    unsigned int frames;
//    frame-speed=128
    unsigned int framespeed;
//    foreground=0
    bool foreground;
//    animation-bidirectional=0
    bool ani_bidir;
//    animation-direction=0
    bool ani_direct;
//    ; for editor
//    custom-animation=0
    bool custom_animate;
//    ; this option useful for non-standart algorithmic sprites (for example, bosses)

//    ;custom-animation-alg=0		; Custom animation algorithm - 0 simple frame range, 1 - frame Jump
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

};

struct npc_Markers
{
//    ;Defines for SMBX64
    unsigned long bubble;
//    bubble=283	; NPC-Container for packed in bubble
    unsigned long egg;
//    egg=96		; NPC-Container for packed in egg
    unsigned long lakitu;
//    lakitu=284	; NPC-Container for spawn by lakitu
    unsigned long buried;
//    burred=91	; NPC-Container for packed in herb

    unsigned long ice_cube;
//    icecube=263	; NPC-Container for frozen NPCs

//    ;markers
//    iceball=265
    unsigned long iceball;
//    fireball=13
    unsigned long fireball;
//    hammer=171
    unsigned long hammer;
//    boomerang=292
    unsigned long boomerang;
//    coin-in-block=10
    unsigned long coin_in_block;

};

struct obj_music
{
    unsigned long id;
    QString name;
    QString file;
};

//////////////Indexing objects////////////////
struct blocksIndexes
{
    unsigned long i; //Target array index
    unsigned int type;//0 - internal GFX, 1 - user defined GFX
};

struct bgoIndexes
{
    unsigned long i; //Target array index
    unsigned int type;//0 - internal GFX, 1 - user defined GFX
};

struct npcIndexes
{
    unsigned long i; //Target array index
    unsigned long gi; //Target array index by GlobalConfig
    unsigned int type;//0 - internal GFX, 1 - user defined GFX
};

class dataconfigs
{
public:
    dataconfigs();
    bool loadconfigs();
    DataFolders dirs;

    QVector<obj_BG > main_bg;

    QVector<obj_bgo > main_bgo;
    QVector<obj_block > main_block;

    QVector<obj_npc > main_npc;
    npc_Markers marker_npc;

    unsigned long music_custom_id;
    QVector<obj_music > main_music_lvl;
    QVector<obj_music > main_music_wld;
    QVector<obj_music > main_music_spc;

    //Indexes
    QVector<blocksIndexes > index_blocks;
    QVector<bgoIndexes > index_bgo;
    QVector<npcIndexes > index_npc;

private:

    //Buffers
    QBitmap mask;
    //QPixmap image;
    QString imgFile, imgFileM;
    QString tmpstr;
    QStringList tmp;

    QString config_dir;
    unsigned long total_data;
    QString bgoPath;
    QString BGPath;
    QString blockPath;
    QString npcPath;


    void loadLevelBGO();
    void loadLevelBlocks();
    void loadLevelNPC();
    void loadLevelBackgrounds();

    void loadMusic();
};



#endif // DATACONFIGS_H
