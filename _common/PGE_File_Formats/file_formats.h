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

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include "lvl_filedata.h"
#include "npc_filedata.h"
#include "wld_filedata.h"
#include "save_filedata.h"

#if defined(PGE_ENGINE)||defined(PGE_EDITOR)
#include <data_configs/obj_npc.h>
#endif

class FileFormats: public QObject
{
    Q_OBJECT

public:
    //File format read/write functions


    static MetaData ReadNonSMBX64MetaData(QString RawData, QString filePath="");
    static QString WriteNonSMBX64MetaData(MetaData metaData);

    /******************************Level files***********************************/
    static LevelData OpenLevelFile(QString filePath); //!< Open supported level file via direct path
    static LevelData OpenLevelFileHeader(QString filePath);

    static LevelData ReadLevelFile(QFile &inf); //!< Parse SMBX64 Level file by file stream
    static LevelData ReadExtendedLevelFile(QFile &inf); //!< Parse PGE-X level file by file stream
    static LevelData dummyLvlDataArray(); //!< Generate empty level map

    // SMBX64 LVL File
    static LevelData ReadSMBX64LvlFileHeader(QString filePath); //!< Read file header only
    static LevelData ReadSMBX64LvlFile(QString RawData, QString filePath="", bool sielent=false); //!< Parse SMBX1-SMBX64 level
    static QString WriteSMBX64LvlFile(LevelData FileData, int file_format=64);  //!< Generate SMBX1-SMBX64 level raw data

    // PGE Extended Level File
    static LevelData ReadExtendedLvlFileHeader(QString filePath); //!< Read file header only
    static LevelData ReadExtendedLvlFile(QString RawData, QString filePath="", bool sielent=false); //!< Parse PGE-X level file
    static QString WriteExtendedLvlFile(LevelData FileData);  //!< Generate PGE-X level raw data

    // Lvl Data
    static LevelNPC dummyLvlNpc();
    static LevelDoors dummyLvlDoor();
    static LevelBlock dummyLvlBlock();
    static LevelBGO dummyLvlBgo();
    static LevelPhysEnv dummyLvlPhysEnv();
    static LevelLayers dummyLvlLayer();
    static LevelEvents dummyLvlEvent();
    static PlayerPoint dummyLvlPlayerPoint(int id=0);
    static LevelSection dummyLvlSection();


    /******************************World file***********************************/
    static WorldData OpenWorldFile(QString filePath);
    static WorldData OpenWorldFileHeader(QString filePath);

    static WorldData ReadWorldFile(QFile &inf); //!< Parse SMBX64 World file by file stream
    static WorldData ReadExtendedWorldFile(QFile &inf); //!< Parse PGE-X World file by file stream
    static WorldData dummyWldDataArray(); //!< Generate empty world map

    // SMBX64 WLD File
    static WorldData ReadSMBX64WldFileHeader(QString filePath); //!< Read file header only
    static WorldData ReadSMBX64WldFile(QString RawData, QString filePath, bool sielent=false); //!< Parse SMBX1-SMBX64 world
    static QString WriteSMBX64WldFile(WorldData FileData, int file_format=64);  //!< Generate SMBX1-SMBX64 world raw data

    // PGE Extended World map File
    static WorldData ReadExtendedWldFileHeader(QString filePath); //!< Read file header only
    static WorldData ReadExtendedWldFile(QString RawData, QString filePath, bool sielent=false); //!< Parse PGE-X world file
    static QString WriteExtendedWldFile(WorldData FileData);  //!< Generate PGE-X world raw data

    //Wld Data
    static WorldTiles dummyWldTile();
    static WorldScenery dummyWldScen();
    static WorldPaths dummyWldPath();
    static WorldLevels dummyWldLevel();
    static WorldMusic dummyWldMusic();

    /****************************Save of game file********************************/
    static GamesaveData ReadSMBX64SavFile(QString RawData, QString filePath);  //!< Parse SMBX1-SMBX64 game save
    static GamesaveData dummySaveDataArray();

    //Save Data
    static saveCharacterState dummySavCharacterState();


    /******************************NPC.txt file***********************************/
    // SMBX64 NPC.TXT File
    static NPCConfigFile ReadNpcTXTFile(QFile &inf, bool IgnoreBad=false); //read
    static QString WriteNPCTxtFile(NPCConfigFile FileData);                //write

    static NPCConfigFile CreateEmpytNpcTXTArray();
    #if defined(PGE_ENGINE)||defined(PGE_EDITOR)
    static obj_npc mergeNPCConfigs(obj_npc &global, NPCConfigFile &local, QSize captured=QSize(0,0));
    #endif

    //common
    static void BadFileMsg(QString fileName_DATA, int str_count, QString line);
    static QString removeQuotes(QString str); // Remove quotes from begin and end
};

#endif // FILE_FORMATS_H
