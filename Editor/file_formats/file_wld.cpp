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

#include "wld_filedata.h"
#include "file_formats.h"


//*********************************************************
//****************READ FILE FORMAT*************************
//*********************************************************

//World file Read
WorldData FileFormats::ReadWorldFile(QFile &inf)
{
    QTextStream in(&inf);   //Read File

    in.setAutoDetectUnicode(true);
    in.setLocale(QLocale::system());
    in.setCodec(QTextCodec::codecForLocale());

    int str_count=0;        //Line Counter
    int file_format=0;        //File format number
    QString line;           //Current Line data

    WorldData FileData = dummyWldDataArray();

    FileData.untitled = false;
    FileData.modified = false;

    //Enable strict mode for SMBX WLD file format
    FileData.smbx64strict = true;

    WorldTiles tile;
    WorldScenery scen;
    WorldPaths pathitem;
    WorldLevels lvlitem;
    WorldMusic musicbox;

    str_count++;line = in.readLine();   //Read first Line
    if( SMBX64::Int(line) ) //File format number
        goto badfile;
    else file_format=line.toInt();

    str_count++;line = in.readLine();
    if( SMBX64::qStr(line) ) //Episode name
        goto badfile;
    else FileData.EpisodeTitle = removeQuotes(line);

    if(file_format >= 55)
    {
        str_count++;line = in.readLine();
        if( SMBX64::wBool(line) ) //Edisode without Mario
            goto badfile;
        else FileData.nocharacter1 = SMBX64::wBoolR(line);

        str_count++;line = in.readLine();
        if( SMBX64::wBool(line) ) //Edisode without Luigi
            goto badfile;
        else FileData.nocharacter2 = SMBX64::wBoolR(line);

        str_count++;line = in.readLine();
        if( SMBX64::wBool(line) ) //Edisode without Peach
            goto badfile;
        else FileData.nocharacter3 = SMBX64::wBoolR(line);

        str_count++;line = in.readLine();
        if( SMBX64::wBool(line) ) //Edisode without Toad
            goto badfile;
        else FileData.nocharacter4 = SMBX64::wBoolR(line);

        str_count++;line = in.readLine();
        if( SMBX64::wBool(line) ) //Edisode without Link
            goto badfile;
        else FileData.nocharacter5 = SMBX64::wBoolR(line);

        //Convert into the bool array
        FileData.nocharacter<<
             FileData.nocharacter1<<
             FileData.nocharacter2<<
             FileData.nocharacter3<<
             FileData.nocharacter4<<
             FileData.nocharacter5;


    }

    if(file_format >= 10)
    {
        str_count++;line = in.readLine();
        if( SMBX64::qStr(line) ) //Autostart level
            goto badfile;
        else FileData.IntroLevel_file = removeQuotes(line);

        str_count++;line = in.readLine();
        if( SMBX64::wBool(line) ) //Don't use world map on this episode
            goto badfile;
        else FileData.HubStyledWorld = SMBX64::wBoolR(line);

        str_count++;line = in.readLine();
        if( SMBX64::wBool(line) ) //Restart level on playable character's death
            goto badfile;
        else FileData.restartlevel = SMBX64::wBoolR(line);
    }

    if(file_format >= 20)
    {
        str_count++;line = in.readLine();
        if( SMBX64::Int(line) ) //Stars quantity
            goto badfile;
        else FileData.stars = line.toInt();
    }

    if(file_format >= 10)
    {
        str_count++;line = in.readLine();
        if( SMBX64::qStr(line) ) //Author 1
            goto badfile;
        else FileData.author1 = removeQuotes(line);

        str_count++;line = in.readLine();
        if( SMBX64::qStr(line) ) //Author 2
            goto badfile;
        else FileData.author2 = removeQuotes(line);

        str_count++;line = in.readLine();
        if( SMBX64::qStr(line) ) //Author 3
            goto badfile;
        else FileData.author3 = removeQuotes(line);

        str_count++;line = in.readLine();
        if( SMBX64::qStr(line) ) //Author 4
            goto badfile;
        else FileData.author4 = removeQuotes(line);

        str_count++;line = in.readLine();
        if( SMBX64::qStr(line) ) //Author 5
            goto badfile;
        else FileData.author5 = removeQuotes(line);

        FileData.authors.clear();
        FileData.authors += (FileData.author1.isEmpty())? "" : FileData.author1+"\n";
        FileData.authors += (FileData.author2.isEmpty())? "" : FileData.author2+"\n";
        FileData.authors += (FileData.author3.isEmpty())? "" : FileData.author3+"\n";
        FileData.authors += (FileData.author4.isEmpty())? "" : FileData.author4+"\n";
        FileData.authors += (FileData.author5.isEmpty())? "" : FileData.author5;
    }


    ////////////Tiles Data//////////
    str_count++;line = in.readLine();
    while(line!="\"next\"")
    {
        tile = dummyWldTile();
        if(SMBX64::sInt(line)) //Tile x
            goto badfile;
        else tile.x = line.toInt();

        str_count++;line = in.readLine();
        if(SMBX64::sInt(line)) //Tile y
            goto badfile;
        else tile.y = line.toInt();

        str_count++;line = in.readLine();
        if(SMBX64::Int(line)) //Tile ID
            goto badfile;
        else tile.id = line.toInt();

        tile.array_id = FileData.tile_array_id;
        FileData.tile_array_id++;
        tile.index = FileData.tiles.size(); //Apply element index

        FileData.tiles.push_back(tile);

        str_count++;line = in.readLine();
    }

    ////////////Scenery Data//////////
    str_count++;line = in.readLine();
    while(line!="\"next\"")
    {
        scen = dummyWldScen();
        if(SMBX64::sInt(line)) //Scenery x
            goto badfile;
        else scen.x = line.toInt();

        str_count++;line = in.readLine();
        if(SMBX64::sInt(line)) //Scenery y
            goto badfile;
        else scen.y = line.toInt();

        str_count++;line = in.readLine();
        if(SMBX64::Int(line)) //Scenery ID
            goto badfile;
        else scen.id = line.toInt();

        scen.array_id = FileData.scene_array_id;
        FileData.scene_array_id++;
        scen.index = FileData.scenery.size(); //Apply element index

        FileData.scenery.push_back(scen);

        str_count++;line = in.readLine();
    }

    ////////////Paths Data//////////
    str_count++;line = in.readLine();
    while(line!="\"next\"")
    {
        pathitem = dummyWldPath();
        if(SMBX64::sInt(line)) //Path x
            goto badfile;
        else pathitem.x = line.toInt();

        str_count++;line = in.readLine();
        if(SMBX64::sInt(line)) //Path y
            goto badfile;
        else pathitem.y = line.toInt();

        str_count++;line = in.readLine();
        if(SMBX64::Int(line)) //Path ID
            goto badfile;
        else pathitem.id = line.toInt();

        pathitem.array_id = FileData.path_array_id;
        FileData.path_array_id++;
        pathitem.index = FileData.paths.size(); //Apply element index

        FileData.paths.push_back(pathitem);

        str_count++;line = in.readLine();
    }

    ////////////LevelBox Data//////////
    str_count++;line = in.readLine();
    while(line!="\"next\"")
    {
        lvlitem = dummyWldLevel();

        if(SMBX64::sInt(line)) //Level x
            goto badfile;
        else lvlitem.x = line.toInt();

        str_count++;line = in.readLine();
        if(SMBX64::sInt(line)) //Level y
            goto badfile;
        else lvlitem.y = line.toInt();

        str_count++;line = in.readLine();
        if(SMBX64::Int(line)) //Level ID
            goto badfile;
        else lvlitem.id = line.toInt();

        str_count++;line = in.readLine();
        if(SMBX64::qStr(line)) //Level file
            goto badfile;
        else lvlitem.lvlfile = removeQuotes(line);

        str_count++;line = in.readLine();
        if(SMBX64::qStr(line)) //Level title
            goto badfile;
        else lvlitem.title = removeQuotes(line);

        str_count++;line = in.readLine();
        if(SMBX64::sInt(line)) //Top exit
            goto badfile;
        else lvlitem.top_exit = line.toInt();

        str_count++;line = in.readLine();
        if(SMBX64::sInt(line)) //Left exit
            goto badfile;
        else lvlitem.left_exit = line.toInt();

        str_count++;line = in.readLine();
        if(SMBX64::sInt(line)) //bottom exit
            goto badfile;
        else lvlitem.bottom_exit = line.toInt();

        str_count++;line = in.readLine();
        if(SMBX64::sInt(line)) //right exit
            goto badfile;
        else lvlitem.right_exit = line.toInt();

        if(file_format >= 10)
        {
            str_count++;line = in.readLine();
            if(SMBX64::Int(line)) //Enter via Level's warp
                goto badfile;
            else lvlitem.entertowarp = line.toInt();
        }

        if(file_format >= 28)
        {
            str_count++;line = in.readLine();
            if(SMBX64::wBool(line)) //Always Visible
                goto badfile;
            else lvlitem.alwaysVisible = SMBX64::wBoolR(line);

            str_count++;line = in.readLine();
            if(SMBX64::wBool(line)) //Path background
                goto badfile;
            else lvlitem.pathbg = SMBX64::wBoolR(line);

            str_count++;line = in.readLine();
            if(SMBX64::wBool(line)) //Game start point
                goto badfile;
            else lvlitem.gamestart = SMBX64::wBoolR(line);

            str_count++;line = in.readLine();
            if(SMBX64::sInt(line)) //Goto x on World map
                goto badfile;
            else lvlitem.gotox = line.toInt();

            str_count++;line = in.readLine();
            if(SMBX64::sInt(line)) //Goto y on World map
                goto badfile;
            else lvlitem.gotoy = line.toInt();

            str_count++;line = in.readLine();
            if(SMBX64::wBool(line)) //Big Path background
                goto badfile;
            else lvlitem.bigpathbg = SMBX64::wBoolR(line);
        }

        lvlitem.array_id = FileData.level_array_id;
        FileData.level_array_id++;
        lvlitem.index = FileData.levels.size(); //Apply element index

        FileData.levels.push_back(lvlitem);

        str_count++;line = in.readLine();
    }

    ////////////MusicBox Data//////////
    str_count++;line = in.readLine();
    while(line!="\"next\"")
    {
        musicbox = dummyWldMusic();
        if(SMBX64::sInt(line)) //MusicBox x
            goto badfile;
        else musicbox.x = line.toInt();

        str_count++;line = in.readLine();
        if(SMBX64::sInt(line)) //MusicBox y
            goto badfile;
        else musicbox.y = line.toInt();

        str_count++;line = in.readLine();
        if(SMBX64::Int(line)) //MusicBox ID
            goto badfile;
        else musicbox.id = line.toInt();

        musicbox.array_id = FileData.musicbox_array_id;
        FileData.musicbox_array_id++;
        musicbox.index = FileData.music.size(); //Apply element index

        FileData.music.push_back(musicbox);

        str_count++;line = in.readLine();
    }


    str_count++;line = in.readLine(); // Read last line

    if((line!="")&&(!line.isNull()))
        goto badfile;


FileData.ReadFileValid=true;

return FileData;

badfile:    //If file format not corrects
    BadFileMsg(inf.fileName(), str_count, line);
    FileData.ReadFileValid=false;
return FileData;
}


//*********************************************************
//****************WRITE FILE FORMAT************************
//*********************************************************


QString FileFormats::WriteSMBX64WldFile(WorldData FileData)
{
    QString TextData;
    int i;

    TextData += SMBX64::IntS(64);                     //Format version 64
    TextData += SMBX64::qStrS(FileData.EpisodeTitle);   //Episode title

    FileData.nocharacter1 = (FileData.nocharacter.size()>0)?FileData.nocharacter[0]:false;
    FileData.nocharacter2 = (FileData.nocharacter.size()>1)?FileData.nocharacter[1]:false;
    FileData.nocharacter3 = (FileData.nocharacter.size()>2)?FileData.nocharacter[2]:false;
    FileData.nocharacter4 = (FileData.nocharacter.size()>3)?FileData.nocharacter[3]:false;
    FileData.nocharacter5 = (FileData.nocharacter.size()>4)?FileData.nocharacter[4]:false;

    TextData += SMBX64::BoolS(FileData.nocharacter1);
    TextData += SMBX64::BoolS(FileData.nocharacter2);
    TextData += SMBX64::BoolS(FileData.nocharacter3);
    TextData += SMBX64::BoolS(FileData.nocharacter4);
    TextData += SMBX64::BoolS(FileData.nocharacter5);
    TextData += SMBX64::qStrS(FileData.IntroLevel_file);
    TextData += SMBX64::BoolS(FileData.HubStyledWorld);
    TextData += SMBX64::BoolS(FileData.restartlevel);
    TextData += SMBX64::IntS(FileData.stars);

    QStringList credits = FileData.authors.split(QChar('\n'));
    FileData.author1 = (credits.size()>0) ? credits[0] : "";
    FileData.author2 = (credits.size()>1) ? credits[1] : "";
    FileData.author3 = (credits.size()>2) ? credits[2] : "";
    FileData.author4 = (credits.size()>3) ? credits[3] : "";
    FileData.author5 = (credits.size()>4) ? credits[4] : "";

    TextData += SMBX64::qStrS(FileData.author1);
    TextData += SMBX64::qStrS(FileData.author2);
    TextData += SMBX64::qStrS(FileData.author3);
    TextData += SMBX64::qStrS(FileData.author4);
    TextData += SMBX64::qStrS(FileData.author5);

    for(i=0;i<FileData.tiles.size();i++)
    {
        TextData += SMBX64::IntS(FileData.tiles[i].x);
        TextData += SMBX64::IntS(FileData.tiles[i].y);
        TextData += SMBX64::IntS(FileData.tiles[i].id);
    }
    TextData += "\"next\"\n";//Separator

    for(i=0;i<FileData.scenery.size();i++)
    {
        TextData += SMBX64::IntS(FileData.scenery[i].x);
        TextData += SMBX64::IntS(FileData.scenery[i].y);
        TextData += SMBX64::IntS(FileData.scenery[i].id);
    }
    TextData += "\"next\"\n";//Separator

    for(i=0;i<FileData.paths.size();i++)
    {
        TextData += SMBX64::IntS(FileData.paths[i].x);
        TextData += SMBX64::IntS(FileData.paths[i].y);
        TextData += SMBX64::IntS(FileData.paths[i].id);
    }
    TextData += "\"next\"\n";//Separator

    for(i=0;i<FileData.levels.size();i++)
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
        TextData += SMBX64::IntS(FileData.levels[i].entertowarp);
        TextData += SMBX64::BoolS(FileData.levels[i].alwaysVisible);
        TextData += SMBX64::BoolS(FileData.levels[i].pathbg);
        TextData += SMBX64::BoolS(FileData.levels[i].gamestart);
        TextData += SMBX64::IntS(FileData.levels[i].gotox);
        TextData += SMBX64::IntS(FileData.levels[i].gotoy);
        TextData += SMBX64::BoolS(FileData.levels[i].bigpathbg);
    }
    TextData += "\"next\"\n";//Separator

    for(i=0;i<FileData.music.size();i++)
    {
        TextData += SMBX64::IntS(FileData.music[i].x);
        TextData += SMBX64::IntS(FileData.music[i].y);
        TextData += SMBX64::IntS(FileData.music[i].id);
    }
    TextData += "\"next\"\n";//Separator


    return TextData;
}
