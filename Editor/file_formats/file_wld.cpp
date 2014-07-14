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

#include "../mainwindow.h"
#include "wld_filedata.h"

#include "file_formats.h"

//*********************************************************
//*******************Dummy arrays**************************
//*********************************************************

WorldTiles FileFormats::dummyWldTile()
{
    WorldTiles dummyTile;
    dummyTile.array_id=0;
    dummyTile.id = 0;
    dummyTile.x = 0;
    dummyTile.y = 0;
    dummyTile.index = 0;

    return dummyTile;
}

WorldScenery FileFormats::dummyWldScen()
{
    WorldScenery dummyScen;
    dummyScen.array_id = 0;
    dummyScen.id = 0;
    dummyScen.index = 0;
    dummyScen.x = 0;
    dummyScen.y = 0;

    return dummyScen;
}

WorldPaths FileFormats::dummyWldPath()
{
    WorldPaths dummyPath;
    dummyPath.array_id = 0;
    dummyPath.id = 0;
    dummyPath.index = 0;
    dummyPath.x = 0;
    dummyPath.y = 0;

    return dummyPath;
}

WorldLevels FileFormats::dummyWldLevel()
{
    WorldLevels dummyLevel;
    dummyLevel.array_id = 0;
    dummyLevel.id = 0;
    dummyLevel.index = 0;

    dummyLevel.x = 0;
    dummyLevel.y = 0;

    dummyLevel.title = "";


    dummyLevel.alwaysVisible = false;
    dummyLevel.bigpathbg = false;
    dummyLevel.pathbg = false;

    dummyLevel.gamestart = false;

    dummyLevel.gotox = -1;
    dummyLevel.gotoy = -1;

    dummyLevel.lvlfile = "";
    dummyLevel.entertowarp = 0;

    dummyLevel.right_exit = -1;
    dummyLevel.bottom_exit = -1;
    dummyLevel.top_exit = -1;
    dummyLevel.left_exit = -1;

    return dummyLevel;
}

WorldMusic FileFormats::dummyWldMusic()
{
    WorldMusic dummyMusicBox;
    dummyMusicBox.array_id = 0;
    dummyMusicBox.id = 0;
    dummyMusicBox.index = 0;
    dummyMusicBox.x = 0;
    dummyMusicBox.y = 0;

    return dummyMusicBox;
}

WorldData FileFormats::dummyWldDataArray()
{
    WorldData NewFileData;

    NewFileData.ReadFileValid = true;
    NewFileData.modified = true;
    NewFileData.untitled = true;

    NewFileData.CurSection=0;
    NewFileData.playmusic=0;
    NewFileData.currentMusic = 0;

    NewFileData.EpisodeTitle = "";

    NewFileData.nocharacter1 = false;
    NewFileData.nocharacter2 = false;
    NewFileData.nocharacter3 = false;
    NewFileData.nocharacter4 = false;
    NewFileData.nocharacter5 = false;

    NewFileData.author1 = "";
    NewFileData.author2 = "";
    NewFileData.author3 = "";
    NewFileData.author4 = "";
    NewFileData.author5 = "";

    NewFileData.autolevel = "";

    NewFileData.noworldmap = false;
    NewFileData.restartlevel = false;

    NewFileData.tile_array_id = 0;
    NewFileData.scene_array_id = 0;
    NewFileData.path_array_id = 0;
    NewFileData.level_array_id = 0;
    NewFileData.musicbox_array_id = 0;

    NewFileData.stars = 0;

    return NewFileData;
}


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
    }

    if(file_format >= 10)
    {
        str_count++;line = in.readLine();
        if( SMBX64::qStr(line) ) //Autostart level
            goto badfile;
        else FileData.autolevel = removeQuotes(line);

        str_count++;line = in.readLine();
        if( SMBX64::wBool(line) ) //Don't use world map on this episode
            goto badfile;
        else FileData.noworldmap = SMBX64::wBoolR(line);

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

