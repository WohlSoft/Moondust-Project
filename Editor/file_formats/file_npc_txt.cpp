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

#include "file_formats.h"
#include "../edit_npc/npcedit.h"

//*********************************************************
//****************READ FILE FORMAT*************************
//*********************************************************


//NPC file Read

NPCConfigFile FileFormats::CreateEmpytNpcTXTArray()
{
    NPCConfigFile FileData;
    FileData.en_gfxoffsetx=false;
    FileData.en_gfxoffsety=false;
    FileData.en_width=false;
    FileData.en_height=false;
    FileData.en_gfxwidth=false;
    FileData.en_gfxheight=false;
    FileData.en_score=false;
    FileData.en_playerblock=false;
    FileData.en_playerblocktop=false;
    FileData.en_npcblock=false;
    FileData.en_npcblocktop=false;
    FileData.en_grabside=false;
    FileData.en_grabtop=false;
    FileData.en_jumphurt=false;
    FileData.en_nohurt=false;
    FileData.en_noblockcollision=false;
    FileData.en_cliffturn=false;
    FileData.en_noyoshi=false;
    FileData.en_foreground=false;
    FileData.en_speed=false;
    FileData.en_nofireball=false;
    FileData.en_nogravity=false;
    FileData.en_frames=false;
    FileData.en_framespeed=false;
    FileData.en_framestyle=false;
    FileData.en_noiceball=false;

    FileData.en_nohammer=false;
    FileData.en_noshell=false;
    FileData.en_name=false;
    return FileData;
}

obj_npc FileFormats::mergeNPCConfigs(obj_npc &global, NPCConfigFile &local, QSize captured)
{
    obj_npc merged;
    merged = global;
    merged.image = QPixmap();   //Clear image values
    merged.mask = QPixmap();

    merged.name = (local.en_name)?local.name:global.name;

    merged.gfx_offset_x = (local.en_gfxoffsetx)?local.gfxoffsetx:global.gfx_offset_x;
    merged.gfx_offset_y = (local.en_gfxoffsety)?local.gfxoffsety:global.gfx_offset_y;

    merged.width = (local.en_width)?local.width:global.width;
    merged.height = (local.en_height)?local.height:global.height;

    merged.foreground = (local.en_foreground)?local.foreground:global.foreground;

    merged.framespeed = (local.en_framespeed)? qRound( qreal(global.framespeed) / (qreal(8) / qreal(local.framespeed)) ) : global.framespeed;
    merged.framestyle = (local.en_framestyle)?local.framestyle:global.framestyle;

    //Copy fixture size to GFX size
    if((local.en_width)&&
                (
                ((merged.width <= (unsigned int)global.gfx_w)&&(merged.framestyle<2)&&(merged.framestyle>0))||
                ((merged.width != (unsigned int)global.gfx_w)&&(merged.framestyle==0))
                )
            )
        merged.gfx_w = merged.width;
    else
    {
        if((!local.en_gfxwidth)&&(captured.width()!=0)&&(global.gfx_w!=captured.width()))
            merged.width = captured.width();

        merged.gfx_w = global.gfx_w;
    }

    //Copy fixture size to GFX size
    if((local.en_height)&&(merged.height != (unsigned int)global.gfx_h)&&(!merged.custom_animate)/*&&(merged.framestyle<2)*/)
        merged.gfx_h = merged.height;
    else
        merged.gfx_h = global.gfx_h;

    if((!local.en_gfxwidth)&&(captured.width()!=0)&&(global.gfx_w!=captured.width()))
        merged.gfx_w = captured.width();
    else
        merged.gfx_w = (local.en_gfxwidth)?local.gfxwidth:merged.gfx_w;

    merged.gfx_h = (local.en_gfxheight)?local.gfxheight:merged.gfx_h;


    if(((int)merged.width>=(int)merged.grid))
        merged.grid_offset_x = -1 * qRound( qreal((int)merged.width % merged.grid)/2 );
    else
        merged.grid_offset_x = qRound( qreal( merged.grid - (int)merged.width )/2 );

    if(merged.grid_attach_style==1) merged.grid_offset_x += 16;

    merged.grid_offset_y = -merged.height % merged.grid;


    if((merged.framestyle==0)&&((local.en_gfxheight)||(local.en_height))&&(!local.en_frames))
    {
        merged.frames = qRound(qreal(captured.height())/qreal(merged.gfx_h));
        //merged.custom_animate = false;
    }
    else
        merged.frames = (local.en_frames)?local.frames:global.frames;

    if((local.en_frames)||(local.en_framestyle))
    {
        merged.ani_bidir = false; //Disable bidirectional animation
        if((local.en_frames)) merged.custom_animate = false; //Disable custom animation
    }

    merged.score = (local.en_score)?local.score:global.score;
    merged.block_player = (local.en_playerblock)?local.playerblock:global.block_player;
    merged.block_player_top = (local.en_playerblocktop)?local.playerblocktop:global.block_player_top;
    merged.block_npc = (local.en_npcblock)?local.npcblock:global.block_npc;
    merged.block_npc_top = (local.en_npcblocktop)?local.npcblocktop:global.block_npc_top;
    merged.grab_side = (local.en_grabside)?local.grabside:global.grab_side;
    merged.grab_top = (local.en_grabtop)?local.grabtop:global.grab_top;
    merged.kill_on_jump = (local.en_jumphurt)? (!local.jumphurt) : global.kill_on_jump ;
    merged.hurt_player = (local.en_nohurt)?!local.nohurt:global.hurt_player;
    merged.collision_with_blocks = (local.en_noblockcollision)?(!local.noblockcollision):global.collision_with_blocks;
    merged.turn_on_cliff_detect = (local.en_cliffturn)?local.cliffturn:global.turn_on_cliff_detect;
    merged.can_be_eaten = (local.en_noyoshi)?(!local.noyoshi):global.can_be_eaten;
    merged.speed = (local.en_speed) ? global.speed * local.speed : global.speed;
    merged.kill_by_fireball = (local.en_nofireball)?(!local.nofireball):global.kill_by_fireball;
    merged.gravity = (local.en_nogravity)?(!local.nogravity):global.gravity;
    merged.freeze_by_iceball = (local.en_noiceball)?(!local.noiceball):global.freeze_by_iceball;
    merged.kill_hammer = (local.en_nohammer)?(!local.nohammer):global.kill_hammer;
    merged.kill_by_npc = (local.en_noshell)?(!local.noshell):global.kill_by_npc;

    WriteToLog(QtDebugMsg, QString("-------------------------------------"));
    WriteToLog(QtDebugMsg, QString("NPC-Merge for NPC-ID=%1").arg(merged.id));
    WriteToLog(QtDebugMsg, QString("NPC-Merge -> Height:   %1").arg(merged.height));
    WriteToLog(QtDebugMsg, QString("NPC-Merge -> Width:    %1").arg(merged.width));
    WriteToLog(QtDebugMsg, QString("NPC-Merge -> GFX h:    %1").arg(merged.gfx_h));
    WriteToLog(QtDebugMsg, QString("NPC-Merge -> GFX w:    %1").arg(merged.gfx_w));
    WriteToLog(QtDebugMsg, QString("NPC-Merge -> Grid size %1").arg(merged.grid));
    WriteToLog(QtDebugMsg, QString("NPC-Merge -> Offset x: %1").arg(merged.grid_offset_x));
    WriteToLog(QtDebugMsg, QString("NPC-Merge -> Offset y: %1").arg(merged.grid_offset_y));
    WriteToLog(QtDebugMsg, QString("NPC-Merge -> GridStl:  %1").arg(merged.grid_attach_style));
    WriteToLog(QtDebugMsg, QString("NPC-Merge -> GFX offX: %1").arg(merged.gfx_offset_x));
    WriteToLog(QtDebugMsg, QString("NPC-Merge -> GFX offY: %1").arg(merged.gfx_offset_y));
    WriteToLog(QtDebugMsg, QString("NPC-Merge -> FrStyle:  %1").arg(merged.framestyle));
    WriteToLog(QtDebugMsg, QString("NPC-Merge -> Frames:   %1").arg((int)merged.frames));
    WriteToLog(QtDebugMsg, QString("-------------------------------------"));

    return merged;
}

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
       if (Params.count() != 2) goto badfile;   // If string does not contain strings with "=" as separator

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


WriteToLog(QtDebugMsg, "NPC.txt -> Successful loaded");
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

