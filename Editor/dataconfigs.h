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
    /*
    [block-1]
    image="block-1.gif"
    name="Wood"
    type="block"	; Cathegory for sort
    sizable=0	; can be resise
    danger=0	; 0 - none, 1 left, -1 right, 2 top, -2 bottom,
            ; 3 left-right, -3 top-bottom, 4 - all sides
    collision=1	; 1 - collision all sides, 2 collision only top side, 0 - no collision
    slope-slide=0
    fixture-type=0	; 0 sqear, 1 tringle up-left, -1 tringle up-right, 2 tringle down-left, -2 tringle down-right
            ; 3 fixture as image form
    lava=0		; lava fataly to all NPC's, what haven't lava-protection
    destruct=0	; Can by destruct with shell/head jump
    destruct-bomb=0	; Destroy on Bomb
    destruct-fireball=0	; Destroy on fireball
    spawn-on-destroy="0"	; Object type-ID for spawn after destroy, if 0 - nothing
    desctruct-effect=1 ; Destructble effect number, 0 - no effect
    bounce=0	; Jumpble block, if player stay in this block, he bouncing
    hitable=0	; can be hit with player; if 0, hitable only if block have contains NPC
    onhit=0		; 0 Nothing, 1 can be hited without contains NPC
    onhit-block=2	; if onhit=1, transform block to block of number 2,
            ; if 0, block will not by transoform, even if contains NPC
    algorithm=0	; Special block algorithm, for example, on/off switcher, invisible block,
            ; player switch block
    animate=0	; Enable animation
    frames=1	; Animation frames
    framespeed=128	; Delay per frime (ms)
    */
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
    /*
[background2-3]
image="background2-3.gif"//
name="Hills (3 + 2)"//
type=double-row//
repeat-h=2//
repeat-v=ZR//
attached=bottom//
animated=0..
frames=1..
magic=0..
magic-strips=1//
magic-splits=0//
magic-speeds=0//
second-image="background2-2.gif"//
second-repeat-h=4
second-repeat-v=ZR
second-attached=overfirst
    */

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

class dataconfigs
{
public:
    dataconfigs();
    bool loadconfigs(bool nobar=false);
    DataFolders dirs;

    QVector<obj_bgo > main_bgo;
    QVector<obj_BG > main_bg;
    QVector<obj_block > main_block;

    unsigned long music_custom_id;
    QVector<obj_music > main_music_lvl;
    QVector<obj_music > main_music_wld;
    QVector<obj_music > main_music_spc;

    //Indexes
    QVector<blocksIndexes > index_blocks;
    QVector<bgoIndexes > index_bgo;

};



#endif // DATACONFIGS_H
