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
#include "pge_x_macro.h"

#ifdef PGE_FILES_USE_MESSAGEBOXES
#include <QMessageBox>
#endif


//*********************************************************
//****************READ FILE FORMAT*************************
//*********************************************************

WorldData FileFormats::ReadExtendedWorldFile(PGEFILE &inf)
{
    QTextStream in(&inf);   //Read File
    in.setCodec("UTF-8");

    return ReadExtendedWldFile( in.readAll(), inf.fileName() );
}

WorldData FileFormats::ReadExtendedWldFileHeader(PGESTRING filePath)
{
    errorString.clear();
    WorldData FileData;
    FileData = dummyWldDataArray();

    QFile inf(filePath);
    if(!inf.open(QIODevice::ReadOnly))
    {
        FileData.ReadFileValid=false;
        return FileData;
    }
    PGESTRING line;
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

    PGESTRINGList header;
    str_count++;line = in.readLine();
    bool closed=false;
    while((line!="HEAD_END") && (!line.isNull()))
    {
        header.push_back(line);
        str_count++;line = in.readLine();
        if(line=="HEAD_END") closed=true;
    }
    if(!closed) goto badfile;

    foreach(PGESTRING header_line, header)
    {
        QList<PGESTRINGList >data = PGEFile::splitDataLine(header_line, &valid);

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
                 FileData.HubStyledWorld = (bool)toInt(data[i][1]);
             else
                 goto badfile;
            }
            else
            if(data[i][0]=="RL") //Restart level on fail
            {
             if(PGEFile::IsBool(data[i][1]))
                 FileData.restartlevel = (bool)toInt(data[i][1]);
             else
                 goto badfile;
            }
            else
            if(data[i][0]=="SZ") //Starz number
            {
             if(PGEFile::IsIntU(data[i][1]))
                 FileData.stars = toInt(data[i][1]);
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

WorldData FileFormats::ReadExtendedWldFile(PGESTRING RawData, PGESTRING filePath, bool sielent)
{
     errorString.clear();

     PGEX_FileBegin();

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

     ///////////////////////////////////////Begin file///////////////////////////////////////
     PGEX_FileParseTree(RawData);

     PGEX_FetchSection() //look sections
     {
         PGEX_FetchSection_begin()

         ///////////////////JOKES//////////////////////
         PGEX_Section("JOKES")
         {
             #ifdef PGE_FILES_USE_MESSAGEBOXES
             if((!silentMode)&&(!f_section.data.isEmpty()))
                 if(!f_section.data[0].values.isEmpty())
                     QMessageBox::information(nullptr, "Jokes",
                             f_section.data[0].values[0].value,
                             QMessageBox::Ok);
             #endif
         }//jokes


         ///////////////////HEADER//////////////////////
         PGEX_Section("HEAD")
         {
             PGEX_SectionBegin(PGEFile::PGEX_Struct);
             PGEX_Items()
             {
                 PGEX_ItemBegin(PGEFile::PGEX_Struct);
                 PGEX_Values() //Look markers and values
                 {
                     PGEX_ValueBegin()
                     PGEX_StrVal    ("TL", FileData.EpisodeTitle)    //Episode Title
                     PGEX_BoolArrVal("DC", FileData.nocharacter)     //Disabled characters
                     PGEX_StrVal    ("IT", FileData.IntroLevel_file) //Intro level
                     PGEX_BoolVal   ("HB", FileData.HubStyledWorld)  //Hub Styled
                     PGEX_BoolVal   ("RL", FileData.restartlevel)    //Restart level on fail
                     PGEX_UIntVal   ("SZ", FileData.stars)           //Starz number
                     PGEX_StrVal    ("CD", FileData.authors) //Credits list
                 }
             }
         }//head


         ///////////////////////////////MetaDATA/////////////////////////////////////////////
         PGEX_Section("META_BOOKMARKS")
         {
             PGEX_SectionBegin(PGEFile::PGEX_Struct);

             PGEX_Items()
             {
                 PGEX_ItemBegin(PGEFile::PGEX_Struct);

                 Bookmark meta_bookmark;
                 meta_bookmark.bookmarkName = "";
                 meta_bookmark.x = 0;
                 meta_bookmark.y = 0;

                 PGEX_Values() //Look markers and values
                 {
                     PGEX_ValueBegin()
                     PGEX_StrVal("BM", meta_bookmark.bookmarkName) //Bookmark name
                     PGEX_SIntVal("X", meta_bookmark.x) // Position X
                     PGEX_SIntVal("Y", meta_bookmark.y) // Position Y
                 }
                 FileData.metaData.bookmarks.push_back(meta_bookmark);
             }
         }

         ////////////////////////meta bookmarks////////////////////////
         PGEX_Section("META_SYS_CRASH")
         {
             PGEX_SectionBegin(PGEFile::PGEX_Struct);

             PGEX_Items()
             {
                 PGEX_ItemBegin(PGEFile::PGEX_Struct);

                 PGEX_Values() //Look markers and values
                 {
                     FileData.metaData.crash.used=true;

                     PGEX_ValueBegin()
                     PGEX_BoolVal("UT", FileData.metaData.crash.untitled) //Untitled
                     PGEX_BoolVal("MD", FileData.metaData.crash.modifyed) //Modyfied
                     PGEX_StrVal ("N",  FileData.metaData.crash.filename) //Filename
                     PGEX_StrVal ("P",  FileData.metaData.crash.path) //Path
                     PGEX_StrVal ("FP", FileData.metaData.crash.fullPath) //Full file Path
                 }
             }
         }//meta sys crash
         ///////////////////////////////MetaDATA//End////////////////////////////////////////



         ///////////////////TILES//////////////////////
         PGEX_Section("TILES")
         {
             PGEX_SectionBegin(PGEFile::PGEX_Struct);

             PGEX_Items()
             {
                 PGEX_ItemBegin(PGEFile::PGEX_Struct);
                 tile = dummyWldTile();

                 PGEX_Values() //Look markers and values
                 {
                     PGEX_ValueBegin()
                     PGEX_UIntVal("ID", tile.id) //Tile ID
                     PGEX_SIntVal("X",  tile.x) //X Position
                     PGEX_SIntVal("Y",  tile.y) //Y Position
                 }

                 tile.array_id = FileData.tile_array_id++;
                 tile.index = FileData.tiles.size();
                 FileData.tiles.push_back(tile);
             }
         }//TILES


         ///////////////////SCENERY//////////////////////
         PGEX_Section("SCENERY")
         {
             PGEX_SectionBegin(PGEFile::PGEX_Struct);

             PGEX_Items()
             {
                 PGEX_ItemBegin(PGEFile::PGEX_Struct);
                 scen = dummyWldScen();

                 PGEX_Values() //Look markers and values
                 {
                     PGEX_ValueBegin()
                     PGEX_UIntVal("ID", scen.id ) //Scenery ID
                     PGEX_SIntVal("X", scen.x) //X Position
                     PGEX_SIntVal("Y", scen.y) //Y Position
                 }

                 scen.array_id = FileData.scene_array_id++;
                 scen.index = FileData.scenery.size();
                 FileData.scenery.push_back(scen);
             }
         }//SCENERY


         ///////////////////PATHS//////////////////////
         PGEX_Section("PATHS")
         {
             PGEX_SectionBegin(PGEFile::PGEX_Struct);

             PGEX_Items()
             {
                 PGEX_ItemBegin(PGEFile::PGEX_Struct);
                 pathitem = dummyWldPath();

                 PGEX_Values() //Look markers and values
                 {
                     PGEX_ValueBegin()
                     PGEX_UIntVal("ID", pathitem.id ) //Path ID
                     PGEX_SIntVal("X", pathitem.x) //X Position
                     PGEX_SIntVal("Y", pathitem.y) //Y Position
                 }
                 pathitem.array_id = FileData.path_array_id++;
                 pathitem.index = FileData.paths.size();
                 FileData.paths.push_back(pathitem);
             }
         }//PATHS


         ///////////////////MUSICBOXES//////////////////////
         PGEX_Section("MUSICBOXES")
         {
             PGEX_SectionBegin(PGEFile::PGEX_Struct);

             PGEX_Items()
             {
                 PGEX_ItemBegin(PGEFile::PGEX_Struct);
                 musicbox = dummyWldMusic();

                 PGEX_Values() //Look markers and values
                 {
                     PGEX_ValueBegin()
                     PGEX_UIntVal("ID", musicbox.id) //MISICBOX ID
                     PGEX_SIntVal("X", musicbox.x) //X Position
                     PGEX_SIntVal("Y", musicbox.y) //X Position
                     PGEX_StrVal ("MF", musicbox.music_file) //Custom music file
                 }
                 musicbox.array_id = FileData.musicbox_array_id++;
                 musicbox.index = FileData.music.size();
                 FileData.music.push_back(musicbox);
             }
         }//MUSICBOXES

         ///////////////////LEVELS//////////////////////
         PGEX_Section("LEVELS")
         {
             PGEX_SectionBegin(PGEFile::PGEX_Struct);

             PGEX_Items()
             {
                 PGEX_ItemBegin(PGEFile::PGEX_Struct);

                 lvlitem = dummyWldLevel();
                 PGEX_Values() //Look markers and values
                 {
                     PGEX_ValueBegin()
                     PGEX_UIntVal("ID", lvlitem.id) //LEVEL IMAGE ID
                     PGEX_SIntVal("X",  lvlitem.x) //X Position
                     PGEX_SIntVal("Y",  lvlitem.y) //X Position
                     PGEX_StrVal ("LF", lvlitem.lvlfile) //Target level file
                     PGEX_StrVal ("LT", lvlitem.title)  //Level title
                     PGEX_UIntVal("EI", lvlitem.entertowarp) //Entrance Warp ID (if 0 - start level from default points)
                     PGEX_SIntVal("ET", lvlitem.top_exit) //Open top path on exit type
                     PGEX_SIntVal("EL", lvlitem.left_exit) //Open left path on exit type
                     PGEX_SIntVal("ER", lvlitem.right_exit) //Open right path on exit type
                     PGEX_SIntVal("EB", lvlitem.bottom_exit) //Open bottom path on exit type
                     PGEX_SIntVal("WX", lvlitem.gotox) //Goto world map X
                     PGEX_SIntVal("WY", lvlitem.gotoy) //Goto world map Y
                     PGEX_BoolVal("AV", lvlitem.alwaysVisible) //Always visible
                     PGEX_BoolVal("SP", lvlitem.gamestart) //Is Game start point
                     PGEX_BoolVal("BP", lvlitem.pathbg) //Path background
                     PGEX_BoolVal("BG", lvlitem.bigpathbg) //Big path background
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

PGESTRING FileFormats::WriteExtendedWldFile(WorldData FileData)
{
    PGESTRING TextData;
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
    if(FileData.metaData.crash.used)
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
