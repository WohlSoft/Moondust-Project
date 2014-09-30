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

#include "file_formats.h"


//*********************************************************
//****************READ FILE FORMAT*************************
//*********************************************************

LevelData FileFormats::ReadExtendedLevelFile(QFile &inf)
{
    QTextStream in(&inf);   //Read File
    in.setCodec("UTF-8");

    int str_count=0;        //Line Counter
    int i;                  //counters
    //int file_format=0;        //File format number
    QString line;           //Current Line data
    LevelData FileData;

    LevelSection section;
    //int sct;
    PlayerPoint player;
    LevelBlock block;
    LevelBGO bgodata;
    LevelNPC npcdata;
    LevelDoors door;
    LevelPhysEnv physiczone;
    LevelLayers layer;
    LevelEvents event;
    //LevelEvents_layers events_layers;
    //LevelEvents_Sets events_sets;

    //Begin all ArrayID's here;
    FileData.blocks_array_id = 1;
    FileData.bgo_array_id = 1;
    FileData.npc_array_id = 1;
    FileData.doors_array_id = 1;
    FileData.physenv_array_id = 1;
    FileData.layers_array_id = 1;
    FileData.events_array_id = 1;

    FileData = dummyLvlDataArray();

    QString errorString;

    typedef QPair<QString, QStringList> PGEXSct;
    PGEXSct PGEXsection;

    QList<PGEXSct > PGEXTree;

    ///////////////////////////////////////Begin file///////////////////////////////////////
    //Read Sections
    bool sectionOpened=false;

    //Read PGE-X Tree
    while(!in.atEnd())
    {
        PGEXsection.first = in.readLine();
        PGEXsection.second.clear();

        if(QString(PGEXsection.first).remove(' ').isEmpty()) continue; //Skip empty strings

        sectionOpened=true;
        QString data;
        while(!in.atEnd())
        {
            data = in.readLine();
            if(data==PGEXsection.first+"_END") {sectionOpened=false; break;} // Close Section
            PGEXsection.second.push_back(data);
        }
        PGEXTree.push_back(PGEXsection);

        // WriteToLog(QtDebugMsg, QString("Section %1, lines %2, %3")
        //        .arg(PGEXsection.first)
        //        .arg(PGEXsection.second.size())
        //        .arg(sectionOpened?"opened":"closed")
        //        );
    }

    if(sectionOpened)
    {
        errorString=QString("Section [%1] is not closed").arg(PGEXsection.first);
        goto badfile;
    }

    foreach(PGEXSct sct, PGEXTree) //look sections
    {

        //WriteToLog(QtDebugMsg, QString("Section %1")
        //          .arg(sct.first) );

            bool good;
            for(i=0; i<sct.second.size();i++) //Look Entries
            {
                if(QString(sct.second[i]).remove(' ').isEmpty()) continue; //Skip empty strings

                if(sct.first=="JOKES")
                {
                    QMessageBox::information(nullptr, "Jokes", sct.second[i], QMessageBox::Ok);
                    continue;
                }


                QList<QStringList > sectData = PGEFile::splitDataLine(sct.second[i], &good);
                line = sct.second[i];

                if(!good)
                {
                    errorString=QString("Wrong data string format [%1]").arg(sct.second[i]);
                    goto badfile;
                }

                errorString=QString("Wrong value data type");

                //Scan values
                if(sct.first=="HEAD") // Head
                {
                    foreach(QStringList value, sectData) //Look markers and values
                    {
                          if(value[0]=="TL") //Level Title
                          {
                              if(PGEFile::IsQStr(value[1]))
                                  FileData.LevelName = PGEFile::X2STR(value[1]);
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="SZ") //Starz number
                          {
                              if(PGEFile::IsIntU(value[1]))
                                  FileData.stars = value[1].toInt();
                              else
                                  goto badfile;
                          }
                    }
                }//Header

                else
                if(sct.first=="SECTION") //Level Sections
                {
                    section = dummyLvlSection();
                    foreach(QStringList value, sectData) //Look markers and values
                    {
                          if(value[0]=="SC") //Section ID
                          {
                              if(PGEFile::IsIntU(value[1]))
                                  section.id = value[1].toInt();
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="L") //Left side
                          {
                              if(PGEFile::IsIntS(value[1]))
                              {
                                  section.size_left= value[1].toInt();
                                  section.PositionX=value[1].toInt()-10;
                              }
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="R")//Right side
                          {
                              if(PGEFile::IsIntS(value[1]))
                                  section.size_right= value[1].toInt();
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="T") //Top side
                          {
                              if(PGEFile::IsIntS(value[1]))
                              {
                                  section.size_top= value[1].toInt();
                                  section.PositionY=value[1].toInt()-10;
                              }
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="B")//Bottom side
                          {
                              if(PGEFile::IsIntS(value[1]))
                                  section.size_bottom= value[1].toInt();
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="MZ")//Stuff music ID
                          {
                              if(PGEFile::IsIntU(value[1]))
                                  section.music_id= value[1].toInt();
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="BG")//Stuff music ID
                          {
                              if(PGEFile::IsIntU(value[1]))
                                  section.background= value[1].toInt();
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="MF")//External music file path
                          {
                              if(PGEFile::IsQStr(value[1]))
                                  section.music_file=PGEFile::X2STR(value[1]);
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="CS")//Connect sides
                          {
                              if(PGEFile::IsBool(value[1]))
                                  section.IsWarp=(bool)value[1].toInt();
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="OE")//Offscreen exit
                          {
                              if(PGEFile::IsBool(value[1]))
                                  section.OffScreenEn=(bool)value[1].toInt();
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="SR")//Right-way scroll only (No Turn-back)
                          {
                              if(PGEFile::IsBool(value[1]))
                                  section.noback=(bool)value[1].toInt();
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="UW")//Underwater bit
                          {
                              if(PGEFile::IsBool(value[1]))
                                  section.underwater=(bool)value[1].toInt();
                              else
                                  goto badfile;
                          }

                    } //Level Sections

                    //add captured value into array
                    bool found=false;
                    int q=0;
                    for(q=0; q<FileData.sections.size();q++)
                    {
                        if(FileData.sections[q].id==section.id){found=true; break;}
                    }
                    if(found)
                        FileData.sections[q] = section;
                    else
                        FileData.sections.push_back(section);

                }//Level Section


                else
                if(sct.first=="STARTPOINT") // Player's points
                {
                    player = dummyLvlPlayerPoint();
                    foreach(QStringList value, sectData) //Look markers and values
                    {
                          if(value[0]=="ID") //ID of player point
                          {
                              if(PGEFile::IsIntU(value[1]))
                                  player.id = value[1].toInt();
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="X")
                          {
                              if(PGEFile::IsIntS(value[1]))
                                  player.x = value[1].toInt();
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="Y")
                          {
                              if(PGEFile::IsIntS(value[1]))
                                  player.y = value[1].toInt();
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="D")
                          {
                              if(PGEFile::IsIntS(value[1]))
                                  player.direction = value[1].toInt();
                              else
                                  goto badfile;
                          }
                    }

                    //add captured value into array
                    bool found=false;
                    int q=0;
                    for(q=0; q<FileData.players.size();q++)
                    {
                        if(FileData.players[q].id==player.id){found=true; break;}
                    }

                    PlayerPoint sz = dummyLvlPlayerPoint(player.id);
                    player.w = sz.w;
                    player.h = sz.h;

                    if(found)
                        FileData.players[q] = player;
                    else
                        FileData.players.push_back(player);

                }//Player's points

                else
                if(sct.first=="BLOCK") // Blocks
                {

                    block = dummyLvlBlock();
                    foreach(QStringList value, sectData) //Look markers and values
                    {
                          if(value[0]=="ID") //Block ID
                          {
                              if(PGEFile::IsIntU(value[1]))
                                  block.id = value[1].toInt();
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="X") // Position X
                          {
                              if(PGEFile::IsIntS(value[1]))
                                  block.x = value[1].toInt();
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="Y") //Position Y
                          {
                              if(PGEFile::IsIntS(value[1]))
                                  block.y = value[1].toInt();
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="W") //Width
                          {
                              if(PGEFile::IsIntU(value[1]))
                                  block.w = value[1].toInt();
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="H") //Height
                          {
                              if(PGEFile::IsIntU(value[1]))
                                  block.h = value[1].toInt();
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="CN") //Contains (coins/NPC)
                          {
                              if(PGEFile::IsIntS(value[1]))
                                  block.npc_id = value[1].toInt();
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="IV") //Invisible
                          {
                              if(PGEFile::IsBool(value[1]))
                                  block.invisible = (bool)value[1].toInt();
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="SL") //Slippery
                          {
                              if(PGEFile::IsBool(value[1]))
                                  block.slippery = (bool)value[1].toInt();
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="LR") //Layer name
                          {
                              if(PGEFile::IsQStr(value[1]))
                                  block.layer = PGEFile::X2STR(value[1]);
                              else
                                  goto badfile;
                          }
                    }

                    block.array_id = FileData.blocks_array_id++;
                    block.index = FileData.blocks.size();
                    FileData.blocks.push_back(block);
                }//Blocks


                else
                if(sct.first=="BGO") // BGO
                {
                    bgodata = dummyLvlBgo();
                    foreach(QStringList value, sectData) //Look markers and values
                    {
                              if(value[0]=="ID") //BGO ID
                              {
                                  if(PGEFile::IsIntU(value[1]))
                                      bgodata.id = value[1].toInt();
                                  else
                                      goto badfile;
                              }
                              else
                              if(value[0]=="X") //X Position
                              {
                                  if(PGEFile::IsIntS(value[1]))
                                      bgodata.x = value[1].toInt();
                                  else
                                      goto badfile;
                              }
                              else
                              if(value[0]=="Y") //Y Position
                              {
                                  if(PGEFile::IsIntS(value[1]))
                                      bgodata.y = value[1].toInt();
                                  else
                                      goto badfile;
                              }
                              else
                              if(value[0]=="ZO") //Z Offset
                              {
                                  if(PGEFile::IsFloat(value[1]))
                                      bgodata.z_offset = value[1].toDouble();
                                  else
                                      goto badfile;
                              }
                              else
                              if(value[0]=="ZP") //Z Position
                              {
                                  if(PGEFile::IsIntS(value[1]))
                                      bgodata.z_mode = value[1].toInt();
                                  else
                                      goto badfile;
                              }
                              else
                              if(value[0]=="SP") //SMBX64 Sorting priority
                              {
                                  if(PGEFile::IsIntS(value[1]))
                                      bgodata.smbx64_sp = value[1].toInt();
                                  else
                                      goto badfile;
                              }
                              else
                              if(value[0]=="LR") //Layer name
                              {
                                  if(PGEFile::IsQStr(value[1]))
                                      bgodata.layer = PGEFile::X2STR(value[1]);
                                  else
                                      goto badfile;
                              }
                    }

                    bgodata.array_id = FileData.bgo_array_id++;
                    bgodata.index = FileData.bgo.size();
                    FileData.bgo.push_back(bgodata);
                }//BGO

                else
                if(sct.first=="NPC") // NPC
                {
                    npcdata = dummyLvlNpc();
                    foreach(QStringList value, sectData) //Look markers and values
                    {
                        if(value[0]=="ID") //NPC ID
                        {
                            if(PGEFile::IsIntU(value[1]))
                                npcdata.id = value[1].toInt();
                            else
                                goto badfile;
                        }
                        else
                        if(value[0]=="X") //X position
                        {
                            if(PGEFile::IsIntS(value[1]))
                                npcdata.x = value[1].toInt();
                            else
                                goto badfile;
                        }
                        else
                        if(value[0]=="Y") //Y position
                        {
                            if(PGEFile::IsIntS(value[1]))
                                npcdata.y = value[1].toInt();
                            else
                                goto badfile;
                        }
                        else
                        if(value[0]=="D") //Direction
                        {
                            if(PGEFile::IsIntS(value[1]))
                                npcdata.direct = value[1].toInt();
                            else
                                goto badfile;
                        }
                        else
                        if(value[0]=="S1") //Special value 1
                        {
                            if(PGEFile::IsIntS(value[1]))
                                npcdata.special_data = value[1].toInt();
                            else
                                goto badfile;
                        }
                        else
                        if(value[0]=="S2") //Special value 2
                        {
                            if(PGEFile::IsIntS(value[1]))
                                npcdata.special_data2 = value[1].toInt();
                            else
                                goto badfile;
                        }
                        else
                        if(value[0]=="GE") //Generator
                        {
                            if(PGEFile::IsBool(value[1]))
                                npcdata.generator = (bool)value[1].toInt();
                            else
                                goto badfile;
                        }
                        else
                        if(value[0]=="GT") //Generator type
                        {
                            if(PGEFile::IsIntS(value[1]))
                                npcdata.generator = value[1].toInt();
                            else
                                goto badfile;
                        }
                        else
                        if(value[0]=="GD") //Generator direction
                        {
                            if(PGEFile::IsIntS(value[1]))
                                npcdata.generator_direct = value[1].toInt();
                            else
                                goto badfile;
                        }
                        else
                        if(value[0]=="GM") //Generator period
                        {
                            if(PGEFile::IsIntU(value[1]))
                                npcdata.generator_period = value[1].toInt();
                            else
                                goto badfile;
                        }
                        else
                        if(value[0]=="MG") //Message
                        {
                            if(PGEFile::IsQStr(value[1]))
                                npcdata.msg = PGEFile::X2STR(value[1]);
                            else
                                goto badfile;
                        }
                        else
                        if(value[0]=="FD") //Friendly
                        {
                            if(PGEFile::IsBool(value[1]))
                                npcdata.friendly = (bool)value[1].toInt();
                            else
                                goto badfile;
                        }
                        else
                        if(value[0]=="NM") //Don't move
                        {
                            if(PGEFile::IsBool(value[1]))
                                npcdata.nomove = (bool)value[1].toInt();
                            else
                                goto badfile;
                        }
                        else
                        if(value[0]=="BS") //Boss algorithm
                        {
                            if(PGEFile::IsBool(value[1]))
                                npcdata.legacyboss = (bool)value[1].toInt();
                            else
                                goto badfile;
                        }
                        else
                        if(value[0]=="LR") //Layer
                        {
                            if(PGEFile::IsQStr(value[1]))
                                npcdata.layer = PGEFile::X2STR(value[1]);
                            else
                                goto badfile;
                        }
                        else
                        if(value[0]=="LA") //Attach Layer
                        {
                            if(PGEFile::IsQStr(value[1]))
                                npcdata.attach_layer = PGEFile::X2STR(value[1]);
                            else
                                goto badfile;
                        }
                        else
                        if(value[0]=="EA") //Event slot "Activated"
                        {
                            if(PGEFile::IsQStr(value[1]))
                                npcdata.event_activate = PGEFile::X2STR(value[1]);
                            else
                                goto badfile;
                        }
                        else
                        if(value[0]=="ED") //Event slot "Death/Take/Destroy"
                        {
                            if(PGEFile::IsQStr(value[1]))
                                npcdata.event_die = PGEFile::X2STR(value[1]);
                            else
                                goto badfile;
                        }
                        else
                        if(value[0]=="ET") //Event slot "Talk"
                        {
                            if(PGEFile::IsQStr(value[1]))
                                npcdata.event_talk = PGEFile::X2STR(value[1]);
                            else
                                goto badfile;
                        }
                        else
                        if(value[0]=="EE") //Event slot "Layer is empty"
                        {
                            if(PGEFile::IsQStr(value[1]))
                                npcdata.event_nomore = PGEFile::X2STR(value[1]);
                            else
                                goto badfile;
                        }
                    }
                    npcdata.array_id = FileData.npc_array_id++;
                    npcdata.index = FileData.npc.size();
                    FileData.npc.push_back(npcdata);
                }//NPC

                else
                if(sct.first=="PHYSICS") // PHYSICS
                {
                    physiczone = dummyLvlPhysEnv();
                    foreach(QStringList value, sectData) //Look markers and values
                    {
                          if(value[0]=="ET") //Environment type
                          {
                              if(PGEFile::IsIntU(value[1]))
                                  physiczone.quicksand = (bool)value[1].toInt();
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="X") //X position
                          {
                              if(PGEFile::IsIntS(value[1]))
                                  physiczone.x = value[1].toInt();
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="Y") //Y position
                          {
                              if(PGEFile::IsIntS(value[1]))
                                  physiczone.y = value[1].toInt();
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="W") //Width
                          {
                              if(PGEFile::IsIntU(value[1]))
                                  physiczone.w = value[1].toInt();
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="H") //Height
                          {
                              if(PGEFile::IsIntU(value[1]))
                                  physiczone.h = value[1].toInt();
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="LR") //Layer
                          {
                              if(PGEFile::IsQStr(value[1]))
                                  physiczone.layer = PGEFile::X2STR(value[1]);
                              else
                                  goto badfile;
                          }
                    }
                    physiczone.array_id = FileData.physenv_array_id++;
                    physiczone.index = FileData.physez.size();
                    FileData.physez.push_back(physiczone);
                }//PHYSICS

                else
                if(sct.first=="DOORS") // DOORS
                {
                    door = dummyLvlDoor();
                    foreach(QStringList value, sectData) //Look markers and values
                    {
                        if(value[0]=="IX") //Input point
                        {
                          if(PGEFile::IsIntS(value[1]))
                              door.ix = value[1].toInt();
                          else
                              goto badfile;
                        }
                        else
                        if(value[0]=="IY") //Input point
                        {
                          if(PGEFile::IsIntS(value[1]))
                              door.iy = value[1].toInt();
                          else
                              goto badfile;
                        }
                        else
                        if(value[0]=="OX") //Output point
                        {
                          if(PGEFile::IsIntS(value[1]))
                              door.ox = value[1].toInt();
                          else
                              goto badfile;
                        }
                        else
                        if(value[0]=="OY") //Output point
                        {
                          if(PGEFile::IsIntS(value[1]))
                              door.oy = value[1].toInt();
                          else
                              goto badfile;
                        }
                        else
                        if(value[0]=="DT") //Input point
                        {
                          if(PGEFile::IsIntU(value[1]))
                              door.type = value[1].toInt();
                          else
                              goto badfile;
                        }
                        else
                        if(value[0]=="ID") //Input direction
                        {
                          if(PGEFile::IsIntU(value[1]))
                              door.idirect = value[1].toInt();
                          else
                              goto badfile;
                        }
                        else
                        if(value[0]=="OD") //Output direction
                        {
                          if(PGEFile::IsIntU(value[1]))
                              door.odirect = value[1].toInt();
                          else
                              goto badfile;
                        }
                        else
                        if(value[0]=="WX") //Target world map point
                        {
                          if(PGEFile::IsIntS(value[1]))
                              door.world_x = value[1].toInt();
                          else
                              goto badfile;
                        }
                        else
                        if(value[0]=="WY") //Target world map point
                        {
                          if(PGEFile::IsIntS(value[1]))
                              door.world_y = value[1].toInt();
                          else
                              goto badfile;
                        }
                        else
                        if(value[0]=="LF") //Target level file
                        {
                          if(PGEFile::IsQStr(value[1]))
                              door.lname = PGEFile::X2STR(value[1]);
                          else
                              goto badfile;
                        }
                        else
                        if(value[0]=="LI") //Target level file's input warp
                        {
                          if(PGEFile::IsIntU(value[1]))
                              door.warpto = value[1].toInt();
                          else
                              goto badfile;
                        }
                        else
                        if(value[0]=="ET") //Level Entrance
                        {
                          if(PGEFile::IsBool(value[1]))
                              door.lvl_i = (bool)value[1].toInt();
                          else
                              goto badfile;
                        }
                        else
                        if(value[0]=="EX") //Level exit
                        {
                          if(PGEFile::IsBool(value[1]))
                              door.lvl_o = (bool)value[1].toInt();
                          else
                              goto badfile;
                        }
                        else
                        if(value[0]=="SL") //Stars limit
                        {
                          if(PGEFile::IsIntU(value[1]))
                              door.stars = value[1].toInt();
                          else
                              goto badfile;
                        }
                        else
                        if(value[0]=="NV") //No Vehicles
                        {
                          if(PGEFile::IsBool(value[1]))
                              door.novehicles = (bool)value[1].toInt();
                          else
                              goto badfile;
                        }
                        else
                        if(value[0]=="AI") //Allow grabbed items
                        {
                          if(PGEFile::IsBool(value[1]))
                              door.allownpc = (bool)value[1].toInt();
                          else
                              goto badfile;
                        }
                        else
                        if(value[0]=="LC") //Door is locked
                        {
                          if(PGEFile::IsBool(value[1]))
                              door.locked = (bool)value[1].toInt();
                          else
                              goto badfile;
                        }
                        else
                        if(value[0]=="LR") //Layer
                        {
                          if(PGEFile::IsQStr(value[1]))
                              door.layer = PGEFile::X2STR(value[1]);
                          else
                              goto badfile;
                        }
                    }

                    door.isSetIn = ( !door.lvl_i );
                    door.isSetOut = ( !door.lvl_o || (door.lvl_i));

                    door.array_id = FileData.doors_array_id++;
                    door.index = FileData.doors.size();
                    FileData.doors.push_back(door);
                }//DOORS

                else
                if(sct.first=="LAYERS")
                {
                    layer = dummyLvlLayer();
                    foreach(QStringList value, sectData) //Look markers and values
                    {
                          if(value[0]=="LR") //Layer name
                          {
                              if(PGEFile::IsQStr(value[1]))
                                  layer.name = PGEFile::X2STR(value[1]);
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="HD") //Hidden
                          {
                              if(PGEFile::IsIntU(value[1]))
                                  layer.hidden = (bool)value[1].toInt();
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="LC") //Locked
                          {
                              if(PGEFile::IsIntU(value[1]))
                                  layer.locked = (bool)value[1].toInt();
                              else
                                  goto badfile;
                          }
                    }

                    //add captured value into array
                    bool found=false;
                    int q=0;
                    for(q=0; q<FileData.layers.size();q++)
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
                }//LAYERS

                //EVENTS comming soon
//                else
//                if(sct.first=="EVENTS_CLASSIC") //Action-styled events
//                {
//                    foreach(QStringList value, sectData) //Look markers and values
//                    {
//                            //  if(value[0]=="TL") //Level Title
//                            //  {
//                            //      if(PGEFile::IsQStr(value[1]))
//                            //          FileData.LevelName = PGEFile::X2STR(value[1]);
//                            //      else
//                            //          goto badfile;
//                            //  }
//                            //  else
//                            //  if(value[0]=="SZ") //Starz number
//                            //  {
//                            //      if(PGEFile::IsIntU(value[1]))
//                            //          FileData.stars = value[1].toInt();
//                            //      else
//                            //          goto badfile;
//                            //  }
//                    }
//                }//EVENTS

                else
                if(sct.first=="EVENTS_CLASSIC") //SMBX-compatible events
                {
                    event = dummyLvlEvent();

                    foreach(QStringList value, sectData) //Look markers and values
                    {
                          if(value[0]=="ET") //Event Title
                          {
                              if(PGEFile::IsQStr(value[1]))
                                  event.name = PGEFile::X2STR(value[1]);
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="MG") //Event Message
                          {
                              if(PGEFile::IsQStr(value[1]))
                                  event.msg = PGEFile::X2STR(value[1]);
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="SD") //Play Sound ID
                          {
                              if(PGEFile::IsIntU(value[1]))
                                  event.sound_id = value[1].toInt();
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="EG") //End game algorithm
                          {
                              if(PGEFile::IsIntU(value[1]))
                                  event.end_game = value[1].toInt();
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="LH") //Hide layers
                          {
                              if(PGEFile::IsStringArray(value[1]))
                                  event.layers_hide = PGEFile::X2STRArr(value[1]);
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="LS") //Show layers
                          {
                              if(PGEFile::IsStringArray(value[1]))
                                  event.layers_show = PGEFile::X2STRArr(value[1]);
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="LT") //Toggle layers
                          {
                              if(PGEFile::IsStringArray(value[1]))
                                  event.layers_toggle = PGEFile::X2STRArr(value[1]);
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="SM") //Switch music
                          {
                              if(PGEFile::IsStringArray(value[1]))
                              {
                                  QStringList musicSets = PGEFile::X2STRArr(value[1]);
                                  int q=0;
                                  for(q=0;q<event.sets.size() && q<musicSets.size(); q++)
                                  {
                                      if(!PGEFile::IsIntS(musicSets[q])) goto badfile;
                                      event.sets[q].music_id = musicSets[q].toInt();
                                  }
                              }
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="SB") //Switch background
                          {
                              if(PGEFile::IsStringArray(value[1]))
                              {
                                  QStringList bgSets = PGEFile::X2STRArr(value[1]);
                                  int q=0;
                                  for(q=0;q<event.sets.size() && q<bgSets.size(); q++)
                                  {
                                      if(!PGEFile::IsIntS(bgSets[q])) goto badfile;
                                      event.sets[q].background_id = bgSets[q].toInt();
                                  }
                              }
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="SS") //Section Size
                          {
                              if(PGEFile::IsStringArray(value[1]))
                              {
                                  QStringList bgSets = PGEFile::X2STRArr(value[1]);
                                  int q=0;
                                  for(q=0;q<event.sets.size() && q<bgSets.size(); q++)
                                  {
                                      QStringList sizes = bgSets[q].split(',');
                                      if(sizes.size()!=4) goto badfile;
                                      if(!PGEFile::IsIntS(sizes[0])) goto badfile;
                                      event.sets[q].position_left = sizes[0].toInt();
                                      if(!PGEFile::IsIntS(sizes[1])) goto badfile;
                                      event.sets[q].position_top = sizes[1].toInt();
                                      if(!PGEFile::IsIntS(sizes[2])) goto badfile;
                                      event.sets[q].position_bottom = sizes[2].toInt();
                                      if(!PGEFile::IsIntS(sizes[3])) goto badfile;
                                      event.sets[q].position_right = sizes[3].toInt();
                                  }
                              }
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="TE") //Trigger event
                          {
                              if(PGEFile::IsQStr(value[1]))
                                  event.trigger = PGEFile::X2STR(value[1]);
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="TD") //Trigger delay
                          {
                              if(PGEFile::IsIntU(value[1]))
                                  event.trigger_timer = value[1].toInt();
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="DS") //Disable smoke
                          {
                              if(PGEFile::IsBool(value[1]))
                                  event.nosmoke = (bool)value[1].toInt();
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="AU") //Auto start
                          {
                              if(PGEFile::IsBool(value[1]))
                                  event.autostart = (bool)value[1].toInt();
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="AU") //Auto start
                          {
                              if(PGEFile::IsBool(value[1]))
                                  event.autostart = (bool)value[1].toInt();
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="PC") //Player controls
                          {
                              if(PGEFile::IsBoolArray(value[1]))
                              {
                                  QList<bool > controls = PGEFile::X2BollArr(value[1]);
                                  if(controls.size()>=1) event.ctrl_up = controls[0];
                                  if(controls.size()>=2) event.ctrl_down = controls[1];
                                  if(controls.size()>=3) event.ctrl_left = controls[2];
                                  if(controls.size()>=4) event.ctrl_right = controls[3];
                                  if(controls.size()>=5) event.ctrl_run = controls[4];
                                  if(controls.size()>=6) event.ctrl_jump = controls[5];
                                  if(controls.size()>=7) event.ctrl_drop = controls[6];
                                  if(controls.size()>=8) event.ctrl_start = controls[7];
                                  if(controls.size()>=9) event.ctrl_altrun = controls[8];
                                  if(controls.size()>=10) event.ctrl_altjump = controls[9];
                              }
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="ML") //Move layer
                          {
                              if(PGEFile::IsQStr(value[1]))
                                  event.movelayer = PGEFile::X2STR(value[1]);
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="MX") //Layer motion speed X
                          {
                              if(PGEFile::IsFloat(value[1]))
                                  event.layer_speed_x = value[1].toDouble();
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="MY") //Layer motion speed Y
                          {
                              if(PGEFile::IsFloat(value[1]))
                                  event.layer_speed_y = value[1].toDouble();
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="AS") //Autoscroll section ID
                          {
                              if(PGEFile::IsIntS(value[1]))
                                  event.scroll_section = value[1].toInt();
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="AX") //Autoscroll speed X
                          {
                              if(PGEFile::IsFloat(value[1]))
                                  event.move_camera_x = value[1].toDouble();
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="AY") //Autoscroll speed Y
                          {
                              if(PGEFile::IsFloat(value[1]))
                                  event.move_camera_y = value[1].toDouble();
                              else
                                  goto badfile;
                          }
                    }

                    //add captured value into array
                    bool found=false;
                    int q=0;
                    for(q=0; q<FileData.events.size();q++)
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
                }//EVENTS_CLASSIC

            }

        //}//Head Section end

    }

    ///////////////////////////////////////EndFile///////////////////////////////////////

    FileData.ReadFileValid=true;
    return FileData;

    badfile:    //If file format is not correct
    BadFileMsg(inf.fileName()+"\nError message: "+errorString, str_count, line);
    FileData.ReadFileValid=false;
    return FileData;
}



//*********************************************************
//****************WRITE FILE FORMAT************************
//*********************************************************

QString FileFormats::WriteExtendedLvlFile(LevelData FileData)
{
    QString TextData;
    long i;

    //Count placed stars on this level
    FileData.stars=0;
    for(i=0;i<FileData.npc.size();i++)
    {
        if(FileData.npc[i].is_star)
            FileData.stars++;
    }

    //HEAD section
    TextData += "HEAD\n";
    TextData += PGEFile::value("TL", PGEFile::qStrS(FileData.LevelName)); // Level title
    TextData += PGEFile::value("SZ", PGEFile::IntS(FileData.stars));      // Stars number
    TextData += "\n";
    TextData += "HEAD_END\n";

    //SECTION section
    TextData += "SECTION\n";

    for(i=0; i< FileData.sections.size(); i++)
    {
        if(
                (FileData.sections[i].size_bottom==0) &&
                (FileData.sections[i].size_left==0) &&
                (FileData.sections[i].size_right==0) &&
                (FileData.sections[i].size_top==0)
           )
            continue; //Skip unitialized sections
        TextData += PGEFile::value("SC", PGEFile::IntS(FileData.sections[i].id));  // Section ID
        TextData += PGEFile::value("L", PGEFile::IntS(FileData.sections[i].size_left));  // Left size
        TextData += PGEFile::value("R", PGEFile::IntS(FileData.sections[i].size_right));  // Right size
        TextData += PGEFile::value("T", PGEFile::IntS(FileData.sections[i].size_top));  // Top size
        TextData += PGEFile::value("B", PGEFile::IntS(FileData.sections[i].size_bottom));  // Bottom size

        TextData += PGEFile::value("MZ", PGEFile::IntS(FileData.sections[i].music_id));  // Music ID
        TextData += PGEFile::value("MF", PGEFile::qStrS(FileData.sections[i].music_file));  // Music file

        TextData += PGEFile::value("BG", PGEFile::IntS(FileData.sections[i].background));  // Background ID
        //TextData += PGEFile::value("BG", PGEFile::qStrS(FileData.sections[i].background_file));  // Background file

        if(FileData.sections[i].IsWarp)
            TextData += PGEFile::value("CS", PGEFile::BoolS(FileData.sections[i].IsWarp));  // Connect sides
        if(FileData.sections[i].OffScreenEn)
            TextData += PGEFile::value("OE", PGEFile::BoolS(FileData.sections[i].OffScreenEn));  // Offscreen exit
        if(FileData.sections[i].noback)
            TextData += PGEFile::value("SR", PGEFile::BoolS(FileData.sections[i].noback));  // Right-way scroll only (No Turn-back)
        if(FileData.sections[i].underwater)
            TextData += PGEFile::value("UW", PGEFile::BoolS(FileData.sections[i].underwater));  // Underwater bit
        //TextData += PGEFile::value("SL", PGEFile::BoolS(FileData.sections[i].noforward));  // Left-way scroll only (No Turn-forward)
        TextData += "\n";
    }
    TextData += "SECTION_END\n";

    //STARTPOINT section
    TextData += "STARTPOINT\n";
    for(i=0; i< FileData.players.size(); i++)
    {
        if((FileData.players[i].w==0)&&
           (FileData.players[i].h==0))
            continue; //Skip empty points

        TextData += PGEFile::value("ID", PGEFile::IntS(FileData.players[i].id));  // Player ID
        TextData += PGEFile::value("X", PGEFile::IntS(FileData.players[i].x));  // Player X
        TextData += PGEFile::value("Y", PGEFile::IntS(FileData.players[i].y));  // Player Y
        TextData += PGEFile::value("D", PGEFile::IntS(FileData.players[i].direction));  // Direction -1 left, 1 right

        TextData += "\n";
    }
    TextData += "STARTPOINT_END\n";


    //BLOCK section
    if(!FileData.blocks.isEmpty())
    {
        TextData += "BLOCK\n";

        LevelBlock defBlock = dummyLvlBlock();

        for(i=0;i<FileData.blocks.size();i++)
        {
            //Type ID
            TextData += PGEFile::value("ID", PGEFile::IntS(FileData.blocks[i].id));  // Block ID

            //Position
            TextData += PGEFile::value("X", PGEFile::IntS(FileData.blocks[i].x));  // Block X
            TextData += PGEFile::value("Y", PGEFile::IntS(FileData.blocks[i].y));  // Block Y

            //Size
            TextData += PGEFile::value("W", PGEFile::IntS(FileData.blocks[i].w));  // Block Width (sizable only)
            TextData += PGEFile::value("H", PGEFile::IntS(FileData.blocks[i].h));  // Block Height (sizable only)

            //Included NPC
            if(FileData.blocks[i].npc_id!=0) //Write only if not zero
                TextData += PGEFile::value("CN", PGEFile::IntS(FileData.blocks[i].npc_id));  // Included NPC

            //Boolean flags
            if(FileData.blocks[i].invisible)
                TextData += PGEFile::value("IV", PGEFile::BoolS(FileData.blocks[i].invisible));  // Invisible
            if(FileData.blocks[i].slippery)
                TextData += PGEFile::value("SL", PGEFile::BoolS(FileData.blocks[i].slippery));  // Slippery flag

            //Layer
            if(FileData.blocks[i].layer!=defBlock.layer) //Write only if not default
                TextData += PGEFile::value("LR", PGEFile::qStrS(FileData.blocks[i].layer));  // Layer

            //Event Slots
            if(!FileData.blocks[i].event_destroy.isEmpty())
                TextData += PGEFile::value("ED", PGEFile::qStrS(FileData.blocks[i].event_destroy));
            if(!FileData.blocks[i].event_hit.isEmpty())
                TextData += PGEFile::value("EH", PGEFile::qStrS(FileData.blocks[i].event_hit));
            if(!FileData.blocks[i].event_no_more.isEmpty())
                TextData += PGEFile::value("EE", PGEFile::qStrS(FileData.blocks[i].event_no_more));

            TextData += "\n";
        }

        TextData += "BLOCK_END\n";
    }

    //BGO section
    if(!FileData.bgo.isEmpty())
    {
        TextData += "BGO\n";

        LevelBGO defBGO = dummyLvlBgo();

        for(i=0;i<FileData.bgo.size();i++)
        {
            TextData += PGEFile::value("ID", PGEFile::IntS(FileData.bgo[i].id));  // BGO ID

            //Position
            TextData += PGEFile::value("X", PGEFile::IntS(FileData.bgo[i].x));  // BGO X
            TextData += PGEFile::value("Y", PGEFile::IntS(FileData.bgo[i].y));  // BGO Y

            if(FileData.bgo[i].z_offset!=defBGO.z_offset)
                TextData += PGEFile::value("ZO", PGEFile::FloatS(FileData.bgo[i].z_offset));  // BGO Z-Offset
            if(FileData.bgo[i].z_mode!=defBGO.z_mode)
                TextData += PGEFile::value("ZP", PGEFile::FloatS(FileData.bgo[i].z_mode));  // BGO Z-Mode

            if(FileData.bgo[i].smbx64_sp != -1)
                TextData += PGEFile::value("SP", PGEFile::FloatS(FileData.bgo[i].smbx64_sp));  // BGO SMBX64 Sort Priority

            if(FileData.bgo[i].layer!=defBGO.layer) //Write only if not default
                TextData += PGEFile::value("LR", PGEFile::qStrS(FileData.bgo[i].layer));  // Layer

            TextData += "\n";
        }

        TextData += "BGO_END\n";
    }

    //NPC section
    if(!FileData.npc.isEmpty())
    {
        TextData += "NPC\n";

        LevelNPC defNPC = dummyLvlNpc();

        for(i=0;i<FileData.npc.size();i++)
        {
            TextData += PGEFile::value("ID", PGEFile::IntS(FileData.npc[i].id));  // NPC ID

            //Position
            TextData += PGEFile::value("X", PGEFile::IntS(FileData.npc[i].x));  // NPC X
            TextData += PGEFile::value("Y", PGEFile::IntS(FileData.npc[i].y));  // NPC Y

            TextData += PGEFile::value("D", PGEFile::IntS(FileData.npc[i].direct));  // NPC Direction

            if(FileData.npc[i].special_data!=defNPC.special_data)
                TextData += PGEFile::value("S1", PGEFile::IntS(FileData.npc[i].special_data));  // Special value 1
            if(FileData.npc[i].special_data2!=defNPC.special_data2)
                TextData += PGEFile::value("S2", PGEFile::IntS(FileData.npc[i].special_data2));  // Special value 2

            if(FileData.npc[i].generator)
            {
                TextData += PGEFile::value("GE", PGEFile::BoolS(FileData.npc[i].generator));  // NPC Generator
                TextData += PGEFile::value("GT", PGEFile::IntS(FileData.npc[i].generator_type));  // Generator type
                TextData += PGEFile::value("GD", PGEFile::IntS(FileData.npc[i].generator_direct));  // Generator direct
                TextData += PGEFile::value("GM", PGEFile::IntS(FileData.npc[i].generator_period));  // Generator time
            }

            if(!FileData.npc[i].msg.isEmpty())
                TextData += PGEFile::value("MG", PGEFile::qStrS(FileData.npc[i].msg));  // Message

            if(FileData.npc[i].friendly)
                TextData += PGEFile::value("FD", PGEFile::BoolS(FileData.npc[i].friendly));  // Friendly
            if(FileData.npc[i].nomove)
                TextData += PGEFile::value("NM", PGEFile::BoolS(FileData.npc[i].nomove));  // Idle
            if(FileData.npc[i].legacyboss)
                TextData += PGEFile::value("BS", PGEFile::BoolS(FileData.npc[i].legacyboss));  // Set as boss

            if(FileData.npc[i].layer!=defNPC.layer) //Write only if not default
                TextData += PGEFile::value("LR", PGEFile::qStrS(FileData.npc[i].layer));  // Layer

            if(!FileData.npc[i].attach_layer.isEmpty())
                TextData += PGEFile::value("LA", PGEFile::qStrS(FileData.npc[i].attach_layer));  // Attach layer

            //Event slots
            if(!FileData.npc[i].attach_layer.isEmpty())
                TextData += PGEFile::value("EA", PGEFile::qStrS(FileData.npc[i].event_activate));
            if(!FileData.npc[i].event_die.isEmpty())
                TextData += PGEFile::value("ED", PGEFile::qStrS(FileData.npc[i].event_die));
            if(!FileData.npc[i].event_talk.isEmpty())
                TextData += PGEFile::value("ET", PGEFile::qStrS(FileData.npc[i].event_talk));
            if(!FileData.npc[i].event_nomore.isEmpty())
                TextData += PGEFile::value("EE", PGEFile::qStrS(FileData.npc[i].event_nomore));

            TextData += "\n";
        }

        TextData += "NPC_END\n";
    }

    //PHYSICS section
    if(!FileData.physez.isEmpty())
    {
        TextData += "PHYSICS\n";
        LevelPhysEnv defPhys = dummyLvlPhysEnv();

        for(i=0;i<FileData.physez.size();i++)
        {
            TextData += PGEFile::value("ET", PGEFile::IntS(FileData.physez[i].quicksand?1:0));

            //Position
            TextData += PGEFile::value("X", PGEFile::IntS(FileData.physez[i].x));  // Physic Env X
            TextData += PGEFile::value("Y", PGEFile::IntS(FileData.physez[i].y));  // Physic Env Y

            //Size
            TextData += PGEFile::value("W", PGEFile::IntS(FileData.physez[i].w));  // Physic Env Width
            TextData += PGEFile::value("H", PGEFile::IntS(FileData.physez[i].h));  // Physic Env Height

            if(FileData.physez[i].layer!=defPhys.layer) //Write only if not default
                TextData += PGEFile::value("LR", PGEFile::qStrS(FileData.physez[i].layer));  // Layer
            TextData += "\n";
        }

        TextData += "PHYSICS_END\n";
    }


    //DOORS section
    if(!FileData.doors.isEmpty())
    {
        TextData += "DOORS\n";

        LevelDoors defDoor = dummyLvlDoor();
        for(i=0;i<FileData.doors.size();i++)
        {
            if( ((!FileData.doors[i].lvl_o) && (!FileData.doors[i].lvl_i)) || ((FileData.doors[i].lvl_o) && (!FileData.doors[i].lvl_i)) )
                if(!FileData.doors[i].isSetIn) continue; // Skip broken door
            if( ((!FileData.doors[i].lvl_o) && (!FileData.doors[i].lvl_i)) || ((FileData.doors[i].lvl_i)) )
                if(!FileData.doors[i].isSetOut) continue; // Skip broken door

            //Entrance
            if(FileData.doors[i].isSetIn)
            {
                TextData += PGEFile::value("IX", PGEFile::IntS(FileData.doors[i].ix));  // Warp Input X
                TextData += PGEFile::value("IY", PGEFile::IntS(FileData.doors[i].iy));  // Warp Input Y
            }

            if(FileData.doors[i].isSetOut)
            {
                TextData += PGEFile::value("OX", PGEFile::IntS(FileData.doors[i].ox));  // Warp Output X
                TextData += PGEFile::value("OY", PGEFile::IntS(FileData.doors[i].oy));  // Warp Output Y
            }

            TextData += PGEFile::value("DT", PGEFile::IntS(FileData.doors[i].type));  // Warp type

            TextData += PGEFile::value("ID", PGEFile::IntS(FileData.doors[i].idirect));  // Warp Input direction
            TextData += PGEFile::value("OD", PGEFile::IntS(FileData.doors[i].odirect));  // Warp Outpu direction


            if(FileData.doors[i].world_x != -1 && FileData.doors[i].world_y != -1)
            {
                TextData += PGEFile::value("WX", PGEFile::IntS(FileData.doors[i].world_x));  // World X
                TextData += PGEFile::value("WY", PGEFile::IntS(FileData.doors[i].world_y));  // World Y
            }

            if(!FileData.doors[i].lname.isEmpty())
            {
                TextData += PGEFile::value("LF", PGEFile::qStrS(FileData.doors[i].lname));  // Warp to level file
                TextData += PGEFile::value("LI", PGEFile::IntS(FileData.doors[i].warpto));  // Warp arrayID
            }

            if(FileData.doors[i].lvl_i)
                TextData += PGEFile::value("ET", PGEFile::BoolS(FileData.doors[i].lvl_i));  // Level Entance

            if(FileData.doors[i].lvl_o)
                TextData += PGEFile::value("EX", PGEFile::BoolS(FileData.doors[i].lvl_o));  // Level Exit

            if(FileData.doors[i].stars>0)
                TextData += PGEFile::value("SL", PGEFile::IntS(FileData.doors[i].stars));  // Need a stars

            if(FileData.doors[i].novehicles)
                TextData += PGEFile::value("NV", PGEFile::BoolS(FileData.doors[i].novehicles));  // Deny Vehicles

            if(FileData.doors[i].allownpc)
                TextData += PGEFile::value("AI", PGEFile::BoolS(FileData.doors[i].allownpc));  // Allow Items

            if(FileData.doors[i].locked)
                TextData += PGEFile::value("LC", PGEFile::BoolS(FileData.doors[i].locked));  // Locked door

            if(FileData.doors[i].layer!=defDoor.layer) //Write only if not default
                TextData += PGEFile::value("LR", PGEFile::qStrS(FileData.doors[i].layer));  // Layer

            TextData += "\n";
        }

        TextData += "DOORS_END\n";
    }
    //LAYERS section
    if(!FileData.layers.isEmpty())
    {
        TextData += "LAYERS\n";
        for(i=0;i<FileData.layers.size();i++)
        {
            TextData += PGEFile::value("LR", PGEFile::qStrS(FileData.layers[i].name));  // Layer name
            if(FileData.layers[i].hidden)
                TextData += PGEFile::value("HD", PGEFile::BoolS(FileData.layers[i].hidden));  // Hidden
            if(FileData.layers[i].locked)
                TextData += PGEFile::value("LC", PGEFile::BoolS(FileData.layers[i].locked));  // Locked
            TextData += "\n";
        }

        TextData += "LAYERS_END\n";
    }

    //EVENTS section (action styled)
        //EVENT sub-section of action-styled events


    //EVENTS_CLASSIC (SMBX-Styled events)
    if(!FileData.events.isEmpty())
    {
        TextData += "EVENTS_CLASSIC\n";
        bool addArray=false;
        for(i=0;i<FileData.events.size();i++)
        {

            TextData += PGEFile::value("ET", PGEFile::qStrS(FileData.events[i].name));  // Event name

            if(!FileData.events[i].msg.isEmpty())
                TextData += PGEFile::value("MG", PGEFile::qStrS(FileData.events[i].msg));  // Show Message

            if(FileData.events[i].sound_id!=0)
                TextData += PGEFile::value("SD", PGEFile::IntS(FileData.events[i].sound_id));  // Play Sound ID

            if(FileData.events[i].end_game!=0)
                TextData += PGEFile::value("EG", PGEFile::IntS(FileData.events[i].end_game));  // End game

            if(!FileData.events[i].layers_hide.isEmpty())
                TextData += PGEFile::value("LH", PGEFile::strArrayS(FileData.events[i].layers_hide));  // Hide Layers
            if(!FileData.events[i].layers_show.isEmpty())
                TextData += PGEFile::value("LS", PGEFile::strArrayS(FileData.events[i].layers_show));  // Show Layers
            if(!FileData.events[i].layers_toggle.isEmpty())
                TextData += PGEFile::value("LT", PGEFile::strArrayS(FileData.events[i].layers_toggle));  // Toggle Layers


            QStringList musicSets;
            addArray=false;
            foreach(LevelEvents_Sets x, FileData.events[i].sets)
            {
                musicSets.push_back(QString::number(x.music_id));
            }
            foreach(QString x, musicSets)
            { if(x!="-1") addArray=true; }

            if(addArray) TextData += PGEFile::value("SM", PGEFile::strArrayS(musicSets));  // Change section's musics


            QStringList backSets;
            addArray=false;
            foreach(LevelEvents_Sets x, FileData.events[i].sets)
            {
                backSets.push_back(QString::number(x.background_id));
            }
            foreach(QString x, backSets)
            { if(x!="-1") addArray=true; }

            if(addArray) TextData += PGEFile::value("SB", PGEFile::strArrayS(backSets));  // Change section's backgrounds


            QStringList sizeSets;
            addArray=false;
            foreach(LevelEvents_Sets x, FileData.events[i].sets)
            {
                sizeSets.push_back(QString::number(x.position_left)+","+QString::number(x.position_top)
                       +","+QString::number(x.position_bottom)+","+QString::number(x.position_right));
            }
            foreach(QString x, sizeSets)
            { if(x!="-1,0,0,0") addArray=true; }
            if(addArray) TextData += PGEFile::value("SS", PGEFile::strArrayS(sizeSets));  // Change section's sizes


            if(!FileData.events[i].trigger.isEmpty())
            {
                TextData += PGEFile::value("TE", PGEFile::qStrS(FileData.events[i].trigger)); // Trigger Event
                if(FileData.events[i].trigger_timer>0)
                    TextData += PGEFile::value("TD", PGEFile::IntS(FileData.events[i].trigger_timer)); // Trigger delay
            }


            if(FileData.events[i].nosmoke)
                TextData += PGEFile::value("DS", PGEFile::BoolS(FileData.events[i].nosmoke)); // Disable Smoke
            if(FileData.events[i].autostart)
                TextData += PGEFile::value("AU", PGEFile::BoolS(FileData.events[i].autostart)); // Autostart event

            QList<bool > controls;
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

            addArray=false;
            foreach(bool x, controls)
            { if(x) addArray=true; }
            if(addArray) TextData += PGEFile::value("PC", PGEFile::BoolArrayS(controls)); // Create boolean array

            if(!FileData.events[i].movelayer.isEmpty())
            {
                TextData += PGEFile::value("ML", PGEFile::qStrS(FileData.events[i].movelayer)); // Move layer
                TextData += PGEFile::value("MX", PGEFile::FloatS(FileData.events[i].layer_speed_x)); // Move layer X
                TextData += PGEFile::value("MY", PGEFile::FloatS(FileData.events[i].layer_speed_y)); // Move layer Y
            }

            TextData += PGEFile::value("AS", PGEFile::IntS(FileData.events[i].scroll_section)); // Move camera
            TextData += PGEFile::value("AX", PGEFile::FloatS(FileData.events[i].move_camera_x)); // Move camera x
            TextData += PGEFile::value("AY", PGEFile::FloatS(FileData.events[i].move_camera_y)); // Move camera y

            TextData += "\n";
        }
        TextData += "EVENTS_CLASSIC_END\n";
    }

    return TextData;
}
