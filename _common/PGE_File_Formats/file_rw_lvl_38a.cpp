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
#ifdef nextLine
#undef nextLine
#endif
#define nextLine() line = in.readLine();

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
    PlayerPoint playerdata;
    LevelBlock blockdata;
    LevelBGO bgodata;
    LevelNPC npcdata;
    LevelDoor doordata;
    LevelPhysEnv waters;
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
            playerdata = CreateLvlPlayerPoint(1);
            for(int i=1;i<(signed)currentLine.size();i++)
            {
                PGESTRING &cLine=currentLine[i];
                switch(i)
                {
                case 1://Pos X
                    {
                        if( SMBX64::sFloat(cLine) )
                            goto badfile;
                        else playerdata.x=(int)toFloat(cLine);
                    } break;
                case 2://Pos Y
                    {
                        if( SMBX64::sFloat(cLine) )
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
                        if( SMBX64::sFloat(cLine) )
                            goto badfile;
                        else playerdata.x=(int)toFloat(cLine);
                    } break;
                case 2://Pos Y
                    {
                        if( SMBX64::sFloat(cLine) )
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
                        if( SMBX64::uInt(cLine) )
                            goto badfile;
                        else section.id=(toInt(cLine)-1);
                        if(section.id<0) section.id = 0;
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
                        else w = toFloat(cLine);
                        if(w<800.0) w=800.0;
                    } break;
                //"h=height of the section[if (h < 600) h = 600]
                case 5:
                    {
                        if( SMBX64::sFloat(cLine) )
                            goto badfile;
                        else h = toFloat(cLine);
                        if(h<600.0) h=600.0;
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
                        if( SMBX64::uInt(cLine) )
                            goto badfile;
                        else blockdata.id = toInt(cLine);
                    } break;
                //    x=block position x
                case 3:
                    {
                        if( SMBX64::sFloat(cLine) )
                            goto badfile;
                        else blockdata.x=(long)round(toFloat(cLine));
                    } break;
                //    y=block position y
                case 4:
                    {
                        if( SMBX64::sFloat(cLine) )
                            goto badfile;
                        else blockdata.y=(long)round(toFloat(cLine));
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
                        if( SMBX64::sFloat(cLine) )
                            goto badfile;
                        else blockdata.w=(long)round(toFloat(cLine));
                    } break;
                //    h=height
                case 10:
                    {
                        if( SMBX64::sFloat(cLine) )
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
                                //b1=[1]left [0]random [-1]right
                                    case 0:
                                        if( SMBX64::sInt(dLine) )
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
                                        if( SMBX64::sInt(dLine) )
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
                                            if( SMBX64::sFloat(dLine) )
                                                goto badfile;
                                            else npcdata.generator_custom_angle = toFloat(dLine);
                                        } break;
                                //    c5=batch[when c3=0][MAX=32]
                                    case 4:
                                        {
                                            if( SMBX64::sFloat(dLine) )
                                                goto badfile;
                                            else npcdata.generator_branches = (long)fabs(round(toFloat(dLine)));
                                        } break;
                                //    c6=angle range[when c3=0]
                                    case 5:
                                        {
                                            if( SMBX64::sFloat(dLine) )
                                                goto badfile;
                                            else npcdata.generator_angle_range = fabs(toFloat(dLine));
                                        } break;
                                //    c7=speed[when c3=0][float]
                                    case 6:
                                        {
                                            if( SMBX64::sFloat(dLine) )
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
                                            if( SMBX64::sFloat(dLine) )
                                                goto badfile;
                                            else waters.env_type = (int)round(toFloat(dLine))-1;
                                        } break;
                                    //b2=friction
                                    case 1:
                                        {
                                            if( SMBX64::sFloat(dLine) )
                                                goto badfile;
                                            else waters.friction = toFloat(dLine);
                                        } break;
                                    //b3=Acceleration Direction
                                    case 2:
                                        {
                                            if( SMBX64::sFloat(dLine) )
                                                goto badfile;
                                            else waters.accel_direct = toFloat(dLine);
                                        } break;
                                    //b4=Acceleration
                                    case 3:
                                        {
                                            if( SMBX64::sFloat(dLine) )
                                                goto badfile;
                                            else waters.accel = toFloat(dLine);
                                        } break;
                                    //b5=Maximum Velocity
                                    case 4:
                                        {
                                            if( SMBX64::sFloat(dLine) )
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
                        if( SMBX64::sFloat(cLine) )
                            goto badfile;
                        else doordata.ix=(long)round(toFloat(cLine));
                    } break;
                //y=entrance postion y
                case 3:
                    {
                        if( SMBX64::sFloat(cLine) )
                            goto badfile;
                        else doordata.iy=(long)round(toFloat(cLine));
                    } break;
                //ex=exit position x
                case 4:
                    {
                        if( SMBX64::sFloat(cLine) )
                            goto badfile;
                        else doordata.ox=(long)round(toFloat(cLine));
                    } break;
                //ey=exit position y
                case 5:
                    {
                        if( SMBX64::sFloat(cLine) )
                            goto badfile;
                        else doordata.oy=(long)round(toFloat(cLine));
                    } break;
                //type=[1=pipe][2=door][0=instant]
                case 6:
                    {
                        if( SMBX64::uInt(cLine) )
                            goto badfile;
                        else doordata.type = toInt(cLine);
                    } break;
                //enterd=entrance direction[1=up 2=left 3=down 4=right]
                case 7:
                    {
                        if( SMBX64::uInt(cLine) )
                            goto badfile;
                        else doordata.idirect = toInt(cLine);
                    } break;
                //exitd=exit direction[1=up 2=left 3=down 4=right]
                case 8:
                    {
                        if( SMBX64::uInt(cLine) )
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
                                    if( SMBX64::sInt(dLine) )
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
                                    if( SMBX64::uInt(dLine) )
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
                                    if( SMBX64::uInt(dLine) )
                                        goto badfile;
                                    else doordata.locked = (bool)toInt(dLine);
                                } break;
                            //noyoshi=no yoshi
                            case 1:
                                {
                                    if( SMBX64::uInt(dLine) )
                                        goto badfile;
                                    else doordata.novehicles = (bool)toInt(dLine);
                                } break;
                            //canpick=allow npc
                            case 2:
                                {
                                    if( SMBX64::uInt(dLine) )
                                        goto badfile;
                                    else doordata.allownpc = (bool)toInt(dLine);
                                } break;
                            //bomb=need a bomb
                            case 3:
                                {
                                    if( SMBX64::uInt(dLine) )
                                        goto badfile;
                                    else doordata.need_a_bomb = (bool)toInt(dLine);
                                } break;
                            //hide=hide the entry scene
                            case 4:
                                {
                                    if( SMBX64::uInt(dLine) )
                                        goto badfile;
                                    else doordata.hide_entering_scene = (bool)toInt(dLine);
                                } break;
                            //anpc=allow npc interlevel
                            case 5:
                                {
                                    if( SMBX64::uInt(dLine) )
                                        goto badfile;
                                    else doordata.allownpc_interlevel = (bool)toInt(dLine);
                                } break;
                            }
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
                        if( SMBX64::uInt(cLine) )
                            goto badfile;
                        else doordata.warpto = toInt(cLine);
                    } break;
                //noexit=level entrance
                case 13:
                    {
                        if( SMBX64::uInt(cLine) )
                            goto badfile;
                        else doordata.lvl_i = (bool)toInt(cLine);
                        doordata.isSetIn = ((doordata.lvl_i)?false:true);
                    } break;
                //wx=warp to x on world map
                case 14:
                    {
                        if( SMBX64::sFloat(cLine) )
                            goto badfile;
                        else doordata.world_x = (long)round(toFloat(cLine));
                    } break;
                //wy=warp to y on world map
                case 15:
                    {
                        if( SMBX64::sFloat(cLine) )
                            goto badfile;
                        else doordata.world_y = (long)round(toFloat(cLine));
                    } break;
                //le=level exit
                case 16:
                    {
                        if( SMBX64::uInt(cLine) )
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
                //    status=is hidden layer
                case 2:
                    {
                        layerdata.hidden = (cLine=="0");
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
                                        if( SMBX64::sFloat(dLine) )
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
                    //    horizontal syntax,vertical syntax[***urlencode!***][syntax]
                    //    way=[0=by speed][1=by Coordinate]
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
                    } break;
                //eef=sound/endgame/ce1/ce2...cen
                case 8:
                    {
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
                    } break;
                //ecn=cn1/cn2...cnn
                case 9:
                    {
                    //    cn(n)=id,x,y,sx,sy,sp
                    //        id=npc id
                    //        x=npc position x[***urlencode!***][syntax]
                    //        y=npc position y[***urlencode!***][syntax]
                    //        sx=npc horizontal speed[***urlencode!***][syntax]
                    //        sy=npc vertical speed[***urlencode!***][syntax]
                    //        sp=advanced settings of generated npc
                    } break;
                //evc=vc1/vc2...vcn
                case 10:
                    {
                    //    vc(n)=name,newvalue
                    //        name=variable name[***urlencode!***]
                    //        newvalue=new value[***urlencode!***][syntax]
                    } break;
                //ene=nextevent/timer/apievent/scriptname
                case 11:
                    {
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
                        if( SMBX64::uInt(cLine) )
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
        if(currentLine[0]=="Su") //next line: scripts (saved as unicode)
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
                //scriptu=script[***base64encode!***][unicode]
                case 2:
                    {
                        //scriptdata.scrupt=PGE_BASE64DEC_W(cLine);//for STD-based version need to convert string into UTF8!
                        scriptdata.script=PGE_BASE64DEC_A(cLine);
                    } break;
                }
            }
            FileData.scripts.push_back(scriptdata);
        }

        goto readLineAgain;
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
