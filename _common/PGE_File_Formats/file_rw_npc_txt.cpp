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
#include "file_strlist.h"
#include "smbx64.h"
#ifdef PGE_EDITOR
#include <QMessageBox>
#include <QTranslator>
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

NPCConfigFile FileFormats::ReadNpcTXTFile(PGESTRING file, bool IgnoreBad)
{
    errorString.clear();
    int str_count=0;        //Line Counter
    //int i;                  //counters
    PGESTRING line;           //Current Line data
    PGESTRINGList Params;
    PGESTRING unknownLines;
    NPCConfigFile FileData = CreateEmpytNpcTXT();

    if(!PGE_FileFormats_misc::TextFileInput::exists(file))
    {
        FileData.ReadFileValid=false;
        errorString="File not exists: "+file;
        #ifdef PGE_EDITOR
        QMessageBox::critical(MainWinConnect::pMainWin, QTranslator::tr("File open error"),
        QTranslator::tr("File is not exist"), QMessageBox::Ok);
        #endif
        return FileData;
    }
    PGE_FileFormats_misc::TextFileInput inf;
    if(!inf.open(file))
    {
        FileData.ReadFileValid=false;
        errorString="Can't open file to read: "+file;
        #ifdef PGE_EDITOR
        QMessageBox::critical(MainWinConnect::pMainWin, QTranslator::tr("File open error"),
        QTranslator::tr("Can't read the file"), QMessageBox::Ok);
        #endif
        return FileData;
    }

    //Read NPC.TXT File config
    #define NextLine(line) str_count++;line = inf.readLine();

    NextLine(line)
    while(!IsNULL(line))
    {
       PGESTRING ln = line;
       if(PGE_RemSSTR(line, " ")=="")
       {
           NextLine(line)
           continue;
       } //Skip empty strings

       line=ln;

       #ifdef PGE_FILES_QT
       Params=line.split("=", QString::SkipEmptyParts); // split the Parameter and value (example: chicken=2)
       #else
       PGE_SPLITSTR(Params, line, "=");
       #endif

       if(Params.size() != 2) // If string does not contain strings with "=" as separator
       {
           if(!IgnoreBad)
           {
               unknownLines += fromNum(str_count)+": "+line+" <wrong syntax!>\n";
               NextLine(line)
               continue;
           }
       }

       Params[0] = PGESTR_Simpl(Params[0]);
       Params[0]=PGE_RemSSTR(Params[0], " "); //Delete spaces

       if(Params[0]=="gfxoffsetx")
        {
           Params[1] = PGESTR_Simpl(Params[1]);
           Params[1]=PGE_RemSSTR(Params[1], " ");//Delete spaces
           if(SMBX64::sInt(Params[1]))
           {
               unknownLines += fromNum(str_count)+": "+line+" <Should be signed intger!>\n";
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
           Params[1]=PGE_RemSSTR(Params[1], " ");//Delete spaces
           if(SMBX64::sInt(Params[1]))
           {
               unknownLines += fromNum(str_count)+": "+line+" <Should be signed intger!>\n";
           }
           else
           {
               Params[1] = PGE_ReplSTR(Params[1], PGESTRING(" "), PGESTRING("")); //Delete spaces
               FileData.gfxoffsety=toInt(Params[1]);
               FileData.en_gfxoffsety=true;
           }
        }
       else
       if(Params[0]=="width")
        {
           Params[1] = PGESTR_Simpl(Params[1]);
           Params[1]=PGE_RemSSTR(Params[1], " ");//Delete spaces
           if(SMBX64::uInt(Params[1]))
           {
               unknownLines += fromNum(str_count)+": "+line+" <Should be unsigned intger!>\n";
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
           Params[1]=PGE_RemSSTR(Params[1], " ");//Delete spaces
           if(SMBX64::uInt(Params[1]))
           {
              unknownLines += fromNum(str_count)+": "+line+" <Should be unsigned intger!>\n";
           }
           else
           {
               Params[1] = PGE_ReplSTR(Params[1], PGESTRING(" "), PGESTRING("")); //Delete spaces
               FileData.height=toInt(Params[1]);
               FileData.en_height=true;
           }
        }
       else
       if(Params[0]=="gfxwidth")
        {
           Params[1] = PGESTR_Simpl(Params[1]);
           Params[1]=PGE_RemSSTR(Params[1], " ");//Delete spaces
           if(SMBX64::uInt(Params[1]))
           {
              unknownLines += fromNum(str_count)+": "+line+" <Should be unsigned intger!>\n";
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
           Params[1]=PGE_RemSSTR(Params[1], " ");//Delete spaces
           if(SMBX64::uInt(Params[1]))
           {
              unknownLines += fromNum(str_count)+": "+line+" <Should be unsigned intger!>\n";
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
           Params[1]=PGE_RemSSTR(Params[1], " ");//Delete spaces
           if(SMBX64::uInt(Params[1]))
           {
              unknownLines += fromNum(str_count)+": "+line+" <Should be unsigned intger!>\n";
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
           Params[1]=PGE_RemSSTR(Params[1], " ");//Delete spaces
           if(SMBX64::uInt(Params[1]))
           {
              unknownLines += fromNum(str_count)+": "+line+" <Should be unsigned intger!>\n";
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
           Params[1]=PGE_RemSSTR(Params[1], " ");//Delete spaces
           if(SMBX64::dBool(Params[1]))
           {
               unknownLines += fromNum(str_count)+": "+line+" <Should be 1 or 0!>\n";
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
           Params[1]=PGE_RemSSTR(Params[1], " ");//Delete spaces
           if(SMBX64::dBool(Params[1]))
           {
               unknownLines += fromNum(str_count)+": "+line+" <Should be 1 or 0!>\n";
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
           Params[1]=PGE_RemSSTR(Params[1], " ");//Delete spaces
           if(SMBX64::dBool(Params[1]))
           {
               unknownLines += fromNum(str_count)+": "+line+" <Should be 1 or 0!>\n";
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
           Params[1]=PGE_RemSSTR(Params[1], " ");//Delete spaces
           if(SMBX64::dBool(Params[1]))
           {
               unknownLines += fromNum(str_count)+": "+line+" <Should be 1 or 0!>\n";
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
           Params[1]=PGE_RemSSTR(Params[1], " ");//Delete spaces
           if(SMBX64::dBool(Params[1]))
           {
               unknownLines += fromNum(str_count)+": "+line+" <Should be 1 or 0!>\n";
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
           Params[1]=PGE_RemSSTR(Params[1], " ");//Delete spaces
           if(SMBX64::dBool(Params[1]))
           {
               unknownLines += fromNum(str_count)+": "+line+" <Should be 1 or 0!>\n";
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
           Params[1]=PGE_RemSSTR(Params[1], " ");//Delete spaces
           if(SMBX64::dBool(Params[1]))
           {
               unknownLines += fromNum(str_count)+": "+line+" <Should be 1 or 0!>\n";
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
           Params[1]=PGE_RemSSTR(Params[1], " ");//Delete spaces
           if(SMBX64::dBool(Params[1]))
           {
               unknownLines += fromNum(str_count)+": "+line+" <Should be 1 or 0!>\n";
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
           Params[1]=PGE_RemSSTR(Params[1], " ");//Delete spaces
           if(SMBX64::dBool(Params[1]))
           {
               unknownLines += fromNum(str_count)+": "+line+" <Should be 1 or 0!>\n";
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
           Params[1]=PGE_RemSSTR(Params[1], " ");//Delete spaces
           if(SMBX64::dBool(Params[1]))
           {
               unknownLines += fromNum(str_count)+": "+line+" <Should be 1 or 0!>\n";
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
           Params[1]=PGE_RemSSTR(Params[1], " ");//Delete spaces
           if(SMBX64::dBool(Params[1]))
           {
               unknownLines += fromNum(str_count)+": "+line+" <Should be 1 or 0!>\n";
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
           Params[1]=PGE_RemSSTR(Params[1], " ");//Delete spaces
           if(SMBX64::dBool(Params[1]))
           {
               unknownLines += fromNum(str_count)+": "+line+" <Should be 1 or 0!>\n";
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
           Params[1]=PGE_RemSSTR(Params[1], " ");//Delete spaces
           if(SMBX64::sFloat(Params[1]))
           {
               unknownLines += fromNum(str_count)+": "+line+" <Should be signed floating point number!>\n";
           }
           else
           {
               FileData.speed=toFloat(PGE_ReplSTR(Params[1], PGESTRING(","), PGESTRING(".")));
               FileData.en_speed=true;
           }
        }
       else
       if(Params[0]=="nofireball")
        {
           Params[1] = PGESTR_Simpl(Params[1]);
           Params[1]=PGE_RemSSTR(Params[1], " ");//Delete spaces
           if(SMBX64::dBool(Params[1]))
           {
               unknownLines += fromNum(str_count)+": "+line+" <Should be 1 or 0!>\n";
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
           Params[1]=PGE_RemSSTR(Params[1], " ");//Delete spaces
           if(SMBX64::dBool(Params[1]))
           {
               unknownLines += fromNum(str_count)+": "+line+" <Should be 1 or 0!>\n";
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
           Params[1]=PGE_RemSSTR(Params[1], " ");//Delete spaces
           if(SMBX64::uInt(Params[1]))
           {
               unknownLines += fromNum(str_count)+": "+line+" <Should be unsigned intger!>\n";
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
           Params[1]=PGE_RemSSTR(Params[1], " ");//Delete spaces
           if(SMBX64::uInt(Params[1]))
           {
               unknownLines += fromNum(str_count)+": "+line+" <Should be unsigned intger!>\n";
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
           Params[1]=PGE_RemSSTR(Params[1], " ");//Delete spaces
           if(SMBX64::uInt(Params[1]))
           {
               unknownLines += fromNum(str_count)+": "+line+" <Should be from 0 to 3!>\n";
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
           Params[1]=PGE_RemSSTR(Params[1], " ");//Delete spaces
           if(SMBX64::dBool(Params[1]))
           {
              unknownLines += fromNum(str_count)+": "+line+" <Should be 1 or 0!>\n";
           }
           else
           {
               FileData.noiceball=(bool)toInt(Params[1]);
               FileData.en_noiceball=true;
           }
        }
       else // Non-SMBX64 parameters (not working in SMBX <=1.3)
       if(Params[0]=="nohammer")
        {
           Params[1] = PGESTR_Simpl(Params[1]);
           Params[1]=PGE_RemSSTR(Params[1], " ");//Delete spaces
           if(SMBX64::dBool(Params[1]))
           {
               unknownLines += fromNum(str_count)+": "+line+" <Should be 1 or 0!>\n";
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
           Params[1]=PGE_RemSSTR(Params[1], " ");//Delete spaces
           if(SMBX64::dBool(Params[1]))
           {
               unknownLines += fromNum(str_count)+": "+line+" <Should be 1 or 0!>\n";
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
           if(!SMBX64::qStr(Params[1]))
               FileData.name = removeQuotes(Params[1]);
           else
               FileData.name = Params[1];
               FileData.en_name=true;
        }
       else
       if(Params[0]=="image")
        {
           if(!SMBX64::qStr(Params[1]))
               FileData.image = removeQuotes(Params[1]);
           else
               FileData.image= Params[1];
               FileData.en_image=true;
        }
       else
       if(Params[0]=="script")
        {
           if(!SMBX64::qStr(Params[1]))
               FileData.script = removeQuotes(Params[1]);
           else
               FileData.script= Params[1];
               FileData.en_script=true;
        }
       else
       {
              //errStr = "Unknown value";
              //if(!IgnoreBad) goto badfile;
           //Store unknown value into warning
           unknownLines += fromNum(str_count)+": "+line+"\n";
       }

    NextLine(line)
    }

    #ifdef PGE_EDITOR
    if(!IgnoreBad)
    {
        if(!unknownLines.isEmpty())
        QMessageBox::warning(MainWinConnect::pMainWin, QTranslator::tr("Unknown values are presented"),
                             QTranslator::tr("Your file have an unknown values which will be removed\n"
                                             " when you will save file")+QString("\n====================================\n%1").arg(unknownLines),
                             QMessageBox::Ok);
    }
    #endif


    FileData.ReadFileValid=true;
    return FileData;

//No more need
    //badfile:    //If file format not corrects
    //BadFileMsg(inf.fileName(), str_count, line+"\n"+Params[0]+"\nReason: "+errStr);
    //FileData.ReadFileValid=false;
    //return FileData;
}




//*********************************************************
//********WRITE FILE*******************************
//*********************************************************

//Convert NPC Options structore to text for saving
PGESTRING FileFormats::WriteNPCTxtFile(NPCConfigFile FileData)
{

    PGESTRING TextData;
    if(FileData.en_gfxoffsetx)
    {
        TextData += "gfxoffsetx=" + fromNum(FileData.gfxoffsetx) +"\n";
    }
    if(FileData.en_gfxoffsety)
    {
        TextData += "gfxoffsety=" + fromNum(FileData.gfxoffsety) +"\n";
    }
    if(FileData.en_gfxwidth)
    {
        TextData += "gfxwidth=" + fromNum(FileData.gfxwidth) +"\n";
    }
    if(FileData.en_gfxheight)
    {
        TextData += "gfxheight=" + fromNum(FileData.gfxheight) +"\n";
    }
    if(FileData.en_foreground)
    {
        TextData += "foreground=" + fromNum((int)FileData.foreground) +"\n";
    }
    if(FileData.en_width)
    {
        TextData += "width=" + fromNum(FileData.width) +"\n";
    }
    if(FileData.en_height)
    {
        TextData += "height=" + fromNum(FileData.height) +"\n";
    }

    if(FileData.en_score)
    {
        TextData += "score=" + fromNum(FileData.score) +"\n";
    }
    if(FileData.en_health)
    {
        TextData += "health=" + fromNum(FileData.health) +"\n";
    }

    if(FileData.en_playerblock)
    {
        TextData += "playerblock=" + fromNum((int)FileData.playerblock) +"\n";
    }

    if(FileData.en_playerblocktop)
    {
        TextData += "playerblocktop=" + fromNum((int)FileData.playerblocktop) +"\n";
    }

    if(FileData.en_npcblock)
    {
        TextData += "npcblock=" + fromNum((int)FileData.npcblock) +"\n";
    }

    if(FileData.en_npcblocktop)
    {
        TextData += "npcblocktop=" + fromNum((int)FileData.npcblocktop) +"\n";
    }
    if(FileData.en_grabside)
    {
        TextData += "grabside=" + fromNum((int)FileData.grabside) +"\n";
    }
    if(FileData.en_grabtop)
    {
        TextData += "grabtop=" + fromNum((int)FileData.grabtop) +"\n";
    }
    if(FileData.en_jumphurt)
    {
        TextData += "jumphurt=" + fromNum((int)FileData.jumphurt) +"\n";
    }
    if(FileData.en_nohurt)
    {
        TextData += "nohurt=" + fromNum((int)FileData.nohurt) +"\n";
    }
    if(FileData.en_speed)
    {
        TextData += "speed=" + fromNum(FileData.speed) +"\n";
    }
    if(FileData.en_noblockcollision)
    {
        TextData += "noblockcollision=" + fromNum((int)FileData.noblockcollision) +"\n";
    }
    if(FileData.en_cliffturn)
    {
        TextData += "cliffturn=" + fromNum((int)FileData.cliffturn) +"\n";
    }
    if(FileData.en_noyoshi)
    {
        TextData += "noyoshi=" + fromNum((int)FileData.noyoshi) +"\n";
    }
    if(FileData.en_nofireball)
    {
        TextData += "nofireball=" + fromNum((int)FileData.nofireball) +"\n";
    }
    if(FileData.en_nogravity)
    {
        TextData += "nogravity=" + fromNum((int)FileData.nogravity) +"\n";
    }
    if(FileData.en_noiceball)
    {
        TextData += "noiceball=" + fromNum((int)FileData.noiceball) +"\n";
    }
    if(FileData.en_frames)
    {
        TextData += "frames=" + fromNum(FileData.frames) +"\n";
    }
    if(FileData.en_framespeed)
    {
        TextData += "framespeed=" + fromNum(FileData.framespeed) +"\n";
    }
    if(FileData.en_framestyle)
    {
        TextData += "framestyle=" + fromNum(FileData.framestyle) +"\n";
    }

    //Extended
    if(FileData.en_nohammer)
    {
        TextData += "nohammer=" + fromNum((int)FileData.nohammer) +"\n";
    }
    if(FileData.en_noshell)
    {
        TextData += "noshell=" + fromNum((int)FileData.noshell) +"\n";
    }
    if(FileData.en_name)
    {
        TextData += "name=" + SMBX64::qStrS(FileData.name);
    }
    if(FileData.en_image)
    {
        TextData += "image=" + SMBX64::qStrS(FileData.image);
    }
    if(FileData.en_script)
    {
        TextData += "script=" + SMBX64::qStrS(FileData.script);
    }

    return TextData;
}

