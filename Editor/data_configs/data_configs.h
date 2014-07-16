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

#include "obj_npc.h"


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
    //    [background-1]
    unsigned long id;
    //    name="Smallest bush"		;background name, default="background-%n"
    QString name;
    //    group="scenery"			;Background group, default="All about my pigeon"
    QString group;
    //    category="scenery"		;Background categoty, default="Scenery"
    QString category;
    //    grid=32				; 32 | 16 Default="32"
    unsigned int grid;
    //    view=background			; background | foreground, default="background"
    unsigned int view;
    int offsetX;
    int offsetY;
    int zOffset;
    //    image="background-1.gif"	;Image file with level file ; the image mask will be have *m.gif name.
    QString image_n;
    QString mask_n;
    QPixmap image;
    QPixmap mask;
    //    climbing=0			; default = 0
    bool climbing;
    //    animated = 0			; default = 0 - no
    bool animated;
    //    frames = 1			; default = 1
    unsigned int frames;
    //    frame-speed=125			; default = 125 ms, etc. 8 frames per sec
    unsigned int framespeed;
};

struct obj_block{
    unsigned long id;
        QString image_n;
        QString mask_n;
        QPixmap image;
        QPixmap mask;
    QString name;
    //    grid=32				; 32 | 16 Default="32"
    unsigned int grid;
    QString group;
    QString category;
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
    bool animation_rev; //Reverse animation
    bool animation_bid; //Bidirectional animation
    unsigned int frames;
    int framespeed;

    //Editor defaults
    bool default_slippery; //Slippery flag
    bool default_slippery_value;

    bool default_invisible; //Invisible flag
    bool default_invisible_value;

    bool default_content; //Content value
    long default_content_value;

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

//////////////Indexing objects////////////////
struct blocksIndexes
{
    unsigned long i; //Target array index
    unsigned long ai; //Animator array index
    unsigned int type;//0 - internal GFX, 1 - user defined GFX
};

struct bgoIndexes
{
    unsigned long i; //Target array index
    unsigned long ai; //Animator array index
    unsigned int type;//0 - internal GFX, 1 - user defined GFX
    long smbx64_sp;//smbx64-sort-priority, array sorting priority, need for compatible with SMBX 1.3
};

struct npcIndexes
{
    unsigned long i; //Target array index
    unsigned long gi; //Target array index by GlobalConfig
    unsigned int type;//0 - internal GFX, 1 - user defined GFX
};



////////////////////World map items///////////////////////////
struct obj_w_tile{
    unsigned long id;
        QString image_n;
        QString mask_n;
        QPixmap image;
        QPixmap mask;

    QString group;
    QString category;
    bool animated;
    unsigned int frames;
    unsigned int framespeed; // Default 128 ms
};

struct obj_w_scenery{
    unsigned long id;
        QString image_n;
        QString mask_n;
        QPixmap image;
        QPixmap mask;
    QString group;
    QString category;
    bool animated;
    unsigned int frames;
    unsigned int framespeed; // Default 128 ms
};

struct obj_w_level{
    unsigned long id;
        QString image_n;
        QString mask_n;
        QPixmap image;
        QPixmap mask;
    QString group;
    QString category;
    bool animated;
    unsigned int frames;
    unsigned int framespeed; // Default 128 ms
};

struct obj_w_path{
    unsigned long id;
        QString image_n;
        QString mask_n;
        QPixmap image;
        QPixmap mask;
    QString group;
    QString category;
    bool animated;
    unsigned int frames;
    unsigned int framespeed; // Default 128 ms
};

//////////////Indexing objects////////////////
struct wTileIndexes
{
    unsigned long i; //Target array index
    unsigned long ai; //Animator array index
    unsigned int type;//0 - internal GFX, 1 - user defined GFX
};

struct wSceneIndexes
{
    unsigned long i; //Target array index
    unsigned long ai; //Animator array index
    unsigned int type;//0 - internal GFX, 1 - user defined GFX
};

struct wLevelIndexes
{
    unsigned long i; //Target array index
    unsigned long ai; //Animator array index
    unsigned int type;//0 - internal GFX, 1 - user defined GFX
};
struct wPathIndexes
{
    unsigned long i; //Target array index
    unsigned long ai; //Animator array index
    unsigned int type;//0 - internal GFX, 1 - user defined GFX
};


//Markers
struct wld_levels_Markers
{
    unsigned long path;
    unsigned long bigpath;
};




////////////////////Common items///////////////////////////
struct obj_music
{
    unsigned long id;
    QString name;
    QString file;
};

struct obj_sound
{
    unsigned long id;
    QString name;
    QString file;
    bool hidden;
};





class dataconfigs
{
public:
    dataconfigs();
    bool loadconfigs(QProgressDialog *prgs=NULL);
    DataFolders dirs;
    QString config_dir;

    //Level map items
    QVector<obj_BG > main_bg;
    QVector<obj_bgo > main_bgo;
    QVector<obj_block > main_block;
    QVector<obj_npc > main_npc;
    npc_Markers marker_npc;

    //Indexes
    QVector<blocksIndexes > index_blocks;
    QVector<bgoIndexes > index_bgo;
    QVector<npcIndexes > index_npc;

    //World map items
    QVector<obj_w_tile > main_wtiles;
    QVector<obj_w_path > main_wpaths;
    QVector<obj_w_scenery > main_wscene;
    QVector<obj_w_level > main_wlevels;
    wld_levels_Markers marker_wlvl;

    //Indexes
    QVector<wTileIndexes > index_wtiles;
    QVector<wPathIndexes > index_wpaths;
    QVector<wSceneIndexes > index_wscene;
    QVector<wLevelIndexes > index_wlvl;

    //Common items
    unsigned long music_custom_id;
    unsigned long music_w_custom_id;
    QVector<obj_music > main_music_lvl;
    QVector<obj_music > main_music_wld;
    QVector<obj_music > main_music_spc;

    QVector<obj_sound > main_sound;

    bool check(); //Returns true, if something config entry is not initialized

    //Graphics
    unsigned int default_grid;

    //Debug
    QStringList errorsList;

    // Get Item of Index
    long getNpcI(unsigned long itemID);
    long getBlockI(unsigned long itemID);
    long getBgoI(unsigned long itemID);
    long getBgI(unsigned long itemID);

    long getTileI(unsigned long itemID);
    long getSceneI(unsigned long itemID);
    long getPathI(unsigned long itemID);
    long getWLevelI(unsigned long itemID);

    long getSndI(unsigned long itemID);
    long getMusLvlI(unsigned long itemID);
    long getMusWldI(unsigned long itemID);
    long getMusSpcI(unsigned long itemID);

private:

    //Buffers
    QPixmap mask;
    //QPixmap image;
    QString imgFile, imgFileM;
    QString tmpstr;
    QStringList tmp;

    unsigned long total_data;
    QString bgoPath;
    QString BGPath;
    QString blockPath;
    QString npcPath;

    QString tilePath;
    QString scenePath;
    QString pathPath;
    QString wlvlPath;

    void loadLevelBGO(QProgressDialog *prgs=NULL);
    void loadLevelBlocks(QProgressDialog *prgs=NULL);
    void loadLevelNPC(QProgressDialog *prgs=NULL);
    void loadLevelBackgrounds(QProgressDialog *prgs=NULL);

    void loadWorldTiles(QProgressDialog *prgs=NULL);
    void loadWorldScene(QProgressDialog *prgs=NULL);
    void loadWorldPaths(QProgressDialog *prgs=NULL);
    void loadWorldLevels(QProgressDialog *prgs=NULL);

    void loadMusic(QProgressDialog *prgs=NULL);
    void loadSound(QProgressDialog *prgs=NULL);

    void addError(QString bug, QtMsgType level=QtWarningMsg);
};



#endif // DATACONFIGS_H
