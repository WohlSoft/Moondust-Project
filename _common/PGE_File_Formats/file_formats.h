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

#if defined(PGE_ENGINE)||defined(PGE_EDITOR)
#include <data_configs/obj_npc.h>
#endif

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


    static MetaData ReadNonSMBX64MetaData(PGESTRING RawData, PGESTRING filePath="");
    static PGESTRING WriteNonSMBX64MetaData(MetaData metaData);

    /******************************Level files***********************************/
    static LevelData OpenLevelFile(PGESTRING filePath, bool silent=false); //!< Open supported level file via direct path
    static LevelData OpenLevelFileHeader(PGESTRING filePath);

    static LevelData ReadLevelFile(PGEFILE &inf); //!< Parse SMBX64 Level file by file stream
    static LevelData ReadExtendedLevelFile(PGEFILE &inf); //!< Parse PGE-X level file by file stream
    static LevelData dummyLvlDataArray(); //!< Generate empty level map

    // SMBX64 LVL File
    static LevelData ReadSMBX64LvlFileHeader(PGESTRING filePath); //!< Read file header only
    static LevelData ReadSMBX64LvlFile(PGESTRING RawData, PGESTRING filePath="", bool sielent=false); //!< Parse SMBX1-SMBX64 level
    static PGESTRING WriteSMBX64LvlFile(LevelData FileData, int file_format=64);  //!< Generate SMBX1-SMBX64 level raw data

    // PGE Extended Level File
    static LevelData ReadExtendedLvlFileHeader(PGESTRING filePath); //!< Read file header only
    static LevelData ReadExtendedLvlFile(PGESTRING RawData, PGESTRING filePath="", bool sielent=false); //!< Parse PGE-X level file
    static PGESTRING WriteExtendedLvlFile(LevelData FileData);  //!< Generate PGE-X level raw data

    // Lvl Data
    static LevelNPC dummyLvlNpc();
    static LevelDoor dummyLvlDoor();
    static LevelBlock dummyLvlBlock();
    static LevelBGO dummyLvlBgo();
    static LevelPhysEnv dummyLvlPhysEnv();
    static LevelLayer dummyLvlLayer();
    static LevelSMBX64Event dummyLvlEvent();
    static PlayerPoint dummyLvlPlayerPoint(int id=0);
    static LevelSection dummyLvlSection();


    /******************************World file***********************************/
    static WorldData OpenWorldFile(PGESTRING filePath, bool silent=false);
    static WorldData OpenWorldFileHeader(PGESTRING filePath);

    static WorldData ReadWorldFile(PGEFILE &inf); //!< Parse SMBX64 World file by file stream
    static WorldData ReadExtendedWorldFile(PGEFILE &inf); //!< Parse PGE-X World file by file stream
    static WorldData dummyWldDataArray(); //!< Generate empty world map

    // SMBX64 WLD File
    static WorldData ReadSMBX64WldFileHeader(PGESTRING filePath); //!< Read file header only
    static WorldData ReadSMBX64WldFile(PGESTRING RawData, PGESTRING filePath, bool sielent=false); //!< Parse SMBX1-SMBX64 world
    static PGESTRING WriteSMBX64WldFile(WorldData FileData, int file_format=64);  //!< Generate SMBX1-SMBX64 world raw data

    // PGE Extended World map File
    static WorldData ReadExtendedWldFileHeader(PGESTRING filePath); //!< Read file header only
    static WorldData ReadExtendedWldFile(PGESTRING RawData, PGESTRING filePath, bool sielent=false); //!< Parse PGE-X world file
    static PGESTRING WriteExtendedWldFile(WorldData FileData);  //!< Generate PGE-X world raw data

    //Wld Data
    static WorldTiles dummyWldTile();
    static WorldScenery dummyWldScen();
    static WorldPaths dummyWldPath();
    static WorldLevels dummyWldLevel();
    static WorldMusic dummyWldMusic();

    /****************************Save of game file********************************/
    static GamesaveData ReadSMBX64SavFile(PGESTRING RawData, PGESTRING filePath);  //!< Parse SMBX1-SMBX64 game save
    static GamesaveData ReadExtendedSaveFile(PGESTRING RawData, PGESTRING filePath, bool sielent=false);  //!< Parse PGE-X game save
    static PGESTRING      WriteExtendedSaveFile(GamesaveData &FileData);
    static GamesaveData dummySaveDataArray();

    //Save Data
    static saveCharacterState dummySavCharacterState();

    /****************************SMBX64 Config file********************************/
    static SMBX64_ConfigFile ReadSMBX64ConfigFile(PGESTRING RawData, PGESTRING filePath);  //!< Parse SMBX1-SMBX64 Config file
    static PGESTRING         WriteSMBX64ConfigFile(SMBX64_ConfigFile &FileData, int file_format);

    /******************************NPC.txt file***********************************/
    // SMBX64 NPC.TXT File
    static NPCConfigFile ReadNpcTXTFile(PGEFILE &inf, bool IgnoreBad=false); //read
    static PGESTRING WriteNPCTxtFile(NPCConfigFile FileData);                //write

    static NPCConfigFile CreateEmpytNpcTXTArray();
    #if defined(PGE_ENGINE)||defined(PGE_EDITOR)
    static obj_npc mergeNPCConfigs(obj_npc &global, NPCConfigFile &local, QSize captured=QSize(0,0));
    #endif

    //common
    static void BadFileMsg(PGESTRING fileName_DATA, int str_count, PGESTRING line);
    static PGESTRING removeQuotes(PGESTRING str); // Remove quotes from begin and end

    static PGESTRING errorString; //!< String which contains info about last happened error
private:
    static bool silentMode;
};

#endif // FILE_FORMATS_H
