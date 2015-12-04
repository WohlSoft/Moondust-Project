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

#ifndef FILE_FORMATS_H
#define FILE_FORMATS_H

#include "pge_file_lib_globs.h"
#include "lvl_filedata.h"
#include "npc_filedata.h"
#include "wld_filedata.h"
#include "save_filedata.h"
#include "smbx64_cnf_filedata.h"

/*!
 * \brief PGE File library class of static functions.
 *        Library is buildable in both Qt and STL applications
 */
#ifdef PGE_FILES_QT
class FileFormats: PGE_FILES_INHERED
{
    Q_OBJECT
#else
class FileFormats
{
#endif

public:

//File format read/write functions
    /*!
     * \brief Parses non-SMBX64 meta-data from additional *.meta files
     *        there are contains data which impossible to save into SMBX64 LVL file
     *        therefore it will be saved into additional *.meta file
     * \param Raw-data Full raw file data contents
     * \return Meta-data structure
     */
    static MetaData         ReadNonSMBX64MetaData(PGESTRING RawData);

    /*!
     * \brief Generates raw data of non-SMBX meta-data file from Meta-data structure
     * \param metaData Meta-data structure
     * \return Full raw-data string
     */
    static PGESTRING        WriteNonSMBX64MetaData(MetaData metaData);

/******************************Level files***********************************/
    /*!
     * \brief Parses a level file with auto-detection of a file type (SMBX1...64 LVL or PGE-LVLX)
     * \param filePath Full path to file which must be opened
     * \return Level data structure
     */
    static LevelData        OpenLevelFile(PGESTRING filePath);
    /*!
     * \brief Parses a level file header only with auto-detection of a file type (SMBX1...64 LVL or PGE-LVLX)
     * \param filePath Full path to file which must be opened
     * \return Level data structure (with initialized header data only)
     */
    static LevelData        OpenLevelFileHeader(PGESTRING filePath);

// SMBX64 LVL File
    /*!
     * \brief Parses SMBX1...64 level file header and skips other part of a file
     * \param filePath Full path to level file
     * \return Level data structure (with initialized header data only)
     */
    static LevelData        ReadSMBX64LvlFileHeader(PGESTRING filePath);
    /*!
     * \brief Parses SMBX1...64 level file data
     * \param RawData Raw data string in the SMBX1...64 level format
     * \param filePath Full path to the file (if empty, custom data in the episode and in the custom directories are will be inaccessible)
     * \return Level data structure
     */
    static LevelData        ReadSMBX64LvlFile(PGESTRING RawData, PGESTRING filePath="");
    /*!
     * \brief Generates SMBX1...64 Level file data
     * \param FileData Level data structure
     * \param file_format SMBX file format version number (from 0 to 64) [Example of level in SMBX0 format is intro.dat included with SMBX 1.0]
     * \return Raw data string in the SMBX1...64 level format
     */
    static PGESTRING        WriteSMBX64LvlFile(LevelData FileData, int file_format=64);  //!< Generate SMBX1-SMBX64 level raw data



// PGE Extended Level File
    /*!
     * \brief Parses PGE-X Level file header
     * \param filePath Full path to PGE-X Level file
     * \return Level data structure (with initialized header data only)
     */
    static LevelData        ReadExtendedLvlFileHeader(PGESTRING filePath);

    /*!
     * \brief Parses PGE-X level file data
     * \param RawData Raw data string in the PGE-X level format
     * \param filePath Full path to the file (if empty, custom data in the episode and in the custom directories are will be inaccessible)
     * \return Level data structure
     */
    static LevelData        ReadExtendedLvlFile(PGESTRING RawData, PGESTRING filePath="");

    /*!
     * \brief Generates PGE-X Level file
     * \param FileData Level data structure
     * \return Raw data string in the PGE-X level format
     */
    static PGESTRING        WriteExtendedLvlFile(LevelData FileData);  //!< Generate PGE-X level raw data

// Lvl Data
    /*!
     * \brief Generates blank initialized level data structure
     * \return Level data structure
     */
    static LevelData        CreateLevelData();
    /*!
     * \brief Initializes Level specific NPC entry structure with default properties
     * \return Initialized with default properties level specific NPC entry structure
     */
    static LevelNPC         CreateLvlNpc();
    /*!
     * \brief Initializes Level specific Warp entry structure with default properties
     * \return Initialized with default properties level specific Warp entry structure
     */
    static LevelDoor        CreateLvlWarp();
    /*!
     * \brief Initializes Level specific Block entry structure with default properties
     * \return Initialized with default properties level specific Block entry structure
     */
    static LevelBlock       CreateLvlBlock();
    /*!
     * \brief Initializes Level specific Background Object entry structure with default properties
     * \return Initialized with default properties level specific Background Object entry structure
     */
    static LevelBGO         CreateLvlBgo();
    /*!
     * \brief Initializes Level specific Physical Environment Zone entry structure with default properties
     * \return Initialized with default properties level specific Physical Environment Zone entry structure
     */
    static LevelPhysEnv     CreateLvlPhysEnv();
    /*!
     * \brief Initializes Level specific Layer entry structure with default properties
     * \return Initialized with default properties level specific Layer entry structure
     */
    static LevelLayer       CreateLvlLayer();
    /*!
     * \brief Initializes Level specific SMBX64 Event entry structure with default properties
     * \return Initialized with default properties level specific SMBX64 Event entry structure
     */
    static LevelSMBX64Event CreateLvlEvent();
    /*!
     * \brief Initializes Level specific Player spawn point entry structure with default properties
     * \return Initialized with default properties level specific Player spawn point entry structure
     */
    static PlayerPoint      CreateLvlPlayerPoint(int id=0);
    /*!
     * \brief Initializes Level specific Section Settings entry structure with default properties
     * \return Initialized with default properties level specific Section Settings entry structure
     */
    static LevelSection     CreateLvlSection();

    /*!
     * \brief Optimizing level data for SMBX64 Standard requirements
     * \param lvl Level data structure object
     */
    static void             smbx64LevelPrepare(LevelData &lvl);

    /*!
     * \brief Sorts blocks array by Y->X positions in the given level data structure.
     *        By SMBX64 standard, blocks array must be sorted because it is required for
     *        NPC's collision detection algorithm of SMBX Engine
     * \param lvl Level data structure object
     */
    static void             smbx64LevelSortBlocks(LevelData &lvl);

    /*!
     * \brief Sorts Background objects by special order priority value
     *        Modifying of order priority values allowing to force specific non-foreground BGO's
     *        to be rendered foreground
     * \param lvl Level data structure object
     */
    static void             smbx64LevelSortBGOs(LevelData &lvl);


/******************************World file***********************************/
    /*!
     * \brief Parses a world map file with auto-detection of a file type (SMBX1...64 LVL or PGE-WLDX)
     * \param filePath Full path to file which must be opened
     * \return World data structure
     */
    static WorldData        OpenWorldFile(PGESTRING filePath);
    /*!
     * \brief Parses a world map file header only with auto-detection of a file type (SMBX1...64 LVL or PGE-WLDX)
     * \param filePath Full path to file which must be opened
     * \return World data structure (with initialized header data only)
     */
    static WorldData        OpenWorldFileHeader(PGESTRING filePath);

// SMBX64 WLD File
    /*!
     * \brief Parsed SMBX1...64 World map file header only
     * \param filePath Full path to file which must be opened
     * \return World data structure (with initialized header data only)
     */
    static WorldData        ReadSMBX64WldFileHeader(PGESTRING filePath);
    /*!
     * \brief Parses SMBX1...64 World map file from raw data
     * \param RawData Raw data string in SMBX1...64 World map format
     * \param filePath Full path to the file (if empty, custom data in the episode and in the custom directories are will be inaccessible)
     * \return World data structure
     */
    static WorldData        ReadSMBX64WldFile(PGESTRING RawData, PGESTRING filePath);
    /*!
     * \brief Generates raw data string in SMBX1...64 World map format
     * \param FileData World map data structure
     * \param file_format SMBX file format version number (from 0 to 64)
     * \return Raw data string in SMBX1...64 World map format
     */
    static PGESTRING        WriteSMBX64WldFile(WorldData FileData, int file_format=64);

// PGE Extended World map File
    /*!
     * \brief Parsed PGE-X World map file header only
     * \param filePath Full path to file which must be opened
     * \return World data structure (with initialized header data only)
     */
    static WorldData        ReadExtendedWldFileHeader(PGESTRING filePath);
    /*!
     * \brief Parses PGE-X World map file from raw data
     * \param RawData Raw data string in PGE-X World map format
     * \param filePath Full path to the file (if empty, custom data in the episode and in the custom directories are will be inaccessible)
     * \return World map data structure
     */
    static WorldData        ReadExtendedWldFile(PGESTRING RawData, PGESTRING filePath);
    /*!
     * \brief Generates raw data string in PGE-X World map format
     * \param FileData World map data structure
     * \return Raw data string in PGE-X World map format
     */
    static PGESTRING        WriteExtendedWldFile(WorldData FileData);

//Wld Data
    /*!
     * \brief Generates blank initialized World map data structure
     * \return World map data structure
     */
    static WorldData        CreateWorldData();
    /*!
     * \brief Initializes World map specific Tile entry structure with default properties
     * \return Initialized with default properties World map specific Tile entry structure
     */
    static WorldTiles       CreateWldTile();
    /*!
     * \brief Initializes World map specific Scenery entry structure with default properties
     * \return Initialized with default properties World map specific Scenery entry structure
     */
    static WorldScenery     CreateWldScenery();
    /*!
     * \brief Initializes World map specific Path entry structure with default properties
     * \return Initialized with default properties World map specific Path entry structure
     */
    static WorldPaths       CreateWldPath();
    /*!
     * \brief Initializes World map specific Level Entrance point entry structure with default properties
     * \return Initialized with default properties World map specific Level Entrance point entry structure
     */
    static WorldLevels      CreateWldLevel();
    /*!
     * \brief Initializes World map specific Music Box entry structure with default properties
     * \return Initialized with default properties World map specific Music Box entry structure
     */
    static WorldMusic       CreateWldMusicbox();


/****************************Save of game file********************************/
    /*!
     * \brief Parses SMBX1...64 Game save file from raw data string
     * \param RawData raw data string in SMBX1..64 game save format
     * \param filePath Path to original file
     * \return Game save data structure
     */
    static GamesaveData     ReadSMBX64SavFile(PGESTRING RawData, PGESTRING filePath);

    /*!
     * \brief Parses PGE-X Game save file from raw data string
     * \param RawData raw data string in PGE-X game save format
     * \param filePath Path to original file
     * \return Game save data structure
     */
    static GamesaveData     ReadExtendedSaveFile(PGESTRING RawData, PGESTRING filePath);
    /*!
     * \brief Generate raw data in PGE-X Game save file from GameSave data structure
     * \param FileData Gamesave data structure
     * \return raw string in PGE-X Game save format
     */
    static PGESTRING        WriteExtendedSaveFile(GamesaveData &FileData);

//Save Data
    /*!
     * \brief Initializes blank game save data structure with default preferences
     * \return Blank Game Save data structure
     */
    static GamesaveData     CreateGameSaveData();
    /*!
     * \brief Inirializes Game Save specific playable character state entry
     * \return Blank game save specific playable character state entry
     */
    static saveCharState    CreateSavCharacterState();

/****************************SMBX64 Config file********************************/
    /*!
     * \brief Parses SMBX Engine config file raw data string
     * \param RawData raw data string in SMBX Engine config file format
     * \return SMBX Engine specific config structure
     */
    static SMBX64_ConfigFile ReadSMBX64ConfigFile(PGESTRING RawData);
    /*!
     * \brief Generates SMBX Engine specific config file raw data string
     * \param FileData SMBX Engine specific config structure
     * \param file_format SMBX file format version number (from 0 to 64)
     * \return raw data string in SMBX Engine config file format
     */
    static PGESTRING        WriteSMBX64ConfigFile(SMBX64_ConfigFile &FileData, int file_format);

/******************************NPC.txt file***********************************/
// SMBX64 NPC.TXT File
    /*!
     * \brief Parses SMBX64 NPC.txt file
     * \param file Full path to NPC.txt
     * \param IgnoreBad Don't spawn warning message on errors in the file syntax
     * \return NPC Customizing Config data structure
     */
    static NPCConfigFile    ReadNpcTXTFile(PGESTRING file, bool IgnoreBad=false);
    /*!
     * \brief Generates NPC.txt raw data string in SMBX64 NPC.txt format
     * \param FileData NPC Customizing Config data structure
     * \return raw data string in SMBX64 NPC.txt format
     */
    static PGESTRING        WriteNPCTxtFile(NPCConfigFile FileData);

    /*!
     * \brief Initialize blank NPC Customizing Config data structure
     * \return Blank NPC Customizing Config data structure
     */
    static NPCConfigFile    CreateEmpytNpcTXT();

/******************************common stuff***********************************/
    /*!
     * \brief File parse error codes
     */
    enum ErrorCodes{
        //! file has been successfully parses
        Success=0,
        //! Requested file is not exist
        ERROR_NotExist,
        //! Access to requested file is denied by operation system
        ERROR_AccessDenied,
        //! File contains invalid format syntax
        ERROR_InvalidSyntax,
        //! PGE-X File contains a not closed data section
        ERROR_PGEX_SectionNotClosed,
        //! PGE-X File contains invalid syntax in the data entried
        ERROR_PGEX_InvalidSyntax,
        //! PGE-X File data field has invalid data type
        ERROR_PGEX_InvalidDataType
    };
    /*!
     * \brief Get detailed information from returned error code
     * \param errCode Returned error code by file parser
     * \return Understandable error description
     */
    static PGESTRING        getErrorString(ErrorCodes errCode);

    /*!
     * \brief SMBX64 Standrd specific violation codes
     */
    enum SMBX64_violations {
        //! File data is conforms to the SMBX64 Standard
        SMBX64_FINE             =0,
        //! File data structure has exited limit of section entries
        SMBX64EXC_SECTIONS      =1<<0,
        //! File data structure has exited limit of block entries
        SMBX64EXC_BLOCKS        =1<<1,
        //! File data structure has exited limit of Background Object entries
        SMBX64EXC_BGOS          =1<<2,
        //! File data structure has exited limit of NPC entries
        SMBX64EXC_NPCS          =1<<3,
        //! File data structure has exited limit of Water boxes entries
        SMBX64EXC_WATERBOXES    =1<<4,
        //! File data structure has exited limit of Warp entries
        SMBX64EXC_WARPS         =1<<5,
        //! File data structure has exited limit of Layer entries
        SMBX64EXC_LAYERS        =1<<6,
        //! File data structure has exited limit of Event entries
        SMBX64EXC_EVENTS        =1<<7,
        //! File data structure has exited limit of Tile entries
        SMBX64EXC_TILES         =1<<8,
        //! File data structure has exited limit of Scenery entries
        SMBX64EXC_SCENERIES     =1<<9,
        //! File data structure has exited limit of Path entries
        SMBX64EXC_PATHS         =1<<10,
        //! File data structure has exited limit of Level Entrance point entries
        SMBX64EXC_LEVELS        =1<<11,
        //! File data structure has exited limit of Music Box entries
        SMBX64EXC_MUSICBOXES    =1<<12
    };

//Check SMBX64 limits
    /*!
     * \brief Validates level file data structure to conformation to SMBX64 Standard
     * \param lvl Level file data structure
     * \return  SMBX64 Standrd specific violation code
     */
    static int              smbx64LevelCheckLimits(LevelData &lvl);
    /*!
     * \brief Validates World map file data structure to conformation to SMBX64 Standard
     * \param wld World map file data structure
     * \return  SMBX64 Standrd specific violation code
     */
    static int              smbx64WorldCheckLimits(WorldData &wld);

/******************************Internal stuff***********************************/
    /*!
     * \brief Removes edge dobule quoties characters from a string
     *        for example '"Meow"' (without signgle-quotes characters)
     *        will be converted into 'Meow' (without signgle-quotes characters)
     * \param str Input string with quites characters at begin and end of string
     * \return String with removed double quotes at edges.
     *         If string has no double quotes at edges, input string will be retured with no changes
     */
    static PGESTRING        removeQuotes(PGESTRING str);
    //! String which contains info about last happened error
    static PGESTRING        errorString;
};

#endif // FILE_FORMATS_H

