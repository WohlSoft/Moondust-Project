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
#include "lvl_filedata.h"

//*********************************************************
//*******************Dummy arrays**************************
//*********************************************************

//Default dataSets
LevelNPC    FileFormats::dummyLvlNpc()
{
    LevelNPC dummyNPC;
    dummyNPC.x = 0;
    dummyNPC.y = 0 ;
    dummyNPC.direct = -1;
    dummyNPC.id=0;
    dummyNPC.special_data=0;
    dummyNPC.special_data2=0;
    dummyNPC.generator=false;
    dummyNPC.generator_type=1;
    dummyNPC.generator_direct=1;
    dummyNPC.generator_period=20;
    dummyNPC.msg="";
    dummyNPC.friendly=false;
    dummyNPC.nomove=false;
    dummyNPC.legacyboss=false;
    dummyNPC.layer = "Default";
    dummyNPC.event_activate="";
    dummyNPC.event_die="";
    dummyNPC.event_talk="";
    dummyNPC.event_nomore="";
    dummyNPC.attach_layer="";

    dummyNPC.array_id=0;
    dummyNPC.index=0;
    dummyNPC.is_star=false;
    return dummyNPC;
}

LevelDoors  FileFormats::dummyLvlDoor()
{
    LevelDoors dummyDoor;

    dummyDoor.ix = 0;
    dummyDoor.iy = 0;
    dummyDoor.isSetIn = false;
    dummyDoor.ox = 0;
    dummyDoor.oy = 0;
    dummyDoor.isSetOut = false;
    dummyDoor.idirect=1;
    dummyDoor.odirect=1;
    dummyDoor.type=0;
    dummyDoor.lname = "";
    dummyDoor.warpto = 0;
    dummyDoor.lvl_i = false;
    dummyDoor.lvl_o = false;
    dummyDoor.world_x = -1;
    dummyDoor.world_y = -1;
    dummyDoor.stars = 0;
    dummyDoor.layer = "Default";
    dummyDoor.unknown = false;
    dummyDoor.novehicles = false;
    dummyDoor.allownpc = false;
    dummyDoor.locked = false;

    dummyDoor.array_id = 0;
    dummyDoor.index = 0;

    return dummyDoor;
}


LevelBlock  FileFormats::dummyLvlBlock()
{
    LevelBlock dummyBlock;
    dummyBlock.x = 0;
    dummyBlock.y = 0;
    dummyBlock.w = 0;
    dummyBlock.h = 0;
    dummyBlock.id = 0;
    dummyBlock.npc_id = 0;
    dummyBlock.invisible = false;
    dummyBlock.slippery = false;
    dummyBlock.layer = "Default";
    dummyBlock.event_destroy = "";
    dummyBlock.event_hit = "";
    dummyBlock.event_no_more = "";

    dummyBlock.array_id = 0;
    dummyBlock.index = 0;
    return dummyBlock;
}

LevelBGO FileFormats::dummyLvlBgo()
{
    LevelBGO dummyBGO;
    //SMBX64
    dummyBGO.x = 0;
    dummyBGO.y = 0;
    dummyBGO.id = 0;
    dummyBGO.layer = "Default";

    //Extended
    dummyBGO.z_mode = LevelBGO::ZDefault;
    dummyBGO.z_offset = 0;

    dummyBGO.smbx64_sp = -1;
    dummyBGO.smbx64_sp_apply = -1;

    dummyBGO.array_id  = 0;
    dummyBGO.index = 0;
    return dummyBGO;
}


LevelPhysEnv FileFormats::dummyLvlPhysEnv()
{
    LevelPhysEnv dummyWater;
    dummyWater.x  = 0;
    dummyWater.y = 0;
    dummyWater.w = 0;
    dummyWater.h = 0;
    dummyWater.unknown = 0;
    dummyWater.quicksand = false;
    dummyWater.layer = "Default";

    dummyWater.array_id = 0;
    dummyWater.index = 0;
    return dummyWater;
}

LevelEvents FileFormats::dummyLvlEvent()
{
    LevelEvents dummyEvent;

    dummyEvent.name="";
    dummyEvent.msg="";
    dummyEvent.sound_id=0;
    dummyEvent.end_game=0;
    dummyEvent.trigger="";
    dummyEvent.trigger_timer=0;
    dummyEvent.nosmoke=false;
    dummyEvent.ctrl_altjump=false;
    dummyEvent.ctrl_altrun=false;
    dummyEvent.ctrl_down=false;
    dummyEvent.ctrl_drop=false;
    dummyEvent.ctrl_jump=false;
    dummyEvent.ctrl_left=false;
    dummyEvent.ctrl_right=false;
    dummyEvent.ctrl_run=false;
    dummyEvent.ctrl_start=false;
    dummyEvent.ctrl_up=false;
    dummyEvent.autostart=false;
    dummyEvent.movelayer="";
    dummyEvent.layer_speed_x=0.0;
    dummyEvent.layer_speed_y=0.0;
    dummyEvent.move_camera_x=0.0;
    dummyEvent.move_camera_y=0.0;
    dummyEvent.scroll_section=0;

    LevelEvents_layers events_layers;
    dummyEvent.layers.clear();
    for(int j=0; j< 21; j++)
    {
        events_layers.hide="";
        events_layers.show="";
        events_layers.toggle="";
        dummyEvent.layers.push_back(events_layers);
    }
    dummyEvent.layers_hide.clear();
    dummyEvent.layers_show.clear();
    dummyEvent.layers_toggle.clear();

    LevelEvents_Sets events_sets;
    dummyEvent.sets.clear();
    for(int j=0; j< 21; j++)
    {
        events_sets.music_id=-1;
        events_sets.background_id=-1;
        events_sets.position_left=-1;
        events_sets.position_top=0;
        events_sets.position_bottom=0;
        events_sets.position_right=0;
        dummyEvent.sets.push_back(events_sets);
    }


    dummyEvent.array_id=0;
    return dummyEvent;
}


LevelSection FileFormats::dummyLvlSection()
{
    LevelSection dummySection;
    dummySection.id = 0;
    dummySection.size_top=0;
    dummySection.size_bottom=0;
    dummySection.size_left=0;
    dummySection.size_right=0;
    dummySection.music_id=0;
    dummySection.bgcolor=16291944;
    dummySection.IsWarp=false;
    dummySection.OffScreenEn=false;
    dummySection.background=0;
    dummySection.noback=false;
    dummySection.underwater=false;
    dummySection.music_file="";
    dummySection.PositionX = -10;
    dummySection.PositionY = -10;

    return dummySection;
}

LevelLayers FileFormats::dummyLvlLayer()
{
    LevelLayers dummyLayer;
    dummyLayer.array_id = 0;
    dummyLayer.name = "";
    dummyLayer.hidden = false;
    dummyLayer.locked = false;
    return dummyLayer;
}

PlayerPoint FileFormats::dummyLvlPlayerPoint(int id)
{
    PlayerPoint dummyPlayer;
    dummyPlayer.id=id;
    dummyPlayer.x=0;
    dummyPlayer.y=0;
    dummyPlayer.w=24;
    switch(id)
    {
    case 1:
        dummyPlayer.h=54;
        break;
    case 2:
        dummyPlayer.h=60;
        break;
    default:
        dummyPlayer.h=32;
    }

    dummyPlayer.direction=1;
    return dummyPlayer;
}

LevelData FileFormats::dummyLvlDataArray()
{
    LevelData NewFileData;

    NewFileData.ReadFileValid = true;
    NewFileData.modified = true;
    NewFileData.untitled = true;
    NewFileData.smbx64strict = false;

    NewFileData.CurSection=0;
    NewFileData.playmusic=0;

    NewFileData.LevelName = "";
    NewFileData.stars = 0;

    NewFileData.bgo_array_id = 1;
    NewFileData.blocks_array_id = 1;
    NewFileData.doors_array_id = 1;
    NewFileData.events_array_id = 1;
    NewFileData.layers_array_id = 1;
    NewFileData.npc_array_id = 1;
    NewFileData.physenv_array_id = 1;

    //Meta-data
    #ifdef PGE_EDITOR
    NewFileData.metaData.script = NULL;
    #endif

    //Create Section array
    LevelSection section;
    for(int i=0; i<21;i++)
    {
        section = dummyLvlSection();
        section.id = i;
        NewFileData.sections.push_back( section );
    }

    //Create players array
    //PlayerPoint players = dummyLvlPlayerPoint();
    //    for(int i=0; i<2;i++)
    //    {
    //        players.id++;
    //        NewFileData.players.push_back(players);
    //    }


    //Create system layers
        //Default
        //Destroyed Blocks
        //Spawned NPCs

    LevelLayers layers;
        layers.hidden = false;
        layers.locked = false;
        layers.name = "Default";
        layers.array_id = NewFileData.layers_array_id++;
        NewFileData.layers.push_back(layers);

        layers.hidden = true;
        layers.locked = false;
        layers.name = "Destroyed Blocks";
        layers.array_id = NewFileData.layers_array_id++;
        NewFileData.layers.push_back(layers);

        layers.hidden = false;
        layers.locked = false;
        layers.name = "Spawned NPCs";
        layers.array_id = NewFileData.layers_array_id++;
        NewFileData.layers.push_back(layers);

    //Create system events
        //Level - Start
        //P Switch - Start
        //P Switch - End

    LevelEvents events = dummyLvlEvent();

        events.array_id = NewFileData.events_array_id;
        NewFileData.events_array_id++;

        events.name = "Level - Start";
        NewFileData.events.push_back(events);

        events.array_id = NewFileData.events_array_id;
        NewFileData.events_array_id++;

        events.name = "P Switch - Start";
        NewFileData.events.push_back(events);

        events.array_id = NewFileData.events_array_id;
        NewFileData.events_array_id++;

        events.name = "P Switch - End";
        NewFileData.events.push_back(events);


return NewFileData;
}



bool LevelData::eventIsExist(QString title)
{
    for(int i=0; i<events.size(); i++)
    {
        if( events[i].name == title )
            return true;
    }
    return false;
}

bool LevelData::layerIsExist(QString title)
{
    for(int i=0; i<layers.size(); i++)
    {
        if( layers[i].name == title )
            return true;
    }
    return false;
}
