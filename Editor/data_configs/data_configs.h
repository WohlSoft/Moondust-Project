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
#include <QDebug>

#include <common_features/logger.h>

#include "obj_block.h"
#include "obj_bgo.h"
#include "obj_npc.h"
#include "obj_BG.h"
#include "obj_wld_items.h"
#include "dc_indexing.h"
#include "obj_tilesets.h"
#include "obj_rotation_table.h"

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

struct EngineSetup
{
    int screen_w;
    int screen_h;
    int wld_viewport_w;
    int wld_viewport_h;
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

struct obj_playable_character
{
    unsigned long id;
    QString name;
};





class dataconfigs
{
public:
    dataconfigs();
    bool loadconfigs(QProgressDialog *prgs=NULL);
    DataFolders dirs;
    QString config_dir;
    QString data_dir;

    QString splash_logo;

    EngineSetup engine;

    //Playable Characters
    QVector<obj_playable_character > characters;

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

    //Tilesets
    QVector<SimpleTileset >      main_tilesets;
    QVector<SimpleTilesetGroup > main_tilesets_grp;

    QVector<obj_rotation_table > main_rotation_table;

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
    long getCharacterI(unsigned long itemID);

    long getSndI(unsigned long itemID);
    long getMusLvlI(unsigned long itemID);
    long getMusWldI(unsigned long itemID);
    long getMusSpcI(unsigned long itemID);


    void  loadTilesets();

    void setConfigPath(QString p);
    void loadBasics();

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

    void loadRotationTable();

    void addError(QString bug, QtMsgType level=QtWarningMsg);
};



#endif // DATACONFIGS_H
