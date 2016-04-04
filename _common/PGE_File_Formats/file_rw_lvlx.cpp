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
#include "file_strlist.h"
#include "pge_x.h"
#include "pge_x_macro.h"

#ifdef PGE_EDITOR
#include <script/commands/memorycommand.h>
#endif
#ifdef PGE_FILES_USE_MESSAGEBOXES
#include <QMessageBox>
#endif


//*********************************************************
//****************READ FILE FORMAT*************************
//*********************************************************
bool FileFormats::ReadExtendedLvlFileHeader(PGESTRING filePath, LevelData &FileData)
{
    CreateLevelHeader(FileData);
    FileData.RecentFormat = LevelData::PGEX;

    PGE_FileFormats_misc::TextFileInput inf;
    if(!inf.open(filePath, true))
    {
        FileData.ReadFileValid=false;
        return false;
    }

    PGESTRING line;
    int str_count=0;
    bool valid=false;
    PGE_FileFormats_misc::FileInfo in_1(filePath);
    FileData.filename = in_1.basename();
    FileData.path = in_1.dirpath();

    #define NextLine(line) str_count++;line = inf.readLine();

    //Find level header part
    do{
    str_count++;NextLine(line);
    }while((line!="HEAD") && (!IsNULL(line)));

    PGESTRINGList header;
    NextLine(line);
    bool closed=false;
    while((line!="HEAD_END") && (!IsNULL(line)))
    {
        header.push_back(line);
        str_count++;NextLine(line);
        if(line=="HEAD_END") closed=true;
    }
    if(!closed) goto badfile;

    for(int qq=0; qq<(signed)header.size(); qq++)
    {
        PGESTRING &header_line=header[qq];
        PGELIST<PGESTRINGList >data = PGEFile::splitDataLine(header_line, &valid);

        for(int i=0;i<(signed)data.size();i++)
        {
            if(data[i].size()!=2) goto badfile;
            if(data[i][0]=="TL") //Level Title
            {
                if(PGEFile::IsQoutedString(data[i][1]))
                    FileData.LevelName = PGEFile::X2STRING(data[i][1]);
                else
                    goto badfile;
            }
            else
            if(data[i][0]=="SZ") //Starz number
            {
                if(PGEFile::IsIntU(data[i][1]))
                    FileData.stars = toInt(data[i][1]);
                else
                    goto badfile;
            }
            else
            if(data[i][0]=="DL") //Open Level on player's fail
            {
                if(PGEFile::IsQoutedString(data[i][1]))
                    FileData.open_level_on_fail = PGEFile::X2STRING(data[i][1]);
                else
                    goto badfile;
            }
            else
            if(data[i][0]=="DE") //Target WarpID of fail-level entrace
            {
                if(PGEFile::IsIntU(data[i][1]))
                    FileData.open_level_on_fail_warpID = toInt(data[i][1]);
                else
                    goto badfile;
            }
        }
    }

    if(!closed)
        goto badfile;

    FileData.CurSection=0;
    FileData.playmusic=0;

    FileData.ReadFileValid=true;

    return true;

badfile:
    FileData.ERROR_info="Invalid file format";
    FileData.ERROR_linenum=str_count;
    FileData.ERROR_linedata=line;
    FileData.ReadFileValid=false;
    return false;
}



bool FileFormats::ReadExtendedLvlFileF(PGESTRING  filePath, LevelData &FileData)
{
    errorString.clear();
    PGE_FileFormats_misc::TextFileInput file;
    if(!file.open(filePath, true))
    {
        errorString="Failed to open file for read";
        return false;
    }
    return ReadExtendedLvlFile(file, FileData);
}

bool FileFormats::ReadExtendedLvlFileRaw(PGESTRING &rawdata, PGESTRING  filePath,  LevelData &FileData)
{
    errorString.clear();
    PGE_FileFormats_misc::RawTextInput file;
    if(!file.open(&rawdata, filePath))
    {
        errorString="Failed to open raw string for read";
        return false;
    }
    return ReadExtendedLvlFile(file, FileData);
}

bool FileFormats::ReadExtendedLvlFile(PGE_FileFormats_misc::TextInput &in, LevelData &FileData)
{
    PGESTRING errorString;
    PGESTRING filePath = in.getFilePath();
    PGESTRING line;  /*Current Line data*/
    //LevelData FileData;
    CreateLevelData(FileData);
    FileData.RecentFormat = LevelData::PGEX;

    //Add path data
    if(filePath.size() > 0)
    {
        PGE_FileFormats_misc::FileInfo  in_1(filePath);
        FileData.filename = in_1.basename();
        FileData.path = in_1.dirpath();
    }

    FileData.untitled = false;
    FileData.modified = false;

    LevelSection lvl_section;

    PlayerPoint player;
    LevelBlock block;
    LevelBGO bgodata;
    LevelNPC npcdata;
    LevelDoor door;
    LevelPhysEnv physiczone;
    LevelLayer layer;
    LevelSMBX64Event event;
    LevelVariable variable;
    LevelScript script;

    ///////////////////////////////////////Begin file///////////////////////////////////////
    PGEX_FileParseTree(in.readAll());

    PGEX_FetchSection() //look sections
    {
        PGEX_FetchSection_begin()
        ///////////////////HEADER//////////////////////
        PGEX_Section("HEAD")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_StrVal("TL", FileData.LevelName) //Level Title
                    PGEX_UIntVal("SZ", FileData.stars) //Starz number
                    PGEX_StrVal("DL", FileData.open_level_on_fail) //Open level on fail
                    PGEX_UIntVal("DE", FileData.open_level_on_fail_warpID) //Open level's warpID on fail
                }
            }
        }//HEADER

        ///////////////////////////////MetaDATA/////////////////////////////////////////////
        PGEX_Section("META_BOOKMARKS")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);

            PGEX_Items()
            {
                PGEX_ItemBegin(PGEFile::PGEX_Struct);

                Bookmark meta_bookmark;
                meta_bookmark.bookmarkName = "";
                meta_bookmark.x = 0;
                meta_bookmark.y = 0;

                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_StrVal("BM", meta_bookmark.bookmarkName) //Bookmark name
                    PGEX_SIntVal("X", meta_bookmark.x) // Position X
                    PGEX_SIntVal("Y", meta_bookmark.y) // Position Y
                }
                FileData.metaData.bookmarks.push_back(meta_bookmark);
            }
        }

        ////////////////////////meta bookmarks////////////////////////
        #ifdef PGE_EDITOR
        PGEX_Section("META_SYS_CRASH")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);

            PGEX_Items()
            {
                PGEX_ItemBegin(PGEFile::PGEX_Struct);

                FileData.metaData.crash.used=true;
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_BoolVal("UT", FileData.metaData.crash.untitled) //Untitled
                    PGEX_BoolVal("MD", FileData.metaData.crash.modifyed) //Modyfied
                    PGEX_StrVal ("N",  FileData.metaData.crash.filename) //Filename
                    PGEX_StrVal ("P",  FileData.metaData.crash.path) //Path
                    PGEX_StrVal ("FP", FileData.metaData.crash.fullPath) //Full file Path
                }
            }
        }//meta sys crash

        PGEX_Section("META_SCRIPT_EVENTS")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);

            if(f_section.subTree.size()>0)
            {
                if(!FileData.metaData.script)
                    FileData.metaData.script.reset(new ScriptHolder());
            }

            //Read subtree
            for(int subtree=0;subtree<f_section.subTree.size();subtree++)
            {
                if(f_section.subTree[subtree].name=="EVENT")
                {
                    if(f_section.type!=PGEFile::PGEX_Struct)
                    {
                        errorString=PGESTRING("Wrong section data syntax:\nSection [%1]\nSubtree [%2]").
                                    arg(f_section.name).
                                    arg(f_section.subTree[subtree].name);
                        goto badfile;
                    }

                    EventCommand * event = new EventCommand(EventCommand::EVENTTYPE_LOAD);

                    for(int sdata=0;sdata<f_section.subTree[subtree].data.size();sdata++)
                    {
                        if(f_section.subTree[subtree].data[sdata].type!=PGEFile::PGEX_Struct)
                        {
                            errorString=PGESTRING("Wrong data item syntax:\nSubtree [%1]\nData line %2")
                                    .arg(f_section.subTree[subtree].name)
                                    .arg(sdata);
                            goto badfile;
                        }

                        PGEFile::PGEX_Item x = f_section.subTree[subtree].data[sdata];

                        //Get values
                        for(int sval=0;sval<x.values.size();sval++) //Look markers and values
                        {
                            PGEX_ValueBegin()
                            if(v.marker=="TL") //Marker
                            {
                                if(PGEFile::IsQoutedString(v.value))
                                    event->setMarker(PGEFile::X2STRING(v.value));
                                else
                                    goto badfile;
                            }
                            else
                            if(v.marker=="ET") //Event type
                            {
                                if(PGEFile::IsIntS(v.value))
                                    event->setEventType( (EventCommand::EventType)toInt(v.value) );
                                else
                                    goto badfile;
                            }
                        }
                    }//Event header

                    //Basic commands subtree
                    if(event->eventType()==EventCommand::EVENTTYPE_LOAD)
                    {
                        for(int subtree2=0;subtree2<f_section.subTree[subtree].subTree.size();subtree2++)
                        {
                            if(f_section.subTree[subtree2].subTree[subtree2].name=="BASIC_COMMANDS")
                            {
                                if(f_section.type!=PGEFile::PGEX_Struct)
                                {
                                    errorString=PGESTRING("Wrong section data syntax:\nSection [%1]\nSubtree [%2]\nSubtree [%2]").
                                                arg(f_section.name).
                                                arg(f_section.subTree[subtree].name).
                                                arg(f_section.subTree[subtree].subTree[subtree2].name);
                                    goto badfile;
                                }

                                for(int sdata=0;sdata<f_section.subTree[subtree].subTree[subtree2].data.size();sdata++)
                                {
                                    if(f_section.subTree[subtree].subTree[subtree2].data[sdata].type!=PGEFile::PGEX_Struct)
                                    {
                                        errorString=PGESTRING("Wrong data item syntax:\nSubtree [%1]\nData line %2")
                                                .arg(f_section.subTree[subtree].subTree[subtree2].name)
                                                .arg(sdata);
                                        goto badfile;
                                    }

                                    PGEFile::PGEX_Item x = f_section.subTree[subtree].subTree[subtree2].data[sdata];

                                    PGESTRING name="";
                                    PGESTRING commandType="";
                                    int hx=0;
                                    int ft=0;
                                    double vf=0.0;

                                    //Get values
                                    PGEX_Values() //Look markers and values
                                    {
                                        PGEX_ValueBegin()
                                        PGEX_StrVal("N", name) //Command name
                                        PGEX_StrVal("CT", commandType) //Command type
                                        //MemoryCommand specific values
                                        PGEX_SIntVal("HX", hx) //Heximal value
                                        PGEX_SIntVal("FT", ft) //Field type
                                        PGEX_FloatVal("V", vf) //Value
                                    }

                                    if(commandType=="MEMORY")
                                    {
                                        MemoryCommand *cmd =
                                           new MemoryCommand(hx,(MemoryCommand::FieldType)ft, vf);
                                        cmd->setMarker(name);
                                        event->addBasicCommand(cmd);
                                    }
                                }//commands list
                            }//Basic command subtree
                        }//subtrees
                    }

                    FileData.metaData.script->revents() << event;
                }//EVENT tree

            }//META_SCRIPT_EVENTS subtree
        }//META_SCRIPT_EVENTS Section
        #endif
        ///////////////////////////////MetaDATA//End////////////////////////////////////////

        ///////////////////SECTION//////////////////////
        PGEX_Section("SECTION")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);

            PGEX_Items()
            {
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                lvl_section = CreateLvlSection();

                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()

                    PGEX_UIntVal("SC", lvl_section.id) //Section ID
                    PGEX_SIntVal("L",  lvl_section.size_left) //Left side
                    PGEX_SIntVal("R",  lvl_section.size_right)//Right side
                    PGEX_SIntVal("T",  lvl_section.size_top) //Top side
                    PGEX_SIntVal("B",  lvl_section.size_bottom)//Bottom side
                    PGEX_UIntVal("MZ", lvl_section.music_id)//Stuff music ID
                    PGEX_UIntVal("BG", lvl_section.background)//Stuff music ID
                    PGEX_StrVal ("MF", lvl_section.music_file)//External music file path
                    PGEX_BoolVal("CS", lvl_section.wrap_h)//Connect sides horizontally
                    PGEX_BoolVal("CSV", lvl_section.wrap_v)//Connect sides vertically
                    PGEX_BoolVal("OE", lvl_section.OffScreenEn)//Offscreen exit
                    PGEX_BoolVal("SR", lvl_section.lock_left_scroll)//Right-way scroll only (No Turn-back)
                    PGEX_BoolVal("SL", lvl_section.lock_right_scroll)//Left-way scroll only (No Turn-forward)
                    PGEX_BoolVal("SD", lvl_section.lock_up_scroll)//Down-way scroll only (No Turn-forward)
                    PGEX_BoolVal("SU", lvl_section.lock_down_scroll)//Up-way scroll only (No Turn-forward)
                    PGEX_BoolVal("UW", lvl_section.underwater)//Underwater bit
                }
                lvl_section.PositionX=lvl_section.size_left-10;
                lvl_section.PositionY=lvl_section.size_top-10;

                //add captured value into array
                bool found=false;
                int q=0;

                if(lvl_section.id >= (signed)FileData.sections.size())
                {
                    int needToAdd = (FileData.sections.size()-1) - lvl_section.id;
                    while(needToAdd > 0)
                    {
                        LevelSection dummySct = CreateLvlSection();
                        dummySct.id = FileData.sections.size();
                        FileData.sections.push_back(dummySct);
                        needToAdd--;
                    }
                }

                for(q=0; q<(signed)FileData.sections.size();q++)
                {
                    if(FileData.sections[q].id==lvl_section.id){found=true; break;}
                }
                if(found)
                    FileData.sections[q] = lvl_section;
                else
                {
                    FileData.sections.push_back(lvl_section);
                }
            }
        }//SECTION



        ///////////////////STARTPOINT//////////////////////
        PGEX_Section("STARTPOINT")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);

            PGEX_Items()
            {
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                player = CreateLvlPlayerPoint();

                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_UIntVal("ID", player.id) //ID of player point
                    PGEX_SIntVal("X",  player.x)
                    PGEX_SIntVal("Y",  player.y)
                    PGEX_SIntVal("D",  player.direction)
                }

                //add captured value into array
                bool found=false;
                int q=0;
                for(q=0; q<(signed)FileData.players.size();q++)
                {
                    if(FileData.players[q].id==player.id){found=true; break;}
                }

                PlayerPoint sz = CreateLvlPlayerPoint(player.id);
                player.w = sz.w;
                player.h = sz.h;

                if(found)
                    FileData.players[q] = player;
                else
                    FileData.players.push_back(player);
            }
        }//STARTPOINT


        ///////////////////BLOCK//////////////////////
        PGEX_Section("BLOCK")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);

            PGEX_Items()
            {
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                block = CreateLvlBlock();

                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_UIntVal("ID", block.id) //Block ID
                    PGEX_SIntVal("X", block.x) // Position X
                    PGEX_SIntVal("Y", block.y) //Position Y
                    PGEX_UIntVal("W", block.w) //Width
                    PGEX_UIntVal("H", block.h) //Height
                    PGEX_SIntVal("CN", block.npc_id) //Contains (coins/NPC)
                    PGEX_BoolVal("IV", block.invisible) //Invisible
                    PGEX_BoolVal("SL", block.slippery) //Slippery
                    PGEX_StrVal("LR", block.layer) //Layer name
                    PGEX_StrVal("ED", block.event_destroy) //Destroy event slot
                    PGEX_StrVal("EH", block.event_hit) //Hit event slot
                    PGEX_StrVal("EE", block.event_emptylayer) //Hit event slot
                }

                block.array_id = FileData.blocks_array_id++;
                block.index = FileData.blocks.size();
                FileData.blocks.push_back(block);
            }
        }//BLOCK


        ///////////////////BGO//////////////////////
        PGEX_Section("BGO")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);

            PGEX_Items()
            {
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                bgodata = CreateLvlBgo();

                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_UIntVal ("ID", bgodata.id) //BGO ID
                    PGEX_SIntVal ("X",  bgodata.x) //X Position
                    PGEX_SIntVal ("Y",  bgodata.y) //Y Position
                    PGEX_FloatVal("ZO", bgodata.z_offset) //Z Offset
                    PGEX_SIntVal ("ZP", bgodata.z_mode) //Z Position
                    PGEX_SIntVal ("SP", bgodata.smbx64_sp) //SMBX64 Sorting priority
                    PGEX_StrVal  ("LR", bgodata.layer) //Layer name
                }

                bgodata.array_id = FileData.bgo_array_id++;
                bgodata.index = FileData.bgo.size();
                FileData.bgo.push_back(bgodata);
            }
        }//BGO


        ///////////////////NPC//////////////////////
        PGEX_Section("NPC")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);

            PGEX_Items()
            {
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                npcdata = CreateLvlNpc();

                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()

                    PGEX_UIntVal("ID", npcdata.id) //NPC ID
                    PGEX_SIntVal("X", npcdata.x) //X position
                    PGEX_SIntVal("Y", npcdata.y) //Y position
                    PGEX_SIntVal("D", npcdata.direct) //Direction
                    PGEX_SIntVal("CN", npcdata.contents) //Contents of container-NPC
                    PGEX_SIntVal("S1", npcdata.special_data) //Special value 1
                    PGEX_SIntVal("S2", npcdata.special_data2) //Special value 2
                    PGEX_BoolVal("GE", npcdata.generator) //Generator
                    PGEX_SIntVal("GT", npcdata.generator_type) //Generator type
                    PGEX_SIntVal("GD", npcdata.generator_direct) //Generator direction
                    PGEX_UIntVal("GM", npcdata.generator_period) //Generator period
                    PGEX_FloatVal("GA", npcdata.generator_custom_angle) //Generator custom angle
                    PGEX_UIntVal("GB",  npcdata.generator_branches) //Generator number of branches
                    PGEX_FloatVal("GR", npcdata.generator_angle_range) //Generator angle range
                    PGEX_FloatVal("GS", npcdata.generator_initial_speed) //Generator custom initial speed
                    PGEX_StrVal("MG", npcdata.msg) //Message
                    PGEX_BoolVal("FD", npcdata.friendly) //Friendly
                    PGEX_BoolVal("NM", npcdata.nomove) //Don't move
                    PGEX_BoolVal("BS", npcdata.is_boss) //Enable boss mode!
                    PGEX_StrVal("LR", npcdata.layer) //Layer
                    PGEX_StrVal("LA", npcdata.attach_layer) //Attach Layer
                    PGEX_StrVal("SV", npcdata.send_id_to_variable) //Send ID to variable
                    PGEX_StrVal("EA", npcdata.event_activate) //Event slot "Activated"
                    PGEX_StrVal("ED", npcdata.event_die) //Event slot "Death/Take/Destroy"
                    PGEX_StrVal("ET", npcdata.event_talk) //Event slot "Talk"
                    PGEX_StrVal("EE", npcdata.event_emptylayer) //Event slot "Layer is empty"
                    PGEX_StrVal("EG", npcdata.event_grab)//Event slot "On grab"
                    PGEX_StrVal("EO", npcdata.event_touch)//Event slot "On touch"
                    PGEX_StrVal("EF", npcdata.event_nextframe)//Evemt slot "Trigger every frame"
                }

                npcdata.array_id = FileData.npc_array_id++;
                npcdata.index = FileData.npc.size();
                FileData.npc.push_back(npcdata);
            }
        }//TILES

        ///////////////////PHYSICS//////////////////////
        PGEX_Section("PHYSICS")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);

            PGEX_Items()
            {
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                physiczone = CreateLvlPhysEnv();

                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_UIntVal("ET", physiczone.env_type) //Environment type
                    PGEX_SIntVal("X",  physiczone.x) //X position
                    PGEX_SIntVal("Y",  physiczone.y) //Y position
                    PGEX_UIntVal("W",  physiczone.w) //Width
                    PGEX_UIntVal("H",  physiczone.h) //Height
                    PGEX_StrVal ("LR", physiczone.layer) //Layer
                    PGEX_FloatVal("FR",physiczone.friction) //Friction
                    PGEX_FloatVal("AD",physiczone.accel_direct) //Custom acceleration direction
                    PGEX_FloatVal("AC",physiczone.accel) //Custom acceleration
                    PGEX_FloatVal("MV",physiczone.max_velocity) //Maximal velocity
                    PGEX_StrVal("EO",  physiczone.touch_event) //Touch event/script
                }
                physiczone.array_id = FileData.physenv_array_id++;
                physiczone.index = FileData.physez.size();
                FileData.physez.push_back(physiczone);
            }
        }//PHYSICS

        ///////////////////DOORS//////////////////////
        PGEX_Section("DOORS")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);

            PGEX_Items()
            {
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                door = CreateLvlWarp();

                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_SIntVal("IX", door.ix) //Input point
                    PGEX_SIntVal("IY", door.iy) //Input point
                    PGEX_SIntVal("OX", door.ox) //Output point
                    PGEX_SIntVal("OY", door.oy) //Output point
                    PGEX_UIntVal("IL", door.length_i) //Length of entrance (input) point
                    PGEX_UIntVal("OL", door.length_o) //Length of exit (output) point
                    PGEX_UIntVal("DT", door.type) //Input point
                    PGEX_UIntVal("ID", door.idirect) //Input direction
                    PGEX_UIntVal("OD", door.odirect) //Output direction
                    PGEX_SIntVal("WX", door.world_x) //Target world map point
                    PGEX_SIntVal("WY", door.world_y) //Target world map point
                    PGEX_StrVal ("LF", door.lname) //Target level file
                    PGEX_UIntVal("LI", door.warpto) //Target level file's input warp
                    PGEX_BoolVal("ET", door.lvl_i) //Level Entrance
                    PGEX_BoolVal("EX", door.lvl_o) //Level exit
                    PGEX_UIntVal("SL", door.stars) //Stars limit
                    PGEX_StrVal ("SM", door.stars_msg) //Message about stars/leeks
                    PGEX_BoolVal("NV", door.novehicles) //No Vehicles
                    PGEX_BoolVal("SH", door.star_num_hide) //Don't show stars number
                    PGEX_BoolVal("AI", door.allownpc) //Allow grabbed items
                    PGEX_BoolVal("LC", door.locked) //Door is locked
                    PGEX_BoolVal("LB", door.need_a_bomb) //Door is blocked, need bomb to unlock
                    PGEX_BoolVal("HS", door.hide_entering_scene) //Don't show entering scene
                    PGEX_BoolVal("AL", door.allownpc_interlevel) //Allow NPC's inter-level
                    PGEX_BoolVal("SR", door.special_state_required) //Required a special state to enter
                    PGEX_BoolVal("PT", door.cannon_exit) //Cannon exit
                    PGEX_FloatVal("PS", door.cannon_exit_speed) //Cannon exit speed
                    PGEX_StrVal ("LR", door.layer) //Layer
                    PGEX_StrVal ("EE", door.event_enter) //On-Enter event slot
                    PGEX_BoolVal("TW", door.two_way) //Two-way warp
                }

                door.isSetIn = ( !door.lvl_i );
                door.isSetOut = ( !door.lvl_o || (door.lvl_i));

                door.array_id = FileData.doors_array_id++;
                door.index = FileData.doors.size();
                FileData.doors.push_back(door);
            }
        }//DOORS

        ///////////////////LAYERS//////////////////////
        PGEX_Section("LAYERS")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                layer = CreateLvlLayer();
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_StrVal ("LR", layer.name) //Layer name
                    PGEX_BoolVal("HD", layer.hidden) //Hidden
                    PGEX_BoolVal("LC", layer.locked) //Locked
                }

                //add captured value into array
                bool found=false;
                int q=0;
                for(q=0; q<(signed)FileData.layers.size();q++)
                {
                    if(FileData.layers[q].name==layer.name){found=true; break;}
                }
                if(found)
                {
                    layer.array_id = FileData.layers[q].array_id;
                    FileData.layers[q] = layer;
                }
                else
                {
                    layer.array_id = FileData.layers_array_id++;
                    FileData.layers.push_back(layer);
                }
            }
        }//LAYERS

        //EVENTS comming soon
//                else
//                if(sct.first=="EVENTS_CLASSIC") //Action-styled events
//                {
//                    foreach(PGESTRINGList value, sectData) //Look markers and values
//                    {
//                            //  if(v.marker=="TL") //Level Title
//                            //  {
//                            //      if(PGEFile::IsQStr(v.value))
//                            //          FileData.LevelName = PGEFile::X2STR(v.value);
//                            //      else
//                            //          goto badfile;
//                            //  }
//                            //  else
//                            //  if(v.marker=="SZ") //Starz number
//                            //  {
//                            //      if(PGEFile::IsIntU(v.value))
//                            //          FileData.stars = toInt(v.value);
//                            //      else
//                            //          goto badfile;
//                            //  }
//                    }
//                }//EVENTS

        ///////////////////EVENTS_CLASSIC//////////////////////
        PGEX_Section("EVENTS_CLASSIC")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);

            PGEX_Items()
            {
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                event = CreateLvlEvent();

                PGESTRINGList musicSets;
                PGESTRINGList bgSets;
                PGESTRINGList ssSets;

                PGESTRINGList movingLayers;
                PGESTRINGList newSectionSettingsSets;
                PGESTRINGList spawnNPCs;
                PGESTRINGList spawnEffectss;
                PGESTRINGList variablesToUpdate;
                PGELIST<bool > controls;

                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()

                    PGEX_StrVal ("ET", event.name) //Event Title
                    PGEX_StrVal ("MG", event.msg) //Event Message
                    PGEX_UIntVal("SD", event.sound_id) //Play Sound ID
                    PGEX_UIntVal("EG", event.end_game) //End game algorithm
                    PGEX_StrArrVal("LH", event.layers_hide) //Hide layers
                    PGEX_StrArrVal("LS", event.layers_show) //Show layers
                    PGEX_StrArrVal("LT", event.layers_toggle) //Toggle layers
                    //Legacy values (without SMBX-38A values support)
                    PGEX_StrArrVal("SM", musicSets)  //Switch music
                    PGEX_StrArrVal("SB", bgSets)     //Switch background
                    PGEX_StrArrVal("SS", ssSets)     //Section Size
                    //-------------------
                    //New values (with SMBX-38A values support)
                    PGEX_StrArrVal("SSS", newSectionSettingsSets) //Section settings in new format
                    //-------------------
                    //---SMBX-38A entries-----
                    PGEX_StrArrVal("MLA",  movingLayers)       //NPC's to spawn
                    PGEX_StrArrVal("SNPC", spawnNPCs)       //NPC's to spawn
                    PGEX_StrArrVal("SEF",  spawnEffectss)    //Effects to spawn
                    PGEX_StrArrVal("UV",   variablesToUpdate) //Variables to update
                    PGEX_StrVal("TSCR", event.trigger_script) //Trigger script
                    PGEX_UIntVal("TAPI", event.trigger_api_id) //Trigger script
                    PGEX_BoolVal("TMR", event.timer_def.enable) //Enable timer
                    PGEX_UIntVal("TMC", event.timer_def.count) //Count of timer units
                    PGEX_UIntVal("TMI", event.timer_def.interval) //Interval of timer tick
                    PGEX_UIntVal("TMD", event.timer_def.count_dir) //Direction of count
                    PGEX_BoolVal("TMV", event.timer_def.show) //Show timer on screen
                    //-------------------
                    PGEX_StrVal("TE", event.trigger) //Trigger event
                    PGEX_UIntVal("TD", event.trigger_timer) //Trigger delay
                    PGEX_BoolVal("DS", event.nosmoke) //Disable smoke
                    PGEX_UIntVal("AU", event.autostart) //Auto start
                    PGEX_StrVal("AUC", event.autostart_condition) //Auto start condition
                    PGEX_BoolArrVal("PC", controls) //Player controls
                    PGEX_StrVal  ("ML", event.movelayer) //Move layer
                    PGEX_FloatVal("MX", event.layer_speed_x) //Layer motion speed X
                    PGEX_FloatVal("MY", event.layer_speed_y) //Layer motion speed Y
                    PGEX_SIntVal("AS", event.scroll_section) //Autoscroll section ID
                    PGEX_FloatVal("AX", event.move_camera_x) //Autoscroll speed X
                    PGEX_FloatVal("AY", event.move_camera_y) //Autoscroll speed Y
                }

                //Parse new-style parameters
                if(!newSectionSettingsSets.empty())
                {
                    for(int q=0; q<(signed)newSectionSettingsSets.size(); q++)
                    {
                        LevelEvent_Sets sectionSet;
                        bool valid=false;
                        PGELIST<PGESTRINGList> sssData=PGEFile::splitDataLine(newSectionSettingsSets[q], &valid);
                        if(!valid)
                        {
                            errorString="Wrong section settings event encoded sub-entry";
                            goto badfile;
                        }

                        for(int ssi=0; ssi < (signed)sssData.size(); ssi++)
                        {
                            PGESTRINGList &param = sssData[ssi];
                            if(param[0]=="ID")
                            {
                                errorString="Invalid sectionID value type";
                                if(PGEFile::IsIntU(param[1]))
                                    sectionSet.id = toInt(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="SL")
                            {
                                errorString="Invalid Section size left value type";
                                if(PGEFile::IsIntS(param[1]))
                                    sectionSet.position_left = toInt(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="ST")
                            {
                                errorString="Invalid Section size top value type";
                                if(PGEFile::IsIntS(param[1]))
                                    sectionSet.position_top = toInt(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="SB")
                            {
                                errorString="Invalid Section size bottom value type";
                                if(PGEFile::IsIntS(param[1]))
                                    sectionSet.position_bottom = toInt(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="SR")
                            {
                                errorString="Invalid Section size right value type";
                                if(PGEFile::IsIntS(param[1]))
                                    sectionSet.position_right = toInt(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="SXX")
                            {
                                errorString="Invalid Section pos x expression value type";
                                if(PGEFile::IsQoutedString(param[1]))
                                    sectionSet.expression_pos_x = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="SYX")
                            {
                                errorString="Invalid Section pos y expression value type";
                                if(PGEFile::IsQoutedString(param[1]))
                                    sectionSet.expression_pos_y = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="SWX")
                            {
                                errorString="Invalid Section pos w expression value type";
                                if(PGEFile::IsQoutedString(param[1]))
                                    sectionSet.expression_pos_w = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="SWH")
                            {
                                errorString="Invalid Section pos h expression value type";
                                if(PGEFile::IsQoutedString(param[1]))
                                    sectionSet.expression_pos_h = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="MI")
                            {
                                errorString="Invalid Section music ID value type";
                                if(PGEFile::IsIntS(param[1]))
                                    sectionSet.music_id= toInt(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="MF")
                            {
                                errorString="Invalid Section music file value type";
                                if(PGEFile::IsQoutedString(param[1]))
                                    sectionSet.music_file = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="BG")
                            {
                                errorString="Invalid Section background ID value type";
                                if(PGEFile::IsIntS(param[1]))
                                    sectionSet.background_id= toInt(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="AS")
                            {
                                errorString="Invalid Section Autoscroll value type";
                                if(PGEFile::IsBool(param[1]))
                                    sectionSet.autoscrol = (bool)toInt(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="AX")
                            {
                                errorString="Invalid Section Autoscroll X value type";
                                if(PGEFile::IsFloat(param[1]))
                                    sectionSet.autoscrol_x = toFloat(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="AY")
                            {
                                errorString="Invalid Section Autoscroll Y value type";
                                if(PGEFile::IsFloat(param[1]))
                                    sectionSet.autoscrol_y = toFloat(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="AXX")
                            {
                                errorString="Invalid Section Autoscroll X expression value type";
                                if(PGEFile::IsQoutedString(param[1]))
                                    sectionSet.expression_autoscrool_x = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="AYX")
                            {
                                errorString="Invalid Section Autoscroll y expression value type";
                                if(PGEFile::IsQoutedString(param[1]))
                                    sectionSet.expression_autoscrool_y = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                        }//for parameters

                        if(
                          ( (sectionSet.id<0) || (sectionSet.id>=(signed)event.sets.size()) )
                           )//Append sections
                        {
                            if( sectionSet.id < 0 )
                            {
                                errorString="Section settings event contains negative section ID value or missed!";
                                goto badfile;//Missmatched section ID!
                            }
                            int last=event.sets.size()-1;
                            while( sectionSet.id >= (signed)event.sets.size() )
                            {
                                LevelEvent_Sets set;
                                set.id=last;
                                event.sets.push_back(set);
                                last++;
                            }
                        }
                        event.sets[sectionSet.id] = sectionSet;
                    }//for section settings entries
                }//If new-styled section settings are gotten

                //Parse odl-style parameters
                else
                {
                    //Apply MusicSets
                    int q=0;
                    for(q=0;q<(signed)event.sets.size() && q<(signed)musicSets.size(); q++)
                    {
                        event.sets[q].id = q;
                        if(!PGEFile::IsIntS(musicSets[q])) goto badfile;
                        event.sets[q].music_id = toInt(musicSets[q]);
                    }

                    //Apply Background sets
                    for(q=0;q<(signed)event.sets.size() && q<(signed)bgSets.size(); q++)
                    {
                        event.sets[q].id = q;
                        if(!PGEFile::IsIntS(bgSets[q])) goto badfile;
                        event.sets[q].background_id = toInt(bgSets[q]);
                    }

                    //Apply section sets
                    for(q=0;q<(signed)event.sets.size() && q<(signed)ssSets.size(); q++)
                    {
                        event.sets[q].id = q;
                        PGESTRINGList sizes;
                        PGE_SPLITSTRING(sizes, ssSets[q], ",");
                        if(sizes.size()!=4) goto badfile;
                        if(!PGEFile::IsIntS(sizes[0])) goto badfile;
                        event.sets[q].position_left = toInt(sizes[0]);
                        if(!PGEFile::IsIntS(sizes[1])) goto badfile;
                        event.sets[q].position_top = toInt(sizes[1]);
                        if(!PGEFile::IsIntS(sizes[2])) goto badfile;
                        event.sets[q].position_bottom = toInt(sizes[2]);
                        if(!PGEFile::IsIntS(sizes[3])) goto badfile;
                        event.sets[q].position_right = toInt(sizes[3]);
                    }
                }

                //Parse Moving layers
                if(!movingLayers.empty())
                {
                    for(int q=0; q<(signed)movingLayers.size(); q++)
                    {
                        LevelEvent_MoveLayer moveLayer;
                        bool valid=false;
                        PGELIST<PGESTRINGList> mlaData=PGEFile::splitDataLine(movingLayers[q], &valid);
                        if(!valid)
                        {
                            errorString="Wrong Move layer event encoded sub-entry";
                            goto badfile;
                        }

                        for(int ssi=0; ssi < (signed)mlaData.size(); ssi++)
                        {
                            PGESTRINGList &param = mlaData[ssi];
                            if(param[0]=="LN")
                            {
                                errorString="Invalid Moving layer name value type";
                                if(PGEFile::IsQoutedString(param[1]))
                                    moveLayer.name = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="SX")
                            {
                                errorString="Invalid movelayer speed X value type";
                                if(PGEFile::IsFloat(param[1]))
                                    moveLayer.speed_x = toFloat(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="SY")
                            {
                                errorString="Invalid movelayer speed Y value type";
                                if(PGEFile::IsFloat(param[1]))
                                    moveLayer.speed_y = toFloat(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="AXX")
                            {
                                errorString="Invalid movelayer speed X expression value type";
                                if(PGEFile::IsQoutedString(param[1]))
                                    moveLayer.expression_x = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="AYX")
                            {
                                errorString="Invalid movelayer speed Y expression value type";
                                if(PGEFile::IsQoutedString(param[1]))
                                    moveLayer.expression_y = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="MW")
                            {
                                errorString="Invalid movelayer way type value type";
                                if(PGEFile::IsIntU(param[1]))
                                    moveLayer.way = toInt(param[1]);
                                else
                                    goto badfile;
                            }
                        }//for parameters

                        event.moving_layers.push_back(moveLayer);
                    }//for moving layers entries
                }//If SMBX38A moving layers are gotten


                //Parse NPCs to spawn
                if(!spawnNPCs.empty())
                {
                    for(int q=0; q<(signed)spawnNPCs.size(); q++)
                    {
                        LevelEvent_SpawnNPC spawnNPC;
                        bool valid=false;
                        PGELIST<PGESTRINGList> mlaData=PGEFile::splitDataLine(spawnNPCs[q], &valid);
                        if(!valid)
                        {
                            errorString="Wrong Spawn NPC event encoded sub-entry";
                            goto badfile;
                        }

                        for(int ssi=0; ssi < (signed)mlaData.size(); ssi++)
                        {
                            PGESTRINGList &param = mlaData[ssi];
                            if(param[0]=="ID")
                            {
                                errorString="Invalid Spawn NPC ID value type";
                                if(PGEFile::IsIntU(param[1]))
                                    spawnNPC.id = toInt(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="SX")
                            {
                                errorString="Invalid Spawn NPC X value type";
                                if(PGEFile::IsFloat(param[1]))
                                    spawnNPC.x = toFloat(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="SY")
                            {
                                errorString="Invalid Spawn NPC Y value type";
                                if(PGEFile::IsFloat(param[1]))
                                    spawnNPC.y = toFloat(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="SXX")
                            {
                                errorString="Invalid  Spawn NPC X expression value type";
                                if(PGEFile::IsQoutedString(param[1]))
                                    spawnNPC.expression_x = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="SYX")
                            {
                                errorString="Invalid Spawn NPC X expression value type";
                                if(PGEFile::IsQoutedString(param[1]))
                                    spawnNPC.expression_y = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="SSX")
                            {
                                errorString="Invalid Spawn NPC X value type";
                                if(PGEFile::IsFloat(param[1]))
                                    spawnNPC.speed_x = toFloat(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="SSY")
                            {
                                errorString="Invalid Spawn NPC Y value type";
                                if(PGEFile::IsFloat(param[1]))
                                    spawnNPC.speed_y = toFloat(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="SSXX")
                            {
                                errorString="Invalid  Spawn NPC Speed X expression value type";
                                if(PGEFile::IsQoutedString(param[1]))
                                    spawnNPC.expression_sx = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="SSYX")
                            {
                                errorString="Invalid Spawn NPC Speed Y expression value type";
                                if(PGEFile::IsQoutedString(param[1]))
                                    spawnNPC.expression_sy = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="SSS")
                            {
                                errorString="Invalid  Spawn NPC Special value type";
                                if(PGEFile::IsIntU(param[1]))
                                    spawnNPC.special = toInt(param[1]);
                                else
                                    goto badfile;
                            }
                        }//for parameters

                        event.spawn_npc.push_back(spawnNPC);
                    }//for Spawn NPC
                }//If SMBX38A NPC Spawning lists are gotten


                //Parse Effects to spawn
                if(!spawnEffectss.empty())
                {
                    for(int q=0; q<(signed)spawnEffectss.size(); q++)
                    {
                        LevelEvent_SpawnEffect spawnEffect;
                        bool valid=false;
                        PGELIST<PGESTRINGList> mlaData=PGEFile::splitDataLine(spawnEffectss[q], &valid);
                        if(!valid)
                        {
                            errorString="Wrong Spawn Effect event encoded sub-entry";
                            goto badfile;
                        }

                        for(int ssi=0; ssi < (signed)mlaData.size(); ssi++)
                        {
                            PGESTRINGList &param = mlaData[ssi];
                            if(param[0]=="ID")
                            {
                                errorString="Invalid Spawn Effect ID value type";
                                if(PGEFile::IsIntU(param[1]))
                                    spawnEffect.id = toInt(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="SX")
                            {
                                errorString="Invalid Spawn Effect X value type";
                                if(PGEFile::IsFloat(param[1]))
                                    spawnEffect.x = toFloat(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="SY")
                            {
                                errorString="Invalid Spawn Effect Y value type";
                                if(PGEFile::IsFloat(param[1]))
                                    spawnEffect.y = toFloat(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="SXX")
                            {
                                errorString="Invalid  Spawn NPC X expression value type";
                                if(PGEFile::IsQoutedString(param[1]))
                                    spawnEffect.expression_x = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="SYX")
                            {
                                errorString="Invalid Spawn NPC X expression value type";
                                if(PGEFile::IsQoutedString(param[1]))
                                    spawnEffect.expression_y = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="SSX")
                            {
                                errorString="Invalid Spawn NPC X value type";
                                if(PGEFile::IsFloat(param[1]))
                                    spawnEffect.speed_x = toFloat(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="SSY")
                            {
                                errorString="Invalid Spawn NPC Y value type";
                                if(PGEFile::IsFloat(param[1]))
                                    spawnEffect.speed_y = toFloat(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="SSXX")
                            {
                                errorString="Invalid  Spawn NPC Speed X expression value type";
                                if(PGEFile::IsQoutedString(param[1]))
                                    spawnEffect.expression_sx = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="SSYX")
                            {
                                errorString="Invalid Spawn NPC Speed Y expression value type";
                                if(PGEFile::IsQoutedString(param[1]))
                                    spawnEffect.expression_sy = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="FP")
                            {
                                errorString="Invalid  Spawn Effect FPS value type";
                                if(PGEFile::IsIntS(param[1]))
                                    spawnEffect.fps = toInt(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="TTL")
                            {
                                errorString="Invalid Spawn Effect time to live value type";
                                if(PGEFile::IsIntS(param[1]))
                                    spawnEffect.max_life_time = toInt(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="GT")
                            {
                                errorString="Invalid Spawn Effect Gravity value type";
                                if(PGEFile::IsBool(param[1]))
                                    spawnEffect.gravity = (bool)toInt(param[1]);
                                else
                                    goto badfile;
                            }
                        }//for parameters

                        event.spawn_effects.push_back(spawnEffect);
                    }//for Spawn Effect
                }//If SMBX38A Effect Spawning lists are gotten


                //Parse Variables to update
                if(!variablesToUpdate.empty())
                {
                    for(int q=0; q<(signed)variablesToUpdate.size(); q++)
                    {
                        LevelEvent_UpdateVariable variableToUpdate;
                        bool valid=false;
                        PGELIST<PGESTRINGList> mlaData=PGEFile::splitDataLine(variablesToUpdate[q], &valid);
                        if(!valid)
                        {
                            errorString="Wrong Variable to update event encoded sub-entry";
                            goto badfile;
                        }

                        for(int ssi=0; ssi < (signed)mlaData.size(); ssi++)
                        {
                            PGESTRINGList &param = mlaData[ssi];
                            if(param[0]=="N")
                            {
                                errorString="Invalid Variable to update name value type";
                                if(PGEFile::IsQoutedString(param[1]))
                                    variableToUpdate.name = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                            else if(param[0]=="V")
                            {
                                errorString="Invalid Variable to update new value type";
                                if(PGEFile::IsQoutedString(param[1]))
                                    variableToUpdate.newval = PGEFile::X2STRING(param[1]);
                                else
                                    goto badfile;
                            }
                        }//for parameters

                        event.update_variable.push_back(variableToUpdate);
                    }//for Variable update events
                }//If SMBX38A variable update lists are gotten


                //Convert boolean array into control flags
                    // SMBX64-only
                if(controls.size()>=1)  event.ctrl_up = controls[0];
                if(controls.size()>=2)  event.ctrl_down = controls[1];
                if(controls.size()>=3)  event.ctrl_left = controls[2];
                if(controls.size()>=4)  event.ctrl_right = controls[3];
                if(controls.size()>=5)  event.ctrl_run = controls[4];
                if(controls.size()>=6)  event.ctrl_jump = controls[5];
                if(controls.size()>=7)  event.ctrl_drop = controls[6];
                if(controls.size()>=8)  event.ctrl_start = controls[7];
                if(controls.size()>=9)  event.ctrl_altrun = controls[8];
                if(controls.size()>=10) event.ctrl_altjump = controls[9];
                    // SMBX64-only end
                    // SMBX-38A begin
                if(controls.size()>=11) event.ctrls_enable = controls[10];
                if(controls.size()>=12) event.ctrl_lock_keyboard = controls[11];
                    // SMBX-38A end

                //add captured value into array
                bool found=false;
                int q=0;
                for(q=0; q<(signed)FileData.events.size();q++)
                {
                    if(FileData.events[q].name==event.name){found=true; break;}
                }
                if(found)
                {
                    event.array_id = FileData.events[q].array_id;
                    FileData.events[q] = event;
                }
                else
                {
                    event.array_id = FileData.events_array_id++;
                    FileData.events.push_back(event);
                }
            }
        }//EVENTS_CLASSIC

        ///////////////////VARIABLES//////////////////////
        PGEX_Section("VARIABLES")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                variable = CreateLvlVariable("unknown");
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_StrVal("N", variable.name) //Variable name
                    PGEX_StrVal("V", variable.value) //Variable value
                }
                FileData.variables.push_back(variable);
            }
        }//VARIABLES

        ///////////////////SCRIPTS//////////////////////
        PGEX_Section("SCRIPTS")
        {
            PGEX_SectionBegin(PGEFile::PGEX_Struct);
            PGEX_Items()
            {
                PGEX_ItemBegin(PGEFile::PGEX_Struct);
                script = CreateLvlScript("unknown", LevelScript::LANG_LUA);
                PGEX_Values() //Look markers and values
                {
                    PGEX_ValueBegin()
                    PGEX_StrVal ("N", script.name) //Variable name
                    PGEX_SIntVal ("L", script.language) //Variable name
                    PGEX_StrVal("S", script.script) //Script text
                }

                switch(script.language)
                {
                    case LevelScript::LANG_LUA:
                    case LevelScript::LANG_TEASCRIPT:
                    case LevelScript::LANG_AUTOCODE:
                        break;
                    default:
                        script.language=LevelScript::LANG_LUA;//LUA by default if any other language code!
                }

                FileData.variables.push_back(variable);
            }
        }//SCRIPTS

    }
    ///////////////////////////////////////EndFile///////////////////////////////////////

    errorString.clear(); //If no errors, clear string;
    FileData.ReadFileValid=true;
    return true;

    badfile:    //If file format is not correct

    FileData.ERROR_info=errorString;
    FileData.ERROR_linenum=in.getCurrentLineNumber();
    FileData.ERROR_linedata=line;

    FileData.ReadFileValid=false;
    return false;
}



//*********************************************************
//****************WRITE FILE FORMAT************************
//*********************************************************

bool FileFormats::WriteExtendedLvlFileF(PGESTRING filePath, LevelData &FileData)
{
    errorString.clear();
    PGE_FileFormats_misc::TextFileOutput file;
    if(!file.open(filePath, false, false, PGE_FileFormats_misc::TextOutput::truncate))
    {
        errorString="Failed to open file for write";
        return false;
    }
    return WriteExtendedLvlFile(file, FileData);
}

bool FileFormats::WriteExtendedLvlFileRaw(LevelData &FileData, PGESTRING &rawdata)
{
    errorString.clear();
    PGE_FileFormats_misc::RawTextOutput file;
    if(!file.open(&rawdata, PGE_FileFormats_misc::TextOutput::truncate))
    {
        errorString="Failed to open raw string for write";
        return false;
    }
    return WriteExtendedLvlFile(file, FileData);
}

bool FileFormats::WriteExtendedLvlFile(PGE_FileFormats_misc::TextOutput &out, LevelData &FileData)
{
    long i;

    FileData.RecentFormat = LevelData::PGEX;

    //Count placed stars on this level
    FileData.stars=0;
    for(i=0;i<(signed)FileData.npc.size();i++)
    {
        if(FileData.npc[i].is_star)
            FileData.stars++;
    }

    //HEAD section
    if( (!IsEmpty(FileData.LevelName))||
        (FileData.stars > 0) ||
        (!IsEmpty(FileData.open_level_on_fail))||
        (FileData.open_level_on_fail_warpID>0))
    {
        out << "HEAD\n";
        if(!IsEmpty(FileData.LevelName))
            out << PGEFile::value("TL", PGEFile::qStrS(FileData.LevelName)); // Level title
        out << PGEFile::value("SZ", PGEFile::IntS(FileData.stars));      // Stars number
        if(!IsEmpty(FileData.open_level_on_fail))
            out << PGEFile::value("DL", PGEFile::qStrS(FileData.open_level_on_fail)); // Open level on fail
        if(FileData.open_level_on_fail_warpID>0)
            out << PGEFile::value("DE", PGEFile::IntS(FileData.open_level_on_fail_warpID));    // Open WarpID of level on fail
        out << "\n";
        out << "HEAD_END\n";
    }

    //////////////////////////////////////MetaData////////////////////////////////////////////////
    //Bookmarks
    if(!FileData.metaData.bookmarks.empty())
    {
        out << "META_BOOKMARKS\n";
        for(i=0;i<(signed)FileData.metaData.bookmarks.size(); i++)
        {
            //Bookmark name
            out << PGEFile::value("BM", PGEFile::qStrS(FileData.metaData.bookmarks[i].bookmarkName));
            out << PGEFile::value("X", PGEFile::IntS(FileData.metaData.bookmarks[i].x));
            out << PGEFile::value("Y", PGEFile::IntS(FileData.metaData.bookmarks[i].y));
            out << "\n";
        }
        out << "META_BOOKMARKS_END\n";
    }

    #ifdef PGE_EDITOR
    //Some System information
    if(FileData.metaData.crash.used)
    {
        out << "META_SYS_CRASH\n";
            out << PGEFile::value("UT", PGEFile::BoolS(FileData.metaData.crash.untitled));
            out << PGEFile::value("MD", PGEFile::BoolS(FileData.metaData.crash.modifyed));
            out << PGEFile::value("N", PGEFile::qStrS(FileData.metaData.crash.filename));
            out << PGEFile::value("P", PGEFile::qStrS(FileData.metaData.crash.path));
            out << PGEFile::value("FP", PGEFile::qStrS(FileData.metaData.crash.fullPath));
            out << "\n";
        out << "META_SYS_CRASH_END\n";
    }

    if(FileData.metaData.script)
    {
        if(!FileData.metaData.script->events().isEmpty())
        {
            out << "META_SCRIPT_EVENTS\n";
            foreach(EventCommand* x, FileData.metaData.script->events())
            {
                out << "EVENT\n";
                out << PGEFile::value("TL", PGEFile::qStrS( x->marker() ) );
                out << PGEFile::value("ET", PGEFile::IntS( (int)x->eventType() ) );
                out << "\n";

                if(x->basicCommands().size()>0)
                {
                    out << "BASIC_COMMANDS\n";
                    foreach(BasicCommand *y, x->basicCommands())
                    {
                        out << PGEFile::value("N", PGEFile::qStrS( y->marker() ) );
                        if(PGESTRING(y->metaObject()->className())=="MemoryCommand")
                        {
                            MemoryCommand *z = dynamic_cast<MemoryCommand*>(y);
                            out << PGEFile::value("CT", PGEFile::qStrS( "MEMORY" ) );
                            out << PGEFile::value("HX", PGEFile::IntS( z->hexValue() ) );
                            out << PGEFile::value("FT", PGEFile::IntS( (int)z->fieldType() ) );
                            out << PGEFile::value("V", PGEFile::FloatS( z->getValue() ) );
                        }
                        out << "\n";
                    }
                    out << "BASIC_COMMANDS_END\n";
                }
                out << "EVENT_END\n";
            }
            out << "META_SCRIPT_EVENTS_END\n";
        }
    }
    #endif
    //////////////////////////////////////MetaData///END//////////////////////////////////////////


    //SECTION section

    //Count available level sections
    int totalSections=0;
    for(i=0; i< (signed)FileData.sections.size(); i++)
    {
        if(
                (FileData.sections[i].size_bottom==0) &&
                (FileData.sections[i].size_left==0) &&
                (FileData.sections[i].size_right==0) &&
                (FileData.sections[i].size_top==0)
           )
            continue; //Skip unitialized sections
        totalSections++;
    }

    //Don't store section data entry if no data to add
    if(totalSections>0)
    {
        out << "SECTION\n";
        for(i=0; i< (signed)FileData.sections.size(); i++)
        {
            if(
                    (FileData.sections[i].size_bottom==0) &&
                    (FileData.sections[i].size_left==0) &&
                    (FileData.sections[i].size_right==0) &&
                    (FileData.sections[i].size_top==0)
               )
                continue; //Skip unitialized sections
            out << PGEFile::value("SC", PGEFile::IntS(FileData.sections[i].id));  // Section ID
            out << PGEFile::value("L", PGEFile::IntS(FileData.sections[i].size_left));  // Left size
            out << PGEFile::value("R", PGEFile::IntS(FileData.sections[i].size_right));  // Right size
            out << PGEFile::value("T", PGEFile::IntS(FileData.sections[i].size_top));  // Top size
            out << PGEFile::value("B", PGEFile::IntS(FileData.sections[i].size_bottom));  // Bottom size

            out << PGEFile::value("MZ", PGEFile::IntS(FileData.sections[i].music_id));  // Music ID
            out << PGEFile::value("MF", PGEFile::qStrS(FileData.sections[i].music_file));  // Music file

            out << PGEFile::value("BG", PGEFile::IntS(FileData.sections[i].background));  // Background ID
            //out << PGEFile::value("BG", PGEFile::qStrS(FileData.sections[i].background_file));  // Background file

            if(FileData.sections[i].wrap_h)
                out << PGEFile::value("CS", PGEFile::BoolS(FileData.sections[i].wrap_h));  // Connect sides horizontally
            if(FileData.sections[i].wrap_v)
                out << PGEFile::value("CSV", PGEFile::BoolS(FileData.sections[i].wrap_v));  // Connect sides vertically
            if(FileData.sections[i].OffScreenEn)
                out << PGEFile::value("OE", PGEFile::BoolS(FileData.sections[i].OffScreenEn));  // Offscreen exit
            if(FileData.sections[i].lock_left_scroll)
                out << PGEFile::value("SR", PGEFile::BoolS(FileData.sections[i].lock_left_scroll));  // Right-way scroll only (No Turn-back)
            if(FileData.sections[i].lock_right_scroll)
                out << PGEFile::value("SL", PGEFile::BoolS(FileData.sections[i].lock_right_scroll));  // Left-way scroll only (No Turn-back)
            if(FileData.sections[i].lock_up_scroll)
                out << PGEFile::value("SD", PGEFile::BoolS(FileData.sections[i].lock_up_scroll));  // Down-way scroll only (No Turn-back)
            if(FileData.sections[i].lock_down_scroll)
                out << PGEFile::value("SU", PGEFile::BoolS(FileData.sections[i].lock_down_scroll));  // Up-way scroll only (No Turn-back)
            if(FileData.sections[i].underwater)
                out << PGEFile::value("UW", PGEFile::BoolS(FileData.sections[i].underwater));  // Underwater bit
            //out << PGEFile::value("SL", PGEFile::BoolS(FileData.sections[i].noforward));  // Left-way scroll only (No Turn-forward)
            out << "\n";
        }
        out << "SECTION_END\n";
    }

    //STARTPOINT section
    int totalPlayerPoints=0;
    for(i=0; i< (signed)FileData.players.size(); i++)
    {
        if((FileData.players[i].w==0)&&
           (FileData.players[i].h==0))
            continue; //Skip empty points
        totalPlayerPoints++;
    }

    //Don't store section data entry if no data to add
    if(totalPlayerPoints>0)
    {
        out << "STARTPOINT\n";
        for(i=0; i< (signed)FileData.players.size(); i++)
        {
            if((FileData.players[i].w==0)&&
               (FileData.players[i].h==0))
                continue; //Skip empty points

            out << PGEFile::value("ID", PGEFile::IntS(FileData.players[i].id));  // Player ID
            out << PGEFile::value("X", PGEFile::IntS(FileData.players[i].x));  // Player X
            out << PGEFile::value("Y", PGEFile::IntS(FileData.players[i].y));  // Player Y
            out << PGEFile::value("D", PGEFile::IntS(FileData.players[i].direction));  // Direction -1 left, 1 right

            out << "\n";
        }
        out << "STARTPOINT_END\n";
    }

    //BLOCK section
    if(!FileData.blocks.empty())
    {
        out << "BLOCK\n";

        LevelBlock defBlock = CreateLvlBlock();

        for(i=0;i<(signed)FileData.blocks.size();i++)
        {
            //Type ID
            out << PGEFile::value("ID", PGEFile::IntS(FileData.blocks[i].id));  // Block ID

            //Position
            out << PGEFile::value("X", PGEFile::IntS(FileData.blocks[i].x));  // Block X
            out << PGEFile::value("Y", PGEFile::IntS(FileData.blocks[i].y));  // Block Y

            //Size
            out << PGEFile::value("W", PGEFile::IntS(FileData.blocks[i].w));  // Block Width (sizable only)
            out << PGEFile::value("H", PGEFile::IntS(FileData.blocks[i].h));  // Block Height (sizable only)

            //Included NPC
            if(FileData.blocks[i].npc_id!=0) //Write only if not zero
                out << PGEFile::value("CN", PGEFile::IntS(FileData.blocks[i].npc_id));  // Included NPC

            //Boolean flags
            if(FileData.blocks[i].invisible)
                out << PGEFile::value("IV", PGEFile::BoolS(FileData.blocks[i].invisible));  // Invisible
            if(FileData.blocks[i].slippery)
                out << PGEFile::value("SL", PGEFile::BoolS(FileData.blocks[i].slippery));  // Slippery flag

            //Layer
            if(FileData.blocks[i].layer!=defBlock.layer) //Write only if not default
                out << PGEFile::value("LR", PGEFile::qStrS(FileData.blocks[i].layer));  // Layer

            //Event Slots
            if(!IsEmpty(FileData.blocks[i].event_destroy))
                out << PGEFile::value("ED", PGEFile::qStrS(FileData.blocks[i].event_destroy));
            if(!IsEmpty(FileData.blocks[i].event_hit))
                out << PGEFile::value("EH", PGEFile::qStrS(FileData.blocks[i].event_hit));
            if(!IsEmpty(FileData.blocks[i].event_emptylayer))
                out << PGEFile::value("EE", PGEFile::qStrS(FileData.blocks[i].event_emptylayer));

            out << "\n";
        }

        out << "BLOCK_END\n";
    }

    //BGO section
    if(!FileData.bgo.empty())
    {
        out << "BGO\n";

        LevelBGO defBGO = CreateLvlBgo();

        for(i=0;i<(signed)FileData.bgo.size();i++)
        {
            out << PGEFile::value("ID", PGEFile::IntS(FileData.bgo[i].id));  // BGO ID

            //Position
            out << PGEFile::value("X", PGEFile::IntS(FileData.bgo[i].x));  // BGO X
            out << PGEFile::value("Y", PGEFile::IntS(FileData.bgo[i].y));  // BGO Y

            if(FileData.bgo[i].z_offset!=defBGO.z_offset)
                out << PGEFile::value("ZO", PGEFile::FloatS(FileData.bgo[i].z_offset));  // BGO Z-Offset
            if(FileData.bgo[i].z_mode!=defBGO.z_mode)
                out << PGEFile::value("ZP", PGEFile::FloatS(FileData.bgo[i].z_mode));  // BGO Z-Mode

            if(FileData.bgo[i].smbx64_sp != -1)
                out << PGEFile::value("SP", PGEFile::FloatS(FileData.bgo[i].smbx64_sp));  // BGO SMBX64 Sort Priority

            if(FileData.bgo[i].layer!=defBGO.layer) //Write only if not default
                out << PGEFile::value("LR", PGEFile::qStrS(FileData.bgo[i].layer));  // Layer

            out << "\n";
        }

        out << "BGO_END\n";
    }

    //NPC section
    if(!FileData.npc.empty())
    {
        out << "NPC\n";

        LevelNPC defNPC = CreateLvlNpc();

        for(i=0;i<(signed)FileData.npc.size();i++)
        {
            out << PGEFile::value("ID", PGEFile::IntS(FileData.npc[i].id));  // NPC ID

            //Position
            out << PGEFile::value("X", PGEFile::IntS(FileData.npc[i].x));  // NPC X
            out << PGEFile::value("Y", PGEFile::IntS(FileData.npc[i].y));  // NPC Y

            out << PGEFile::value("D", PGEFile::IntS(FileData.npc[i].direct));  // NPC Direction

            if(FileData.npc[i].contents != 0)
                out << PGEFile::value("CN", PGEFile::IntS(FileData.npc[i].contents));  // Contents of container
            if(FileData.npc[i].special_data!=defNPC.special_data)
                out << PGEFile::value("S1", PGEFile::IntS(FileData.npc[i].special_data));  // Special value 1
            if(FileData.npc[i].special_data2!=defNPC.special_data2)
                out << PGEFile::value("S2", PGEFile::IntS(FileData.npc[i].special_data2));  // Special value 2

            if(FileData.npc[i].generator)
            {
                out << PGEFile::value("GE", PGEFile::BoolS(FileData.npc[i].generator));  // NPC Generator
                out << PGEFile::value("GT", PGEFile::IntS(FileData.npc[i].generator_type));  // Generator type
                out << PGEFile::value("GD", PGEFile::IntS(FileData.npc[i].generator_direct));  // Generator direct
                out << PGEFile::value("GM", PGEFile::IntS(FileData.npc[i].generator_period));  // Generator time
                if(FileData.npc[i].generator_direct==0)
                {
                    out << PGEFile::value("GA", PGEFile::FloatS(FileData.npc[i].generator_custom_angle));  // Generator custom angle
                    out << PGEFile::value("GB", PGEFile::IntS(FileData.npc[i].generator_branches));  // Generator branches
                    out << PGEFile::value("GR", PGEFile::FloatS(FileData.npc[i].generator_angle_range));  // Generator angle range
                    out << PGEFile::value("GS", PGEFile::FloatS(FileData.npc[i].generator_initial_speed));  // Generator initial speed
                }
            }

            if(!IsEmpty(FileData.npc[i].msg))
                out << PGEFile::value("MG", PGEFile::qStrS(FileData.npc[i].msg));  // Message

            if(FileData.npc[i].friendly)
                out << PGEFile::value("FD", PGEFile::BoolS(FileData.npc[i].friendly));  // Friendly
            if(FileData.npc[i].nomove)
                out << PGEFile::value("NM", PGEFile::BoolS(FileData.npc[i].nomove));  // Idle
            if(FileData.npc[i].is_boss)
                out << PGEFile::value("BS", PGEFile::BoolS(FileData.npc[i].is_boss));  // Set as boss

            if(FileData.npc[i].layer!=defNPC.layer) //Write only if not default
                out << PGEFile::value("LR", PGEFile::qStrS(FileData.npc[i].layer));  // Layer

            if(!IsEmpty(FileData.npc[i].attach_layer))
                out << PGEFile::value("LA", PGEFile::qStrS(FileData.npc[i].attach_layer));  // Attach layer
            if(!IsEmpty(FileData.npc[i].send_id_to_variable))
                out << PGEFile::value("SV", PGEFile::qStrS(FileData.npc[i].send_id_to_variable)); //Send ID to variable

            //Event slots
            if(!IsEmpty(FileData.npc[i].event_activate))
                out << PGEFile::value("EA", PGEFile::qStrS(FileData.npc[i].event_activate));
            if(!IsEmpty(FileData.npc[i].event_die))
                out << PGEFile::value("ED", PGEFile::qStrS(FileData.npc[i].event_die));
            if(!IsEmpty(FileData.npc[i].event_talk))
                out << PGEFile::value("ET", PGEFile::qStrS(FileData.npc[i].event_talk));
            if(!IsEmpty(FileData.npc[i].event_emptylayer))
                out << PGEFile::value("EE", PGEFile::qStrS(FileData.npc[i].event_emptylayer));
            if(!IsEmpty(FileData.npc[i].event_grab))
                out << PGEFile::value("EG", PGEFile::qStrS(FileData.npc[i].event_grab));
            if(!IsEmpty(FileData.npc[i].event_touch))
                out << PGEFile::value("EO", PGEFile::qStrS(FileData.npc[i].event_touch));
            if(!IsEmpty(FileData.npc[i].event_touch))
                out << PGEFile::value("EF", PGEFile::qStrS(FileData.npc[i].event_nextframe));
            out << "\n";
        }

        out << "NPC_END\n";
    }

    //PHYSICS section
    if(!FileData.physez.empty())
    {
        out << "PHYSICS\n";
        LevelPhysEnv defPhys = CreateLvlPhysEnv();

        for(i=0;i<(signed)FileData.physez.size();i++)
        {
            out << PGEFile::value("ET", PGEFile::IntS(FileData.physez[i].env_type));

            //Position
            out << PGEFile::value("X", PGEFile::IntS(FileData.physez[i].x));  // Physic Env X
            out << PGEFile::value("Y", PGEFile::IntS(FileData.physez[i].y));  // Physic Env Y

            //Size
            out << PGEFile::value("W", PGEFile::IntS(FileData.physez[i].w));  // Physic Env Width
            out << PGEFile::value("H", PGEFile::IntS(FileData.physez[i].h));  // Physic Env Height

            if(FileData.physez[i].env_type==LevelPhysEnv::ENV_CUSTOM_LIQUID)
                out << PGEFile::value("FR", PGEFile::FloatS(FileData.physez[i].friction)); //Friction
            if(FileData.physez[i].accel_direct>=0.0)
                out << PGEFile::value("AD", PGEFile::FloatS(FileData.physez[i].accel_direct)); //Acceleration direction
            if(FileData.physez[i].accel!=0.0)
                out << PGEFile::value("AC", PGEFile::FloatS(FileData.physez[i].accel)); //Acceleration
            if(FileData.physez[i].max_velocity != 0.0)
                out << PGEFile::value("MV", PGEFile::FloatS(FileData.physez[i].max_velocity)); //Max-velocity
            if(FileData.physez[i].layer != defPhys.layer) //Write only if not default
                out << PGEFile::value("LR", PGEFile::qStrS(FileData.physez[i].layer));  // Layer
            if(!IsEmpty(FileData.physez[i].touch_event))
                out << PGEFile::value("EO", PGEFile::qStrS(FileData.physez[i].touch_event));  // Touch event slot
            out << "\n";
        }

        out << "PHYSICS_END\n";
    }


    //DOORS section
    if(!FileData.doors.empty())
    {
        out << "DOORS\n";

        LevelDoor defDoor = CreateLvlWarp();
        for(i=0;i<(signed)FileData.doors.size();i++)
        {
            if( ((!FileData.doors[i].lvl_o) && (!FileData.doors[i].lvl_i)) || ((FileData.doors[i].lvl_o) && (!FileData.doors[i].lvl_i)) )
                if(!FileData.doors[i].isSetIn) continue; // Skip broken door
            if( ((!FileData.doors[i].lvl_o) && (!FileData.doors[i].lvl_i)) || ((FileData.doors[i].lvl_i)) )
                if(!FileData.doors[i].isSetOut) continue; // Skip broken door

            //Entrance
            if(FileData.doors[i].isSetIn)
            {
                out << PGEFile::value("IX", PGEFile::IntS(FileData.doors[i].ix));  // Warp Input X
                out << PGEFile::value("IY", PGEFile::IntS(FileData.doors[i].iy));  // Warp Input Y
            }

            if(FileData.doors[i].isSetOut)
            {
                out << PGEFile::value("OX", PGEFile::IntS(FileData.doors[i].ox));  // Warp Output X
                out << PGEFile::value("OY", PGEFile::IntS(FileData.doors[i].oy));  // Warp Output Y
            }

            if(FileData.doors[i].length_i != 32)
                out << PGEFile::value("IL", PGEFile::IntS(FileData.doors[i].length_i));  //Length of entrance
            if(FileData.doors[i].length_o != 32)
                out << PGEFile::value("OL", PGEFile::IntS(FileData.doors[i].length_o));  //Length of exit

            out << PGEFile::value("DT", PGEFile::IntS(FileData.doors[i].type));  // Warp type

            out << PGEFile::value("ID", PGEFile::IntS(FileData.doors[i].idirect));  // Warp Input direction
            out << PGEFile::value("OD", PGEFile::IntS(FileData.doors[i].odirect));  // Warp Outpu direction


            if(FileData.doors[i].world_x != -1 && FileData.doors[i].world_y != -1)
            {
                out << PGEFile::value("WX", PGEFile::IntS(FileData.doors[i].world_x));  // World X
                out << PGEFile::value("WY", PGEFile::IntS(FileData.doors[i].world_y));  // World Y
            }

            if(!IsEmpty(FileData.doors[i].lname))
            {
                out << PGEFile::value("LF", PGEFile::qStrS(FileData.doors[i].lname));  // Warp to level file
                out << PGEFile::value("LI", PGEFile::IntS(FileData.doors[i].warpto));  // Warp arrayID
            }

            if(FileData.doors[i].lvl_i)
                out << PGEFile::value("ET", PGEFile::BoolS(FileData.doors[i].lvl_i));  // Level Entance

            if(FileData.doors[i].lvl_o)
                out << PGEFile::value("EX", PGEFile::BoolS(FileData.doors[i].lvl_o));  // Level Exit

            if(FileData.doors[i].stars>0)
                out << PGEFile::value("SL", PGEFile::IntS(FileData.doors[i].stars));  // Need a stars

            if(!IsEmpty(FileData.doors[i].stars_msg))
                out << PGEFile::value("SM", PGEFile::qStrS(FileData.doors[i].stars_msg));  // Message for start requirement

            if(FileData.doors[i].star_num_hide)
                out << PGEFile::value("SH", PGEFile::BoolS(FileData.doors[i].star_num_hide));  // Don't show number of stars

            if(FileData.doors[i].novehicles)
                out << PGEFile::value("NV", PGEFile::BoolS(FileData.doors[i].novehicles));  // Deny Vehicles

            if(FileData.doors[i].allownpc)
                out << PGEFile::value("AI", PGEFile::BoolS(FileData.doors[i].allownpc));  // Allow Items

            if(FileData.doors[i].locked)
                out << PGEFile::value("LC", PGEFile::BoolS(FileData.doors[i].locked));  // Locked door

            if(FileData.doors[i].need_a_bomb)
                out << PGEFile::value("LB", PGEFile::BoolS(FileData.doors[i].need_a_bomb));  //Need a bomb to open door

            if(FileData.doors[i].hide_entering_scene)
                out << PGEFile::value("HS", PGEFile::BoolS(FileData.doors[i].hide_entering_scene));   //Hide entrance scene

            if(FileData.doors[i].allownpc_interlevel)
                out << PGEFile::value("AL", PGEFile::BoolS(FileData.doors[i].allownpc_interlevel));   //Allow Items inter-level

            if(FileData.doors[i].special_state_required)
                out << PGEFile::value("SR", PGEFile::BoolS(FileData.doors[i].special_state_required));//Special state required

            if(FileData.doors[i].cannon_exit)
            {
                out << PGEFile::value("PT", PGEFile::BoolS(FileData.doors[i].cannon_exit));//cannon exit
                out << PGEFile::value("PS", PGEFile::FloatS(FileData.doors[i].cannon_exit_speed));//cannon exit projectile speed
            }

            if(FileData.doors[i].layer!=defDoor.layer) //Write only if not default
                out << PGEFile::value("LR", PGEFile::qStrS(FileData.doors[i].layer));  // Layer

            if(!IsEmpty(FileData.doors[i].event_enter)) //Write only if not default
                out << PGEFile::value("EE", PGEFile::qStrS(FileData.doors[i].event_enter));  // On-Enter event

            if(FileData.doors[i].two_way)
                out << PGEFile::value("TW", PGEFile::BoolS(FileData.doors[i].two_way)); //Two-way warp

            out << "\n";
        }

        out << "DOORS_END\n";
    }
    //LAYERS section
    if(!FileData.layers.empty())
    {
        out << "LAYERS\n";
        for(i=0;i<(signed)FileData.layers.size();i++)
        {
            out << PGEFile::value("LR", PGEFile::qStrS(FileData.layers[i].name));  // Layer name
            if(FileData.layers[i].hidden)
                out << PGEFile::value("HD", PGEFile::BoolS(FileData.layers[i].hidden));  // Hidden
            if(FileData.layers[i].locked)
                out << PGEFile::value("LC", PGEFile::BoolS(FileData.layers[i].locked));  // Locked
            out << "\n";
        }

        out << "LAYERS_END\n";
    }

    //EVENTS section (action styled)
        //EVENT sub-section of action-styled events


    //EVENTS_CLASSIC (SMBX-Styled events)
    if(!FileData.events.empty())
    {
        out << "EVENTS_CLASSIC\n";
        bool addArray=false;
        for(i=0;i<(signed)FileData.events.size();i++)
        {

            out << PGEFile::value("ET", PGEFile::qStrS(FileData.events[i].name));  // Event name

            if(!IsEmpty(FileData.events[i].msg))
                out << PGEFile::value("MG", PGEFile::qStrS(FileData.events[i].msg));  // Show Message

            if(FileData.events[i].sound_id!=0)
                out << PGEFile::value("SD", PGEFile::IntS(FileData.events[i].sound_id));  // Play Sound ID

            if(FileData.events[i].end_game!=0)
                out << PGEFile::value("EG", PGEFile::IntS(FileData.events[i].end_game));  // End game

            if(!FileData.events[i].layers_hide.empty())
                out << PGEFile::value("LH", PGEFile::strArrayS(FileData.events[i].layers_hide));  // Hide Layers
            if(!FileData.events[i].layers_show.empty())
                out << PGEFile::value("LS", PGEFile::strArrayS(FileData.events[i].layers_show));  // Show Layers
            if(!FileData.events[i].layers_toggle.empty())
                out << PGEFile::value("LT", PGEFile::strArrayS(FileData.events[i].layers_toggle));  // Toggle Layers

            /*
            PGESTRINGList musicSets;
            addArray=false;
            for(int ttt=0; ttt<(signed)FileData.events[i].sets.size(); ttt++)
            {
                musicSets.push_back(fromNum(FileData.events[i].sets[ttt].music_id));
            }
            for(int tt=0; tt<(signed)musicSets.size(); tt++)
            { if(musicSets[tt]!="-1") addArray=true; }

            if(addArray) out << PGEFile::value("SM", PGEFile::strArrayS(musicSets));  // Change section's musics


            addArray=false;
            for(int ttt=0; ttt<(signed)FileData.events[i].sets.size(); ttt++)
            {
                musicSets.push_back(FileData.events[i].sets[ttt].music_file);
            }
            for(int tt=0; tt<(signed)musicSets.size(); tt++)
            { if(!musicSets[tt].PGESTRINGisEmpty()) addArray=true; }

            if(addArray) out << PGEFile::value("SMF", PGEFile::strArrayS(musicSets));  // Change section's music files


            PGESTRINGList backSets;
            addArray=false;
            for(int tt=0; tt<(signed)FileData.events[i].sets.size(); tt++)
            {
                backSets.push_back(fromNum(FileData.events[i].sets[tt].background_id));
            }
            for(int tt=0; tt<(signed)backSets.size(); tt++)
            { if(backSets[tt]!="-1") addArray=true; }

            if(addArray) out << PGEFile::value("SB", PGEFile::strArrayS(backSets));  // Change section's backgrounds


            PGESTRINGList sizeSets;
            addArray=false;
            for(int tt=0; tt<(signed)FileData.events[i].sets.size(); tt++)
            {
                LevelEvent_Sets &x=FileData.events[i].sets[tt];
                QString sizeSect=   fromNum(x.position_left)+","+
                                    fromNum(x.position_top)+","+
                                    fromNum(x.position_bottom)+","+
                                    fromNum(x.position_right);
                if(sizeSect != "-1,0,0,0")
                    addArray=true;
                sizeSets.push_back(sizeSect);
            }
            if(addArray)
                out << PGEFile::value("SS", PGEFile::strArrayS(sizeSets));// Change section's sizes
            */
            PGESTRINGList sectionSettingsSets;
            addArray=false;
            for(int tt=0; tt<(signed)FileData.events[i].sets.size(); tt++)
            {
                bool hasParams = false;
                PGESTRING sectionSettings;
                LevelEvent_Sets &x = FileData.events[i].sets[tt];
                sectionSettings += PGEFile::value("ID", PGEFile::IntS(x.id));
                bool customSize = (x.position_left != LevelEvent_Sets::LESet_Nothing)&&
                                  (x.position_left != LevelEvent_Sets::LESet_ResetDefault);
                if( x.position_left != -1 )
                {
                    sectionSettings += PGEFile::value("SL", PGEFile::IntS(x.position_left));
                    hasParams=true;
                }
                if( customSize && (x.position_top != 0) )
                {
                    sectionSettings += PGEFile::value("ST", PGEFile::IntS(x.position_top));
                    hasParams=true;
                }
                if( customSize && (x.position_bottom != 0) )
                {
                    sectionSettings += PGEFile::value("SB", PGEFile::IntS(x.position_bottom));
                    hasParams=true;
                }
                if( customSize && (x.position_right != 0) )
                {
                    sectionSettings += PGEFile::value("SR", PGEFile::IntS(x.position_right));
                    hasParams=true;
                }
                if( !IsEmpty(x.expression_pos_x) && (x.expression_pos_x != "0") )
                {
                    sectionSettings += PGEFile::value("SXX", PGEFile::qStrS(x.expression_pos_x));
                    hasParams=true;
                }
                if( !IsEmpty(x.expression_pos_y) && (x.expression_pos_y != "0") )
                {
                    sectionSettings += PGEFile::value("SYX", PGEFile::qStrS(x.expression_pos_y));
                    hasParams=true;
                }
                if( !IsEmpty(x.expression_pos_w) && (x.expression_pos_w != "0") )
                {
                    sectionSettings += PGEFile::value("SWX", PGEFile::qStrS(x.expression_pos_w));
                    hasParams=true;
                }
                if( !IsEmpty(x.expression_pos_h) && (x.expression_pos_h != "0") )
                {
                    sectionSettings += PGEFile::value("SHX", PGEFile::qStrS(x.expression_pos_h));
                    hasParams=true;
                }
                if( x.music_id != LevelEvent_Sets::LESet_Nothing )
                {
                    sectionSettings += PGEFile::value("MI", PGEFile::IntS(x.music_id));
                    hasParams=true;
                }
                if( !IsEmpty(x.music_file) )
                {
                    sectionSettings += PGEFile::value("MF", PGEFile::qStrS(x.music_file));
                    hasParams=true;
                }
                if( x.background_id != LevelEvent_Sets::LESet_Nothing )
                {
                    sectionSettings += PGEFile::value("BG", PGEFile::IntS(x.background_id));
                    hasParams=true;
                }
                if( x.autoscrol )
                {
                    sectionSettings += PGEFile::value("AS", PGEFile::BoolS(x.autoscrol));
                    hasParams=true;
                }
                if( x.autoscrol_x != 0.0f )
                {
                    sectionSettings += PGEFile::value("AX", PGEFile::FloatS(x.autoscrol_x));
                    hasParams=true;
                }
                if( x.autoscrol_y != 0.0f )
                {
                    sectionSettings += PGEFile::value("AY", PGEFile::FloatS(x.autoscrol_y));
                    hasParams=true;
                }
                if( !IsEmpty(x.expression_autoscrool_x) && (x.expression_autoscrool_x != "0") )
                {
                    sectionSettings += PGEFile::value("AXX", PGEFile::qStrS(x.expression_autoscrool_x));
                    hasParams=true;
                }
                if( !IsEmpty(x.expression_autoscrool_y) && (x.expression_autoscrool_y != "0") )
                {
                    sectionSettings += PGEFile::value("AYX", PGEFile::qStrS(x.expression_autoscrool_y));
                    hasParams=true;
                }
                if(hasParams)
                    sectionSettingsSets.push_back(sectionSettings);
            }
            if(!sectionSettingsSets.empty())
                out << PGEFile::value("SSS", PGEFile::strArrayS(sectionSettingsSets));//Change section's settings



            if(!IsEmpty(FileData.events[i].trigger))
            {
                out << PGEFile::value("TE", PGEFile::qStrS(FileData.events[i].trigger)); // Trigger Event
                if(FileData.events[i].trigger_timer>0)
                    out << PGEFile::value("TD", PGEFile::IntS(FileData.events[i].trigger_timer)); // Trigger delay
            }
            if(!IsEmpty(FileData.events[i].trigger_script))
                out << PGEFile::value("TSCR", PGEFile::qStrS(FileData.events[i].trigger_script));

            if(FileData.events[i].trigger_api_id != 0)
                out << PGEFile::value("TAPI", PGEFile::IntS(FileData.events[i].trigger_api_id));

            if(FileData.events[i].nosmoke)
                out << PGEFile::value("DS", PGEFile::BoolS(FileData.events[i].nosmoke)); // Disable Smoke
            if(FileData.events[i].autostart > 0 )
                out << PGEFile::value("AU", PGEFile::IntS(FileData.events[i].autostart)); // Autostart event
            if(!IsEmpty(FileData.events[i].autostart_condition))
                out << PGEFile::value("AUC", PGEFile::qStrS(FileData.events[i].autostart_condition)); // Autostart condition event

            PGELIST<bool > controls;
            controls.push_back(FileData.events[i].ctrl_up);
            controls.push_back(FileData.events[i].ctrl_down);
            controls.push_back(FileData.events[i].ctrl_left);
            controls.push_back(FileData.events[i].ctrl_right);
            controls.push_back(FileData.events[i].ctrl_run);
            controls.push_back(FileData.events[i].ctrl_jump);
            controls.push_back(FileData.events[i].ctrl_drop);
            controls.push_back(FileData.events[i].ctrl_start);
            controls.push_back(FileData.events[i].ctrl_altrun);
            controls.push_back(FileData.events[i].ctrl_altjump);
            controls.push_back(FileData.events[i].ctrls_enable);
            controls.push_back(FileData.events[i].ctrl_lock_keyboard);

            addArray=false;
            for(int tt=0; tt<(signed)controls.size(); tt++)
            { if(controls[tt]) addArray=true; }
            if(addArray) out << PGEFile::value("PC", PGEFile::BoolArrayS(controls)); // Create boolean array

            if(!IsEmpty(FileData.events[i].movelayer))
            {
                out << PGEFile::value("ML", PGEFile::qStrS(FileData.events[i].movelayer)); // Move layer
                out << PGEFile::value("MX", PGEFile::FloatS(FileData.events[i].layer_speed_x)); // Move layer X
                out << PGEFile::value("MY", PGEFile::FloatS(FileData.events[i].layer_speed_y)); // Move layer Y
            }

            if(!FileData.events[i].moving_layers.empty())
            {
                PGESTRINGList moveLayers;
                for(int j=0; j<(signed)FileData.events[i].moving_layers.size(); j++)
                {
                    LevelEvent_MoveLayer &mvl=FileData.events[i].moving_layers[j];
                    PGESTRING moveLayer;
                    if(IsEmpty(mvl.name))
                        continue;
                    moveLayer += PGEFile::value("LN", PGEFile::qStrS(mvl.name));

                    if(mvl.speed_x!=0.0)
                        moveLayer += PGEFile::value("SX", PGEFile::FloatS(mvl.speed_x));

                    if(!IsEmpty(mvl.expression_x) && (mvl.expression_x != "0"))
                        moveLayer += PGEFile::value("SXX", PGEFile::qStrS(mvl.expression_x));

                    if(mvl.speed_y!=0.0)
                        moveLayer += PGEFile::value("SY", PGEFile::FloatS(mvl.speed_y));

                    if(!IsEmpty(mvl.expression_y) && (mvl.expression_y != "0"))
                        moveLayer += PGEFile::value("SYX", PGEFile::qStrS(mvl.expression_y));

                    if( mvl.way != 0 )
                        moveLayer += PGEFile::value("MW", PGEFile::IntS(mvl.way));

                    moveLayers.push_back(moveLayer);
                }
                out << PGEFile::value("MLA", PGEFile::strArrayS(moveLayers));
            }

            //NPC's to spawn
            if(!FileData.events[i].spawn_npc.empty())
            {
                PGESTRINGList spawnNPCs;
                for(int j=0; j<(signed)FileData.events[i].spawn_npc.size(); j++)
                {
                    PGESTRING spawnNPC;
                    LevelEvent_SpawnNPC &npc=FileData.events[i].spawn_npc[j];

                    spawnNPC += PGEFile::value("ID", PGEFile::IntS(npc.id));

                    if(npc.x != 0.0)
                        spawnNPC += PGEFile::value("SX", PGEFile::FloatS(npc.x));

                    if(!IsEmpty(npc.expression_x) && (npc.expression_x != "0"))
                        spawnNPC += PGEFile::value("SXX", PGEFile::qStrS(npc.expression_x));

                    if(npc.y != 0.0)
                        spawnNPC += PGEFile::value("SY", PGEFile::FloatS(npc.y));

                    if(!IsEmpty(npc.expression_y) && (npc.expression_y != "0"))
                        spawnNPC += PGEFile::value("SYX", PGEFile::qStrS(npc.expression_y));

                    if(npc.speed_x != 0.0)
                        spawnNPC += PGEFile::value("SSX", PGEFile::FloatS(npc.speed_x));

                    if(!IsEmpty(npc.expression_sx)  && (npc.expression_sx != "0"))
                        spawnNPC += PGEFile::value("SSXX", PGEFile::qStrS(npc.expression_sx));

                    if(npc.speed_y != 0.0)
                        spawnNPC += PGEFile::value("SSY", PGEFile::FloatS(npc.speed_y));

                    if(!IsEmpty(npc.expression_sy) && (npc.expression_sy != "0"))
                        spawnNPC += PGEFile::value("SSYX", PGEFile::qStrS(npc.expression_sy));

                    if(npc.special != 0)
                        spawnNPC += PGEFile::value("SSS", PGEFile::IntS(npc.special));

                    spawnNPCs.push_back(spawnNPC);
                }
                out << PGEFile::value("SNPC", PGEFile::strArrayS(spawnNPCs));
            }

            //Effects to spawn
            if(!FileData.events[i].spawn_effects.empty())
            {
                PGESTRINGList spawnEffects;
                for(int j=0; j<(signed)FileData.events[i].spawn_effects.size(); j++)
                {
                    PGESTRING spawnEffect;
                    LevelEvent_SpawnEffect &effect=FileData.events[i].spawn_effects[j];
                    spawnEffect += PGEFile::value("ID", PGEFile::IntS(effect.id));

                    if(effect.x!=0.0)
                        spawnEffect += PGEFile::value("SX", PGEFile::FloatS(effect.x));

                    if(!IsEmpty(effect.expression_x) && (effect.expression_x != "0"))
                        spawnEffect += PGEFile::value("SXX", PGEFile::qStrS(effect.expression_x));

                    if(effect.y!=0.0)
                        spawnEffect += PGEFile::value("SY", PGEFile::FloatS(effect.y));

                    if(!IsEmpty(effect.expression_y) && (effect.expression_y != "0"))
                        spawnEffect += PGEFile::value("SYX", PGEFile::qStrS(effect.expression_y));

                    if(effect.speed_x!=0.0)
                        spawnEffect += PGEFile::value("SSX", PGEFile::FloatS(effect.speed_x));

                    if(!IsEmpty(effect.expression_sx) && (effect.expression_sx != "0"))
                        spawnEffect += PGEFile::value("SSXX", PGEFile::qStrS(effect.expression_sx));

                    if(effect.speed_y!=0.0)
                        spawnEffect += PGEFile::value("SSY", PGEFile::FloatS(effect.speed_y));

                    if(!IsEmpty(effect.expression_sy) && (effect.expression_sy != "0"))
                        spawnEffect += PGEFile::value("SSYX", PGEFile::qStrS(effect.expression_sy));

                    if(effect.fps!=0)
                        spawnEffect += PGEFile::value("FP", PGEFile::IntS(effect.fps));

                    if(effect.max_life_time!=0)
                        spawnEffect += PGEFile::value("TTL", PGEFile::IntS(effect.max_life_time));

                    if(effect.gravity)
                        spawnEffect += PGEFile::value("GT", PGEFile::BoolS(effect.gravity));

                    spawnEffects.push_back(spawnEffect);
                }
                out << PGEFile::value("SEF", PGEFile::strArrayS(spawnEffects));
            }

            out << PGEFile::value("AS", PGEFile::IntS(FileData.events[i].scroll_section)); // Move camera
            out << PGEFile::value("AX", PGEFile::FloatS(FileData.events[i].move_camera_x)); // Move camera x
            out << PGEFile::value("AY", PGEFile::FloatS(FileData.events[i].move_camera_y)); // Move camera y

            //Variables to update
            if(!FileData.events[i].update_variable.empty())
            {
                PGESTRINGList updateVars;
                for(int j=0; j<(signed)FileData.events[i].update_variable.size(); j++)
                {
                    PGESTRING updateVar;
                    LevelEvent_UpdateVariable &updVar = FileData.events[i].update_variable[j];
                    updateVar += PGEFile::value("N", PGEFile::qStrS(updVar.name));
                    updateVar += PGEFile::value("V", PGEFile::qStrS(updVar.newval));
                    updateVars.push_back(updateVar);
                }
                out << PGEFile::value("UV", PGEFile::strArrayS(updateVars));
            }
            if(FileData.events[i].timer_def.enable)
            {
                out << PGEFile::value("TMR", PGEFile::BoolS(FileData.events[i].timer_def.enable));     //Enable timer
                out << PGEFile::value("TMC", PGEFile::IntS(FileData.events[i].timer_def.count));       //Time left (ticks)
                out << PGEFile::value("TMI", PGEFile::IntS(FileData.events[i].timer_def.interval));    //Tick Interval
                out << PGEFile::value("TMD", PGEFile::IntS(FileData.events[i].timer_def.count_dir));   //Count direction
                out << PGEFile::value("TMV", PGEFile::BoolS(FileData.events[i].timer_def.show));       //Is timer vizible
            }
            out << "\n";
        }
        out << "EVENTS_CLASSIC_END\n";

        //VARIABLES section
        if(!FileData.variables.empty())
        {
            out << "VARIABLES\n";
            for(i=0;i<(signed)FileData.variables.size();i++)
            {
                out << PGEFile::value("N", PGEFile::qStrS(FileData.variables[i].name));  // Variable name
                if(!IsEmpty(FileData.variables[i].value))
                    out << PGEFile::value("V", PGEFile::qStrS(FileData.variables[i].value));  // Value
                out << "\n";
            }
            out << "VARIABLES_END\n";
        }

        //SCRIPTS section
        if(!FileData.scripts.empty())
        {
            out << "SCRIPTS\n";
            for(i=0;i<(signed)FileData.scripts.size();i++)
            {
                out << PGEFile::value("N", PGEFile::qStrS(FileData.scripts[i].name));  // Variable name
                out << PGEFile::value("L", PGEFile::IntS(FileData.scripts[i].language));// Code of language
                if(!IsEmpty(FileData.scripts[i].script))
                    out << PGEFile::value("S", PGEFile::qStrS(FileData.scripts[i].script));  // Script text
                out << "\n";
            }
            out << "SCRIPTS_END\n";
        }
    }

    return true;
}
