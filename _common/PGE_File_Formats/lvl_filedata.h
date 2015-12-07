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

/*!
 *  \file lvl_filedata.h
 *  \brief Contains data structure definitions for a level file data
 */

#ifndef LVL_FILEDATA_H
#define LVL_FILEDATA_H

#include "pge_file_lib_globs.h"
#include "meta_filedata.h"

//////////////////////Level file Data//////////////////////
/*!
 * \brief Level specific Section entry structure. Defines prererences of one section
 */
struct LevelSection
{
    //! ID of section (starts from 0)
    int id;
    //! Y-Position of top side of section
    long size_top;
    //! Y-Position of bottom side of section
    long size_bottom;
    //! X-Position of left side of section
    long size_left;
    //! X-Position of right side of section
    long size_right;
    //! ID of default music in this section (starts from 1, 0 is silence)
    unsigned int music_id;
    //! [UNUSED] RGBA defines color of background if image is not defined.
    long bgcolor;
    //! Enables horisontal wrap
    bool wrap_h;
    //! Enables vertical wrap
    bool wrap_v;
    //! Level will be exited when playable character will exit out of screen
    bool OffScreenEn;
    //! ID of background image of this section
    unsigned int background;
    //! Enable lock of walking to left direction
    bool lock_left_scroll;
    //! Sets default physical environment of this section is - water
    bool underwater;
    //! Custom music file which will be playd if music ID defined to "Custom" music id
    PGESTRING music_file;

/*
 * Editor-only parameters which are not saving into file
 */
    //! Recent camera X position
    long PositionX;
    //! Recent camera Y position
    long PositionY;
};

/*!
 * \brief Level specific Player spawn point entry definition structure.
 */
struct PlayerPoint
{
    //! Defined ID of player
    unsigned int id;
    //! X-position of player spawn point
    long x;
    //! Y-position of player spawn point
    long y;
    //! Height of player spawn point (used to calculate position of bottom to place playable character correctly)
    long h;
    //! Wodth of player spawn point (used to calculate position of bottom to place playable character correctly)
    long w;
    //! Initial direction of playable character (-1 is left, 1 is right, 0 is right by default)
    int direction;
};

/*!
 * \brief Level specific Block entry structure. Defines preferences of each block
 */
struct LevelBlock
{
    //! X position of block
    long x;
    //! Y position of block
    long y;
    //! Height of sizable block (takes no effect on non-sizable blocks in PGE Engine,
    //! but takes effect in the SMBX Engine)
    long h;
    //! Width of sizable block (takes no effect on non-sizable blocks in PGE Engine,
    //! but takes effect in the SMBX Engine)
    long w;
    //! ID of block type defined in the lvl_blocks.ini
    unsigned long id;
    //! ID of the included NPC (0 - empty, <0 - number of coins, >0 - NPC-ID of included NPC)
    long npc_id;
    //! Block is invizible until player will impacted to it at bottom side
    bool invisible;
    //! Block has a splippery surface
    bool slippery;
    //! Name of a parent layer. Default value is "Default"
    PGESTRING layer;
    //! Trigger event on destroying of this block
    PGESTRING event_destroy;
    //! Trigger event on hiting of this block
    PGESTRING event_hit;
    //! Trigger event on destroying of this block and at sametime parent layer has no more other objects
    PGESTRING event_emptylayer;

/*
 * Editor-only parameters which are not saving into file
 */
    //! Array-ID is an unique key value identificates each unique block object.
    //! Re-counts on each file reloading
    unsigned int array_id;
    //! Recent array index where block was saved (used to speed-up settings synchronization)
    unsigned int index;
};


/*!
 * \brief Level specific Background object entry structure. Defines preferences of each Background object
 */
struct LevelBGO
{
/*
 * SMBX64
 */
    //! X position of Background Object
    long x;
    //! Y position of Background Object
    long y;
    //! ID of background object type defined in the lvl_bgo.ini
    unsigned long id;
    //! Name of a parent layer. Default value is "Default"
    PGESTRING layer;

/*
 * Extended
 */
    /*!
     * \brief Z order Modes of background objects
     */
    enum zmodes{
        //! Background-2 (under sizable blocks)
        Background2=-2,
        //! Background-1 (over sizable blocks, under regular blocks)
        Background1=-1,
        //! Use config default Z-Mode (which defined in the lvl_bgo.ini)
        ZDefault=0,
        //! Foreground-1 (over regular blocks, but under lava blocks and foreground NPC's)
        Foreground1=1,
        //! Foreground-2 (over everything)
        Foreground2=2
    };

    //! Z-Mode of displaying of BGO
    int    z_mode;
    //! Z-Offset relative to current of Z-value
    double z_offset;

    /*! SMBX64 Order priority. Defines priority for ordering of entries in array of BGO's before writing into a SMBX-lvl file. \
        -1 - use system default order priority */
    long smbx64_sp;
/*
 * Editor-only parameters which are not saving into file
 */
    //! Automatically calculated value of SMBX64 Order priority
    long smbx64_sp_apply;
    //! Array-ID is an unique key value identificates each unique block object. Re-counts on each file reloading
    unsigned int array_id;
    //! Recent array index where block was saved (used to speed-up settings synchronization)
    unsigned int index;

};


/*!
 * \brief Level specific NPC entry structure. Defines preferences of each NPC
 */
struct LevelNPC
{
    //! X position of NPC
    long x;
    //! Y position of NPC
    long y;
    //! Initial direction of NPC (-1 left, 1 right, 0 left or right randomly)
    int direct;
    //! ID of NPC type defined in the lvl_npc.ini
    unsigned long id;
    //! User-data integer #1 used for configuring some NPC-AI's (kept for SMBX64)
    long special_data;
    //! User-data integer #2 used for configuring some NPC-AI's (kept for SMBX64)
    long special_data2;
    //! Makes a generator of this NPC
    bool generator;
    //! Generator direction [1] up, [2] left, [3] down, [4] right
    int generator_direct;
    //! Generator type 0 - instant appearence, 1 - warp, 2 - projectile shoot
    int generator_type;
    //! Generator's delay between each shoot in deci-seconds
    int generator_period;
    //! Talking message. If not empty, NPC will be talkable
    PGESTRING msg;
    //! NPC will not communicate and will not collide with playable characters
    bool friendly;
    //! NPC will stay idle and will always keep look to the playable character
    bool nomove;
    //! Enables some boss-specific NPC-AI features (Used by NPC-AI)
    bool is_boss;
    //! Name of a parent layer. Default value is "Default"
    PGESTRING layer;
    //! Trigger event on actiovation of this NPC (on appearence on screen)
    PGESTRING event_activate;
    //! Trigger event on death of this NPC
    PGESTRING event_die;
    //! Trigger event on player's attempt to talk with this NPC
    PGESTRING event_talk;
    //! Trigger event on destroying of this block and at sametime parent layer has no more other objects
    PGESTRING event_emptylayer;
    //! Attach layer to this NPC. All memberes of this layer are will follow to motion of this NPC.
    PGESTRING attach_layer;

/*
 * Editor-only parameters which are not saving into file
 */
    //! Array-ID is an unique key value identificates each unique block object. Re-counts on each file reloading.
    unsigned int array_id;
    //!< Recent array index where block was saved (used to speed-up settings synchronization)
    unsigned int index;
    //!< Is this NPC a star (Copying from lvl_npc.ini config on file read). Stars are special bonus which required by player to be able enter into some doors/warps
    bool is_star;
};

/*!
 * \brief Level specific Warp entry structure. Defines preferences of each Warp entry
 */
struct LevelDoor
{
    //! X position of Entrance point
    long ix;
    //! Y position of Entrance point
    long iy;
    //! [Editing only, is not saving into file] is entrance point placed to the level
    bool isSetIn;

    //! X position of Exit point
    long ox;
    //! Y position of Exit point
    long oy;
    //! [Editing only, is not saving into file] is exit point placed to the level
    bool isSetOut;

    //! Direction of entrance point: [3] down, [1] up, [2] left, [4] right (not same as exit!)
    int idirect;
    //! Direction of exit point: [1] down [3] up [4] left [2] right (not same as entrance!)
    int odirect;
    //! Warp type: [1] pipe, [2] door, [0] instant
    int type;
    //! Target level filename (Exit from this leven and enter to target level)
    PGESTRING lname;
    //! Warp Array-ID in the target level (if 0 - enter into target level at spawn point)
    long warpto;
    //! Level entrance (this point can be used only as entrance point where player will enter into level)
    bool lvl_i;
    //! Level exit (entering into this warp will trigger exiting of level)
    bool lvl_o;
    //! Target World map X coordinate
    long world_x;
    //! Target World map Н coordinate
    long world_y;
    //! Stars required to be allowed for enter into this warp
    int stars;
    //! Name of a parent layer. Default value is "Default"
    PGESTRING layer;
    //! [Unused] resrved boolean flag, always false
    bool unknown;
    //! Unmount all vehicles when player tried to enter into this warp. (all vehucles are will be returned back on exiting from level)
    bool novehicles;
    //! Allows player to move through this warp carried NPC's
    bool allownpc;
    //! Player need to carry a key to be allowed to enter into this warp
    bool locked;

/*
 * Editor-only parameters which are not saving into file
 */
    //! Array-ID is an unique key value identificates each unique block object. Re-counts on each file reloading
    unsigned int array_id;
    //!< Recent array index where block was saved (used to speed-up settings synchronization)
    unsigned int index;
};

/*!
 * \brief  Level specific Physical Environment entry structure. Defines preferences of each Physical Environment entry
 */
struct LevelPhysEnv
{
    //! X position of physical environment zone
    long x;
    //! Y position of physical environment zone
    long y;
    //! Height of physical environment zone
    long h;
    //! Width of physical environment zone
    long w;
    //! [Unused] reserved long integer value, always 0
    long unknown;
    //! Enable quicksand physical environment, overwise water physical environment
    bool quicksand;
    //! Name of a parent layer. Default value is "Default"
    PGESTRING layer;

/*
 * Editor-only parameters which are not saving into file
 */
    //! Array-ID is an unique key value identificates each unique block object. Re-counts on each file reloading
    unsigned int array_id;
    //! Recent array index where block was saved (used to speed-up settings synchronization)
    unsigned int index;
};

/*!
 * \brief Level specific Layer entry structure
 */
struct LevelLayer
{
    //! Name of layer
    PGESTRING name;
    //! Is this layer hidden?
    bool hidden;
    //! Are all members of this layer are locked for modifying?
    bool locked;

/*
 * Editor-only parameters which are not saving into file
 */
    //!< Array-ID is an unique key value identificates each unique block object. Re-counts on each file reloading.
    unsigned int array_id;
};

/*!
 * \brief Level events specific settings set per each section
 */
struct LevelEvent_Sets
{
    //! Set new Music ID in this section (-1 - do nothing, -2 - reset to defaint, >=0 - set music ID)
    long music_id;
    //! Set new Background ID in this section (-1 - do nothing, -2 - reset to defaint, >=0 - set background ID)
    long background_id;
    //! Change section borders if not (-1 - do nothing, -2 set default, any other values - set X position of left section boundary)
    long position_left;
    //! Change Y position of top section boundary
    long position_top;
    //! Change Y position of bottom section boundary
    long position_bottom;
    //! Change X position of right section boundary
    long position_right;
};

/*!
 * \brief Level specific Classic Event Entry structure
 */
struct LevelSMBX64Event
{
    //! Name of event
    PGESTRING name;
    //! Message box to spawn if not empty
    PGESTRING msg;
    //! Sound ID to play if not zero
    long sound_id;
    //! Trigger end of game and go to credits screen if not zero
    long end_game;

    //! Don't show smoke effect on show/hide/toggle layer visibility
    bool nosmoke;
    //! List of layers to hide
    PGESTRINGList layers_hide;
    //! List of layers to show
    PGESTRINGList layers_show;
    //! List of layers to toggle (hide visible and show invisible)
    PGESTRINGList layers_toggle;

    //! List of section settings per each section to apply
    PGELIST<LevelEvent_Sets > sets;
    //! Trigger another event if not empty
    PGESTRING trigger;
    //! Trigger another event after time in deci-seconds
    long trigger_timer;

    //! Hold "Up" key controllers
    bool ctrl_up;
    //! Hold "Down" key controllers
    bool ctrl_down;
    //! Hold "Left" key controllers
    bool ctrl_left;
    //! Hold "Right" key controllers
    bool ctrl_right;
    //! Hold "Jump" key controllers
    bool ctrl_jump;
    //! Hold "Alt-jump" key controllers
    bool ctrl_altjump;
    //! Hold "Run" key controllers
    bool ctrl_run;
    //! Hold "Alt-run" key controllers
    bool ctrl_altrun;

    //! Hold "Start" key controllers
    bool ctrl_start;
    //! Hold "Drop" key controllers
    bool ctrl_drop;

    //! Trigger event automatically on level startup
    bool autostart;

    //! Install layer motion settings for layer if is not empt
    PGESTRING movelayer;
    //! Set layer X motion in pixels per 1/65
    float layer_speed_x;
    //! Set layer Y motion in pixels per 1/65
    float layer_speed_y;
    //! Setup autoscrool X speed in pixels per 1/65
    float move_camera_x;
    //! Setup autoscrool Y speed in pixels per 1/65
    float move_camera_y;
    //! Setup autoscrool for section ID (starts from 0)
    long scroll_section;

/*
 * Editor-only parameters which are not saving into file
 */
    //! Array-ID is an unique key value identificates each unique block object. Re-counts on each file reloading
    unsigned int array_id;
};

/*!
 * \brief Level data structure. Contains all available settings and element lists on the level.
 */
struct LevelData
{
/*
 * Level header
 */
    //! Total number of stars on the level
    int stars;
    //! Is file parsed correctly, false if some error is occouped
    bool ReadFileValid;
    //! Error messsage
    PGESTRING ERROR_info;
    //! Line data where error was occouped
    PGESTRING ERROR_linedata;
    //! Number of line where error was occouped
    int       ERROR_linenum;

    //! Understandable name of the level
    PGESTRING LevelName;

/*
 * Level data
 */
    //! Sections settings array
    PGELIST<LevelSection > sections;

    //! Player spawn points array
    PGELIST<PlayerPoint > players;

    //! Array of all presented Blocks in this level
    PGELIST<LevelBlock > blocks;
    //! Last used block's array ID
    unsigned int blocks_array_id;

    //! Array of all presented Background objects in this level
    PGELIST<LevelBGO > bgo;
    //! Last used Background object array ID
    unsigned int bgo_array_id;

    //! Array of all presented NPCs in this level
    PGELIST<LevelNPC > npc;
    //! Last used NPC's array ID
    unsigned int npc_array_id;

    //! Array of all presented Warp Entries in this level
    PGELIST<LevelDoor > doors;
    //! Last used warp's array ID
    unsigned int doors_array_id;

    //! Array of all presented Physical Environment Zones in this level
    PGELIST<LevelPhysEnv > physez;
    //! Last used Physical Environment Zone's array ID
    unsigned int physenv_array_id;

    //! Array of all presented layers in this level
    PGELIST<LevelLayer > layers;
    //! Last used Layer's array ID
    unsigned int layers_array_id;

    //! Array of all presented events in this level
    PGELIST<LevelSMBX64Event > events;
    //! Last used Event's array ID
    unsigned int events_array_id;

    //! Meta-data: Position bookmarks, Auto-Script configuration, etc., Crash meta-data, etc.
    MetaData metaData;

/*
 * Editor-only parameters which are not saving into file
 */
    //! ID of currently editing section
    int CurSection;
    //! is music playing button pressed?
    bool playmusic;
    //! Is level was modified since open?
    bool modified;
    //! Is this level made from scratch and was not saved into file?
    bool untitled;
    //! Enable SMBX64 Standard restrictions
    //! (disable unsupported features and warn about limit exiting)
    bool smbx64strict;
    //! Recent file name since file was opened
    PGESTRING filename;
    //! Recent file path where file was located since it was opened
    PGESTRING path;

/*
 * Helpful functions
 */
    /*!
     * \brief Checks is event with specified title exist in this level
     * \param title Event name which need to check for existsing
     * \return true if requested event is exists
     */
    bool eventIsExist(PGESTRING title);
    /*!
     * \brief Checks is layer with specified title exist in this level
     * \param title Layer name which need to check for existsing
     * \return true if requested event is exists
     */
    bool layerIsExist(PGESTRING title);
};



#endif // LVL_FILEDATA_H
