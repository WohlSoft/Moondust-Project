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

#include "pge_file_lib_sys.h"
#include "file_formats.h"
#include "wld_filedata.h"
#include "file_strlist.h"
#include "smbx64.h"
#include "smbx64_macro.h"

//*********************************************************
//****************READ FILE FORMAT*************************
//*********************************************************

//WorldData FileFormats::ReadWorldFile(PGEFILE &inf)
//{
//    errorString.clear();
//    QByteArray data;

//    typedef QPair<QByteArray, QString > magicFileFormat;

//    //Check known file formats before start parsing
//    QList<magicFileFormat > formats;
//    magicFileFormat format;

//    char terraria[7] = {0x66,0x00,0x00,0x00,0x0A,0x00,0x5B};
//        format.first.setRawData((char *)&terraria, 7);
//        format.second = "Terraria world";
//    formats.push_back(format);

//    char smbgm[7] = {0x32,0x44,0x30,0x31,0x30,0x30,0x30};
//        format.first.setRawData((char *)&smbgm, 7);
//        format.second = "Super Mario Game Master world map";
//    formats.push_back(format);

//    data = inf.read(7);
//    if(data.size()==0)
//    {
//        WorldData FileData = dummyWldDataArray();
//        if(data.size()==0)
//        BadFileMsg(inf.fileName()+
//            QString("\nFile is empty! (size of file is 0 bytes)"),
//                   0, "<FILE IS EMPTY>");
//        else
//        BadFileMsg(inf.fileName()+
//            QString("\nFile is too small! (size of file is %1 bytes)").arg(data.size()),
//                   0, "<FILE IS TOO SMALL>");
//        FileData.ReadFileValid=false;
//        return FileData;
//    }
//    else
//    //Check magic number: should be number from 0 to 64 and \n character after
//    if(
//            ((int)data.at(0)>0x36)||
//            ((int)data.at(0)<0x30)||
//            ( ((int)data.at(1)!=0x0D && (int)data.at(1)!=0x0A)&&
//             ((int)data.at(2)!=0x0D && (int)data.at(2)!=0x0A) )
//      )
//    {
//        foreach (magicFileFormat format, formats)
//        {
//            if(data == format.first)
//            {
//                WorldData FileData = dummyWldDataArray();
//                BadFileMsg(inf.fileName()+
//                    "\nThis is a "+format.second+" file, this format is not support.", 0, "<BYNARY>");
//                FileData.ReadFileValid=false;
//                return FileData;
//            }
//        }

//        WorldData FileData = dummyWldDataArray();
//        BadFileMsg(inf.fileName()+
//            "\nThis is not SMBX64 world map file, Bad magic number!", 0, "<NULL>");
//        FileData.ReadFileValid=false;
//        return FileData;
//    }


//    inf.reset();
//    QTextStream in(&inf);   //Read File

//    in.setAutoDetectUnicode(true);
//    in.setLocale(QLocale::system());
//    in.setCodec(QTextCodec::codecForLocale());

//    return ReadSMBX64WldFile( in.readAll(), inf.fileName() );
//}


WorldData FileFormats::ReadSMBX64WldFileHeader(PGESTRING filePath)
{
    SMBX64_FileBegin();

    errorString.clear();
    WorldData FileData;
    FileData = CreateWorldData();

    PGE_FileFormats_misc::TextFileInput in;
    if(!in.open(filePath, false))
    {
        FileData.ReadFileValid=false;
        return FileData;
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
    if( SMBX64::uInt(line) ) //File format number
        goto badfile;
    else file_format=toInt(line);

    nextLine();
    if( SMBX64::qStr(line) ) //Episode name
        goto badfile;
    else FileData.EpisodeTitle = removeQuotes(line);

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
        FileData.authors += (FileData.author1.PGESTRINGisEmpty())? "" : FileData.author1+"\n";
        FileData.authors += (FileData.author2.PGESTRINGisEmpty())? "" : FileData.author2+"\n";
        FileData.authors += (FileData.author3.PGESTRINGisEmpty())? "" : FileData.author3+"\n";
        FileData.authors += (FileData.author4.PGESTRINGisEmpty())? "" : FileData.author4+"\n";
        FileData.authors += (FileData.author5.PGESTRINGisEmpty())? "" : FileData.author5;
    }

    FileData.ReadFileValid=true;
    in.close();
    return FileData;
badfile:
    //qDebug()<<"Wrong file"<<filePath<<"format"<<file_format<<"line: "<<str_count<< "data: "<<line;
    in.close();
    FileData.ReadFileValid=false;
    return FileData;
}

WorldData FileFormats::ReadSMBX64WldFile(PGESTRING RawData, PGESTRING filePath, bool sielent)
{
    SMBX64_File(RawData);

    WorldData FileData = CreateWorldData();

    //Add path data
    if(!filePath.PGESTRINGisEmpty())
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
        FileData.authors += (FileData.author1.PGESTRINGisEmpty())? "" : FileData.author1+"\n";
        FileData.authors += (FileData.author2.PGESTRINGisEmpty())? "" : FileData.author2+"\n";
        FileData.authors += (FileData.author3.PGESTRINGisEmpty())? "" : FileData.author3+"\n";
        FileData.authors += (FileData.author4.PGESTRINGisEmpty())? "" : FileData.author4+"\n";
        FileData.authors += (FileData.author5.PGESTRINGisEmpty())? "" : FileData.author5;
    }


    ////////////Tiles Data//////////
    nextLine();
    while(line!="\"next\"")
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
    while(line!="\"next\"")
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
    while(line!="\"next\"")
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
    while(line!="\"next\"")
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
    while(line!="\"next\"")
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

    if((line!="")&&(!in.isEOF()))
        goto badfile;


FileData.ReadFileValid=true;

return FileData;

badfile:    //If file format not corrects
    if(!sielent)
        BadFileMsg(filePath+"\nFile format "+fromNum(file_format), str_count, line);
    FileData.ReadFileValid=false;
return FileData;
}


//*********************************************************
//****************WRITE FILE FORMAT************************
//*********************************************************


PGESTRING FileFormats::WriteSMBX64WldFile(WorldData FileData, int file_format)
{
    PGESTRING TextData;
    int i;

    //Prevent out of range: 0....64
    if(file_format<0) file_format = 0;
    else
    if(file_format>64) file_format = 64;



    TextData += SMBX64::IntS(file_format);              //Format version
    TextData += SMBX64::qStrS(FileData.EpisodeTitle);   //Episode title

    FileData.nocharacter1 = (FileData.nocharacter.size()>0)?FileData.nocharacter[0]:false;
    FileData.nocharacter2 = (FileData.nocharacter.size()>1)?FileData.nocharacter[1]:false;
    FileData.nocharacter3 = (FileData.nocharacter.size()>2)?FileData.nocharacter[2]:false;
    FileData.nocharacter4 = (FileData.nocharacter.size()>3)?FileData.nocharacter[3]:false;
    FileData.nocharacter5 = (FileData.nocharacter.size()>4)?FileData.nocharacter[4]:false;

    if(file_format>=55)
    {
        TextData += SMBX64::BoolS(FileData.nocharacter1);
        TextData += SMBX64::BoolS(FileData.nocharacter2);
        TextData += SMBX64::BoolS(FileData.nocharacter3);
        TextData += SMBX64::BoolS(FileData.nocharacter4);
        if(file_format>=56)
            TextData += SMBX64::BoolS(FileData.nocharacter5);
    }
    if(file_format>=3)
    {
        TextData += SMBX64::qStrS(FileData.IntroLevel_file);
        TextData += SMBX64::BoolS(FileData.HubStyledWorld);
        TextData += SMBX64::BoolS(FileData.restartlevel);
    }
    if(file_format>=20)
        TextData += SMBX64::IntS(FileData.stars);

    PGESTRINGList credits;
    PGE_SPLITSTR(credits, FileData.authors, "\n");
    FileData.author1 = (credits.size()>0) ? credits[0] : "";
    FileData.author2 = (credits.size()>1) ? credits[1] : "";
    FileData.author3 = (credits.size()>2) ? credits[2] : "";
    FileData.author4 = (credits.size()>3) ? credits[3] : "";
    FileData.author5 = (credits.size()>4) ? credits[4] : "";

    if(file_format>=17)
    {
        TextData += SMBX64::qStrS(FileData.author1);
        TextData += SMBX64::qStrS(FileData.author2);
        TextData += SMBX64::qStrS(FileData.author3);
        TextData += SMBX64::qStrS(FileData.author4);
        TextData += SMBX64::qStrS(FileData.author5);
    }

    for(i=0;i<(signed)FileData.tiles.size();i++)
    {
        TextData += SMBX64::IntS(FileData.tiles[i].x);
        TextData += SMBX64::IntS(FileData.tiles[i].y);
        TextData += SMBX64::IntS(FileData.tiles[i].id);
    }
    TextData += "\"next\"\n";//Separator

    for(i=0;i<(signed)FileData.scenery.size();i++)
    {
        TextData += SMBX64::IntS(FileData.scenery[i].x);
        TextData += SMBX64::IntS(FileData.scenery[i].y);
        TextData += SMBX64::IntS(FileData.scenery[i].id);
    }
    TextData += "\"next\"\n";//Separator

    for(i=0;i<(signed)FileData.paths.size();i++)
    {
        TextData += SMBX64::IntS(FileData.paths[i].x);
        TextData += SMBX64::IntS(FileData.paths[i].y);
        TextData += SMBX64::IntS(FileData.paths[i].id);
    }
    TextData += "\"next\"\n";//Separator

    for(i=0;i<(signed)FileData.levels.size();i++)
    {
        TextData += SMBX64::IntS(FileData.levels[i].x);
        TextData += SMBX64::IntS(FileData.levels[i].y);
        TextData += SMBX64::IntS(FileData.levels[i].id);
        TextData += SMBX64::qStrS(FileData.levels[i].lvlfile);
        TextData += SMBX64::qStrS(FileData.levels[i].title);
        TextData += SMBX64::IntS(FileData.levels[i].top_exit);
        TextData += SMBX64::IntS(FileData.levels[i].left_exit);
        TextData += SMBX64::IntS(FileData.levels[i].bottom_exit);
        TextData += SMBX64::IntS(FileData.levels[i].right_exit);
        if(file_format>=4)
            TextData += SMBX64::IntS(FileData.levels[i].entertowarp);
        if(file_format>=22)
        {
            TextData += SMBX64::BoolS(FileData.levels[i].alwaysVisible);
            TextData += SMBX64::BoolS(FileData.levels[i].pathbg);
            TextData += SMBX64::BoolS(FileData.levels[i].gamestart);
            TextData += SMBX64::IntS(FileData.levels[i].gotox);
            TextData += SMBX64::IntS(FileData.levels[i].gotoy);
            TextData += SMBX64::BoolS(FileData.levels[i].bigpathbg);
        }
    }
    TextData += "\"next\"\n";//Separator

    for(i=0;i<(signed)FileData.music.size();i++)
    {
        TextData += SMBX64::IntS(FileData.music[i].x);
        TextData += SMBX64::IntS(FileData.music[i].y);
        TextData += SMBX64::IntS(FileData.music[i].id);
    }
    TextData += "\"next\"\n";//Separator


    return TextData;
}
