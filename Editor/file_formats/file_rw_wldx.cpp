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

#include <QFileInfo>
#include <QDir>

#include "file_formats.h"
#include "file_strlist.h"
#include "wld_filedata.h"
#include "pge_x.h"

#ifndef PGE_ENGINE
#include <QMessageBox>
#endif

//*********************************************************
//****************READ FILE FORMAT*************************
//*********************************************************

WorldData FileFormats::ReadExtendedWorldFile(QFile &inf)
{
    QTextStream in(&inf);   //Read File
    in.setCodec("UTF-8");

    return ReadExtendedWldFile( in.readAll(), inf.fileName() );
}

WorldData FileFormats::ReadExtendedWldFileHeader(QString filePath)
{
    WorldData FileData;
    FileData = dummyWldDataArray();

    QFile inf(filePath);
    if(!inf.open(QIODevice::ReadOnly))
    {
        FileData.ReadFileValid=false;
        return FileData;
    }
    QString line;
    int str_count=0;
    bool valid=false;
    QFileInfo in_1(filePath);
    FileData.filename = in_1.baseName();
    FileData.path = in_1.absoluteDir().absolutePath();
    QTextStream in(&inf);
    in.setCodec("UTF-8");

    //Find level header part
    do{
    str_count++;line = in.readLine();
    }while((line!="HEAD") && (!line.isNull()));

    QStringList header;
    str_count++;line = in.readLine();
    bool closed=false;
    while((line!="HEAD_END") && (!line.isNull()))
    {
        header.push_back(line);
        str_count++;line = in.readLine();
        if(line=="HEAD_END") closed=true;
    }
    if(!closed) goto badfile;

    foreach(QString header_line, header)
    {
        QList<QStringList >data = PGEFile::splitDataLine(header_line, &valid);

        for(int i=0;i<data.size();i++)
        {
            if(data[i].size()!=2) goto badfile;
            if(data[i][0]=="TL") //Episode Title
            {
             if(PGEFile::IsQStr(data[i][1]))
                 FileData.EpisodeTitle = PGEFile::X2STR(data[i][1]);
             else
                 goto badfile;
            }
            else
            if(data[i][0]=="DC") //Disabled characters
            {
             if(PGEFile::IsBoolArray(data[i][1]))
                 FileData.nocharacter = PGEFile::X2BollArr(data[i][1]);
             else
                 goto badfile;
            }
            else
            if(data[i][0]=="IT") //Intro level
            {
             if(PGEFile::IsQStr(data[i][1]))
                 FileData.IntroLevel_file = PGEFile::X2STR(data[i][1]);
             else
                 goto badfile;
            }
            else
            if(data[i][0]=="HB") //Hub Styled
            {
             if(PGEFile::IsBool(data[i][1]))
                 FileData.HubStyledWorld = (bool)data[i][1].toInt();
             else
                 goto badfile;
            }
            else
            if(data[i][0]=="RL") //Restart level on fail
            {
             if(PGEFile::IsBool(data[i][1]))
                 FileData.restartlevel = (bool)data[i][1].toInt();
             else
                 goto badfile;
            }
            else
            if(data[i][0]=="SZ") //Starz number
            {
             if(PGEFile::IsIntU(data[i][1]))
                 FileData.stars = data[i][1].toInt();
             else
                 goto badfile;
            }
            else
            if(data[i][0]=="CD") //Credits list
            {
             if(PGEFile::IsQStr(data[i][1]))
                 FileData.authors = PGEFile::X2STR(data[i][1]);
             else
                 goto badfile;
            }
        }
    }

    if(!closed)
        goto badfile;

    FileData.CurSection=0;
    FileData.playmusic=0;

    FileData.ReadFileValid=true;

    inf.close();
    return FileData;
badfile:
    inf.close();
    FileData.ReadFileValid=false;
    return FileData;
}

WorldData FileFormats::ReadExtendedWldFile(QString RawData, QString filePath, bool sielent)
{
     FileStringList in;
     in.addData( RawData );

     int str_count=0;        //Line Counter
     QString line;           //Current Line data

     WorldData FileData = dummyWldDataArray();

     //Add path data
     if(!filePath.isEmpty())
     {
         QFileInfo in_1(filePath);
         FileData.filename = in_1.baseName();
         FileData.path = in_1.absoluteDir().absolutePath();
     }

     FileData.untitled = false;
     FileData.modified = false;

     WorldTiles tile;
     WorldScenery scen;
     WorldPaths pathitem;
     WorldMusic musicbox;
     WorldLevels lvlitem;


     QString errorString;

     ///////////////////////////////////////Begin file///////////////////////////////////////
     PGEFile pgeX_Data(RawData);
     if( !pgeX_Data.buildTreeFromRaw() )
     {
         errorString = pgeX_Data.lastError();
         goto badfile;
     }

     for(int section=0; section<pgeX_Data.dataTree.size(); section++) //look sections
     {
         ///////////////////JOKES//////////////////////
         if(pgeX_Data.dataTree[section].name=="JOKES")
         {
             #ifndef PGE_ENGINE
             if(!pgeX_Data.dataTree[section].data.isEmpty())
                 if(!pgeX_Data.dataTree[section].data[0].values.isEmpty())
                     QMessageBox::information(nullptr, "Jokes",
                             pgeX_Data.dataTree[section].data[0].values[0].value,
                             QMessageBox::Ok);
             #endif
         }//jokes
         ///////////////////HEADER//////////////////////
         else
         if(pgeX_Data.dataTree[section].name=="HEAD")
         {
             if(pgeX_Data.dataTree[section].type!=PGEFile::PGEX_Struct)
             {
                 errorString=QString("Wrong section data syntax:\nSection [%1]").arg(pgeX_Data.dataTree[section].name);
                 goto badfile;
             }

             for(int sdata=0;sdata<pgeX_Data.dataTree[section].data.size();sdata++)
             {
                 if(pgeX_Data.dataTree[section].data[sdata].type!=PGEFile::PGEX_Struct)
                 {
                     errorString=QString("Wrong data item syntax:\nSection [%1]\nData line %2")
                             .arg(pgeX_Data.dataTree[section].name)
                             .arg(sdata);
                     goto badfile;
                 }

                 PGEFile::PGEX_Item x = pgeX_Data.dataTree[section].data[sdata];
                 for(int sval=0;sval<x.values.size();sval++) //Look markers and values
                 {
                     PGEFile::PGEX_Val v = x.values[sval];
                     errorString=QString("Wrong value syntax\nSection [%1]\nData line %2\nMarker %3\nValue %4")
                             .arg(pgeX_Data.dataTree[section].name)
                             .arg(sdata)
                             .arg(v.marker)
                             .arg(v.value);

                     if(v.marker=="TL") //Episode Title
                     {
                         if(PGEFile::IsQStr(v.value))
                             FileData.EpisodeTitle = PGEFile::X2STR(v.value);
                         else
                             goto badfile;
                     }
                     else
                     if(v.marker=="DC") //Disabled characters
                     {
                         if(PGEFile::IsBoolArray(v.value))
                             FileData.nocharacter = PGEFile::X2BollArr(v.value);
                         else
                             goto badfile;
                     }
                     else
                     if(v.marker=="IT") //Intro level
                     {
                         if(PGEFile::IsQStr(v.value))
                             FileData.IntroLevel_file = PGEFile::X2STR(v.value);
                         else
                             goto badfile;
                     }
                     else
                     if(v.marker=="HB") //Hub Styled
                     {
                         if(PGEFile::IsBool(v.value))
                             FileData.HubStyledWorld = (bool)v.value.toInt();
                         else
                             goto badfile;
                     }
                     else
                     if(v.marker=="RL") //Restart level on fail
                     {
                         if(PGEFile::IsBool(v.value))
                             FileData.restartlevel = (bool)v.value.toInt();
                         else
                             goto badfile;
                     }
                     else
                     if(v.marker=="SZ") //Starz number
                     {
                         if(PGEFile::IsIntU(v.value))
                             FileData.stars = v.value.toInt();
                         else
                             goto badfile;
                     }
                     else
                     if(v.marker=="CD") //Credits list
                     {
                         if(PGEFile::IsQStr(v.value))
                             FileData.authors = PGEFile::X2STR(v.value);
                         else
                             goto badfile;
                     }
                 }
             }
         }//head
         ///////////////////////////////MetaDATA/////////////////////////////////////////////
         else
         if(pgeX_Data.dataTree[section].name=="META_BOOKMARKS")
         {
             if(pgeX_Data.dataTree[section].type!=PGEFile::PGEX_Struct)
             {
                 errorString=QString("Wrong section data syntax:\nSection [%1]").arg(pgeX_Data.dataTree[section].name);
                 goto badfile;
             }

             for(int sdata=0;sdata<pgeX_Data.dataTree[section].data.size();sdata++)
             {
                 if(pgeX_Data.dataTree[section].data[sdata].type!=PGEFile::PGEX_Struct)
                 {
                     errorString=QString("Wrong data item syntax:\nSection [%1]\nData line %2")
                             .arg(pgeX_Data.dataTree[section].name)
                             .arg(sdata);
                     goto badfile;
                 }

                 PGEFile::PGEX_Item x = pgeX_Data.dataTree[section].data[sdata];

                 Bookmark meta_bookmark;
                 meta_bookmark.bookmarkName = "";
                 meta_bookmark.x = 0;
                 meta_bookmark.y = 0;

                 for(int sval=0;sval<x.values.size();sval++) //Look markers and values
                 {
                     PGEFile::PGEX_Val v = x.values[sval];
                     errorString=QString("Wrong value syntax\nSection [%1]\nData line %2\nMarker %3\nValue %4")
                             .arg(pgeX_Data.dataTree[section].name)
                             .arg(sdata)
                             .arg(v.marker)
                             .arg(v.value);

                       if(v.marker=="BM") //Bookmark name
                       {
                           if(PGEFile::IsQStr(v.value))
                               meta_bookmark.bookmarkName = PGEFile::X2STR(v.value);
                           else
                               goto badfile;
                       }
                       else
                       if(v.marker=="X") // Position X
                       {
                           if(PGEFile::IsIntS(v.value))
                               meta_bookmark.x = v.value.toInt();
                           else
                               goto badfile;
                       }
                       else
                       if(v.marker=="Y") //Position Y
                       {
                           if(PGEFile::IsIntS(v.value))
                               meta_bookmark.y = v.value.toInt();
                           else
                               goto badfile;
                       }
                 }
                 FileData.metaData.bookmarks.push_back(meta_bookmark);
             }
         }//meta bookmarks
         else
         if(pgeX_Data.dataTree[section].name=="META_SYS_CRASH")
         {
             if(pgeX_Data.dataTree[section].type!=PGEFile::PGEX_Struct)
             {
                 errorString=QString("Wrong section data syntax:\nSection [%1]").arg(pgeX_Data.dataTree[section].name);
                 goto badfile;
             }

             for(int sdata=0;sdata<pgeX_Data.dataTree[section].data.size();sdata++)
             {
                 if(pgeX_Data.dataTree[section].data[sdata].type!=PGEFile::PGEX_Struct)
                 {
                     errorString=QString("Wrong data item syntax:\nSection [%1]\nData line %2")
                             .arg(pgeX_Data.dataTree[section].name)
                             .arg(sdata);
                     goto badfile;
                 }

                 PGEFile::PGEX_Item x = pgeX_Data.dataTree[section].data[sdata];

                 for(int sval=0;sval<x.values.size();sval++) //Look markers and values
                 {
                     PGEFile::PGEX_Val v = x.values[sval];
                     errorString=QString("Wrong value syntax\nSection [%1]\nData line %2\nMarker %3\nValue %4")
                             .arg(pgeX_Data.dataTree[section].name)
                             .arg(sdata)
                             .arg(v.marker)
                             .arg(v.value);

                       FileData.metaData.crash.used=true;

                       if(v.marker=="UT") //Untitled
                       {
                           if(PGEFile::IsBool(v.value))
                               FileData.metaData.crash.untitled = (bool)v.value.toInt();
                           else
                               goto badfile;
                       }
                       else
                       if(v.marker=="MD") //Modyfied
                       {
                           if(PGEFile::IsBool(v.value))
                               FileData.metaData.crash.modifyed = (bool)v.value.toInt();
                           else
                               goto badfile;
                       }
                       else
                       if(v.marker=="N") //Filename
                       {
                           if(PGEFile::IsQStr(v.value))
                               FileData.metaData.crash.filename = PGEFile::X2STR(v.value);
                           else
                               goto badfile;
                       }
                       else
                       if(v.marker=="P") //Path
                       {
                           if(PGEFile::IsQStr(v.value))
                               FileData.metaData.crash.path = PGEFile::X2STR(v.value);
                           else
                               goto badfile;
                       }
                       else
                       if(v.marker=="FP") //Full file Path
                       {
                           if(PGEFile::IsQStr(v.value))
                               FileData.metaData.crash.fullPath = PGEFile::X2STR(v.value);
                           else
                               goto badfile;
                       }
                 }
             }
         }//meta sys crash
         ///////////////////////////////MetaDATA//End////////////////////////////////////////
         else ///////////////////TILES//////////////////////
         if(pgeX_Data.dataTree[section].name=="TILES")
         {
             if(pgeX_Data.dataTree[section].type!=PGEFile::PGEX_Struct)
             {
                 errorString=QString("Wrong section data syntax:\nSection [%1]").arg(pgeX_Data.dataTree[section].name);
                 goto badfile;
             }

             for(int sdata=0;sdata<pgeX_Data.dataTree[section].data.size();sdata++)
             {
                 if(pgeX_Data.dataTree[section].data[sdata].type!=PGEFile::PGEX_Struct)
                 {
                     errorString=QString("Wrong data item syntax:\nSection [%1]\nData line %2")
                             .arg(pgeX_Data.dataTree[section].name)
                             .arg(sdata);
                     goto badfile;
                 }
                 PGEFile::PGEX_Item x = pgeX_Data.dataTree[section].data[sdata];

                 tile = dummyWldTile();
                 for(int sval=0;sval<x.values.size();sval++) //Look markers and values
                 {
                     PGEFile::PGEX_Val v = x.values[sval];
                     errorString=QString("Wrong value syntax\nSection [%1]\nData line %2\nMarker %3\nValue %4")
                             .arg(pgeX_Data.dataTree[section].name)
                             .arg(sdata)
                             .arg(v.marker)
                             .arg(v.value);

                     if(v.marker=="ID") //Tile ID
                     {
                         if(PGEFile::IsIntU(v.value))
                             tile.id = v.value.toInt();
                         else
                             goto badfile;
                     }
                     else
                     if(v.marker=="X") //X Position
                     {
                         if(PGEFile::IsIntS(v.value))
                             tile.x = v.value.toInt();
                         else
                             goto badfile;
                     }
                     else
                     if(v.marker=="Y") //Y Position
                     {
                         if(PGEFile::IsIntS(v.value))
                             tile.y = v.value.toInt();
                         else
                             goto badfile;
                     }
                 }
                 tile.array_id = FileData.tile_array_id++;
                 tile.index = FileData.tiles.size();
                 FileData.tiles.push_back(tile);
             }
         }//TILES
         else ///////////////////SCENERY//////////////////////
         if(pgeX_Data.dataTree[section].name=="SCENERY")
         {
             if(pgeX_Data.dataTree[section].type!=PGEFile::PGEX_Struct)
             {
                 errorString=QString("Wrong section data syntax:\nSection [%1]").arg(pgeX_Data.dataTree[section].name);
                 goto badfile;
             }

             for(int sdata=0;sdata<pgeX_Data.dataTree[section].data.size();sdata++)
             {
                 if(pgeX_Data.dataTree[section].data[sdata].type!=PGEFile::PGEX_Struct)
                 {
                     errorString=QString("Wrong data item syntax:\nSection [%1]\nData line %2")
                             .arg(pgeX_Data.dataTree[section].name)
                             .arg(sdata);
                     goto badfile;
                 }
                 PGEFile::PGEX_Item x = pgeX_Data.dataTree[section].data[sdata];
                 scen = dummyWldScen();

                 for(int sval=0;sval<x.values.size();sval++) //Look markers and values
                 {
                     PGEFile::PGEX_Val v = x.values[sval];
                     errorString=QString("Wrong value syntax\nSection [%1]\nData line %2\nMarker %3\nValue %4")
                             .arg(pgeX_Data.dataTree[section].name)
                             .arg(sdata)
                             .arg(v.marker)
                             .arg(v.value);

                     if(v.marker=="ID") //Scenery ID
                     {
                         if(PGEFile::IsIntU(v.value))
                             scen.id = v.value.toInt();
                         else
                             goto badfile;
                     }
                     else
                     if(v.marker=="X") //X Position
                     {
                         if(PGEFile::IsIntS(v.value))
                             scen.x = v.value.toInt();
                         else
                             goto badfile;
                     }
                     else
                     if(v.marker=="Y") //Y Position
                     {
                         if(PGEFile::IsIntS(v.value))
                             scen.y = v.value.toInt();
                         else
                             goto badfile;
                     }
                 }
                 scen.array_id = FileData.scene_array_id++;
                 scen.index = FileData.scenery.size();
                 FileData.scenery.push_back(scen);
             }
         }//SCENERY
         else ///////////////////TILES//////////////////////
         if(pgeX_Data.dataTree[section].name=="PATHS")
         {
             if(pgeX_Data.dataTree[section].type!=PGEFile::PGEX_Struct)
             {
                 errorString=QString("Wrong section data syntax:\nSection [%1]").arg(pgeX_Data.dataTree[section].name);
                 goto badfile;
             }

             for(int sdata=0;sdata<pgeX_Data.dataTree[section].data.size();sdata++)
             {
                 if(pgeX_Data.dataTree[section].data[sdata].type!=PGEFile::PGEX_Struct)
                 {
                     errorString=QString("Wrong data item syntax:\nSection [%1]\nData line %2")
                             .arg(pgeX_Data.dataTree[section].name)
                             .arg(sdata);
                     goto badfile;
                 }
                 PGEFile::PGEX_Item x = pgeX_Data.dataTree[section].data[sdata];
                 pathitem = dummyWldPath();

                 for(int sval=0;sval<x.values.size();sval++) //Look markers and values
                 {
                     PGEFile::PGEX_Val v = x.values[sval];
                     errorString=QString("Wrong value syntax\nSection [%1]\nData line %2\nMarker %3\nValue %4")
                             .arg(pgeX_Data.dataTree[section].name)
                             .arg(sdata)
                             .arg(v.marker)
                             .arg(v.value);

                     if(v.marker=="ID") //PATHS ID
                     {
                         if(PGEFile::IsIntU(v.value))
                             pathitem.id = v.value.toInt();
                         else
                             goto badfile;
                     }
                     else
                     if(v.marker=="X") //X Position
                     {
                         if(PGEFile::IsIntS(v.value))
                             pathitem.x = v.value.toInt();
                         else
                             goto badfile;
                     }
                     else
                     if(v.marker=="Y") //Y Position
                     {
                         if(PGEFile::IsIntS(v.value))
                             pathitem.y = v.value.toInt();
                         else
                             goto badfile;
                     }
                 }
                 pathitem.array_id = FileData.path_array_id++;
                 pathitem.index = FileData.paths.size();
                 FileData.paths.push_back(pathitem);
             }
         }//PATHS
         else ///////////////////MUSICBOXES//////////////////////
         if(pgeX_Data.dataTree[section].name=="MUSICBOXES")
         {
             if(pgeX_Data.dataTree[section].type!=PGEFile::PGEX_Struct)
             {
                 errorString=QString("Wrong section data syntax:\nSection [%1]").arg(pgeX_Data.dataTree[section].name);
                 goto badfile;
             }

             for(int sdata=0;sdata<pgeX_Data.dataTree[section].data.size();sdata++)
             {
                 if(pgeX_Data.dataTree[section].data[sdata].type!=PGEFile::PGEX_Struct)
                 {
                     errorString=QString("Wrong data item syntax:\nSection [%1]\nData line %2")
                             .arg(pgeX_Data.dataTree[section].name)
                             .arg(sdata);
                     goto badfile;
                 }
                 PGEFile::PGEX_Item x = pgeX_Data.dataTree[section].data[sdata];
                 musicbox = dummyWldMusic();

                 for(int sval=0;sval<x.values.size();sval++) //Look markers and values
                 {
                     PGEFile::PGEX_Val v = x.values[sval];
                     errorString=QString("Wrong value syntax\nSection [%1]\nData line %2\nMarker %3\nValue %4")
                             .arg(pgeX_Data.dataTree[section].name)
                             .arg(sdata)
                             .arg(v.marker)
                             .arg(v.value);

                     if(v.marker=="ID") //MISICBOX ID
                     {
                         if(PGEFile::IsIntU(v.value))
                             musicbox.id = v.value.toInt();
                         else
                             goto badfile;
                     }
                     else
                     if(v.marker=="X") //X Position
                     {
                         if(PGEFile::IsIntS(v.value))
                             musicbox.x = v.value.toInt();
                         else
                             goto badfile;
                     }
                     else
                     if(v.marker=="Y") //Y Position
                     {
                         if(PGEFile::IsIntS(v.value))
                             musicbox.y = v.value.toInt();
                         else
                             goto badfile;
                     }
                     else
                     if(v.marker=="MF") //Custom music file
                     {
                         if(PGEFile::IsQStr(v.value))
                             musicbox.music_file = PGEFile::X2STR(v.value);
                         else
                             goto badfile;
                     }
                 }
                 musicbox.array_id = FileData.musicbox_array_id++;
                 musicbox.index = FileData.music.size();
                 FileData.music.push_back(musicbox);
             }
         }//MUSICBOXES

         else ///////////////////LEVELS//////////////////////
         if(pgeX_Data.dataTree[section].name=="LEVELS")
         {
             if(pgeX_Data.dataTree[section].type!=PGEFile::PGEX_Struct)
             {
                 errorString=QString("Wrong section data syntax:\nSection [%1]").arg(pgeX_Data.dataTree[section].name);
                 goto badfile;
             }

             for(int sdata=0;sdata<pgeX_Data.dataTree[section].data.size();sdata++)
             {
                 if(pgeX_Data.dataTree[section].data[sdata].type!=PGEFile::PGEX_Struct)
                 {
                     errorString=QString("Wrong data item syntax:\nSection [%1]\nData line %2")
                             .arg(pgeX_Data.dataTree[section].name)
                             .arg(sdata);
                     goto badfile;
                 }
                 PGEFile::PGEX_Item x = pgeX_Data.dataTree[section].data[sdata];
                 lvlitem = dummyWldLevel();

                 for(int sval=0;sval<x.values.size();sval++) //Look markers and values
                 {
                     PGEFile::PGEX_Val v = x.values[sval];
                     errorString=QString("Wrong value syntax\nSection [%1]\nData line %2\nMarker %3\nValue %4")
                             .arg(pgeX_Data.dataTree[section].name)
                             .arg(sdata)
                             .arg(v.marker)
                             .arg(v.value);

                     if(v.marker=="ID") //LEVEL IMAGE ID
                     {
                         if(PGEFile::IsIntU(v.value))
                             lvlitem.id = v.value.toInt();
                         else
                             goto badfile;
                     }
                     else
                     if(v.marker=="X") //X Position
                     {
                         if(PGEFile::IsIntS(v.value))
                             lvlitem.x = v.value.toInt();
                         else
                             goto badfile;
                     }
                     else
                     if(v.marker=="Y") //Y Position
                     {
                         if(PGEFile::IsIntS(v.value))
                             lvlitem.y = v.value.toInt();
                         else
                             goto badfile;
                     }
                     else
                     if(v.marker=="LF") //Target level file
                     {
                         if(PGEFile::IsQStr(v.value))
                             lvlitem.lvlfile = PGEFile::X2STR(v.value);
                         else
                             goto badfile;
                     }
                     else
                     if(v.marker=="LT") //Level title
                     {
                         if(PGEFile::IsQStr(v.value))
                             lvlitem.title = PGEFile::X2STR(v.value);
                         else
                             goto badfile;
                     }
                     else
                     if(v.marker=="EI") //Entrance Warp ID (if 0 - start level from default points)
                     {
                         if(PGEFile::IsIntU(v.value))
                             lvlitem.title = v.value.toInt();
                         else
                             goto badfile;
                     }
                     else
                     if(v.marker=="ET") //Open top path on exit type
                     {
                         if(PGEFile::IsIntS(v.value))
                             lvlitem.top_exit = v.value.toInt();
                         else
                             goto badfile;
                     }
                     else
                     if(v.marker=="EL") //Open left path on exit type
                     {
                         if(PGEFile::IsIntS(v.value))
                             lvlitem.left_exit = v.value.toInt();
                         else
                             goto badfile;
                     }
                     else
                     if(v.marker=="ER") //Open right path on exit type
                     {
                         if(PGEFile::IsIntS(v.value))
                             lvlitem.right_exit = v.value.toInt();
                         else
                             goto badfile;
                     }
                     else
                     if(v.marker=="EB") //Open bottom path on exit type
                     {
                         if(PGEFile::IsIntS(v.value))
                             lvlitem.bottom_exit = v.value.toInt();
                         else
                             goto badfile;
                     }
                     else
                     if(v.marker=="WX") //Goto world map X
                     {
                         if(PGEFile::IsIntS(v.value))
                             lvlitem.gotox = v.value.toInt();
                         else
                             goto badfile;
                     }
                     else
                     if(v.marker=="WY") //Goto world map Y
                     {
                         if(PGEFile::IsIntS(v.value))
                             lvlitem.gotoy = v.value.toInt();
                         else
                             goto badfile;
                     }
                     else
                     if(v.marker=="AV") //Always visible
                     {
                         if(PGEFile::IsBool(v.value))
                             lvlitem.alwaysVisible = (bool)v.value.toInt();
                         else
                             goto badfile;
                     }
                     else
                     if(v.marker=="SP") //Is Game start point
                     {
                         if(PGEFile::IsBool(v.value))
                             lvlitem.gamestart = (bool)v.value.toInt();
                         else
                             goto badfile;
                     }
                     else
                     if(v.marker=="BP") //Path background
                     {
                         if(PGEFile::IsBool(v.value))
                             lvlitem.pathbg = (bool)v.value.toInt();
                         else
                             goto badfile;
                     }
                     else
                     if(v.marker=="BG") //Big path background
                     {
                         if(PGEFile::IsBool(v.value))
                             lvlitem.bigpathbg = (bool)v.value.toInt();
                         else
                             goto badfile;
                     }
                 }
                 lvlitem.array_id = FileData.level_array_id++;
                 lvlitem.index = FileData.levels.size();
                 FileData.levels.push_back(lvlitem);
             }
         }//LEVELS

     }
     ///////////////////////////////////////EndFile///////////////////////////////////////

     errorString.clear(); //If no errors, clear string;
     FileData.ReadFileValid=true;

     return FileData;

     badfile:    //If file format not corrects
         if(!sielent)
            BadFileMsg(FileData.path, str_count, line);
         FileData.ReadFileValid=false;
     return FileData;
}



//*********************************************************
//****************WRITE FILE FORMAT************************
//*********************************************************

QString FileFormats::WriteExtendedWldFile(WorldData FileData)
{
    QString TextData;
    long i;
    bool addArray=false;

    addArray=false;
    foreach(bool x, FileData.nocharacter)
    { if(x) addArray=true; }
    //HEAD section
    if(
            (!FileData.EpisodeTitle.isEmpty())||
            (addArray)||
            (!FileData.IntroLevel_file.isEmpty())||
            (FileData.HubStyledWorld)||
            (FileData.restartlevel)||
            (FileData.stars>0)||
            (!FileData.authors.isEmpty())
      )
    {
        TextData += "HEAD\n";
            if(!FileData.EpisodeTitle.isEmpty())
                TextData += PGEFile::value("TL", PGEFile::qStrS(FileData.EpisodeTitle)); // Episode title

            addArray=false;
            foreach(bool x, FileData.nocharacter)
            { if(x) addArray=true; }
            if(addArray)
                TextData += PGEFile::value("DC", PGEFile::BoolArrayS(FileData.nocharacter)); // Disabled characters

            if(!FileData.IntroLevel_file.isEmpty())
                TextData += PGEFile::value("IT", PGEFile::qStrS(FileData.IntroLevel_file)); // Intro level

            if(FileData.HubStyledWorld)
                TextData += PGEFile::value("HB", PGEFile::BoolS(FileData.HubStyledWorld)); // Hub-styled episode

            if(FileData.restartlevel)
                TextData += PGEFile::value("RL", PGEFile::BoolS(FileData.restartlevel)); // Restart on fail
            if(FileData.stars>0)
                TextData += PGEFile::value("SZ", PGEFile::IntS(FileData.stars));      // Total stars number
            if(!FileData.authors.isEmpty())
                TextData += PGEFile::value("CD", PGEFile::qStrS( FileData.authors )); // Credits

        TextData += "\n";
        TextData += "HEAD_END\n";
    }

    //////////////////////////////////////MetaData////////////////////////////////////////////////
    //Bookmarks
    if(!FileData.metaData.bookmarks.isEmpty())
    {
        TextData += "META_BOOKMARKS\n";
        for(i=0;i<FileData.metaData.bookmarks.size(); i++)
        {
            //Bookmark name
            TextData += PGEFile::value("BM", PGEFile::qStrS(FileData.metaData.bookmarks[i].bookmarkName));
            TextData += PGEFile::value("X", PGEFile::IntS(FileData.metaData.bookmarks[i].x));
            TextData += PGEFile::value("Y", PGEFile::IntS(FileData.metaData.bookmarks[i].y));
            TextData += "\n";
        }
        TextData += "META_BOOKMARKS_END\n";
    }

    //Some System information
    if(!FileData.metaData.crash.used)
    {
        TextData += "META_SYS_CRASH\n";
            TextData += PGEFile::value("UT", PGEFile::BoolS(FileData.metaData.crash.untitled));
            TextData += PGEFile::value("MD", PGEFile::BoolS(FileData.metaData.crash.modifyed));
            TextData += PGEFile::value("N", PGEFile::qStrS(FileData.metaData.crash.filename));
            TextData += PGEFile::value("P", PGEFile::qStrS(FileData.metaData.crash.path));
            TextData += PGEFile::value("FP", PGEFile::qStrS(FileData.metaData.crash.fullPath));
            TextData += "\n";
        TextData += "META_SYS_CRASH_END\n";
    }
    //////////////////////////////////////MetaData///END//////////////////////////////////////////

    if(!FileData.tiles.isEmpty())
    {
        TextData += "TILES\n";

        for(i=0; i<FileData.tiles.size();i++)
        {
            TextData += PGEFile::value("ID", PGEFile::IntS(FileData.tiles[i].id ));
            TextData += PGEFile::value("X", PGEFile::IntS(FileData.tiles[i].x ));
            TextData += PGEFile::value("Y", PGEFile::IntS(FileData.tiles[i].y ));
            TextData += "\n";
        }

        TextData += "TILES_END\n";
    }

    if(!FileData.scenery.isEmpty())
    {
        TextData += "SCENERY\n";

        for(i=0; i<FileData.scenery.size();i++)
        {
            TextData += PGEFile::value("ID", PGEFile::IntS(FileData.scenery[i].id ));
            TextData += PGEFile::value("X", PGEFile::IntS(FileData.scenery[i].x ));
            TextData += PGEFile::value("Y", PGEFile::IntS(FileData.scenery[i].y ));
            TextData += "\n";
        }

        TextData += "SCENERY_END\n";
    }

    if(!FileData.paths.isEmpty())
    {
        TextData += "PATHS\n";

        for(i=0; i<FileData.paths.size();i++)
        {
            TextData += PGEFile::value("ID", PGEFile::IntS(FileData.paths[i].id ));
            TextData += PGEFile::value("X", PGEFile::IntS(FileData.paths[i].x ));
            TextData += PGEFile::value("Y", PGEFile::IntS(FileData.paths[i].y ));
            TextData += "\n";
        }

        TextData += "PATHS_END\n";
    }

    if(!FileData.music.isEmpty())
    {
        TextData += "MUSICBOXES\n";

        for(i=0; i<FileData.music.size();i++)
        {
            TextData += PGEFile::value("ID", PGEFile::IntS(FileData.music[i].id ));
            TextData += PGEFile::value("X", PGEFile::IntS(FileData.music[i].x ));
            TextData += PGEFile::value("Y", PGEFile::IntS(FileData.music[i].y ));
            if(!FileData.music[i].music_file.isEmpty())
                TextData += PGEFile::value("MF", PGEFile::qStrS(FileData.music[i].music_file ));
            TextData += "\n";
        }

        TextData += "MUSICBOXES_END\n";
    }


    if(!FileData.levels.isEmpty())
    {
        TextData += "LEVELS\n";

        WorldLevels defLvl = dummyWldLevel();
        for(i=0; i<FileData.levels.size();i++)
        {
            TextData += PGEFile::value("ID", PGEFile::IntS(FileData.levels[i].id ));
            TextData += PGEFile::value("X", PGEFile::IntS(FileData.levels[i].x ));
            TextData += PGEFile::value("Y", PGEFile::IntS(FileData.levels[i].y ));
            if(!FileData.levels[i].title.isEmpty())
                TextData += PGEFile::value("LT", PGEFile::qStrS(FileData.levels[i].title ));
            if(!FileData.levels[i].lvlfile.isEmpty())
                TextData += PGEFile::value("LF", PGEFile::qStrS(FileData.levels[i].lvlfile ));
            if(FileData.levels[i].entertowarp!=defLvl.entertowarp)
                TextData += PGEFile::value("EI", PGEFile::IntS(FileData.levels[i].entertowarp ));
            if(FileData.levels[i].left_exit!=defLvl.left_exit)
                TextData += PGEFile::value("EL", PGEFile::IntS(FileData.levels[i].left_exit ));
            if(FileData.levels[i].top_exit!=defLvl.top_exit)
                TextData += PGEFile::value("ET", PGEFile::IntS(FileData.levels[i].top_exit ));
            if(FileData.levels[i].right_exit!=defLvl.right_exit)
                TextData += PGEFile::value("ER", PGEFile::IntS(FileData.levels[i].right_exit ));
            if(FileData.levels[i].bottom_exit!=defLvl.bottom_exit)
                TextData += PGEFile::value("EB", PGEFile::IntS(FileData.levels[i].bottom_exit ));
            if(FileData.levels[i].gotox!=defLvl.gotox)
                TextData += PGEFile::value("WX", PGEFile::IntS(FileData.levels[i].gotox ));
            if(FileData.levels[i].gotoy!=defLvl.gotoy)
                TextData += PGEFile::value("WY", PGEFile::IntS(FileData.levels[i].gotoy ));
            if(FileData.levels[i].alwaysVisible)
                TextData += PGEFile::value("AV", PGEFile::BoolS(FileData.levels[i].alwaysVisible ));
            if(FileData.levels[i].gamestart)
                TextData += PGEFile::value("SP", PGEFile::BoolS(FileData.levels[i].gamestart ));
            if(FileData.levels[i].pathbg)
                TextData += PGEFile::value("BP", PGEFile::BoolS(FileData.levels[i].pathbg ));
            if(FileData.levels[i].bigpathbg)
                TextData += PGEFile::value("BG", PGEFile::BoolS(FileData.levels[i].bigpathbg ));
            TextData += "\n";
        }

        TextData += "LEVELS_END\n";
    }



    return TextData;
}
