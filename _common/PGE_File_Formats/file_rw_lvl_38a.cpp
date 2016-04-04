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
#include "smbx64.h"
#include "smbx64_macro.h"

#include "CSVReaderPGE.h"
#include "CSVUtils.h"

using namespace CSVReader;

//for Header readers.
//Use it if you want read file partially
//(you must create QTextStream in(&fstream); !!!)
#define SMBX65_FileBegin() int file_format=0;  /*File format number*/\
                           PGESTRING line;     /*Current Line data*/
#define SMBX65_FileBeginN() /*int file_format=0;*/  /*File format number*/\
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
constexpr std::function<void(T&)> MakeMinFunc(T min){
    return [=](T& value)
    {
        if(value < min)
            value = min;
    };
}

//*********************************************************
//****************READ FILE FORMAT*************************
//*********************************************************

bool FileFormats::ReadSMBX38ALvlFileHeader(PGESTRING filePath, LevelData &FileData)
{
    errorString.clear();
    CreateLevelHeader(FileData);
    FileData.RecentFormat = LevelData::SMBX38A;

    PGE_FileFormats_misc::TextFileInput inf;
    if(!inf.open(filePath, false))
    {
        FileData.ReadFileValid=false;
        return false;
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
            } else {
                dataReader.ReadDataLine();
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
        return false;
    }
    FileData.CurSection = 0;
    FileData.playmusic = 0;

    return true;
}

bool FileFormats::ReadSMBX38ALvlFileF(PGESTRING  filePath, LevelData &FileData)
{
    errorString.clear();
    PGE_FileFormats_misc::TextFileInput file;
    if(!file.open(filePath, false))
    {
        errorString="Failed to open file for read";
        FileData.ERROR_info = errorString;
        FileData.ERROR_linedata = "";
        FileData.ERROR_linenum = -1;
        FileData.ReadFileValid = false;
        return false;
    }
    return ReadSMBX38ALvlFile(file, FileData);
}

bool FileFormats::ReadSMBX38ALvlFileRaw(PGESTRING &rawdata, PGESTRING  filePath,  LevelData &FileData)
{
    errorString.clear();
    PGE_FileFormats_misc::RawTextInput file;
    if(!file.open(&rawdata, filePath))
    {
        errorString="Failed to open raw string for read";
        FileData.ERROR_info = errorString;
        FileData.ERROR_linedata = "";
        FileData.ERROR_linenum = -1;
        FileData.ReadFileValid = false;
        return false;
    }
    return ReadSMBX38ALvlFile(file, FileData);
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

/*!
 * \brief Converts floating point number value from the expression field.
 * \param [__inout] expression Expression field. Clears if valid floating point number has been detected
 * \param [__out] target Target value
 */
inline void SMBX65_Exp2Float(PGESTRING &expression, float& target)
{
    if(!SMBX64::IsFloat( expression ))
        target = 0.0f;
    else
    {
        target = toFloat( expression );
        expression.clear();
    }
}

/*!
 * \brief Converts floating point number value from the expression field.
 * \param [__inout] expression Expression field. Clears if valid floating point number has been detected
 * \param [__out] target Target value
 */
inline void SMBX65_Exp2Double(PGESTRING &expression, double& target)
{
    if(!SMBX64::IsFloat( expression ))
        target = 0.0;
    else
    {
        target = toDouble( expression );
        expression.clear();
    }
}

/*!
 * \brief Converts integer value from the expression field (with rounding possible floating point number value).
 * \param [__inout] expression Expression field. Clears if valid floating point number has been detected
 * \param [__out] target Target value
 */
template<typename T>
inline void SMBX65_Exp2Int(PGESTRING &expression, T& target)
{
    if(!SMBX64::IsFloat( expression ))
        target = 0;
    else
    {
        target = (T)round(toDouble( expression ));
        expression.clear();
    }
}

template<typename T>
inline void SMBX65_Num2Exp(T source, PGESTRING &expression)
{
    if(IsEmpty(expression))
    {
        expression = fromNum(source);
    }
}




/***********  Pre-defined values dependent to NPC Generator Effect field value  **************/

/*

FIELD to Types/Directions conversion table
2	1	0	3	4 <- types (PGE/SMBX64)
0	1	2	3	4 <- types (SMBX-38A)
                          ___ directions
                         /
0	0	0	0	0	    0
1	5	9	13	17	    1
2	6	10	14	18      2
3	7	11	15	19      3
4	8	12	16	20      4
9	13	17	21	25      9
10	14	18	22	26      10
11	15	19	23	27      11
12	16	20	24	28      12

*/

constexpr int SMBX65_NpcGeneratorTypes[29] =
  //0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28
  { 0,2,2,2,2,1,1,1,1,0,0, 0, 0, 3, 3, 3, 3, 4, 4, 4, 4, 3, 3, 3, 3, 4, 4, 4, 4 };

constexpr int SMBX65_NpcGeneratorDirections[29] =
  //0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28
  { 0,1,2,3,4,1,2,3,4,9,10,11,12,1, 2, 3, 4, 1, 2, 3, 4, 9, 10,11,12,9, 10,11,12};

/**********************************************************************************************/


//LevelData FileFormats::ReadSMBX65by38ALvlFile(PGESTRING RawData, PGESTRING filePath)
bool FileFormats::ReadSMBX38ALvlFile(PGE_FileFormats_misc::TextInput &in, LevelData &FileData)
{
    SMBX65_FileBeginN();
    PGESTRING filePath = in.getFilePath();
    errorString.clear();
    CreateLevelData(FileData);

    FileData.RecentFormat = LevelData::SMBX38A;

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
    //LevelEvent_Sets event_sets;
    LevelVariable vardata;
    LevelScript scriptdata;

    //Add path data
    if(!IsEmpty(filePath))
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

        if(!PGE_StartsWith(fileIndentifier, "SMBXFile"))
            throw std::logic_error("Invalid file format");

        while(!in.eof())
        {
            PGESTRING identifier = dataReader.ReadField<PGESTRING>(1);
            if(identifier == "A") {
                // FIXME: Remove copy from line 77
                // A|param1|param2[|param3|param4]
                dataReader.ReadDataLine(CSVDiscard(), // Skip the first field (this is already "identifier")
                                        &FileData.stars,
                                        MakeCSVPostProcessor(&FileData.LevelName, PGEUrlDecodeFunc),
                                        MakeCSVOptional(&FileData.open_level_on_fail, PGESTRING(""), nullptr, PGEUrlDecodeFunc),
                                        MakeCSVOptional(&FileData.open_level_on_fail_warpID, 0u));
            } else if(identifier == "P1") {
                // P1|x1|y1
                playerdata = CreateLvlPlayerPoint(1);
                dataReader.ReadDataLine(CSVDiscard(), &playerdata.x, &playerdata.y);
                FileData.players.push_back(playerdata);
            } else if(identifier == "P2") {
                // P2|x2|y2
                // FIXME: Copy from above (can be solved with switch?)
                playerdata = CreateLvlPlayerPoint(2);
                dataReader.ReadDataLine(CSVDiscard(), &playerdata.x, &playerdata.y);
                FileData.players.push_back(playerdata);
            } else if(identifier == "M") {
                // M|id|x|y|w|h|b1|b2|b3|b4|b5|b6|music|background|musicfile
                section = CreateLvlSection();
                double x = 0.0, y = 0.0, w = 0.0, h = 0.0;
                PGESTRING scroll_lock_x;
                PGESTRING scroll_lock_y;
                dataReader.ReadDataLine(CSVDiscard(),
                                        //id=[1-SectionMAX]
                                        MakeCSVPostProcessor(&section.id, [](int& sectionID) {
                                            sectionID--;
                                            if(sectionID < 0) sectionID = 0;
                                        }),
                                        //x=Left size[-left/+right]
                                        &x,
                                        //y=Top size[-down/+up]
                                        &y,
                                        //w=width of the section[if (w < 800) w = 800]
                                        &w,//MakeCSVPostProcessor(&w, MakeMinFunc(800.0)),
                                        //h=height of the section[if (h < 600) h = 600]
                                        &h,//MakeCSVPostProcessor(&h, MakeMinFunc(600.0)),
                                        //b1=under water?[0=false !0=true]
                                        &section.underwater,
                                        //b2=is x-level wrap[0=false !0=true]
                                        &section.wrap_h,
                                        //b3=enable off screen exit[0=false !0=true]
                                        &section.OffScreenEn,
                                        //b4=no turn back(x)[0=no x-scrolllock 1=scrolllock left 2=scrolllock right]
                                        &scroll_lock_x,
                                        //b5=no turn back(y)[0=no y-scrolllock 1=scrolllock up 2=scrolllock down]
                                        &scroll_lock_y,
                                        //b6=is y-level wrap[0=false !0=true]
                                        &section.wrap_v,
                                        //music=music number[same as smbx1.3]
                                        &section.music_id,
                                        //background=background number[same as the filename in 'background2' folder]
                                        &section.background,
                                        //musicfile=custom music file[***urlencode!***]
                                        MakeCSVPostProcessor(&section.music_file, PGEUrlDecodeFunc));

                section.lock_left_scroll = (scroll_lock_x == "1");
                section.lock_right_scroll = (scroll_lock_x == "2");
                section.lock_up_scroll = (scroll_lock_y == "1");
                section.lock_down_scroll = (scroll_lock_y == "2");

                if((x!=0.0)||(y!=0.0)||(w!=0.0)||(h!=0.0))
                {
                    section.size_left = (long)round(x);
                    section.size_top = (long)round(y);
                    section.size_right = (long)round(x+w);
                    section.size_bottom = (long)round(y+h);
                }
                //Very important data! I'ts a camera position in the editor!
                section.PositionX = section.size_left-10;
                section.PositionY = section.size_top-10;

                if(section.id < (signed)FileData.sections.size())
                    FileData.sections[section.id] = section;//Replace if already exists
                else
                    FileData.sections.push_back(section); //Add Section in main array
            } else if(identifier == "B") {
                // B|layer|id|x|y|contain|b1|b2|e1,e2,e3|w|h
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
                // T|layer|id|x|y
                bgodata = CreateLvlBgo();
                dataReader.ReadDataLine(CSVDiscard(),
                                        MakeCSVPostProcessor(&bgodata.layer, PGELayerOrDefault),
                                        &bgodata.id,
                                        &bgodata.x,
                                        &bgodata.y);

                bgodata.array_id = FileData.bgo_array_id++;
                FileData.bgo.push_back(bgodata);
            } else if(identifier == "N") {
                // N|layer|id|x|y|b1,b2,b3,b4|sp|e1,e2,e3,e4,e5,e6,e7|a1,a2|c1[,c2,c3,c4,c5,c6,c7]|msg|
                npcdata = CreateLvlNpc();
                double specialData;
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
                                                         &npcdata.contents),
                                        &specialData,
                                        MakeCSVSubReader(dataReader, ',',
                                                         MakeCSVPostProcessor(&npcdata.event_die, PGEUrlDecodeFunc),
                                                         MakeCSVPostProcessor(&npcdata.event_talk, PGEUrlDecodeFunc),
                                                         MakeCSVPostProcessor(&npcdata.event_activate, PGEUrlDecodeFunc),
                                                         MakeCSVPostProcessor(&npcdata.event_emptylayer, PGEUrlDecodeFunc),
                                                         MakeCSVPostProcessor(&npcdata.event_grab, PGEUrlDecodeFunc),
                                                         MakeCSVPostProcessor(&npcdata.event_nextframe, PGEUrlDecodeFunc),
                                                         MakeCSVOptional(&npcdata.event_touch, "", nullptr, PGEUrlDecodeFunc)
                                                         ),
                                        MakeCSVSubReader(dataReader, ',',
                                                         MakeCSVPostProcessor(&npcdata.attach_layer, PGEUrlDecodeFunc),
                                                         MakeCSVPostProcessor(&npcdata.send_id_to_variable, PGEUrlDecodeFunc)
                                                         ),
                                        MakeCSVSubReader(dataReader, ',',
                                                         &npcdata.generator,
                                                         MakeCSVOptional(&npcdata.generator_period, 65, nullptr, [](int& value){ value = (int)round( ((double)value*10.0) /65.0);} ),
                                                         MakeCSVOptional(&genType, 0),
                                                         MakeCSVOptional(&npcdata.generator_custom_angle, 0.0),
                                                         MakeCSVOptional(&npcdata.generator_branches, 1),
                                                         MakeCSVOptional(&npcdata.generator_angle_range, 360.0),
                                                         MakeCSVOptional(&npcdata.generator_initial_speed, 10.0)
                                                         ),
                                        MakeCSVPostProcessor(&npcdata.msg, PGEUrlDecodeFunc)
                                        );

                if(npcdata.contents>0)
                {
                    int contID = npcdata.contents;
                    npcdata.contents = npcdata.id;
                    switch(contID)
                    {
                        case 1: npcdata.id=91;break;
                        case 2: npcdata.id=96;break;
                        case 3: npcdata.id=283;break;
                        case 4: npcdata.id=284;break;
                        case 5: npcdata.id=300;break;
                        default: npcdata.contents = 0; //Invalid container type
                    }
                }

                npcdata.special_data = (long)round(specialData);
                switch(npcdata.id)
                {
                case 15: case 39: case 86: //Bind "Is Boss" flag for supported NPC's
                    npcdata.is_boss = (bool)npcdata.special_data;
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
                // Q|layer|x|y|w|h|b1,b2,b3,b4,b5|event
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
                // W|layer|x|y|ex|ey|type|enterd|exitd|sn,msg,hide|locked,noyoshi,canpick,bomb,hidef,anpc,mini,size|lik|liid|noexit|wx|wy|le|we
                doordata = CreateLvlWarp();
                dataReader.ReadDataLine(CSVDiscard(),
                                        //layer=layer name["" == "Default"][***urlencode!***]
                                        MakeCSVPostProcessor(&doordata.layer, PGELayerOrDefault),
                                        //x=entrance position x
                                        &doordata.ix,
                                        //y=entrance postion y
                                        &doordata.iy,
                                        //ex=exit position x
                                        &doordata.ox,
                                        //ey=exit position y
                                        &doordata.oy,
                                        //type=[1=pipe][2=door][0=instant][3=portal/loop]
                                        &doordata.type,
                                        //enterd=entrance direction[1=up 2=left 3=down 4=right]
                                        &doordata.idirect,
                                        //exitd=exit direction[1=up 2=left 3=down 4=right]
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
                                                         //sn=need stars for enter
                                                         &doordata.stars,
                                                         //msg=a message when you have not enough stars
                                                         MakeCSVPostProcessor(&doordata.stars_msg, PGEUrlDecodeFunc),
                                                         //hide=hide the star number in this warp
                                                         &doordata.star_num_hide),
                                        MakeCSVSubReader(dataReader, ',',
                                                         //locked=locked
                                                         &doordata.locked,
                                                         //noyoshi=no yoshi
                                                         &doordata.novehicles,
                                                         //canpick=allow npc
                                                         &doordata.allownpc,
                                                         //bomb=need a bomb
                                                         &doordata.need_a_bomb,
                                                         //hide=hide the entry scene
                                                         &doordata.hide_entering_scene,
                                                         //anpc=allow npc interlevel
                                                         &doordata.allownpc_interlevel,
                                                         //mini=Mini-Only
                                                         MakeCSVOptional(&doordata.special_state_required, false),
                                                         //size=Warp Size(pixel)
                                                         MakeCSVOptional(&doordata.length_i, 32u)),
                                        //lik=warp to level[***urlencode!***]
                                        MakeCSVPostProcessor(&doordata.lname, PGEUrlDecodeFunc),
                                        //liid=normal enterance / to warp[0-WARPMAX]
                                        &doordata.warpto,
                                        //noexit=level entrance
                                        &doordata.lvl_i,
                                        //wx=warp to x on world map
                                        &doordata.world_x,
                                        //wy=warp to y on world map
                                        &doordata.world_y,
                                        //le=level exit
                                        MakeCSVOptional(&doordata.lvl_o, false),
                                        //we=warp event[***urlencode!***]
                                        MakeCSVOptional(&doordata.event_enter, "", nullptr, PGEUrlDecodeFunc)
                                        );
                doordata.length_o = doordata.length_i;
                doordata.isSetIn = (doordata.lvl_i ? false : true);
                doordata.isSetOut = (doordata.lvl_o ? false : true) || doordata.lvl_i;

                doordata.array_id = FileData.doors_array_id++;
                FileData.doors.push_back(doordata);
            } else if(identifier == "L") {
                // L|name|status
                layerdata = CreateLvlLayer();
                dataReader.ReadDataLine(CSVDiscard(),
                                        MakeCSVPostProcessor(&layerdata.name, PGELayerOrDefault),
                                        MakeCSVPostProcessor(&layerdata.hidden, PGEFilpBool)
                                        );

                layerdata.array_id = FileData.layers_array_id++;
                FileData.layers.push_back(layerdata);
            } else if(identifier == "E") {
                // E|name|msg|ea|el|elm|epy|eps|eef|ecn|evc|ene
                eventdata = CreateLvlEvent();

                // Here we can just align the section id with the index of the set
                // It is an unsafe method, however, we should be safe when reading from the file, where the data object is empty.
                eventdata.sets.clear();
                for(int q=0;q<(signed)FileData.sections.size(); q++)
                {
                    LevelEvent_Sets set;
                    set.id=q;
                    eventdata.sets.push_back(set);
                }

                // Temp Field 11
                float trigger_time_raw;
                float timer_def_interval_raw;

                // This variable is used for the spawn npc section.
                // The first two values are static ones, after that they come in packages (see below)
                int spawnNpcReaderCurrentIndex = 0;
                dataReader.ReadDataLine(CSVDiscard(),
                                        MakeCSVPostProcessor(&eventdata.name, PGEUrlDecodeFunc),
                                        MakeCSVPostProcessor(&eventdata.msg, PGEUrlDecodeFunc),
                                        MakeCSVSubReader(dataReader, ',',
                                                         &eventdata.autostart,
                                                         MakeCSVPostProcessor(&eventdata.autostart_condition, PGEUrlDecodeFunc)
                                                         ),
                                        MakeCSVSubReader(dataReader, '/',
                                                         &eventdata.nosmoke,
                                                         MakeCSVBatchReader(dataReader, ',', &eventdata.layers_show, PGEUrlDecodeFunc),
                                                         MakeCSVBatchReader(dataReader, ',', &eventdata.layers_hide, PGEUrlDecodeFunc),
                                                         MakeCSVBatchReader(dataReader, ',', &eventdata.layers_toggle, PGEUrlDecodeFunc)
                                                         ),
                                        MakeCSVIterator(dataReader, '/', [&eventdata](const PGESTRING& nextFieldStr) {
                                            auto fieldReader = MakeDirectReader(nextFieldStr);
                                            auto fullReader  = MakeCSVReaderForPGESTRING(&fieldReader, ',');
                                            LevelEvent_MoveLayer movingLayer;
                                            fullReader.ReadDataLine(MakeCSVPostProcessor(&movingLayer.name, PGEUrlDecodeFunc),
                                                                    MakeCSVPostProcessor(&movingLayer.expression_x, PGEUrlDecodeFunc),
                                                                    MakeCSVPostProcessor(&movingLayer.expression_y, PGEUrlDecodeFunc),
                                                                    &movingLayer.way
                                                                    );
                                            SMBX65_Exp2Float(movingLayer.expression_x, movingLayer.speed_x);
                                            SMBX65_Exp2Float(movingLayer.expression_y, movingLayer.speed_y);
                                            eventdata.moving_layers.push_back(movingLayer);
                                            eventdata.movelayer = movingLayer.name;
                                            eventdata.layer_speed_x = movingLayer.speed_x;
                                            eventdata.layer_speed_y = movingLayer.speed_y;
                                        }),
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
                                                         MakeCSVIterator(dataReader, ':', [&eventdata](const PGESTRING& nextFieldStr){
                                                             auto fieldReader = MakeDirectReader(nextFieldStr);
                                                             auto fullReader = MakeCSVReaderForPGESTRING(&fieldReader, ',');
                                                             int sectionID = fullReader.ReadField<int>(1) - 1;
                                                             LevelEvent_Sets& nextSet = eventdata.sets[sectionID];
                                                             bool customSize = false;
                                                             bool canAutoscroll = false;
                                                             fullReader.ReadDataLine(CSVDiscard(),
                                                                                     MakeCSVPostProcessor(&nextSet.position_left, [&customSize](long& value){
                                                                                         switch(value)
                                                                                         {
                                                                                             case 0: value = LevelEvent_Sets::LESet_Nothing; break;
                                                                                             case 1: value = LevelEvent_Sets::LESet_ResetDefault; break;
                                                                                             case 2: customSize = true; value = 0; break;
                                                                                         }
                                                                                     }),
                                                                                     MakeCSVPostProcessor(&nextSet.expression_pos_x, PGEUrlDecodeFunc),
                                                                                     MakeCSVPostProcessor(&nextSet.expression_pos_y, PGEUrlDecodeFunc),
                                                                                     MakeCSVPostProcessor(&nextSet.expression_pos_w, PGEUrlDecodeFunc),
                                                                                     MakeCSVPostProcessor(&nextSet.expression_pos_h, PGEUrlDecodeFunc),
                                                                                     MakeCSVPostProcessor(&nextSet.autoscrol, [&canAutoscroll](bool& value) {canAutoscroll = value;}),
                                                                                     MakeCSVPostProcessor(&nextSet.expression_autoscrool_x, PGEUrlDecodeFunc),
                                                                                     MakeCSVPostProcessor(&nextSet.expression_autoscrool_y, PGEUrlDecodeFunc)
                                                                                     );
                                                             if( customSize )
                                                             {
                                                                 SMBX65_Exp2Int(nextSet.expression_pos_x, nextSet.position_left);
                                                                 SMBX65_Exp2Int(nextSet.expression_pos_y, nextSet.position_top);
                                                                 SMBX65_Exp2Int(nextSet.expression_pos_w, nextSet.position_right);
                                                                 SMBX65_Exp2Int(nextSet.expression_pos_h, nextSet.position_bottom);
                                                                 if(IsEmpty(nextSet.expression_pos_w))
                                                                     nextSet.position_right += nextSet.position_left;
                                                                 if(IsEmpty(nextSet.expression_pos_h))
                                                                     nextSet.position_bottom += nextSet.position_top;
                                                             }
                                                             if(canAutoscroll)
                                                             {
                                                                 SMBX65_Exp2Float(nextSet.expression_autoscrool_x, nextSet.autoscrol_x);
                                                                 SMBX65_Exp2Float(nextSet.expression_autoscrool_y, nextSet.autoscrol_y);
                                                                 //SMBX64 backwarth compatibility:
                                                                 eventdata.scroll_section = nextSet.id;//Set ID of autoscrollable section :-P
                                                                 eventdata.move_camera_x = nextSet.autoscrol_x;
                                                                 eventdata.move_camera_y = nextSet.autoscrol_y;
                                                             }else{
                                                                 nextSet.autoscrol_x = 0.f;
                                                                 nextSet.autoscrol_y = 0.f;
                                                                 // Doesn't even make sense:
                                                                 // eventdata.move_camera_x = 0.f;
                                                                 // eventdata.move_camera_y = 0.f;
                                                             }
                                                             eventdata.scroll_section = sectionID;
                                                         }),
                                                         MakeCSVIterator(dataReader, ':', [&eventdata](const PGESTRING& nextFieldStr){
                                                             auto fieldReader = MakeDirectReader(nextFieldStr);
                                                             auto fullReader = MakeCSVReaderForPGESTRING(&fieldReader, ',');
                                                             int sectionID = fullReader.ReadField<int>(1) - 1;
                                                             LevelEvent_Sets& nextSet = eventdata.sets[sectionID];
                                                             bool customBG = false;
                                                             long bgID = 0;
                                                             fullReader.ReadDataLine(CSVDiscard(),
                                                                                     MakeCSVPostProcessor(&nextSet.background_id, [&customBG](long& value){
                                                                                         switch(value)
                                                                                         {
                                                                                             case 0: value = LevelEvent_Sets::LESet_Nothing; break;
                                                                                             case 1: value = LevelEvent_Sets::LESet_ResetDefault; break;
                                                                                             case 2: customBG = true; value = 0; break;
                                                                                         }
                                                                                     }),
                                                                                     &bgID
                                                                                     );
                                                             if(customBG)
                                                                 nextSet.background_id = bgID;
                                                         }),
                                                         MakeCSVIterator(dataReader, ':', [&eventdata](const PGESTRING& nextFieldStr){
                                                             auto fieldReader = MakeDirectReader(nextFieldStr);
                                                             auto fullReader = MakeCSVReaderForPGESTRING(&fieldReader, ',');
                                                             int sectionID = fullReader.ReadField<int>(1) - 1;
                                                             LevelEvent_Sets& nextSet = eventdata.sets[sectionID];
                                                             bool customMusic = false;
                                                             long music_id;
                                                             fullReader.ReadDataLine(CSVDiscard(),
                                                                                     MakeCSVPostProcessor(&nextSet.music_id, [&customMusic](long& value){
                                                                                         switch(value)
                                                                                         {
                                                                                             case 0: value = LevelEvent_Sets::LESet_Nothing; break;
                                                                                             case 1: value = LevelEvent_Sets::LESet_ResetDefault; break;
                                                                                             default: case 2: customMusic = true; value = 0; break;
                                                                                         }
                                                                                     }),
                                                                                     &music_id,
                                                                                     MakeCSVOptional(&nextSet.music_file, "", nullptr, PGEUrlDecodeFunc)
                                                                                     );
                                                             if(customMusic)
                                                                 nextSet.music_id = music_id;
                                                         })
                                                         ),
                                        MakeCSVIterator(dataReader, '/', [&eventdata, &spawnNpcReaderCurrentIndex](const PGESTRING& nextFieldStr){
                                            switch (spawnNpcReaderCurrentIndex) {
                                            case 0:
                                                if(!SMBX64::IsUInt(nextFieldStr))
                                                    throw std::invalid_argument("Cannot convert field 1 to int.");
                                                eventdata.sound_id = toInt(nextFieldStr);
                                                spawnNpcReaderCurrentIndex++;
                                                break;
                                            case 1:
                                                if(!SMBX64::IsUInt(nextFieldStr))
                                                    throw std::invalid_argument("Cannot convert field 2 to int.");
                                                eventdata.end_game = toInt(nextFieldStr);
                                                spawnNpcReaderCurrentIndex++;
                                                break;
                                            default:
                                                auto fieldReader = MakeDirectReader(nextFieldStr);
                                                auto fullReader = MakeCSVReaderForPGESTRING(&fieldReader, ',');
                                                LevelEvent_SpawnEffect effect;
                                                fullReader.ReadDataLine(&effect.id,
                                                                        MakeCSVPostProcessor(&effect.expression_x, PGEUrlDecodeFunc),
                                                                        MakeCSVPostProcessor(&effect.expression_y, PGEUrlDecodeFunc),
                                                                        MakeCSVPostProcessor(&effect.expression_sx, PGEUrlDecodeFunc),
                                                                        MakeCSVPostProcessor(&effect.expression_sy, PGEUrlDecodeFunc),
                                                                        &effect.gravity,
                                                                        &effect.fps,
                                                                        &effect.max_life_time
                                                                        );
                                                SMBX65_Exp2Int(effect.expression_x, effect.x);
                                                SMBX65_Exp2Int(effect.expression_y, effect.y);
                                                SMBX65_Exp2Float(effect.expression_sx, effect.speed_x);
                                                SMBX65_Exp2Float(effect.expression_sy, effect.speed_y);
                                                eventdata.spawn_effects.push_back(effect);
                                                break;
                                            }
                                        }),
                                        // &effects,
                                        MakeCSVIterator(dataReader, '/', [&eventdata](const PGESTRING& nextFieldStr){
                                            auto fieldReader = MakeDirectReader(nextFieldStr);
                                            auto fullReader = MakeCSVReaderForPGESTRING(&fieldReader, ',');
                                            LevelEvent_SpawnNPC spawnnpc;
                                            fullReader.ReadDataLine(&spawnnpc.id,
                                                                    MakeCSVPostProcessor(&spawnnpc.expression_x, PGEUrlDecodeFunc),
                                                                    MakeCSVPostProcessor(&spawnnpc.expression_y, PGEUrlDecodeFunc),
                                                                    MakeCSVPostProcessor(&spawnnpc.expression_sx, PGEUrlDecodeFunc),
                                                                    MakeCSVPostProcessor(&spawnnpc.expression_sy, PGEUrlDecodeFunc),
                                                                    &spawnnpc.special
                                                                    );
                                            SMBX65_Exp2Int(spawnnpc.expression_x, spawnnpc.x);
                                            SMBX65_Exp2Int(spawnnpc.expression_y, spawnnpc.y);
                                            SMBX65_Exp2Float(spawnnpc.expression_sx, spawnnpc.speed_x);
                                            SMBX65_Exp2Float(spawnnpc.expression_sy, spawnnpc.speed_y);
                                            eventdata.spawn_npc.push_back(spawnnpc);
                                        }),
                                        // &spawn_npcs,
                                        MakeCSVIterator(dataReader, '/', [&eventdata](const PGESTRING& nextFieldStr){
                                            auto fieldReader = MakeDirectReader(nextFieldStr);
                                            auto fullReader = MakeCSVReaderForPGESTRING(&fieldReader, ',');
                                            LevelEvent_UpdateVariable updVar;
                                            fullReader.ReadDataLine(MakeCSVPostProcessor(&updVar.name, PGEUrlDecodeFunc),
                                                                    MakeCSVPostProcessor(&updVar.newval, PGEUrlDecodeFunc)
                                                                    );
                                            eventdata.update_variable.push_back(updVar);
                                        }),
                                        // &update_var,
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

                eventdata.trigger_timer = (long)round(SMBX64::t65_to_ms(trigger_time_raw)/100.0);
                eventdata.timer_def.interval = SMBX64::t65_to_ms(timer_def_interval_raw);

                eventdata.array_id = FileData.events_array_id++;
                FileData.events.push_back(eventdata);

            } else if(identifier == "V") {
                // V|name|value
                vardata = CreateLvlVariable("var");
                dataReader.ReadDataLine(CSVDiscard(),
                                        MakeCSVPostProcessor(&vardata.name, PGEUrlDecodeFunc),
                                        &vardata.value /* save variable value as string
                                                          because in PGE is planned to have
                                                          variables to be universal */
                                        );
                FileData.variables.push_back(vardata);
            } else if(identifier == "S") {
                // S|name|script
                scriptdata = CreateLvlScript("doScript", LevelScript::LANG_TEASCRIPT);
                dataReader.ReadDataLine(CSVDiscard(),
                                        MakeCSVPostProcessor(&scriptdata.name, PGEUrlDecodeFunc),
                                        MakeCSVPostProcessor(&scriptdata.script, PGEBase64DecodeFunc)
                                        );
                FileData.scripts.push_back(scriptdata);
            } else if(identifier == "Su") {
                // Su|name|scriptu
                scriptdata = CreateLvlScript("doScript", LevelScript::LANG_TEASCRIPT);
                dataReader.ReadDataLine(CSVDiscard(),
                                        MakeCSVPostProcessor(&scriptdata.name, PGEUrlDecodeFunc),
                                        MakeCSVPostProcessor(&scriptdata.script, PGEBase64DecodeFuncA)
                                        );
                //Convert to LF
                PGE_ReplSTRING(scriptdata.script, "\r\n", "\n");
                FileData.scripts.push_back(scriptdata);
            } else {
                dataReader.ReadDataLine();
            }
        }//while is not EOF
    }
    catch(const std::exception& err)
    {
        // First we try to extract the line number out of the nested exception.
        const std::exception* curErr = &err;
        const std::nested_exception* possibleNestedException = dynamic_cast<const std::nested_exception*>(curErr);
        if(possibleNestedException){
            try{
                std::rethrow_exception(possibleNestedException->nested_ptr());
            }catch(const parse_error& parseErr){
                FileData.ERROR_linenum = parseErr.get_line_number();
            }catch(...){
                // Do Nothing
            }
        }


        // Now fill in the error data.
        FileData.ReadFileValid=false;
        FileData.ERROR_info = "Invalid file format, detected file SMBX-" + fromNum(newest_file_format) + "format\n"
                "Caused by: \n" + PGESTRING(exception_to_pretty_string(err).c_str());

        // If we were unable to find error line number from the exception, then get the line number from the file reader.
        if(FileData.ERROR_linenum == 0)
            FileData.ERROR_linenum = in.getCurrentLineNumber();

        FileData.ERROR_linedata = "";
        return false;
    }

    LevelAddInternalEvents(FileData);

    FileData.CurSection = 0;
    FileData.playmusic = 0;

    FileData.ReadFileValid = true;
    return true;
}




bool FileFormats::ReadSMBX38ALvlFile_OLD(PGE_FileFormats_misc::TextInput &in, LevelData &FileData)
{
    SMBX65_FileBegin();
    PGESTRING filePath = in.getFilePath();
    errorString.clear();
    CreateLevelData(FileData);

    FileData.RecentFormat = LevelData::SMBX38A;

    FileData.LevelName="";
    FileData.stars=0;
    FileData.CurSection=0;
    FileData.playmusic=0;

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
    LevelPhysEnv waters;
    LevelLayer layerdata;
    LevelSMBX64Event eventdata;
    //LevelEvent_Sets event_sets;
    LevelVariable vardata;
    LevelScript scriptdata;

    //Add path data
    if(!IsEmpty(filePath))
    {
        PGE_FileFormats_misc::FileInfo in_1(filePath);
        FileData.filename = in_1.basename();
        FileData.path = in_1.dirpath();
    }

    in.seek(0, PGE_FileFormats_misc::TextFileInput::begin);
    PGESTRINGList currentLine;

    nextLine();   //Read first line
    if( !PGE_StartsWith(line, "SMBXFile") ) //File format number
        goto badfile;
    else file_format = 65;

    while(!in.eof())
    {
        nextLine();   //Read second Line
        SMBX65_SplitLine(currentLine, line);

        if(currentLine.size()==0)
            continue;

        if(currentLine[0]=="A")//Level settings
        {
            for(int i=1;i<(signed)currentLine.size();i++)
            {
                PGESTRING &cLine=currentLine[i];
                switch(i)
                {
                case 1://Number of stars
                    {
                        if( !SMBX64::IsUInt(cLine) )
                            goto badfile;
                        else FileData.stars=toInt(cLine);
                    } break;
                case 2://Level title (URL Encoded!)
                    {
                        FileData.LevelName = PGE_URLDEC(cLine);
                    } break;
                //param3=a filename, when player died, the player will be sent to this level.
                case 3:
                    {
                        FileData.open_level_on_fail = PGE_URLDEC(cLine);
                    } break;
                //param4=normal entrance / to warp [0-WARPMAX]
                case 4:
                    {
                        if( !SMBX64::IsUInt(cLine) )
                            goto badfile;
                        else FileData.open_level_on_fail_warpID = toInt(cLine);
                    } break;
                }
            }
        }


        else
        if(currentLine[0]=="P1")//Player 1 point
        {
            playerdata = CreateLvlPlayerPoint(1);
            for(int i=1;i<(signed)currentLine.size();i++)
            {
                PGESTRING &cLine=currentLine[i];
                switch(i)
                {
                case 1://Pos X
                    {
                        if( !SMBX64::IsFloat(cLine) )
                            goto badfile;
                        else playerdata.x=(int)toFloat(cLine);
                    } break;
                case 2://Pos Y
                    {
                        if( !SMBX64::IsFloat(cLine) )
                            goto badfile;
                        else playerdata.y=(int)toFloat(cLine);
                    } break;
                }
            }
            FileData.players.push_back(playerdata);
        }


        else
        if(currentLine[0]=="P2")//Player 2 point
        {
            playerdata = CreateLvlPlayerPoint(2);
            for(int i=1;i<(signed)currentLine.size();i++)
            {
                PGESTRING &cLine=currentLine[i];
                switch(i)
                {
                case 1://Pos X
                    {
                        if( !SMBX64::IsFloat(cLine) )
                            goto badfile;
                        else playerdata.x=(int)toFloat(cLine);
                    } break;
                case 2://Pos Y
                    {
                        if( !SMBX64::IsFloat(cLine) )
                            goto badfile;
                        else playerdata.y=(int)toFloat(cLine);
                    } break;
                }
            }
            FileData.players.push_back(playerdata);
        }


        else
        if(currentLine[0]=="M")//Section
        {
            //M|id|x|y|w|h|b1|b2|b3|b4|b5|b6|music|background|musicfile
            section = CreateLvlSection();
            double x=0.0, y=0.0, w=0.0, h=0.0;
            for(int i=1;i<(signed)currentLine.size();i++)
            {
                PGESTRING &cLine=currentLine[i];
                switch(i)
                {
                //"id=[1-SectionMAX]
                case 1:
                    {
                        if( !SMBX64::IsUInt(cLine) )
                            goto badfile;
                        else section.id=(toInt(cLine)-1);
                        if(section.id<0) section.id = 0;
                    } break;
                //"x=Left size[-left/+right]
                case 2:
                    {
                        if( !SMBX64::IsFloat(cLine) )
                            goto badfile;
                        else x=toFloat(cLine);
                    } break;
                //"y=Top size[-down/+up]
                case 3:
                    {
                        if( !SMBX64::IsFloat(cLine) )
                            goto badfile;
                        else y=toFloat(cLine);
                    } break;
                //"w=width of the section[if (w < 800) w = 800]
                case 4:
                    {
                        if( !SMBX64::IsFloat(cLine) )
                            goto badfile;
                        else w = toFloat(cLine);
                        if((w<800.0)&&(w!=0)) w=800.0;
                    } break;
                //"h=height of the section[if (h < 600) h = 600]
                case 5:
                    {
                        if( !SMBX64::IsFloat(cLine) )
                            goto badfile;
                        else h = toFloat(cLine);
                        if((h<600.0)&&(h!=0)) h=600.0;
                    } break;
                //"b1=under water?[0=false !0=true]
                case 6:
                    {
                        section.underwater = (cLine!="0");
                    } break;
                //"b2=is x-level wrap[0=false !0=true]
                case 7:
                    {
                        section.wrap_h = (cLine!="0");
                    } break;
                //"b3=enable off screen exit[0=false !0=true]
                case 8:
                    {
                        section.OffScreenEn = (cLine!="0");
                    } break;
                //"b4=no turn back(x)[0=no x-scrolllock 1=scrolllock left 2=scrolllock right]
                case 9:
                    {
                        section.lock_left_scroll  = (cLine=="1");
                        section.lock_right_scroll = (cLine=="2");
                    } break;
                //"b5=no turn back(y)[0=no y-scrolllock 1=scrolllock up 2=scrolllock down]
                case 10:
                    {
                        section.lock_up_scroll = (cLine=="1");
                        section.lock_down_scroll = (cLine=="2");
                    } break;
                //"b6=is y-level wrap[0=false !0=true]
                case 11:
                    {
                        section.wrap_v= (cLine!="0");
                    } break;
                //"music=music number[same as smbx1.3]
                case 12:
                    {
                        if( !SMBX64::IsFloat(cLine) )
                            goto badfile;
                        else section.music_id=(unsigned int)toFloat(cLine);
                    } break;
                //"background=background number[same as the filename in 'background2' folder]
                case 13:
                    {
                        if( !SMBX64::IsFloat(cLine) )
                            goto badfile;
                        else section.background=(unsigned int)toFloat(cLine);
                    } break;
                //"musicfile=custom music file[***urlencode!***]
                case 14:
                    {
                        section.music_file = PGE_URLDEC(cLine);
                    } break;
                }
            }

            section.size_left = (long)round(x);
            section.size_top = (long)round(y);
            section.size_right = (long)round(x+w);
            section.size_bottom = (long)round(y+h);

            //Very important data! I'ts a camera position in the editor!
            section.PositionX = section.size_left-10;
            section.PositionY = section.size_top-10;

            if(section.id < (signed)FileData.sections.size())
                FileData.sections[section.id]=section;//Replace if already exists
            else
                FileData.sections.push_back(section); //Add Section in main array
        }

        else
        if(currentLine[0]=="B")//Blocks
        {
            //B|layer|id|x|y|contain|b1|b2|e1,e2,e3|w|h
            blockdata = CreateLvlBlock();
            for(int i=1;i<(signed)currentLine.size();i++)
            {
                PGESTRING &cLine=currentLine[i];
                switch(i)
                {
                //    layer=layer name["" == "Default"][***urlencode!***]
                case 1:
                    {
                        blockdata.layer=(cLine==""?"Default":PGE_URLDEC(cLine));
                    } break;
                //    id=block id
                case 2:
                    {
                        if( !SMBX64::IsUInt(cLine) )
                            goto badfile;
                        else blockdata.id = toInt(cLine);
                    } break;
                //    x=block position x
                case 3:
                    {
                        if( !SMBX64::IsFloat(cLine) )
                            goto badfile;
                        else blockdata.x=(long)round(toFloat(cLine));
                    } break;
                //    y=block position y
                case 4:
                    {
                        if( !SMBX64::IsFloat(cLine) )
                            goto badfile;
                        else blockdata.y=(long)round(toFloat(cLine));
                    } break;
                //    contain=containing npc number
                //        [1001-1000+NPCMAX] npc-id
                //        [1-999] coin number
                //        [0] nothing
                case 5:
                    {
                        if( !SMBX64::IsUInt(cLine) )
                            goto badfile;
                        else
                        {
                            long npcid=toInt(cLine);
                            blockdata.npc_id = ((npcid<1000) ? -1*npcid : npcid-1000 );
                        }
                    } break;
                //    b1=slippery[0=false !0=true]
                case 6:
                    {
                        blockdata.slippery=(cLine!="0");
                    } break;
                //    b2=invisible[0=false !0=true]
                case 7:
                    {
                        blockdata.invisible=(cLine!="0");
                    } break;
                case 8:
                    {
                    PGESTRINGList bevents;
                    SplitCSVStr(bevents, cLine);
                    for(int j=0; j<(signed)bevents.size(); j++)
                        {
                            PGESTRING &dLine=bevents[j];
                            switch(j)
                            {
                            //    e1=block destory event name[***urlencode!***]
                                case 0: blockdata.event_destroy=PGE_URLDEC(dLine); break;
                            //    e2=block hit event name[***urlencode!***]
                                case 1: blockdata.event_hit=PGE_URLDEC(dLine); break;
                            //    e3=no more object in layer event name[***urlencode!***]4
                                case 2: blockdata.event_emptylayer=PGE_URLDEC(dLine); break;
                            }
                        }
                    } break;
                //    w=width
                case 9:
                    {
                        if( !SMBX64::IsFloat(cLine) )
                            goto badfile;
                        else blockdata.w=(long)round(toFloat(cLine));
                    } break;
                //    h=height
                case 10:
                    {
                        if( !SMBX64::IsFloat(cLine) )
                            goto badfile;
                        else blockdata.h=(long)round(toFloat(cLine));
                    } break;
                }
            }
            blockdata.array_id = FileData.blocks_array_id++;
            FileData.blocks.push_back(blockdata);
        }

        else
        if(currentLine[0]=="T")//BGOs
        {
            //T|layer|id|x|y
            bgodata = CreateLvlBgo();
            for(int i=1;i<(signed)currentLine.size();i++)
            {
                PGESTRING &cLine=currentLine[i];
                switch(i)
                {
                //    layer=layer name["" == "Default"][***urlencode!***]
                case 1:
                    {
                        bgodata.layer=(cLine==""?"Default":PGE_URLDEC(cLine));
                    } break;
                //    id=background id
                case 2:
                    {
                        if( !SMBX64::IsUInt(cLine) )
                            goto badfile;
                        else bgodata.id=toInt(cLine);
                    } break;
                //    x=background position x
                case 3:
                    {
                        if( !SMBX64::IsFloat(cLine) )
                            goto badfile;
                        else bgodata.x=(long)round(toFloat(cLine));
                    } break;
                //    y=background position y
                case 4:
                    {
                        if( !SMBX64::IsFloat(cLine) )
                            goto badfile;
                        else bgodata.y=(long)round(toFloat(cLine));
                    } break;
                }
            }
            bgodata.array_id = FileData.bgo_array_id++;
            FileData.bgo.push_back(bgodata);
        }

        else
        if(currentLine[0]=="N")//NPC
        {
            npcdata = CreateLvlNpc();
            for(int i=1;i<(signed)currentLine.size();i++)
            {
                //next line: npcs
                //N|layer|id|x|y|b1,b2,b3,b4|sp|e1,e2,e3,e4,e5,e6,e7|a1,a2|c1[,c2,c3,c4,c5,c6,c7]|msg|
                PGESTRING &cLine=currentLine[i];
                switch(i)
                {
                //layer=layer name["" == "Default"][***urlencode!***]
                case 1:
                    {
                        npcdata.layer=(cLine==""?"Default":PGE_URLDEC(cLine));
                    } break;
                //id=npc id
                case 2:
                    {
                        if( !SMBX64::IsUInt(cLine) )
                            goto badfile;
                        else npcdata.id=toInt(cLine);
                    } break;
                //x=npc position x
                case 3:
                    {
                        if( !SMBX64::IsFloat(cLine) )
                            goto badfile;
                        else npcdata.x=(long)round(toFloat(cLine));
                    } break;
                //y=npc position y
                case 4:
                    {
                        if( !SMBX64::IsFloat(cLine) )
                            goto badfile;
                        else npcdata.y=(long)round(toFloat(cLine));
                    } break;

                //b1=[-1]left [0]random [1]right
                //b2=friendly npc
                //b3=don't move npc
                //b4=[1=npc91][2=npc96][3=npc283][4=npc284][5=npc300]
                case 5:
                    {
                        PGESTRINGList physparams;
                        SplitCSVStr(physparams, cLine);
                        for(int j=0; j<(signed)physparams.size(); j++)
                            {
                                PGESTRING &dLine=physparams[j];
                                switch(j)
                                {
                                //b1=[1]left [0]random [-1]right
                                    case 0:
                                        if( !SMBX64::IsSInt(dLine) )
                                        goto badfile;
                                        else npcdata.direct = -1 * toInt(dLine);//Convert into SMBX64/PGE-X Compatible form
                                    break;
                                //b2=friendly npc
                                    case 1:
                                        npcdata.friendly = ((dLine!="")&&(dLine!="0"));
                                    break;
                                //b3=don't move npc
                                    case 2:
                                        npcdata.nomove = ((dLine!="")&&(dLine!="0"));
                                        break;
                                //b4=[1=npc91][2=npc96][3=npc283][4=npc284][5=npc300]
                                    case 3:
                                        //CONTAINER with packed NPC
                                        if( !SMBX64::IsSInt(dLine) )
                                            goto badfile;
                                        int contID=toInt(dLine);
                                        if(contID==0)
                                            break;
                                        npcdata.contents = npcdata.id;
                                        switch(contID)
                                        {
                                            case 1: npcdata.id=91;break;
                                            case 2: npcdata.id=96;break;
                                            case 3: npcdata.id=283;break;
                                            case 4: npcdata.id=284;break;
                                            case 5: npcdata.id=300;break;
                                        }
                                        break;
                                }
                            }
                    } break;

                //sp=special option
                case 6:
                    {
                        if( !SMBX64::IsFloat(cLine) )
                            goto badfile;
                        else
                        {
                            npcdata.special_data = (long)round(toDouble(cLine));
                            switch(npcdata.id)
                            {
                            case 15: case 39: case 86: //Bind "Is Boss" flag for supported NPC's
                                npcdata.is_boss = (bool)npcdata.special_data;
                                npcdata.special_data = 0;
                            default: break;
                            }
                        }
                    } break;

                //Event slots
                case 7:
                    {
                    PGESTRINGList nevents;
                    SplitCSVStr(nevents, cLine);
                    for(int j=0; j<(signed)nevents.size(); j++)
                        {
                            PGESTRING &dLine=nevents[j];
                            switch(j) //    [***urlencode!***]
                            {
                            //    e1=death event
                                case 0: npcdata.event_die=PGE_URLDEC(dLine); break;
                            //    e2=talk event
                                case 1: npcdata.event_talk=PGE_URLDEC(dLine); break;
                            //    e3=activate event
                                case 2: npcdata.event_activate=PGE_URLDEC(dLine); break;
                            //    e4=no more object in layer event
                                case 3: npcdata.event_emptylayer=PGE_URLDEC(dLine); break;
                            //    e5=grabed event
                                case 4: npcdata.event_grab=PGE_URLDEC(dLine); break;
                            //    e6=next frame event
                                case 5: npcdata.event_nextframe=PGE_URLDEC(dLine); break;
                            //    e7=touch event
                                case 6: npcdata.event_touch=PGE_URLDEC(dLine); break;
                            }
                        }
                    } break;
                //Attach layer / Updated variable with
                case 8:
                    {
                    PGESTRINGList nelayers;
                    SplitCSVStr(nelayers, cLine);
                    for(int j=0; j<(signed)nelayers.size(); j++)
                        {
                            PGESTRING &dLine=nelayers[j];
                            switch(j) //    [***urlencode!***]
                            {
                            //    a1=layer name to attach
                                case 0: npcdata.attach_layer=PGE_URLDEC(dLine); break;
                            //    a2=variable name to send
                                case 1: npcdata.send_id_to_variable=PGE_URLDEC(dLine); break;
                            }
                        }
                    } break;
                //Generators
                case 9:
                    {
                        PGESTRINGList nevents;
                        SplitCSVStr(nevents, cLine);
                        for(int j=0; j<(signed)nevents.size(); j++)
                            {
                                PGESTRING &dLine=nevents[j];
                                if((j>0)&&(!npcdata.generator)) break;
                                switch(j)
                                {
                                //c1=generator enable
                                    case 0: npcdata.generator = ( (dLine != "") && (dLine!="0") ); break;
                                //[if c1!=0]
                                //    c2=generator period[1 frame]
                                    case 1:
                                        if( !SMBX64::IsSInt(dLine) )
                                        goto badfile;
                                        else npcdata.generator_period = (int)round( (toDouble(dLine)*10.0)/65.0 );//Convert into deci-seconds
                                    break;
                                //    c3=generator effect
                                //        c3-1[1=warp][0=projective][4=no effect]
                                //        c3-2[0=center][1=up][2=left][3=down][4=right]
                                //            [9=up+left][10=left+down][11=down+right][12=right+up]
                                //            if (c3-2)!=0
                                //              c3=4*(c3-1)+(c3-2)
                                //            else
                                //              c3=0
                                    case 2:
                                        if( !SMBX64::IsSInt(dLine) )
                                            goto badfile;
                                        else
                                        {
                                            int gentype=toInt(dLine);
                                            switch(gentype)
                                            {
                                                case 0:
                                                    npcdata.generator_type   = LevelNPC::NPC_GENERATOR_APPEAR;
                                                    npcdata.generator_direct = LevelNPC::NPC_GEN_CENTER;
                                                break;
                                                default:
                                                if(gentype<29)
                                                {
                                                    npcdata.generator_type   = SMBX65_NpcGeneratorTypes[gentype];
                                                    npcdata.generator_direct = SMBX65_NpcGeneratorDirections[gentype];
                                                } else {
                                                    npcdata.generator_type   = LevelNPC::NPC_GENERATOR_APPEAR;
                                                    npcdata.generator_direct = LevelNPC::NPC_GEN_CENTER;
                                                }
                                            }
                                            //Convert value into SMBX64 and PGEX compatible
                                            switch(npcdata.generator_type)
                                            {
                                                case 0:npcdata.generator_type   = LevelNPC::NPC_GENERATPR_PROJECTILE; break;
                                                case 1:npcdata.generator_type   = LevelNPC::NPC_GENERATOR_WARP; break;
                                                case 4:npcdata.generator_type   = LevelNPC::NPC_GENERATOR_APPEAR; break;
                                            }
                                        }
                                    break;
                                //    c4=generator direction[angle][when c3=0]
                                    case 3:
                                        {
                                            if( !SMBX64::IsFloat(dLine) )
                                                goto badfile;
                                            else npcdata.generator_custom_angle = toFloat(dLine);
                                        } break;
                                //    c5=batch[when c3=0][MAX=32]
                                    case 4:
                                        {
                                            if( !SMBX64::IsFloat(dLine) )
                                                goto badfile;
                                            else npcdata.generator_branches = (long)fabs(round(toFloat(dLine)));
                                        } break;
                                //    c6=angle range[when c3=0]
                                    case 5:
                                        {
                                            if( !SMBX64::IsFloat(dLine) )
                                                goto badfile;
                                            else npcdata.generator_angle_range = fabs(toFloat(dLine));
                                        } break;
                                //    c7=speed[when c3=0][float]
                                    case 6:
                                        {
                                            if( !SMBX64::IsFloat(dLine) )
                                                goto badfile;
                                            else npcdata.generator_initial_speed = toFloat(dLine);
                                        } break;
                                }
                            }
                    } break;
                //msg=message by this npc talkative[***urlencode!***]
                case 10:
                    {
                        npcdata.msg=PGE_URLDEC(cLine);
                    } break;
                }
            }
            npcdata.array_id = FileData.npc_array_id++;
            FileData.npc.push_back(npcdata);
        }

        else
        if(currentLine[0]=="Q")//next line: waters
        {
            //Q|layer|x|y|w|h|b1,b2,b3,b4,b5|event
            waters = CreateLvlPhysEnv();
            for(int i=1;i<(signed)currentLine.size();i++)
            {
                PGESTRING &cLine=currentLine[i];
                switch(i)
                {
                //    layer=layer name["" == "Default"][***urlencode!***]
                case 1:
                    {
                        waters.layer=(cLine==""?"Default":PGE_URLDEC(cLine));
                    } break;
                //x=position x
                case 2:
                    {
                        if( !SMBX64::IsFloat(cLine) )
                            goto badfile;
                        else waters.x=(long)round(toFloat(cLine));
                    } break;
                //y=position y
                case 3:
                    {
                        if( !SMBX64::IsFloat(cLine) )
                            goto badfile;
                        else waters.y=(long)round(toFloat(cLine));
                    } break;
                //w=width
                case 4:
                    {
                        if( !SMBX64::IsFloat(cLine) )
                            goto badfile;
                        else waters.w=(long)round(toFloat(cLine));
                    } break;
                //h=height
                case 5:
                    {
                        if( !SMBX64::IsFloat(cLine) )
                            goto badfile;
                        else waters.h=(long)round(toFloat(cLine));
                    } break;

                case 6:
                    {
                        PGESTRINGList nevents;
                        SplitCSVStr(nevents, cLine);
                        for(int j=0; j<(signed)nevents.size(); j++)
                            {
                                PGESTRING &dLine=nevents[j];
                                switch(j)
                                {
                                    //b1=liquid type
                                    //    01-Water[friction=0.5]
                                    //    02-Quicksand[friction=0.1]
                                    //    03-Custom Water
                                    //    04-Gravitational Field
                                    //    05-Event Once
                                    //    06-Event Always
                                    //    07-NPC Event Once
                                    //    08-NPC Event Always
                                    //    09-Click Event
                                    //    10-Collision Script
                                    //    11-Click Script
                                    //    12-Collision Event
                                    //    13-Air chamber
                                    case 0:
                                        {
                                            if( !SMBX64::IsFloat(dLine) )
                                                goto badfile;
                                            else waters.env_type = (int)round(toFloat(dLine))-1;
                                        } break;
                                    //b2=friction
                                    case 1:
                                        {
                                            if( !SMBX64::IsFloat(dLine) )
                                                goto badfile;
                                            else waters.friction = toFloat(dLine);
                                        } break;
                                    //b3=Acceleration Direction
                                    case 2:
                                        {
                                            if( !SMBX64::IsFloat(dLine) )
                                                goto badfile;
                                            else waters.accel_direct = toFloat(dLine);
                                        } break;
                                    //b4=Acceleration
                                    case 3:
                                        {
                                            if( !SMBX64::IsFloat(dLine) )
                                                goto badfile;
                                            else waters.accel = toFloat(dLine);
                                        } break;
                                    //b5=Maximum Velocity
                                    case 4:
                                        {
                                            if( !SMBX64::IsFloat(dLine) )
                                                goto badfile;
                                            else waters.accel = toFloat(dLine);
                                        } break;
                                }
                            }
                    } break;
                //event=touch event
                case 7:
                    {
                        waters.touch_event = PGE_URLDEC(cLine);
                    } break;
                }
            }
            waters.array_id = FileData.physenv_array_id++;
            FileData.physez.push_back(waters);
        }

        else
        if(currentLine[0]=="W")//next line: warps
        {
            //W|layer|x|y|ex|ey|type|enterd|exitd|sn,msg,hide|locked,noyoshi,canpick,bomb,hidef,anpc|lik|liid|noexit|wx|wy|le|we
            doordata = CreateLvlWarp();
            for(int i=1;i<(signed)currentLine.size();i++)
            {
                PGESTRING &cLine=currentLine[i];
                switch(i)
                {
                case 1:
                    {
                        doordata.layer=(cLine==""?"Default":PGE_URLDEC(cLine));
                    } break;
                //x=entrance position x
                case 2:
                    {
                        if( !SMBX64::IsFloat(cLine) )
                            goto badfile;
                        else doordata.ix=(long)round(toFloat(cLine));
                    } break;
                //y=entrance postion y
                case 3:
                    {
                        if( !SMBX64::IsFloat(cLine) )
                            goto badfile;
                        else doordata.iy=(long)round(toFloat(cLine));
                    } break;
                //ex=exit position x
                case 4:
                    {
                        if( !SMBX64::IsFloat(cLine) )
                            goto badfile;
                        else doordata.ox=(long)round(toFloat(cLine));
                    } break;
                //ey=exit position y
                case 5:
                    {
                        if( !SMBX64::IsFloat(cLine) )
                            goto badfile;
                        else doordata.oy=(long)round(toFloat(cLine));
                    } break;
                //type=[1=pipe][2=door][0=instant]
                case 6:
                    {
                        if( !SMBX64::IsUInt(cLine) )
                            goto badfile;
                        else doordata.type = toInt(cLine);
                    } break;
                //enterd=entrance direction[1=up 2=left 3=down 4=right]
                case 7:
                    {
                        if( !SMBX64::IsUInt(cLine) )
                            goto badfile;
                        else doordata.idirect = toInt(cLine);
                    } break;
                //exitd=exit direction[1=up 2=left 3=down 4=right]
                case 8:
                    {
                        if( !SMBX64::IsUInt(cLine) )
                            goto badfile;
                        else switch(toInt(cLine))//Convert into SMBX64/PGE-X Compatible form
                                {
                                    case 1: doordata.odirect = LevelDoor::EXIT_UP;break;
                                    case 2: doordata.odirect = LevelDoor::EXIT_LEFT;break;
                                    case 3: doordata.odirect = LevelDoor::EXIT_DOWN;break;
                                    case 4: doordata.odirect = LevelDoor::EXIT_RIGHT;break;
                                }
                    } break;

                case 9:
                    {
                    PGESTRINGList bevents;
                    SplitCSVStr(bevents, cLine);
                    for(int j=0; j<(signed)bevents.size(); j++)
                        {
                            PGESTRING &dLine=bevents[j];
                            switch(j)
                            {
                            //sn=need stars for enter
                            case 0:
                                {
                                    if( !SMBX64::IsSInt(dLine) )
                                        goto badfile;
                                    else doordata.stars = toInt(dLine);
                                } break;
                            //msg=a message when you have not enough stars
                            case 1:
                                {
                                    doordata.stars_msg=PGE_URLDEC(dLine);
                                } break;
                            //hide=hide the star number in this warp
                            case 2:
                                {
                                    if( !SMBX64::IsUInt(dLine) )
                                        goto badfile;
                                    else doordata.star_num_hide = (dLine!="0");
                                } break;
                            }
                        }
                    } break;
                case 10:
                    {
                    PGESTRINGList bevents;
                    SplitCSVStr(bevents, cLine);
                    for(int j=0; j<(signed)bevents.size(); j++)
                        {
                            PGESTRING &dLine=bevents[j];
                            switch(j)
                            {
                            //locked=locked
                            case 0:
                                {
                                    if( !SMBX64::IsUInt(dLine) )
                                        goto badfile;
                                    else doordata.locked = (bool)toInt(dLine);
                                } break;
                            //noyoshi=no yoshi
                            case 1:
                                {
                                    if( !SMBX64::IsUInt(dLine) )
                                        goto badfile;
                                    else doordata.novehicles = (bool)toInt(dLine);
                                } break;
                            //canpick=allow npc
                            case 2:
                                {
                                    if( !SMBX64::IsUInt(dLine) )
                                        goto badfile;
                                    else doordata.allownpc = (bool)toInt(dLine);
                                } break;
                            //bomb=need a bomb
                            case 3:
                                {
                                    if( !SMBX64::IsUInt(dLine) )
                                        goto badfile;
                                    else doordata.need_a_bomb = (bool)toInt(dLine);
                                } break;
                            //hide=hide the entry scene
                            case 4:
                                {
                                    if( !SMBX64::IsUInt(dLine) )
                                        goto badfile;
                                    else doordata.hide_entering_scene = (bool)toInt(dLine);
                                } break;
                            //anpc=allow npc interlevel
                            case 5:
                                {
                                    if( !SMBX64::IsUInt(dLine) )
                                        goto badfile;
                                    else doordata.allownpc_interlevel = (bool)toInt(dLine);
                                } break;
                         //Since SMBX-66-38A: {
                            //mini=Mini-Only
                            case 6:
                                {
                                    if( !SMBX64::IsUInt(dLine) )
                                        goto badfile;
                                    else doordata.special_state_required = (bool)toInt(dLine);
                                } break;
                            //size=Warp Size(pixel)
                            case 7:
                                {
                                    if( !SMBX64::IsUInt(dLine) )
                                        goto badfile;
                                    else doordata.length_i = toInt(dLine);
                                    doordata.length_o = doordata.length_i;
                                } break;
                            }
                         // }//Since SMBX-66-38A
                        }
                    } break;
                //lik=warp to level[***urlencode!***]
                case 11:
                    {
                        doordata.lname=PGE_URLDEC(cLine);
                    } break;
                //liid=normal enterance / to warp[0-WARPMAX]
                case 12:
                    {
                        if( !SMBX64::IsUInt(cLine) )
                            goto badfile;
                        else doordata.warpto = toInt(cLine);
                    } break;
                //noexit=level entrance
                case 13:
                    {
                        if( !SMBX64::IsUInt(cLine) )
                            goto badfile;
                        else doordata.lvl_i = (bool)toInt(cLine);
                        doordata.isSetIn = ((doordata.lvl_i)?false:true);
                    } break;
                //wx=warp to x on world map
                case 14:
                    {
                        if( !SMBX64::IsFloat(cLine) )
                            goto badfile;
                        else doordata.world_x = (long)round(toFloat(cLine));
                    } break;
                //wy=warp to y on world map
                case 15:
                    {
                        if( !SMBX64::IsFloat(cLine) )
                            goto badfile;
                        else doordata.world_y = (long)round(toFloat(cLine));
                    } break;
                //le=level exit
                case 16:
                    {
                        if( !SMBX64::IsUInt(cLine) )
                            goto badfile;
                        else doordata.lvl_o = (bool)toInt(cLine);
                        doordata.isSetOut = (((doordata.lvl_o)?false:true) || (doordata.lvl_i));
                    } break;
                //we=warp event[***urlencode!***]
                case 17:
                    {
                        doordata.event_enter=PGE_URLDEC(cLine);
                    } break;
                }
            }
            doordata.array_id = FileData.doors_array_id++;
            FileData.doors.push_back(doordata);
        }

        else
        if(currentLine[0]=="L")//Layers
        {
            //L|name|status
            layerdata = CreateLvlLayer();
            for(int i=1;i<(signed)currentLine.size();i++)
            {
                PGESTRING &cLine=currentLine[i];
                switch(i)
                {
                //    layer=layer name["" == "Default"][***urlencode!***]
                case 1:
                    {
                        layerdata.name=PGE_URLDEC(cLine);
                    } break;
                //    status=is vizible layer
                case 2:
                    {
                        layerdata.hidden = (cLine == "0");
                    } break;
                }
            }
            layerdata.array_id = FileData.layers_array_id++;
            FileData.layers.push_back(layerdata);
        }

        else
        if(currentLine[0]=="E")//next line: events
        {
            //E|name|msg|ea|el|elm|epy|eps|eef|ecn|evc|ene
            eventdata = CreateLvlEvent();
            for(int i=1;i<(signed)currentLine.size();i++)
            {
                PGESTRING &cLine=currentLine[i];
                switch(i)
                {
                //    name=event name[***urlencode!***]
                case 1:
                    {
                        eventdata.name=PGE_URLDEC(cLine);
                    } break;
                //    msg=spawn message
                case 2:
                    {
                        eventdata.msg=PGE_URLDEC(cLine);
                    } break;
                //ea=val,syntax
                case 3:
                    {
                        PGESTRINGList autorun;
                        SplitCSVStr(autorun, cLine);
                        for(int j=0; j<(signed)autorun.size(); j++)
                            {
                                PGESTRING &dLine=autorun[j];
                                switch(j)
                                {
                                //    val=[0=not auto start][1=auto start when level start][2=auto start when match all condition][3=start when called and match all condidtion]
                                    case 0:
                                        if( !SMBX64::IsFloat(dLine) )
                                            goto badfile;
                                        eventdata.autostart=(int)round(toFloat(dLine));
                                    break;
                                //    syntax=condidtion expression[***urlencode!***]
                                    case 1: eventdata.autostart_condition=PGE_URLDEC(dLine); break;
                                }
                            }
                    } break;
                //el=b/s1,s2...sn/h1,h2...hn/t1,t2...tn
                case 4:
                    {
                        PGESTRINGList EvPref;
                        SMBX65_SplitSubLine(EvPref, cLine);
                        for(int j=0; j<(signed)EvPref.size(); j++)
                            {
                                PGESTRING &dLine=EvPref[j];
                                switch(j)
                                {
                                //    b=no smoke[0=false !0=true]
                                    case 0: eventdata.nosmoke = (dLine!="0");
                                    break;
                                    //    [***urlencode!***]
                                    //    s(n)=show layer
                                    case 1:
                                        {
                                            PGESTRINGList showlayers;
                                            SplitCSVStr(showlayers, dLine);
                                            for(int k=0; k<(signed)showlayers.size(); k++)
                                                eventdata.layers_show.push_back(PGE_URLDEC(showlayers[k]));
                                        }
                                    break;
                                    //    l(n)=hide layer
                                    case 2:
                                        {
                                            PGESTRINGList hidelayers;
                                            SplitCSVStr(hidelayers, dLine);
                                            for(int k=0; k<(signed)hidelayers.size(); k++)
                                                eventdata.layers_hide.push_back(PGE_URLDEC(hidelayers[k]));
                                        }
                                    break;
                                    //    t(n)=toggle layer
                                    case 3:
                                        {
                                            PGESTRINGList togglelayers;
                                            SplitCSVStr(togglelayers, dLine);
                                            for(int k=0; k<(signed)togglelayers.size(); k++)
                                                eventdata.layers_toggle.push_back(PGE_URLDEC(togglelayers[k]));
                                        }
                                    break;
                                }
                            }
                    } break;
                //elm=elm1/elm2...elmn
                case 5:
                    {
                    //    elm(n)=layername,horizontal syntax,vertical syntax,way
                    //    layername=layer name for movement[***urlencode!***]
                    //    horizontal syntax, vertical syntax[***urlencode!***][syntax]
                    //    way=[0=by speed][1=by Coordinate]
                        PGESTRINGList EvMvLayers;
                        SMBX65_SplitSubLine(EvMvLayers, cLine);
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
                                            SMBX65_Exp2Float(ml.expression_x, ml.speed_x);
                                            eventdata.layer_speed_x=ml.speed_x;
                                        break;
                                        case 2:
                                            ml.expression_y=PGE_URLDEC(eLine);
                                            SMBX65_Exp2Float(ml.expression_y, ml.speed_y);
                                            eventdata.layer_speed_y=ml.speed_y;
                                        break;
                                        case 3:
                                            if(!SMBX64::IsUInt(eLine))
                                                goto badfile;
                                            ml.way=toInt(eLine);
                                        break;
                                    }
                                }
                                eventdata.moving_layers.push_back(ml);
                            }
                    } break;
                    //1,  0, 0, 0, 0, 0, 0, 0, 0,  0,  0, 1
                //epy=b1,b2,b3,b4,b5,b6,b7,b8,b9,b10,b11,b12
                case 6:
                    {
                        PGESTRINGList PlrCtrls;
                        SplitCSVStr(PlrCtrls, cLine);
                        for(int j=0; j<(signed)PlrCtrls.size(); j++)
                        {
                            PGESTRING &dLine=PlrCtrls[j];
                            switch(j)
                            {
                                //    b1=enable player controls
                                case 0: eventdata.ctrls_enable=(dLine!="0");break;
                                //    b2=drop
                                case 1: eventdata.ctrl_drop=(dLine!="0");break;
                                //    b3=alt run
                                case 2: eventdata.ctrl_altrun=(dLine!="0");break;
                                //    b4=run
                                case 3: eventdata.ctrl_run=(dLine!="0");break;
                                //    b5=jump
                                case 4: eventdata.ctrl_jump=(dLine!="0");break;
                                //    b6=alt jump
                                case 5: eventdata.ctrl_altjump=(dLine!="0");break;
                                //    b7=up
                                case 6: eventdata.ctrl_up=(dLine!="0");break;
                                //    b8=down
                                case 7: eventdata.ctrl_down=(dLine!="0");break;
                                //    b9=left
                                case 8: eventdata.ctrl_left=(dLine!="0");break;
                                //    b10=right
                                case 9: eventdata.ctrl_right=(dLine!="0");break;
                                //    b11=start
                                case 10: eventdata.ctrl_start=(dLine!="0");break;
                                //    b12=lock keyboard
                                case 11: eventdata.ctrl_lock_keyboard=(dLine!="0");break;
                            }
                        }
                    } break;
                //eps=esection/ebackground/emusic
                case 7:
                    {
                    //    esection=es1:es2...esn
                    //    ebackground=eb1:eb2...ebn
                    //    emusic=em1:em2...emn
                        PGESTRINGList EvSets;
                        SMBX65_SplitSubLine(EvSets, cLine);

                        PGESTRINGList ev_sections;
                        PGESTRINGList ev_bgs;
                        PGESTRINGList ev_musics;
                        //Collect entries
                        for(int j=0; j<(signed)EvSets.size(); j++)
                        {
                            PGESTRING &dLine=EvSets[j];
                            switch(j)
                            {
                            case 0: SMBX65_SplitLine(ev_sections, dLine, ':'); break;
                            case 1: SMBX65_SplitLine(ev_bgs, dLine, ':'); break;
                            case 2: SMBX65_SplitLine(ev_musics, dLine, ':'); break;
                            }
                        }
                        //fill entries
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
                                        if(!SMBX64::IsUInt(eLine))
                                            goto badfile;
                                        id=toInt(eLine)-1;
                                        break;
                                    //stype=[0=don't change][1=default][2=custom]
                                    case 1:
                                        if(!SMBX64::IsUInt(eLine))
                                            goto badfile;
                                        switch(toInt(eLine))
                                        {
                                            case 0: eventdata.sets[id].position_left=LevelEvent_Sets::LESet_Nothing; break;
                                            case 1: eventdata.sets[id].position_left=LevelEvent_Sets::LESet_ResetDefault; break;
                                            case 2: customSizes=true; break;
                                        }
                                        break;
                                    //x=left x coordinates for section [id][***urlencode!***][syntax]
                                    case 2:
                                        if(customSizes)
                                        {
                                            eventdata.sets[id].expression_pos_x = PGE_URLDEC(eLine);
                                            SMBX65_Exp2Int(eventdata.sets[id].expression_pos_x, eventdata.sets[id].position_left);
                                        }
                                        break;
                                    //y=top y coordinates for section [id][***urlencode!***][syntax]
                                    case 3:
                                        if(customSizes)
                                        {
                                            eventdata.sets[id].expression_pos_y = PGE_URLDEC(eLine);
                                            SMBX65_Exp2Int(eventdata.sets[id].expression_pos_y, eventdata.sets[id].position_top);
                                        } else {
                                            eventdata.sets[id].position_top = -1;
                                        }
                                        break;
                                    //w=width for section [id][***urlencode!***][syntax]
                                    case 4:
                                        if(customSizes)
                                        {
                                            eventdata.sets[id].expression_pos_w = PGE_URLDEC(eLine);
                                            SMBX65_Exp2Int(eventdata.sets[id].expression_pos_w, eventdata.sets[id].position_right);
                                            if(IsEmpty(eventdata.sets[id].expression_pos_w))
                                                eventdata.sets[id].position_right+=eventdata.sets[id].position_left;
                                        } else {
                                            eventdata.sets[id].position_right = -1;
                                        }
                                        break;
                                    //h=height for section [id][***urlencode!***][syntax]
                                    case 5:
                                        if(customSizes)
                                        {
                                            eventdata.sets[id].expression_pos_h = PGE_URLDEC(eLine);
                                            SMBX65_Exp2Int(eventdata.sets[id].expression_pos_h, eventdata.sets[id].position_bottom);
                                            if(IsEmpty(eventdata.sets[id].expression_pos_h))
                                                eventdata.sets[id].position_bottom += eventdata.sets[id].position_top;
                                        } else {
                                            eventdata.sets[id].position_bottom = -1;
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
                                            SMBX65_Exp2Float(eventdata.sets[id].expression_autoscrool_x, eventdata.sets[id].autoscrol_x);
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
                                            SMBX65_Exp2Float( eventdata.sets[id].expression_autoscrool_y, eventdata.sets[id].autoscrol_y );
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
                                        if(!SMBX64::IsUInt(eLine))
                                            goto badfile;
                                        id=toInt(eLine)-1;
                                        break;
                                    //btype=[0=don't change][1=default][2=custom]
                                    case 1:
                                        if(!SMBX64::IsUInt(eLine))
                                            goto badfile;
                                            switch(toInt(eLine))
                                            {
                                                case 0: eventdata.sets[id].background_id=LevelEvent_Sets::LESet_Nothing; break;
                                                case 1: eventdata.sets[id].background_id=LevelEvent_Sets::LESet_ResetDefault; break;
                                            case 2: customBg=true; eventdata.sets[id].background_id=0; break;
                                            }
                                        break;
                                    //backgroundid=[when btype=2]custom background id
                                    case 2:
                                        if(customBg)
                                        {
                                            if(!SMBX64::IsFloat(eLine))
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
                                        if(!SMBX64::IsUInt(eLine))
                                            goto badfile;
                                        id = toInt(eLine)-1;
                                        break;
                                    //mtype=[0=don't change][1=default][2=custom]
                                    case 1:
                                        if(!SMBX64::IsUInt(eLine))
                                            goto badfile;
                                            switch(toInt(eLine))
                                            {
                                                case 0: eventdata.sets[id].music_id=LevelEvent_Sets::LESet_Nothing; break;
                                                case 1: eventdata.sets[id].music_id=LevelEvent_Sets::LESet_ResetDefault; break;
                                                case 2:
                                                default: customMusics=true; eventdata.sets[id].music_id = 0; break;
                                            }
                                        break;
                                    //musicid=[when mtype=2]custom music id
                                    case 2:
                                        if(customMusics)
                                        {
                                            if(!SMBX64::IsFloat(eLine))
                                                goto badfile;
                                            eventdata.sets[id].music_id=(long)round(toDouble(eLine));
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
                    } break;
                //eef=sound/endgame/ce1/ce2...cen
                case 8:
                    {
                    PGESTRINGList Effects;
                    SMBX65_SplitSubLine(Effects, cLine);
                    for(int j=0; j<(signed)Effects.size(); j++)
                        {
                            PGESTRING &dLine=Effects[j];
                            switch(j)
                            {
                                //sound=play sound number
                                case 0:
                                    if(!SMBX64::IsUInt(dLine))
                                        goto badfile;
                                    eventdata.sound_id = toInt(dLine);
                                break;
                                //    endgame=[0=none][1=bowser defeat]
                                case 1:
                                    if(!SMBX64::IsUInt(dLine))
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
                                            if(!SMBX64::IsUInt(eLine))
                                                goto badfile;
                                            effect.id = toInt(eLine);
                                            break;
                                        //        x=effect position x[***urlencode!***][syntax]
                                        case 1:
                                            effect.expression_x = PGE_URLDEC(eLine);
                                            SMBX65_Exp2Int( effect.expression_x, effect.x );
                                            break;
                                        //        y=effect position y[***urlencode!***][syntax]
                                        case 2:
                                            effect.expression_y = PGE_URLDEC(eLine);
                                            SMBX65_Exp2Int( effect.expression_y, effect.y );
                                            break;
                                        //        sx=effect horizontal speed[***urlencode!***][syntax]
                                        case 3:
                                            effect.expression_sx = PGE_URLDEC(eLine);
                                            SMBX65_Exp2Float( effect.expression_sx, effect.speed_x );
                                            break;
                                        //        sy=effect vertical speed[***urlencode!***][syntax]
                                        case 4:
                                            effect.expression_sy = PGE_URLDEC(eLine);
                                            SMBX65_Exp2Float( effect.expression_sy, effect.speed_y );
                                            break;
                                        //        grv=to decide whether the effects are affected by gravity[0=false !0=true]
                                        case 5: effect.gravity = (eLine!="0"); break;
                                        //        fsp=frame speed of effect generated
                                        case 6:
                                            if(!SMBX64::IsUInt(eLine))
                                                goto badfile;
                                            effect.fps = toInt(eLine);
                                            break;
                                        //        life=effect existed over this time will be destroyed.
                                        case 7:
                                            if(!SMBX64::IsUInt(eLine))
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
                    } break;
                //ecn=cn1/cn2...cnn
                case 9:
                    {
                        PGESTRINGList SpawnNPCs;
                        SMBX65_SplitSubLine(SpawnNPCs, cLine);
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
                                    if(!SMBX64::IsUInt(eLine))
                                        goto badfile;
                                    spawnnpc.id=toInt(eLine);
                                    break;
                                //x=npc position x[***urlencode!***][syntax]
                                case 1:
                                    spawnnpc.expression_x = PGE_URLDEC(eLine);
                                    SMBX65_Exp2Int( spawnnpc.expression_x, spawnnpc.x );
                                    break;
                                //y=npc position y[***urlencode!***][syntax]
                                case 2:
                                    spawnnpc.expression_y = PGE_URLDEC(eLine);
                                    SMBX65_Exp2Int( spawnnpc.expression_y, spawnnpc.y );
                                    break;
                                //sx=npc horizontal speed[***urlencode!***][syntax]
                                case 3:
                                    spawnnpc.expression_sx = PGE_URLDEC(eLine);
                                    SMBX65_Exp2Float( spawnnpc.expression_sx, spawnnpc.speed_x );
                                    break;
                                //sy=npc vertical speed[***urlencode!***][syntax]
                                case 4:
                                    spawnnpc.expression_sy = PGE_URLDEC(eLine);
                                    SMBX65_Exp2Float( spawnnpc.expression_sy, spawnnpc.speed_y );
                                    break;
                                //sp=advanced settings of generated npc
                                case 5:
                                    if(!SMBX64::IsSInt(eLine))
                                        goto badfile;
                                    spawnnpc.special=toInt(eLine);
                                    break;
                                }
                            }
                            eventdata.spawn_npc.push_back(spawnnpc);
                        }
                    } break;
                //evc=vc1/vc2...vcn
                case 10:
                    {
                        LevelEvent_UpdateVariable updVar;
                        PGESTRINGList updVars;
                        SplitCSVStr(updVars, cLine);
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
                        if(!IsEmpty(updVar.name))
                            eventdata.update_variable.push_back(updVar);
                    } break;
                //ene=nextevent/timer/apievent/scriptname
                case 11:
                    {
                        PGESTRINGList extraProps;
                        SMBX65_SplitSubLine(extraProps, cLine);
                        for(int j=0; j<(signed)extraProps.size(); j++)
                        {
                            PGESTRING &dLine=extraProps[j];
                            switch(j)
                            {
                            //nextevent=name,delay
                            case 0:
                                {
                                    PGESTRINGList TriggerEvent;
                                    SplitCSVStr(TriggerEvent, dLine);
                                    for(int k=0; k<(signed)TriggerEvent.size(); k++)
                                    {
                                        PGESTRING &eLine=TriggerEvent[k];
                                        switch(k)
                                        {
                                        //name=trigger event name[***urlencode!***]
                                        case 0: eventdata.trigger=PGE_URLDEC(eLine); break;
                                        //delay=trigger delay[1 frame]
                                        case 1:
                                            if(!SMBX64::IsUInt(eLine))
                                                goto badfile;
                                            //Convert 1/65 seconds into 1/10 seconds for SMBX-64 Standard
                                            eventdata.trigger_timer = (long)round(SMBX64::t65_to_ms(toFloat(eLine))/100.0);
                                            break;
                                        }
                                    }
                                } break;
                            //timer=enable,count,interval,type,show
                            case 1:
                                {
                                    PGESTRINGList Timer;
                                    SplitCSVStr(Timer, dLine);
                                    for(int k=0; k<(signed)Timer.size(); k++)
                                    {
                                        PGESTRING &eLine=Timer[k];
                                        switch(k)
                                        {
                                        //enable=enable the game timer controlling[0=false !0=true]
                                        case 0: eventdata.timer_def.enable=(eLine!="0"); break;
                                        //count=set the time left of the game timer
                                        case 1:
                                            if(!SMBX64::IsUInt(eLine))
                                                goto badfile;
                                            //Convert 1/65 seconds into milliseconds units
                                            eventdata.timer_def.count = toInt(eLine);
                                            break;
                                        //interval=set the time count interval of the game timer
                                        case 2:
                                            if(!SMBX64::IsFloat(eLine))
                                                goto badfile;
                                            //Convert 1/65 seconds into milliseconds units
                                            eventdata.timer_def.interval = SMBX64::t65_to_ms(toDouble(eLine));
                                            break;
                                        //type=to choose the way timer counts[0=counting down][1=counting up]
                                        case 3:
                                            if(!SMBX64::IsUInt(eLine))
                                                goto badfile;
                                            eventdata.timer_def.count_dir = toInt(eLine);
                                            break;
                                        //show=to choose whether the game timer is showed in hud[0=false !0=true]
                                        case 4:
                                            eventdata.timer_def.show = (eLine!="0");
                                            break;
                                        }
                                    }
                                } break;
                            //    apievent=the id of apievent
                            case 2:
                                if(!SMBX64::IsUInt(dLine))
                                    goto badfile;
                                eventdata.trigger_api_id = toInt(dLine);
                                break;
                            //    scriptname=script name[***urlencode!***]
                            case 3: eventdata.trigger_script = PGE_URLDEC(dLine); break;
                            }
                        }
                    } break;
                }
            }
            eventdata.array_id = FileData.events_array_id++;
            FileData.events.push_back(eventdata);
        }

        else
        if(currentLine[0]=="V")//next line: variables
        {
            //V|name|value
            vardata = CreateLvlVariable("var");
            for(int i=1;i<(signed)currentLine.size();i++)
            {
                PGESTRING &cLine=currentLine[i];
                switch(i)
                {
                //name=variable name[***urlencode!***]
                case 1:
                    {
                        vardata.name=PGE_URLDEC(cLine);
                    } break;
                //value=initial value of the variable
                case 2:
                    {
                        if( !SMBX64::IsUInt(cLine) )
                            goto badfile;
                        else vardata.value = cLine; /*save variable value as string
                                                      because in PGE is planned to have
                                                      variables to be universal*/
                    } break;
                }
            }
            FileData.variables.push_back(vardata);
        }

        else
        if(currentLine[0]=="S") //next line: scripts
        {
            //S|name|script
            scriptdata = CreateLvlScript("doScript", LevelScript::LANG_TEASCRIPT);
            for(int i=1;i<(signed)currentLine.size();i++)
            {
                PGESTRING &cLine=currentLine[i];
                switch(i)
                {
                //name=name of script[***urlencode!***]
                case 1:
                    {
                        scriptdata.name=PGE_URLDEC(cLine);
                    } break;
                //script=script[***base64encode!***][utf-8]
                case 2:
                    {
                        scriptdata.script=PGE_BASE64DEC(cLine);
                    } break;
                }
            }
            FileData.variables.push_back(vardata);
        }

        else
        if(currentLine[0]=="Su") //next line: scripts (saved as ASCII)
        {
            //Su|name|scriptu
            scriptdata = CreateLvlScript("doScript", LevelScript::LANG_TEASCRIPT);
            for(int i=1;i<(signed)currentLine.size();i++)
            {
                PGESTRING &cLine=currentLine[i];
                switch(i)
                {
                //name=name of script[***urlencode!***]
                case 1:
                    {
                        scriptdata.name=PGE_URLDEC(cLine);
                    } break;
                //scriptu=script[***base64encode!***][ASCII]
                case 2:
                    {
                        scriptdata.script=PGE_BASE64DEC_A(cLine);
                        PGE_ReplSTRING(scriptdata.script, "\r\n", "\n");
                    } break;
                }
            }
            FileData.scripts.push_back(scriptdata);
        }

    }//while is not EOF

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


//*********************************************************
//****************WRITE FILE FORMAT************************
//*********************************************************

bool FileFormats::WriteSMBX38ALvlFileF(PGESTRING filePath, LevelData &FileData)
{
    errorString.clear();
    PGE_FileFormats_misc::TextFileOutput file;
    if(!file.open(filePath, false, true, PGE_FileFormats_misc::TextOutput::truncate))
    {
        errorString="Failed to open file for write";
        return false;
    }
    return WriteSMBX38ALvlFile(file, FileData);
}

bool FileFormats::WriteSMBX38ALvlFileRaw(LevelData &FileData, PGESTRING &rawdata)
{
    errorString.clear();
    PGE_FileFormats_misc::RawTextOutput file;
    if(!file.open(&rawdata, PGE_FileFormats_misc::TextOutput::truncate))
    {
        errorString="Failed to open raw string for write";
        return false;
    }
    return WriteSMBX38ALvlFile(file, FileData);
}

bool FileFormats::WriteSMBX38ALvlFile(PGE_FileFormats_misc::TextOutput &out, LevelData &FileData)
{
    long i = 0;
    FileData.RecentFormat = LevelData::SMBX38A;

    //Count placed stars on this level
    FileData.stars = smbx64CountStars(FileData);

    #define layerNotDef(lr) ( ((lr) != "Default") ? PGE_URLENC(lr) : "" )

    //========================================================
    //Data type markers:
    //A 	    – Level header settings
    //P1, P2 	– Player spawn points
    //M       – Section settings
    //B       – blocks
    //T       – Background objects
    //N       – Non-playable characters
    //Q       – Liquid/Environment boxes
    //W       – Warp entries
    //L       – Layers
    //E       – Events
    //V       – Local level variables
    //S       – UTF-8 encoded local level scripts
    //Su      – ASCII-encoded local level scripts
    //--------------------------------------------------------
    //line 1:
    //    SMBXFile??
    //    ??=Version number
    out << "SMBXFile66\n";
    //next line: level settings
    //    A|param1|param2[|param3|param4]
    //    []=optional
    out << "A";
    //    param1=the number of stars on this level
    out << "|" << fromNum(FileData.stars);
    //    param2=level title
    out << "|" << PGE_URLENC(FileData.LevelName);
    if(!IsEmpty(FileData.open_level_on_fail))
    {
        //    param3=a filename, when player died, the player will be sent to this level.
        out << "|" << PGE_URLENC(FileData.open_level_on_fail);
        //    param4=normal entrance / to warp [0-WARPMAX]
        out << "|" << fromNum(FileData.open_level_on_fail_warpID);
    }
    out << "\n";

    //next line: player start points
    for(i=0;i<(signed)FileData.players.size(); i++)
    {
    //    P1|x1|y1
    //    P2|x2|y2
        PlayerPoint &pl=FileData.players[i];
        out << "P" << fromNum(pl.id);
    //    x1=first player position x
    //    x2=second player position x
        out << "|" << fromNum(pl.x);
    //    y1=first player position y
    //    y2=second player position y
        out << "|" << fromNum(pl.y);
        out << "\n";
    }

    //next line: section properties
    for(i=0; i<(signed)FileData.sections.size(); i++)
    {
    //    M|id|x|y|w|h|b1|b2|b3|b4|b5|b6|music|background|musicfile
        LevelSection &sct = FileData.sections[i];
        out << "M";
    //    id=[1-SectionMAX]
        out << "|" << fromNum(sct.id+1);
    //    x=Left size[-left/+right]
        out << "|" << fromNum(sct.size_left);
    //    y=Top size[-down/+up]
        out << "|" << fromNum(sct.size_top);
    //    w=width of the section[if (w < 800) w = 800]
        out << "|" << fromNum(sct.size_right-sct.size_left);
    //    h=height of the section[if (h < 600) h = 600]
        out << "|" << fromNum(sct.size_bottom-sct.size_top);
    //    b1=under water?[0=false !0=true]
        out << "|" << fromNum((int)sct.underwater);
    //    b2=is x-level wrap[0=false !0=true]
        out << "|" << fromNum(sct.wrap_h);
    //    b3=enable off screen exit[0=false !0=true]
        out << "|" << fromNum((int)sct.OffScreenEn);
    //    b4=no turn back(x)[0=no x-scrolllock 1=scrolllock left 2=scrolllock right]
        if((!sct.lock_left_scroll) && (!sct.lock_right_scroll))
            out << "|" << fromNum(0);
        else if((sct.lock_left_scroll) && (!sct.lock_right_scroll))
            out << "|" << fromNum(1);
        else
            out << "|" << fromNum(2);
    //    b5=no turn back(y)[0=no y-scrolllock 1=scrolllock up 2=scrolllock down]
        if((!sct.lock_up_scroll) && (!sct.lock_down_scroll))
            out << "|" << fromNum(0);
        else if((sct.lock_up_scroll) && (!sct.lock_down_scroll))
            out << "|" << fromNum(1);
        else
            out << "|" << fromNum(2);
    //    b6=is y-level wrap[0=false !0=true]
        out << "|" << fromNum(sct.wrap_v);
    //    music=music number[same as smbx1.3]
        out << "|" << fromNum(sct.music_id);
    //    background=background number[same as the filename in 'background2' folder]
        out << "|" << fromNum(sct.background);
    //    musicfile=custom music file[***urlencode!***]
        out << "|" << PGE_URLENC(sct.music_file);
        out << "\n";
    }

    //next line: blocks
    for(i=0; i<(signed)FileData.blocks.size(); i++)
    {
    //    B|layer|id|x|y|contain|b1|b2|e1,e2,e3|w|h
        LevelBlock &blk = FileData.blocks[i];
        out << "B";
    //    layer=layer name["" == "Default"][***urlencode!***]
        out << "|" << layerNotDef(blk.layer);
    //    id=block id
        out << "|" << fromNum(blk.id);
    //    x=block position x
        out << "|" << fromNum(blk.x);
    //    y=block position y
        out << "|" << fromNum(blk.y);
    //    contain=containing npc number
    //        [1001-1000+NPCMAX] npc-id
    //        [1-999] coin number
    //        [0] nothing
        out << "|" << fromNum(blk.npc_id <= 0 ? (-1 * blk.npc_id) : (blk.npc_id+1000) );
    //    b1=slippery[0=false !0=true]
        out << "|" << fromNum((int)blk.slippery);
    //    b2=invisible[0=false !0=true]
        out << "|" << fromNum((int)blk.invisible);
    //    e1=block destory event name[***urlencode!***]
        out << "|" << PGE_URLENC(blk.event_destroy);
    //    e2=block hit event name[***urlencode!***]
        out << "," << PGE_URLENC(blk.event_hit);
    //    e3=no more object in layer event name[***urlencode!***]4
        out << "," << PGE_URLENC(blk.event_emptylayer);
    //    w=width
        out << "|" << fromNum(blk.w);
    //    h=height
        out << "|" << fromNum(blk.h);
        out << "\n";
    }


    //next line: backgrounds
    for(i=0; i<(signed)FileData.bgo.size(); i++)
    {
    //    T|layer|id|x|y
        LevelBGO &bgo = FileData.bgo[i];
        out << "T";
    //    layer=layer name["" == "Default"][***urlencode!***]
        out << "|" << layerNotDef(bgo.layer);
    //    id=background id
        out << "|" << fromNum(bgo.id);
    //    x=background position x
        out << "|" << fromNum(bgo.x);
    //    y=background position y
        out << "|" << fromNum(bgo.y);
        out << "\n";
    }

    //next line: npcs
    for(i=0; i<(signed)FileData.npc.size(); i++)
    {
        LevelNPC &npc = FileData.npc[i];
        //Pre-convert some data into SMBX-38A compatible format
        int npcID = npc.id;
        int containerType = 0;
        int specialData = npc.special_data;
        switch(npcID)//Convert npcID and contents ID into container type
        {
            case 91:  containerType=1; break;
            case 96:  containerType=2; break;
            case 283: containerType=3; break;
            case 284: containerType=4; break;
            case 300: containerType=5; break;
            default:
            containerType=0; break;
        }
        if(containerType != 0)
        {
            //Set NPC-ID of contents as main NPC-ID for this NPC
            npcID = npc.contents;
        }

        //Convert "Is Boss" flag into special ID
        switch(npc.id)
        {
            case 15: case 39: case 86:
            if(npc.is_boss)
            {
                specialData = (int)npc.is_boss;
            }
            default: break;
        }
        //Convert generator type and direction into SMBX-38A Compatible format
        int genType_1 = npc.generator_type;
        //Swap "Appear" and "Projectile" types
        switch(genType_1)
        {
            case 0: genType_1 = 2;break;
            case 2: genType_1 = 0;break;
        }
        int genType_2 = npc.generator_direct;
        int genType = (genType_2 != 0) ? ( (4*genType_1) + genType_2 ) : 0 ;

    //    N|layer|id|x|y|b1,b2,b3,b4|sp|e1,e2,e3,e4,e5,e6,e7|a1,a2|c1[,c2,c3,c4,c5,c6,c7]|msg|
        out << "N";
    //    layer=layer name["" == "Default"][***urlencode!***]
        out << "|" << layerNotDef(npc.layer);
    //    id=npc id
        out << "|" << fromNum(npcID);
    //    x=npc position x
        out << "|" << fromNum(npc.x);
    //    y=npc position y
        out << "|" << fromNum(npc.y);
    //    b1=[1]left [0]random [-1]right
            out << "|" << fromNum( -1 * npc.direct );
    //    b2=friendly npc
            out << "," << fromNum((int)npc.friendly);
    //    b3=don't move npc
            out << "," << fromNum((int)npc.nomove);
    //    b4=[1=npc91][2=npc96][3=npc283][4=npc284][5=npc300]
            out << "," << fromNum( containerType );
    //    sp=special option
        out << "|" << fromNum( specialData );
    //        [***urlencode!***]
    //        e1=death event
        out << "|" << PGE_URLENC(npc.event_die);
    //        e2=talk event
            out << "," << PGE_URLENC(npc.event_talk);
    //        e3=activate event
            out << "," << PGE_URLENC(npc.event_activate);
    //        e4=no more object in layer event
            out << "," << PGE_URLENC(npc.event_emptylayer);
    //        e5=grabed event
            out << "," << PGE_URLENC(npc.event_grab);
    //        e6=next frame event
            out << "," << PGE_URLENC(npc.event_nextframe);
    //        e7=touch event
            out << "," << PGE_URLENC(npc.event_touch);
    //        a1=layer name to attach
            out << "|" << PGE_URLENC(npc.attach_layer);
    //        a2=variable name to send
            out << "," << PGE_URLENC(npc.send_id_to_variable);
    //    c1=generator enable
        out << "|" << fromNum((int)npc.generator);
    //        [if c1!=0]
        if(npc.generator)
        {
    //        c2=generator period[1 frame]
            //Convert deciseconds into frames with rounding
            out << "," << fromNum( (int)round(  ((double)npc.generator_period * 65.0) / 10.0 )  );
    //        c3=generator effect
    //            c3-1 [1=warp][0=projective][4=no effect]
    //            c3-2 [0=center][1=up][2=left][3=down][4=right][9=up+left][10=left+down][11=down+right][12=right+up]
    //                if (c3-2)!=0
    //                c3=4*(c3-1)+(c3-2)
    //                else
    //                c3=0
            out << "," << fromNum(genType);
    //        c4=generator direction[angle][when c3=0]
            out << "," << fromNum(npc.generator_custom_angle);
    //        c5=batch[when c3=0][MAX=32]
            out << "," << fromNum(npc.generator_branches);
    //        c6=angle range[when c3=0]
            out << "," << fromNum(npc.generator_angle_range);
    //        c7=speed[when c3=0][float]
            out << "," << fromNum(npc.generator_initial_speed);
        }
    //    msg=message by this npc talkative[***urlencode!***]
        out << "|" << PGE_URLENC(npc.msg);
        out << "\n";
    }

    //next line: warps
    for(i=0; i<(signed)FileData.doors.size(); i++)
    {
        LevelDoor &door = FileData.doors[i];

        if( ((!door.lvl_o) && (!door.lvl_i)) || ((door.lvl_o) && (!door.lvl_i)) )
            if(!door.isSetIn) continue; // Skip broken warp entry
        if( ((!door.lvl_o) && (!door.lvl_i)) || ((door.lvl_i)) )
            if(!door.isSetOut) continue; // Skip broken warp entry

        int oDirect = door.odirect;
        switch(oDirect)//Convert from SMBX64/PGE-X into SMBX-38A compatible form
        {
            case LevelDoor::EXIT_UP:    oDirect = 1; break;
            case LevelDoor::EXIT_LEFT:  oDirect = 2; break;
            case LevelDoor::EXIT_DOWN:  oDirect = 3; break;
            case LevelDoor::EXIT_RIGHT: oDirect = 4; break;
        }

    //    W|layer|x|y|ex|ey|type|enterd|exitd|sn,msg,hide|locked,noyoshi,canpick,bomb,hidef,anpc,mini,size|lik|liid|noexit|wx|wy|le|we
        out << "W";
    //    layer=layer name["" == "Default"][***urlencode!***]
        out << "|" << layerNotDef(door.layer);
    //    x=entrance position x
        out << "|" << fromNum(door.ix);
    //    y=entrance postion y
        out << "|" << fromNum(door.iy);
    //    ex=exit position x
        out << "|" << fromNum(door.ox);
    //    ey=exit position y
        out << "|" << fromNum(door.oy);
    //    type=[1=pipe][2=door][0=instant]
        out << "|" << fromNum(door.type);
    //    enterd=entrance direction[1=up 2=left 3=down 4=right]
        out << "|" << fromNum(door.idirect);
    //    exitd=exit direction[1=up 2=left 3=down 4=right]
        out << "|" << fromNum(oDirect);
    //    sn=need stars for enter
        out << "|" << fromNum(door.stars);
    //    msg=a message when you have not enough stars
        out << "," << PGE_URLENC(door.stars_msg);
    //    hide=hide the star number in this warp
        out << "," << fromNum((int)door.star_num_hide);
    //    locked=locked
        out << "|" << fromNum((int)door.locked);
    //    noyoshi=no yoshi
        out << "," << fromNum((int)door.novehicles);
    //    canpick=allow npc
        out << "," << fromNum((int)door.allownpc);
    //    bomb=need a bomb
        out << "," << fromNum((int)door.need_a_bomb);
    //    hide=hide the entry scene
        out << "," << fromNum((int)door.hide_entering_scene);
    //    anpc=allow npc interlevel
        out << "," << fromNum((int)door.allownpc_interlevel);
    //    mini=Mini-Only
        out << "," << fromNum((int)door.special_state_required);
    //    size=Warp Size(pixel)
        out << "," << fromNum(door.length_i);
    //    lik=warp to level[***urlencode!***]
        out << "|" << PGE_URLENC(door.lname);
    //    liid=normal enterance / to warp[0-WARPMAX]
        out << "|" << fromNum(door.warpto);
    //    noexit=level entrance
        out << "|" << fromNum((int)door.lvl_i);
    //    wx=warp to x on world map
        out << "|" << fromNum(door.world_x);
    //    wy=warp to y on world map
        out << "|" << fromNum(door.world_y);
    //    le=level exit
        out << "|" << fromNum((int)door.lvl_o);
    //    we=warp event[***urlencode!***]
        out << "|" << PGE_URLENC(door.event_enter);
        out << "\n";
    }

    //next line: waters
    for(i=0; i<(signed)FileData.physez.size(); i++)
    {
        LevelPhysEnv &pez = FileData.physez[i];
        /*TRIVIA: It is NOT a PEZ candy brand, just "Physical Environment Zone" :-P*/
    //    Q|layer|x|y|w|h|b1,b2,b3,b4,b5|event
        out << "Q";
    //    layer=layer name["" == "Default"][***urlencode!***]
        out << "|" << layerNotDef(pez.layer);
    //    x=position x
        out << "|" << fromNum(pez.x);
    //    y=position y
        out << "|" << fromNum(pez.y);
    //    w=width
        out << "|" << fromNum(pez.w);
    //    h=height
        out << "|" << fromNum(pez.h);
    //    b1=liquid type
    //        01-Water[friction=0.5]
    //        02-Quicksand[friction=0.1]
    //        03-Custom Water
    //        04-Gravitational Field
    //        05-Event Once
    //        06-Event Always
    //        07-NPC Event Once
    //        08-NPC Event Always
    //        09-Click Event
    //        10-Collision Script
    //        11-Click Script
    //        12-Collision Event
    //        13-Air
        out << "|" << fromNum((pez.env_type+1));
    //    b2=friction
        out << "," << fromNum(pez.friction);
    //    b3=Acceleration Direction
        out << "," << fromNum(pez.accel_direct);
    //    b4=Acceleration
        out << "," << fromNum(pez.accel);
    //    b5=Maximum Velocity
        out << "," << fromNum(pez.max_velocity);
    //    event=touch event
        out << "|" << PGE_URLENC(pez.touch_event);
        out << "\n";
    }

    for(i=0; i<(signed)FileData.layers.size(); i++)
    {
        LevelLayer &lyr = FileData.layers[i];
    //next line: layers
    //    L|name|status
        out << "L";
    //    name=layer name[***urlencode!***]
        out << "|" << PGE_URLENC(lyr.name);
    //    status=is vizible layer
        out << "|" << fromNum((int)(!lyr.hidden));
        out << "\n";
    }



    //next line: events
    for(i=0; i<(signed)FileData.events.size(); i++)
    {
        LevelSMBX64Event &evt = FileData.events[i];
    //    E|name|msg|ea|el|elm|epy|eps|eef|ecn|evc|ene
        out << "E";
    //    name=event name[***urlencode!***]
        out << "|" << PGE_URLENC(evt.name);
    //    msg=show message after start event[***urlencode!***]
        out << "|" << PGE_URLENC(evt.msg);
    //    ea=val,syntax
    //        val=[0=not auto start][1=auto start when level start][2=auto start when match all condition][3=start when called and match all condidtion]
        out << "|" << fromNum(evt.autostart);
    //        syntax=condidtion expression[***urlencode!***]
        out << "," << PGE_URLENC(evt.autostart_condition);
    //    el=b/s1,s2...sn/h1,h2...hn/t1,t2...tn
    //        b=no smoke[0=false !0=true]
        out << "|" << fromNum((int)evt.nosmoke);
    //        [***urlencode!***]
        out << "/";
    //        s(n)=show layer
        for(int j=0; j<(signed)evt.layers_show.size();j++)
        {
            if(j>0) out << ","; out << PGE_URLENC(evt.layers_show[j]);
        }
        out << "/";
    //        l(n)=hide layer
        for(int j=0; j<(signed)evt.layers_hide.size();j++)
        {
            if(j>0) out << ","; out << PGE_URLENC(evt.layers_hide[j]);
        }
        out << "/";
    //        t(n)=toggle layer
        for(int j=0; j<(signed)evt.layers_toggle.size();j++)
        {
            if(j>0) out << ","; out << PGE_URLENC(evt.layers_toggle[j]);
        }

        out << "|";
    //    elm=elm1/elm2...elmn
        for(int j=0; j<(signed)evt.moving_layers.size(); j++)
        {
            if(j>0) out << "/";
    //        elm(n)=layername,horizontal syntax,vertical syntax,way
    //        layername=layer name for movement[***urlencode!***]
            LevelEvent_MoveLayer &mvl = evt.moving_layers[j];
            //Convert all floats into strings if expression fields are empty
            PGESTRING expression_x = mvl.expression_x;
            PGESTRING expression_y = mvl.expression_y;
            SMBX65_Num2Exp(mvl.speed_x, expression_x);
            SMBX65_Num2Exp(mvl.speed_y, expression_y);
            out << PGE_URLENC(mvl.name);
    //        horizontal syntax,vertical syntax[***urlencode!***][syntax]
            out << "," << PGE_URLENC(expression_x);
            out << "," << PGE_URLENC(expression_y);
    //        way=[0=by speed][1=by Coordinate]
            out << "," << fromNum(mvl.way);
        }

        out << "|";
    //    epy=b1,b2,b3,b4,b5,b6,b7,b8,b9,b10,b11,b12
    //        b1=enable player controls
        out << fromNum(evt.ctrls_enable);
    //        b2=drop
        out << "," << fromNum(evt.ctrl_drop);
    //        b3=alt run
        out << "," << fromNum(evt.ctrl_altrun);
    //        b4=run
        out << "," << fromNum(evt.ctrl_run);
    //        b5=jump
        out << "," << fromNum(evt.ctrl_jump);
    //        b6=alt jump
        out << "," << fromNum(evt.ctrl_altjump);
    //        b7=up
        out << "," << fromNum(evt.ctrl_up);
    //        b8=down
        out << "," << fromNum(evt.ctrl_down);
    //        b9=left
        out << "," << fromNum(evt.ctrl_left);
    //        b10=right
        out << "," << fromNum(evt.ctrl_right);
    //        b11=start
        out << "," << fromNum(evt.ctrl_start);
    //        b12=lock keyboard
        out << "," << fromNum(evt.ctrl_lock_keyboard);

        out << "|";
    //    eps=esection/ebackground/emusic
    //        esection=es1:es2...esn
    //        ebackground=eb1:eb2...ebn
    //        emusic=em1:em2...emn
        bool size_set_added=false;
        for(int j=0; j<(signed)evt.sets.size(); j++)
        {
            int section_pos = evt.sets[j].position_left;
            switch( section_pos )
            {
                case -1: section_pos=0; continue; break;
                case -2: section_pos=1;break;
                default: section_pos=2;break;
            }
            //Convert floats into expressions if there are empty
            PGESTRING expression_x=evt.sets[j].expression_pos_x;
            PGESTRING expression_y=evt.sets[j].expression_pos_y;
            PGESTRING expression_w=evt.sets[j].expression_pos_w;
            PGESTRING expression_h=evt.sets[j].expression_pos_h;
            PGESTRING expression_as_x=evt.sets[j].expression_autoscrool_x;
            PGESTRING expression_as_y=evt.sets[j].expression_autoscrool_y;
            if(section_pos>=2)
            {
                SMBX65_Num2Exp(evt.sets[j].position_left,   expression_x);
                SMBX65_Num2Exp(evt.sets[j].position_bottom, expression_y);
                SMBX65_Num2Exp(evt.sets[j].position_right - evt.sets[j].position_left, expression_w);
                SMBX65_Num2Exp(evt.sets[j].position_bottom - evt.sets[j].position_top, expression_h);
            }
            if(evt.sets[j].autoscrol)
            {
                SMBX65_Num2Exp(evt.sets[j].autoscrol_x, expression_as_x);
                SMBX65_Num2Exp(evt.sets[j].autoscrol_y, expression_as_y);
            }
    //            es=id,x,y,w,h,auto,sx,sy
            if(size_set_added) out << ":";
            size_set_added=true;
    //                id=section id
            out        << fromNum(evt.sets[j].id);
    //                stype=[0=don't change][1=default][2=custom]
            out << "," << fromNum( section_pos );
    //                x=left x coordinates for section [id][***urlencode!***][syntax]
            out << "," << PGE_URLENC( expression_x );
    //                y=top y coordinates for section [id][***urlencode!***][syntax]
            out << "," << PGE_URLENC( expression_y );
    //                w=width for section [id][***urlencode!***][syntax]
            out << "," << PGE_URLENC( expression_w );
    //                h=height for section [id][***urlencode!***][syntax]
            out << "," << PGE_URLENC( expression_h);
    //                auto=enable autoscroll controls[0=false !0=tru
            out << "," << fromNum( (int)evt.sets[j].autoscrol );
    //                sx=move screen horizontal syntax[***urlencode!***][syntax]
            out << "," << PGE_URLENC( expression_as_x );
    //                sy=move screen vertical syntax[***urlencode!***][syntax]
            out << "," << PGE_URLENC( expression_as_y );
        }
        out << "/";

        bool bg_set_added=false;
        for(int j=0; j<(signed)evt.sets.size(); j++)
        {
            int section_bg = evt.sets[j].background_id;
            switch( section_bg )
            {
                case -1: section_bg=0; continue; break;
                case -2: section_bg=1; break;
                default: section_bg=2; break;
            }
    //            eb=id,btype,backgroundid
            if(bg_set_added) out << ":";
            bg_set_added=true;
    //                id=section id
            out        << fromNum(evt.sets[j].id);
    //                btype=[0=don't change][1=default][2=custom]
            out << "," << fromNum(section_bg);
    //                backgroundid=[when btype=2]custom background id
            out << "," << fromNum( evt.sets[j].background_id >=0 ? evt.sets[j].background_id : 0 );
        }
        out << "/";

        bool muz_set_added=false;
        for(int j=0; j<(signed)evt.sets.size(); j++)
        {
            int section_muz = evt.sets[j].music_id;
            switch( section_muz )
            {
                case -1: section_muz=0; continue; break;
                case -2: section_muz=1;break;
                default: section_muz=2;break;
            }
    //            em=id,mtype,musicid,customfile
            if(muz_set_added) out << ":";
            muz_set_added=true;
    //                id=section id
            out       << fromNum(evt.sets[j].id);
    //                mtype=[0=don't change][1=default][2=custom]
            out << "," << fromNum(section_muz);
    //                musicid=[when mtype=2]custom music id
            out << "," << fromNum( evt.sets[j].music_id >= 0 ? evt.sets[j].music_id : 0 );
    //                customfile=[when mtype=3]custom music file name[***urlencode!***]
            out << "," << PGE_URLENC( evt.sets[j].music_file );
        }
        out << "|";
    //    eef=sound/endgame/ce1/ce2...cen
    //        sound=play sound number
        out << fromNum(evt.sound_id);
    //        endgame=[0=none][1=bowser defeat]
        out << "/" << fromNum(evt.end_game);
        for(int j=0; j<(signed)evt.spawn_effects.size(); j++)
        {
            LevelEvent_SpawnEffect &eff = evt.spawn_effects[j];
            //if(j<(evt.spawn_effects.size()-1))
            out << "/";
            //Convert floats into expressions if there are empty
            PGESTRING expression_x=eff.expression_x;
            PGESTRING expression_y=eff.expression_y;
            PGESTRING expression_sx=eff.expression_sx;
            PGESTRING expression_sy=eff.expression_sy;
            SMBX65_Num2Exp(eff.x, expression_x);
            SMBX65_Num2Exp(eff.y, expression_y);
            SMBX65_Num2Exp(eff.speed_x, expression_sx);
            SMBX65_Num2Exp(eff.speed_y, expression_sy);

    //        ce(n)=id,x,y,sx,sy,grv,fsp,life
    //            id=effect id
            out        << fromNum(eff.id);
    //            x=effect position x[***urlencode!***][syntax]
            out << "," << PGE_URLENC(expression_x);
    //            y=effect position y[***urlencode!***][syntax]
            out << "," << PGE_URLENC(expression_y);
    //            sx=effect horizontal speed[***urlencode!***][syntax]
            out << "," << PGE_URLENC(expression_sx);
    //            sy=effect vertical speed[***urlencode!***][syntax]
            out << "," << PGE_URLENC(expression_sy);
    //            grv=to decide whether the effects are affected by gravity[0=false !0=true]
            out << "," << fromNum((int)eff.gravity);
    //            fsp=frame speed of effect generated
            out << "," << fromNum(eff.fps);
    //            life=effect existed over this time will be destroyed.
            out << "," << fromNum(eff.max_life_time);
        }
        out << "|";

    //    ecn=cn1/cn2...cnn
        for(int j=0; j<(signed)evt.spawn_npc.size(); j++)
        {
            LevelEvent_SpawnNPC &snpc = evt.spawn_npc[j];

            //Convert floats into expressions if there are empty
            PGESTRING expression_x=snpc.expression_x;
            PGESTRING expression_y=snpc.expression_y;
            PGESTRING expression_sx=snpc.expression_sx;
            PGESTRING expression_sy=snpc.expression_sy;
            SMBX65_Num2Exp(snpc.x, expression_x);
            SMBX65_Num2Exp(snpc.y, expression_y);
            SMBX65_Num2Exp(snpc.speed_x, expression_sx);
            SMBX65_Num2Exp(snpc.speed_y, expression_sy);

    //        cn(n)=id,x,y,sx,sy,sp
            if(j>0)
                out << "/";
    //            id=npc id
            out        << fromNum(snpc.id);
    //            x=npc position x[***urlencode!***][syntax]
            out << "," << PGE_URLENC(expression_x);
    //            y=npc position y[***urlencode!***][syntax]
            out << "," << PGE_URLENC(expression_y);
    //            sx=npc horizontal speed[***urlencode!***][syntax]
            out << "," << PGE_URLENC(expression_sx);
    //            sy=npc vertical speed[***urlencode!***][syntax]
            out << "," << PGE_URLENC(expression_sy);
    //            sp=advanced settings of generated npc
            out << "," << fromNum(snpc.special);
        }
        out << "|";
    //    evc=vc1/vc2...vcn
        for(int j=0; j<(signed)evt.update_variable.size(); j++)
        {
            LevelEvent_UpdateVariable &uvar= evt.update_variable[j];
            if(j>0)
            out << "/";
    //        vc(n)=name,newvalue
            out        << PGE_URLENC( uvar.name );
    //            name=variable name[***urlencode!***]
            out << "," << PGE_URLENC( uvar.newval );
    //            newvalue=new value[***urlencode!***][syntax]
        }

        out << "|";
    //    ene=nextevent/timer/apievent/scriptname
    //        nextevent=name,delay
    //            name=trigger event name[***urlencode!***]
        out        << PGE_URLENC(evt.trigger);
    //            delay=trigger delay[1 frame]
        out << "," << fromNum( (int)round(((double)evt.trigger_timer * 65.0) / 10.0) );
    //        timer=enable,count,interval,type,show
    //            enable=enable the game timer controlling[0=false !0=true]
        out << "/" << fromNum( (int)evt.timer_def.enable );
    //            count=set the time left of the game timer
        out << "," << fromNum( evt.timer_def.count );
    //            interval=set the time count interval of the game timer
        out << "," << fromNum( (int)round(SMBX64::ms_to_65(evt.timer_def.interval)) );
    //            type=to choose the way timer counts[0=counting down][1=counting up]
        out << "," << fromNum( evt.timer_def.count_dir );
    //            show=to choose whether the game timer is showed in hud[0=false !0=true]
        out << "," << fromNum( evt.timer_def.show );
    //        apievent=the id of apievent
        out << "/" << fromNum( evt.trigger_api_id );
    //        scriptname=script name[***urlencode!***]
        out << "/" << PGE_URLENC( evt.trigger_script );
        out << "\n";
    }

    //next line: variables
    for(i=0; i<(signed)FileData.variables.size(); i++)
    {
        LevelVariable &var = FileData.variables[i];
    //    V|name|value
        out << "V";
    //    name=variable name[***urlencode!***]
        out << "|" << PGE_URLENC(var.name);
    //    value=initial value of the variable
        if(!SMBX64::IsSInt(var.value))//if is not signed integer, set value as zero
            out << "|" << fromNum(0);
        else
            out << "|" << var.value;
        out << "\n";
    }

    //next line: scripts
    for(i=0; i<(signed)FileData.scripts.size(); i++)
    {
        LevelScript &script= FileData.scripts[i];
    //    S|name|script
        out << "S";
    //    Su|name|scriptu
    //    name=name of script[***urlencode!***]
        out << "|" << PGE_URLENC(script.name);
    //    script=script[***base64encode!***][utf-8]

        PGESTRING scriptT = script.script;
        if(scriptT.size()>0 && (PGEGetChar(scriptT[scriptT.size()-1])!='\n'))
            scriptT.append("\n");

        //Convert into CRLF
        PGE_ReplSTRING(scriptT, "\n", "\r\n");

        out << "|" << PGE_BASE64ENC( scriptT );
    //    scriptu=script[***base64encode!***][ASCII]
        out << "\n";
    }

    return true;
}



