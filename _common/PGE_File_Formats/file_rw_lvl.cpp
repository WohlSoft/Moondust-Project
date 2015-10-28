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
#include "file_strlist.h"
#include "smbx64.h"
#include "smbx64_macro.h"

//*********************************************************
//****************READ FILE FORMAT*************************
//*********************************************************
struct LevelEvent_layers
{
    PGESTRING hide;
    PGESTRING show;
    PGESTRING toggle;
};

LevelData FileFormats::ReadSMBX64LvlFileHeader(PGESTRING filePath)
{
    errorString.clear();
    LevelData FileData;
    FileData = CreateLevelData();

    PGE_FileFormats_misc::TextFileInput inf;
    if(!inf.open(filePath, false))
    {
        FileData.ReadFileValid=false;
        return FileData;
    }
    PGE_FileFormats_misc::FileInfo in_1(filePath);
    FileData.filename = in_1.basename();
    FileData.path = in_1.dirpath();

    inf.seek(0, PGE_FileFormats_misc::TextFileInput::begin);
    SMBX64_FileBegin();

    #define nextLineH() str_count++;line = inf.readLine();

    nextLineH();   //Read first line
    if( SMBX64::uInt(line) ) //File format number
        goto badfile;

    else file_format=toInt(line);

    if(file_format >= 17)
    {
        nextLineH();   //Read second Line
        if( SMBX64::uInt(line) ) //File format number
            goto badfile;
        else FileData.stars=toInt(line);   //Number of stars
    } else FileData.stars=0;

    if(file_format >= 60)
    {
        nextLineH();   //Read third line
        if( SMBX64::qStr(line) ) //LevelTitle
            goto badfile;
        else FileData.LevelName = removeQuotes(line); //remove quotes
    } else FileData.LevelName="";

    FileData.CurSection=0;
    FileData.playmusic=0;

    FileData.ReadFileValid=true;

    return FileData;
badfile:
    FileData.ReadFileValid=false;
    FileData.ERROR_info="Invalid file format, detected file SMBX-"+fromNum(file_format)+"format";
    FileData.ERROR_linenum=str_count;
    FileData.ERROR_linedata=line;
    return FileData;
}

LevelData FileFormats::ReadSMBX64LvlFile(PGESTRING RawData, PGESTRING filePath)
{
    errorString.clear();
    SMBX64_File( RawData );

    int i;                  //counters
    LevelData FileData;
        FileData.LevelName="";
        FileData.stars=0;
        FileData.CurSection=0;
        FileData.playmusic=0;

        //Enable strict mode for SMBX LVL file format
        FileData.smbx64strict = true;

        //Begin all ArrayID's here;
        FileData.blocks_array_id = 1;
        FileData.bgo_array_id = 1;
        FileData.npc_array_id = 1;
        FileData.doors_array_id = 1;
        FileData.physenv_array_id = 1;
        FileData.layers_array_id = 1;
        FileData.events_array_id = 1;

    LevelSection section;
    int sct;
    PlayerPoint players;
    LevelBlock blocks;
    LevelBGO bgodata;
    LevelNPC npcdata;
    LevelDoor doors;
    LevelPhysEnv waters;
    LevelLayer layers;
    LevelSMBX64Event events;
    LevelEvent_layers events_layers;
    LevelEvent_Sets events_sets;

    //Add path data
    if(!filePath.PGESTRINGisEmpty())
    {
        PGE_FileFormats_misc::FileInfo in_1(filePath);
        FileData.filename = in_1.basename();
        FileData.path = in_1.dirpath();
    }

    #ifdef PGE_EDITOR
    FileData.metaData.script = NULL; //set NULL to pointers in the Meta
    #endif

    ///////////////////////////////////////Begin file///////////////////////////////////////
    nextLine();   //Read first line
    UIntVar(file_format, line);//File format number

    if(ge(17))
    {
        nextLine(); UIntVar(FileData.stars, line); //Number of stars
    } else FileData.stars=0;

    if(ge(60)) { nextLine(); strVar(FileData.LevelName, line);} //LevelTitle

    //total sections
    sct = (ge(8) ? 21 : 6);


    ////////////SECTION Data//////////
    for(i=0;i<sct;i++)
    {
        section = CreateLvlSection();

        nextLine(); SIntVar(section.size_left, line); section.PositionX=toInt(line)-10; //left
        nextLine(); SIntVar(section.size_top,  line); section.PositionY=toInt(line)-10; //top
        nextLine(); SIntVar(section.size_bottom, line); //bottom
        nextLine(); SIntVar(section.size_right, line);  //right

        nextLine(); UIntVar(section.music_id, line);    //Music ID
        nextLine(); UIntVar(section.bgcolor, line);     //BG Color
        nextLine(); wBoolVar(section.wrap_h, line);     //Connect sides of section
        nextLine(); wBoolVar(section.OffScreenEn, line);//Offscreen exit
        nextLine(); UIntVar(section.background, line);  //BackGround id
        if(ge(1)) {nextLine(); wBoolVar(section.lock_left_scroll, line);} //Don't walk to left (no turn back)
        if(ge(30)){nextLine(); wBoolVar(section.underwater, line);}//Underwater
        if(ge(2)) {nextLine(); strVar(section.music_file, line);}//Custom Music

        section.id = i;
    FileData.sections.push_back(section); //Add Section in main array
    }

    if(lt(8))
        for(;i<21;i++) { section = CreateLvlSection(); section.id=i;
                            FileData.sections.push_back(section); }


    //Player's point config
    for(i=0;i<2;i++)
    {
        players=CreateLvlPlayerPoint();

        nextLine(); SIntVar(players.x, line);//Player x
        nextLine(); SIntVar(players.y, line);//Player y
        nextLine(); UIntVar(players.w, line);//Player w
        nextLine(); UIntVar(players.h, line);//Player h

        players.id = i+1;

        if(players.x!=0 && players.y!=0 && players.w !=0 && players.h != 0) //Don't add into array non-exist point
            FileData.players.push_back(players);    //Add player in array
    }

    ////////////Block Data//////////
    nextLine();
    while(line!="\"next\"")
    {
        blocks = CreateLvlBlock();

                    SIntVar(blocks.x, line);
        nextLine(); SIntVar(blocks.y, line);
        nextLine(); UIntVar(blocks.h, line);
        nextLine(); UIntVar(blocks.w, line);
        nextLine(); UIntVar(blocks.id, line);

        long xnpcID;
        nextLine(); UIntVar(xnpcID, line); //Containing NPC id
        {
            //Convert NPC-ID value from SMBX1/2 to SMBX64
            switch(xnpcID)
            {
                case 100: xnpcID = 1009; break;//Mushroom
                case 101: xnpcID = 1001; break;//Goomba
                case 102: xnpcID = 1014; break;//Fire flower
                case 103: xnpcID = 1034; break;//Super leaf
                case 104: xnpcID = 1035; break;//Shoe
                default:break;
            }

            //Convert NPC-ID value from SMBX64 into PGE format
            if(xnpcID != 0)
            {
                if(xnpcID > 1000)
                    xnpcID = xnpcID-1000;
                else
                    xnpcID *= -1;
            }
            blocks.npc_id = xnpcID;
        }

        nextLine(); wBoolVar(blocks.invisible, line);
        if(ge(61)) { nextLine(); wBoolVar(blocks.slippery, line); }
        if(ge(10)) { nextLine(); strVar(blocks.layer, line); }
        if(ge(14)) {
            nextLine(); strVar(blocks.event_destroy, line);
            nextLine(); strVar(blocks.event_hit, line);
            nextLine(); strVar(blocks.event_emptylayer, line); }

        blocks.array_id = FileData.blocks_array_id;
        FileData.blocks_array_id++;
        blocks.index = FileData.blocks.size(); //Apply element index
    FileData.blocks.push_back(blocks); //AddBlock into array

    nextLine();
    }


    ////////////BGO Data//////////
    nextLine();
    while(line!="\"next\"")
    {
        bgodata = CreateLvlBgo();

                    SIntVar(bgodata.x, line);
        nextLine(); SIntVar(bgodata.y, line);
        nextLine(); UIntVar(bgodata.id, line);
        if(ge(10)) { nextLine(); strVar(bgodata.layer, line);}
            bgodata.smbx64_sp = -1;

        if( (file_format < 10) && (bgodata.id==65) ) //set foreground for BGO-65 (SMBX 1.0)
        {
            bgodata.z_mode = LevelBGO::Foreground1;
            bgodata.smbx64_sp = 80;
        }

        bgodata.array_id = FileData.bgo_array_id;
        FileData.bgo_array_id++;

        bgodata.index = FileData.bgo.size(); //Apply element index

    FileData.bgo.push_back(bgodata); //Add Background object into array

    nextLine();
    }


    ////////////NPC Data//////////
     nextLine();
     while(line!="\"next\"")
     {
         npcdata = CreateLvlNpc();


                     parseLine( SMBX64::sFloat(line), npcdata.x, round(toDouble(line)));
         nextLine(); parseLine( SMBX64::sFloat(line), npcdata.y, round(toDouble(line)));
         nextLine(); SIntVar(npcdata.direct, line); //NPC direction
         nextLine(); UIntVar(npcdata.id, line); //NPC id

         npcdata.special_data = 0;
         switch(npcdata.id)
         {
         //SMBX64 Fixed special options for NPC
         /*Containers*/
         case 283:/*Bubble*/ case 91: /*buried*/ case 284: /*SMW Lakitu*/
         case 96: /*egg*/
         /*parakoopas*/
         case 76: case 121: case 122:case 123:case 124: case 161:case 176:case 177:
         /*Paragoomba*/ case 243: case 244:
         /*Cheep-Cheep*/ case 28: case 229: case 230: case 232: case 233: case 234: case 236:
         /*WarpSelection*/ case 288: case 289: /*firebar*/ case 260:

             if(
                     ((npcdata.id!=76)&&(npcdata.id!=28))
                     ||
                     (
                           ((ge(15))&&(npcdata.id==76))
                         ||((ge(31))&&(npcdata.id==28))
                     )
               )
             {
                 nextLine(); SIntVar(npcdata.special_data, line); //NPC special option
             }

            if(npcdata.id==91)
            switch(npcdata.special_data)
            {
            /*WarpSelection*/ case 288: /* case 289:*/ /*firebar*/ /*case 260:*/
             nextLine(); SIntVar(npcdata.special_data2, line);
             break;
            default: break;
            }

             break;
         default: break;
         }

         if(ge(3))
         {
             nextLine(); wBoolVar(npcdata.generator, line); //Generator enabled
             npcdata.generator_direct = -1;
             npcdata.generator_type = 1;

             if(npcdata.generator)
             {
                 nextLine(); UIntVar(npcdata.generator_direct, line); //Generator direction
                 nextLine(); UIntVar(npcdata.generator_type, line);   //Generator type [1] Warp, [2] Projectile
                 nextLine(); UIntVar(npcdata.generator_period, line); //Generator period ( sec*10 ) [1-600]
             }
         }

         if(ge(5))
         {
             nextLine();
             strVarMultiLine(npcdata.msg, line)//Message
         }

         if(ge(6))
         {
             nextLine(); wBoolVar(npcdata.friendly, line);//Friendly NPC
             nextLine(); wBoolVar(npcdata.nomove, line); //Don't move NPC
         }

         if(ge(9))
         {
             nextLine(); wBoolVar(npcdata.is_boss, line); //Set as boss flag
         }
         else
         {
             switch(npcdata.id)
             {
             //set boss flag to TRUE for old file formats automatically
             case 15: case 39: case 86:
                 npcdata.is_boss=true;
             default: break;
             }
         }

         if(ge(10))
         {
             nextLine(); strVar(npcdata.layer, line);
             nextLine(); strVar(npcdata.event_activate, line);
             nextLine(); strVar(npcdata.event_die, line);
             nextLine(); strVar(npcdata.event_talk, line);
         }

         if(ge(14)) {nextLine(); strVar(npcdata.event_emptylayer, line);} //No more objects in layer event
         if(ge(63)) {nextLine(); strVar(npcdata.attach_layer, line);} //Layer name to attach

         npcdata.array_id = FileData.npc_array_id;
         FileData.npc_array_id++;

         npcdata.index = FileData.npc.size(); //Apply element index

    FileData.npc.push_back(npcdata); //Add NPC into array
    nextLine();
    }


    ////////////Warp and Doors Data//////////
    nextLine();
    while( ((line!="\"next\"")&&(file_format>=10)) || ((file_format<10)&&(line!="")&&(!line.PGESTRINGisEmpty())))
    {
        doors = CreateLvlWarp();
        doors.isSetIn=true;
        doors.isSetOut=true;

                    SIntVar(doors.ix, line); //Entrance x
        nextLine(); SIntVar(doors.iy, line); //Entrance y
        nextLine(); SIntVar(doors.ox, line); //Exit x
        nextLine(); SIntVar(doors.oy, line); //Exit y

        nextLine(); UIntVar(doors.idirect, line); //Entrance direction: [3] down, [1] up, [2] left, [4] right
        nextLine(); UIntVar(doors.odirect, line); //Exit direction: [1] down [3] up [4] left [2] right
        nextLine(); UIntVar(doors.type, line);    //Door type: [1] pipe, [2] door, [0] instant

        if(ge(3)){ nextLine(); strVar(doors.lname, line);   //Warp to level
            nextLine(); UIntVar(doors.warpto, line); //Normal entrance or Warp to other door
            nextLine(); wBoolVar(doors.lvl_i, line); //Level Entrance (cannot enter)
                doors.isSetIn = ((doors.lvl_i)?false:true);}

        if(ge(4)){ nextLine(); wBoolVar(doors.lvl_o, line);
                  doors.isSetOut = (((doors.lvl_o)?false:true) || (doors.lvl_i));
                nextLine(); SIntVar(doors.world_x, line); //WarpTo X
                nextLine(); SIntVar(doors.world_y, line); //WarpTo y
        }

        if(ge(7)){ nextLine(); UIntVar(doors.stars, line);} //Need a stars
        if(ge(12)) { nextLine(); strVar(doors.layer, line); //Layer
            nextLine(); wBoolVar(doors.unknown, line); }    //<unused>, always FALSE

        if(ge(23)) { nextLine(); wBoolVar(doors.novehicles, line); } //Deny vehicles
        if(ge(25)) { nextLine(); wBoolVar(doors.allownpc, line); }   //Allow carried items
        if(ge(26)) { nextLine(); wBoolVar(doors.locked, line); }     //Locked

        doors.array_id = FileData.doors_array_id;
        FileData.doors_array_id++;
        doors.index = FileData.doors.size(); //Apply element index

    FileData.doors.push_back(doors); //Add NPC into array
    nextLine();
    }

    ////////////Water/QuickSand Data//////////
    if(file_format>=29)
    {
        nextLine();
        while(line!="\"next\"")
        {
            waters = CreateLvlPhysEnv();

                        SIntVar(waters.x, line);
            nextLine(); SIntVar(waters.y, line);
            nextLine(); UIntVar(waters.w, line);
            nextLine(); UIntVar(waters.h, line);
            nextLine(); UIntVar(waters.unknown, line); //Unused value
            if(ge(62)) { nextLine(); wBoolVar(waters.quicksand, line);}
            nextLine(); strVar(waters.layer, line);

            waters.array_id = FileData.physenv_array_id;
            FileData.physenv_array_id++;

            waters.index = FileData.physez.size(); //Apply element index

        FileData.physez.push_back(waters); //Add Water area into array
        nextLine();
        }
    }

    if(ge(10)) {
        ////////////Layers Data//////////
        nextLine();
        while((line!="\"next\"")&&(!in.isEOF())&&(!line.PGESTRINGisEmpty()))
        {

                          strVar(layers.name, line);     //Layer name
            nextLine(); wBoolVar(layers.hidden, line); //hidden layer

            layers.locked = false;

            layers.array_id = FileData.layers_array_id;
            FileData.layers_array_id++;

        FileData.layers.push_back(layers); //Add Water area into array
        nextLine();
        }

        ////////////Events Data//////////
        nextLine();
        while((line!="")&&(!in.isEOF())&&(!line.PGESTRINGisEmpty()))
        {
            events = CreateLvlEvent();

            strVar(events.name, line);//Event name

            if(ge(11))
            {
                nextLine();
                strVarMultiLine(events.msg, line)//Event message
            }

            if(ge(14)){ nextLine(); UIntVar(events.sound_id, line);}
            if(ge(18)){ nextLine(); UIntVar(events.end_game, line);}

            PGELIST<LevelEvent_layers > events_layersArr;
            events_layersArr.clear();

            events.layers_hide.clear();
            events.layers_show.clear();
            events.layers_toggle.clear();

            for(i=0; i<sct; i++)
            {
                nextLine(); strVar(events_layers.hide, line); //Hide layer
                nextLine(); strVar(events_layers.show, line); //Show layer
                if(ge(14)){ nextLine(); strVar(events_layers.toggle, line);//Toggle layer
                } else events_layers.toggle="";

                if(events_layers.hide!="") events.layers_hide.push_back(events_layers.hide);
                if(events_layers.show!="") events.layers_show.push_back(events_layers.show);
                if(events_layers.toggle!="") events.layers_toggle.push_back(events_layers.toggle);
                events_layersArr.push_back(events_layers);
            }

            if(ge(13))
            {
                events.sets.clear();
                for(i=0; i<21; i++)
                {
                    nextLine(); SIntVar(events_sets.music_id, line);        //Set Music
                    nextLine(); SIntVar(events_sets.background_id, line);   //Set Background
                    nextLine(); SIntVar(events_sets.position_left, line);   //Set Position to: LEFT
                    nextLine(); SIntVar(events_sets.position_top, line);    //Set Position to: TOP
                    nextLine(); SIntVar(events_sets.position_bottom, line); //Set Position to: BOTTOM
                    nextLine(); SIntVar(events_sets.position_right, line);  //Set Position to: RIGHT
                    events.sets.push_back(events_sets);
                }
            }

            if(ge(26)) { nextLine(); strVar(events.trigger, line); //Trigger
                         nextLine(); UIntVar(events.trigger_timer, line);} //Start trigger event after x [sec*10]. Etc. 153,2 sec

            if(ge(27)) { nextLine(); wBoolVar(events.nosmoke, line); }//Don't smoke tobacco, let's healthy! :D

            if(ge(28))
            {
                nextLine(); wBoolVar(events.ctrl_altjump, line);//Hold ALT-JUMP player control
                nextLine(); wBoolVar(events.ctrl_altrun, line); //ALT-RUN
                nextLine(); wBoolVar(events.ctrl_down, line);   //DOWN
                nextLine(); wBoolVar(events.ctrl_drop, line);   //DROP
                nextLine(); wBoolVar(events.ctrl_jump, line);   //JUMP
                nextLine(); wBoolVar(events.ctrl_left, line);   //LEFT
                nextLine(); wBoolVar(events.ctrl_right, line);  //RIGHT
                nextLine(); wBoolVar(events.ctrl_run, line);    //RUN
                nextLine(); wBoolVar(events.ctrl_start, line);  //START
                nextLine(); wBoolVar(events.ctrl_up, line);  //UP
            }

            if(ge(32))
            {
                nextLine(); wBoolVar(events.autostart, line);  //Auto start
                nextLine(); strVar(events.movelayer, line);  //Layer for movement
                nextLine(); SFltVar(events.layer_speed_x, line); //Layer moving speed – horizontal
                nextLine(); SFltVar(events.layer_speed_y, line); //Layer moving speed – vertical
            }

            if(ge(33))
            {
                nextLine(); SFltVar(events.move_camera_x, line); //Move screen horizontal speed
                nextLine(); SFltVar(events.move_camera_y, line); //Move screen vertical speed
                nextLine(); SIntVar(events.scroll_section, line); //Scroll section x, (in file value is x-1)
            }

            events.array_id = FileData.events_array_id;
            FileData.events_array_id++;

        FileData.events.push_back(events);
        nextLine();
        }
    }

    {
        //Add system layers if not exist
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

        //Add system events if not exist
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
    ///////////////////////////////////////EndFile///////////////////////////////////////

    FileData.ReadFileValid=true;
    return FileData;

    badfile:    //If file format is not correct
        if(file_format>0)
            FileData.ERROR_info="Detected file format: SMBX-"+fromNum(file_format)+" is invalid";
        else
            FileData.ERROR_info="It is not an SMBX level file";
        FileData.ERROR_linenum=str_count;
        FileData.ERROR_linedata=line;
        FileData.ReadFileValid=false;
    return FileData;
}








//*********************************************************
//****************WRITE FILE FORMAT************************
//*********************************************************

PGESTRING FileFormats::WriteSMBX64LvlFile(LevelData FileData, int file_format)
{
    PGESTRING TextData;
    int i, j;

    //Count placed stars on this level
    FileData.stars=0;
    for(i=0;i<(signed)FileData.npc.size();i++)
    {
        if(FileData.npc[i].is_star)
            FileData.stars++;
    }

    //Sort blocks array by X->Y
    smbx64LevelSortBlocks(FileData);
    //Sort BGO's array by SortPriority->X->Y
    smbx64LevelSortBGOs(FileData);

    //Prevent out of range: 0....64
    if(file_format<0) file_format = 0;
    else
    if(file_format>64) file_format = 64;



    TextData += SMBX64::IntS(file_format);                     //Format version
    if(file_format>=17)
    TextData += SMBX64::IntS(FileData.stars);         //Number of stars
    if(file_format>=60)
    TextData += SMBX64::qStrS(FileData.LevelName);  //Level name

    int s_limit=21;

    if(file_format < 8)
        s_limit=6;
    //qDebug() << "sections "<<s_limit << "format "<<file_format ;

    //Sections settings
    for(i=0; (i<s_limit)&&(i<(signed)FileData.sections.size()) ; i++)
    {
        TextData += SMBX64::IntS(FileData.sections[i].size_left);
        TextData += SMBX64::IntS(FileData.sections[i].size_top);
        TextData += SMBX64::IntS(FileData.sections[i].size_bottom);
        TextData += SMBX64::IntS(FileData.sections[i].size_right);
        TextData += SMBX64::IntS(FileData.sections[i].music_id);
        TextData += SMBX64::IntS(FileData.sections[i].bgcolor);
        TextData += SMBX64::BoolS(FileData.sections[i].wrap_h);
        TextData += SMBX64::BoolS(FileData.sections[i].OffScreenEn);
        TextData += SMBX64::IntS(FileData.sections[i].background);
        if(file_format>=1)
        TextData += SMBX64::BoolS(FileData.sections[i].lock_left_scroll);
        if(file_format>=30)
        TextData += SMBX64::BoolS(FileData.sections[i].underwater);
        if(file_format>=2)
        TextData += SMBX64::qStrS(FileData.sections[i].music_file);
    }
    for( ; i<s_limit ; i++) //Protector
    {
        if(file_format<1)
            TextData += "0\n0\n0\n0\n0\n16291944\n#FALSE#\n#FALSE#\n0\n";
        else if(file_format<2)
            TextData += "0\n0\n0\n0\n0\n16291944\n#FALSE#\n#FALSE#\n0\n#FALSE#\n";
        else if(file_format<30)
            TextData += "0\n0\n0\n0\n0\n16291944\n#FALSE#\n#FALSE#\n0\n#FALSE#\n\"\"\n";
        else
            TextData += "0\n0\n0\n0\n0\n16291944\n#FALSE#\n#FALSE#\n0\n#FALSE#\n#FALSE#\n\"\"\n";
    }
        //append dummy section data, if array size is less than 21

    //qDebug() << "i="<< i;

    //Players start point
    int playerpoints=0;
    for(j=1;j<=2 && playerpoints<2;j++)
    {
        bool found=false;
        for(i=0; i<(signed)FileData.players.size(); i++ )
        {
            if(FileData.players[i].id!=(unsigned int)j) continue;
            TextData += SMBX64::IntS(FileData.players[i].x);
            TextData += SMBX64::IntS(FileData.players[i].y);
            TextData += SMBX64::IntS(FileData.players[i].w);
            TextData += SMBX64::IntS(FileData.players[i].h);
            playerpoints++;found=true;
        }
        if(!found)
        {
            TextData += "0\n0\n0\n0\n";
            playerpoints++;
        }

    }
    for( ;playerpoints<2; playerpoints++ ) //Protector
        TextData += "0\n0\n0\n0\n";


    //Blocks
    for(int blkID=0;blkID<(signed)FileData.blocks.size();blkID++)
    {
        LevelBlock &block=FileData.blocks[blkID];
        TextData += SMBX64::IntS(block.x);
        TextData += SMBX64::IntS(block.y);
        TextData += SMBX64::IntS(block.h);
        TextData += SMBX64::IntS(block.w);
        TextData += SMBX64::IntS(block.id);
        int npcID = block.npc_id;
        if(npcID < 0)
        {
            npcID *= -1; if(npcID>99) npcID = 99;
        }
        else
        if(npcID!=0)
        {
            npcID+=1000;
            if(file_format<=64)
            {
                long xnpcID = npcID;
                //Convert NPC-ID value from SMBX64 to SMBX1/2
                switch(xnpcID)
                {
                    case 1009://Mushroom
                        xnpcID = 100; break;
                    case 1001://Goomba
                        xnpcID = 101; break;
                    case 1014://Fire flower
                        xnpcID = 102; break;
                    case 1034://Super leaf
                        xnpcID = 103; break;
                    case 1035://Shoe
                        xnpcID = 104; break;
                    default:
                        break;
                }
                npcID = xnpcID;
            }
        }

        TextData += SMBX64::IntS(npcID);
        TextData += SMBX64::BoolS(block.invisible);
        if(file_format>=61)
        TextData += SMBX64::BoolS(block.slippery);
        if(file_format>=10)
        TextData += SMBX64::qStrS(block.layer);
        if(file_format>=14)
        {
            TextData += SMBX64::qStrS(block.event_destroy);
            TextData += SMBX64::qStrS(block.event_hit);
            TextData += SMBX64::qStrS(block.event_emptylayer);
        }
    }
    TextData += "\"next\"\n";//Separator


    //BGOs
    for(int bgoID=0;bgoID<(signed)FileData.bgo.size();bgoID++)
    {
        LevelBGO &bgo1=FileData.bgo[bgoID];
        TextData += SMBX64::IntS( bgo1.x);
        TextData += SMBX64::IntS( bgo1.y);
        TextData += SMBX64::IntS( bgo1.id);
        if(file_format>=10)
        TextData += SMBX64::qStrS( bgo1.layer);
    }
    TextData += "\"next\"\n";//Separator

    //NPCs
    for(i=0; i<(signed)FileData.npc.size(); i++)
    {
        //Section size
        TextData += SMBX64::IntS(FileData.npc[i].x);
        TextData += SMBX64::IntS(FileData.npc[i].y);
        TextData += SMBX64::IntS(FileData.npc[i].direct);
        TextData += SMBX64::IntS(FileData.npc[i].id);

        if(file_format>=10)
        {
            switch(FileData.npc[i].id)
            {
                //SMBX64 Fixed special options for NPC
                /*Containers*/
                case 283:/*Bubble*/
                case 91: /*buried*/
                case 284: /*SMW Lakitu*/
                case 96: /*egg*/

                /*Items*/
                /*parakoopa*/ case 76: case 121: case 122:case 123:case 124: case 161:case 176:case 177:
                /*paragoomba*/ case 243: case 244:
                /*Cheep-Cheep*/ case 28: case 229: case 230: case 232: case 233: case 234: case 236:
                /*WarpSelection*/ case 288: case 289:
                /*firebar*/ case 260:

                if(
                        ((FileData.npc[i].id!=76)&&(FileData.npc[i].id!=28))
                        ||
                        (
                            ((file_format >= 15)&&(FileData.npc[i].id==76))
                            ||((file_format >= 31)&&(FileData.npc[i].id==28))
                        )
                  )
            TextData += SMBX64::IntS(FileData.npc[i].special_data);

                if((FileData.npc[i].id==91)&&(FileData.npc[i].special_data==288)) // Warp Section value for included into herb magic potion
                    TextData += SMBX64::IntS(FileData.npc[i].special_data2);

                break;
                default:
                    break;
            }
        }

        if(file_format>=3)
        {
            TextData += SMBX64::BoolS(FileData.npc[i].generator);
            if(FileData.npc[i].generator)
            {
                TextData += SMBX64::IntS(FileData.npc[i].generator_direct);
                TextData += SMBX64::IntS(FileData.npc[i].generator_type);
                TextData += SMBX64::IntS(FileData.npc[i].generator_period);
            }
        }

        if(file_format>=5)
        TextData += SMBX64::qStrS_multiline(FileData.npc[i].msg);

        if(file_format>=6)
        {
            TextData += SMBX64::BoolS(FileData.npc[i].friendly);
            TextData += SMBX64::BoolS(FileData.npc[i].nomove);
        }
        if(file_format>=9)
        TextData += SMBX64::BoolS(FileData.npc[i].is_boss);

        if(file_format>=10)
        {
            TextData += SMBX64::qStrS(FileData.npc[i].layer);
            TextData += SMBX64::qStrS(FileData.npc[i].event_activate);
            TextData += SMBX64::qStrS(FileData.npc[i].event_die);
            TextData += SMBX64::qStrS(FileData.npc[i].event_talk);
        }
        if(file_format>=14)
        TextData += SMBX64::qStrS(FileData.npc[i].event_emptylayer);
        if(file_format>=63)
        TextData += SMBX64::qStrS(FileData.npc[i].attach_layer);
    }
    TextData += "\"next\"\n";//Separator

    //Doors
    for(i=0; i<(signed)FileData.doors.size(); i++)
    {
        if( ((!FileData.doors[i].lvl_o) && (!FileData.doors[i].lvl_i)) || ((FileData.doors[i].lvl_o) && (!FileData.doors[i].lvl_i)) )
            if(!FileData.doors[i].isSetIn) continue; // Skip broken door
        if( ((!FileData.doors[i].lvl_o) && (!FileData.doors[i].lvl_i)) || ((FileData.doors[i].lvl_i)) )
            if(!FileData.doors[i].isSetOut) continue; // Skip broken door

        TextData += SMBX64::IntS(FileData.doors[i].ix);
        TextData += SMBX64::IntS(FileData.doors[i].iy);
        TextData += SMBX64::IntS(FileData.doors[i].ox);
        TextData += SMBX64::IntS(FileData.doors[i].oy);
        TextData += SMBX64::IntS(FileData.doors[i].idirect);
        TextData += SMBX64::IntS(FileData.doors[i].odirect);
        TextData += SMBX64::IntS(FileData.doors[i].type);
        if(file_format>=3)
        {
            TextData += SMBX64::qStrS(FileData.doors[i].lname);
            TextData += SMBX64::IntS(FileData.doors[i].warpto);
            TextData += SMBX64::BoolS(FileData.doors[i].lvl_i);
        }
        if(file_format>=4)
        {
            TextData += SMBX64::BoolS(FileData.doors[i].lvl_o);
            TextData += SMBX64::IntS(FileData.doors[i].world_x);
            TextData += SMBX64::IntS(FileData.doors[i].world_y);
        }
        if(file_format>=7)
        TextData += SMBX64::IntS(FileData.doors[i].stars);
        if(file_format>=12)
        {
            TextData += SMBX64::qStrS(FileData.doors[i].layer);
            TextData += SMBX64::BoolS(FileData.doors[i].unknown);
        }
        if(file_format>=23)
        TextData += SMBX64::BoolS(FileData.doors[i].novehicles);
        if(file_format>=25)
        TextData += SMBX64::BoolS(FileData.doors[i].allownpc);
        if(file_format>=26)
        TextData += SMBX64::BoolS(FileData.doors[i].locked);
    }

    //Water
    if(file_format>=29)
    {
        TextData += "\"next\"\n";//Separator
        for(i=0; i<(signed)FileData.physez.size(); i++)
        {
            TextData += SMBX64::IntS(FileData.physez[i].x);
            TextData += SMBX64::IntS(FileData.physez[i].y);
            TextData += SMBX64::IntS(FileData.physez[i].w);
            TextData += SMBX64::IntS(FileData.physez[i].h);
            TextData += SMBX64::IntS(FileData.physez[i].unknown);
            if(file_format>=62)
            TextData += SMBX64::BoolS(FileData.physez[i].quicksand);
            TextData += SMBX64::qStrS(FileData.physez[i].layer);
        }
    }

    if(file_format>=10)
    {

        TextData += "\"next\"\n";//Separator

        //Layers
        for(i=0; i<(signed)FileData.layers.size(); i++)
        {
            TextData += SMBX64::qStrS(FileData.layers[i].name);
            TextData += SMBX64::BoolS(FileData.layers[i].hidden);
        }
        TextData += "\"next\"\n";//Separator

        LevelEvent_layers layerSet;
        for(i=0; i<(signed)FileData.events.size(); i++)
        {
            TextData += SMBX64::qStrS(FileData.events[i].name);
            if(file_format>=11)
            TextData += SMBX64::qStrS_multiline(FileData.events[i].msg);
            if(file_format>=14)
            TextData += SMBX64::IntS(FileData.events[i].sound_id);
            if(file_format>=18)
            TextData += SMBX64::IntS(FileData.events[i].end_game);

            PGELIST<LevelEvent_layers > events_layersArr;
            //FileData.events[i].layers.clear();
            for(j=0; j<20; j++)
            {
                layerSet.hide =
                        ((j<(signed)FileData.events[i].layers_hide.size())?
                          FileData.events[i].layers_hide[j] : "");
                layerSet.show =
                        ((j<(signed)FileData.events[i].layers_show.size())?
                          FileData.events[i].layers_show[j] : "");;
                layerSet.toggle =
                        ((j<(signed)FileData.events[i].layers_toggle.size())?
                          FileData.events[i].layers_toggle[j] : "");

                events_layersArr.push_back(layerSet);
            }

            for(j=0; j< (signed)events_layersArr.size()  && j<s_limit-1; j++)
            {
                TextData += SMBX64::qStrS(events_layersArr[j].hide);
                TextData += SMBX64::qStrS(events_layersArr[j].show);
                if(file_format>=14)
                TextData += SMBX64::qStrS(events_layersArr[j].toggle);
            }
            for( ; j<s_limit; j++)
            {
                if(file_format>=14)
                TextData += "\"\"\n\"\"\n\"\"\n"; //(21st element is SMBX 1.3 bug protector)
                else
                TextData += "\"\"\n\"\"\n";
            }

            if(file_format>=13)
            {
                for(j=0; j< (signed)FileData.events[i].sets.size()  && j<s_limit; j++)
                {
                    TextData += SMBX64::IntS(FileData.events[i].sets[j].music_id);
                    TextData += SMBX64::IntS(FileData.events[i].sets[j].background_id);
                    TextData += SMBX64::IntS(FileData.events[i].sets[j].position_left);
                    TextData += SMBX64::IntS(FileData.events[i].sets[j].position_top);
                    TextData += SMBX64::IntS(FileData.events[i].sets[j].position_bottom);
                    TextData += SMBX64::IntS(FileData.events[i].sets[j].position_right);
                }
                for( ; j<s_limit; j++) // Protector
                    TextData += "0\n0\n0\n-1\n-1\n-1\n";
            }

            if(file_format>=26){
            TextData += SMBX64::qStrS(FileData.events[i].trigger);
            TextData += SMBX64::IntS(FileData.events[i].trigger_timer);
            }

            if(file_format>=27)
            TextData += SMBX64::BoolS(FileData.events[i].nosmoke);

            if(file_format>=28)
            {
            TextData += SMBX64::BoolS(FileData.events[i].ctrl_altjump);
            TextData += SMBX64::BoolS(FileData.events[i].ctrl_altrun);
            TextData += SMBX64::BoolS(FileData.events[i].ctrl_down);
            TextData += SMBX64::BoolS(FileData.events[i].ctrl_drop);
            TextData += SMBX64::BoolS(FileData.events[i].ctrl_jump);
            TextData += SMBX64::BoolS(FileData.events[i].ctrl_left);
            TextData += SMBX64::BoolS(FileData.events[i].ctrl_right);
            TextData += SMBX64::BoolS(FileData.events[i].ctrl_run);
            TextData += SMBX64::BoolS(FileData.events[i].ctrl_start);
            TextData += SMBX64::BoolS(FileData.events[i].ctrl_up);
            }

            if(file_format>=32)
            {
            TextData += SMBX64::BoolS(FileData.events[i].autostart);

            TextData += SMBX64::qStrS(FileData.events[i].movelayer);
            TextData += SMBX64::FloatS(FileData.events[i].layer_speed_x);
            TextData += SMBX64::FloatS(FileData.events[i].layer_speed_y);
            }
            if(file_format>=33)
            {
            TextData += SMBX64::FloatS(FileData.events[i].move_camera_x);
            TextData += SMBX64::FloatS(FileData.events[i].move_camera_y);
            TextData += SMBX64::IntS(FileData.events[i].scroll_section);
            }
        }
    }

    return TextData;
}
