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

#include "pge_file_lib_sys.h"
#include "file_formats.h"
#include "wld_filedata.h"
#include "file_strlist.h"
#include "smbx64.h"
#include "smbx64_macro.h"

//*********************************************************
//****************READ FILE FORMAT*************************
//*********************************************************

bool FileFormats::ReadSMBX64WldFileHeader(PGESTRING filePath, WorldData &FileData)
{
    SMBX64_FileBegin();
    int str_count=0;
    errorString.clear();
    CreateWorldHeader(FileData);
    FileData.RecentFormat = WorldData::SMBX64;
    FileData.RecentFormatVersion = 64;

    PGE_FileFormats_misc::TextFileInput in;
    if(!in.open(filePath, false))
    {
        FileData.ReadFileValid=false;
        return false;
    }

    PGE_FileFormats_misc::FileInfo in_1(filePath);
    FileData.filename = in_1.basename();
    FileData.path = in_1.dirpath();

    in.seek(0, PGE_FileFormats_misc::TextFileInput::begin);

    FileData.untitled = false;
    FileData.modified = false;

    //Enable strict mode for SMBX WLD file format
    FileData.smbx64strict = true;

    nextLine();   //Read first Line
    if( !SMBX64::IsUInt(line) ) //File format number
        goto badfile;
    else file_format=toInt(line);

    FileData.RecentFormatVersion = file_format;

    nextLine();
    FileData.EpisodeTitle = removeQuotes(line);
    /*if( SMBX64::qStr(line) ) //Episode name
        goto badfile;
    else FileData.EpisodeTitle = removeQuotes(line);*/

    if(ge(55))
    {
        nextLine(); wBoolVar(FileData.nocharacter1, line);//Edisode without Mario
        nextLine(); wBoolVar(FileData.nocharacter2, line);//Edisode without Luigi
        nextLine(); wBoolVar(FileData.nocharacter3, line);//Edisode without Peach
        nextLine(); wBoolVar(FileData.nocharacter4, line);//Edisode without Toad
        if(ge(56))
        {
            nextLine(); wBoolVar(FileData.nocharacter5, line);//Edisode without Link
        }
        //Convert into the bool array
        FileData.nocharacter.push_back(FileData.nocharacter1);
        FileData.nocharacter.push_back(FileData.nocharacter2);
        FileData.nocharacter.push_back(FileData.nocharacter3);
        FileData.nocharacter.push_back(FileData.nocharacter4);
        FileData.nocharacter.push_back(FileData.nocharacter5);
    }

    if(ge(3))
    {
        nextLine(); strVar(FileData.IntroLevel_file, line);//Autostart level
        nextLine(); wBoolVar(FileData.HubStyledWorld,line);//Don't use world map on this episode
        nextLine(); wBoolVar(FileData.restartlevel, line);//Restart level on playable character's death
    }

    if(ge(20))
    {
        nextLine(); UIntVar(FileData.stars, line);//Stars number
    }

    if(file_format >= 17)
    {
        nextLine(); strVar(FileData.author1, line); //Author 1
        nextLine(); strVar(FileData.author2, line); //Author 2
        nextLine(); strVar(FileData.author3, line); //Author 3
        nextLine(); strVar(FileData.author4, line); //Author 4
        nextLine(); strVar(FileData.author5, line); //Author 5

        FileData.authors.clear();
        FileData.authors += (IsEmpty(FileData.author1))? "" : FileData.author1+"\n";
        FileData.authors += (IsEmpty(FileData.author2))? "" : FileData.author2+"\n";
        FileData.authors += (IsEmpty(FileData.author3))? "" : FileData.author3+"\n";
        FileData.authors += (IsEmpty(FileData.author4))? "" : FileData.author4+"\n";
        FileData.authors += (IsEmpty(FileData.author5))? "" : FileData.author5;
    }

    FileData.ReadFileValid=true;
    in.close();
    return true;
badfile:
    in.close();
    FileData.ERROR_info="Invalid file format, detected file SMBX-"+fromNum(file_format)+"format";
    FileData.ERROR_linenum=str_count;
    FileData.ERROR_linedata=line;
    FileData.ReadFileValid=false;
    return false;
}



bool FileFormats::ReadSMBX64WldFileF(PGESTRING  filePath, WorldData &FileData)
{
    PGE_FileFormats_misc::TextFileInput file(filePath, false);
    return ReadSMBX64WldFile(file, FileData);
}

bool FileFormats::ReadSMBX64WldFileRaw(PGESTRING &rawdata, PGESTRING  filePath,  WorldData &FileData)
{
    PGE_FileFormats_misc::RawTextInput file(&rawdata, filePath);
    return ReadSMBX64WldFile(file, FileData);
}

bool FileFormats::ReadSMBX64WldFile(PGE_FileFormats_misc::TextInput &in, WorldData &FileData)
{
    SMBX64_FileBegin();
    PGESTRING filePath = in.getFilePath();
    //SMBX64_File( RawData );

    CreateWorldData(FileData);

    FileData.RecentFormat = WorldData::SMBX64;
    FileData.RecentFormatVersion = 64;

    //Add path data
    if(!IsEmpty(filePath))
    {
        PGE_FileFormats_misc::FileInfo in_1(filePath);
        FileData.filename = in_1.basename();
        FileData.path = in_1.dirpath();
    }

    FileData.untitled = false;
    FileData.modified = false;

    //Enable strict mode for SMBX WLD file format
    FileData.smbx64strict = true;

    WorldTiles tile;
    WorldScenery scen;
    WorldPaths pathitem;
    WorldLevels lvlitem;
    WorldMusic musicbox;

    nextLine();   //Read first line
    UIntVar(file_format, line);//File format number

    FileData.RecentFormatVersion = file_format;

    nextLine(); strVar(FileData.EpisodeTitle, line);

    if(ge(55))
    {
        nextLine(); wBoolVar(FileData.nocharacter1, line);//Edisode without Mario
        nextLine(); wBoolVar(FileData.nocharacter2, line);//Edisode without Luigi
        nextLine(); wBoolVar(FileData.nocharacter3, line);//Edisode without Peach
        nextLine(); wBoolVar(FileData.nocharacter4, line);//Edisode without Toad
        if(ge(56))
        {
            nextLine(); wBoolVar(FileData.nocharacter5, line);//Edisode without Link
        }
        //Convert into the bool array
        FileData.nocharacter.push_back(FileData.nocharacter1);
        FileData.nocharacter.push_back(FileData.nocharacter2);
        FileData.nocharacter.push_back(FileData.nocharacter3);
        FileData.nocharacter.push_back(FileData.nocharacter4);
        FileData.nocharacter.push_back(FileData.nocharacter5);
    }

    if(ge(3))
    {
        nextLine(); strVar(FileData.IntroLevel_file, line);//Autostart level
        nextLine(); wBoolVar(FileData.HubStyledWorld,line);//Don't use world map on this episode
        nextLine(); wBoolVar(FileData.restartlevel, line);//Restart level on playable character's death
    }

    if(ge(20))
    {
        nextLine(); UIntVar(FileData.stars, line);//Stars number
    }

    if(file_format >= 17)
    {
        nextLine(); strVar(FileData.author1, line); //Author 1
        nextLine(); strVar(FileData.author2, line); //Author 2
        nextLine(); strVar(FileData.author3, line); //Author 3
        nextLine(); strVar(FileData.author4, line); //Author 4
        nextLine(); strVar(FileData.author5, line); //Author 5

        FileData.authors.clear();
        FileData.authors += (IsEmpty(FileData.author1))? "" : FileData.author1+"\n";
        FileData.authors += (IsEmpty(FileData.author2))? "" : FileData.author2+"\n";
        FileData.authors += (IsEmpty(FileData.author3))? "" : FileData.author3+"\n";
        FileData.authors += (IsEmpty(FileData.author4))? "" : FileData.author4+"\n";
        FileData.authors += (IsEmpty(FileData.author5))? "" : FileData.author5;
    }


    ////////////Tiles Data//////////
    nextLine();
    while( (line!="next") && (!in.eof()) )
    {
        tile = CreateWldTile();
                    SIntVar(tile.x,line);//Tile x
        nextLine(); SIntVar(tile.y,line);//Tile y
        nextLine(); UIntVar(tile.id,line);//Tile ID

        tile.array_id = FileData.tile_array_id;
        FileData.tile_array_id++;
        tile.index = FileData.tiles.size(); //Apply element index

        FileData.tiles.push_back(tile);
        nextLine();
    }

    ////////////Scenery Data//////////
    nextLine();
    while( (line!="next")  && (!in.eof()) )
    {
        scen = CreateWldScenery();
                    SIntVar(scen.x,line);//Scenery x
        nextLine(); SIntVar(scen.y,line);//Scenery y
        nextLine(); UIntVar(scen.id,line);//Scenery ID

        scen.array_id = FileData.scene_array_id;
        FileData.scene_array_id++;
        scen.index = FileData.scenery.size(); //Apply element index

        FileData.scenery.push_back(scen);

        nextLine();
    }

    ////////////Paths Data//////////
    nextLine();
    while( (line!="next") && (!in.eof()) )
    {
        pathitem = CreateWldPath();
                    SIntVar(pathitem.x,line);//Path x
        nextLine(); SIntVar(pathitem.y,line);//Path y
        nextLine(); UIntVar(pathitem.id,line);//Path ID

        pathitem.array_id = FileData.path_array_id;
        FileData.path_array_id++;
        pathitem.index = FileData.paths.size(); //Apply element index

        FileData.paths.push_back(pathitem);

        nextLine();
    }

    ////////////LevelBox Data//////////
    nextLine();
    while( (line!="next")  && (!in.eof()) )
    {
        lvlitem = CreateWldLevel();

                    SIntVar(lvlitem.x,line);//Level x
        nextLine(); SIntVar(lvlitem.y,line);//Level y
        nextLine(); UIntVar(lvlitem.id,line);//Level ID
        nextLine(); strVar(lvlitem.lvlfile, line);//Level file
        nextLine(); strVar(lvlitem.title, line);//Level title
        nextLine(); SIntVar(lvlitem.top_exit, line);//Top exit
        nextLine(); SIntVar(lvlitem.left_exit, line);//Left exit
        nextLine(); SIntVar(lvlitem.bottom_exit, line);//bottom exit
        nextLine(); SIntVar(lvlitem.right_exit, line);//right exit
        if(ge(4)) { nextLine(); UIntVar(lvlitem.entertowarp, line);}//Enter via Level's warp

        if(ge(22))
        {
            nextLine(); wBoolVar(lvlitem.alwaysVisible, line);//Always Visible
            nextLine(); wBoolVar(lvlitem.pathbg, line);//Path background
            nextLine(); wBoolVar(lvlitem.gamestart, line);//Game start point
            nextLine(); SIntVar(lvlitem.gotox, line);//Goto x on World map
            nextLine(); SIntVar(lvlitem.gotoy, line);//Goto y on World map
            nextLine(); wBoolVar(lvlitem.bigpathbg, line);//Big Path background
        }
        else
        {
            if(lvlitem.id==1)
                lvlitem.gamestart=true;
        }


        lvlitem.array_id = FileData.level_array_id;
        FileData.level_array_id++;
        lvlitem.index = FileData.levels.size(); //Apply element index

        FileData.levels.push_back(lvlitem);

        nextLine();
    }

    ////////////MusicBox Data//////////
    nextLine();
    while( (line != "next") && (line !="") && (!in.eof()) )
    {
        musicbox = CreateWldMusicbox();
                    SIntVar(musicbox.x,line);//MusicBox x
        nextLine(); SIntVar(musicbox.y,line);//MusicBox y
        nextLine(); UIntVar(musicbox.id,line);//MusicBox ID

        musicbox.array_id = FileData.musicbox_array_id;
        FileData.musicbox_array_id++;
        musicbox.index = FileData.music.size(); //Apply element index

        FileData.music.push_back(musicbox);

        nextLine();
    }

    nextLine(); // Read last line

    /*if((line!="")&&(!in.eof()))
        goto badfile;*/

FileData.ReadFileValid=true;

return true;

badfile:    //If file format not corrects
    if(file_format>0)
        FileData.ERROR_info="Detected file format: SMBX-"+fromNum(file_format)+" is invalid";
    else
        FileData.ERROR_info="It is not an SMBX world map file";
    FileData.ERROR_linenum = in.getCurrentLineNumber();
    FileData.ERROR_linedata=line;
    FileData.ReadFileValid=false;
return false;
}


//*********************************************************
//****************WRITE FILE FORMAT************************
//*********************************************************

bool FileFormats::WriteSMBX64WldFileF(PGESTRING filePath, WorldData &FileData, int file_format)
{
    PGE_FileFormats_misc::TextFileOutput file;
    if(!file.open(filePath, false, true, PGE_FileFormats_misc::TextOutput::truncate))
        return false;
    return WriteSMBX64WldFile(file, FileData, file_format);
}

bool FileFormats::WriteSMBX64WldFileRaw(WorldData &FileData, PGESTRING &rawdata, int file_format)
{
    PGE_FileFormats_misc::RawTextOutput file;
    if(!file.open(&rawdata, PGE_FileFormats_misc::TextOutput::truncate))
        return false;
    return WriteSMBX64WldFile(file, FileData, file_format);
}

bool FileFormats::WriteSMBX64WldFile(PGE_FileFormats_misc::TextOutput &out, WorldData &FileData, int file_format)
{
    int i;

    //Prevent out of range: 0....64
    if(file_format<0) file_format = 0;
    else
    if(file_format>64) file_format = 64;

    FileData.RecentFormat = WorldData::SMBX64;
    FileData.RecentFormatVersion = file_format;

    out << SMBX64::IntS(file_format);              //Format version
    out << SMBX64::qStrS(FileData.EpisodeTitle);   //Episode title

    FileData.nocharacter1 = (FileData.nocharacter.size()>0)?FileData.nocharacter[0]:false;
    FileData.nocharacter2 = (FileData.nocharacter.size()>1)?FileData.nocharacter[1]:false;
    FileData.nocharacter3 = (FileData.nocharacter.size()>2)?FileData.nocharacter[2]:false;
    FileData.nocharacter4 = (FileData.nocharacter.size()>3)?FileData.nocharacter[3]:false;
    FileData.nocharacter5 = (FileData.nocharacter.size()>4)?FileData.nocharacter[4]:false;

    if(file_format>=55)
    {
        out << SMBX64::BoolS(FileData.nocharacter1);
        out << SMBX64::BoolS(FileData.nocharacter2);
        out << SMBX64::BoolS(FileData.nocharacter3);
        out << SMBX64::BoolS(FileData.nocharacter4);
        if(file_format>=56)
            out << SMBX64::BoolS(FileData.nocharacter5);
    }
    if(file_format>=3)
    {
        out << SMBX64::qStrS(FileData.IntroLevel_file);
        out << SMBX64::BoolS(FileData.HubStyledWorld);
        out << SMBX64::BoolS(FileData.restartlevel);
    }
    if(file_format>=20)
        out << SMBX64::IntS(FileData.stars);

    PGESTRINGList credits;
    PGE_SPLITSTRING(credits, FileData.authors, "\n");
    FileData.author1 = (credits.size()>0) ? credits[0] : "";
    FileData.author2 = (credits.size()>1) ? credits[1] : "";
    FileData.author3 = (credits.size()>2) ? credits[2] : "";
    FileData.author4 = (credits.size()>3) ? credits[3] : "";
    FileData.author5 = (credits.size()>4) ? credits[4] : "";

    if(file_format>=17)
    {
        out << SMBX64::qStrS(FileData.author1);
        out << SMBX64::qStrS(FileData.author2);
        out << SMBX64::qStrS(FileData.author3);
        out << SMBX64::qStrS(FileData.author4);
        out << SMBX64::qStrS(FileData.author5);
    }

    for(i=0;i<(signed)FileData.tiles.size();i++)
    {
        out << SMBX64::IntS(FileData.tiles[i].x);
        out << SMBX64::IntS(FileData.tiles[i].y);
        out << SMBX64::IntS(FileData.tiles[i].id);
    }
    out << "\"next\"\n";//Separator

    for(i=0;i<(signed)FileData.scenery.size();i++)
    {
        out << SMBX64::IntS(FileData.scenery[i].x);
        out << SMBX64::IntS(FileData.scenery[i].y);
        out << SMBX64::IntS(FileData.scenery[i].id);
    }
    out << "\"next\"\n";//Separator

    for(i=0;i<(signed)FileData.paths.size();i++)
    {
        out << SMBX64::IntS(FileData.paths[i].x);
        out << SMBX64::IntS(FileData.paths[i].y);
        out << SMBX64::IntS(FileData.paths[i].id);
    }
    out << "\"next\"\n";//Separator

    for(i=0;i<(signed)FileData.levels.size();i++)
    {
        out << SMBX64::IntS(FileData.levels[i].x);
        out << SMBX64::IntS(FileData.levels[i].y);
        out << SMBX64::IntS(FileData.levels[i].id);
        out << SMBX64::qStrS(FileData.levels[i].lvlfile);
        out << SMBX64::qStrS(FileData.levels[i].title);
        out << SMBX64::IntS(FileData.levels[i].top_exit);
        out << SMBX64::IntS(FileData.levels[i].left_exit);
        out << SMBX64::IntS(FileData.levels[i].bottom_exit);
        out << SMBX64::IntS(FileData.levels[i].right_exit);
        if(file_format>=4)
            out << SMBX64::IntS(FileData.levels[i].entertowarp);
        if(file_format>=22)
        {
            out << SMBX64::BoolS(FileData.levels[i].alwaysVisible);
            out << SMBX64::BoolS(FileData.levels[i].pathbg);
            out << SMBX64::BoolS(FileData.levels[i].gamestart);
            out << SMBX64::IntS(FileData.levels[i].gotox);
            out << SMBX64::IntS(FileData.levels[i].gotoy);
            out << SMBX64::BoolS(FileData.levels[i].bigpathbg);
        }
    }
    out << "\"next\"\n";//Separator

    for(i=0;i<(signed)FileData.music.size();i++)
    {
        out << SMBX64::IntS(FileData.music[i].x);
        out << SMBX64::IntS(FileData.music[i].y);
        out << SMBX64::IntS(FileData.music[i].id);
    }
    out << "\"next\"\n";//Separator

    return true;
}

