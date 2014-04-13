/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include "mainwindow.h"
#include "lvl_filedata.h"
#include "wld_filedata.h"
#include "npcedit.h"

QRegExp isint("\\d+");     //Check "Is Numeric"
QRegExp boolwords("^(#TRUE#|#FALSE#)$");
QRegExp issint("^[\\-0]?\\d*$");     //Check "Is signed Numeric"
QRegExp issfloat("^[\\-]?(\\d*)?[\\.]?\\d*[Ee]?[\\-\\+]?\\d*$");     //Check "Is signed Float Numeric"
QRegExp booldeg("^(1|0)$");
QRegExp qstr("^\"(?:[^\"\\\\]|\\\\.)*\"$");
QString Quotes1 = "^\"(?:[^\"\\\\]|\\\\.)*\"$";
QString Quotes2 = "^(?:[^\"\\\\]|\\\\.)*$";

/*
if(myString.startsWith("\"") myString.remove(0,1);
if(myString.endsWith("\"") myString.remove(myString.size()-1,1);
*/

//NPC file Read
NPCConfigFile MainWindow::ReadNpcTXTFile(QFile &inf)
{
    int str_count=0;        //Line Counter
    //int i;                  //counters
    QString line;           //Current Line data
    QStringList Params;
    QTextStream in(&inf);   //Read File

    in.setAutoDetectUnicode(true); //Test Fix for MacOS
    in.setLocale(QLocale::system());   //Test Fix for MacOS
    in.setCodec(QTextCodec::codecForLocale()); //Test Fix for MacOS

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


    //Read NPC.TXT File config

    str_count++;line = in.readLine(); // Read file line
    while((!line.isNull()))
    {

       if(line.replace(QString(" "), QString(""))=="")
       {
           str_count++;line = in.readLine();
           continue;
       } //Skip empty strings

       Params=line.replace(QString(" "), QString("")). //Delete spaces
               split("=", QString::SkipEmptyParts); // splited the Parameter and value (example: chicken=2)

       if (Params.count() != 2) goto badfile;   // If string not contain strings with "=" as separator

       if(Params[0]=="gfxoffsetx")
        {
           if(!issint.exactMatch(Params[1]))
               goto badfile;

           FileData.gfxoffsetx=Params[1].toInt();
           FileData.en_gfxoffsetx=true;
        }
       else
       if(Params[0]=="gfxoffsety")
        {
           if(!issint.exactMatch(Params[1]))
              goto badfile;
           FileData.gfxoffsety=Params[1].toInt();
           FileData.en_gfxoffsety=true;
        }
       else
       if(Params[0]=="width")
        {
           if(!isint.exactMatch(Params[1]))
              goto badfile;
           FileData.width=Params[1].toInt();
           FileData.en_width=true;
        }
       else
       if(Params[0]=="height")
        {
           if(!isint.exactMatch(Params[1]))
              goto badfile;
           FileData.height=Params[1].toInt();
           FileData.en_height=true;
        }
       else
       if(Params[0]=="gfxwidth")
        {
           if(!isint.exactMatch(Params[1]))
              goto badfile;
           FileData.gfxwidth=Params[1].toInt();
           FileData.en_gfxwidth=true;
        }
       else
       if(Params[0]=="gfxheight")
        {
           if(!isint.exactMatch(Params[1]))
              goto badfile;
           FileData.gfxheight=Params[1].toInt();
           FileData.en_gfxheight=true;
        }
       else
       if(Params[0]=="score")
        {
           if(!isint.exactMatch(Params[1]))
              goto badfile;
           FileData.score=Params[1].toInt();
           FileData.en_score=true;
        }
       else
       if(Params[0]=="playerblock")
        {
           if(!booldeg.exactMatch(Params[1]))
              goto badfile;
           FileData.playerblock=(bool)Params[1].toInt();
           FileData.en_playerblock=true;
        }
       else
       if(Params[0]=="playerblocktop")
        {
           if(!booldeg.exactMatch(Params[1]))
              goto badfile;
           FileData.playerblocktop=(bool)Params[1].toInt();
           FileData.en_playerblocktop=true;
        }
       else
       if(Params[0]=="npcblock")
        {
           if(!booldeg.exactMatch(Params[1]))
              goto badfile;
           FileData.npcblock=(bool)Params[1].toInt();
           FileData.en_npcblock=true;
        }
       else
       if(Params[0]=="npcblocktop")
        {
           if(!booldeg.exactMatch(Params[1]))
              goto badfile;
           FileData.npcblocktop=(bool)Params[1].toInt();
           FileData.en_npcblocktop=true;
        }
       else
       if(Params[0]=="grabside")
        {
           if(!booldeg.exactMatch(Params[1]))
              goto badfile;
           FileData.grabside=(bool)Params[1].toInt();
           FileData.en_grabside=true;
        }
       else
       if(Params[0]=="grabtop")
        {
           if(!booldeg.exactMatch(Params[1]))
              goto badfile;
           FileData.grabtop=(bool)Params[1].toInt();
           FileData.en_grabtop=true;
        }
       else
       if(Params[0]=="jumphurt")
        {
           if(!booldeg.exactMatch(Params[1]))
              goto badfile;
           FileData.jumphurt=(bool)Params[1].toInt();
           FileData.en_jumphurt=true;
        }
       else
       if(Params[0]=="nohurt")
        {
           if(!booldeg.exactMatch(Params[1]))
              goto badfile;
           FileData.nohurt=(bool)Params[1].toInt();
           FileData.en_nohurt=true;
        }
       else
       if(Params[0]=="noblockcollision")
        {
           if(!booldeg.exactMatch(Params[1]))
              goto badfile;
           FileData.noblockcollision=(bool)Params[1].toInt();
           FileData.en_noblockcollision=true;
        }
       else
       if(Params[0]=="cliffturn")
        {
           if(!booldeg.exactMatch(Params[1]))
              goto badfile;
           FileData.cliffturn=(bool)Params[1].toInt();
           FileData.en_cliffturn=true;
        }
       else
       if(Params[0]=="noyoshi")
        {
           if(!booldeg.exactMatch(Params[1]))
              goto badfile;
           FileData.noyoshi=(bool)Params[1].toInt();
           FileData.en_noyoshi=true;
        }
       else
       if(Params[0]=="foreground")
        {
           if(!booldeg.exactMatch(Params[1]))
              goto badfile;
           FileData.foreground=(bool)Params[1].toInt();
           FileData.en_foreground=true;
        }
       else
       if(Params[0]=="speed")
        {
           if(!issfloat.exactMatch(Params[1]))
              goto badfile;
           FileData.speed=Params[1].toFloat();
           FileData.en_speed=true;
        }
       else
       if(Params[0]=="nofireball")
        {
           if(!booldeg.exactMatch(Params[1]))
              goto badfile;
           FileData.nofireball=(bool)Params[1].toInt();
           FileData.en_nofireball=true;
        }
       else
       if(Params[0]=="nogravity")
        {
           if(!booldeg.exactMatch(Params[1]))
              goto badfile;
           FileData.nogravity=(bool)Params[1].toInt();
           FileData.en_nogravity=true;
        }
       else
       if(Params[0]=="frames")
        {
           if(!isint.exactMatch(Params[1]))
              goto badfile;
           FileData.frames=Params[1].toInt();
           FileData.en_frames=true;
        }
       else
       if(Params[0]=="framespeed")
        {
           if(!isint.exactMatch(Params[1]))
              goto badfile;
           FileData.framespeed=Params[1].toInt();
           FileData.en_framespeed=true;
        }
       else
       if(Params[0]=="framestyle")
        {
           if(!isint.exactMatch(Params[1]))
              goto badfile;
           FileData.framestyle=Params[1].toInt();
           FileData.en_framestyle=true;
        }
       else
       if(Params[0]=="noiceball")
        {
           if(!booldeg.exactMatch(Params[1]))
              goto badfile;
           FileData.noiceball=(bool)Params[1].toInt();
           FileData.en_noiceball=true;
        }
       else
           if(Params[0]=="nohammer")
            {
               if(!booldeg.exactMatch(Params[1]))
                  goto badfile;
               FileData.nohammer=(bool)Params[1].toInt();
               FileData.en_nohammer=true;
            }
       else
           if(Params[0]=="noshell")
            {
               if(!booldeg.exactMatch(Params[1]))
                  goto badfile;
               FileData.noshell=(bool)Params[1].toInt();
               FileData.en_noshell=true;
            }
       else
       {
              goto badfile;
       }

    str_count++;line = in.readLine();
    }

FileData.ReadFileValid=true;
return FileData;


badfile:    //If file format not corrects
BadFileMsg(this, inf.fileName(), str_count, line+Params[0]);
FileData.ReadFileValid=false;
return FileData;
}


//Convert NPC Options structore to text for saving
QString npcedit::WriteNPCTxtFile(NPCConfigFile FileData)
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
    if(FileData.en_nohammer)
    {
        TextData += "nohammer=" + QString::number((int)FileData.nohammer) +"\n";
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

    return TextData;

}














//World file Read
WorldData MainWindow::ReadWorldFile(QFile &inf)
{
    int str_count=0;        //Line Counter
    //int i;                  //counters
    int file_format;        //File format number
    QString line;           //Current Line data
    QTextStream in(&inf);   //Read File

    in.setAutoDetectUnicode(true); //Test Fix for MacOS
    in.setLocale(QLocale::system());   //Test Fix for MacOS
    in.setCodec(QTextCodec::codecForLocale()); //Test Fix for MacOS


    WorldData FileData;

    str_count++;line = in.readLine();   //Read first Line
    if(!isint.exactMatch(line)) //File format number
        goto badfile;
    else file_format=line.toInt();


FileData.ReadFileValid=true;

return FileData;
badfile:    //If file format not corrects
BadFileMsg(this, inf.fileName(), str_count, line);
FileData.ReadFileValid=false;
return FileData;
}







//Level File Read
LevelData MainWindow::ReadLevelFile(QFile &inf)
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



    ///////////////////////////////////////Begin file///////////////////////////////////////
    str_count++;line = in.readLine();   //Read first Line
    if(!isint.exactMatch(line)) //File format number
        goto badfile;

    else file_format=line.toInt();

    if(file_format >= 17)
    {
        str_count++;line = in.readLine();   //Read second Line
        if(!isint.exactMatch(line)) //File format number
            goto badfile;
        else FileData.stars=line.toInt();   //Number of stars
    } else FileData.stars=0;

    if(file_format >= 63)
    {
        str_count++;line = in.readLine();   //Read Third Line
        if(!qstr.exactMatch(line)) //LevelTitle
            goto badfile;
        else FileData.LevelName = (line.remove(0,1)).remove(line.size()-2,1);//remove quotes
    } else FileData.LevelName="";

    /*
    if(myString.startsWith("\"") myString.remove(0,1);
    if(myString.endsWith("\"") myString.remove(myString.size()-1,1);
    */

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
        str_count++;line = in.readLine();
        if(!issint.exactMatch(line)) //left
            goto badfile;
        else {
               section.size_left=line.toInt();
               section.PositionX=line.toInt();
             }

        str_count++;line = in.readLine();
        if(!issint.exactMatch(line)) //top
            goto badfile;
        else section.size_top=line.toInt();

        str_count++;line = in.readLine();
        if(!issint.exactMatch(line)) //bottom
            goto badfile;
        else {
               section.size_bottom=line.toInt();
               section.PositionY=line.toInt()-602;
             }

        str_count++;line = in.readLine();
        if(!issint.exactMatch(line)) //right
            goto badfile;
        else section.size_right=line.toInt();

        str_count++;line = in.readLine();
        if(!isint.exactMatch(line)) //music
            goto badfile;
        else section.music_id=line.toInt();

        str_count++;line = in.readLine();
        if(!isint.exactMatch(line)) //bgcolor
            goto badfile;
        else section.bgcolor=line.toInt();

        str_count++;line = in.readLine();
        if(!boolwords.exactMatch(line)) //IsLevelWarp
            goto badfile;
        else section.IsWarp=((line=="#TRUE#")?true:false);

        str_count++;line = in.readLine();
        if(!boolwords.exactMatch(line)) //OffScreen
            goto badfile;
        else section.OffScreenEn=((line=="#TRUE#")?true:false);

        str_count++;line = in.readLine();
        if(!isint.exactMatch(line)) //BackGround id
            goto badfile;
        else section.background=line.toInt();

        str_count++;line = in.readLine();
        if(!boolwords.exactMatch(line)) //NoTurnBack
            goto badfile;
        else section.noback=((line=="#TRUE#")?true:false);

        if(file_format >= 32)
        {
            str_count++;line = in.readLine();
            if(!boolwords.exactMatch(line)) //UnderWater
                goto badfile;
            else section.underwater=((line=="#TRUE#")?true:false);
        }
        else section.underwater=false;

        if(file_format >= 2)
        {
            str_count++;line = in.readLine();
            if(!qstr.exactMatch(line)) //Custom Music
                goto badfile;
            else section.music_file = (line.remove(0,1)).remove(line.size()-2,1);//remove quotes
        } else section.music_file="";

    FileData.sections.push_back(section); //Add Section in main array
    }
    if(file_format<8)
    {
        section.size_left=0;
        section.size_top=0;
        section.size_bottom=0;
        section.size_right=0;
        section.music_id=0;
        section.bgcolor=16291944;
        section.IsWarp=false;
        section.OffScreenEn=false;
        section.background=0;
        section.noback=false;
        section.underwater=false;
        section.music_file="";
    }


    //Player's point config
    for(i=0;i<2;i++)
    {
        str_count++;line = in.readLine();
        if(!issint.exactMatch(line)) //1 Player x
            goto badfile;
        else players.x=line.toInt();

        str_count++;line = in.readLine();
        if(!issint.exactMatch(line)) //1 Player y
            goto badfile;
        else players.y=line.toInt();

        str_count++;line = in.readLine();
        if(!isint.exactMatch(line)) //1 Player w
            goto badfile;
        else players.w=line.toInt();

        str_count++;line = in.readLine();
        if(!isint.exactMatch(line)) //1 Player h
            goto badfile;
        else players.h=line.toInt();

    FileData.players.push_back(players);    //Add player in array
    }


    ////////////Block Data//////////
    str_count++;line = in.readLine();
    while(line!="\"next\"")
    {
        if(!issint.exactMatch(line)) //Block x
            goto badfile;
        else blocks.x = line.toInt();

        str_count++;line = in.readLine();
        if(!issint.exactMatch(line)) //Block y
            goto badfile;
        else blocks.y = line.toInt();

        str_count++;line = in.readLine();
        if(!isint.exactMatch(line)) //Block h
            goto badfile;
        else blocks.h = line.toInt();

        str_count++;line = in.readLine();
        if(!isint.exactMatch(line)) //Block w
            goto badfile;
        else blocks.w = line.toInt();

        str_count++;line = in.readLine();
        if(!isint.exactMatch(line)) //Block id
            goto badfile;
        else blocks.id = line.toInt();

        str_count++;line = in.readLine();
        if(!issint.exactMatch(line)) //Containing NPC id
            goto badfile;
        else blocks.npc_id = line.toInt();

        str_count++;line = in.readLine();
        if(!boolwords.exactMatch(line)) //Invisible
            goto badfile;
        else blocks.invisible = ((line=="#TRUE#")?true:false);

        if(file_format >= 63)
        {
            str_count++;line = in.readLine();
            if(!boolwords.exactMatch(line)) //Slippery
                goto badfile;
            else blocks.slippery = ((line=="#TRUE#")?true:false);
        } else blocks.slippery = false;

        if(file_format >= 10)
        {
            str_count++;line = in.readLine();
            if(!qstr.exactMatch(line)) //layer
                goto badfile;
            else blocks.layer = (line.remove(0,1)).remove(line.size()-2,1);

            str_count++;line = in.readLine();
            if(!qstr.exactMatch(line)) //event destroy
                goto badfile;
            else blocks.event_destroy = (line.remove(0,1)).remove(line.size()-2,1);

            str_count++;line = in.readLine();
            if(!qstr.exactMatch(line)) //event hit
                goto badfile;
            else blocks.event_hit = (line.remove(0,1)).remove(line.size()-2,1);

            str_count++;line = in.readLine();
            if(!qstr.exactMatch(line)) //event no more objects in layer
                goto badfile;
            else blocks.event_no_more = (line.remove(0,1)).remove(line.size()-2,1);
        }
        else
        {  //Set default if loading old file version
            blocks.layer = "Default";
            blocks.event_destroy="";
            blocks.event_hit="";
            blocks.event_no_more="";
        }

    FileData.blocks.push_back(blocks); //AddBlock into array

    str_count++;line = in.readLine();
    }


    ////////////BGO Data//////////
    str_count++;line = in.readLine();
    while(line!="\"next\"")
    {
        if(!issint.exactMatch(line)) //BGO x
            goto badfile;
        else bgodata.x = line.toInt();

        str_count++;line = in.readLine();
        if(!issint.exactMatch(line)) //BGO y
            goto badfile;
        else bgodata.y= line.toInt();

        str_count++;line = in.readLine();
        if(!isint.exactMatch(line)) //BGO id
            goto badfile;
        else bgodata.id= line.toInt();

        if(file_format >= 10)
        {
            str_count++;line = in.readLine();
            if(!qstr.exactMatch(line)) //layer
                goto badfile;
            else bgodata.layer = (line.remove(0,1)).remove(line.size()-2,1);
        }
        else bgodata.layer = "Default";

    FileData.bgo.push_back(bgodata); // Add Background object into array

    str_count++;line = in.readLine();
    }


    ////////////NPC Data//////////
     str_count++;line = in.readLine();
     while(line!="\"next\"")
     {
        /*  Used only for debug
         QMessageBox::warning(this, tr("Debug"),
         tr(QString("Debug\nLine X: %1\n"
                    "Converted: %2").arg(line)  //Print Bad data string
                    .arg( (long)round(line.toDouble()))
            .toStdString().c_str() ),
             QMessageBox::Ok);
        */

         if(!issfloat.exactMatch(line)) //NPC x
             goto badfile;
         else npcdata.x = round(line.toDouble());

         str_count++;line = in.readLine();

         if(!issfloat.exactMatch(line)) //NPC y
             goto badfile;
         else npcdata.y = round(line.toDouble());

         str_count++;line = in.readLine();
         if(!issint.exactMatch(line)) //NPC direction
             goto badfile;
         else npcdata.direct = line.toInt();

         str_count++;line = in.readLine();
         if(!isint.exactMatch(line)) //NPC id
             goto badfile;

         else npcdata.id = line.toInt();

         if(file_format >= 10)
         {
             npcdata.special_data = -1;
             switch(npcdata.id)
             {
             case 283:/*Buble*/ case 91: /*burried*/ case 284: /*SMW Lakitu*/
             case 96: /*egg*/   /*parakoopa*/
             case 76: case 121: case 122:case 123:case 124: case 161:case 176:case 177:
             /*paragoomba*/ case 243: case 244:
             /*Cheep-Cheep*/ case 28: case 229: case 230: case 232: case 233: case 234: case 236:
             /*WarpSelection*/ case 288: case 289: /*firebar*/ case 260:

                 str_count++;line = in.readLine();
                 if(!isint.exactMatch(line)) //NPC id
                     goto badfile;
                 else npcdata.special_data = line.toInt();

                 break;
             default: break;
             }

             str_count++;line = in.readLine();
             if(!boolwords.exactMatch(line)) //Generator enabled
                 goto badfile;
             else npcdata.generator = ((line=="#TRUE#")?true:false);

             npcdata.generator_direct = -1;
             npcdata.generator_type = -1;

             if(npcdata.generator)
             {

                 str_count++;line = in.readLine();
                 if(!isint.exactMatch(line)) //Generator direction
                     goto badfile;
                 else npcdata.generator_direct= line.toInt();

                 str_count++;line = in.readLine();
                 if(!isint.exactMatch(line)) //Generator type [1] Warp, [2] Projective
                     goto badfile;
                 else npcdata.generator_type = line.toInt();
                 str_count++;line = in.readLine();
                 if(!isint.exactMatch(line)) //Generator period ( sec*10 ) [1-600]
                     goto badfile;
                 else npcdata.generator_period = line.toInt();
             }

             str_count++;line = in.readLine();
             if(!qstr.exactMatch(line)) //Message
                 goto badfile;
             else npcdata.msg = (line.remove(0,1)).remove(line.size()-2,1);

             str_count++;line = in.readLine();
             if(!boolwords.exactMatch(line)) //Friedly NPC
                 goto badfile;
             else npcdata.fridly = ((line=="#TRUE#")?true:false);

             str_count++;line = in.readLine();
             if(!boolwords.exactMatch(line)) //Don't move NPC
                 goto badfile;
             else npcdata.nomove = ((line=="#TRUE#")?true:false);

             str_count++;line = in.readLine();
             if(!boolwords.exactMatch(line)) //LegacyBoss
                 goto badfile;
             else npcdata.legacyboss = ((line=="#TRUE#")?true:false);

             str_count++;line = in.readLine();
             if(!qstr.exactMatch(line)) //Layer
                 goto badfile;
             else npcdata.layer = (line.remove(0,1)).remove(line.size()-2,1);

             str_count++;line = in.readLine();
             if(!qstr.exactMatch(line)) //Activate event
                 goto badfile;
             else npcdata.event_activate = (line.remove(0,1)).remove(line.size()-2,1);

             str_count++;line = in.readLine();
             if(!qstr.exactMatch(line)) //Death event
                 goto badfile;
             else npcdata.event_die = (line.remove(0,1)).remove(line.size()-2,1);

             str_count++;line = in.readLine();
             if(!qstr.exactMatch(line)) //Talk event
                 goto badfile;
             else npcdata.event_talk = (line.remove(0,1)).remove(line.size()-2,1);

             str_count++;line = in.readLine();
             if(!qstr.exactMatch(line)) //No more object in layer event
                 goto badfile;
             else npcdata.event_nomore = (line.remove(0,1)).remove(line.size()-2,1);
        }
         else
         {
            npcdata.special_data = -1;
            npcdata.generator = false;
            npcdata.generator_direct = -1;
            npcdata.generator_type = -1;
            npcdata.msg = "";
            npcdata.fridly=false;
            npcdata.nomove=false;
            npcdata.legacyboss=false;
            npcdata.layer="Default";
            npcdata.event_activate="";
            npcdata.event_die="";
            npcdata.event_talk="";
            npcdata.event_nomore="";
         }


         if(file_format >= 63)
         {
             str_count++;line = in.readLine();
             if(!qstr.exactMatch(line)) //Layer name to attach
                 goto badfile;
             else npcdata.attach_layer = (line.remove(0,1)).remove(line.size()-2,1);
         }  else npcdata.attach_layer = "";

    FileData.npc.push_back(npcdata); //Add NPC into array
    str_count++;line = in.readLine();
    }


    ////////////Warp and Doors Data//////////
    str_count++;line = in.readLine();
    while( ((line!="\"next\"")&&(file_format>=10)) || ((file_format<10)&&(line!="")&&(!line.isNull())))
    {
        if(!issint.exactMatch(line)) //Entrance x
            goto badfile;
        else doors.ix = line.toInt();

        str_count++;line = in.readLine();
        if(!issint.exactMatch(line)) //Entrance y
            goto badfile;
        else doors.iy = line.toInt();

        str_count++;line = in.readLine();
        if(!issint.exactMatch(line)) //Exit x
            goto badfile;
        else doors.ox = line.toInt();

        str_count++;line = in.readLine();
        if(!issint.exactMatch(line)) //Exit y
            goto badfile;
        else doors.oy = line.toInt();

        str_count++;line = in.readLine();
        if(!isint.exactMatch(line)) //Entrance direction: [3] down, [1] up, [2] left, [4] right
            goto badfile;
        else doors.idirect= line.toInt();


        str_count++;line = in.readLine();
        if(!isint.exactMatch(line)) //Exit direction: [1] down [3] up [4] left [2] right
            goto badfile;
        else doors.odirect= line.toInt();


        str_count++;line = in.readLine();
        if(!isint.exactMatch(line)) //Door type: [1] pipe, [2] door, [0] instant
            goto badfile;
        else doors.type= line.toInt();


        if(file_format>=8)
        {
            str_count++;line = in.readLine();
            if(!qstr.exactMatch(line)) //Warp to level
                goto badfile;
            else doors.lname = (line.remove(0,1)).remove(line.size()-2,1);


            str_count++;line = in.readLine();
            if(!isint.exactMatch(line)) //Normal entrance or Warp to other door
                goto badfile;
            else doors.waprto= line.toInt();

            str_count++;line = in.readLine();
            if(!boolwords.exactMatch(line)) //Level Entrance (can not enter)
                goto badfile;
            else doors.lvl_i= ((line=="#TRUE#")?true:false);

            str_count++;line = in.readLine();
            if(!boolwords.exactMatch(line)) //Level Exit (End of level)
                goto badfile;
            else doors.lvl_o= ((line=="#TRUE#")?true:false);

            str_count++;line = in.readLine();
            if(!issint.exactMatch(line)) //WarpTo X
                goto badfile;
            else doors.world_x= line.toInt();

            str_count++;line = in.readLine();
            if(!issint.exactMatch(line)) //WarpTo y
                goto badfile;
            else doors.world_y= line.toInt();
        }
        else
        {
            doors.lname = "";
            doors.waprto = 0;
            doors.lvl_i = false;
            doors.lvl_o = false;
            doors.world_x = -1;
            doors.world_y = -1;
        }

        if(file_format>=8)
        {
            str_count++;line = in.readLine();
            if(!isint.exactMatch(line)) //Need a stars
                goto badfile;
            else doors.stars= line.toInt();
        } else doors.stars=0;

        if(file_format>=10)
        {
            str_count++;line = in.readLine();
            if(!qstr.exactMatch(line)) //Layer
                goto badfile;
            else doors.layer = (line.remove(0,1)).remove(line.size()-2,1);

            str_count++;line = in.readLine();
            if(!boolwords.exactMatch(line)) //<unused>, always FALSE
                goto badfile;
            else doors.unknown= ((line=="#TRUE#")?true:false);

        } else
        {
            doors.layer = "Default";
            doors.unknown = false;
        }

        if(file_format>=28)
        {
            str_count++;line = in.readLine();
            if(!boolwords.exactMatch(line)) //No Yoshi
                goto badfile;
            else doors.noyoshi = ((line=="#TRUE#")?true:false);

            str_count++;line = in.readLine();
            if(!boolwords.exactMatch(line)) //Allow NPC
                goto badfile;
            else doors.allownpc= ((line=="#TRUE#")?true:false);

            str_count++;line = in.readLine();
            if(!boolwords.exactMatch(line)) //Locked
                goto badfile;
            else doors.locked= ((line=="#TRUE#")?true:false);
        }
        else
        {
            doors.noyoshi=false;
            doors.allownpc=false;
            doors.locked=false;
        }


    FileData.doors.push_back(doors); //Add NPC into array
    str_count++;line = in.readLine();
    }

    ////////////Water/QuickSand Data//////////
    if(file_format>=32)
    {
        str_count++;line = in.readLine();
        while(line!="\"next\"")
        {
            if(!issint.exactMatch(line)) //Water x
                goto badfile;
            else waters.x = line.toInt();

            str_count++;line = in.readLine();
            if(!issint.exactMatch(line)) //Water y
                goto badfile;
            else waters.y = line.toInt();

            str_count++;line = in.readLine();
            if(!isint.exactMatch(line)) //Water w
                goto badfile;
            else waters.w = line.toInt();

            str_count++;line = in.readLine();
            if(!isint.exactMatch(line)) //Water h
                goto badfile;
            else waters.h = line.toInt();

            str_count++;line = in.readLine();
            if(!isint.exactMatch(line)) //Unused
                goto badfile;
            else waters.unknown = line.toInt();


            if(file_format>=63)
            {
                str_count++;line = in.readLine();
                if(!boolwords.exactMatch(line)) //Quicksand
                    goto badfile;
                else waters.quicksand = ((line=="#TRUE#")?true:false);
            }
            else waters.quicksand = false;


            str_count++;line = in.readLine();
            if(!qstr.exactMatch(line)) //Layer
                goto badfile;
            else waters.layer = (line.remove(0,1)).remove(line.size()-2,1);

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
            if(!qstr.exactMatch(line)) //Layer name
                goto badfile;
            else layers.name=(line.remove(0,1)).remove(line.size()-2,1);;

            str_count++;line = in.readLine();
            if(!boolwords.exactMatch(line)) //hidden layer
                goto badfile;
            else layers.hidden = ((line=="#TRUE#")?true:false);

        FileData.layers.push_back(layers); //Add Water area into array
        str_count++;line = in.readLine();
        }

        ////////////Events Data//////////
        str_count++;line = in.readLine();
        while((line!="")&&(!line.isNull()))
        {
            if(!qstr.exactMatch(line)) //Event name
                goto badfile;
            else events.name=(line.remove(0,1)).remove(line.size()-2,1);

            str_count++;line = in.readLine();
            if(!qstr.exactMatch(line)) //Event message
                goto badfile;


            if(file_format>=18)
            {
                str_count++;line = in.readLine();
                if(!isint.exactMatch(line)) //PlaySound
                    goto badfile;
                else events.sound_id  = line.toInt();
            }
            else events.sound_id  = 0;

            str_count++;line = in.readLine();
            if(!isint.exactMatch(line)) //EndGame
                goto badfile;
            else events.end_game  = line.toInt();

            for(i=0; i<21; i++)
            {
                str_count++;line = in.readLine();
                if(!qstr.exactMatch(line)) //Hide layer
                    goto badfile;
                else events_layers.hide=(line.remove(0,1)).remove(line.size()-2,1);

                str_count++;line = in.readLine();
                if(!qstr.exactMatch(line)) //Show layer
                    goto badfile;
                else events_layers.show=(line.remove(0,1)).remove(line.size()-2,1);

                str_count++;line = in.readLine();
                if(!qstr.exactMatch(line)) //Toggle layer
                    goto badfile;
                else events_layers.toggle=(line.remove(0,1)).remove(line.size()-2,1);

            events.layers.push_back(events_layers);
            }

            for(i=0; i<21; i++)
            {
                str_count++;line = in.readLine();
                if(!issint.exactMatch(line)) //Set Music
                    goto badfile;
                else events_sets.music_id  = line.toInt();
                str_count++;line = in.readLine();
                if(!issint.exactMatch(line)) //Set Background
                    goto badfile;
                else events_sets.background_id = line.toInt();

                str_count++;line = in.readLine();
                if(!issint.exactMatch(line)) //Set Position to: LEFT
                    goto badfile;
                else events_sets.position_left = line.toInt();

                str_count++;line = in.readLine();
                if(!issint.exactMatch(line)) //Set Position to: TOP
                    goto badfile;
                else events_sets.position_top = line.toInt();

                str_count++;line = in.readLine();
                if(!issint.exactMatch(line)) //Set Position to: BOTTOM
                    goto badfile;
                else events_sets.position_bottom = line.toInt();

                str_count++;line = in.readLine();
                if(!issint.exactMatch(line)) //Set Position to: RIGHT
                    goto badfile;
                else events_sets.position_right = line.toInt();

            events.sets.push_back(events_sets);
            }

            if(file_format>=28)
            {
                str_count++;line = in.readLine();
                if(!qstr.exactMatch(line)) //Trigger
                    goto badfile;
                else events.trigger=(line.remove(0,1)).remove(line.size()-2,1);

                str_count++;line = in.readLine();
                if(!isint.exactMatch(line)) //Start trigger event after x [sec*10]. Etc. 153,2 sec
                    goto badfile;
                else events.triiger_timer = line.toInt();

                str_count++;line = in.readLine();
                if(!boolwords.exactMatch(line)) //No Smoke
                    goto badfile;
                else events.nosmoke = ((line=="#TRUE#")?true:false);

                str_count++;line = in.readLine();
                if(!boolwords.exactMatch(line)) //Hold ALT-JUMP player control
                    goto badfile;
                else events.altjump = ((line=="#TRUE#")?true:false);

                str_count++;line = in.readLine();
                if(!boolwords.exactMatch(line)) //ALT-RUN
                    goto badfile;
                else events.altrun = ((line=="#TRUE#")?true:false);

                str_count++;line = in.readLine();
                if(!boolwords.exactMatch(line)) //DOWN
                    goto badfile;
                else events.down = ((line=="#TRUE#")?true:false);

                str_count++;line = in.readLine();
                if(!boolwords.exactMatch(line)) //DROP
                    goto badfile;
                else events.drop = ((line=="#TRUE#")?true:false);

                str_count++;line = in.readLine();
                if(!boolwords.exactMatch(line)) //JUMP
                    goto badfile;
                else events.jump = ((line=="#TRUE#")?true:false);

                str_count++;line = in.readLine();
                if(!boolwords.exactMatch(line)) //LEFT
                    goto badfile;
                else events.left = ((line=="#TRUE#")?true:false);

                str_count++;line = in.readLine();
                if(!boolwords.exactMatch(line)) //RIGHT
                    goto badfile;
                else events.right = ((line=="#TRUE#")?true:false);

                str_count++;line = in.readLine();
                if(!boolwords.exactMatch(line)) //RUN
                    goto badfile;
                else events.run = ((line=="#TRUE#")?true:false);

                str_count++;line = in.readLine();
                if(!boolwords.exactMatch(line)) //START
                    goto badfile;
                else events.start = ((line=="#TRUE#")?true:false);

                str_count++;line = in.readLine();
                if(!boolwords.exactMatch(line)) //UP
                    goto badfile;
                else events.up = ((line=="#TRUE#")?true:false);
            }
            else
            {
                events.trigger= "";
                events.triiger_timer=0;
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
            }

            if(file_format>=32)
            {
                str_count++;line = in.readLine();
                if(!boolwords.exactMatch(line)) //Auto start
                    goto badfile;
                else events.autostart = ((line=="#TRUE#")?true:false);

                str_count++;line = in.readLine();
                if(!qstr.exactMatch(line)) //Layer for movement
                    goto badfile;
                else events.movelayer = (line.remove(0,1)).remove(line.size()-2,1);

                str_count++;line = in.readLine();
                if(!issfloat.exactMatch(line)) //Layer moving speed – horizontal
                    goto badfile;
                else events.layer_speed_x = line.toFloat();

                str_count++;line = in.readLine();
                if(!issfloat.exactMatch(line)) //Layer moving speed – vertical
                    goto badfile;
                else events.layer_speed_y = line.toFloat();
            }
            else
            {
                events.autostart = false;
                events.movelayer = "";
                events.layer_speed_x = 0;
                events.layer_speed_x = 0;
            }


            if(file_format>=49)
            {
                str_count++;line = in.readLine();
                if(!issfloat.exactMatch(line)) //Move screen horizontal speed
                    goto badfile;
                else events.move_camera_x = line.toFloat();

                str_count++;line = in.readLine();
                if(!issfloat.exactMatch(line)) //Move screen vertical speed
                    goto badfile;
                else events.move_camera_y = line.toFloat();

                str_count++;line = in.readLine();
                if(!issint.exactMatch(line)) //Scroll section x, (in file value is x-1)
                    goto badfile;
                else events.scrool_section = line.toInt();
            }
            else
            {
                events.move_camera_x = 0;
                events.move_camera_y = 0;
                events.scrool_section = 0;
            }

        FileData.events.push_back(events);
        str_count++;line = in.readLine();
        }
    }
    ///////////////////////////////////////EndFile///////////////////////////////////////

    FileData.ReadFileValid=true;
    return FileData;

    badfile:    //If file format not corrects
    BadFileMsg(this, inf.fileName()+"\nFile format "+QString::number(file_format), str_count, line);
    FileData.ReadFileValid=false;
    return FileData;
}

//Errror for file parsing
void BadFileMsg(MainWindow *window, QString fileName_DATA, int str_count, QString line)
{
    QMessageBox::warning(window, MainWindow::tr("Bad File"),
    MainWindow::tr(QString("Bad file format\nFile: %1\n"
               "Line Number: %3\n"         //Print Line With error
               "Line Data: %2").arg(fileName_DATA.toStdString().c_str(),line)  //Print Bad data string
               .arg(str_count)
               .toStdString().c_str()),
        QMessageBox::Ok);
}
