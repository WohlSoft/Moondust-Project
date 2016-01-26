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

//for Header readers.
//Use it if you want read file partially
//(you must create QTextStream in(&fstream); !!!)
#define SMBX65_FileBegin() int file_format=0;   /*File format number*/\
                           PGESTRING line;      /*Current Line data*/

//Jump to next line
#define nextLine() line = in.readCVSLine();


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
    SMBX64_FileBegin();
    PGESTRINGList currentLine;

    #define nextLineH() line = inf.readLine();

    nextLineH();   //Read first line
    if( !PGE_StartsWith(line, "SMBXFile") ) //File format number
        goto badfile;
    else file_format = 65;
readLineAgain:
    nextLineH();   //Read second Line
    PGE_SPLITSTR(currentLine, line, "|");

    if(currentLine.size()==0)
    {
        if(!inf.eof())
            goto readLineAgain;
    } else {
        if(currentLine[0]=="A")
        {
            for(int i=1;i<(signed)currentLine.size();i++)
            {
                PGESTRING &cLine=currentLine[i];
                switch(i)
                {
                case 1://Number of stars
                    {
                        if( SMBX64::uInt(cLine) )
                            goto badfile;
                        else FileData.stars=toInt(cLine);   //Number of stars
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
                        if( SMBX64::uInt(cLine) )
                            goto badfile;
                        else FileData.open_level_on_fail_warpID = toInt(cLine);
                    } break;
                }

            }
        } else goto readLineAgain;
    }

    FileData.CurSection=0;
    FileData.playmusic=0;

    FileData.ReadFileValid=true;

    return FileData;
badfile:
    FileData.ReadFileValid=false;
    FileData.ERROR_info="Invalid file format, detected file SMBX-"+fromNum(file_format)+"format";
    FileData.ERROR_linenum = inf.getCurrentLineNumber();
    FileData.ERROR_linedata=line;
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

void SMBX65_SplitLine(PGESTRINGList &dst, PGESTRING &Src)
{
    dst.clear();
    for(int i=0; i<(signed)Src.size(); )
    {
        PGESTRING Buffer="";
        PGEChar cur=' ';
        do
        {
            cur=Src[i++];
            if(cur!='|') Buffer.push_back(cur);
        } while( (i<(signed)Src.size()) && (cur!='|') );
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

static const int SMBX65_NpcGeneratorTypes[29] =
  //0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28
  { 0,0,0,0,0,1,1,1,1,0,0, 0, 0, 1, 1, 1, 1, 4, 4, 4, 4, 0, 0, 0, 0, 4, 4, 4, 4};

static const int SMBX65_NpcGeneratorDirections[29] =
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

    in.seek(0, PGE_FileFormats_misc::TextFileInput::begin);
    PGESTRINGList currentLine;

    nextLine();   //Read first line
    if( !PGE_StartsWith(line, "SMBXFile") ) //File format number
        goto badfile;
    else file_format = 65;
readLineAgain:
    nextLine();   //Read second Line
    SMBX65_SplitLine(currentLine, line);

    if(currentLine.size()==0)
    {
        if(!in.eof())
            goto readLineAgain;
    } else {

        if(currentLine[0]=="A")//Level settings
        {
            for(int i=1;i<(signed)currentLine.size();i++)
            {
                PGESTRING &cLine=currentLine[i];
                switch(i)
                {
                case 1://Number of stars
                    {
                        if( SMBX64::uInt(cLine) )
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
                        if( SMBX64::uInt(cLine) )
                            goto badfile;
                        else FileData.open_level_on_fail_warpID = toInt(cLine);
                    } break;
                }
            }
        }


        else
        if(currentLine[0]=="P1")//Player 1 point
        {
            players = CreateLvlPlayerPoint(1);
            for(int i=1;i<(signed)currentLine.size();i++)
            {
                PGESTRING &cLine=currentLine[i];
                switch(i)
                {
                case 1://Pos X
                    {
                        if( SMBX64::sFloat(cLine) )
                            goto badfile;
                        else players.x=(int)toFloat(cLine);
                    } break;
                case 2://Pos Y
                    {
                        if( SMBX64::sFloat(cLine) )
                            goto badfile;
                        else players.y=(int)toFloat(cLine);
                    } break;
                }
            }
            FileData.players.push_back(players);
        }


        else
        if(currentLine[0]=="P2")//Player 2 point
        {
            players = CreateLvlPlayerPoint(2);
            for(int i=1;i<(signed)currentLine.size();i++)
            {
                PGESTRING &cLine=currentLine[i];
                switch(i)
                {
                case 1://Pos X
                    {
                        if( SMBX64::sFloat(cLine) )
                            goto badfile;
                        else players.x=(int)toFloat(cLine);
                    } break;
                case 2://Pos Y
                    {
                        if( SMBX64::sFloat(cLine) )
                            goto badfile;
                        else players.y=(int)toFloat(cLine);
                    } break;
                }
            }
            FileData.players.push_back(players);
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
                        if( SMBX64::uInt(cLine) )
                            goto badfile;
                        else section.id=toInt(cLine);
                    } break;
                //"x=Left size[-left/+right]
                case 2:
                    {
                        if( SMBX64::sFloat(cLine) )
                            goto badfile;
                        else x=toFloat(cLine);
                    } break;
                //"y=Top size[-down/+up]
                case 3:
                    {
                        if( SMBX64::sFloat(cLine) )
                            goto badfile;
                        else y=toFloat(cLine);
                    } break;
                //"w=width of the section[if (w < 800) w = 800]
                case 4:
                    {
                        if( SMBX64::sFloat(cLine) )
                            goto badfile;
                        else w=toFloat(cLine);
                    } break;
                //"h=height of the section[if (h < 600) h = 600]
                case 5:
                    {
                        if( SMBX64::sFloat(cLine) )
                            goto badfile;
                        else h=toFloat(cLine);
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
                        if( SMBX64::sFloat(cLine) )
                            goto badfile;
                        else section.music_id=(unsigned int)toFloat(cLine);
                    } break;
                //"background=background number[same as the filename in 'background2' folder]
                case 13:
                    {
                        if( SMBX64::sFloat(cLine) )
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


            section.size_left=(long)round(x);
            section.size_top=(long)round(y);
            section.size_right=(long)round(x+w);
            section.size_bottom=(long)round(y+h);

            if((unsigned)section.id <= FileData.sections.size())
                FileData.sections[section.id-1]=section;//Replace if already exists
            else
                FileData.sections.push_back(section); //Add Section in main array
        }

        else
        if(currentLine[0]=="B")//Blocks
        {
            //B|layer|id|x|y|contain|b1|b2|e1,e2,e3|w|h
            blocks = CreateLvlBlock();
            for(int i=1;i<(signed)currentLine.size();i++)
            {
                PGESTRING &cLine=currentLine[i];
                switch(i)
                {
                //    layer=layer name["" == "Default"][***urlencode!***]
                case 1:
                    {
                        blocks.layer=(cLine==""?"Default":PGE_URLDEC(cLine));
                    } break;
                //    id=block id
                case 2:
                    {
                        if( SMBX64::uInt(cLine) )
                            goto badfile;
                        else blocks.id=toInt(cLine);
                    } break;
                //    x=block position x
                case 3:
                    {
                        if( SMBX64::sFloat(cLine) )
                            goto badfile;
                        else blocks.x=(long)round(toFloat(cLine));
                    } break;
                //    y=block position y
                case 4:
                    {
                        if( SMBX64::sFloat(cLine) )
                            goto badfile;
                        else blocks.y=(long)round(toFloat(cLine));
                    } break;
                //    contain=containing npc number
                //        [1001-1000+NPCMAX] npc-id
                //        [1-999] coin number
                //        [0] nothing
                case 5:
                    {
                        if( SMBX64::uInt(cLine) )
                            goto badfile;
                        else
                        {
                            long npcid=toInt(cLine);
                            blocks.npc_id=((npcid<1000) ? -1*npcid : npcid-1000 );
                        }
                    } break;
                //    b1=slippery[0=false !0=true]
                case 6:
                    {
                        blocks.slippery=(cLine!="0");
                    } break;
                //    b2=invisible[0=false !0=true]
                case 7:
                    {
                        blocks.invisible=(cLine!="0");
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
                                case 0: blocks.event_destroy=PGE_URLDEC(dLine); break;
                            //    e2=block hit event name[***urlencode!***]
                                case 1: blocks.event_hit=PGE_URLDEC(dLine); break;
                            //    e3=no more object in layer event name[***urlencode!***]4
                                case 2: blocks.event_emptylayer=PGE_URLDEC(dLine); break;
                            }
                        }
                    } break;
                //    w=width
                case 9:
                    {
                        if( SMBX64::sFloat(cLine) )
                            goto badfile;
                        else blocks.w=(long)round(toFloat(cLine));
                    } break;
                //    h=height
                case 10:
                    {
                        if( SMBX64::sFloat(cLine) )
                            goto badfile;
                        else blocks.h=(long)round(toFloat(cLine));
                    } break;
                }
            }
            FileData.blocks.push_back(blocks);
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
                        if( SMBX64::uInt(cLine) )
                            goto badfile;
                        else bgodata.id=toInt(cLine);
                    } break;
                //    x=background position x
                case 3:
                    {
                        if( SMBX64::sFloat(cLine) )
                            goto badfile;
                        else bgodata.x=(long)round(toFloat(cLine));
                    } break;
                //    y=background position y
                case 4:
                    {
                        if( SMBX64::sFloat(cLine) )
                            goto badfile;
                        else bgodata.y=(long)round(toFloat(cLine));
                    } break;
                }
            }
            FileData.bgo.push_back(bgodata);
        }

        else
        if(currentLine[0]=="N")//NPC
        {
            //T|layer|id|x|y
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
                        if( SMBX64::uInt(cLine) )
                            goto badfile;
                        else npcdata.id=toInt(cLine);
                    } break;
                //x=npc position x
                case 3:
                    {
                        if( SMBX64::sFloat(cLine) )
                            goto badfile;
                        else npcdata.x=(long)round(toFloat(cLine));
                    } break;
                //y=npc position y
                case 4:
                    {
                        if( SMBX64::sFloat(cLine) )
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
                                //b1=[-1]left [0]random [1]right
                                    case 0:
                                        if( SMBX64::sInt(dLine) )
                                        goto badfile;
                                        else npcdata.direct=toInt(dLine);
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
                                        //this field is useless here, ignore it
                                        //on file save it can be quickly re-calculated
                                        break;
                                }
                            }
                    } break;

                //sp=special option
                case 6:
                    {
                        if( SMBX64::sFloat(cLine) )
                            goto badfile;
                        else
                        {
                            npcdata.special_data = (int)round(toFloat(cLine));
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
                            //    a1=layer name to attach
                                case 7: npcdata.attach_layer=PGE_URLDEC(dLine); break;
                            //    a2=variable name to send
                                case 8: npcdata.send_it_to_variable=PGE_URLDEC(dLine); break;
                            }
                        }
                    } break;

                case 8:
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
                                    case 0: npcdata.generator = ((cLine!="")&&(cLine!="0")); break;
                                //[if c1!=0]
                                //    c2=generator period[1 frame]
                                    case 1:
                                        if( SMBX64::sInt(dLine) )
                                        goto badfile;
                                        else npcdata.generator_period = (toFloat(dLine)/65.0)*100;
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
                                        if( SMBX64::sInt(dLine) )
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
                                            if( SMBX64::sFloat(cLine) )
                                                goto badfile;
                                            else npcdata.generator_custom_angle = toFloat(cLine);
                                        } break;
                                //    c5=batch[when c3=0][MAX=32]
                                    case 4:
                                        {
                                            if( SMBX64::sFloat(cLine) )
                                                goto badfile;
                                            else npcdata.generator_branches = (long)fabs(round(toFloat(cLine)));
                                        } break;
                                //    c6=angle range[when c3=0]
                                    case 5:
                                        {
                                            if( SMBX64::sFloat(cLine) )
                                                goto badfile;
                                            else npcdata.generator_angle_range = fabs(toFloat(cLine));
                                        } break;
                                //    c7=speed[when c3=0][float]
                                    case 6:
                                        {
                                            if( SMBX64::sFloat(cLine) )
                                                goto badfile;
                                            else npcdata.generator_initial_speed = toFloat(cLine);
                                        } break;
                                }
                            }
                    } break;
                //msg=message by this npc talkative[***urlencode!***]
                case 9:
                    {
                        npcdata.msg=PGE_URLDEC(cLine);
                    } break;
                }
            }
            FileData.npc.push_back(npcdata);
        }

        else
        if(currentLine[0]=="T")//next line: waters
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
                        if( SMBX64::sFloat(cLine) )
                            goto badfile;
                        else waters.x=(long)round(toFloat(cLine));
                    } break;
                //y=position y
                case 3:
                    {
                        if( SMBX64::sFloat(cLine) )
                            goto badfile;
                        else waters.y=(long)round(toFloat(cLine));
                    } break;
                //w=width
                case 4:
                    {
                        if( SMBX64::sFloat(cLine) )
                            goto badfile;
                        else waters.w=(long)round(toFloat(cLine));
                    } break;
                //h=height
                case 5:
                    {
                        if( SMBX64::sFloat(cLine) )
                            goto badfile;
                        else waters.h=(long)round(toFloat(cLine));
                    } break;
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
                //b2=friction
                //b3=Acceleration Direction
                //b4=Acceleration
                //b5=Maximum Velocity
                //event=touch event
                }
            }
            FileData.physez.push_back(waters);
        }


        //next line: warps
        //W|layer|x|y|ex|ey|type|enterd|exitd|sn,msg,hide|locked,noyoshi,canpick,bomb,hidef,anpc|lik|liid|noexit|wx|wy|le|we
        //layer=layer name["" == "Default"][***urlencode!***]
        //x=entrance position x
        //y=entrance postion y
        //ex=exit position x
        //ey=exit position y
        //type=[1=pipe][2=door][0=instant]
        //enterd=entrance direction[1=up 2=left 3=down 4=right]
        //exitd=exit direction[1=up 2=left 3=down 4=right]
        //sn=need stars for enter
        //msg=a message when you have not enough stars
        //hide=hide the star number in this warp
        //locked=locked
        //noyoshi=no yoshi
        //canpick=allow npc
        //bomb=need a bomb
        //hide=hide the entry scene
        //anpc=allow npc interlevel
        //lik=warp to level[***urlencode!***]
        //liid=normal enterance / to warp[0-WARPMAX]
        //noexit=level entrance
        //wx=warp to x on world map
        //wy=warp to y on world map
        //le=level exit
        //we=warp event[***urlencode!***]

        else
        if(currentLine[0]=="L")//Layers
        {
            //L|name|status
            layers = CreateLvlLayer();
            for(int i=1;i<(signed)currentLine.size();i++)
            {
                PGESTRING &cLine=currentLine[i];
                switch(i)
                {
                //    layer=layer name["" == "Default"][***urlencode!***]
                case 1:
                    {
                        layers.name=PGE_URLDEC(cLine);
                    } break;
                //    status=is hidden layer
                case 2:
                    {
                        layers.hidden = (cLine!="0");
                    } break;
                }
            }
            FileData.layers.push_back(layers);
        }

        //next line: events
        //E|name|msg|ea|el|elm|epy|eps|eef|ecn|evc|ene
        //name=event name[***urlencode!***]
        //msg=show message after start event
        //ea=val,syntax
        //    val=[0=not auto start][1=auto start when level start][2=auto start when match all condition][3=start when called and match all condidtion]
        //    syntax=condidtion expression[***urlencode!***]
        //el=b/s1,s2...sn/h1,h2...hn/t1,t2...tn
        //    b=no smoke[0=false !0=true]
        //    [***urlencode!***]
        //    s(n)=show layer
        //    l(n)=hide layer
        //    t(n)=toggle layer
        //elm=elm1/elm2...elmn
        //    elm(n)=layername,horizontal syntax,vertical syntax,way
        //    layername=layer name for movement[***urlencode!***]
        //    horizontal syntax,vertical syntax[***urlencode!***][syntax]
        //    way=[0=by speed][1=by Coordinate]
        //epy=b1,b2,b3,b4,b5,b6,b7,b8,b9,b10,b11,b12
        //    b1=enable player controls
        //    b2=drop
        //    b3=alt run
        //    b4=run
        //    b5=jump
        //    b6=alt jump
        //    b7=up
        //    b8=down
        //    b9=left
        //    b10=right
        //    b11=start
        //    b12=lock keyboard
        //eps=esection/ebackground/emusic
        //    esection=es1:es2...esn
        //    ebackground=eb1:eb2...ebn
        //    emusic=em1:em2...emn
        //        es=id,x,y,w,h,auto,sx,sy
        //            id=section id
        //            x=left x coordinates for section [id][***urlencode!***][syntax]
        //            y=top y coordinates for section [id][***urlencode!***][syntax]
        //            w=width for section [id][***urlencode!***][syntax]
        //            h=height for section [id][***urlencode!***][syntax]
        //            auto=enable autoscroll controls[0=false !0=true]
        //            sx=move screen horizontal syntax[***urlencode!***][syntax]
        //            sy=move screen vertical syntax[***urlencode!***][syntax]
        //        eb=id,btype,backgroundid
        //            id=section id
        //            btype=[0=don't change][1=default][2=custom]
        //            backgroundid=[when btype=2]custom background id
        //        em=id,mtype,musicid,customfile
        //            id=section id
        //            mtype=[0=don't change][1=default][2=custom]
        //            musicid=[when mtype=2]custom music id
        //            customfile=[when mtype=3]custom music file name[***urlencode!***]
        //eef=sound/endgame/ce1/ce2...cen
        //    sound=play sound number
        //    endgame=[0=none][1=bowser defeat]
        //    ce(n)=id,x,y,sx,sy,grv,fsp,life
        //        id=effect id
        //        x=effect position x[***urlencode!***][syntax]
        //        y=effect position y[***urlencode!***][syntax]
        //        sx=effect horizontal speed[***urlencode!***][syntax]
        //        sy=effect vertical speed[***urlencode!***][syntax]
        //        grv=to decide whether the effects are affected by gravity[0=false !0=true]
        //        fsp=frame speed of effect generated
        //        life=effect existed over this time will be destroyed.
        //ecn=cn1/cn2...cnn
        //    cn(n)=id,x,y,sx,sy,sp
        //        id=npc id
        //        x=npc position x[***urlencode!***][syntax]
        //        y=npc position y[***urlencode!***][syntax]
        //        sx=npc horizontal speed[***urlencode!***][syntax]
        //        sy=npc vertical speed[***urlencode!***][syntax]
        //        sp=advanced settings of generated npc
        //evc=vc1/vc2...vcn
        //    vc(n)=name,newvalue
        //        name=variable name[***urlencode!***]
        //        newvalue=new value[***urlencode!***][syntax]
        //ene=nextevent/timer/apievent/scriptname
        //    nextevent=name,delay
        //        name=trigger event name[***urlencode!***]
        //        delay=trigger delay[1 frame]
        //    timer=enable,count,interval,type,show
        //        enable=enable the game timer controlling[0=false !0=true]
        //        count=set the time left of the game timer
        //        interval=set the time count interval of the game timer
        //        type=to choose the way timer counts[0=counting down][1=counting up]
        //        show=to choose whether the game timer is showed in hud[0=false !0=true]
        //    apievent=the id of apievent
        //    scriptname=script name[***urlencode!***]

        LevelAddInternalEvents(FileData);

        //next line: variables
        //V|name|value
        //name=variable name[***urlencode!***]
        //value=initial value of the variable


        //next line: scripts
        //S|name|script
        //Su|name|scriptu
        //name=name of script[***urlencode!***]
        //script=script[***base64encode!***][utf-8]
        //scriptu=script[***base64encode!***][unicode]

        goto readLineAgain;
    }

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
