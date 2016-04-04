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
#include "file_strlist.h"
#include "smbx64.h"
#ifdef PGE_EDITOR
#include <QMessageBox>
#include <QApplication>
#include <common_features/mainwinconnect.h>
#endif

#ifdef PGE_FILES_QT
#include <QFile>
#include <QFileInfo>
#endif

#include <functional>

//*********************************************************
//****************READ FILE FORMAT*************************
//*********************************************************
bool FileFormats::ReadNpcTXTFileF(PGESTRING filePath, NPCConfigFile &FileData, bool IgnoreBad)
{
    errorString.clear();
    PGE_FileFormats_misc::TextFileInput file;
    if(!file.open(filePath, IgnoreBad))
    {
        errorString="Failed to open file for read";
        return false;
    }
    return ReadNpcTXTFile(file, FileData, IgnoreBad);
}

bool FileFormats::ReadNpcTXTFileRAW(PGESTRING &rawdata, NPCConfigFile &FileData, bool IgnoreBad)
{
    errorString.clear();
    PGE_FileFormats_misc::RawTextInput file;
    if(!file.open(&rawdata))
    {
        errorString="Failed to open raw string for read";
        return false;
    }
    return ReadNpcTXTFile(file, FileData, IgnoreBad);
}

bool FileFormats::ReadNpcTXTFile(PGE_FileFormats_misc::TextInput &inf, NPCConfigFile &FileData, bool IgnoreBad)
{
    //PGESTRING errorString;
    //int str_count=0;        //Line Counter
    //int i;                  //counters
    PGESTRING line;           //Current Line data
    PGESTRINGList Params;
    PGESTRING unknownLines;
    /*NPCConfigFile */FileData = CreateEmpytNpcTXT();

//    if(!PGE_FileFormats_misc::TextFileInput::exists(file))
//    {
//        FileData.ReadFileValid=false;
//        errorString="File not exists: "+file;
//        #ifdef PGE_EDITOR
//        QMessageBox::critical(MainWinConnect::pMainWin, QObject::tr("File open error"),
//        QObject::tr("File is not exist"), QMessageBox::Ok);
//        #endif
//        return FileData;
//    }
//    if(!inf.open(file))
//    {
//        FileData.ReadFileValid=false;
//        errorString="Can't open file to read: "+file;
//        #ifdef PGE_EDITOR
//        QMessageBox::critical(MainWinConnect::pMainWin, QObject::tr("File open error"),
//        QObject::tr("Can't read the file"), QMessageBox::Ok);
//        #endif
//        return FileData;
//    }

    //Read NPC.TXT File config
    #define NextLine(line) line = inf.readCVSLine();

    do
    {
       NextLine(line)
       PGESTRING ln = line;
       if(PGE_RemSubSTRING(line, " ")=="")
       {
           NextLine(line)
           continue;
       } //Skip empty strings

       line=ln;

       #ifdef PGE_FILES_QT
       Params=line.split("=", QString::SkipEmptyParts); // split the Parameter and value (example: chicken=2)
       #else
       PGE_SPLITSTRING(Params, line, "=");
       #endif

       if(Params.size() != 2) // If string does not contain strings with "=" as separator
       {
           if(!IgnoreBad)
           {
               unknownLines += fromNum(inf.getCurrentLineNumber())+": "+line+" <wrong syntax!>\n";
               NextLine(line)
               continue;
           }
           if(Params.size()<2)
           {
               NextLine(line);
               continue;
           }
       }

       Params[0] = PGESTR_Simpl(Params[0]);
       Params[0]=PGE_RemSubSTRING(Params[0], " "); //Delete spaces

       if(Params[0]=="gfxoffsetx")
        {
           Params[1] = PGESTR_Simpl(Params[1]);
           Params[1]=PGE_RemSubSTRING(Params[1], " ");//Delete spaces
           if(!SMBX64::IsSInt(Params[1]))
           {
               unknownLines += fromNum(inf.getCurrentLineNumber())+": "+line+" <Should be signed intger!>\n";
           }
           else
           {
               FileData.gfxoffsetx=toInt(Params[1]);
               FileData.en_gfxoffsetx=true;
           }
        }
       else
       if(Params[0]=="gfxoffsety")
        {
           Params[1] = PGESTR_Simpl(Params[1]);
           Params[1]=PGE_RemSubSTRING(Params[1], " ");//Delete spaces
           if(!SMBX64::IsSInt(Params[1]))
           {
               unknownLines += fromNum(inf.getCurrentLineNumber())+": "+line+" <Should be signed intger!>\n";
           }
           else
           {
               Params[1] = PGE_ReplSTRING(Params[1], PGESTRING(" "), PGESTRING("")); //Delete spaces
               FileData.gfxoffsety=toInt(Params[1]);
               FileData.en_gfxoffsety=true;
           }
        }
       else
       if(Params[0]=="width")
        {
           Params[1] = PGESTR_Simpl(Params[1]);
           Params[1]=PGE_RemSubSTRING(Params[1], " ");//Delete spaces
           if(!SMBX64::IsUInt(Params[1]))
           {
               unknownLines += fromNum(inf.getCurrentLineNumber())+": "+line+" <Should be unsigned intger!>\n";
           }
           else
           {
               FileData.width=toInt(Params[1]);
               FileData.en_width=true;
           }
        }
       else
       if(Params[0]=="height")
        {
           Params[1] = PGESTR_Simpl(Params[1]);
           Params[1]=PGE_RemSubSTRING(Params[1], " ");//Delete spaces
           if(!SMBX64::IsUInt(Params[1]))
           {
              unknownLines += fromNum(inf.getCurrentLineNumber())+": "+line+" <Should be unsigned intger!>\n";
           }
           else
           {
               Params[1] = PGE_ReplSTRING(Params[1], PGESTRING(" "), PGESTRING("")); //Delete spaces
               FileData.height=toInt(Params[1]);
               FileData.en_height=true;
           }
        }
       else
       if(Params[0]=="gfxwidth")
        {
           Params[1] = PGESTR_Simpl(Params[1]);
           Params[1]=PGE_RemSubSTRING(Params[1], " ");//Delete spaces
           if(!SMBX64::IsUInt(Params[1]))
           {
              unknownLines += fromNum(inf.getCurrentLineNumber())+": "+line+" <Should be unsigned intger!>\n";
           }
           else
           {
               FileData.gfxwidth=toInt(Params[1]);
               FileData.en_gfxwidth=true;
           }
        }
       else
       if(Params[0]=="gfxheight")
        {
           Params[1] = PGESTR_Simpl(Params[1]);
           Params[1]=PGE_RemSubSTRING(Params[1], " ");//Delete spaces
           if(!SMBX64::IsUInt(Params[1]))
           {
              unknownLines += fromNum(inf.getCurrentLineNumber())+": "+line+" <Should be unsigned intger!>\n";
           }
           else
           {
              FileData.gfxheight=toInt(Params[1]);
              FileData.en_gfxheight=true;
           }
        }
       else
       if(Params[0]=="score")
        {
           Params[1] = PGESTR_Simpl(Params[1]);
           Params[1]=PGE_RemSubSTRING(Params[1], " ");//Delete spaces
           if(!SMBX64::IsUInt(Params[1]))
           {
              unknownLines += fromNum(inf.getCurrentLineNumber())+": "+line+" <Should be unsigned intger!>\n";
           }
           else
           {
              FileData.score=toInt(Params[1]);
              FileData.en_score=true;
           }
        }
       else
       if(Params[0]=="health")
        {
           Params[1] = PGESTR_Simpl(Params[1]);
           Params[1]=PGE_RemSubSTRING(Params[1], " ");//Delete spaces
           if(!SMBX64::IsUInt(Params[1]))
           {
              unknownLines += fromNum(inf.getCurrentLineNumber())+": "+line+" <Should be unsigned intger!>\n";
           }
           else
           {
              FileData.health=toInt(Params[1]);
              if(FileData.health<1)
                  FileData.health=1;
              FileData.en_health=true;
           }
        }
       else
       if(Params[0]=="playerblock")
        {
           Params[1] = PGESTR_Simpl(Params[1]);
           Params[1]=PGE_RemSubSTRING(Params[1], " ");//Delete spaces
           if(!SMBX64::IsBool(Params[1]))
           {
               unknownLines += fromNum(inf.getCurrentLineNumber())+": "+line+" <Should be 1 or 0!>\n";
           }
           else
           {
               FileData.playerblock=(bool)toInt(Params[1]);
               FileData.en_playerblock=true;
           }
        }
       else
       if(Params[0]=="playerblocktop")
        {
           Params[1] = PGESTR_Simpl(Params[1]);
           Params[1]=PGE_RemSubSTRING(Params[1], " ");//Delete spaces
           if(!SMBX64::IsBool(Params[1]))
           {
               unknownLines += fromNum(inf.getCurrentLineNumber())+": "+line+" <Should be 1 or 0!>\n";
           }
           else
           {
               FileData.playerblocktop=(bool)toInt(Params[1]);
               FileData.en_playerblocktop=true;
           }
        }
       else
       if(Params[0]=="npcblock")
        {
           Params[1] = PGESTR_Simpl(Params[1]);
           Params[1]=PGE_RemSubSTRING(Params[1], " ");//Delete spaces
           if(!SMBX64::IsBool(Params[1]))
           {
               unknownLines += fromNum(inf.getCurrentLineNumber())+": "+line+" <Should be 1 or 0!>\n";
           }
           else
           {
               FileData.npcblock=(bool)toInt(Params[1]);
               FileData.en_npcblock=true;
           }
        }
       else
       if(Params[0]=="npcblocktop")
        {
           Params[1] = PGESTR_Simpl(Params[1]);
           Params[1]=PGE_RemSubSTRING(Params[1], " ");//Delete spaces
           if(!SMBX64::IsBool(Params[1]))
           {
               unknownLines += fromNum(inf.getCurrentLineNumber())+": "+line+" <Should be 1 or 0!>\n";
           }
           else
           {
               FileData.npcblocktop=(bool)toInt(Params[1]);
               FileData.en_npcblocktop=true;
           }
        }
       else
       if(Params[0]=="grabside")
        {
           Params[1] = PGESTR_Simpl(Params[1]);
           Params[1]=PGE_RemSubSTRING(Params[1], " ");//Delete spaces
           if(!SMBX64::IsBool(Params[1]))
           {
               unknownLines += fromNum(inf.getCurrentLineNumber())+": "+line+" <Should be 1 or 0!>\n";
           }
           else
           {
               FileData.grabside=(bool)toInt(Params[1]);
               FileData.en_grabside=true;
           }
        }
       else
       if(Params[0]=="grabtop")
        {
           Params[1] = PGESTR_Simpl(Params[1]);
           Params[1]=PGE_RemSubSTRING(Params[1], " ");//Delete spaces
           if(!SMBX64::IsBool(Params[1]))
           {
               unknownLines += fromNum(inf.getCurrentLineNumber())+": "+line+" <Should be 1 or 0!>\n";
           }
           else
           {
               FileData.grabtop=(bool)toInt(Params[1]);
               FileData.en_grabtop=true;
           }
        }
       else
       if(Params[0]=="jumphurt")
        {
           Params[1] = PGESTR_Simpl(Params[1]);
           Params[1]=PGE_RemSubSTRING(Params[1], " ");//Delete spaces
           if(!SMBX64::IsBool(Params[1]))
           {
               unknownLines += fromNum(inf.getCurrentLineNumber())+": "+line+" <Should be 1 or 0!>\n";
           }
           else
           {
               FileData.jumphurt=(bool)toInt(Params[1]);
               FileData.en_jumphurt=true;
           }
        }
       else
       if(Params[0]=="nohurt")
        {
           Params[1] = PGESTR_Simpl(Params[1]);
           Params[1]=PGE_RemSubSTRING(Params[1], " ");//Delete spaces
           if(!SMBX64::IsBool(Params[1]))
           {
               unknownLines += fromNum(inf.getCurrentLineNumber())+": "+line+" <Should be 1 or 0!>\n";
           }
           else
           {
               FileData.nohurt=(bool)toInt(Params[1]);
               FileData.en_nohurt=true;
           }
        }
       else
       if(Params[0]=="noblockcollision")
        {
           Params[1] = PGESTR_Simpl(Params[1]);
           Params[1]=PGE_RemSubSTRING(Params[1], " ");//Delete spaces
           if(!SMBX64::IsBool(Params[1]))
           {
               unknownLines += fromNum(inf.getCurrentLineNumber())+": "+line+" <Should be 1 or 0!>\n";
           }
           else
           {
               FileData.noblockcollision=(bool)toInt(Params[1]);
               FileData.en_noblockcollision=true;
           }
        }
       else
       if(Params[0]=="cliffturn")
        {
           Params[1] = PGESTR_Simpl(Params[1]);
           Params[1]=PGE_RemSubSTRING(Params[1], " ");//Delete spaces
           if(!SMBX64::IsBool(Params[1]))
           {
               unknownLines += fromNum(inf.getCurrentLineNumber())+": "+line+" <Should be 1 or 0!>\n";
           }
           else
           {
               FileData.cliffturn=(bool)toInt(Params[1]);
               FileData.en_cliffturn=true;
           }
        }
       else
       if(Params[0]=="noyoshi")
        {
           Params[1] = PGESTR_Simpl(Params[1]);
           Params[1]=PGE_RemSubSTRING(Params[1], " ");//Delete spaces
           if(!SMBX64::IsBool(Params[1]))
           {
               unknownLines += fromNum(inf.getCurrentLineNumber())+": "+line+" <Should be 1 or 0!>\n";
           }
           else
           {
               FileData.noyoshi=(bool)toInt(Params[1]);
               FileData.en_noyoshi=true;
           }
        }
       else
       if(Params[0]=="foreground")
        {
           Params[1] = PGESTR_Simpl(Params[1]);
           Params[1]=PGE_RemSubSTRING(Params[1], " ");//Delete spaces
           if(!SMBX64::IsBool(Params[1]))
           {
               unknownLines += fromNum(inf.getCurrentLineNumber())+": "+line+" <Should be 1 or 0!>\n";
           }
           else
           {
               FileData.foreground=(bool)toInt(Params[1]);
               FileData.en_foreground=true;
           }
        }
       else
       if(Params[0]=="speed")
        {
           Params[1] = PGESTR_Simpl(Params[1]);
           Params[1]=PGE_RemSubSTRING(Params[1], " ");//Delete spaces
           if(!SMBX64::IsFloat(Params[1]))
           {
               unknownLines += fromNum(inf.getCurrentLineNumber())+": "+line+" <Should be signed floating point number!>\n";
           }
           else
           {
               FileData.speed=toFloat(PGE_ReplSTRING(Params[1], PGESTRING(","), PGESTRING(".")));
               FileData.en_speed=true;
           }
        }
       else
       if(Params[0]=="nofireball")
        {
           Params[1] = PGESTR_Simpl(Params[1]);
           Params[1]=PGE_RemSubSTRING(Params[1], " ");//Delete spaces
           if(!SMBX64::IsBool(Params[1]))
           {
               unknownLines += fromNum(inf.getCurrentLineNumber())+": "+line+" <Should be 1 or 0!>\n";
           }
           else
           {
               FileData.nofireball=(bool)toInt(Params[1]);
               FileData.en_nofireball=true;
           }
        }
       else
       if(Params[0]=="nogravity")
        {
           Params[1] = PGESTR_Simpl(Params[1]);
           Params[1]=PGE_RemSubSTRING(Params[1], " ");//Delete spaces
           if(!SMBX64::IsBool(Params[1]))
           {
               unknownLines += fromNum(inf.getCurrentLineNumber())+": "+line+" <Should be 1 or 0!>\n";
           }
           else
           {
               FileData.nogravity=(bool)toInt(Params[1]);
               FileData.en_nogravity=true;
           }
        }
       else
       if(Params[0]=="frames")
        {
           Params[1] = PGESTR_Simpl(Params[1]);
           Params[1]=PGE_RemSubSTRING(Params[1], " ");//Delete spaces
           if(!SMBX64::IsUInt(Params[1]))
           {
               unknownLines += fromNum(inf.getCurrentLineNumber())+": "+line+" <Should be unsigned intger!>\n";
           }
           else
           {
               FileData.frames=toInt(Params[1]);
               FileData.en_frames=true;
           }
        }
       else
       if(Params[0]=="framespeed")
        {
           Params[1] = PGESTR_Simpl(Params[1]);
           Params[1]=PGE_RemSubSTRING(Params[1], " ");//Delete spaces
           if(!SMBX64::IsUInt(Params[1]))
           {
               unknownLines += fromNum(inf.getCurrentLineNumber())+": "+line+" <Should be unsigned intger!>\n";
           }
           else
           {
               FileData.framespeed=toInt(Params[1]);
               FileData.en_framespeed=true;
           }
        }
       else
       if(Params[0]=="framestyle")
        {
           Params[1] = PGESTR_Simpl(Params[1]);
           Params[1]=PGE_RemSubSTRING(Params[1], " ");//Delete spaces
           if(!SMBX64::IsUInt(Params[1]))
           {
               unknownLines += fromNum(inf.getCurrentLineNumber())+": "+line+" <Should be from 0 to 3!>\n";
           }
           else
           {
               FileData.framestyle=toInt(Params[1]);
               FileData.en_framestyle=true;
           }
        }
       else
       if(Params[0]=="noiceball")
        {
           Params[1] = PGESTR_Simpl(Params[1]);
           Params[1]=PGE_RemSubSTRING(Params[1], " ");//Delete spaces
           if(!SMBX64::IsBool(Params[1]))
           {
              unknownLines += fromNum(inf.getCurrentLineNumber())+": "+line+" <Should be 1 or 0!>\n";
           }
           else
           {
               FileData.noiceball=(bool)toInt(Params[1]);
               FileData.en_noiceball=true;
           }
        }
       else


       // Non-SMBX64 parameters (not working in SMBX <=1.3)
       if(Params[0]=="nohammer")
        {
           Params[1] = PGESTR_Simpl(Params[1]);
           Params[1]=PGE_RemSubSTRING(Params[1], " ");//Delete spaces
           if(!SMBX64::IsBool(Params[1]))
           {
               unknownLines += fromNum(inf.getCurrentLineNumber())+": "+line+" <Should be 1 or 0!>\n";
           }
           else
           {
               FileData.nohammer=(bool)toInt(Params[1]);
               FileData.en_nohammer=true;
           }
        }
       else
       if(Params[0]=="noshell")
        {
           Params[1] = PGESTR_Simpl(Params[1]);
           Params[1]=PGE_RemSubSTRING(Params[1], " ");//Delete spaces
           if(!SMBX64::IsBool(Params[1]))
           {
               unknownLines += fromNum(inf.getCurrentLineNumber())+": "+line+" <Should be 1 or 0!>\n";
           }
           else
           {
               FileData.noshell=(bool)toInt(Params[1]);
               FileData.en_noshell=true;
           }
        }
       else
       if(Params[0]=="name")
        {
           if(SMBX64::IsQuotedString(Params[1]))
               FileData.name = removeQuotes(Params[1]);
           else
               FileData.name = Params[1];
               FileData.en_name=!IsEmpty(FileData.name);
        }
       else
       if(Params[0]=="image")
        {
           if(SMBX64::IsQuotedString(Params[1]))
               FileData.image = removeQuotes(Params[1]);
           else
               FileData.image= Params[1];
               FileData.en_image=!IsEmpty(FileData.image);
        }
       else
       if(Params[0]=="script")
        {
           if(SMBX64::IsQuotedString(Params[1]))
               FileData.script = removeQuotes(Params[1]);
           else
               FileData.script= Params[1];
               FileData.en_script=!IsEmpty(FileData.script);
        }
       else
       if(Params[0]=="grid")
        {
           if(!SMBX64::IsUInt(Params[1]))
           {
               unknownLines += fromNum(inf.getCurrentLineNumber())+": "+line+" <Should be unsigned integer!>\n";
           } else {
               if(Params[1].size()>7)
                   FileData.grid=32;
               else {
                   FileData.grid = toInt(Params[1]);
                   FileData.en_grid=true;
               }
           }
        }
       else
       if(Params[0]=="gridoffsetx")
        {
           if(!SMBX64::IsSInt(Params[1]))
           {
               unknownLines += fromNum(inf.getCurrentLineNumber())+": "+line+" <Should be signed integer!>\n";
           } else {
               if(Params[1].size()>7)
                   FileData.grid_offset_x=0;
               else {
                   FileData.grid_offset_x = toInt(Params[1]);
                   FileData.en_grid_offset_x=true;
               }
           }
        }
       else
       if(Params[0]=="gridoffsety")
        {
           if(!SMBX64::IsSInt(Params[1]))
           {
               unknownLines += fromNum(inf.getCurrentLineNumber())+": "+line+" <Should be signed integer!>\n";
           } else {
               if(Params[1].size()>7)
                   FileData.grid_offset_y=0;
               else {
                   FileData.grid_offset_y = toInt(Params[1]);
                   FileData.en_grid_offset_y=true;
               }
           }
        }
       else
       if(Params[0]=="gridalign")
        {
           if(!SMBX64::IsUInt(Params[1]))
           {
               unknownLines += fromNum(inf.getCurrentLineNumber())+": "+line+" <Should be unsigned integer!>\n";
           } else {
               if(Params[1].size()>7)
                   FileData.grid_offset_y=0;
               else {
                   FileData.grid_align = toInt(Params[1]);
                   FileData.en_grid_align=true;
               }
           }
        }
       else
       {
              //errStr = "Unknown value";
              //if(!IgnoreBad) goto badfile;
           //Store unknown value into warning
           unknownLines += fromNum(inf.getCurrentLineNumber())+": "+line+"\n";
       }

    } while(!inf.eof());

    #ifdef PGE_EDITOR
    if(!IgnoreBad)
    {
        if(!unknownLines.isEmpty())
        QMessageBox::warning(MainWinConnect::pMainWin, QObject::tr("Unknown values are presented"),
                             QObject::tr("Your file have an unknown values which will be removed\n"
                                             " when you will save file")+QString("\n====================================\n%1").arg(unknownLines),
                             QMessageBox::Ok);
    }
    #endif

    FileData.ReadFileValid=true;
    return true;

//No more need
    //badfile:    //If file format not corrects
    //BadFileMsg(inf.fileName(), inf.getCurrentLineNumber(), line+"\n"+Params[0]+"\nReason: "+errStr);
    //FileData.ReadFileValid=false;
    //return FileData;
}




//*********************************************************
//****************WRITE FILE*******************************
//*********************************************************
bool FileFormats::WriteNPCTxtFileF(PGESTRING filePath, NPCConfigFile &FileData)
{
    PGE_FileFormats_misc::TextFileOutput file;
    if(!file.open(filePath, false, true, PGE_FileFormats_misc::TextOutput::truncate))
    {
        errorString="Failed to open file for write";
        return false;
    }
    return WriteNPCTxtFile(file, FileData);
}

bool FileFormats::WriteNPCTxtFileRaw(NPCConfigFile &FileData, PGESTRING &rawdata)
{
    PGE_FileFormats_misc::RawTextOutput file;
    if(!file.open(&rawdata, PGE_FileFormats_misc::TextOutput::truncate))
    {
        errorString="Failed to open target raw string for write";
        return false;
    }
    return WriteNPCTxtFile(file, FileData);
}

//Convert NPC Options structore to text for saving
bool FileFormats::WriteNPCTxtFile(PGE_FileFormats_misc::TextOutput &out, NPCConfigFile &FileData)
{
    if(FileData.en_gfxoffsetx)
    {
        out << "gfxoffsetx=" + fromNum(FileData.gfxoffsetx) +"\n";
    }
    if(FileData.en_gfxoffsety)
    {
        out << "gfxoffsety=" + fromNum(FileData.gfxoffsety) +"\n";
    }
    if(FileData.en_gfxwidth)
    {
        out << "gfxwidth=" + fromNum(FileData.gfxwidth) +"\n";
    }
    if(FileData.en_gfxheight)
    {
        out << "gfxheight=" + fromNum(FileData.gfxheight) +"\n";
    }
    if(FileData.en_foreground)
    {
        out << "foreground=" + fromNum((int)FileData.foreground) +"\n";
    }
    if(FileData.en_width)
    {
        out << "width=" + fromNum(FileData.width) +"\n";
    }
    if(FileData.en_height)
    {
        out << "height=" + fromNum(FileData.height) +"\n";
    }

    if(FileData.en_score)
    {
        out << "score=" + fromNum(FileData.score) +"\n";
    }
    if(FileData.en_health)
    {
        out << "health=" + fromNum(FileData.health) +"\n";
    }

    if(FileData.en_playerblock)
    {
        out << "playerblock=" + fromNum((int)FileData.playerblock) +"\n";
    }

    if(FileData.en_playerblocktop)
    {
        out << "playerblocktop=" + fromNum((int)FileData.playerblocktop) +"\n";
    }

    if(FileData.en_npcblock)
    {
        out << "npcblock=" + fromNum((int)FileData.npcblock) +"\n";
    }

    if(FileData.en_npcblocktop)
    {
        out << "npcblocktop=" + fromNum((int)FileData.npcblocktop) +"\n";
    }
    if(FileData.en_grabside)
    {
        out << "grabside=" + fromNum((int)FileData.grabside) +"\n";
    }
    if(FileData.en_grabtop)
    {
        out << "grabtop=" + fromNum((int)FileData.grabtop) +"\n";
    }
    if(FileData.en_jumphurt)
    {
        out << "jumphurt=" + fromNum((int)FileData.jumphurt) +"\n";
    }
    if(FileData.en_nohurt)
    {
        out << "nohurt=" + fromNum((int)FileData.nohurt) +"\n";
    }
    if(FileData.en_speed)
    {
        out << "speed=" + fromNum(FileData.speed) +"\n";
    }
    if(FileData.en_noblockcollision)
    {
        out << "noblockcollision=" + fromNum((int)FileData.noblockcollision) +"\n";
    }
    if(FileData.en_cliffturn)
    {
        out << "cliffturn=" + fromNum((int)FileData.cliffturn) +"\n";
    }
    if(FileData.en_noyoshi)
    {
        out << "noyoshi=" + fromNum((int)FileData.noyoshi) +"\n";
    }
    if(FileData.en_nofireball)
    {
        out << "nofireball=" + fromNum((int)FileData.nofireball) +"\n";
    }
    if(FileData.en_nogravity)
    {
        out << "nogravity=" + fromNum((int)FileData.nogravity) +"\n";
    }
    if(FileData.en_noiceball)
    {
        out << "noiceball=" + fromNum((int)FileData.noiceball) +"\n";
    }
    if(FileData.en_frames)
    {
        out << "frames=" + fromNum(FileData.frames) +"\n";
    }
    if(FileData.en_framespeed)
    {
        out << "framespeed=" + fromNum(FileData.framespeed) +"\n";
    }
    if(FileData.en_framestyle)
    {
        out << "framestyle=" + fromNum(FileData.framestyle) +"\n";
    }

//Extended
    if(FileData.en_nohammer)
    {
        out << "nohammer=" + fromNum((int)FileData.nohammer) +"\n";
    }
    if(FileData.en_noshell)
    {
        out << "noshell=" + fromNum((int)FileData.noshell) +"\n";
    }
    if(FileData.en_name && !IsEmpty(FileData.name))
    {
        out << "name=" + SMBX64::qStrS(FileData.name);
    }
    if(FileData.en_image && !IsEmpty(FileData.image))
    {
        out << "image=" + SMBX64::qStrS(FileData.image);
    }
    if(FileData.en_script && !IsEmpty(FileData.script))
    {
        out << "script=" + SMBX64::qStrS(FileData.script);
    }
    if(FileData.en_grid)
    {
        out << "grid=" + fromNum(FileData.grid) +"\n";
    }
    if(FileData.en_grid_offset_x)
    {
        out << "gridoffsetx=" + fromNum(FileData.grid_offset_x) +"\n";
    }
    if(FileData.en_grid_offset_y)
    {
        out << "gridoffsety=" + fromNum(FileData.grid_offset_y) +"\n";
    }
    if(FileData.en_grid_align)
    {
        out << "gridalign=" + fromNum(FileData.grid_align) +"\n";
    }

    return true;
}

