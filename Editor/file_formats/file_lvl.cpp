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
#include "../edit_level/leveledit.h"
#include "file_formats.h"




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
    dummyDoor.noyoshi = false;
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
    dummyBGO.x = 0;
    dummyBGO.y = 0;
    dummyBGO.id = 0;
    dummyBGO.layer = "Default";
    dummyBGO.smbx64_sp = -1;

    dummyBGO.array_id  = 0;
    dummyBGO.index = 0;
    return dummyBGO;
}


LevelWater FileFormats::dummyLvlWater()
{
    LevelWater dummyWater;
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
    dummyEvent.altjump=false;
    dummyEvent.altrun=false;
    dummyEvent.down=false;
    dummyEvent.drop=false;
    dummyEvent.jump=false;
    dummyEvent.left=false;
    dummyEvent.right=false;
    dummyEvent.run=false;
    dummyEvent.start=false;
    dummyEvent.up=false;
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

    return dummySection;
}


LevelData FileFormats::dummyLvlDataArray()
{
    LevelData NewFileData;

    NewFileData.ReadFileValid = true;
    NewFileData.modified = true;

    NewFileData.CurSection=0;
    NewFileData.playmusic=0;

    NewFileData.LevelName = "";
    NewFileData.stars = 0;

    NewFileData.bgo_array_id = 0;
    NewFileData.blocks_array_id = 0;
    NewFileData.doors_array_id = 1;
    NewFileData.events_array_id = 0;
    NewFileData.layers_array_id = 0;
    NewFileData.npc_array_id = 0;
    NewFileData.water_array_id = 0;

    //Create Section array
    LevelSection section;
    for(int i=0; i<21;i++)
    {
        section = dummyLvlSection();
        section.id = i;
        NewFileData.sections.push_back( section );
    }

    //Create players array
    PlayerPoint players;
        players.x = 0;
        players.y = 0;
        players.w = 0;
        players.h = 0;
        players.id=0;
        for(int i=0; i<2;i++)
        {
            players.id++;
            NewFileData.players.push_back(players);
        }


    //Create system layers
        //Default
        //Destroyed Blocks
        //Spawned NPCs

    LevelLayers layers;
        layers.hidden = false;
        layers.name = "Default";
        layers.array_id = NewFileData.layers_array_id++;
        NewFileData.layers.push_back(layers);

        layers.hidden = true;
        layers.name = "Destroyed Blocks";
        layers.array_id = NewFileData.layers_array_id++;
        NewFileData.layers.push_back(layers);

        layers.hidden = false;
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




//*********************************************************
//****************READ FILE FORMAT*************************
//*********************************************************


//Level File Read
LevelData FileFormats::ReadLevelFile(QFile &inf)
{
    QTextStream in(&inf);   //Read File

    in.setAutoDetectUnicode(true); //Test Fix for MacOS
    in.setLocale(QLocale::system());   //Test Fix for MacOS
    in.setCodec(QTextCodec::codecForLocale()); //Test Fix for MacOS

    int str_count=0;        //Line Counter
    int i;                  //counters
    int file_format=0;        //File format number
    QString line;           //Current Line data
    LevelData FileData;

    LevelSection section;
    int sct;
    PlayerPoint players;
    LevelBlock blocks;
    LevelBGO bgodata;
    LevelNPC npcdata;
    LevelDoors doors;
    LevelWater waters;
    LevelLayers layers;
    LevelEvents events;
    LevelEvents_layers events_layers;
    LevelEvents_Sets events_sets;

    //Begin all ArrayID's here;
    FileData.blocks_array_id = 1;
    FileData.bgo_array_id = 1;
    FileData.npc_array_id = 1;
    FileData.doors_array_id = 1;
    FileData.water_array_id = 1;
    FileData.layers_array_id = 1;
    FileData.events_array_id = 1;


    ///////////////////////////////////////Begin file///////////////////////////////////////
    str_count++;line = in.readLine();   //Read first line
    if( SMBX64::Int(line) ) //File format number
        goto badfile;

    else file_format=line.toInt();

    if(file_format >= 17)
    {
        str_count++;line = in.readLine();   //Read second Line
        if( SMBX64::Int(line) ) //File format number
            goto badfile;
        else FileData.stars=line.toInt();   //Number of stars
    } else FileData.stars=0;

    if(file_format >= 62)
    {
        str_count++;line = in.readLine();   //Read third line
        if( SMBX64::qStr(line) ) //LevelTitle
            goto badfile;
        else FileData.LevelName = removeQuotes(line); //remove quotes
    } else FileData.LevelName="";

    //total sections
    if(file_format>=8)
        sct=21;
    else
        sct=6;

    FileData.CurSection=0;
    FileData.playmusic=0;

    ////////////SECTION Data//////////
    for(i=0;i<sct;i++)
    {
        section = dummyLvlSection();

        str_count++;line = in.readLine();
        if(SMBX64::sInt(line)) //left
            goto badfile;
        else {
               section.size_left=line.toInt();
               section.PositionX=line.toInt();
             }

        str_count++;line = in.readLine();
        if(SMBX64::sInt(line)) //top
            goto badfile;
        else section.size_top=line.toInt();

        str_count++;line = in.readLine();
        if(SMBX64::sInt(line)) //bottom
            goto badfile;
        else {
               section.size_bottom=line.toInt();
               section.PositionY=line.toInt()-602;
             }

        str_count++;line = in.readLine();
        if(SMBX64::sInt(line)) //right
            goto badfile;
        else section.size_right=line.toInt();

        str_count++;line = in.readLine();
        if(SMBX64::Int(line)) //music
            goto badfile;
        else section.music_id=line.toInt();

        str_count++;line = in.readLine();
        if(SMBX64::Int(line)) //bgcolor
            goto badfile;
        else section.bgcolor=line.toInt();

        str_count++;line = in.readLine();
        if(SMBX64::wBool(line)) //IsLevelWarp
            goto badfile;
        else section.IsWarp=((line=="#TRUE#")?true:false);

        str_count++;line = in.readLine();
        if(SMBX64::wBool(line)) //OffScreen
            goto badfile;
        else section.OffScreenEn=((line=="#TRUE#")?true:false);

        str_count++;line = in.readLine();
        if(SMBX64::Int(line)) //BackGround id
            goto badfile;
        else section.background=line.toInt();

        str_count++;line = in.readLine();
        if(SMBX64::wBool(line)) //NoTurnBack
            goto badfile;
        else section.noback=((line=="#TRUE#")?true:false);

        if(file_format >= 32)
        {
            str_count++;line = in.readLine();
            if(SMBX64::wBool(line)) //Underwater
                goto badfile;
            else section.underwater=((line=="#TRUE#")?true:false);
        }
        else section.underwater=false;

        if(file_format >= 2)
        {
            str_count++;line = in.readLine();
            if(SMBX64::qStr(line)) //Custom Music
                goto badfile;
            else section.music_file = removeQuotes(line); //remove quotes
        } else section.music_file="";

        section.id = i;
    FileData.sections.push_back(section); //Add Section in main array
    }
    if(file_format<8)
    {
        for(i=i;i<21;i++)
            {
            section = dummyLvlSection();
            section.id=i;

        FileData.sections.push_back(section); //Add Section in main array
        }
    }


    //Player's point config
    for(i=0;i<2;i++)
    {
        str_count++;line = in.readLine();
        if(SMBX64::sInt(line)) //1 Player x
            goto badfile;
        else players.x=line.toInt();

        str_count++;line = in.readLine();
        if(SMBX64::sInt(line)) //1 Player y
            goto badfile;
        else players.y=line.toInt();

        str_count++;line = in.readLine();
        if(SMBX64::Int(line)) //1 Player w
            goto badfile;
        else players.w=line.toInt();

        str_count++;line = in.readLine();
        if(SMBX64::Int(line)) //1 Player h
            goto badfile;
        else players.h=line.toInt();

        players.id = i+1;

    FileData.players.push_back(players);    //Add player in array
    }


    ////////////Block Data//////////
    str_count++;line = in.readLine();
    while(line!="\"next\"")
    {
        blocks = dummyLvlBlock();

        if(SMBX64::sInt(line)) //Block x
            goto badfile;
        else blocks.x = line.toInt();

        str_count++;line = in.readLine();
        if(SMBX64::sInt(line)) //Block y
            goto badfile;
        else blocks.y = line.toInt();

        str_count++;line = in.readLine();
        if(SMBX64::Int(line)) //Block h
            goto badfile;
        else blocks.h = line.toInt();

        str_count++;line = in.readLine();
        if(SMBX64::Int(line)) //Block w
            goto badfile;
        else blocks.w = line.toInt();

        str_count++;line = in.readLine();
        if(SMBX64::Int(line)) //Block id
            goto badfile;
        else blocks.id = line.toInt();

        str_count++;line = in.readLine();
        if(SMBX64::sInt(line)) //Containing NPC id
            goto badfile;
        else blocks.npc_id = line.toInt();

        str_count++;line = in.readLine();
        if(SMBX64::wBool(line)) //Invisible
            goto badfile;
        else blocks.invisible = ((line=="#TRUE#")?true:false);

        if(file_format >= 62)
        {
            str_count++;line = in.readLine();
            if(SMBX64::wBool(line)) //Slippery
                goto badfile;
            else blocks.slippery = ((line=="#TRUE#")?true:false);
        }// else blocks.slippery = false;

        if(file_format >= 10)
        {
            str_count++;line = in.readLine();
            if(SMBX64::qStr(line)) //layer
                goto badfile;
            else blocks.layer = removeQuotes(line);

            str_count++;line = in.readLine();
            if(SMBX64::qStr(line)) //event destroy
                goto badfile;
            else blocks.event_destroy = removeQuotes(line);

            str_count++;line = in.readLine();
            if(SMBX64::qStr(line)) //event hit
                goto badfile;
            else blocks.event_hit = removeQuotes(line);

            str_count++;line = in.readLine();
            if(SMBX64::qStr(line)) //event no more objects in layer
                goto badfile;
            else blocks.event_no_more = removeQuotes(line);
        }
        /*
        else
        {  //Set default if loading old file version
            blocks.layer = "Default";
            blocks.event_destroy="";
            blocks.event_hit="";
            blocks.event_no_more="";
        }*/

        blocks.array_id = FileData.blocks_array_id;
        FileData.blocks_array_id++;
        blocks.index = FileData.blocks.size(); //Apply element index
    FileData.blocks.push_back(blocks); //AddBlock into array

    str_count++;line = in.readLine();
    }


    ////////////BGO Data//////////
    str_count++;line = in.readLine();
    while(line!="\"next\"")
    {
        bgodata = dummyLvlBgo();

        if(SMBX64::sInt(line)) //BGO x
            goto badfile;
        else bgodata.x = line.toInt();

        str_count++;line = in.readLine();
        if(SMBX64::sInt(line)) //BGO y
            goto badfile;
        else bgodata.y= line.toInt();

        str_count++;line = in.readLine();
        if(SMBX64::Int(line)) //BGO id
            goto badfile;
        else bgodata.id= line.toInt();

        if(file_format >= 10)
        {
            str_count++;line = in.readLine();
            if(SMBX64::qStr(line)) //layer
                goto badfile;
            else bgodata.layer = removeQuotes(line);
        }
        //else bgodata.layer = "Default";

        bgodata.smbx64_sp = -1;

        bgodata.array_id = FileData.bgo_array_id;
        FileData.bgo_array_id++;

        bgodata.index = FileData.bgo.size(); //Apply element index

    FileData.bgo.push_back(bgodata); //Add Background object into array

    str_count++;line = in.readLine();
    }


    ////////////NPC Data//////////
     str_count++;line = in.readLine();
     while(line!="\"next\"")
     {

         npcdata = dummyLvlNpc();

         if(SMBX64::sFloat(line)) //NPC x
             goto badfile;
         else npcdata.x = round(line.toDouble());

         str_count++;line = in.readLine();

         if(SMBX64::sFloat(line)) //NPC y
             goto badfile;
         else npcdata.y = round(line.toDouble());

         str_count++;line = in.readLine();
         if(SMBX64::sInt(line)) //NPC direction
             goto badfile;
         else npcdata.direct = line.toInt();

         str_count++;line = in.readLine();
         if(SMBX64::Int(line)) //NPC id
             goto badfile;

         else npcdata.id = line.toInt();

         if(file_format >= 10)
         {
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

                 str_count++;line = in.readLine();
                 if(SMBX64::sInt(line)) //NPC special option
                     goto badfile;
                 else npcdata.special_data = line.toInt();

                if(npcdata.id==91)
                switch(npcdata.special_data)
                {
                /*WarpSelection*/ case 288: /* case 289:*/ /*firebar*/ /*case 260:*/

                 str_count++;line = in.readLine();
                 if(SMBX64::sInt(line)) //skip line
                     goto badfile;
                 else npcdata.special_data2 = line.toInt();
                 break;
                default: break;
                }

                 break;
             default: break;
             }

             str_count++;line = in.readLine();
             if(SMBX64::wBool(line)) //Generator enabled
                 goto badfile;
             else npcdata.generator = ((line=="#TRUE#")?true:false);

             npcdata.generator_direct = -1;
             npcdata.generator_type = 1;

             if(npcdata.generator)
             {

                 str_count++;line = in.readLine();
                 if(SMBX64::Int(line)) //Generator direction
                     goto badfile;
                 else npcdata.generator_direct= line.toInt();

                 str_count++;line = in.readLine();
                 if(SMBX64::Int(line)) //Generator type [1] Warp, [2] Projectile
                     goto badfile;
                 else npcdata.generator_type = line.toInt();
                 str_count++;line = in.readLine();
                 if(SMBX64::Int(line)) //Generator period ( sec*10 ) [1-600]
                     goto badfile;
                 else npcdata.generator_period = line.toInt();
             }

             str_count++;line = in.readLine();
             if(SMBX64::qStr(line)) //Message
                 goto badfile;
             else npcdata.msg = removeQuotes(line);

             str_count++;line = in.readLine();
             if(SMBX64::wBool(line)) //Friendly NPC
                 goto badfile;
             else npcdata.friendly = ((line=="#TRUE#")?true:false);

             str_count++;line = in.readLine();
             if(SMBX64::wBool(line)) //Don't move NPC
                 goto badfile;
             else npcdata.nomove = ((line=="#TRUE#")?true:false);

             str_count++;line = in.readLine();
             if(SMBX64::wBool(line)) //LegacyBoss
                 goto badfile;
             else npcdata.legacyboss = ((line=="#TRUE#")?true:false);

             str_count++;line = in.readLine();
             if(SMBX64::qStr(line)) //Layer
                 goto badfile;
             else npcdata.layer = removeQuotes(line);

             str_count++;line = in.readLine();
             if(SMBX64::qStr(line)) //Activate event
                 goto badfile;
             else npcdata.event_activate = removeQuotes(line);

             str_count++;line = in.readLine();
             if(SMBX64::qStr(line)) //Death event
                 goto badfile;
             else npcdata.event_die = removeQuotes(line);

             str_count++;line = in.readLine();
             if(SMBX64::qStr(line)) //Talk event
                 goto badfile;
             else npcdata.event_talk = removeQuotes(line);

             str_count++;line = in.readLine();
             if(SMBX64::qStr(line)) //No more objects in layer event
                 goto badfile;
             else npcdata.event_nomore = removeQuotes(line);
        }
         /*
         else
         {
            npcdata.special_data = -1;
            npcdata.generator = false;
            npcdata.generator_direct = -1;
            npcdata.generator_type = -1;
            npcdata.msg = "";
            npcdata.friendly=false;
            npcdata.nomove=false;
            npcdata.legacyboss=false;
            npcdata.layer="Default";
            npcdata.event_activate="";
            npcdata.event_die="";
            npcdata.event_talk="";
            npcdata.event_nomore="";
         }*/


         if(file_format >= 63)
         {
             str_count++;line = in.readLine();
             if(SMBX64::qStr(line)) //Layer name to attach
                 goto badfile;
             else npcdata.attach_layer = removeQuotes(line);
         } // else npcdata.attach_layer = "";

         npcdata.array_id = FileData.npc_array_id;
         FileData.npc_array_id++;

         npcdata.index = FileData.npc.size(); //Apply element index

    FileData.npc.push_back(npcdata); //Add NPC into array
    str_count++;line = in.readLine();
    }


    ////////////Warp and Doors Data//////////
    str_count++;line = in.readLine();
    while( ((line!="\"next\"")&&(file_format>=10)) || ((file_format<10)&&(line!="")&&(!line.isNull())))
    {
        doors = dummyLvlDoor();

        if(SMBX64::sInt(line)) //Entrance x
            goto badfile;
        else doors.ix = line.toInt();

        str_count++;line = in.readLine();
        if(SMBX64::sInt(line)) //Entrance y
            goto badfile;
        else doors.iy = line.toInt();

        str_count++;line = in.readLine();
        if(SMBX64::sInt(line)) //Exit x
            goto badfile;
        else doors.ox = line.toInt();

        str_count++;line = in.readLine();
        if(SMBX64::sInt(line)) //Exit y
            goto badfile;
        else doors.oy = line.toInt();

        str_count++;line = in.readLine();
        if(SMBX64::Int(line)) //Entrance direction: [3] down, [1] up, [2] left, [4] right
            goto badfile;
        else doors.idirect= line.toInt();


        str_count++;line = in.readLine();
        if(SMBX64::Int(line)) //Exit direction: [1] down [3] up [4] left [2] right
            goto badfile;
        else doors.odirect= line.toInt();


        str_count++;line = in.readLine();
        if(SMBX64::Int(line)) //Door type: [1] pipe, [2] door, [0] instant
            goto badfile;
        else doors.type= line.toInt();


        if(file_format>=8)
        {
            str_count++;line = in.readLine();
            if(SMBX64::qStr(line)) //Warp to level
                goto badfile;
            else doors.lname = removeQuotes(line);


            str_count++;line = in.readLine();
            if(SMBX64::Int(line)) //Normal entrance or Warp to other door
                goto badfile;
            else doors.warpto= line.toInt();

            str_count++;line = in.readLine();
            if(SMBX64::wBool(line)) //Level Entrance (cannot enter)
                goto badfile;
            else
            {
                doors.lvl_i = ((line=="#TRUE#")?true:false);
                doors.isSetIn = ((line=="#TRUE#")?false:true);
            }

            str_count++;line = in.readLine();
            if(SMBX64::wBool(line)) //Level Exit (End of level)
                goto badfile;
            else
            {
                doors.lvl_o= ((line=="#TRUE#")?true:false);
                doors.isSetOut = (((line=="#TRUE#")?false:true) || (doors.lvl_i));
            }

            str_count++;line = in.readLine();
            if(SMBX64::sInt(line)) //WarpTo X
                goto badfile;
            else doors.world_x= line.toInt();

            str_count++;line = in.readLine();
            if(SMBX64::sInt(line)) //WarpTo y
                goto badfile;
            else doors.world_y= line.toInt();
        }
        else
        {
            doors.isSetIn=true;
            doors.isSetOut=true;
        }

        if(file_format>=8)
        {
            str_count++;line = in.readLine();
            if(SMBX64::Int(line)) //Need a stars
                goto badfile;
            else doors.stars= line.toInt();
        } //else doors.stars=0;

        if(file_format>=10)
        {
            str_count++;line = in.readLine();
            if(SMBX64::qStr(line)) //Layer
                goto badfile;
            else doors.layer = removeQuotes(line);

            str_count++;line = in.readLine();
            if(SMBX64::wBool(line)) //<unused>, always FALSE
                goto badfile;
            else doors.unknown= ((line=="#TRUE#")?true:false);

        }/* else
        {
            doors.layer = "Default";
            doors.unknown = false;
        }*/

        if(file_format>=28)
        {
            str_count++;line = in.readLine();
            if(SMBX64::wBool(line)) //No Yoshi
                goto badfile;
            else doors.noyoshi = ((line=="#TRUE#")?true:false);

            str_count++;line = in.readLine();
            if(SMBX64::wBool(line)) //Allow NPC
                goto badfile;
            else doors.allownpc= ((line=="#TRUE#")?true:false);

            str_count++;line = in.readLine();
            if(SMBX64::wBool(line)) //Locked
                goto badfile;
            else doors.locked= ((line=="#TRUE#")?true:false);
        }
        /*
        else
        {
            doors.noyoshi=false;
            doors.allownpc=false;
            doors.locked=false;
        }*/
        doors.array_id = FileData.doors_array_id;
        FileData.doors_array_id++;
        doors.index = FileData.doors.size(); //Apply element index

    FileData.doors.push_back(doors); //Add NPC into array
    str_count++;line = in.readLine();
    }

    ////////////Water/QuickSand Data//////////
    if(file_format>=32)
    {
        str_count++;line = in.readLine();
        while(line!="\"next\"")
        {
            waters = dummyLvlWater();
            if(SMBX64::sInt(line)) //Water x
                goto badfile;
            else waters.x = line.toInt();

            str_count++;line = in.readLine();
            if(SMBX64::sInt(line)) //Water y
                goto badfile;
            else waters.y = line.toInt();

            str_count++;line = in.readLine();
            if(SMBX64::Int(line)) //Water w
                goto badfile;
            else waters.w = line.toInt();

            str_count++;line = in.readLine();
            if(SMBX64::Int(line)) //Water h
                goto badfile;
            else waters.h = line.toInt();

            str_count++;line = in.readLine();
            if(SMBX64::Int(line)) //Unused
                goto badfile;
            else waters.unknown = line.toInt();


            if(file_format>=63)
            {
                str_count++;line = in.readLine();
                if(SMBX64::wBool(line)) //Quicksand
                    goto badfile;
                else waters.quicksand = ((line=="#TRUE#")?true:false);
            }
            //else waters.quicksand = false;


            str_count++;line = in.readLine();
            if(SMBX64::qStr(line)) //Layer
                goto badfile;
            else waters.layer = removeQuotes(line);

            waters.array_id = FileData.water_array_id;
            FileData.water_array_id++;

            waters.index = FileData.water.size(); //Apply element index

        FileData.water.push_back(waters); //Add Water area into array
        str_count++;line = in.readLine();
        }
    }

    if(file_format>=10)
    {
        ////////////Layers Data//////////
        str_count++;line = in.readLine();
        while(line!="\"next\"")
        {
            if(SMBX64::qStr(line)) //Layer name
                goto badfile;
            else layers.name=removeQuotes(line);

            str_count++;line = in.readLine();
            if(SMBX64::wBool(line)) //hidden layer
                goto badfile;
            else layers.hidden = ((line=="#TRUE#")?true:false);

            layers.array_id = FileData.layers_array_id;
            FileData.layers_array_id++;

        FileData.layers.push_back(layers); //Add Water area into array
        str_count++;line = in.readLine();
        }

        ////////////Events Data//////////
        str_count++;line = in.readLine();
        while((line!="")&&(!line.isNull()))
        {
            events = dummyLvlEvent();
            if(SMBX64::qStr(line)) //Event name
                goto badfile;
            else events.name=removeQuotes(line);

            str_count++;line = in.readLine();
            if(SMBX64::qStr(line)) //Event message
                goto badfile;
            else events.msg=removeQuotes(line);


            if(file_format>=18)
            {
                str_count++;line = in.readLine();
                if(SMBX64::Int(line)) //PlaySound
                    goto badfile;
                else events.sound_id  = line.toInt();
            }
            //else events.sound_id  = 0;

            str_count++;line = in.readLine();
            if(SMBX64::Int(line)) //EndGame
                goto badfile;
            else events.end_game  = line.toInt();

            events.layers.clear();

            events.layers_hide.clear();
            events.layers_show.clear();
            events.layers_toggle.clear();

            for(i=0; i<21; i++)
            {
                str_count++;line = in.readLine();
                if(SMBX64::qStr(line)) //Hide layer
                    goto badfile;
                else events_layers.hide=removeQuotes(line);

                str_count++;line = in.readLine();
                if(SMBX64::qStr(line)) //Show layer
                    goto badfile;
                else events_layers.show=removeQuotes(line);


                str_count++;line = in.readLine();
                if(SMBX64::qStr(line)) //Toggle layer
                    goto badfile;
                else events_layers.toggle=removeQuotes(line);

                if(events_layers.hide!="") events.layers_hide.push_back(events_layers.hide);
                if(events_layers.show!="") events.layers_show.push_back(events_layers.show);
                if(events_layers.toggle!="") events.layers_toggle.push_back(events_layers.toggle);

                events.layers.push_back(events_layers);
            }

            events.sets.clear();
            for(i=0; i<21; i++)
            {
                str_count++;line = in.readLine();
                if(SMBX64::sInt(line)) //Set Music
                    goto badfile;
                else events_sets.music_id  = line.toInt();
                str_count++;line = in.readLine();
                if(SMBX64::sInt(line)) //Set Background
                    goto badfile;
                else events_sets.background_id = line.toInt();

                str_count++;line = in.readLine();
                if(SMBX64::sInt(line)) //Set Position to: LEFT
                    goto badfile;
                else events_sets.position_left = line.toInt();

                str_count++;line = in.readLine();
                if(SMBX64::sInt(line)) //Set Position to: TOP
                    goto badfile;
                else events_sets.position_top = line.toInt();

                str_count++;line = in.readLine();
                if(SMBX64::sInt(line)) //Set Position to: BOTTOM
                    goto badfile;
                else events_sets.position_bottom = line.toInt();

                str_count++;line = in.readLine();
                if(SMBX64::sInt(line)) //Set Position to: RIGHT
                    goto badfile;
                else events_sets.position_right = line.toInt();

            events.sets.push_back(events_sets);
            }

            if(file_format>=28)
            {
                str_count++;line = in.readLine();
                if(SMBX64::qStr(line)) //Trigger
                    goto badfile;
                else events.trigger=removeQuotes(line);

                str_count++;line = in.readLine();
                if(SMBX64::Int(line)) //Start trigger event after x [sec*10]. Etc. 153,2 sec
                    goto badfile;
                else events.trigger_timer = line.toInt();

                str_count++;line = in.readLine();
                if(SMBX64::wBool(line)) //No Smoke
                    goto badfile;
                else events.nosmoke = ((line=="#TRUE#")?true:false);

                str_count++;line = in.readLine();
                if(SMBX64::wBool(line)) //Hold ALT-JUMP player control
                    goto badfile;
                else events.altjump = ((line=="#TRUE#")?true:false);

                str_count++;line = in.readLine();
                if(SMBX64::wBool(line)) //ALT-RUN
                    goto badfile;
                else events.altrun = ((line=="#TRUE#")?true:false);

                str_count++;line = in.readLine();
                if(SMBX64::wBool(line)) //DOWN
                    goto badfile;
                else events.down = ((line=="#TRUE#")?true:false);

                str_count++;line = in.readLine();
                if(SMBX64::wBool(line)) //DROP
                    goto badfile;
                else events.drop = ((line=="#TRUE#")?true:false);

                str_count++;line = in.readLine();
                if(SMBX64::wBool(line)) //JUMP
                    goto badfile;
                else events.jump = ((line=="#TRUE#")?true:false);

                str_count++;line = in.readLine();
                if(SMBX64::wBool(line)) //LEFT
                    goto badfile;
                else events.left = ((line=="#TRUE#")?true:false);

                str_count++;line = in.readLine();
                if(SMBX64::wBool(line)) //RIGHT
                    goto badfile;
                else events.right = ((line=="#TRUE#")?true:false);

                str_count++;line = in.readLine();
                if(SMBX64::wBool(line)) //RUN
                    goto badfile;
                else events.run = ((line=="#TRUE#")?true:false);

                str_count++;line = in.readLine();
                if(SMBX64::wBool(line)) //START
                    goto badfile;
                else events.start = ((line=="#TRUE#")?true:false);

                str_count++;line = in.readLine();
                if(SMBX64::wBool(line)) //UP
                    goto badfile;
                else events.up = ((line=="#TRUE#")?true:false);
            }
            /*
            else
            {
                events.trigger= "";
                events.trigger_timer=0;
                events.nosmoke = false;
                events.altjump=false;
                events.altrun=false;
                events.down=false;
                events.drop=false;
                events.jump=false;
                events.left=false;
                events.right=false;
                events.run=false;
                events.start=false;
                events.up=false;
            }*/

            if(file_format>=32)
            {
                str_count++;line = in.readLine();
                if(SMBX64::wBool(line)) //Auto start
                    goto badfile;
                else events.autostart = ((line=="#TRUE#")?true:false);

                str_count++;line = in.readLine();
                if(SMBX64::qStr(line)) //Layer for movement
                    goto badfile;
                else events.movelayer = removeQuotes(line);

                str_count++;line = in.readLine();
                if(SMBX64::sFloat(line)) //Layer moving speed – horizontal
                    goto badfile;
                else events.layer_speed_x = line.replace(QChar(','), QChar('.')).toFloat();

                str_count++;line = in.readLine();
                if(SMBX64::sFloat(line)) //Layer moving speed – vertical
                    goto badfile;
                else events.layer_speed_y = line.replace(QChar(','), QChar('.')).toFloat();

            }
            /*
            else
            {
                events.autostart = false;
                events.movelayer = "";
                events.layer_speed_x = 0;
                events.layer_speed_x = 0;
            }*/


            if(file_format>=49)
            {
                str_count++;line = in.readLine();
                if(SMBX64::sFloat(line)) //Move screen horizontal speed
                    goto badfile;
                else events.move_camera_x = line.replace(QChar(','), QChar('.')).toFloat();

                str_count++;line = in.readLine();
                if(SMBX64::sFloat(line)) //Move screen vertical speed
                    goto badfile;
                else events.move_camera_y = line.replace(QChar(','), QChar('.')).toFloat();

                str_count++;line = in.readLine();
                if(SMBX64::sInt(line)) //Scroll section x, (in file value is x-1)
                    goto badfile;
                else events.scroll_section = line.toInt();
            }
            /*
            else
            {
                events.move_camera_x = 0;
                events.move_camera_y = 0;
                events.scroll_section = 0;
            }*/

            events.array_id = FileData.events_array_id;
            FileData.events_array_id++;

        FileData.events.push_back(events);
        str_count++;line = in.readLine();
        }
    }

    {
        //Add system layers if not exist
        bool def=false,desb=false,spawned=false;

        foreach(LevelLayers lr, FileData.layers)
        {
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
        foreach(LevelEvents ev, FileData.events)
        {
            if(ev.name=="Level - Start") lstart=true;
            else
            if(ev.name=="P Switch - Start") pstart=true;
            else
            if(ev.name=="P Switch - End") pend=true;
        }

        events = dummyLvlEvent();

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
    BadFileMsg(inf.fileName()+"\nFile format "+QString::number(file_format), str_count, line);
    FileData.ReadFileValid=false;
    return FileData;
}








//*********************************************************
//****************WRITE FILE FORMAT************************
//*********************************************************


QString FileFormats::WriteSMBX64LvlFile(LevelData FileData)
{
    QString TextData;
    int i, j;

    //Count placed stars on this level
    FileData.stars=0;
    for(i=0;i<FileData.npc.size();i++)
    {
        if(FileData.npc[i].is_star)
            FileData.stars++;
    }


    TextData += SMBX64::IntS(64);                     //Format version 64
    TextData += SMBX64::IntS(FileData.stars);         //Number of stars
    TextData += SMBX64::qStrS(FileData.LevelName);  //Level name


    //Sections settings
    for(i=0; i<FileData.sections.size() && i<21; i++)
    {
        TextData += SMBX64::IntS(FileData.sections[i].size_left);
        TextData += SMBX64::IntS(FileData.sections[i].size_top);
        TextData += SMBX64::IntS(FileData.sections[i].size_bottom);
        TextData += SMBX64::IntS(FileData.sections[i].size_right);
        TextData += SMBX64::IntS(FileData.sections[i].music_id);
        TextData += SMBX64::IntS(FileData.sections[i].bgcolor);
        TextData += SMBX64::BoolS(FileData.sections[i].IsWarp);
        TextData += SMBX64::BoolS(FileData.sections[i].OffScreenEn);
        TextData += SMBX64::IntS(FileData.sections[i].background);
        TextData += SMBX64::BoolS(FileData.sections[i].noback);
        TextData += SMBX64::BoolS(FileData.sections[i].underwater);
        TextData += SMBX64::qStrS(FileData.sections[i].music_file);
    }
    for( ; i<21 ; i++) //Protector
        TextData += "0\n0\n0\n0\n0\n16291944\n#FALSE#\n#FALSE#\n0\n#FALSE#\n#FALSE#\n\"\"\n";
        //append dummy section data, if array size is less than 21

    //Players start point
    for(i=0; i<FileData.players.size() && i<2; i++ )
    {
        TextData += SMBX64::IntS(FileData.players[i].x);
        TextData += SMBX64::IntS(FileData.players[i].y);
        TextData += SMBX64::IntS(FileData.players[i].w);
        TextData += SMBX64::IntS(FileData.players[i].h);
    }
    for( ;i<2; i++ ) //Protector
        TextData += "0\n0\n0\n0\n";


    //Blocks
    QMap<long, QMap<long, QMap<long, LevelBlock > > > sortedBlocks;
    foreach(LevelBlock block, FileData.blocks)
    {
        sortedBlocks[block.x][block.y][block.array_id] = block;
    }

    //for(i=0; i<FileData.blocks.size(); i++)
    //{
    for (QMap<long, QMap<long, QMap<long, LevelBlock > > >::iterator bArr = sortedBlocks.begin(); bArr != sortedBlocks.end(); bArr++)
    {
      for (QMap<long, QMap<long, LevelBlock > >::iterator bBrr = (* bArr).begin(); bBrr != (* bArr).end(); bBrr++)
        for (QMap<long, LevelBlock >::iterator block = (* bBrr).begin(); block != (*bBrr).end(); block++)
        {
        TextData += SMBX64::IntS((*block).x);
        TextData += SMBX64::IntS((*block).y);
        TextData += SMBX64::IntS((*block).h);
        TextData += SMBX64::IntS((*block).w);
        TextData += SMBX64::IntS((*block).id);
        TextData += SMBX64::IntS((*block).npc_id);
        TextData += SMBX64::BoolS((*block).invisible);
        TextData += SMBX64::BoolS((*block).slippery);
        TextData += SMBX64::qStrS((*block).layer);
        TextData += SMBX64::qStrS((*block).event_destroy);
        TextData += SMBX64::qStrS((*block).event_hit);
        TextData += SMBX64::qStrS((*block).event_no_more);
        }
    }
    TextData += "\"next\"\n";//Separator


    //BGOs
    QMap<long, QMap<long, QMap<long, QMap<long, LevelBGO > > > > sortedBGO;
    foreach(LevelBGO bgo1, FileData.bgo)
    {
        sortedBGO[bgo1.smbx64_sp][bgo1.x][bgo1.y][bgo1.array_id] = bgo1;
    }

    //for(i=0; i<FileData.bgo.size(); i++)
    for (QMap<long, QMap<long, QMap<long, QMap<long, LevelBGO > > > >::iterator sArr = sortedBGO.begin(); sArr != sortedBGO.end(); sArr++)
    {
        for (QMap<long, QMap<long, QMap<long, LevelBGO > > >::iterator bArr = (* sArr).begin(); bArr != (* sArr).end(); bArr++)
          for (QMap<long, QMap<long, LevelBGO > >::iterator bBrr = (* bArr).begin(); bBrr != (* bArr).end(); bBrr++)
            for (QMap<long, LevelBGO >::iterator bgo = (* bBrr).begin(); bgo != (*bBrr).end(); bgo++)
            {
            TextData += SMBX64::IntS( (*bgo).x);
            TextData += SMBX64::IntS( (*bgo).y);
            TextData += SMBX64::IntS( (*bgo).id);
            TextData += SMBX64::qStrS( (*bgo).layer);
            }
    }
    TextData += "\"next\"\n";//Separator

    //NPCs
    for(i=0; i<FileData.npc.size(); i++)
    {
        //Section size
        TextData += SMBX64::IntS(FileData.npc[i].x);
        TextData += SMBX64::IntS(FileData.npc[i].y);
        TextData += SMBX64::IntS(FileData.npc[i].direct);
        TextData += SMBX64::IntS(FileData.npc[i].id);

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
        TextData += SMBX64::IntS(FileData.npc[i].special_data);

            if((FileData.npc[i].id==91)&&(FileData.npc[i].special_data==288)) // Warp Section value for included into herb magic potion
                TextData += SMBX64::IntS(FileData.npc[i].special_data2);

            break;
            default:
                break;
        }

        TextData += SMBX64::BoolS(FileData.npc[i].generator);

        if(FileData.npc[i].generator)
        {
            TextData += SMBX64::IntS(FileData.npc[i].generator_direct);
            TextData += SMBX64::IntS(FileData.npc[i].generator_type);
            TextData += SMBX64::IntS(FileData.npc[i].generator_period);
        }
        TextData += SMBX64::qStrS(FileData.npc[i].msg);

        TextData += SMBX64::BoolS(FileData.npc[i].friendly);
        TextData += SMBX64::BoolS(FileData.npc[i].nomove);
        TextData += SMBX64::BoolS(FileData.npc[i].legacyboss);

        TextData += SMBX64::qStrS(FileData.npc[i].layer);
        TextData += SMBX64::qStrS(FileData.npc[i].event_activate);
        TextData += SMBX64::qStrS(FileData.npc[i].event_die);
        TextData += SMBX64::qStrS(FileData.npc[i].event_talk);
        TextData += SMBX64::qStrS(FileData.npc[i].event_nomore);
        TextData += SMBX64::qStrS(FileData.npc[i].attach_layer);
    }
    TextData += "\"next\"\n";//Separator


    //Doors
    for(i=0; i<FileData.doors.size(); i++)
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
        TextData += SMBX64::qStrS(FileData.doors[i].lname);
        TextData += SMBX64::IntS(FileData.doors[i].warpto);
        TextData += SMBX64::BoolS(FileData.doors[i].lvl_i);
        TextData += SMBX64::BoolS(FileData.doors[i].lvl_o);
        TextData += SMBX64::IntS(FileData.doors[i].world_x);
        TextData += SMBX64::IntS(FileData.doors[i].world_y);
        TextData += SMBX64::IntS(FileData.doors[i].stars);
        TextData += SMBX64::qStrS(FileData.doors[i].layer);
        TextData += SMBX64::BoolS(FileData.doors[i].unknown);
        TextData += SMBX64::BoolS(FileData.doors[i].noyoshi);
        TextData += SMBX64::BoolS(FileData.doors[i].allownpc);
        TextData += SMBX64::BoolS(FileData.doors[i].locked);
    }
    TextData += "\"next\"\n";//Separator

    //Water
    for(i=0; i<FileData.water.size(); i++)
    {
        TextData += SMBX64::IntS(FileData.water[i].x);
        TextData += SMBX64::IntS(FileData.water[i].y);
        TextData += SMBX64::IntS(FileData.water[i].w);
        TextData += SMBX64::IntS(FileData.water[i].h);
        TextData += SMBX64::IntS(FileData.water[i].unknown);
        TextData += SMBX64::BoolS(FileData.water[i].quicksand);
        TextData += SMBX64::qStrS(FileData.water[i].layer);
    }
    TextData += "\"next\"\n";//Separator

    //Layers
    for(i=0; i<FileData.layers.size(); i++)
    {
        TextData += SMBX64::qStrS(FileData.layers[i].name);
        TextData += SMBX64::BoolS(FileData.layers[i].hidden);
    }
    TextData += "\"next\"\n";//Separator

    LevelEvents_layers layerSet;
    for(i=0; i<FileData.events.size(); i++)
    {
        TextData += SMBX64::qStrS(FileData.events[i].name);
        TextData += SMBX64::qStrS(FileData.events[i].msg);
        TextData += SMBX64::IntS(FileData.events[i].sound_id);
        TextData += SMBX64::IntS(FileData.events[i].end_game);

        FileData.events[i].layers.clear();
        for(j=0; j<20; j++)
        {
            layerSet.hide =
                    ((j<FileData.events[i].layers_hide.size())?
                      FileData.events[i].layers_hide[j] : "");
            layerSet.show =
                    ((j<FileData.events[i].layers_show.size())?
                      FileData.events[i].layers_show[j] : "");;
            layerSet.toggle =
                    ((j<FileData.events[i].layers_toggle.size())?
                      FileData.events[i].layers_toggle[j] : "");

            FileData.events[i].layers.push_back(layerSet);
        }

        for(j=0; j< FileData.events[i].layers.size()  && j<20; j++)
        {
            TextData += SMBX64::qStrS(FileData.events[i].layers[j].hide);
            TextData += SMBX64::qStrS(FileData.events[i].layers[j].show);
            TextData += SMBX64::qStrS(FileData.events[i].layers[j].toggle);
        }
        for( ; j<21; j++)
            TextData += "\"\"\n\"\"\n\"\"\n"; //(21st element is SMBX 1.3 bug protector)

        for(j=0; j< FileData.events[i].sets.size()  && j<21; j++)
        {
            TextData += SMBX64::IntS(FileData.events[i].sets[j].music_id);
            TextData += SMBX64::IntS(FileData.events[i].sets[j].background_id);
            TextData += SMBX64::IntS(FileData.events[i].sets[j].position_left);
            TextData += SMBX64::IntS(FileData.events[i].sets[j].position_top);
            TextData += SMBX64::IntS(FileData.events[i].sets[j].position_bottom);
            TextData += SMBX64::IntS(FileData.events[i].sets[j].position_right);
        }
        for( ; j<21; j++) // Protector
            TextData += "0\n0\n0\n-1\n-1\n-1\n";

        TextData += SMBX64::qStrS(FileData.events[i].trigger);
        TextData += SMBX64::IntS(FileData.events[i].trigger_timer);

        TextData += SMBX64::BoolS(FileData.events[i].nosmoke);

        TextData += SMBX64::BoolS(FileData.events[i].altjump);
        TextData += SMBX64::BoolS(FileData.events[i].altrun);
        TextData += SMBX64::BoolS(FileData.events[i].down);
        TextData += SMBX64::BoolS(FileData.events[i].drop);
        TextData += SMBX64::BoolS(FileData.events[i].jump);
        TextData += SMBX64::BoolS(FileData.events[i].left);
        TextData += SMBX64::BoolS(FileData.events[i].right);
        TextData += SMBX64::BoolS(FileData.events[i].run);
        TextData += SMBX64::BoolS(FileData.events[i].start);
        TextData += SMBX64::BoolS(FileData.events[i].up);

        TextData += SMBX64::BoolS(FileData.events[i].autostart);

        TextData += SMBX64::qStrS(FileData.events[i].movelayer);
        TextData += SMBX64::FloatS(FileData.events[i].layer_speed_x);
        TextData += SMBX64::FloatS(FileData.events[i].layer_speed_y);
        TextData += SMBX64::FloatS(FileData.events[i].move_camera_x);
        TextData += SMBX64::FloatS(FileData.events[i].move_camera_y);
        TextData += SMBX64::IntS(FileData.events[i].scroll_section);
    }

    return TextData;
}
