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

//*********************************************************
//****************READ FILE FORMAT*************************
//*********************************************************

NPCConfigFile FileFormats::ReadNpcTXTFile(QFile &inf, bool IgnoreBad)
{
    int str_count=0;        //Line Counter
    //int i;                  //counters
    QString line;           //Current Line data
    QStringList Params;
    QTextStream in(&inf);   //Read File

    in.setAutoDetectUnicode(true); //Test Fix for MacOS
    in.setLocale(QLocale::system()); //Test Fix for MacOS
    in.setCodec(QTextCodec::codecForLocale()); //Test Fix for MacOS

    NPCConfigFile FileData = CreateEmpytNpcTXTArray();

    //Read NPC.TXT File config

    str_count++;line = in.readLine(); // Read file line
    while((!line.isNull()))
    {

       QString ln = line;
       if(line.replace(QString(" "), QString(""))=="")
       {
           str_count++;line = in.readLine();
           continue;
       } //Skip empty strings

       line=ln;

       Params=line.split("=", QString::SkipEmptyParts); // split the Parameter and value (example: chicken=2)
       if(Params.count() != 2) // If string does not contain strings with "=" as separator
       {
           if(!IgnoreBad)
               goto badfile;
           else
               continue;
       }

       Params[0] = Params[0].replace(QString(" "), QString("")); //Delete spaces

       if(Params[0]=="gfxoffsetx")
        {
           Params[1] = Params[1].replace(QString(" "), QString("")); //Delete spaces
           if(SMBX64::sInt(Params[1]))
           {
               if(!IgnoreBad) goto badfile;
           }
           else
           {
               FileData.gfxoffsetx=Params[1].toInt();
               FileData.en_gfxoffsetx=true;
           }
        }
       else
       if(Params[0]=="gfxoffsety")
        {
           Params[1] = Params[1].replace(QString(" "), QString("")); //Delete spaces
           if(SMBX64::sInt(Params[1]))
           {
               if(!IgnoreBad) goto badfile;
           }
           else
           {
               Params[1] = Params[1].replace(QString(" "), QString("")); //Delete spaces
               FileData.gfxoffsety=Params[1].toInt();
               FileData.en_gfxoffsety=true;
           }
        }
       else
       if(Params[0]=="width")
        {
           Params[1] = Params[1].replace(QString(" "), QString("")); //Delete spaces
           if(SMBX64::Int(Params[1]))
           {
              if(!IgnoreBad) goto badfile;
           }
           else
           {
               FileData.width=Params[1].toInt();
               FileData.en_width=true;
           }
        }
       else
       if(Params[0]=="height")
        {
           Params[1] = Params[1].replace(QString(" "), QString("")); //Delete spaces
           if(SMBX64::Int(Params[1]))
           {
              if(!IgnoreBad) goto badfile;
           }
           else
           {
               Params[1] = Params[1].replace(QString(" "), QString("")); //Delete spaces
               FileData.height=Params[1].toInt();
               FileData.en_height=true;
           }
        }
       else
       if(Params[0]=="gfxwidth")
        {
           Params[1] = Params[1].replace(QString(" "), QString("")); //Delete spaces
           if(SMBX64::Int(Params[1]))
           {
              if(!IgnoreBad) goto badfile;
           }
           else
           {
               FileData.gfxwidth=Params[1].toInt();
               FileData.en_gfxwidth=true;
           }
        }
       else
       if(Params[0]=="gfxheight")
        {
           Params[1] = Params[1].replace(QString(" "), QString("")); //Delete spaces
           if(SMBX64::Int(Params[1]))
           {
              if(!IgnoreBad) goto badfile;
           }
           else
           {
              FileData.gfxheight=Params[1].toInt();
              FileData.en_gfxheight=true;
           }
        }
       else
       if(Params[0]=="score")
        {
           Params[1] = Params[1].replace(QString(" "), QString("")); //Delete spaces
           if(SMBX64::Int(Params[1]))
           {
              if(!IgnoreBad) goto badfile;
           }
           else
           {
              FileData.score=Params[1].toInt();
              FileData.en_score=true;
           }
        }
       else
       if(Params[0]=="playerblock")
        {
           Params[1] = Params[1].replace(QString(" "), QString("")); //Delete spaces
           if(SMBX64::dBool(Params[1]))
           {
              if(!IgnoreBad) goto badfile;
           }
           else
           {
              FileData.playerblock=(bool)Params[1].toInt();
              FileData.en_playerblock=true;
           }
        }
       else
       if(Params[0]=="playerblocktop")
        {
           Params[1] = Params[1].replace(QString(" "), QString("")); //Delete spaces
           if(SMBX64::dBool(Params[1]))
           {
              if(!IgnoreBad) goto badfile;
           }
           else
           {
              FileData.playerblocktop=(bool)Params[1].toInt();
              FileData.en_playerblocktop=true;
           }
        }
       else
       if(Params[0]=="npcblock")
        {
           Params[1] = Params[1].replace(QString(" "), QString("")); //Delete spaces
           if(SMBX64::dBool(Params[1]))
           {
               if(!IgnoreBad) goto badfile;
           }
           else
           {
               FileData.npcblock=(bool)Params[1].toInt();
               FileData.en_npcblock=true;
           }
        }
       else
       if(Params[0]=="npcblocktop")
        {
           Params[1] = Params[1].replace(QString(" "), QString("")); //Delete spaces
           if(SMBX64::dBool(Params[1]))
           {
              if(!IgnoreBad) goto badfile;
           }
           else
           {
              FileData.npcblocktop=(bool)Params[1].toInt();
              FileData.en_npcblocktop=true;
           }
        }
       else
       if(Params[0]=="grabside")
        {
           Params[1] = Params[1].replace(QString(" "), QString("")); //Delete spaces
           if(SMBX64::dBool(Params[1]))
           {
             if(!IgnoreBad) goto badfile;
           }
           else
           {
               FileData.grabside=(bool)Params[1].toInt();
               FileData.en_grabside=true;
           }
        }
       else
       if(Params[0]=="grabtop")
        {
           Params[1] = Params[1].replace(QString(" "), QString("")); //Delete spaces
           if(SMBX64::dBool(Params[1]))
           {
              if (!IgnoreBad)goto badfile;
           }
           else
           {
               FileData.grabtop=(bool)Params[1].toInt();
               FileData.en_grabtop=true;
           }
        }
       else
       if(Params[0]=="jumphurt")
        {
           Params[1] = Params[1].replace(QString(" "), QString("")); //Delete spaces
           if(SMBX64::dBool(Params[1]))
           {
              if(!IgnoreBad) goto badfile;
           }
           else
           {
               FileData.jumphurt=(bool)Params[1].toInt();
               FileData.en_jumphurt=true;
           }
        }
       else
       if(Params[0]=="nohurt")
        {
           Params[1] = Params[1].replace(QString(" "), QString("")); //Delete spaces
           if(SMBX64::dBool(Params[1]))
           {
              if(!IgnoreBad) goto badfile;
           }
           else
           {
               FileData.nohurt=(bool)Params[1].toInt();
               FileData.en_nohurt=true;
           }
        }
       else
       if(Params[0]=="noblockcollision")
        {
           Params[1] = Params[1].replace(QString(" "), QString("")); //Delete spaces
           if(SMBX64::dBool(Params[1]))
           {
              if(!IgnoreBad) goto badfile;
           }
           else
           {
               FileData.noblockcollision=(bool)Params[1].toInt();
               FileData.en_noblockcollision=true;
           }
        }
       else
       if(Params[0]=="cliffturn")
        {
           Params[1] = Params[1].replace(QString(" "), QString("")); //Delete spaces
           if(SMBX64::dBool(Params[1]))
           {
              if(!IgnoreBad) goto badfile;
           }
           else
           {
               FileData.cliffturn=(bool)Params[1].toInt();
               FileData.en_cliffturn=true;
           }
        }
       else
       if(Params[0]=="noyoshi")
        {
           Params[1] = Params[1].replace(QString(" "), QString("")); //Delete spaces
           if(SMBX64::dBool(Params[1]))
           {
              if(!IgnoreBad) goto badfile;
           }
           else
           {
               FileData.noyoshi=(bool)Params[1].toInt();
               FileData.en_noyoshi=true;
           }
        }
       else
       if(Params[0]=="foreground")
        {
           Params[1] = Params[1].replace(QString(" "), QString("")); //Delete spaces
           if(SMBX64::dBool(Params[1]))
           {
              if(!IgnoreBad) goto badfile;
           }
           else
           {
               FileData.foreground=(bool)Params[1].toInt();
               FileData.en_foreground=true;
           }
        }
       else
       if(Params[0]=="speed")
        {
           Params[1] = Params[1].replace(QString(" "), QString("")); //Delete spaces
           if(SMBX64::sFloat(Params[1]))
           {
              if(!IgnoreBad) goto badfile;
           }
           else
           {
               FileData.speed=Params[1].replace(QChar(','), QChar('.')).toFloat();
               FileData.en_speed=true;
           }
        }
       else
       if(Params[0]=="nofireball")
        {
           Params[1] = Params[1].replace(QString(" "), QString("")); //Delete spaces
           if(SMBX64::dBool(Params[1]))
           {
              if(!IgnoreBad) goto badfile;
           }
           else
           {
               FileData.nofireball=(bool)Params[1].toInt();
               FileData.en_nofireball=true;
           }
        }
       else
       if(Params[0]=="nogravity")
        {
           Params[1] = Params[1].replace(QString(" "), QString("")); //Delete spaces
           if(SMBX64::dBool(Params[1]))
           {
              if(!IgnoreBad) goto badfile;
           }
           else
           {
               FileData.nogravity=(bool)Params[1].toInt();
               FileData.en_nogravity=true;
           }
        }
       else
       if(Params[0]=="frames")
        {
           Params[1] = Params[1].replace(QString(" "), QString("")); //Delete spaces
           if(SMBX64::Int(Params[1]))
           {
              if(!IgnoreBad) goto badfile;
           }
           else
           {
               FileData.frames=Params[1].toInt();
               FileData.en_frames=true;
           }
        }
       else
       if(Params[0]=="framespeed")
        {
           Params[1] = Params[1].replace(QString(" "), QString("")); //Delete spaces
           if(SMBX64::Int(Params[1]))
           {
              if(!IgnoreBad) goto badfile;
           }
           else
           {
               FileData.framespeed=Params[1].toInt();
               FileData.en_framespeed=true;
           }
        }
       else
       if(Params[0]=="framestyle")
        {
           Params[1] = Params[1].replace(QString(" "), QString("")); //Delete spaces
           if(SMBX64::Int(Params[1]))
           {
              if(!IgnoreBad) goto badfile;
           }
           else
           {
               FileData.framestyle=Params[1].toInt();
               FileData.en_framestyle=true;
           }
        }
       else
       if(Params[0]=="noiceball")
        {
           Params[1] = Params[1].replace(QString(" "), QString("")); //Delete spaces
           if(SMBX64::dBool(Params[1]))
           {
              if(!IgnoreBad) goto badfile;
           }
           else
           {
               FileData.noiceball=(bool)Params[1].toInt();
               FileData.en_noiceball=true;
           }
        }
       else // Non-SMBX64 parameters (not working in SMBX <=1.3)
       if(Params[0]=="nohammer")
        {
           Params[1] = Params[1].replace(QString(" "), QString("")); //Delete spaces
           if(SMBX64::dBool(Params[1]))
           {
               if(!IgnoreBad) goto badfile;
           }
           else
           {
               FileData.nohammer=(bool)Params[1].toInt();
               FileData.en_nohammer=true;
           }
        }
       else
       if(Params[0]=="noshell")
        {
           Params[1] = Params[1].replace(QString(" "), QString("")); //Delete spaces
           if(SMBX64::dBool(Params[1]))
           {
               if(!IgnoreBad) goto badfile;
           }
           else
           {
               FileData.noshell=(bool)Params[1].toInt();
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
       {
              if(!IgnoreBad) goto badfile;
       }

    str_count++;line = in.readLine();
    }

    FileData.ReadFileValid=true;
    return FileData;


    badfile:    //If file format not corrects
    BadFileMsg(inf.fileName(), str_count, line+"\n"+Params[0]);
    FileData.ReadFileValid=false;
    return FileData;
}




//*********************************************************
//********WRITE FILE*******************************
//*********************************************************

//Convert NPC Options structore to text for saving
QString FileFormats::WriteNPCTxtFile(NPCConfigFile FileData)
{

    QString TextData;
    if(FileData.en_gfxoffsetx)
    {
        TextData += "gfxoffsetx=" + QString::number(FileData.gfxoffsetx) +"\n";
    }
    if(FileData.en_gfxoffsety)
    {
        TextData += "gfxoffsety=" + QString::number(FileData.gfxoffsety) +"\n";
    }
    if(FileData.en_gfxwidth)
    {
        TextData += "gfxwidth=" + QString::number(FileData.gfxwidth) +"\n";
    }
    if(FileData.en_gfxheight)
    {
        TextData += "gfxheight=" + QString::number(FileData.gfxheight) +"\n";
    }
    if(FileData.en_foreground)
    {
        TextData += "foreground=" + QString::number((int)FileData.foreground) +"\n";
    }
    if(FileData.en_width)
    {
        TextData += "width=" + QString::number(FileData.width) +"\n";
    }
    if(FileData.en_height)
    {
        TextData += "height=" + QString::number(FileData.height) +"\n";
    }

    if(FileData.en_score)
    {
        TextData += "score=" + QString::number(FileData.score) +"\n";
    }

    if(FileData.en_playerblock)
    {
        TextData += "playerblock=" + QString::number((int)FileData.playerblock) +"\n";
    }

    if(FileData.en_playerblocktop)
    {
        TextData += "playerblocktop=" + QString::number((int)FileData.playerblocktop) +"\n";
    }

    if(FileData.en_npcblock)
    {
        TextData += "npcblock=" + QString::number((int)FileData.npcblock) +"\n";
    }

    if(FileData.en_npcblocktop)
    {
        TextData += "npcblocktop=" + QString::number((int)FileData.npcblocktop) +"\n";
    }
    if(FileData.en_grabside)
    {
        TextData += "grabside=" + QString::number((int)FileData.grabside) +"\n";
    }
    if(FileData.en_grabtop)
    {
        TextData += "grabtop=" + QString::number((int)FileData.grabtop) +"\n";
    }
    if(FileData.en_jumphurt)
    {
        TextData += "jumphurt=" + QString::number((int)FileData.jumphurt) +"\n";
    }
    if(FileData.en_nohurt)
    {
        TextData += "nohurt=" + QString::number((int)FileData.nohurt) +"\n";
    }
    if(FileData.en_speed)
    {
        TextData += "speed=" + QString::number(FileData.speed) +"\n";
    }
    if(FileData.en_noblockcollision)
    {
        TextData += "noblockcollision=" + QString::number((int)FileData.noblockcollision) +"\n";
    }
    if(FileData.en_cliffturn)
    {
        TextData += "cliffturn=" + QString::number((int)FileData.cliffturn) +"\n";
    }
    if(FileData.en_noyoshi)
    {
        TextData += "noyoshi=" + QString::number((int)FileData.noyoshi) +"\n";
    }
    if(FileData.en_nofireball)
    {
        TextData += "nofireball=" + QString::number((int)FileData.nofireball) +"\n";
    }
    if(FileData.en_nogravity)
    {
        TextData += "nogravity=" + QString::number((int)FileData.nogravity) +"\n";
    }
    if(FileData.en_noiceball)
    {
        TextData += "noiceball=" + QString::number((int)FileData.noiceball) +"\n";
    }
    if(FileData.en_frames)
    {
        TextData += "frames=" + QString::number(FileData.frames) +"\n";
    }
    if(FileData.en_framespeed)
    {
        TextData += "framespeed=" + QString::number(FileData.framespeed) +"\n";
    }
    if(FileData.en_framestyle)
    {
        TextData += "framestyle=" + QString::number(FileData.framestyle) +"\n";
    }

    //Extended
    if(FileData.en_nohammer)
    {
        TextData += "nohammer=" + QString::number((int)FileData.nohammer) +"\n";
    }
    if(FileData.en_noshell)
    {
        TextData += "noshell=" + QString::number((int)FileData.noshell) +"\n";
    }
    if(FileData.en_name)
    {
        TextData += "name=" + SMBX64::qStrS(FileData.name);
    }

    return TextData;
}

