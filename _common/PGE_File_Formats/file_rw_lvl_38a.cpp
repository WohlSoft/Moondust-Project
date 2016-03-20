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

#include "CSVReaderPGE.h"
#include "CSVUtils.h"

//for Header readers.
//Use it if you want read file partially
//(you must create QTextStream in(&fstream); !!!)
#define SMBX65_FileBegin() int file_format=0;   /*File format number*/\
                           PGESTRING line;      /*Current Line data*/

//Jump to next line
#ifdef nextLine
#undef nextLine
#endif
#define nextLine() line = in.readLine();

// Settings
constexpr int newest_file_format = 65;

// Common functions
static auto PGEUrlDecodeFunc = [](PGESTRING& data){data = PGE_URLDEC(data);};
static auto PGEBase64DecodeFunc = [](PGESTRING& data){data = PGE_BASE64DEC(data);};
static auto PGEBase64DecodeFuncA = [](PGESTRING& data){data = PGE_BASE64DEC_A(data);};
static auto PGELayerOrDefault = [](PGESTRING& data){data = (data == "" ? "Default" : PGE_URLDEC(data));};
static auto PGEFilpBool = [](bool& value){value = !value;};

template<class T>
constexpr auto MakeClampFunc(T min, T max){
    return [=](auto& value)
    {
        if(value < min)
            value = min;
        if(value > max)
            value = max;
    };
}
template<class T>
constexpr auto MakeMinFunc(T min){
    return [=](auto& value)
    {
        if(value < min)
            value = min;
    };
}
template<class T>
constexpr auto MakeMaxFunc(T max){
    return [=](auto& value)
    {
        if(value > max)
            value = max;
    };
}



LevelData FileFormats::ReadSMBX65by38ALvlFileHeader(PGESTRING filePath)
{
    errorString.clear();
    LevelData FileData;
    CreateLevelHeader(FileData);

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
    try
    {
        CSVPGEReader readerBridge(&inf);
        auto dataReader = MakeCSVReaderForPGESTRING(&readerBridge, '|');
        PGESTRING fileIndentifier = dataReader.ReadField<PGESTRING>(1);
        dataReader.ReadDataLine();

        if(!PGE_StartsWith(fileIndentifier, "SMBXFile"))
            throw std::logic_error("Invalid file format");

        while(!inf.eof())
        {
            PGESTRING identifier = dataReader.ReadField<PGESTRING>(1);
            if(identifier == "A"){
                dataReader.ReadDataLine(CSVDiscard(), // Skip the first field (this is already "identifier")
                                        &FileData.stars,
                                        MakeCSVPostProcessor(&FileData.LevelName, PGEUrlDecodeFunc),
                                        MakeCSVOptional(&FileData.LevelName, PGESTRING(""), nullptr, PGEUrlDecodeFunc),
                                        MakeCSVOptional(&FileData.open_level_on_fail_warpID, 0u));
            }
        }
    }
    catch(const std::exception& err)
    {
        FileData.ReadFileValid=false;
        FileData.ERROR_info = "Invalid file format, detected file SMBX-" + fromNum(newest_file_format) + "format\n"
                "Caused by: \n" + PGESTRING(exception_to_pretty_string(err).c_str());
        FileData.ERROR_linenum = inf.getCurrentLineNumber();
        FileData.ERROR_linedata = "";
    }

    FileData.CurSection = 0;
    FileData.playmusic = 0;

    return FileData;
}

bool FileFormats::ReadSMBX65by38ALvlFileF(PGESTRING  filePath, LevelData &FileData)
{
    PGE_FileFormats_misc::TextFileInput file(filePath, false);
    return ReadSMBX65by38ALvlFile(file, FileData);
}

bool FileFormats::ReadSMBX65by38ALvlFileRaw(PGESTRING &rawdata, PGESTRING  filePath,  LevelData &FileData)
{
    PGE_FileFormats_misc::RawTextInput file(&rawdata, filePath);
    return ReadSMBX65by38ALvlFile(file, FileData);
}

LevelData FileFormats::ReadSMBX65by38ALvlFile(PGESTRING RawData, PGESTRING filePath)
{
    LevelData FileData;
    PGE_FileFormats_misc::RawTextInput file(&RawData, filePath);
    ReadSMBX65by38ALvlFile(file, FileData);
    return FileData;
}

struct LevelEvent_layers
{
    PGESTRING hide;
    PGESTRING show;
    PGESTRING toggle;
};

void SplitCSVStr(PGESTRINGList &dst, PGESTRING &Src)
{
    dst.clear();
    bool quoteOpen=false;
    for(int i=0; i<(signed)Src.size(); )
    {
        PGESTRING Buffer="";
        PGEChar cur=' ';
        do
        {
            cur=Src[i++];
            if(quoteOpen)
                quoteOpen = !quoteOpen;
            else
            {
                if(cur!=',') Buffer.push_back(cur);
            }
        } while( (i<(signed)Src.size()) && (cur!=',') );
        dst.push_back(Buffer);
    }
}

#define SMBX65_SplitSubLine(dst, src) SMBX65_SplitLine(dst, src, '/')
void SMBX65_SplitLine(PGESTRINGList &dst, PGESTRING &Src, char sep='|')
{
    dst.clear();
    for(int i=0; i<(signed)Src.size(); )
    {
        PGESTRING Buffer="";
        PGEChar cur=' ';
        do
        {
            cur=Src[i++];
            if(cur!=sep) Buffer.push_back(cur);
        } while( (i<(signed)Src.size()) && (cur!=sep) );
        dst.push_back(Buffer);
    }
}

/***********  Pre-defined values dependent to NPC Generator Effect field value  **************/

/*  FIELD to Types/Directions conversion table
0	1	2	3	4 <- types ___ directions
                          /
0	0	0	0	0       0
0	5	0	0	17      1
0	6	0	0	18      2
0	7	0	0	19      3
0	8	0	0	20  	4
0	13	0	0	25      9
0	14	0	0	26      10
0	15	0	0	27      11
0	16	0	0	28      12
*/

constexpr int SMBX65_NpcGeneratorTypes[29] =
  //0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28
  { 0,0,0,0,0,1,1,1,1,0,0, 0, 0, 1, 1, 1, 1, 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4};

constexpr int SMBX65_NpcGeneratorDirections[29] =
  //0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28
  { 0,0,0,0,0,1,2,3,4,0,0, 0, 0, 9, 10,11,12, 1, 2, 3, 4, 0, 0, 0, 0,9, 10,11,12};

/**********************************************************************************************/


//LevelData FileFormats::ReadSMBX65by38ALvlFile(PGESTRING RawData, PGESTRING filePath)
bool FileFormats::ReadSMBX65by38ALvlFile(PGE_FileFormats_misc::TextInput &in, LevelData &FileData)
{
    SMBX65_FileBegin();
    PGESTRING filePath = in.getFilePath();
    errorString.clear();
    CreateLevelData(FileData);

    FileData.LevelName = "" ;
    FileData.stars = 0;
    FileData.CurSection = 0;
    FileData.playmusic = 0;

    //Enable strict mode for SMBX LVL file format
    FileData.smbx64strict = false;

    //Begin all ArrayID's here;
    FileData.blocks_array_id = 1;
    FileData.bgo_array_id = 1;
    FileData.npc_array_id = 1;
    FileData.doors_array_id = 1;
    FileData.physenv_array_id = 1;
    FileData.layers_array_id = 1;
    FileData.events_array_id = 1;

    FileData.layers.clear();
    FileData.events.clear();

    LevelSection section;
    PlayerPoint playerdata;
    LevelBlock blockdata;
    LevelBGO bgodata;
    LevelNPC npcdata;
    LevelDoor doordata;
    LevelPhysEnv phyEnv;
    LevelLayer layerdata;
    LevelSMBX64Event eventdata;
    LevelEvent_Sets event_sets;
    LevelVariable vardata;
    LevelScript scriptdata;

    //Add path data
    if(!filePath.PGESTRINGisEmpty())
    {
        PGE_FileFormats_misc::FileInfo in_1(filePath);
        FileData.filename = in_1.basename();
        FileData.path = in_1.dirpath();
    }

    in.seek(0, PGE_FileFormats_misc::TextFileInput::begin);
    try
    {
        CSVPGEReader readerBridge(&in);
        auto dataReader = MakeCSVReaderForPGESTRING(&readerBridge, '|');
        PGESTRING fileIndentifier = dataReader.ReadField<PGESTRING>(1);
        dataReader.ReadDataLine();

        qWarning() << "File format: " << fileIndentifier;
        if(!PGE_StartsWith(fileIndentifier, "SMBXFile"))
            throw std::logic_error("Invalid file format");

        while(!in.eof())
        {
#if 1
            PGESTRING identifier = dataReader.ReadField<PGESTRING>(1);
            if(identifier == "A") {
                // FIXME: Remove copy from line 77
                dataReader.ReadDataLine(CSVDiscard(), // Skip the first field (this is already "identifier")
                                        &FileData.stars,
                                        MakeCSVPostProcessor(&FileData.LevelName, PGEUrlDecodeFunc),
                                        MakeCSVOptional(&FileData.open_level_on_fail, PGESTRING(""), nullptr, PGEUrlDecodeFunc),
                                        MakeCSVOptional(&FileData.open_level_on_fail_warpID, 0u));
            } else if(identifier == "P1") {
                playerdata = CreateLvlPlayerPoint(1);
                dataReader.ReadDataLine(CSVDiscard(), &playerdata.x, &playerdata.y);
                FileData.players.push_back(playerdata);
            } else if(identifier == "P2") {
                // FIXME: Copy from above (can be solved with switch?)
                playerdata = CreateLvlPlayerPoint(2);
                dataReader.ReadDataLine(CSVDiscard(), &playerdata.x, &playerdata.y);
                FileData.players.push_back(playerdata);
            } else if(identifier == "M") {
                section = CreateLvlSection();
                double x = 0.0, y = 0.0, w = 0.0, h = 0.0;
                PGESTRING scroll_lock_x;
                PGESTRING scroll_lock_y;
                dataReader.ReadDataLine(CSVDiscard(),
                                        MakeCSVPostProcessor(&section.id, [](int& sectionID){
                                            sectionID--;
                                            if(sectionID < 0) sectionID = 0;
                                        }),
                                        &x,
                                        &y,
                                        MakeCSVPostProcessor(&w, MakeMinFunc(800.0)),
                                        MakeCSVPostProcessor(&h, MakeMinFunc(600.0)),
                                        &section.underwater,
                                        &section.wrap_h,
                                        &section.OffScreenEn,
                                        &scroll_lock_x,
                                        &scroll_lock_y,
                                        &section.wrap_v,
                                        &section.music_id,
                                        &section.background,
                                        MakeCSVPostProcessor(&section.music_file, PGEUrlDecodeFunc));
                section.lock_left_scroll = (scroll_lock_x == "1");
                section.lock_right_scroll = (scroll_lock_x == "2");
                section.lock_up_scroll = (scroll_lock_y == "1");
                section.lock_down_scroll = (scroll_lock_y == "2");

                section.size_left = (long)round(x);
                section.size_top = (long)round(y);
                section.size_right = (long)round(x+w);
                section.size_bottom = (long)round(y+h);

                //Very important data! I'ts a camera position in the editor!
                section.PositionX = section.size_left-10;
                section.PositionY = section.size_top-10;

                if(section.id < (signed)FileData.sections.size())
                    FileData.sections[section.id] = section;//Replace if already exists
                else
                    FileData.sections.push_back(section); //Add Section in main array
            } else if(identifier == "B") {
                blockdata = CreateLvlBlock();
                dataReader.ReadDataLine(CSVDiscard(),
                                        MakeCSVPostProcessor(&blockdata.layer, PGELayerOrDefault),
                                        &blockdata.id,
                                        &blockdata.x, //FIXME rounding error?
                                        &blockdata.y,
                                        MakeCSVPostProcessor(&blockdata.npc_id, [](long& npcValue){npcValue = (npcValue < 1000 ? -1 * npcValue : npcValue - 1000 );}),
                                        &blockdata.slippery,
                                        &blockdata.invisible,
                                        MakeCSVSubReader(dataReader, ',',
                                                         MakeCSVPostProcessor(&blockdata.event_destroy, PGEUrlDecodeFunc),
                                                         MakeCSVPostProcessor(&blockdata.event_hit, PGEUrlDecodeFunc),
                                                         MakeCSVPostProcessor(&blockdata.event_emptylayer, PGEUrlDecodeFunc)
                                                         ),
                                        &blockdata.w,
                                        &blockdata.h);

                blockdata.array_id = FileData.blocks_array_id++;
                FileData.blocks.push_back(blockdata);
            } else if(identifier == "T") {
                bgodata = CreateLvlBgo();
                dataReader.ReadDataLine(CSVDiscard(),
                                        MakeCSVPostProcessor(&blockdata.layer, PGELayerOrDefault),
                                        &bgodata.id,
                                        &bgodata.x,
                                        &bgodata.y);

                bgodata.array_id = FileData.bgo_array_id++;
                FileData.bgo.push_back(bgodata);
            } else if(identifier == "N") {
                npcdata = CreateLvlNpc();
                long specialData; // We have to handle that later :(
                int genType; // We have to handle that later :(
                dataReader.ReadDataLine(CSVDiscard(),
                                        MakeCSVPostProcessor(&npcdata.layer, PGELayerOrDefault),
                                        &npcdata.id,
                                        &npcdata.x,
                                        &npcdata.y,
                                        MakeCSVSubReader(dataReader, ',',
                                                         MakeCSVPostProcessor(&npcdata.direct, [](int& value){value = value * -1;} ),
                                                         &npcdata.friendly,
                                                         &npcdata.nomove,
                                                         MakeCSVPostProcessor(&npcdata.contents, [](long& value){
                                                            switch(value)
                                                            {
                                                                case 1: value = 91; break;
                                                                case 2: value = 96; break;
                                                                case 3: value = 283; break;
                                                                case 4: value = 284; break;
                                                                case 5: value = 300; break;
                                                            }
                                                         })),
                                        &specialData,
                                        MakeCSVSubReader(dataReader, ',',
                                                         MakeCSVPostProcessor(&npcdata.event_die, PGEUrlDecodeFunc),
                                                         MakeCSVPostProcessor(&npcdata.event_talk, PGEUrlDecodeFunc),
                                                         MakeCSVPostProcessor(&npcdata.event_activate, PGEUrlDecodeFunc),
                                                         MakeCSVPostProcessor(&npcdata.event_emptylayer, PGEUrlDecodeFunc),
                                                         MakeCSVPostProcessor(&npcdata.event_grab, PGEUrlDecodeFunc),
                                                         MakeCSVPostProcessor(&npcdata.event_nextframe, PGEUrlDecodeFunc),
                                                         MakeCSVPostProcessor(&npcdata.event_touch, PGEUrlDecodeFunc)
                                                         ),
                                        MakeCSVSubReader(dataReader, ',',
                                                         MakeCSVPostProcessor(&npcdata.attach_layer, PGEUrlDecodeFunc),
                                                         MakeCSVPostProcessor(&npcdata.send_id_to_variable, PGEUrlDecodeFunc)
                                                         ),
                                        MakeCSVSubReader(dataReader, ',',
                                                         &npcdata.generator,
                                                         MakeCSVOptional(&npcdata.generator_period, 0, nullptr, [](int& value){value = (int)((float)value / 65.f * 100.f);}),
                                                         MakeCSVOptional(&genType, 0),
                                                         MakeCSVOptional(&npcdata.generator_custom_angle, 0.0),
                                                         MakeCSVOptional(&npcdata.generator_branches, 1),
                                                         MakeCSVOptional(&npcdata.generator_angle_range, 360.0),
                                                         MakeCSVOptional(&npcdata.generator_initial_speed, 10.0)
                                                         ),
                                        MakeCSVPostProcessor(&FileData.LevelName, PGEUrlDecodeFunc)
                                        );

                switch(specialData)
                {
                case 15: case 39: case 86: //Bind "Is Boss" flag for supported NPC's
                    npcdata.is_boss = (bool)specialData;
                    npcdata.special_data = 0;
                    default: break;
                }


                switch(genType)
                {
                    case 0:
                        npcdata.generator_type   = LevelNPC::NPC_GENERATOR_APPEAR;
                        npcdata.generator_direct = LevelNPC::NPC_GEN_CENTER;
                    break;
                    default:
                    if(genType < 29)
                    {
                        npcdata.generator_type   = SMBX65_NpcGeneratorTypes[genType];
                        npcdata.generator_direct = SMBX65_NpcGeneratorDirections[genType];
                    } else {
                        npcdata.generator_type   = LevelNPC::NPC_GENERATOR_APPEAR;
                        npcdata.generator_direct = LevelNPC::NPC_GEN_CENTER;
                    }
                }
                //Convert value into SMBX64 and PGEX compatible
                switch(npcdata.generator_type)
                {
                    case 0: npcdata.generator_type = LevelNPC::NPC_GENERATPR_PROJECTILE; break;
                    case 1: npcdata.generator_type = LevelNPC::NPC_GENERATOR_WARP; break;
                    case 4: npcdata.generator_type = LevelNPC::NPC_GENERATOR_APPEAR; break;
                }

                npcdata.array_id = FileData.npc_array_id++;
                FileData.npc.push_back(npcdata);
            } else if(identifier == "Q") {
                phyEnv = CreateLvlPhysEnv();
                dataReader.ReadDataLine(CSVDiscard(),
                                        MakeCSVPostProcessor(&phyEnv.layer, PGELayerOrDefault),
                                        &phyEnv.x,
                                        &phyEnv.y,
                                        &phyEnv.w,
                                        &phyEnv.h,
                                        MakeCSVSubReader(dataReader, ',',
                                                         MakeCSVPostProcessor(&phyEnv.env_type, [](int& value){value--;}),
                                                         &phyEnv.friction,
                                                         &phyEnv.accel_direct,
                                                         &phyEnv.accel,
                                                         &phyEnv.accel),
                                        MakeCSVPostProcessor(&phyEnv.touch_event, PGEUrlDecodeFunc)
                                        );

                phyEnv.array_id = FileData.physenv_array_id++;
                FileData.physez.push_back(phyEnv);
            } else if(identifier == "W") {
                doordata = CreateLvlWarp();
                dataReader.ReadDataLine(CSVDiscard(),
                                        MakeCSVPostProcessor(&doordata.layer, PGELayerOrDefault),
                                        &doordata.ix,
                                        &doordata.iy,
                                        &doordata.ox,
                                        &doordata.oy,
                                        &doordata.type,
                                        &doordata.idirect,
                                        MakeCSVPostProcessor(&doordata.odirect, [](int& value){
                                            switch(value)//Convert into SMBX64/PGE-X Compatible form
                                            {
                                                case 1: value = LevelDoor::EXIT_UP; break;
                                                case 2: value = LevelDoor::EXIT_LEFT; break;
                                                case 3: value = LevelDoor::EXIT_DOWN; break;
                                                case 4: value = LevelDoor::EXIT_RIGHT; break;
                                            }
                                        }),
                                        MakeCSVSubReader(dataReader, ',',
                                                         &doordata.stars,
                                                         MakeCSVPostProcessor(&doordata.stars_msg, PGEUrlDecodeFunc),
                                                         &doordata.star_num_hide),
                                        MakeCSVSubReader(dataReader, ',',
                                                         &doordata.locked,
                                                         &doordata.novehicles,
                                                         &doordata.allownpc,
                                                         &doordata.need_a_bomb,
                                                         &doordata.hide_entering_scene,
                                                         &doordata.allownpc_interlevel,
                                                         &doordata.special_state_required,
                                                         &doordata.length_i),
                                        MakeCSVPostProcessor(&doordata.lname, PGEUrlDecodeFunc),
                                        &doordata.warpto,
                                        &doordata.lvl_i,
                                        &doordata.world_x,
                                        &doordata.world_y,
                                        &doordata.lvl_o,
                                        MakeCSVPostProcessor(&doordata.event_enter, PGEUrlDecodeFunc)
                                        );
                doordata.length_o = doordata.length_i;
                doordata.isSetIn = (doordata.lvl_i ? false : true);
                doordata.isSetOut = (doordata.lvl_o ? false : true) || doordata.lvl_i;

                doordata.array_id = FileData.doors_array_id++;
                FileData.doors.push_back(doordata);
            } else if(identifier == "L") {
                layerdata = CreateLvlLayer();
                dataReader.ReadDataLine(CSVDiscard(),
                                        MakeCSVPostProcessor(&layerdata.name, PGELayerOrDefault),
                                        MakeCSVPostProcessor(&layerdata.hidden, PGEFilpBool)
                                        );

                layerdata.array_id = FileData.layers_array_id++;
                FileData.layers.push_back(layerdata);
            } else if(identifier == "E") {
                eventdata = CreateLvlEvent();

                // Temp Field 5
                PGESTRING moving_layers;

                // Temp Field 7
                PGESTRINGList ev_sections;
                PGESTRINGList ev_bgs;
                PGESTRINGList ev_musics;

                // Temp Field 8
                PGESTRING effects;

                // Temp Field 9
                PGESTRING spawn_npcs;

                // Temp Field 10
                PGESTRING update_var;

                // Temp Field 11
                float trigger_time_raw;
                float timer_def_interval_raw;

                dataReader.ReadDataLine(CSVDiscard(),
                                        MakeCSVPostProcessor(&eventdata.name, PGEUrlDecodeFunc),
                                        MakeCSVPostProcessor(&eventdata.msg, PGEUrlDecodeFunc),
                                        MakeCSVSubReader(dataReader, ',',
                                                         &eventdata.autostart,
                                                         MakeCSVPostProcessor(&eventdata.autostart_condition, PGEUrlDecodeFunc)
                                                         ),
                                        MakeCSVSubReader(dataReader, '/',
                                                         &eventdata.nosmoke,
                                                         MakeCSVBatchReader(dataReader, QChar(','), &eventdata.layers_show, PGEUrlDecodeFunc),
                                                         MakeCSVBatchReader(dataReader, QChar(','), &eventdata.layers_hide, PGEUrlDecodeFunc),
                                                         MakeCSVBatchReader(dataReader, QChar(','), &eventdata.layers_toggle, PGEUrlDecodeFunc)
                                                         ),
                                        &moving_layers,
                                        MakeCSVSubReader(dataReader, ',',
                                                         &eventdata.ctrls_enable,
                                                         &eventdata.ctrl_drop,
                                                         &eventdata.ctrl_altrun,
                                                         &eventdata.ctrl_run,
                                                         &eventdata.ctrl_jump,
                                                         &eventdata.ctrl_altjump,
                                                         &eventdata.ctrl_up,
                                                         &eventdata.ctrl_down,
                                                         &eventdata.ctrl_left,
                                                         &eventdata.ctrl_right,
                                                         &eventdata.ctrl_start,
                                                         &eventdata.ctrl_lock_keyboard
                                                         ),
                                        MakeCSVSubReader(dataReader, '/',
                                                         MakeCSVBatchReader(dataReader, QChar(':'), &ev_sections),
                                                         MakeCSVBatchReader(dataReader, QChar(':'), &ev_bgs),
                                                         MakeCSVBatchReader(dataReader, QChar(':'), &ev_musics)
                                                         ),
                                        &effects,
                                        &spawn_npcs,
                                        &update_var,
                                        MakeCSVSubReader(dataReader, '/',
                                                         MakeCSVSubReader(dataReader, ',',
                                                                          MakeCSVPostProcessor(&eventdata.trigger, PGEUrlDecodeFunc),
                                                                          &trigger_time_raw
                                                                          ),
                                                         MakeCSVSubReader(dataReader, ',',
                                                                          &eventdata.timer_def.enable,
                                                                          &eventdata.timer_def.count,
                                                                          &timer_def_interval_raw,
                                                                          &eventdata.timer_def.count_dir,
                                                                          &eventdata.timer_def.show),
                                                         &eventdata.trigger_api_id,
                                                         MakeCSVPostProcessor(&eventdata.trigger_script, PGEUrlDecodeFunc)
                                                         )

                                        );

                //FIXME: Implement in CSV Reader
                PGESTRINGList EvMvLayers;
                SMBX65_SplitSubLine(EvMvLayers, moving_layers);
                for(int j=0; j<(signed)EvMvLayers.size(); j++)
                {
                    LevelEvent_MoveLayer ml;
                    PGESTRING &dLine=EvMvLayers[j];

                    PGESTRINGList movelayers;
                    SplitCSVStr(movelayers, dLine);
                    for(int k=0; k<(signed)movelayers.size(); k++)
                    {
                        PGESTRING &eLine=movelayers[k];
                        switch(k)
                        {
                            case 0:
                                ml.name=PGE_URLDEC(eLine);
                                eventdata.movelayer=ml.name;
                            break;
                            case 1:
                                ml.expression_x=PGE_URLDEC(eLine);
                                if(SMBX64::sFloat(ml.expression_x))
                                    ml.speed_x=0.0f;
                                else
                                    ml.speed_x=toFloat(ml.expression_x);
                                eventdata.layer_speed_x=ml.speed_x;
                            break;
                            case 2:
                                ml.expression_y=PGE_URLDEC(eLine);
                                if(SMBX64::sFloat(ml.expression_y))
                                    ml.speed_y=0.0f;
                                else
                                    ml.speed_y=toFloat(ml.expression_y);
                                eventdata.layer_speed_y=ml.speed_y;
                            break;
                            case 3:
                                if(SMBX64::uInt(eLine))
                                    goto badfile;
                                ml.way=toInt(eLine);
                            break;
                        }
                        eventdata.moving_layers.push_back(ml);
                    }
                }

                //FIXME: Implement in CSV Reader
                eventdata.sets.clear();
                for(int q=0;q<(signed)FileData.sections.size(); q++)
                {
                    LevelEvent_Sets set;
                    set.id=q;
                    eventdata.sets.push_back(set);
                }

                int evSetsSize = ev_sections.size();
                if(evSetsSize<(signed)ev_bgs.size())
                    evSetsSize=ev_bgs.size();
                if(evSetsSize<(signed)ev_musics.size())
                    evSetsSize=ev_musics.size();

                for(int j=0; j<evSetsSize; j++)
                {
                    //SECTIONS
                    if(j<(signed)ev_sections.size())
                    {
                        PGESTRINGList params;
                        SplitCSVStr(params, ev_sections[j]);
                        //        es=id,stype,x,y,w,h,auto,sx,sy
                        int id = -1;
                        bool customSizes=false;
                        bool autoscroll=false;
                        for(int k=0; k<(signed)params.size();k++)
                        {
                            if(     (k>0)&&
                                    ( (id<0) || (id>=(signed)eventdata.sets.size()) )
                               )//Append sections
                            {
                                if(id<0) goto badfile;//Missmatched section ID!
                                int last=eventdata.sets.size()-1;
                                while(id>=(signed)eventdata.sets.size())
                                {
                                    LevelEvent_Sets set;
                                    set.id=last;
                                    eventdata.sets.push_back(set);
                                    last++;
                                }
                            }
                            PGESTRING &eLine=params[k];
                            switch(k)
                            {
                            //id=section id
                            case 0:
                                if(SMBX64::uInt(eLine))
                                    goto badfile;
                                id=toInt(eLine)-1;
                                break;
                            //stype=[0=don't change][1=default][2=custom]
                            case 1:
                                if(SMBX64::uInt(eLine))
                                    goto badfile;
                                    switch(toInt(eLine))
                                    {
                                        case 0: eventdata.sets[id].position_left=LevelEvent_Sets::LESet_Nothing;
                                        case 1: eventdata.sets[id].position_left=LevelEvent_Sets::LESet_ResetDefault;
                                        case 2: customSizes=true;
                                    }
                                break;
                            //x=left x coordinates for section [id][***urlencode!***][syntax]
                            case 2:
                                if(customSizes)
                                {
                                    eventdata.sets[id].expression_pos_x = PGE_URLDEC(eLine);
                                    if(SMBX64::sFloat(eventdata.sets[id].expression_pos_x))
                                        eventdata.sets[id].position_left=0;
                                    else
                                        eventdata.sets[id].position_left=(long)round(toFloat(eventdata.sets[id].expression_pos_x));
                                }
                                break;
                            //y=top y coordinates for section [id][***urlencode!***][syntax]
                            case 3:
                                if(customSizes)
                                {
                                    eventdata.sets[id].expression_pos_y = PGE_URLDEC(eLine);
                                    if(SMBX64::sFloat(eventdata.sets[id].expression_pos_y))
                                        eventdata.sets[id].position_top=0;
                                    else
                                        eventdata.sets[id].position_top=(long)round(toFloat(eventdata.sets[id].expression_pos_y));
                                } else {
                                    eventdata.sets[id].position_top = 0;
                                }
                                break;
                            //w=width for section [id][***urlencode!***][syntax]
                            case 4:
                                if(customSizes)
                                {
                                    eventdata.sets[id].expression_pos_w = PGE_URLDEC(eLine);
                                    if(SMBX64::sFloat(eventdata.sets[id].expression_pos_w))
                                        eventdata.sets[id].position_right=0;
                                    else
                                        eventdata.sets[id].position_right=(long)round(toFloat(eventdata.sets[id].expression_pos_w))+
                                                                           eventdata.sets[id].position_left;
                                } else {
                                    eventdata.sets[id].position_right = 0;
                                }
                                break;
                            //h=height for section [id][***urlencode!***][syntax]
                            case 5:
                                if(customSizes)
                                {
                                    eventdata.sets[id].expression_pos_h = PGE_URLDEC(eLine);
                                    if(SMBX64::sFloat(eventdata.sets[id].expression_pos_h))
                                        eventdata.sets[id].position_bottom=0;
                                    else
                                        eventdata.sets[id].position_bottom=(long)round(toFloat(eventdata.sets[id].expression_pos_h))+
                                                                            eventdata.sets[id].position_top;
                                } else {
                                    eventdata.sets[id].position_bottom = 0;
                                }
                                break;
                            //auto=enable autoscroll controls[0=false !0=true]
                            case 6:
                                autoscroll = (eLine!="0");
                                eventdata.sets[id].autoscrol = autoscroll;
                                break;
                            //sx=move screen horizontal syntax[***urlencode!***][syntax]
                            case 7:
                                if(autoscroll)
                                {
                                    eventdata.sets[id].expression_autoscrool_x = PGE_URLDEC(eLine);
                                    if(SMBX64::sFloat(eventdata.sets[id].expression_autoscrool_x))
                                        eventdata.sets[id].autoscrol_x=0.f;
                                    else
                                        eventdata.sets[id].autoscrol_x=toFloat(eventdata.sets[id].expression_autoscrool_x);
                                    eventdata.scroll_section=id;
                                    eventdata.move_camera_x = eventdata.sets[id].autoscrol_x;
                                } else {
                                    eventdata.sets[id].autoscrol_x = 0.f;
                                    eventdata.scroll_section = id;
                                    eventdata.move_camera_x = 0.f;
                                }
                                break;
                            //sy=move screen vertical syntax[***urlencode!***][syntax]
                            case 8:
                                if(autoscroll)
                                {
                                    eventdata.sets[id].expression_autoscrool_y = PGE_URLDEC(eLine);
                                    if(SMBX64::sFloat(eventdata.sets[id].expression_autoscrool_y))
                                        eventdata.sets[id].autoscrol_y=0.f;
                                    else
                                        eventdata.sets[id].autoscrol_y=toFloat(eventdata.sets[id].expression_autoscrool_y);
                                    eventdata.scroll_section=id;
                                    eventdata.move_camera_y = eventdata.sets[id].autoscrol_y;
                                } else {
                                    eventdata.sets[id].autoscrol_y = 0.f;
                                    eventdata.scroll_section = id;
                                    eventdata.move_camera_y = 0.f;
                                }
                                break;
                            }
                        }
                    }
                    //BACKGROUNDS
                    if(j<(signed)ev_bgs.size())
                    {
                        PGESTRINGList params;
                        SplitCSVStr(params, ev_bgs[j]);
                        //eb=id,btype,backgroundid
                        int id = -1;
                        bool customBg=false;
                        for(int k=0; k<(signed)params.size();k++)
                        {
                            if(     (k>0)&&
                                    ( (id<0) || (id>=(signed)eventdata.sets.size()) )
                               )//Append sections
                            {
                                if(id<0) goto badfile;//Missmatched section ID!
                                int last=eventdata.sets.size()-1;
                                while(id>=(signed)eventdata.sets.size())
                                {
                                    LevelEvent_Sets set;
                                    set.id=last;
                                    eventdata.sets.push_back(set);
                                    last++;
                                }
                            }

                            PGESTRING &eLine=params[k];
                            switch(k)
                            {
                            //id=section id
                            case 0:
                                if(SMBX64::uInt(eLine))
                                    goto badfile;
                                id=toInt(eLine)-1;
                                break;
                            //btype=[0=don't change][1=default][2=custom]
                            case 1:
                                if(SMBX64::uInt(eLine))
                                    goto badfile;
                                    switch(toInt(eLine))
                                    {
                                        case 0: eventdata.sets[id].background_id=LevelEvent_Sets::LESet_Nothing;
                                        case 1: eventdata.sets[id].background_id=LevelEvent_Sets::LESet_ResetDefault;
                                        case 2: customBg=true;
                                    }
                                break;
                            //backgroundid=[when btype=2]custom background id
                            case 2:
                                if(customBg)
                                {
                                    if(SMBX64::sFloat(eLine))
                                        goto badfile;
                                    eventdata.sets[id].background_id=(long)round(toFloat(eLine));
                                }
                                break;
                            }
                        }
                    }

                    //em=id,mtype,musicid,customfile
                    //  id=section id
                    //  mtype=[0=don't change][1=default][2=custom]
                    //  musicid=[when mtype=2]custom music id
                    //  customfile=[when mtype=3]custom music file name[***urlencode!***]
                    //MUSICS
                    if(j<(signed)ev_musics.size())
                    {
                        PGESTRINGList params;
                        SplitCSVStr(params, ev_musics[j]);
                        //em=id,mtype,musicid,customfile
                        int id = -1;
                        bool customMusics=false;
                        for(int k=0; k<(signed)params.size();k++)
                        {
                            if(     (k>0)&&
                                    ( (id<0) || (id>=(signed)eventdata.sets.size()) )
                               )//Append sections
                            {
                                if(id<0) goto badfile;//Missmatched section ID!
                                int last=eventdata.sets.size()-1;
                                while(id>=(signed)eventdata.sets.size())
                                {
                                    LevelEvent_Sets set;
                                    set.id=last;
                                    eventdata.sets.push_back(set);
                                    last++;
                                }
                            }

                            PGESTRING &eLine=params[k];
                            switch(k)
                            {
                            //id=section id
                            case 0:
                                if(SMBX64::uInt(eLine))
                                    goto badfile;
                                id=toInt(eLine)-1;
                                break;
                            //mtype=[0=don't change][1=default][2=custom]
                            case 1:
                                if(SMBX64::uInt(eLine))
                                    goto badfile;
                                    switch(toInt(eLine))
                                    {
                                        case 0: eventdata.sets[id].music_id=LevelEvent_Sets::LESet_Nothing;
                                        case 1: eventdata.sets[id].music_id=LevelEvent_Sets::LESet_ResetDefault;
                                        case 2:
                                        default: customMusics=true;
                                    }
                                break;
                            //musicid=[when mtype=2]custom music id
                            case 2:
                                if(customMusics)
                                {
                                    if(SMBX64::sFloat(eLine))
                                        goto badfile;
                                    eventdata.sets[id].music_id=(long)round(toFloat(eLine));
                                }
                                break;
                            case 3:
                                if(customMusics)
                                {
                                    eventdata.sets[id].music_file = PGE_URLDEC(eLine);
                                    if(eventdata.sets[id].music_file=="0")
                                        eventdata.sets[id].music_file.clear();
                                }
                                break;
                            }
                        }
                    }
                }

                // FIXME: Implement in CSV Reader
                PGESTRINGList Effects;
                SMBX65_SplitSubLine(Effects, effects);
                for(int j=0; j<(signed)Effects.size(); j++)
                {
                    PGESTRING &dLine=Effects[j];
                    switch(j)
                    {
                        //sound=play sound number
                        case 0:
                            if(SMBX64::uInt(dLine))
                                goto badfile;
                            eventdata.sound_id = toInt(dLine);
                        break;
                        //    endgame=[0=none][1=bowser defeat]
                        case 1:
                            if(SMBX64::uInt(dLine))
                                goto badfile;
                            eventdata.end_game = toInt(dLine);
                        break;
                        default:
                        {
                            LevelEvent_SpawnEffect effect;
                            PGESTRINGList EffectsToSpawn;
                            SplitCSVStr(EffectsToSpawn, dLine);
                            for(int k=0; k<(signed)EffectsToSpawn.size();k++)
                            {
                                //ce(n)=id,x,y,sx,sy,grv,fsp,life
                                PGESTRING &eLine=EffectsToSpawn[k];
                                switch(k)
                                {
                                //        id=effect id
                                case 0:
                                    if(SMBX64::uInt(eLine))
                                        goto badfile;
                                    effect.id = toInt(eLine);
                                    break;
                                //        x=effect position x[***urlencode!***][syntax]
                                case 1:
                                    effect.expression_x = PGE_URLDEC(eLine);
                                    if(SMBX64::sFloat(effect.expression_x))
                                        effect.x=0;
                                    else
                                        effect.x=(long)toFloat(effect.expression_x);
                                    break;
                                //        y=effect position y[***urlencode!***][syntax]
                                case 2:
                                    effect.expression_y = PGE_URLDEC(eLine);
                                    if(SMBX64::sFloat(effect.expression_y))
                                        effect.y=0;
                                    else
                                        effect.y=(long)toFloat(effect.expression_y);
                                    break;
                                //        sx=effect horizontal speed[***urlencode!***][syntax]
                                case 3:
                                    effect.expression_sx = PGE_URLDEC(eLine);
                                    if(SMBX64::sFloat(effect.expression_sx))
                                        effect.speed_x=0.f;
                                    else
                                        effect.speed_x=toFloat(effect.expression_sx);
                                    break;
                                //        sy=effect vertical speed[***urlencode!***][syntax]
                                case 4:
                                    effect.expression_sy = PGE_URLDEC(eLine);
                                    if(SMBX64::sFloat(effect.expression_sy))
                                        effect.speed_y=0.f;
                                    else
                                        effect.speed_y=toFloat(effect.expression_sy);
                                    break;
                                //        grv=to decide whether the effects are affected by gravity[0=false !0=true]
                                case 5: effect.gravity = (eLine!="0"); break;
                                //        fsp=frame speed of effect generated
                                case 6:
                                    if(SMBX64::uInt(eLine))
                                        goto badfile;
                                    effect.fps = toInt(eLine);
                                    break;
                                //        life=effect existed over this time will be destroyed.
                                case 7:
                                    if(SMBX64::uInt(eLine))
                                        goto badfile;
                                    effect.max_life_time = toInt(eLine);
                                    break;
                                }
                            }
                            eventdata.spawn_effects.push_back(effect);
                        }
                        break;
                    }
                }

                // FIXME: Implement in CSV Reader
                PGESTRINGList SpawnNPCs;
                SMBX65_SplitSubLine(SpawnNPCs, spawn_npcs);
                //cn(n)=id,x,y,sx,sy,sp
                for(int j=0;j<(signed)SpawnNPCs.size(); j++)
                {
                    LevelEvent_SpawnNPC spawnnpc;
                    PGESTRING &dLine=SpawnNPCs[j];
                    PGESTRINGList SpawnNPC;
                    SplitCSVStr(SpawnNPC, dLine);
                    for(int k=0;k<(signed)SpawnNPC.size(); k++)
                    {
                        PGESTRING &eLine=SpawnNPC[k];
                        switch(k)
                        {
                        //id=npc id
                        case 0:
                            if(SMBX64::uInt(eLine))
                                goto badfile;
                            spawnnpc.id=toInt(eLine);
                            break;
                        //x=npc position x[***urlencode!***][syntax]
                        case 1:
                            spawnnpc.expression_x = PGE_URLDEC(eLine);
                            if(SMBX64::sFloat(spawnnpc.expression_x))
                                spawnnpc.speed_x=0.f;
                            else
                                spawnnpc.speed_x=toFloat(spawnnpc.expression_x);
                            break;
                        //y=npc position y[***urlencode!***][syntax]
                        case 2:
                            spawnnpc.expression_y = PGE_URLDEC(eLine);
                            if(SMBX64::sFloat(spawnnpc.expression_y))
                                spawnnpc.speed_y=0.f;
                            else
                                spawnnpc.speed_y=toFloat(spawnnpc.expression_y);
                            break;
                        //sx=npc horizontal speed[***urlencode!***][syntax]
                        case 3:
                            spawnnpc.expression_sx = PGE_URLDEC(eLine);
                            if(SMBX64::sFloat(spawnnpc.expression_sx))
                                spawnnpc.speed_x=0.f;
                            else
                                spawnnpc.speed_x=toFloat(spawnnpc.expression_sx);
                            break;
                        //sy=npc vertical speed[***urlencode!***][syntax]
                        case 4:
                            spawnnpc.expression_sy = PGE_URLDEC(eLine);
                            if(SMBX64::sFloat(spawnnpc.expression_sy))
                                spawnnpc.speed_y=0.f;
                            else
                                spawnnpc.speed_y=toFloat(spawnnpc.expression_sy);
                            break;
                        //sp=advanced settings of generated npc
                        case 5:
                            if(SMBX64::sInt(eLine))
                                goto badfile;
                            spawnnpc.special=toInt(eLine);
                            break;
                        }
                    }
                    eventdata.spawn_npc.push_back(spawnnpc);
                }

                // FIXME: Implement in CSV Reader
                LevelEvent_UpdateVariable updVar;
                PGESTRINGList updVars;
                SMBX65_SplitSubLine(updVars, update_var);
                //    vc(n)=name,newvalue
                for(int j=0; j<(signed)updVars.size(); j++)
                {
                    PGESTRING &dLine=updVars[j];
                    switch(j)
                    {
                    //name=variable name[***urlencode!***]
                    case 0:updVar.name=PGE_URLDEC(dLine); break;
                    //newvalue=new value[***urlencode!***][syntax]
                    case 1:updVar.newval=PGE_URLDEC(dLine); break;
                    }
                }
                eventdata.update_variable.push_back(updVar);


                eventdata.trigger_timer = (long)round(SMBX64::t65_to_ms(trigger_time_raw)/100.0);
                eventdata.timer_def.interval = SMBX64::t65_to_ms(timer_def_interval_raw);

                eventdata.array_id = FileData.events_array_id++;
                FileData.events.push_back(eventdata);

            } else if(identifier == "V") {
                vardata = CreateLvlVariable("var");
                dataReader.ReadDataLine(CSVDiscard(),
                                        MakeCSVPostProcessor(&vardata.name, PGEUrlDecodeFunc),
                                        &vardata.value /* save variable value as string
                                                          because in PGE is planned to have
                                                          variables to be universal */
                                        );
                FileData.variables.push_back(vardata);
            } else if(identifier == "S") {
                scriptdata = CreateLvlScript("doScript", LevelScript::LANG_TEASCRIPT);
                dataReader.ReadDataLine(CSVDiscard(),
                                        MakeCSVPostProcessor(&scriptdata.name, PGEUrlDecodeFunc),
                                        MakeCSVPostProcessor(&scriptdata.script, PGEBase64DecodeFunc)
                                        );
                FileData.scripts.push_back(scriptdata);
            } else if(identifier == "Su") {
                scriptdata = CreateLvlScript("doScript", LevelScript::LANG_TEASCRIPT);
                dataReader.ReadDataLine(CSVDiscard(),
                                        MakeCSVPostProcessor(&scriptdata.name, PGEUrlDecodeFunc),
                                        MakeCSVPostProcessor(&scriptdata.script, PGEBase64DecodeFuncA)
                                        );
                FileData.scripts.push_back(scriptdata);
            } else {
                dataReader.ReadDataLine();
            }
#else
#endif
        }//while is not EOF
    } catch(const std::exception& err)
    {
        qWarning() << "Except: " << exception_to_pretty_string(err).c_str();
        FileData.ReadFileValid=false;
        FileData.ERROR_info = "Invalid file format, detected file SMBX-" + fromNum(newest_file_format) + "format\n"
                "Caused by: \n" + PGESTRING(exception_to_pretty_string(err).c_str());
        FileData.ERROR_linenum = in.getCurrentLineNumber();
        FileData.ERROR_linedata = "";
        return false;
    }


    LevelAddInternalEvents(FileData);

    FileData.CurSection=0;
    FileData.playmusic=0;

    FileData.ReadFileValid=true;
    return true;
badfile:
    FileData.ReadFileValid=false;
    FileData.ERROR_info="Invalid file format, detected file SMBX-"+fromNum(file_format)+"format";
    FileData.ERROR_linenum = in.getCurrentLineNumber();
    FileData.ERROR_linedata=line;
    return false;
}


PGESTRING WriteSMBX65by38ALvlFile(LevelData FileData, int file_format)
{
    (void)file_format;//reserved!
    return "";
}
