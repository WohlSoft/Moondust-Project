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
#ifndef DATACONFIGS_H
#define DATACONFIGS_H

#include <QVector>
#include <QHash>
#include <QWidget>
#include <QObject>
#include <QPixmap>
#include <QBitmap>
#include <QSettings>
#include <PGEString.h>
#include <IniProcessor/ini_processing.h>

#include <common_features/logger.h>
#include <common_features/data_array.h>

#include "obj_block.h"
#include "obj_bgo.h"
#include "obj_npc.h"
#include "obj_BG.h"
#include "obj_player.h"
#include "obj_wld_items.h"
#include "obj_tilesets.h"
#include "obj_custom_property.h"
#include "obj_rotation_table.h"

struct DataFolders
{
    PGEString worlds;

    PGEString music;
    PGEString sounds;

    PGEString glevel;
    PGEString gworld;
    PGEString gplayble;

    PGEString gcustom;
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
    QPixmap      img;
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

struct obj_gridSizes
{
    unsigned int general;
    unsigned int block;
    unsigned int bgo;
    unsigned int npc;
    unsigned int terrain;
    unsigned int scenery;
    unsigned int paths;
    unsigned int levels;
};

class dataconfigs : public QObject
{
    Q_OBJECT
    bool    m_isValid;
public:
    dataconfigs();
    virtual ~dataconfigs();

    bool loadconfigs();
    DataFolders dirs;
    QString config_dir;
    QString data_dir;
    QString localScriptName_lvl;
    QString commonScriptName_lvl;
    QString localScriptName_wld;
    QString commonScriptName_wld;

    QString splash_logo;
    QList<obj_splash_ani > animations;

    EngineSetup engine;

    //Playable Characters
    QList<obj_player > main_characters;

    //Level map items
    PGE_DataArray<obj_BG > main_bg;
    PGE_DataArray<obj_bgo > main_bgo;
    PGE_DataArray<obj_block > main_block;
    PGE_DataArray<obj_npc > main_npc;
    npc_Markers marker_npc;

    //World map items
    PGE_DataArray<obj_w_tile > main_wtiles;
    PGE_DataArray<obj_w_path > main_wpaths;
    PGE_DataArray<obj_w_scenery > main_wscene;
    PGE_DataArray<obj_w_level > main_wlevels;
    wld_levels_Markers marker_wlvl;

    //Common items
    unsigned long music_custom_id;
    unsigned long music_w_custom_id;
    PGE_DataArray<obj_music > main_music_lvl;
    PGE_DataArray<obj_music > main_music_wld;
    PGE_DataArray<obj_music > main_music_spc;

    PGE_DataArray<obj_sound > main_sound;

    //Tilesets
    QList<SimpleTileset >      main_tilesets;
    QList<SimpleTilesetGroup > main_tilesets_grp;

    // Custom properties
    QList<obj_custom_property> main_custom_properties;

    QList<obj_rotation_table > main_rotation_table;

    bool check(); //Returns true, if something config entry is not initialized
    bool checkCustom(); //Returns true, if some of custom config has failed

    //Graphics
    obj_gridSizes defaultGrid;

    enum ErrorListType
    {
        ERR_GLOBAL = 0,
        ERR_CUSTOM = 1
    };
    //! Errors of config pack loading
    QStringList errorsList[2];

    long getCharacterI(unsigned long itemID);

    long getSndI(unsigned long itemID);
    long getMusLvlI(unsigned long itemID);
    long getMusWldI(unsigned long itemID);
    long getMusSpcI(unsigned long itemID);


    void loadTilesets();
    void loadCustomProperties(); // Depends on loadLevelNPC

    void setConfigPath(QString p);
    bool loadBasics();

    bool loadLevelBackground(obj_BG &sbg, QString section, obj_BG *merge_with=nullptr, QString iniFile="", IniProcessing *setup=nullptr);
    bool loadLevelBlock(obj_block &sblock, QString section, obj_block *merge_with=nullptr, QString iniFile="", IniProcessing *setup=nullptr);
    bool loadLevelBGO(obj_bgo &sbgo, QString section, obj_bgo *merge_with=nullptr, QString iniFile="", IniProcessing *setup=nullptr);
    bool loadLevelNPC(obj_npc &snpc, QString section, obj_npc *merge_with=nullptr, QString iniFile="", IniProcessing *setup=nullptr);

    bool loadWorldTerrain(obj_w_tile &stile, QString section, obj_w_tile *merge_with=nullptr, QString iniFile="", IniProcessing *setup=nullptr);
    bool loadWorldScene(obj_w_scenery &sScene, QString section, obj_w_scenery *merge_with=nullptr, QString iniFile="", IniProcessing *setup=nullptr);
    bool loadWorldPath(obj_w_path &spath, QString section, obj_w_path *merge_with=nullptr, QString iniFile="", IniProcessing *setup=nullptr);
    bool loadWorldLevel(obj_w_level &slevel, QString section, obj_w_level *merge_with=nullptr, QString iniFile="", IniProcessing *setup=nullptr);

    inline QString getBgoPath()  {return bgoPath;}
    inline QString getBGPath()   {return BGPath;}
    inline QString getBlockPath(){return blockPath;}
    inline QString getNpcPath()  {return npcPath;}

    inline QString getTilePath() {return tilePath;}
    inline QString getScenePath(){return scenePath;}
    inline QString getPathPath() {return pathPath;}
    inline QString getWlvlPath() {return wlvlPath;}

signals:
    void progressValue(int);
    void progressMax(int);
    void progressTitle(QString);
    void progressPartsTotal(int);
    void progressPartNumber(int);

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

    void loadPlayers();

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

    //! Recently loaded INI-file
    std::string m_recentIniFile;

    QString     getFullIniPath(QString iniFileName);
    bool        openSection(IniProcessing *config, const std::string &section);
    inline void closeSection(IniProcessing* file) { file->endGroup(); }

    //! Write errors into custom errors config
    ErrorListType m_errOut = ERR_GLOBAL;

    void        addError(QString bug, PGE_LogLevel level = PGE_LogLevel::Warning);
};




#endif // DATACONFIGS_H
