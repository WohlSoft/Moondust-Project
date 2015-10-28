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

#ifndef DATACONFIGS_H
#define DATACONFIGS_H

#include <QVector>
#include <QHash>
#include <QObject>
#include <QPixmap>
#include <QBitmap>
#include <QtWidgets>
#include <QSettings>
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

struct obj_splash_ani
{
    QPixmap img;
    unsigned int frames;
    unsigned int speed;
    unsigned int x;
    unsigned int y;
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





class dataconfigs : public QObject
{
    Q_OBJECT
public:
    dataconfigs();
    virtual ~dataconfigs();
    bool loadconfigs();
    DataFolders dirs;
    QString config_dir;
    QString data_dir;

    QString splash_logo;
    QList<obj_splash_ani > animations;

    EngineSetup engine;

    //Playable Characters
    QList<obj_playable_character > characters;

    //Level map items
    QHash<int, obj_BG > main_bg;
    QList<obj_bgo > main_bgo;
    QList<obj_block > main_block;
    QList<obj_npc > main_npc;
    npc_Markers marker_npc;

    //Indexes
    QList<blocksIndexes > index_blocks;
    QList<bgoIndexes > index_bgo;
    QList<npcIndexes > index_npc;

    //World map items
    QList<obj_w_tile > main_wtiles;
    QList<obj_w_path > main_wpaths;
    QList<obj_w_scenery > main_wscene;
    QList<obj_w_level > main_wlevels;
    wld_levels_Markers marker_wlvl;

    //Indexes
    QList<wTileIndexes > index_wtiles;
    QList<wPathIndexes > index_wpaths;
    QList<wSceneIndexes > index_wscene;
    QList<wLevelIndexes > index_wlvl;

    //Common items
    unsigned long music_custom_id;
    unsigned long music_w_custom_id;
    QList<obj_music > main_music_lvl;
    QList<obj_music > main_music_wld;
    QList<obj_music > main_music_spc;

    QVector<obj_sound > main_sound;

    //Tilesets
    QList<SimpleTileset >      main_tilesets;
    QList<SimpleTilesetGroup > main_tilesets_grp;

    QList<obj_rotation_table > main_rotation_table;

    bool check(); //Returns true, if something config entry is not initialized

    //Graphics
    unsigned int default_grid;

    //Debug
    QStringList errorsList;

    // Get Item of Index
    long getNpcI(unsigned long itemID);
    long getBlockI(unsigned long itemID);
    long getBgoI(unsigned long itemID);

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

    bool loadLevelBackground(obj_BG &sbg, QString section, obj_BG *merge_with=0, QString iniFile="", QSettings *setup=0);
signals:
    void progressValue(int);
    void progressMax(int);
    void progressTitle(QString);

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

    void loadLevelBGO();
    void loadLevelBlocks();
    void loadLevelNPC();
    void loadLevelBackgrounds();

    void loadWorldTiles();
    void loadWorldScene();
    void loadWorldPaths();
    void loadWorldLevels();

    void loadMusic();
    void loadSound();

    void loadRotationTable();

    void addError(QString bug, QtMsgType level=QtWarningMsg);
};



#endif // DATACONFIGS_H
