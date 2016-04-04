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

/*! \file file_formats.h
 *  \brief Main header of PGE File Library which includes all provided features set
 */


#ifndef FILE_FORMATS_H
#define FILE_FORMATS_H

#include "pge_file_lib_globs.h"
#include "lvl_filedata.h"
#include "npc_filedata.h"
#include "wld_filedata.h"
#include "save_filedata.h"
#include "smbx64_cnf_filedata.h"

#ifdef __GNUC__
#define DEPRECATED(func) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#define DEPRECATED(func) __declspec(deprecated) func
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define DEPRECATED(func) func
#endif

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

/******************************non-SMBX64 Meda-data file***********************************/
    /*!
     * \brief Parses non-SMBX64 meta-data from additional *.meta files
     *        there are contains data which impossible to save into SMBX64 LVL file
     *        therefore it will be saved into additional *.meta file
     * \param [__in] filePath Full path to file with meta-data
     * \param [__out] FileData Meta-data structure
     * \return true if file successfully opened and parsed, false if error occouped
     */
    static bool ReadNonSMBX64MetaDataF(PGESTRING  filePath, MetaData &FileData);
    /*!
     * \brief Parses non-SMBX64 meta-data from additional *.meta files
     *        there are contains data which impossible to save into SMBX64 LVL file
     *        therefore it will be saved into additional *.meta file
     * \param [__in] rawdata Raw data of the meta-data
     * \param [__in] filePath Full path to file with meta-data (needed to detect custom directory info)
     * \param [__out] FileData Meta-data structure
     * \return true if file successfully opened and parsed, false if error occouped
     */
    static bool ReadNonSMBX64MetaDataRaw(PGESTRING &rawdata, PGESTRING  filePath, MetaData &FileData);
    /*!
     * \brief Parses non-SMBX64 meta-data from additional *.meta files
     *        there are contains data which impossible to save into SMBX64 LVL file
     *        therefore it will be saved into additional *.meta file
     * \param [__in] in File input descriptor
     * \param [__out] FileData FileData Meta-data structure
     * \return true if file successfully opened and parsed, false if error occouped
     */
    static bool ReadNonSMBX64MetaDataFile(PGE_FileFormats_misc::TextInput &in, MetaData /*output*/ &FileData);
    /*!
     * \brief Saves data of non-SMBX meta-data into the file from Meta-data structure
     * \param [__in] filePath Full path to file where need to save
     * \param [__in] metaData Meta-data structure
     * \return true if file successfully opened and parsed, false if error occouped
     */
    static bool WriteNonSMBX64MetaDataF(PGESTRING  filePath, MetaData &metaData);
    /*!
     * \brief Generates raw data of non-SMBX meta-data from level data structure
     * \param [__in] metaData Meta-data structure
     * \param [__out] rawdata raw PGE-X data of meta-data structure
     * \return true if file successfully opened and parsed, false if error occouped
     */
    static bool WriteNonSMBX64MetaDataRaw(MetaData &metaData, PGESTRING &rawdata);
    /*!
     * \brief Writes raw data of non-SMBX meta-data from level data through file descriptor
     * \param [__inout] out File output descriptor
     * \param [__in] metaData Meta-data structure
     * \return true if file successfully opened and parsed, false if error occouped
     */
    static bool WriteNonSMBX64MetaData(PGE_FileFormats_misc::TextOutput &out, MetaData /*Output*/ &metaData);


/******************************Level files***********************************/
    /*!
     * \brief Supported level file formats
     */
    enum LevelFileFormat
    {
        //! PGE-X LVLX Level File format
        LVL_PGEX=0,
        //! SMBX1...64 LVL Level File format
        LVL_SMBX64,
        //! SMBX-38A LVL Level File format
        LVL_SMBX38A,
    };
    /*!
     * \brief Parses a level file with auto-detection of a file type (SMBX1...64 LVL or PGE-LVLX)
     * \param [__in] filePath Full path to file which must be opened
     * \param [__out] FileData Level data structure
     * \return true if file successfully opened and parsed, false if error occouped
     */
    static bool OpenLevelFile(PGESTRING filePath, LevelData &FileData);
    /*!
     * \brief Parses a level file header only with auto-detection of a file type (SMBX1...64 LVL or PGE-LVLX)
     * \param [__in] filePath Full path to file which must be opened
     * \param [__out] data Level data structure (with initialized header data only)
     * \return true if file successfully opened and parsed, false if error occouped
     */
    static bool OpenLevelFileHeader(PGESTRING filePath, LevelData &data);
    /*!
     * \brief Save a level file to the disk
     * \param [__in] FileData Level data structure
     * \param [__in] filePath Path to file to save encoded in UTF-8 (for STL-version)
     * \param [__in] format Target file format (PGE LVLX, SMBX1...64 LVL, SMBX-38A LVL)
     * \param [__in] FormatVersion Version of target SMBX1...64 file. Takes no effect for other file formats
     * \return true if file successfully saved
     */
    static bool SaveLevelFile(LevelData &FileData, PGESTRING filePath, LevelFileFormat format, unsigned int FormatVersion=64);
    /*!
     * \brief Save a level file to the raw string
     * \param FileData Level data structure
     * \param filePath Path to file to save encoded in UTF-8 (for STL-version)
     * \param format Target file format (PGE LVLX, SMBX1...64 LVL, SMBX-38A LVL)
     * \param FormatVersion Version of target SMBX1...64 file. Takes no effect for other file formats
     * \return true if data successfully generated
     */
    static bool SaveLevelData(LevelData &FileData, PGESTRING &RawData, LevelFileFormat format, unsigned int FormatVersion=64);

// SMBX64 LVL File
    /*!
     * \brief Parses SMBX1...64 level file header and skips other part of a file
     * \param [__in] filePath Full path to level file
     * \param [__out] FileData Level data structure (with initialized header data only)
     * \return true if file successfully parsed, false if error occouped
     */
    static bool ReadSMBX64LvlFileHeader(PGESTRING filePath, LevelData &FileData);
    /*!
     * \brief Parses SMBX1...64 level file data
     * \param [__in] filePath Full path to the file (if empty, custom data in the episode and in the custom directories are will be inaccessible)
     * \param [__out] FileData Level data structure
     * \return true if file successfully parsed, false if error occouped
     */
    static bool ReadSMBX64LvlFileF(PGESTRING  filePath, LevelData &FileData);
    /*!
     * \brief Parses SMBX1...64 level file data
     * \param [__in] rawdata Raw data of the SMBX1...64 level file
     * \param [__in] filePath Full path to the file (if empty, custom data in the episode and in the custom directories are will be inaccessible)
     * \param [__Out] FileData
     * \return true if file successfully parsed, false if error occouped
     */
    static bool ReadSMBX64LvlFileRaw(PGESTRING &rawdata, PGESTRING  filePath, LevelData &FileData);
    /*!
     * \brief ReadSMBX64LvlFile
     * \param [__in] in
     * \param [__out] FileData
     * \return true if file successfully parsed, false if error occouped
     */
    static bool ReadSMBX64LvlFile(PGE_FileFormats_misc::TextInput &in, LevelData /*output*/ &FileData);
    /*!
     * \brief Generates SMBX1...64 Level file data and saves into file
     * \param [__in] filePath Target file path
     * \param [__in] FileData Level data structure
     * \param [__in] file_format SMBX file format version number (from 0 to 64) [Example of level in SMBX0 format is intro.dat included with SMBX 1.0]
     * \return true if file successfully saved, false if error occouped
     */
    static bool WriteSMBX64LvlFileF(PGESTRING  filePath, LevelData &FileData, int file_format=64);
    /*!
     * \brief Generates SMBX1...64 Level file data and saves into raw string
     * \param [__in] FileData Target file path
     * \param [__out] rawdata Level data structure
     * \param [__in] file_format SMBX file format version number (from 0 to 64) [Example of level in SMBX0 format is intro.dat included with SMBX 1.0]
     * \return true if file successfully saved, false if error occouped
     */
    static bool WriteSMBX64LvlFileRaw(LevelData &FileData, PGESTRING &rawdata, int file_format=64);
    /*!
     * \brief Generates SMBX1...64 Level file data and saves it through file output descriptor
     * \param [__inout] out Output file descriptor
     * \param [__in] FileData Target file path
     * \param [__in] file_format SMBX file format version number (from 0 to 64) [Example of level in SMBX0 format is intro.dat included with SMBX 1.0]
     * \return true if file successfully saved, false if error occouped
     */
    static bool WriteSMBX64LvlFile(PGE_FileFormats_misc::TextOutput &out, LevelData /*output*/ &FileData, int file_format=64);

// SMBX-38A LVL File
    /*!
     * \brief Parses SMBX-38A level file header and skips other part of a file
     * \param [__in] filePath Full path to level file
     * \param [__out] FileData Level data structure (with initialized header data only)
     * \return true if file successfully parsed, false if error occouped
     */
    static bool ReadSMBX38ALvlFileHeader(PGESTRING filePath, LevelData &FileData);
    /*!
     * \brief Parses SMBX-38A level file data from file
     * \param [__in] filePath Full path to flie
     * \param [__out] FileData
     * \return true if file successfully parsed, false if error occouped
     */
    static bool ReadSMBX38ALvlFileF(PGESTRING  filePath, LevelData &FileData);
    /*!
     * \brief Parses SMBX-38A level file data from raw data string
     * \param [__in] rawdata Raw-data string contains SMBX-38A Level file data
     * \param [__in] filePath Full path to the file (if empty, custom data in the episode and in the custom directories are will be inaccessible)
     * \param [__out] FileData Level data structure
     * \return true if file successfully parsed, false if error occouped
     */
    static bool ReadSMBX38ALvlFileRaw(PGESTRING &rawdata, PGESTRING  filePath, LevelData &FileData);
    /*!
     * \brief Parses SMBX-38A level file data from raw data string
     * \param [__in] in File input descriptor
     * \param [__out] FileData FileData Level data structure
     * \return true if file successfully parsed, false if error occouped
     */
    static bool ReadSMBX38ALvlFile(PGE_FileFormats_misc::TextInput &in, LevelData /*output*/ &FileData);
    /*!
     * \brief Parses SMBX-38A level file data from raw data string (Old algorithm)
     * \param [__in] in File input descriptor
     * \param [__out] FileData FileData Level data structure
     * \return true if file successfully parsed, false if error occouped
     */
    static bool ReadSMBX38ALvlFile_OLD(PGE_FileFormats_misc::TextInput &in, LevelData &FileData);
    /*!
     * \brief Generates SMBX-38A Level file data and saves into file
     * \param [__in] filePath Target file path
     * \param [__in] FileData Level data structure
     * \return true if file successfully saved, false if error occouped
     */
    static bool WriteSMBX38ALvlFileF(PGESTRING  filePath, LevelData &FileData);
    /*!
     * \brief Generates SMBX-38A Level file data and saves into raw string
     * \param [__in] FileData Target file path
     * \param [__out] rawdata Raw data string in the SMBX-38A level format
     * \return true if file successfully saved, false if error occouped
     */
    static bool WriteSMBX38ALvlFileRaw(LevelData &FileData, PGESTRING &rawdata);
    /*!
     * \brief Generates SMBX-38A Level file data and saves it through file output descriptor
     * \param [__inout] out Output file descriptor
     * \param [__in] FileData Target file path
     * \return true if file successfully saved, false if error occouped
     */
    static bool WriteSMBX38ALvlFile(PGE_FileFormats_misc::TextOutput &out, LevelData &FileData);



// PGE Extended Level File
    /*!
     * \brief Parses PGE-X Level file header from the file
     * \param filePath Full path to PGE-X Level file
     * \param FileData Level data structure (with initialized header data only)
     * \return true if file successfully parsed, false if error occouped
     */
    static bool ReadExtendedLvlFileHeader(PGESTRING filePath, LevelData &FileData);
    /*!
     * \brief Parses PGE-X level file data from file
     * \param [__in] filePath Full path to the file
     * \param [__out] FileData Level data structure
     * \return true if file successfully parsed, false if error occouped
     */
    static bool ReadExtendedLvlFileF(PGESTRING  filePath, LevelData &FileData);
    /*!
     * \brief Parses PGE-X level file data from raw data string
     * \param [__in] rawdata Raw data string in the PGE-X level format
     * \param [__in] filePath Full path to the file (if empty, custom data in the episode and in the custom directories are will be inaccessible)
     * \param [__out] FileData Level data structure
     * \return true if file successfully parsed, false if error occouped
     */
    static bool ReadExtendedLvlFileRaw(PGESTRING &rawdata, PGESTRING  filePath, LevelData &FileData);
    /*!
     * \brief Parses PGE-X level file data from file input descriptor
     * \param [__in] in File Input descriptor
     * \param [__out] FileData Level data structure
     * \return true if file successfully parsed, false if error occouped
     */
    static bool ReadExtendedLvlFile(PGE_FileFormats_misc::TextInput &in, LevelData /*output*/ &FileData);
    /*!
     * \brief Generates PGE-X Level file
     * \param [__in] filePath Target file path
     * \param [__in] FileData Level data structure
     * \return true if file successfully saved, false if error occouped
     */
    static bool WriteExtendedLvlFileF(PGESTRING  filePath, LevelData &FileData);
    /*!
     * \brief Generates PGE-X Level raw data string
     * \param [__in] FileData Level data structure
     * \param [__out] rawdata Raw data string in the PGE-X level format
     * \return true if file successfully saved, false if error occouped
     */
    static bool WriteExtendedLvlFileRaw(LevelData &FileData, PGESTRING &rawdata);
    /*!
     * \brief Generates PGE-X Level data and sends into file output descriptor
     * \param [__inout] out Output file descriptor
     * \param [__in] FileData Level data structure
     * \return true if file successfully saved, false if error occouped
     */
    static bool WriteExtendedLvlFile(PGE_FileFormats_misc::TextOutput &out, LevelData /*output*/ &FileData);

// Lvl Data
    /*!
     * \brief Generates blank initialized level data structure
     * \return Level data structure
     */
    static LevelData        CreateLevelData();
    /*!
     * \brief Initializes blank level data structure
     * \param NewFileData blank level data structure
     */
    static void CreateLevelData(LevelData &NewFileData);
    /*!
     * \brief Initializes header only of the blank level data structure
     * \param NewFileData blank level data structure
     */
    static void CreateLevelHeader(LevelData &NewFileData);
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
     * \brief Initalizes Level specific Variable entry
     * \param vname name of variable
     * \return Initialized with default properties level specific Variable entry
     */
    static LevelVariable    CreateLvlVariable(PGESTRING vname);
    /*!
     * \brief Initalizes Level specific Script Entry
     * \param name name of script
     * \param lang language code of script (Lua, Luna-Autocode or Tea-Script)
     * \return Initialized with default properties level specific Script entry
     */
    static LevelScript      CreateLvlScript(PGESTRING name, int lang=LevelScript::LANG_LUA);

    /*!
     * \brief Appends internal layers and events if not exists
     * \param FileData initalized and filled level file
     */
    static void             LevelAddInternalEvents(LevelData &FileData);
    /*!
     * \brief Optimizing level data for SMBX64 Standard requirements
     * \param [__inout] lvl Level data structure object
     */
    static void             smbx64LevelPrepare(LevelData &lvl);
    /*!
     * \brief Counts number of SMBX-64 Stars (NPC-97 and NPC-196). Friendly NPC's are not counts as stars.
     * \param [__inout] lvl Level data structure
     * \return number of found SMBX-64 stars
     */
    static int smbx64CountStars(LevelData &lvl);
    /*!
     * \brief Sorts blocks array by Y->X positions in the given level data structure.
     *        By SMBX64 standard, blocks array must be sorted because it is required for
     *        NPC's collision detection algorithm of SMBX Engine
     * \param [__inout] lvl Level data structure object
     */
    static void             smbx64LevelSortBlocks(LevelData &lvl);
    /*!
     * \brief Sorts Background objects by special order priority value
     *        Modifying of order priority values allowing to force specific non-foreground BGO's
     *        to be rendered foreground
     * \param [__inout] lvl Level data structure object
     */
    static void             smbx64LevelSortBGOs(LevelData &lvl);


/******************************World map files***********************************/
    /*!
     * \brief Supported world map file formats
     */
    enum WorldFileFormat{
        //! PGE-X WLDX World map file format
        WLD_PGEX=0,
        //! SMBX1...64 WLD World map file format
        WLD_SMBX64,
        //! SMBX-38A WLD World map file format
        WLD_SMBX38A
    };

    /*!
     * \brief Parses a world map file with auto-detection of a file type (SMBX1...64 LVL or PGE-WLDX)
     * \param [__in] filePath Full path to file which must be opened
     * \param [__out] data World data structure
     * \return true on success file reading, false if error was occouped
     */
    static bool OpenWorldFile(PGESTRING filePath, WorldData &data);
    /*!
     * \brief Parses a world map file header only with auto-detection of a file type (SMBX1...64 LVL or PGE-WLDX)
     * \param [__in] filePath Full path to file which must be opened
     * \param [__out] data World data structure (with initialized header data only)
     * \return true on success file reading, false if error was occouped
     */
    static bool OpenWorldFileHeader(PGESTRING filePath, WorldData &data);
    /*!
     * \brief Save a world file to the disk
     * \param [__in] FileData World data structure
     * \param [__in] filePath Path to file to save encoded in UTF-8 (for STL-version)
     * \param [__in] format Target file format (PGE WLDX, SMBX1...64 WLD, SMBX-38A WLD)
     * \param [__in] FormatVersion Version of target SMBX1...64 file. Takes no effect for other file formats
     * \return true if file successfully saved
     */
    static bool SaveWorldFile(WorldData &FileData, PGESTRING filePath, WorldFileFormat format, unsigned int FormatVersion=64);
    /*!
     * \brief Save a world map file to the raw string
     * \param [__in] FileData World data structure
     * \param [__in] filePath Path to file to save encoded in UTF-8 (for STL-version)
     * \param [__in] format Target file format (PGE WLDX, SMBX1...64 WLD, SMBX-38A WLD)
     * \param [__in] FormatVersion Version of target SMBX1...64 file. Takes no effect for other file formats
     * \return true if data successfully generated
     */
    static bool SaveWorldData(WorldData &FileData, PGESTRING &RawData, WorldFileFormat format, unsigned int FormatVersion=64);


// SMBX64 WLD File
    /*!
     * \brief Parsed SMBX1...64 World map file header only
     * \param [__in] filePath Full path to file to open
     * \param [__out] Filedata World data structure (with initialized header data only)
     * \return true if file successfully parsed, false if error occouped
     */
    static bool ReadSMBX64WldFileHeader(PGESTRING filePath, WorldData &FileData);
    /*!
     * \brief Parses SMBX1...64 World map file from raw data from file
     * \param [__in] filePath Full path to file to open
     * \param [__out] FileData World data structure
     * \return true if file successfully parsed, false if error occouped
     */
    static bool ReadSMBX64WldFileF(PGESTRING  filePath, WorldData &FileData);
    /*!
     * \brief Parses SMBX1...64 World map file from raw data from raw-data string
     * \param [__in] rawdata Raw-data string which contains SMBX1...64 World map data
     * \param [__in] filePath
     * \param [__out] FileData World data structure
     * \return true if file successfully parsed, false if error occouped
     */
    static bool ReadSMBX64WldFileRaw(PGESTRING &rawdata, PGESTRING  filePath, WorldData &FileData);
    /*!
     * \brief Parses SMBX1...64 World map file from raw data from file input descriptor
     * \param [__in] in File Input descriptor
     * \param [__out] FileData World data structure
     * \return true if file successfully parsed, false if error occouped
     */
    static bool ReadSMBX64WldFile(PGE_FileFormats_misc::TextInput &in, WorldData /*output*/ &FileData);
    /*!
     * \brief Saves level data into file of SMBX1...64 World map format
     * \param [__in] filePath Target file path
     * \param [__in] FileData World map data structure
     * \param [__in] file_format SMBX file format version number (from 0 to 64) [Example of level in SMBX0 format is intro.dat included with SMBX 1.0]
     * \return true if file successfully saved, false if error occouped
     */
    static bool WriteSMBX64WldFileF(PGESTRING  filePath, WorldData &FileData, int file_format=64);
    /*!
     * \brief Generates raw data string in SMBX1...64 World map format
     * \param [__in] FileData World map data structure
     * \param [__out] rawdata Raw data string in SMBX1...64 World map format
     * \param [__in] file_format SMBX file format version number (from 0 to 64) [Example of level in SMBX0 format is intro.dat included with SMBX 1.0]
     * \return true if file successfully saved, false if error occouped
     */
    static bool WriteSMBX64WldFileRaw(WorldData &FileData, PGESTRING &rawdata, int file_format=64);
    /*!
     * \brief Writes world map data into file output descriptor of SMBX1...64 World map format
     * \param [__inout] out Output file descriptor
     * \param [__in] FileData World map data structure
     * \param [__in] file_format SMBX file format version number (from 0 to 64) [Example of level in SMBX0 format is intro.dat included with SMBX 1.0]
     * \return true if file successfully saved, false if error occouped
     */
    static bool WriteSMBX64WldFile(PGE_FileFormats_misc::TextOutput &out, WorldData /*output*/ &FileData, int file_format=64);

// PGE Extended World map File
    /*!
     * \brief Parsed PGE-X World map file header only
     * \param filePath Full path to file which must be opened
     * \param FileData World data structure (with initialized header data only)
     * \return true if file successfully saved, false if error occouped
     */
    static bool ReadExtendedWldFileHeader(PGESTRING filePath, WorldData &FileData);
    /*!
     * \brief Parses PGE-X World map file from file
     * \param [__in] filePath
     * \param [__out] FileData World map data structure
     * \return true if file successfully parsed, false if error occouped
     */
    static bool ReadExtendedWldFileF(PGESTRING  filePath, WorldData &FileData);
    /*!
     * \brief Parses PGE-X World map file from raw data string
     * \param [__in] rawdata Raw data strign with PGE-X World map data
     * \param [__in] filePath Full path to the file (if empty, custom data in the episode and in the custom directories are will be inaccessible)
     * \param [__out] FileData World map data structure
     * \return true if file successfully parsed, false if error occouped
     */
    static bool ReadExtendedWldFileRaw(PGESTRING &rawdata, PGESTRING  filePath, WorldData &FileData);
    /*!
     * \brief Parses PGE-X World map file from file input descriptor
     * \param [__in] in File Input descriptor
     * \param [__out] FileData World map data structure
     * \return true if file successfully parsed, false if error occouped
     */
    static bool ReadExtendedWldFile(PGE_FileFormats_misc::TextInput &in, WorldData /*output*/ &FileData);
    /*!
     * \brief Saves world map data into file of PGE-X World map format
     * \param [__in] filePath Target file path
     * \param [__in] FileData World map data structure
     * \return true if file successfully saved, false if error occouped
     */
    static bool WriteExtendedWldFileF(PGESTRING  filePath, WorldData &FileData);
    /*!
     * \brief Generates raw data string in PGE-X World map format
     * \param [__in] FileData World map data structure
     * \param [__out] rawdata Raw data string in PGE-X World map format
     * \return true if file successfully saved, false if error occouped
     */
    static bool WriteExtendedWldFileRaw(WorldData &FileData, PGESTRING &rawdata);
    /*!
     * \brief Generates data into file output descriptor in PGE-X World map format
     * \param [__inout] out Output file descriptor
     * \param [__in] FileData World map data structure
     * \return true if file successfully saved, false if error occouped
     */
    static bool WriteExtendedWldFile(PGE_FileFormats_misc::TextOutput &out, WorldData /*output*/ &FileData);

//Wld Data
    /*!
     * \brief Initializes world map structure header
     * \param [__out] NewFileData World map data structure with initialized header only
     */
    static void CreateWorldHeader(WorldData &NewFileData);
    /*!
     * \brief Generates blank initialized World map data structure
     * \param [__out] NewFileData World map data structure
     */
    static void CreateWorldData(WorldData &NewFileData);
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

// SMBX1..64 SAV file
    /*!
     * \brief Parses SMBX1...64 Game save data from file
     * \param [__in] filePath File path to open
     * \param [__out] FileData Game save data structure
     * \return true if file successfully parsed, false if error occouped
     */
    static bool ReadSMBX64SavFileF(PGESTRING  filePath, GamesaveData &FileData);
    /*!
     * \brief Parses SMBX1...64 Game save data from raw data string
     * \param [__in] rawdata raw data string in SMBX1..64 game save format
     * \param [__in] filePath Path to original file
     * \param [__out] FileData Game save data structure
     * \return true if file successfully parsed, false if error occouped
     */
    static bool ReadSMBX64SavFileRaw(PGESTRING &rawdata, PGESTRING  filePath, GamesaveData &FileData);
    /*!
     * \brief Parses SMBX1...64 Game save data from input descriptor
     * \param [__in] in File Input descriptor
     * \param [__out] FileData Game save data structure
     * \return true if file successfully parsed, false if error occouped
     */
    static bool ReadSMBX64SavFile(PGE_FileFormats_misc::TextInput &in, GamesaveData /*output*/ &FileData);

//PGE-X SAVX file
    /*!
     * \brief Parses PGE-X level file data from file
     * \param [__in] filePath Full path to the file
     * \param [__out] FileData Game save data structure
     * \return true if file successfully parsed, false if error occouped
     */
    static bool ReadExtendedSaveFileF(PGESTRING  filePath, GamesaveData &FileData);
    /*!
     * \brief Parses PGE-X level file data from raw data string
     * \param [__in] rawdata Raw data string in the PGE-X Game save format
     * \param [__in] filePath Full path to the file
     * \param [__out] FileData Game save data structure
     * \return true if file successfully parsed, false if error occouped
     */
    static bool ReadExtendedSaveFileRaw(PGESTRING &rawdata, PGESTRING  filePath, GamesaveData &FileData);
    /*!
     * \brief Parses PGE-X level file data from file input descriptor
     * \param [__in] in File Input descriptor
     * \param [__out] FileData Game save data structure
     * \return true if file successfully parsed, false if error occouped
     */
    static bool ReadExtendedSaveFile(PGE_FileFormats_misc::TextInput &in, GamesaveData &FileData);
    /*!
     * \brief Saves Game save data into file of PGE-X Game save format
     * \param [__in] filePath Target file path
     * \param [__in] FileData Game save data structure
     * \return true if file successfully saved, false if error occouped
     */
    static bool WriteExtendedSaveFileF(PGESTRING  filePath, GamesaveData &FileData);
    /*!
     * \brief Generates raw data string in PGE-X Game save format
     * \param [__in] FileData World map data structure
     * \param [__out] rawdata Raw data string in PGE-X Game save format
     * \return true if file successfully saved, false if error occouped
     */
    static bool WriteExtendedSaveFileRaw(GamesaveData &FileData, PGESTRING &rawdata);
    /*!
     * \brief WriteExtendedWldFile
     * \param [__inout] out Output file descriptor
     * \param [__in] FileData Game save data structure
     * \return true if file successfully saved, false if error occouped
     */
    static bool WriteExtendedSaveFile(PGE_FileFormats_misc::TextOutput &out, GamesaveData &FileData);

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
     * \param [__in] RawData raw data string in SMBX Engine config file format
     * \return SMBX Engine specific config structure
     */
    DEPRECATED(static SMBX64_ConfigFile ReadSMBX64ConfigFile(PGESTRING RawData));


    /*!
     * \brief Parses SMBX Engine config data from file
     * \param [__in] filePath Filepath to open
     * \param [__out] FileData SMBX Engine specific config structure
     * \return true if file successfully parsed, false if error occouped
     */
    static bool ReadSMBX64ConfigFileF(PGESTRING  filePath, SMBX64_ConfigFile &FileData);
    /*!
     * \brief Parses SMBX Engine config data from raw data string
     * \param [__in] rawdata Raw data string contains SMBX Engine config data
     * \param [__in] filePath
     * \param [__out] FileData SMBX Engine specific config structure
     * \return true if file successfully parsed, false if error occouped
     */
    static bool ReadSMBX64ConfigFileRaw(PGESTRING &rawdata, PGESTRING  filePath, SMBX64_ConfigFile &FileData);
    /*!
     * \brief Parses SMBX Engine config data from file input descriptor
     * \param [__in] in File Input descriptor
     * \param [__out] FileData SMBX Engine specific config structure
     * \return true if file successfully parsed, false if error occouped
     */
    static bool ReadSMBX64ConfigFile(PGE_FileFormats_misc::TextInput &in, SMBX64_ConfigFile /*output*/ &FileData);



    /*!
     * \brief Generates SMBX Engine specific config file raw data string
     * \param [__in] FileData SMBX Engine specific config structure
     * \param [__in] file_format SMBX file format version number (from 0 to 64)
     * \return raw data string in SMBX Engine config file format
     */
    DEPRECATED(static PGESTRING WriteSMBX64ConfigFile(SMBX64_ConfigFile &FileData, int file_format));


    /*!
     * \brief Saves SMBX Engine specific config data into file of SMBX1...64 World map format
     * \param [__in] filePath Target file path
     * \param [__in] FileData World map data structure
     * \param [__in] file_format SMBX file format version number (from 0 to 64) [Example of level in SMBX0 format is intro.dat included with SMBX 1.0]
     * \return true if file successfully saved, false if error occouped
     */
    static bool WriteSMBX64ConfigFileF(PGESTRING  filePath, SMBX64_ConfigFile &FileData, int file_format=64);
    /*!
     * \brief Generates raw data string in SMBX1...64 SMBX Engine specific config format
     * \param [__in] FileData World map data structure
     * \param [__out] rawdata Raw data string in SMBX1...64 World map format
     * \param [__in] file_format SMBX file format version number (from 0 to 64) [Example of level in SMBX0 format is intro.dat included with SMBX 1.0]
     * \return true if file successfully saved, false if error occouped
     */
    static bool WriteSMBX64ConfigFileRaw(SMBX64_ConfigFile &FileData, PGESTRING &rawdata, int file_format=64);
    /*!
     * \brief Writes data into file output descriptor of SMBX1...64 SMBX Engine specific config format
     * \param [__inout] out Output file descriptor
     * \param [__in] FileData SMBX Engine specific config data structure
     * \param [__in] file_format SMBX file format version number (from 0 to 64) [Example of level in SMBX0 format is intro.dat included with SMBX 1.0]
     * \return true if file successfully saved, false if error occouped
     */
    static bool WriteSMBX64ConfigFile(PGE_FileFormats_misc::TextOutput &out, SMBX64_ConfigFile &FileData, int file_format=64);


/******************************NPC.txt file***********************************/
// SMBX64 NPC.TXT File
    /*!
     * \brief Parses SMBX64 NPC.txt config data from file
     * \param [__in] filePath Filepath to open
     * \param [__out] FileData SMBX Engine specific config structure
     * \return true if file successfully parsed, false if error occouped
     */
    static bool ReadNpcTXTFileF(PGESTRING  filePath, NPCConfigFile &FileData, bool IgnoreBad=false);
    /*!
     * \brief Parses SMBX64 NPC.txt config data from raw data string
     * \param [__in] rawdata Raw data string contains SMBX Engine config data
     * \param [__out] FileData SMBX Engine specific config structure
     * \return true if file successfully parsed, false if error occouped
     */
    static bool ReadNpcTXTFileRAW(PGESTRING &rawdata, NPCConfigFile &FileData, bool IgnoreBad=false);
    /*!
     * \brief Parses SMBX64 NPC.txt config data from file input descriptor
     * \param [__in] in File Input descriptor
     * \param [__out] FileData SMBX Engine specific config structure
     * \return true if file successfully parsed, false if error occouped
     */
    static bool ReadNpcTXTFile(PGE_FileFormats_misc::TextInput &in, NPCConfigFile &FileData, bool IgnoreBad=false);

    /*!
     * \brief Saves world map data into file of SMBX64 NPC.txt config data format
     * \param [__in] filePath Target file path
     * \param [__in] FileData SMBX64 NPC.txt config data structure
     * \return true if file successfully saved, false if error occouped
     */
    static bool WriteNPCTxtFileF(PGESTRING  filePath, NPCConfigFile &FileData);
    /*!
     * \brief Generates raw data string in SMBX64 NPC.txt config data format
     * \param [__in] FileData SMBX64 NPC.txt config data structure
     * \param [__out] rawdata Raw data string in SMBX64 NPC.txt config data format
     * \return true if file successfully saved, false if error occouped
     */
    static bool WriteNPCTxtFileRaw(NPCConfigFile &FileData, PGESTRING &rawdata);
    /*!
     * \brief Generates data into file output descriptor in SMBX64 NPC.txt config data format
     * \param [__inout] out Output file descriptor
     * \param [__in] FileData World map data structure
     * \return true if file successfully saved, false if error occouped
     */
    static bool WriteNPCTxtFile(PGE_FileFormats_misc::TextOutput &out, NPCConfigFile &FileData);

    /*!
     * \brief Initialize blank NPC Customizing Config data structure
     * \return Blank NPC Customizing Config data structure
     */
    static NPCConfigFile    CreateEmpytNpcTXT();

/******************************Common stuff***********************************/
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
        SMBX64_FINE             = 0x000,
        //! File data structure has exited limit of section entries
        SMBX64EXC_SECTIONS      = 0x001,
        //! File data structure has exited limit of block entries
        SMBX64EXC_BLOCKS        = 0x002,
        //! File data structure has exited limit of Background Object entries
        SMBX64EXC_BGOS          = 0x004,
        //! File data structure has exited limit of NPC entries
        SMBX64EXC_NPCS          = 0x008,
        //! File data structure has exited limit of Water boxes entries
        SMBX64EXC_WATERBOXES    = 0x010,
        //! File data structure has exited limit of Warp entries
        SMBX64EXC_WARPS         = 0x020,
        //! File data structure has exited limit of Layer entries
        SMBX64EXC_LAYERS        = 0x040,
        //! File data structure has exited limit of Event entries
        SMBX64EXC_EVENTS        = 0x080,
        //! File data structure has exited limit of Tile entries
        SMBX64EXC_TILES         = 0x100,
        //! File data structure has exited limit of Scenery entries
        SMBX64EXC_SCENERIES     = 0x200,
        //! File data structure has exited limit of Path entries
        SMBX64EXC_PATHS         = 0x400,
        //! File data structure has exited limit of Level Entrance point entries
        SMBX64EXC_LEVELS        = 0x800,
        //! File data structure has exited limit of Music Box entries
        SMBX64EXC_MUSICBOXES    = 0x1000
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

