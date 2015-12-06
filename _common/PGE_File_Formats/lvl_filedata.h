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

/*! \file lvl_filedata.h
    \brief Contains data structure definitions for a level file data
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
    int id;                 //!< ID of section (starts from 0)
    long size_top;          //!< Y-Position of top side of section
    long size_bottom;       //!< Y-Position of bottom side of section
    long size_left;         //!< X-Position of left side of section
    long size_right;        //!< X-Position of right side of section
    unsigned int music_id;  //!< ID of default music in this section (starts from 1, 0 is silence)
    long bgcolor;           //!< [UNUSED] RGBA defines color of background if image is not defined.
    bool wrap_h;            //!< Enables horisontal wrap
    bool wrap_v;            //!< Enables vertical wrap
    bool OffScreenEn;       //!< Level will be exited when playable character will exit out of screen
    unsigned int background;//!< ID of background image of this section
    bool lock_left_scroll;  //!< Enable lock of walking to left direction
    bool underwater;        //!< Sets default physical environment of this section is - water
    PGESTRING music_file;   //!< Custom music file which will be playd if music ID defined to "Custom" music id

    /*!
     * Editor-only parameters which are not saving into file
     */
    long PositionX;         //!< Recent camera X position
    long PositionY;         //!< Recent camera Y position
};

/*!
 * \brief Level specific Player spawn point entry definition structure.
 */
struct PlayerPoint
{
    unsigned int id;    //!< Defined ID of player
    long x;             //!< X-position of player spawn point
    long y;             //!< Y-position of player spawn point
    long h;             //!< Height of player spawn point (used to calculate position of bottom to place playable character correctly)
    long w;             //!< Wodth of player spawn point (used to calculate position of bottom to place playable character correctly)
    int direction;      //!< Initial direction of playable character (-1 is left, 1 is right, 0 is right by default)
};

/*!
 * \brief Level specific Block entry structure. Defines preferences of each block
 */
struct LevelBlock
{
    long x;                     //!< X position of block
    long y;                     //!< Y position of block
    long h;                     //!< Height of sizable block (takes no effect on non-sizable blocks in PGE Engine,
                                //!< but takes effect in the SMBX Engine)
    long w;                     //!< Width of sizable block (takes no effect on non-sizable blocks in PGE Engine,
                                //!< but takes effect in the SMBX Engine)
    unsigned long id;           //!< ID of block type defined in the lvl_blocks.ini
    long npc_id;                //!< ID of the included NPC (0 - empty, <0 - number of coins, >0 - NPC-ID of included NPC)
    bool invisible;             //!< Block is invizible until player will impacted to it at bottom side
    bool slippery;              //!< Block has a splippery surface
    PGESTRING layer;            //!< Name of a parent layer. Default value is "Default"
    PGESTRING event_destroy;    //!< Trigger event on destroying of this block
    PGESTRING event_hit;        //!< Trigger event on hiting of this block
    PGESTRING event_emptylayer; //!< Trigger event on destroying of this block and at sametime parent layer has no more other objects

    /*!
     * Editor-only parameters which are not saving into file
     */
    unsigned int array_id;      //!< Array-ID is an unique key value identificates each unique block object.
                                //!< Re-counts on each file reloading
    unsigned int index;         //!< Recent array index where block was saved (used to speed-up settings synchronization)
};


/*!
 * \brief Level specific Background object entry structure. Defines preferences of each Background object
 */
struct LevelBGO
{
    /*!
     * SMBX64
     */
    long x;                     //!< X position of Background Object
    long y;                     //!< Y position of Background Object
    unsigned long id;           //!< ID of background object type defined in the lvl_bgo.ini
    PGESTRING layer;            //!< Name of a parent layer. Default value is "Default"

    /*!
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

    int    z_mode;              //!< Z-Mode of displaying of BGO
    double z_offset;            //!< Z-Offset relative to current of Z-value

    long smbx64_sp;             //!< SMBX64 Order priority. Defines priority for ordering of entries in array
                                //!< of BGO's before writing into a SMBX-lvl file.
                                //!< -1 - use system default order priority
    /*!
     * Editor-only parameters which are not saving into file
     */
    long smbx64_sp_apply;       //!< Automatically calculated value of SMBX64 Order priority
    unsigned int array_id;      //!< Array-ID is an unique key value identificates each unique block object.
                                //!< Re-counts on each file reloading
    unsigned int index;         //!< Recent array index where block was saved (used to speed-up settings synchronization)

};


/*!
 * \brief Level specific NPC entry structure. Defines preferences of each NPC
 */
struct LevelNPC
{
    long x;                         //!< X position of NPC
    long y;                         //!< Y position of NPC
    int direct;                     //!< Initial direction of NPC (-1 left, 1 right, 0 left or right randomly)
    unsigned long id;               //!< ID of NPC type defined in the lvl_npc.ini
    long special_data;              //!< User-data integer #1 used for configuring some NPC-AI's (kept for SMBX64)
    long special_data2;             //!< User-data integer #2 used for configuring some NPC-AI's (kept for SMBX64)
    bool generator;                 //!< Makes a generator of this NPC
    int generator_direct;           //!< Generator direction [1] up, [2] left, [3] down, [4] right
    int generator_type;             //!< Generator type 0 - instant appearence, 1 - warp, 2 - projectile shoot
    int generator_period;           //!< Generator's delay between each shoot in deci-seconds
    PGESTRING msg;                  //!< Talking message. If not empty, NPC will be talkable
    bool friendly;                  //!< NPC will not communicate and will not collide with playable characters
    bool nomove;                    //!< NPC will stay idle and will always keep look to the playable character
    bool is_boss;                   //!< Enables some boss-specific NPC-AI features (Used by NPC-AI)
    PGESTRING layer;                //!< Name of a parent layer. Default value is "Default"
    PGESTRING event_activate;       //!< Trigger event on actiovation of this NPC (on appearence on screen)
    PGESTRING event_die;            //!< Trigger event on death of this NPC
    PGESTRING event_talk;           //!< Trigger event on player's attempt to talk with this NPC
    PGESTRING event_emptylayer;     //!< Trigger event on destroying of this block and at sametime parent layer has no more other objects
    PGESTRING attach_layer;         //!< Attach layer to this NPC. All memberes of this layer are will follow to motion of this NPC.

    /*!
     * Editor-only parameters which are not saving into file
     */
    unsigned int array_id;          //!< Array-ID is an unique key value identificates each unique block object.
                                    //!< Re-counts on each file reloading
    unsigned int index;             //!< Recent array index where block was saved (used to speed-up settings synchronization)
    bool is_star;                   //!< Is this NPC a star (Copying from lvl_npc.ini config on file read)
                                    //!< Stars are special bonus which required by player to be able enter into some doors/warps
};

/*!
 * \brief Level specific Warp entry structure. Defines preferences of each Warp entry
 */
struct LevelDoor
{
    long ix;            //!< X position of Entrance point
    long iy;            //!< Y position of Entrance point
    bool isSetIn;       //!< [Editing only, is not saving into file] is entrance point placed to the level

    long ox;            //!< X position of Exit point
    long oy;            //!< Y position of Exit point
    bool isSetOut;      //!< [Editing only, is not saving into file] is exit point placed to the level

    int idirect;        //!< Direction of entrance point: [3] down, [1] up, [2] left, [4] right (not same as exit!)
    int odirect;        //!< Direction of exit point: [1] down [3] up [4] left [2] right (not same as entrance!)
    int type;           //!< Warp type: [1] pipe, [2] door, [0] instant
    PGESTRING lname;    //!< Target level filename (Exit from this leven and enter to target level)
    long warpto;        //!< Warp Array-ID in the target level (if 0 - enter into target level at spawn point)
    bool lvl_i;         //!< Level entrance (this point can be used only as entrance point where player will enter into level)
    bool lvl_o;         //!< Level exit (entering into this warp will trigger exiting of level)
    long world_x;       //!< Target World map X coordinate
    long world_y;       //!< Target World map Н coordinate
    int stars;          //!< Stars required to be allowed for enter into this warp
    PGESTRING layer;    //!< Name of a parent layer. Default value is "Default"
    bool unknown;       //!< [Unused] resrved boolean flag, always false
    bool novehicles;    //!< Unmount all vehicles when player tried to enter into this warp
                        //!< (all vehucles are will be returned back on exiting from level)
    bool allownpc;      //!< Allows player to move through this warp carried NPC's
    bool locked;        //!< Player need to carry a key to be allowed to enter into this warp

    /*!
     * Editor-only parameters which are not saving into file
     */
    unsigned int array_id;          //!< Array-ID is an unique key value identificates each unique block object.
                                    //!< Re-counts on each file reloading
    unsigned int index;             //!< Recent array index where block was saved (used to speed-up settings synchronization)
};

/*!
 * \brief  Level specific Physical Environment entry structure. Defines preferences of each Physical Environment entry
 */
struct LevelPhysEnv
{
    long x;                 //!< X position of physical environment zone
    long y;                 //!< Y position of physical environment zone
    long h;                 //!< Height of physical environment zone
    long w;                 //!< Width of physical environment zone
    long unknown;           //!< [Unused] reserved long integer value, always 0
    bool quicksand;         //!< Enable quicksand physical environment, overwise water physical environment
    PGESTRING layer;        //!< Name of a parent layer. Default value is "Default"

    /*!
     * Editor-only parameters which are not saving into file
     */
    unsigned int array_id;          //!< Array-ID is an unique key value identificates each unique block object.
                                    //!< Re-counts on each file reloading
    unsigned int index;             //!< Recent array index where block was saved (used to speed-up settings synchronization)
};

/*!
 * \brief Level specific Layer entry structure
 */
struct LevelLayer
{
    PGESTRING name; //!< Name of layer
    bool hidden;    //!< Is this layer hidden?
    bool locked;    //!< Are all members of this layer are locked for modifying?

    /*!
     * Editor-only parameters which are not saving into file
     */
    unsigned int array_id;          //!< Array-ID is an unique key value identificates each unique block object.
                                    //!< Re-counts on each file reloading
};

/*!
 * \brief Level events specific settings set per each section
 */
struct LevelEvent_Sets
{
    long music_id;          //!< Set new Music ID in this section (-1 - do nothing, -2 - reset to defaint, >=0 - set music ID)
    long background_id;     //!< Set new Background ID in this section (-1 - do nothing, -2 - reset to defaint, >=0 - set background ID)
    long position_left;     //!< Change section borders if not (-1 - do nothing, -2 set default, any other values - set X
                            //!< position of left section boundary)
    long position_top;      //!< Change Y position of top section boundary
    long position_bottom;   //!< Change Y position of bottom section boundary
    long position_right;    //!< Change X position of right section boundary
};

/*!
 * \brief Level specific Classic Event Entry structure
 */
struct LevelSMBX64Event
{
    PGESTRING name;                     //!< Name of event
    PGESTRING msg;                      //!< Message box to spawn if not empty
    long sound_id;                      //!< Sound ID to play if not zero
    long end_game;                      //!< Trigger end of game and go to credits screen if not zero

    bool nosmoke;                       //!< Don't show smoke effect on show/hide/toggle layer visibility
    PGESTRINGList layers_hide;          //!< List of layers to hide
    PGESTRINGList layers_show;          //!< List of layers to show
    PGESTRINGList layers_toggle;        //!< List of layers to toggle (hide visible and show invisible)

    PGELIST<LevelEvent_Sets > sets;     //!< List of section settings per each section to apply
    PGESTRING trigger;                  //!< Trigger another event if not empty
    long trigger_timer;                 //!< Trigger another event after time in deci-seconds

    bool ctrl_up;                       //!< Hold "Up" key controllers
    bool ctrl_down;                     //!< Hold "Down" key controllers
    bool ctrl_left;                     //!< Hold "Left" key controllers
    bool ctrl_right;                    //!< Hold "Right" key controllers
    bool ctrl_jump;                     //!< Hold "Jump" key controllers
    bool ctrl_altjump;                  //!< Hold "Alt-jump" key controllers
    bool ctrl_run;                      //!< Hold "Run" key controllers
    bool ctrl_altrun;                   //!< Hold "Alt-run" key controllers

    bool ctrl_start;                    //!< Hold "Start" key controllers
    bool ctrl_drop;                     //!< Hold "Drop" key controllers

    bool autostart;                     //!< Trigger event automatically on level startup

    PGESTRING movelayer;                //!< Install layer motion settings for layer if is not empt
    float layer_speed_x;                //!< Set layer X motion in pixels per 1/65
    float layer_speed_y;                //!< Set layer Y motion in pixels per 1/65
    float move_camera_x;                //!< Setup autoscrool X speed in pixels per 1/65
    float move_camera_y;                //!< Setup autoscrool Y speed in pixels per 1/65
    long scroll_section;                //!< Setup autoscrool for section ID (starts from 0)

    /*!
     * Editor-only parameters which are not saving into file
     */
    unsigned int array_id;          //!< Array-ID is an unique key value identificates each unique block object.
                                    //!< Re-counts on each file reloading
};

/*!
 * \brief Level data structure. Contains all available settings and element lists on the level.
 */
struct LevelData
{
    /*!
     * Level header
     */
    int stars;                              //!< Total number of stars on the level
    bool ReadFileValid;                     //!< Is file parsed correctly, false if some error is occouped
    PGESTRING ERROR_info;                   //!< Error messsage
    PGESTRING ERROR_linedata;               //!< Line data where error was occouped
    int       ERROR_linenum;                //!< Number of line where error was occouped

    PGESTRING LevelName;                    //!< Understandable name of the level

    /*!
     * Level data
     */
    PGELIST<LevelSection > sections;       //!< Sections settings array
    PGELIST<PlayerPoint > players;         //!< Player spawn points array
    PGELIST<LevelBlock > blocks;           //!< Array of all presented Blocks in this level
    unsigned int blocks_array_id;          //!< Last used block's array ID
    PGELIST<LevelBGO > bgo;                //!< Array of all presented Background objects in this level
    unsigned int bgo_array_id;             //!< Last used Background object array ID
    PGELIST<LevelNPC > npc;                //!< Array of all presented NPCs in this level
    unsigned int npc_array_id;             //!< Last used NPC's array ID
    PGELIST<LevelDoor > doors;             //!< Array of all presented Warp Entries in this level
    unsigned int doors_array_id;           //!< Last used warp's array ID
    PGELIST<LevelPhysEnv > physez;         //!< Array of all presented Physical Environment Zones in this level
    unsigned int physenv_array_id;         //!< Last used Physical Environment Zone's array ID
    PGELIST<LevelLayer > layers;           //!< Array of all presented layers in this level
    unsigned int layers_array_id;          //!< Last used Layer's array ID
    PGELIST<LevelSMBX64Event > events;     //!< Array of all presented events in this level
    unsigned int events_array_id;          //!< Last used Event's array ID

    //! Meta-data: Position bookmarks, Auto-Script configuration, etc., Crash meta-data, etc.
    MetaData metaData;

    /*!
     * Editor-only parameters which are not saving into file
     */
    int CurSection;     //!< ID of currently editing section
    bool playmusic;     //!< is music playing button pressed?
    bool modified;      //!< Is level was modified since open?
    bool untitled;      //!< Is this level made from scratch and was not saved into file?
    bool smbx64strict;  //!< Enable SMBX64 Standard restrictions
                        //!< (disable unsupported features and warn about limit exiting)
    PGESTRING filename; //!< Recent file name since file was opened
    PGESTRING path;     //!< Recent file path where file was located since it was opened

    /*!
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
