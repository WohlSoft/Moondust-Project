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

#include "file_formats.h"
#include "wld_filedata.h"


bool FileFormats::smbx64WorldCheckLimits(WorldData &wld, PGESTRING *message)
{
    bool isSMBX64limit=true;

    //Tiles limit
    if(wld.tiles.size()>20000)
    {
        if(message)
        {
            *message+=
            "SMBX64 standard isn't allows to save "+fromNum(wld.tiles.size())+" Tiles\n"
            "The maximum number of Tiles is "+fromNum(20000)+".\n\n";
        }
        isSMBX64limit=false;
    }
    //Sceneries limit
    if(wld.scenery.size()>5000)
    {
        if(message)
        {
            *message+=
            "SMBX64 standard isn't allows to save "+fromNum(wld.scenery.size())+" Sceneries\n"
            "The maximum number of Sceneries is "+fromNum(5000)+".\n\n";
        }
        isSMBX64limit=false;
    }
    //Paths limit
    if(wld.paths.size()>2000)
    {
        if(message)
        {
            *message+=
            "SMBX64 standard isn't allows to save "+fromNum(wld.paths.size())+" Paths\n"
            "The maximum number of Paths is "+fromNum(2000)+".\n\n";
        }
        isSMBX64limit=false;
    }
    //Levels limit
    if(wld.levels.size()>400)
    {
        if(message)
        {
            *message+=
            "SMBX64 standard isn't allows to save "+fromNum(wld.levels.size())+" Levels\n"
            "The maximum number of Levels is "+fromNum(400)+".\n\n";
        }
        isSMBX64limit=false;
    }

    //Music boxes limit
    if(wld.music.size()>1000)
    {
        if(message)
        {
            *message+=
            "SMBX64 standard isn't allows to save "+fromNum(wld.music.size())+" Music Boxes\n"
            "The maximum number of Music Boxes is "+fromNum(1000)+".\n\n";
        }
        isSMBX64limit=false;
    }

    //Append common message part
    if(!isSMBX64limit && message)
        *message+="Please remove excess elements from this world map or save file into WLDX format.";

    return isSMBX64limit;
}



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
    dummyMusicBox.music_file="";

    return dummyMusicBox;
}

WorldData FileFormats::dummyWldDataArray()
{
    WorldData NewFileData;

    NewFileData.ReadFileValid = true;
    NewFileData.modified = true;
    NewFileData.untitled = true;
    NewFileData.smbx64strict = false;

    NewFileData.CurSection=0;
    NewFileData.playmusic=0;
    NewFileData.currentMusic = 0;

    NewFileData.EpisodeTitle = "";

    NewFileData.nocharacter1 = false;
    NewFileData.nocharacter2 = false;
    NewFileData.nocharacter3 = false;
    NewFileData.nocharacter4 = false;
    NewFileData.nocharacter5 = false;

    NewFileData.authors = "";
    NewFileData.author1 = "";
    NewFileData.author2 = "";
    NewFileData.author3 = "";
    NewFileData.author4 = "";
    NewFileData.author5 = "";

    NewFileData.IntroLevel_file = "";

    NewFileData.HubStyledWorld = false;
    NewFileData.restartlevel = false;

    NewFileData.tile_array_id = 0;
    NewFileData.scene_array_id = 0;
    NewFileData.path_array_id = 0;
    NewFileData.level_array_id = 0;
    NewFileData.musicbox_array_id = 0;

    NewFileData.stars = 0;

    //Meta-data
    #ifdef PGE_EDITOR
    NewFileData.metaData.script = NULL;
    #endif

    return NewFileData;
}

