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

#include <stack>

/*********************************************************************************/
/***************************SMBX64-Specific features******************************/
/*********************************************************************************/

//Built-in order priorities per SMBX-64 BGO's
const int _smbx64_bgo_sort_priorities[190] = {
    44,25,27,25,33,27,27,27,27,27,5,4,13,3,37,2,2,30,30,30,18,18,51,51,51,7,29,29,33,1,
    28,28,33,39,39,24,37,34,1,1,1,1,5,5,56,56,3,48,59,58,58,14,14,14,14,14,14,11,14,4,
    5,34,34,1,6,5,19,55,55,47,47,47,47,47,1,0,1,1,8,33,20,6,7,37,21,37,46,46,31,31,
    31,46,22,38,38,39,5,39,17,47,8,8,23,46,46,50,46,34,34,9,10,10,10,26,11,12,12,12,24,24,
    21,21,21,21,21,22,22,22,41,43,42,11,12,32,32,32,53,53,45,45,46,20,54,21,54,22,22,23,23,19,
    20,24,9,52,52,52,52,5,5,8,49,20,40,7,6,6,7,7,6,16,16,5,36,35,35,35,35,35,35,35,
    35,35,35,35,15,9,57,57,13,13
};

void FileFormats::smbx64LevelPrepare(LevelData &lvl)
{
    //Set SMBX64 specific option to BGO
    for(int q=0; q< (signed)lvl.bgo.size(); q++)
    {
        if(lvl.bgo[q].smbx64_sp < 0)
        {
            if( (lvl.bgo[q].id>0) && (lvl.bgo[q].id < (unsigned)190) )
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
    lvl.stars = 0;
    for(int q=0; q< (signed)lvl.npc.size(); q++)
    {
        lvl.npc[q].is_star = ((lvl.npc[q].id==97)||(lvl.npc[q].id==196));
        if((lvl.npc[q].is_star) && (lvl.npc[q].friendly))
        {
            lvl.npc[q].is_star=false;
        } else {
            lvl.stars+=1;
        }
    }
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
                           ( ((lvl.blocks[R].x == piv.x) && (lvl.blocks[R].y > piv.y))||
                              ((lvl.blocks[R].y == piv.y) && (lvl.blocks[R].array_id >= piv.array_id)) )
                        ) && (L<R) ) R--;
                if (L<R) lvl.blocks[L++]=lvl.blocks[R];

                while (
                       (
                          (lvl.blocks[R].x < piv.x)||
                          ( ((lvl.blocks[R].x == piv.x) && (lvl.blocks[R].y < piv.y))||
                             ((lvl.blocks[R].y == piv.y) && (lvl.blocks[R].array_id <= piv.array_id)) )
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
                            (((lvl.bgo[R].smbx64_sp_apply == piv.smbx64_sp_apply)&&(lvl.bgo[R].x > piv.x))||
                            ( ((lvl.bgo[R].x == piv.x) && (lvl.bgo[R].y > piv.y))||
                               ((lvl.bgo[R].y == piv.y) && (lvl.bgo[R].array_id >= piv.array_id)) ))
                         ) && (L<R) ) R--;
                if (L<R) lvl.bgo[L++]=lvl.bgo[R];

                while (
                       (
                           (lvl.bgo[R].smbx64_sp_apply < piv.smbx64_sp_apply)||
                           (((lvl.bgo[R].smbx64_sp_apply == piv.smbx64_sp_apply)&&(lvl.bgo[R].x < piv.x))||
                           ( ((lvl.bgo[R].x == piv.x) && (lvl.bgo[R].y < piv.y))||
                              ((lvl.bgo[R].y == piv.y) && (lvl.bgo[R].array_id <= piv.array_id)) ))
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



//*********************************************************
//*******************Dummy arrays**************************
//*********************************************************

//Default dataSets
LevelNPC    FileFormats::CreateLvlNpc()
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
    dummyDoor.layer = "Default";
    dummyDoor.unknown = false;
    dummyDoor.novehicles = false;
    dummyDoor.allownpc = false;
    dummyDoor.locked = false;

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
    dummyWater.quicksand = false;
    dummyWater.layer = "Default";

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

    dummyEvent.layers_hide.clear();
    dummyEvent.layers_show.clear();
    dummyEvent.layers_toggle.clear();

    LevelEvent_Sets events_sets;
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
    dummySection.IsWarp=false;
    dummySection.OffScreenEn=false;
    dummySection.background=0;
    dummySection.lock_left_scroll=false;
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

LevelData FileFormats::CreateLevelData()
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
        section = CreateLvlSection();
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
