/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2023 Vitaly Novichkov <admin@wohlnet.ru>
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
#include "obj_rotation_table.h"
#include "meta_wld_exit_codes.h"


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

struct EditorSetup
{
    struct WidgetVisiblity
    {
        bool lvl_itembox = true;
        bool lvl_section_props = false;
        bool lvl_warp_props = true;
        bool lvl_layers = true;
        bool lvl_events = true;
        bool lvl_search = false;

        bool wld_itembox = true;
        bool wld_musicboxes = false;
        bool wld_settings = false;
        bool wld_search = false;

        bool tilesets_box = true;
        bool debugger_box = false;
        bool bookmarks_box = false;
        bool variables_box = false;
    };

    //! Show greeting message at first launch
    bool enable_first_launch_greeting = true;
    //! Show tip of day on a launching
    bool enable_tip_of_the_day = true;

    //! Inital visibiltiy of toolboxes
    WidgetVisiblity default_visibility;
    //! Enforce default toolbox visibility and ignore last state
    WidgetVisiblity default_visibility_enforce = {0};

    struct DefaultFileFormats
    {
        enum Format
        {
            SMBX64 = 0,
            PGEX,
            SMBX38A
        };
        Format level = SMBX64;
        Format world = SMBX64;
    } default_file_formats;

    struct DefaultToolboxPositions
    {
        struct State
        {
            enum Dock
            {
                F_FLOATING = 0,
                F_DOCKED_LEFT = Qt::LeftDockWidgetArea,
                F_DOCKED_RIGHT = Qt::RightDockWidgetArea,
                F_DOCKED_BOTTOM = Qt::BottomDockWidgetArea
            } dock;

            State(const Dock &d = F_FLOATING)
            {
                dock = d;
            }
            State(const State &) = default;
            State &operator=(const State &) = default;

            int x = 0;
            int y = 0;
            int width = -1;
            int height = -1;
        };

        State level_item_browser        = State(State::F_DOCKED_LEFT);
        State level_warps_box           = State(State::F_FLOATING);
        State level_item_properties     = State(State::F_FLOATING);
        State level_search_box          = State(State::F_FLOATING);
        State level_classic_events_box  = State(State::F_FLOATING);
        State level_layers_box          = State(State::F_FLOATING);
        State level_section_properties  = State(State::F_FLOATING);

        State world_item_browser        = State(State::F_DOCKED_LEFT);
        State world_music_boxes         = State(State::F_DOCKED_LEFT);
        State world_item_properties     = State(State::F_FLOATING);
        State world_settings_box        = State(State::F_FLOATING);
        State world_search_box          = State(State::F_FLOATING);

        State bookmarks_box             = State(State::F_FLOATING);
        State debugger_box              = State(State::F_FLOATING);

        State variables_box             = State(State::F_FLOATING);

        State tilesets_item_box         = State(State::F_FLOATING);
    } default_widget_state;

    struct FeaturesSupport
    {
        enum State
        {
            F_DISABLED = 0,
            F_ENABLED,
            F_HIDDEN
        };
        State level_section_vertical_wrap = F_ENABLED;

        State level_section_21plus = F_ENABLED;

        State level_phys_ez_new_types = F_ENABLED;

        State level_bgo_z_layer = F_ENABLED;
        State level_bgo_z_position = F_ENABLED;
        State level_bgo_smbx64_sp = F_ENABLED;

        State level_warp_transit_type = F_ENABLED;

        State level_warp_two_way = F_ENABLED;
        State level_warp_portal = F_ENABLED;
        State level_warp_bomb_exit = F_ENABLED;
        State level_warp_allow_sp_state_only = F_ENABLED;
        State level_warp_allow_needs_floor = F_ENABLED;
        State level_warp_hide_interlevel_scene = F_ENABLED;
        State level_warp_allow_interlevel_npc = F_ENABLED;
        State level_warp_hide_stars = F_ENABLED;
        State level_warp_needstars_message = F_ENABLED;
        State level_warp_on_enter_event = F_ENABLED;
        State level_warp_cannon_exit = F_ENABLED;

        State level_event_new_autoscroll = F_ENABLED;

        State level_npc_vertical_dontmove = F_DISABLED;
        State world_custom_credits = F_DISABLED;
    } supported_features;
};

struct EngineSetup
{
    uint32_t screen_w;
    uint32_t screen_h;
    uint32_t wld_viewport_w;
    uint32_t wld_viewport_h;
};

struct obj_splash_ani
{
    QPixmap      img;
    uint32_t frames;
    uint32_t speed;
    uint32_t x;
    uint32_t y;
};


////////////////////Common items///////////////////////////
struct obj_music
{
    uint64_t id;
    QString name;
    QString file;
};

struct obj_sound
{
    uint64_t id;
    QString name;
    QString file;
    bool hidden;
    bool silent;
};

/**
 * @brief Global setup of default grid size
 */
struct obj_gridSizes
{
    uint32_t general;
    uint32_t block;
    uint32_t bgo;
    uint32_t npc;
    uint32_t terrain;
    uint32_t scenery;
    uint32_t paths;
    uint32_t levels;
};

struct obj_blockGlobalSetup
{
    int32_t sizable_block_border_size = -1;
};

class DataConfig : public QObject
{
    Q_OBJECT
public:
    DataConfig();
    ~DataConfig() override = default;

    bool loadFullConfig();
    DataFolders dirs;
    QString config_dir;
    QString arg_config_app_dir;
    QString data_dir;
    QString localScriptName_lvl;
    QString commonScriptName_lvl;
    QString localScriptName_wld;
    QString commonScriptName_wld;

    QString splash_logo;
    QList<obj_splash_ani > animations;

    //! An identify key to mark level and world files to maintain a compatibility between different config packs.
    QString configPackId;
    //! Target engine name, shown in the incompatibility warning message
    QString targetEngineName;

    EditorSetup editor;
    EngineSetup engine;

    //Playable Characters
    QList<obj_player > main_characters;

    //Vehicles list
    QList<obj_vehicle > main_vehicles;

    //Level map items
    //! Global level background image types setup
    PGE_DataArray<obj_BG > main_bg;
    //! Global Background object types setup list
    PGE_DataArray<obj_bgo > main_bgo;
    //! Global setup for all blocks
    obj_blockGlobalSetup defaultBlock;
    //! Global block types setup list
    PGE_DataArray<obj_block > main_block;
    //! Global non-playable character types setup list
    PGE_DataArray<obj_npc > main_npc;
    //! Special NPC element types
    npc_Markers marker_npc;

    //World map items
    //! Global terrain tile types setup list
    PGE_DataArray<obj_w_tile > main_wtiles;
    //! Global path cell types setup list
    PGE_DataArray<obj_w_path > main_wpaths;
    //! Global scenery types setup list
    PGE_DataArray<obj_w_scenery > main_wscene;
    //! Global level entrance types setup list
    PGE_DataArray<obj_w_level > main_wlevels;
    //! Special level entrance element types
    wld_levels_Markers marker_wlvl;

    //Common items
    //! Music ID which will be reserved for custom music on levels
    uint64_t music_custom_id;
    //! Music ID which will be reserved for custom music on world maps
    uint64_t music_w_custom_id;
    //! Available config pack standard level music set
    PGE_DataArray<obj_music > main_music_lvl;
    //! Available config pack standard world map music set
    PGE_DataArray<obj_music > main_music_wld;
    //! Available config pack standard special-use music set
    PGE_DataArray<obj_music > main_music_spc;

    //! Available config pack standard SFX set
    PGE_DataArray<obj_sound > main_sound;

    //Tilesets
    //! Full set of config-pack standard tilesets
    QList<SimpleTileset >      main_tilesets;
    //! Full set of config-pack standard tileset groups are holds tilesets
    QList<SimpleTilesetGroup > main_tilesets_grp;
    //! Full set of config-pack standard tileset categories, generated from tileset groups
    QList<SimpleTilesetCachedCategory> main_tileset_categogies;

    QList<obj_rotation_table > main_rotation_table;
    QList<meta_wld_exitcodes > meta_exit_codes;

    /**
     * @brief Verify config pack is valid (Standard configs only)
     * @return Returns true if something config entry is not initialized
     */
    bool check();

    /**
     * @brief Verify custom-loaded config files are valid (are not a part of config pack)
     * @return Returns true if some of custom config has failed
     */
    bool checkCustom();

    //Graphics

    //! Default alignment grid size for everything
    obj_gridSizes defaultGrid;

    /**
     * @brief Category of config pack errors
     */
    enum ErrorListType
    {
        //! Errors of standard config pack parts
        ERR_GLOBAL = 0,
        //! Errors of custom out of pack outside parts
        ERR_CUSTOM = 1
    };

    //! Errors of config pack loading
    QStringList errorsList[2];

    long getCharacterI(unsigned long itemID);

    long getSndI(unsigned long itemID);
    long getMusLvlI(unsigned long itemID);
    long getMusWldI(unsigned long itemID);
    long getMusSpcI(unsigned long itemID);


    void  loadTilesets();

    void setConfigPath(const QString &p, const QString &appDir = QString());
    bool loadBasics();

    bool loadLevelBackground(obj_BG &sbg, QString section, obj_BG *merge_with=nullptr, QString iniFile="", IniProcessing *setup=nullptr);
    bool loadLevelBlock(obj_block &sblock, QString section, obj_block *merge_with=nullptr, QString iniFile="", IniProcessing *setup=nullptr);
    bool loadLevelBGO(obj_bgo &sbgo, QString section, obj_bgo *merge_with=nullptr, QString iniFile="", IniProcessing *setup=nullptr);
    bool loadLevelNPC(obj_npc &snpc, QString section, obj_npc *merge_with=nullptr, QString iniFile="", IniProcessing *setup=nullptr);

    bool loadWorldTerrain(obj_w_tile &stile, QString section, obj_w_tile *merge_with=nullptr, QString iniFile="", IniProcessing *setup=nullptr);
    bool loadWorldScene(obj_w_scenery &sScene, QString section, obj_w_scenery *merge_with=nullptr, QString iniFile="", IniProcessing *setup=nullptr);
    bool loadWorldPath(obj_w_path &spath, QString section, obj_w_path *merge_with=nullptr, QString iniFile="", IniProcessing *setup=nullptr);
    bool loadWorldLevel(obj_w_level &slevel, QString section, obj_w_level *merge_with=nullptr, QString iniFile="", IniProcessing *setup=nullptr);

    QString getBgoPath();
    QString getBGPath();
    QString getBlockPath();
    QString getNpcPath();

    QString getTilePath();
    QString getScenePath();
    QString getPathPath();
    QString getWlvlPath();

    bool isExtraSettingsLocalAtRoot();

    QString getBgoExtraSettingsPath();
    QString getBlockExtraSettingsPath();
    QString getNpcExtraSettingsPath();

    QString getTileExtraSettingsPath();
    QString getSceneExtraSettingsPath();
    QString getPathExtraSettingsPath();
    QString getWlvlExtraSettingsPath();

    static QString buildLocalConfigPath(const QString &configPackPath);

signals:
    void progressValue(int);
    void progressMax(int);
    void progressTitle(QString);
    void progressPartsTotal(int);
    void progressPartNumber(int);
    void errorOccured();

private:
    unsigned long total_data;

    struct ResourceFolder
    {
        QString graphics;
        QString items;
        QString extraSettings;
    };

    bool m_extraSettingsLocalAtRoot = true;

    ResourceFolder folderLvlBgo;
    ResourceFolder folderLvlBG;
    ResourceFolder folderLvlBlocks;
    ResourceFolder folderLvlNPC;

    ResourceFolder folderWldTerrain;
    ResourceFolder folderWldScenery;
    ResourceFolder folderWldPaths;
    ResourceFolder folderWldLevelPoints;

    void loadPlayers();
    void loadVehicles();

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
    void loadExitCodes();

    QString     getFullIniPath(QString iniFileName);
    bool        openSection(IniProcessing *config, const std::string &section, bool tryGeneral = false);
    inline void closeSection(IniProcessing* file) { file->endGroup(); }

    //! Write errors into custom errors config
    ErrorListType m_errOut = ERR_GLOBAL;

    void        addError(QString bug, PGE_LogLevel level = PGE_LogLevel::Warning);
};




#endif // DATACONFIGS_H
