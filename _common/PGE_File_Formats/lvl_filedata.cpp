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

#include "file_formats.h"
#include "lvl_filedata.h"

#include <stack>

/*********************************************************************************/
/***************************SMBX64-Specific features******************************/
/*********************************************************************************/

//Built-in order priorities per SMBX-64 BGO's
const int _smbx64_bgo_sort_priorities[190] = {
    77,75,75,75,75,75,75,75,75,75,20,20,75,10,75,75,75,75,75,75,75,75,125,125,125,26,
    75,75,75,75,75,75,75,75,75,75,75,75,75,75,75,75,75,75,125,125,75,80,125,125,125,30,
    75,75,75,75,75,75,75,20,20,75,75,75,26,25,75,125,125,90,90,90,90,90,10,10,10,10,30,
    75,75,26,26,75,75,75,98,98,75,75,75,98,75,75,75,75,75,75,99,75,75,75,75,98,98,125,
    98,75,75,75,75,75,75,75,75,75,75,75,75,75,75,75,75,75,75,75,75,75,76,76,76,75,75,
    75,75,75,125,125,80,80,90,75,125,75,125,75,75,75,75,75,75,75,75,125,125,125,125,25,
    25,75,75,75,75,26,26,26,26,26,26,75,75,25,75,75,75,75,75,75,75,75,75,75,75,75,75,
    75,125,125,75,75
};

void FileFormats::smbx64LevelPrepare(LevelData &lvl)
{
    //Set SMBX64 specific option to BGO
    for(int q=0; q< (signed)lvl.bgo.size(); q++)
    {
        if(lvl.bgo[q].smbx64_sp < 0)
        {
            if( (lvl.bgo[q].id>0) && (lvl.bgo[q].id <= (unsigned)190) )
            {
                lvl.bgo[q].smbx64_sp_apply = _smbx64_bgo_sort_priorities[lvl.bgo[q].id-1];
            }
        }
        else
        {
            lvl.bgo[q].smbx64_sp_apply = lvl.bgo[q].smbx64_sp;
        }
    }

    //Mark & Count Stars
    lvl.stars = smbx64CountStars(lvl);
}

int FileFormats::smbx64CountStars(LevelData &lvl)
{
    int stars = 0;
    for(int q=0; q<(signed)lvl.npc.size(); q++)
    {
        LevelNPC &npc = lvl.npc[q];
        npc.is_star = ( (npc.id == 97) || (npc.id == 196) ) && !npc.friendly;
        if( npc.is_star )
        {
            stars += 1;
        }
    }
    return stars;
}

void FileFormats::smbx64LevelSortBlocks(LevelData &lvl)
{
    if(lvl.blocks.size()<=1) return; //Nothing to sort!

    class my_stack : public std::stack< int > {
    public:
        using std::stack<int>::c; // expose the container
    };

    my_stack beg;
    my_stack end;
    LevelBlock piv;
    int i=0, L, R, swapv;
    beg.push(0);
    end.push(lvl.blocks.size());
    while (i>=0)
    {
        L=beg.c[i]; R=end.c[i]-1;
        if (L<R)
        {
            piv=lvl.blocks[L];
            while (L<R)
            {
                while ( (
                           (lvl.blocks[R].x > piv.x)||
                           ((lvl.blocks[R].x == piv.x) && (lvl.blocks[R].y > piv.y))||
                           ((lvl.blocks[R].x == piv.x) && (lvl.blocks[R].y == piv.y) && (lvl.blocks[R].array_id >= piv.array_id))
                        ) && (L<R) ) R--;
                if (L<R) lvl.blocks[L++]=lvl.blocks[R];

                while (
                       (
                          (lvl.blocks[L].x < piv.x)||
                          ((lvl.blocks[L].x == piv.x) && (lvl.blocks[L].y < piv.y))||
                          ((lvl.blocks[L].x == piv.x) && (lvl.blocks[L].y == piv.y) && (lvl.blocks[L].array_id <= piv.array_id) )
                       ) && (L<R)) L++;
                if (L<R) lvl.blocks[R--]=lvl.blocks[L];
            }
            lvl.blocks[L]=piv; beg.push(L+1); end.push(end.c[i]); end.c[i++]=(L);
            if((end.c[i]-beg.c[i]) > (end.c[i-1]-beg.c[i-1]))
            {
                swapv=beg.c[i]; beg.c[i]=beg.c[i-1]; beg.c[i-1]=swapv;
                swapv=end.c[i]; end.c[i]=end.c[i-1]; end.c[i-1]=swapv;
            }
        }
        else
        {
            i--;
            beg.pop();
            end.pop();
        }
    }
}

void FileFormats::smbx64LevelSortBGOs(LevelData &lvl)
{
    if(lvl.bgo.size()<=1) return; //Nothing to sort!

    class my_stack : public std::stack< int > {
    public:
        using std::stack<int>::c; // expose the container
    };

    my_stack beg;
    my_stack end;
    LevelBGO piv;
    int i=0, L, R, swapv;
    beg.push(0);
    end.push(lvl.bgo.size());
    while (i>=0)
    {
        L=beg.c[i]; R=end.c[i]-1;
        if (L<R)
        {
            piv=lvl.bgo[L];
            while (L<R)
            {
                while ( (
                            (lvl.bgo[R].smbx64_sp_apply > piv.smbx64_sp_apply)||
                          /*((lvl.bgo[R].smbx64_sp_apply == piv.smbx64_sp_apply) && (lvl.bgo[R].x > piv.x))||
                            ((lvl.bgo[R].smbx64_sp_apply == piv.smbx64_sp_apply) && (lvl.bgo[R].x == piv.x) && (lvl.bgo[R].y > piv.y))||
                            ((lvl.bgo[R].smbx64_sp_apply == piv.smbx64_sp_apply) && (lvl.bgo[R].x == piv.x) && (lvl.bgo[R].y == piv.y) && (lvl.bgo[R].array_id >= piv.array_id))*/
                            ((lvl.bgo[R].smbx64_sp_apply == piv.smbx64_sp_apply) && (lvl.bgo[R].array_id >= piv.array_id))
                         ) && (L<R) ) R--;
                if (L<R) lvl.bgo[L++]=lvl.bgo[R];

                while (
                       (
                           (lvl.bgo[L].smbx64_sp_apply < piv.smbx64_sp_apply)||
                         /*((lvl.bgo[L].smbx64_sp_apply == piv.smbx64_sp_apply) && (lvl.bgo[L].x < piv.x))||
                           ((lvl.bgo[L].smbx64_sp_apply == piv.smbx64_sp_apply) && (lvl.bgo[L].x == piv.x) && (lvl.bgo[L].y < piv.y))||
                           ((lvl.bgo[L].smbx64_sp_apply == piv.smbx64_sp_apply) && (lvl.bgo[L].x == piv.x) && (lvl.bgo[L].y == piv.y) && (lvl.bgo[L].array_id <= piv.array_id))*/
                           ((lvl.bgo[L].smbx64_sp_apply == piv.smbx64_sp_apply) && (lvl.bgo[L].array_id <= piv.array_id))
                        ) && (L<R)) L++;
                if (L<R) lvl.bgo[R--]=lvl.bgo[L];
            }
            lvl.bgo[L]=piv; beg.push(L+1); end.push(end.c[i]); end.c[i++]=(L);
            if((end.c[i]-beg.c[i]) > (end.c[i-1]-beg.c[i-1]))
            {
                swapv=beg.c[i]; beg.c[i]=beg.c[i-1]; beg.c[i-1]=swapv;
                swapv=end.c[i]; end.c[i]=end.c[i-1]; end.c[i-1]=swapv;
            }
        }
        else
        {
            i--;
            beg.pop();
            end.pop();
        }
    }
}

int FileFormats::smbx64LevelCheckLimits(LevelData &lvl)
{
    int errorCode=SMBX64_FINE;
    //Sections limit
    if(lvl.sections.size()>21) errorCode|=SMBX64EXC_SECTIONS;
    //Blocks limit
    if(lvl.blocks.size()>16384) errorCode|=SMBX64EXC_BLOCKS;
    //BGO limits
    if(lvl.bgo.size()>8000) errorCode|=SMBX64EXC_BGOS;
    //NPC limits
    if(lvl.npc.size()>5000) errorCode|=SMBX64EXC_NPCS;
    //Warps limits
    if(lvl.doors.size()>199) errorCode|=SMBX64EXC_WARPS;
    //Physical Environment zones
    if(lvl.physez.size()>450) errorCode|=SMBX64EXC_WATERBOXES;
    //Layers limits
    if(lvl.layers.size()>100) errorCode|=SMBX64EXC_LAYERS;
    //Events limits
    if(lvl.events.size()>100) errorCode|=SMBX64EXC_EVENTS;

    return errorCode;
}
/*********************************************************************************/



//********************************************************************
//*******************Structure initializators*************************
//********************************************************************

//Default dataSets
LevelNPC    FileFormats::CreateLvlNpc()
{
    LevelNPC dummyNPC;
    dummyNPC.x = 0;
    dummyNPC.y = 0 ;
    dummyNPC.direct = -1;
    dummyNPC.id=0;
    dummyNPC.contents=0;
    dummyNPC.special_data=0;
    dummyNPC.special_data2=0;
    dummyNPC.generator=false;
    dummyNPC.generator_type=1;
    dummyNPC.generator_direct=1;
    dummyNPC.generator_period=20;
    dummyNPC.generator_custom_angle=0.0;
    dummyNPC.generator_branches=1;
    dummyNPC.generator_angle_range=360.0;
    dummyNPC.generator_initial_speed=10.0;
    dummyNPC.msg="";
    dummyNPC.friendly=false;
    dummyNPC.nomove=false;
    dummyNPC.is_boss=false;
    dummyNPC.layer = "Default";
    dummyNPC.event_activate="";
    dummyNPC.event_die="";
    dummyNPC.event_talk="";
    dummyNPC.event_emptylayer="";
    dummyNPC.attach_layer="";

    dummyNPC.array_id=0;
    dummyNPC.index=0;
    dummyNPC.is_star=false;
    return dummyNPC;
}

LevelDoor  FileFormats::CreateLvlWarp()
{
    LevelDoor dummyDoor;

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
    dummyDoor.stars_msg = "";
    dummyDoor.star_num_hide = false;
    dummyDoor.layer = "Default";
    dummyDoor.unknown = false;
    dummyDoor.novehicles = false;
    dummyDoor.allownpc = false;
    dummyDoor.locked = false;
    dummyDoor.need_a_bomb = false;
    dummyDoor.hide_entering_scene = false;
    dummyDoor.allownpc_interlevel = false;
    dummyDoor.special_state_required = false;
    dummyDoor.length_i = 32;
    dummyDoor.length_o = 32;
    dummyDoor.event_enter = "";
    dummyDoor.two_way=false;
    dummyDoor.cannon_exit=false;
    dummyDoor.cannon_exit_speed=10.0f;

    dummyDoor.array_id = 0;
    dummyDoor.index = 0;

    return dummyDoor;
}


LevelBlock  FileFormats::CreateLvlBlock()
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
    dummyBlock.event_emptylayer = "";

    dummyBlock.array_id = 0;
    dummyBlock.index = 0;
    return dummyBlock;
}

LevelBGO FileFormats::CreateLvlBgo()
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


LevelPhysEnv FileFormats::CreateLvlPhysEnv()
{
    LevelPhysEnv dummyWater;
    dummyWater.x  = 0;
    dummyWater.y = 0;
    dummyWater.w = 0;
    dummyWater.h = 0;
    dummyWater.unknown = 0;
    dummyWater.env_type = LevelPhysEnv::ENV_WATER;
    dummyWater.layer = "Default";
    dummyWater.friction = 0.5;
    dummyWater.accel_direct=-1;
    dummyWater.accel = 0.0;
    dummyWater.max_velocity = 0.0;
    dummyWater.touch_event = "";

    dummyWater.array_id = 0;
    dummyWater.index = 0;
    return dummyWater;
}

LevelSMBX64Event FileFormats::CreateLvlEvent()
{
    LevelSMBX64Event dummyEvent;

    dummyEvent.name="";
    dummyEvent.msg="";
    dummyEvent.sound_id=0;
    dummyEvent.end_game=0;
    dummyEvent.trigger="";
    dummyEvent.trigger_timer=0;
    dummyEvent.nosmoke=false;
    dummyEvent.ctrls_enable=false;
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
    dummyEvent.ctrl_lock_keyboard=false;
    dummyEvent.autostart = LevelSMBX64Event::AUTO_None;
    dummyEvent.autostart_condition="";
    dummyEvent.movelayer = "";
    dummyEvent.layer_speed_x=0.0;
    dummyEvent.layer_speed_y=0.0;
    dummyEvent.move_camera_x=0.0;
    dummyEvent.move_camera_y=0.0;
    dummyEvent.scroll_section=0;
    dummyEvent.trigger_api_id=0;
    dummyEvent.layers_hide.clear();
    dummyEvent.layers_show.clear();
    dummyEvent.layers_toggle.clear();

    LevelEvent_Sets events_sets;
    dummyEvent.sets.clear();
    for(int j=0; j< 21; j++)
    {
        events_sets.id = j;
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

LevelVariable FileFormats::CreateLvlVariable(PGESTRING vname)
{
    LevelVariable var;
    var.name=vname;
    var.value="";
    return var;
}

LevelScript FileFormats::CreateLvlScript(PGESTRING name, int lang)
{
    LevelScript scr;
    scr.language = lang;
    scr.name = name;
    scr.script = "";
    return scr;
}

LevelSection FileFormats::CreateLvlSection()
{
    LevelSection dummySection;
    dummySection.id = 0;
    dummySection.size_top=0;
    dummySection.size_bottom=0;
    dummySection.size_left=0;
    dummySection.size_right=0;
    dummySection.music_id=0;
    dummySection.bgcolor=16291944;
    dummySection.wrap_h=false;
    dummySection.wrap_v=false;
    dummySection.OffScreenEn=false;
    dummySection.background=0;
    dummySection.lock_left_scroll=false;
    dummySection.lock_right_scroll=false;
    dummySection.lock_up_scroll=false;
    dummySection.lock_down_scroll=false;
    dummySection.underwater=false;
    dummySection.music_file="";
    dummySection.PositionX = -10;
    dummySection.PositionY = -10;

    return dummySection;
}

LevelLayer FileFormats::CreateLvlLayer()
{
    LevelLayer dummyLayer;
    dummyLayer.array_id = 0;
    dummyLayer.name = "";
    dummyLayer.hidden = false;
    dummyLayer.locked = false;
    return dummyLayer;
}

PlayerPoint FileFormats::CreateLvlPlayerPoint(int id)
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

void FileFormats::LevelAddInternalEvents(LevelData &FileData)
{
    LevelLayer layers;
    LevelSMBX64Event events;

    layers= CreateLvlLayer();

    //Append system layers if not exist
    bool def=false,desb=false,spawned=false;

    for(int lrID=0; lrID<(signed)FileData.layers.size();lrID++)
    {
        LevelLayer &lr=FileData.layers[lrID];
        if(lr.name=="Default") def=true;
        else
        if(lr.name=="Destroyed Blocks") desb=true;
        else
        if(lr.name=="Spawned NPCs") spawned=true;
    }

    if(!def)
    {
        layers.hidden = false;
        layers.name = "Default";
        FileData.layers.push_back(layers);
    }
    if(!desb)
    {
        layers.hidden = true;
        layers.name = "Destroyed Blocks";
        FileData.layers.push_back(layers);
    }
    if(!spawned)
    {
        layers.hidden = false;
        layers.name = "Spawned NPCs";
        FileData.layers.push_back(layers);
    }

    //Append system events if not exist
    //Level - Start
    //P Switch - Start
    //P Switch - End
    bool lstart=false, pstart=false, pend=false;
    for(int evID=0; evID<(signed)FileData.events.size(); evID++)
    {
        LevelSMBX64Event &ev=FileData.events[evID];
        if(ev.name=="Level - Start") lstart=true;
        else
        if(ev.name=="P Switch - Start") pstart=true;
        else
        if(ev.name=="P Switch - End") pend=true;
    }

    events = CreateLvlEvent();

    if(!lstart)
    {
        events.array_id = FileData.events_array_id;
        FileData.events_array_id++;

        events.name = "Level - Start";
        FileData.events.push_back(events);
    }
    if(!pstart)
    {
        events.array_id = FileData.events_array_id;
        FileData.events_array_id++;

        events.name = "P Switch - Start";
        FileData.events.push_back(events);
    }
    if(!pend)
    {
        events.array_id = FileData.events_array_id;
        FileData.events_array_id++;

        events.name = "P Switch - End";
        FileData.events.push_back(events);
    }
}


void FileFormats::CreateLevelHeader(LevelData &NewFileData)
{
    NewFileData.ReadFileValid = true;
    NewFileData.modified = true;
    NewFileData.untitled = true;
    NewFileData.smbx64strict = false;

    NewFileData.RecentFormat = LevelData::PGEX;
    NewFileData.RecentFormatVersion = 64;

    NewFileData.ERROR_info = "";
    NewFileData.ERROR_linedata = "";
    NewFileData.ERROR_linenum = -1;

    NewFileData.CurSection=0;
    NewFileData.playmusic=0;
    NewFileData.filename = "";
    NewFileData.path = "";

    NewFileData.open_level_on_fail = "";
    NewFileData.open_level_on_fail_warpID = 0;

    NewFileData.LevelName = "";
    NewFileData.stars = 0;

    //Meta-data
    #ifdef PGE_EDITOR
    NewFileData.metaData.script = NULL;
    #endif
    NewFileData.metaData.ReadFileValid=true;
    NewFileData.metaData.ERROR_info="";
    NewFileData.metaData.ERROR_linedata="";
    NewFileData.metaData.ERROR_linenum=-1;
}

void FileFormats::CreateLevelData(LevelData &NewFileData)
{
    CreateLevelHeader(NewFileData);

    NewFileData.bgo_array_id = 1;
    NewFileData.blocks_array_id = 1;
    NewFileData.doors_array_id = 1;
    NewFileData.events_array_id = 1;
    NewFileData.layers_array_id = 1;
    NewFileData.npc_array_id = 1;
    NewFileData.physenv_array_id = 1;

    NewFileData.sections.clear();
    //Create Section array
    LevelSection section;
    for(int i=0; i<21;i++)
    {
        section = CreateLvlSection();
        section.id = i;
        NewFileData.sections.push_back( section );
    }

    NewFileData.players.clear();
    //Create players array
    //PlayerPoint players = dummyLvlPlayerPoint();
    //    for(int i=0; i<2;i++)
    //    {
    //        players.id++;
    //        NewFileData.players.push_back(players);
    //    }

    NewFileData.blocks.clear();
    NewFileData.bgo.clear();
    NewFileData.npc.clear();
    NewFileData.doors.clear();
    NewFileData.physez.clear();

    NewFileData.layers.clear();

    //Create system layers
        //Default
        //Destroyed Blocks
        //Spawned NPCs

    LevelLayer layers;
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

    NewFileData.events.clear();
    //Create system events
        //Level - Start
        //P Switch - Start
        //P Switch - End

    LevelSMBX64Event events = CreateLvlEvent();

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
}

LevelData FileFormats::CreateLevelData()
{
    LevelData NewFileData;
    CreateLevelData(NewFileData);
    return NewFileData;
}



bool LevelData::eventIsExist(PGESTRING title)
{
    for(int i=0; i<(signed)events.size(); i++)
    {
        if( events[i].name == title )
            return true;
    }
    return false;
}

bool LevelData::layerIsExist(PGESTRING title)
{
    for(int i=0; i<(signed)layers.size(); i++)
    {
        if( layers[i].name == title )
            return true;
    }
    return false;
}

bool LevelSMBX64Event::ctrlKeyPressed()
{
    return ctrl_up||
           ctrl_down||
           ctrl_left||
           ctrl_right||
           ctrl_jump||
           ctrl_altjump||
           ctrl_run||
           ctrl_altrun||
           ctrl_drop||
           ctrl_start;
}

LevelEvent_Sets::LevelEvent_Sets()
{
    id            = -1;
    music_id=       LESet_Nothing;
    background_id=  LESet_Nothing;
    position_left=  LESet_Nothing;
    position_top=   0;
    position_bottom=0;
    position_right= 0;
    autoscrol     = false;
    autoscrol_x   = 0.f;
    autoscrol_y   = 0.f;
}

LevelEvent_MoveLayer::LevelEvent_MoveLayer()
{
    name="";
    speed_x=0.0f;
    speed_y=0.0f;
    way=LM_Speed;
}

LevelEvent_SpawnEffect::LevelEvent_SpawnEffect()
{
    id=0;
    x=0;
    y=0;
    speed_x=0.0f;
    speed_y=0.0f;
    gravity=false;
    fps=-1;//Default FPS
    max_life_time=-1;//Default life time
}

LevelEvent_SpawnNPC::LevelEvent_SpawnNPC()
{
    id=0;
    x=0;
    y=0;
    speed_x=0.0;
    speed_y=0.0;
    special=0;
}

LevelEvent_SetTimer::LevelEvent_SetTimer()
{
    enable=false;
    count=0;
    interval=1000.0f;
    count_dir=DIR_REVERSE;
    show=false;
}
