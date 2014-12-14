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

#ifndef FILE_FORMATS_H
#define FILE_FORMATS_H

#include <QRegExp>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#ifndef PGE_ENGINE
#include <QMessageBox>
#endif

#include "lvl_filedata.h"
#include "npc_filedata.h"
#include "wld_filedata.h"

#include "../data_configs/obj_npc.h"

//SMBX64 standard data
class SMBX64
{
public:
    SMBX64() {}

    // /////////////Validators///////////////
    //returns TRUE on wrong data
    static bool Int(QString in); // UNSIGNED INT
    static bool sInt(QString in); // SIGNED INT
    static bool sFloat(QString in); // SIGNED FLOAT
    static bool qStr(QString in); // QUOTED STRING
    static bool wBool(QString in); //Worded BOOL
    static bool dBool(QString in); //Worded BOOL

    //Convert from string to internal data
    static bool wBoolR(QString in);

    //SMBX64 parameter string generators
    static QString IntS(long input);
    static QString BoolS(bool input);
    static QString qStrS(QString input);
    static QString FloatS(float input);
    static QString qStrS_multiline(QString input);
};


typedef QPair<QString, QStringList> PGEXSct;

class PGEFile
{
public:
    enum PGEX_Item_type
    {
        PGEX_Struct=0,
        PGEX_PlainText
    };


    struct PGEX_Val
    {
        QString marker;
        QString value;
    };

    struct PGEX_Item
    {
        PGEX_Item_type type;
        QList<PGEX_Val > values;
    };

    struct PGEX_Entry
    {
        QString name;
        PGEX_Item_type type;
        QList<PGEX_Item > data;
        QList<PGEX_Entry > subTree;
    };

    PGEFile();
    PGEFile(PGEFile &pgeFile);
    PGEFile(QString _rawData);
    void setRawData(QString _rawData);
    bool buildTreeFromRaw();
    QString lastError();

    //data tree
    QList<PGEX_Entry > dataTree;

private:
    QString _lastError;
    QString rawData;
    QList<PGEXSct > rawDataTree;

    //Static functions
public:
    // /////////////Validators///////////////
    static bool IsSectionTitle(QString in);//Section Title

    static PGEX_Entry buildTree(QStringList &src_data, bool *_valid = 0);

    //returns FALSE on valid data
    static bool IsQStr(QString in);// QUOTED STRING
    static bool IsHex(QString in);// Hex Encoded String
    static bool IsIntU(QString in);// UNSIGNED INT
    static bool IsIntS(QString in);// SIGNED INT
    static bool IsFloat(QString in);// FLOAT
    static bool IsBool(QString in);//BOOL
    static bool IsBoolArray(QString in);//Boolean array
    static bool IsIntArray(QString in);//Integer array
    static bool IsStringArray(QString in);//String array

    //Split string into data values
    static QList<QStringList> splitDataLine(QString src_data, bool *valid = 0);

    //PGE Extended File parameter string generators
    static QString IntS(long input);
    static QString BoolS(bool input);
    static QString FloatS(double input);
    static QString qStrS(QString input);
    static QString hStrS(QString input);
    static QString strArrayS(QStringList input);
    static QString intArrayS(QList<int > input);
    static QString BoolArrayS(QList<bool > input);

    static QString X2STR(QString input);
    static QStringList X2STRArr(QString src);
    static QList<bool> X2BollArr(QString src);

    static QString escapeStr(QString input);
    static QString restoreStr(QString input);
    static QString encodeEscape(QString input);
    static QString decodeEscape(QString input);

    static QString value(QString marker, QString data);
};


class FileStringList
{
public:
    FileStringList();

    FileStringList(QString fileData);
    ~FileStringList();

    void addData(QString fileData);
    QString readLine();
    bool isEOF();
    bool atEnd();
private:
    QStringList buffer;
    long lineID;
};


class FileFormats
{
public:
    //File format read/write functions


    static MetaData ReadNonSMBX64MetaData(QString RawData, QString filePath="");
    static QString WriteNonSMBX64MetaData(MetaData metaData);

    /******************************Level files***********************************/
    static LevelData OpenLevelFile(QString filePath); //!< Open supported level file via direct path

    static LevelData ReadLevelFile(QFile &inf); //!< Parse SMBX64 Level file by file stream
    static LevelData ReadExtendedLevelFile(QFile &inf); //!< Parse PGE-X level file by file stream
    static LevelData dummyLvlDataArray(); //!< Generate empty level map

    // SMBX64 LVL File
    static LevelData ReadSMBX64LvlFile(QString RawData, QString filePath=""); //!< Parse SMBX1-SMBX64 level
    static QString WriteSMBX64LvlFile(LevelData FileData, int file_format=64);  //!< Generate SMBX1-SMBX64 level raw data

    // PGE Extended Level File
    static LevelData ReadExtendedLvlFile(QString RawData, QString filePath=""); //!< Parse PGE-X level file
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

    static WorldData ReadWorldFile(QFile &inf); //!< Parse SMBX64 World file by file stream
    static WorldData ReadExtendedWorldFile(QFile &inf); //!< Parse PGE-X World file by file stream
    static WorldData dummyWldDataArray(); //!< Generate empty world map

    // SMBX64 WLD File
    static WorldData ReadSMBX64WldFile(QString RawData, QString filePath); //!< Parse SMBX1-SMBX64 world
    static QString WriteSMBX64WldFile(WorldData FileData, int file_format=64);  //!< Generate SMBX1-SMBX64 world raw data

    // PGE Extended World map File
    static WorldData ReadExtendedWldFile(QString RawData, QString filePath); //!< Parse PGE-X world file
    static QString WriteExtendedWldFile(WorldData FileData);  //!< Generate PGE-X world raw data

    //Wld Data
    static WorldTiles dummyWldTile();
    static WorldScenery dummyWldScen();
    static WorldPaths dummyWldPath();
    static WorldLevels dummyWldLevel();
    static WorldMusic dummyWldMusic();


    /******************************NPC.txt file***********************************/
    // SMBX64 NPC.TXT File
    static NPCConfigFile ReadNpcTXTFile(QFile &inf, bool IgnoreBad=false); //read
    static QString WriteNPCTxtFile(NPCConfigFile FileData);                //write

    static NPCConfigFile CreateEmpytNpcTXTArray();
    static obj_npc mergeNPCConfigs(obj_npc &global, NPCConfigFile &local, QSize captured=QSize(0,0));


    //common
    static void BadFileMsg(QString fileName_DATA, int str_count, QString line);
    static QString removeQuotes(QString str); // Remove quotes from begin and end
};

#endif // FILE_FORMATS_H
