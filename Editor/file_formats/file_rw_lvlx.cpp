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

#include <QFileInfo>
#include <QDir>

#include "file_formats.h"
#include "file_strlist.h"
#include "pge_x.h"

#ifdef PGE_EDITOR
#include <script/commands/memorycommand.h>
#endif
#ifndef PGE_ENGINE
#include <QMessageBox>
#endif

//*********************************************************
//****************READ FILE FORMAT*************************
//*********************************************************
LevelData FileFormats::ReadExtendedLevelFile(QFile &inf)
{
    QTextStream in(&inf);   //Read File
    in.setCodec("UTF-8");

    return ReadExtendedLvlFile( in.readAll(), inf.fileName() );
}


LevelData FileFormats::ReadExtendedLvlFileHeader(QString filePath)
{
    LevelData FileData;
    FileData = dummyLvlDataArray();

    QFile inf(filePath);
    if(!inf.open(QIODevice::ReadOnly))
    {
        FileData.ReadFileValid=false;
        return FileData;
    }
    QString line;
    int str_count=0;
    bool valid=false;
    QFileInfo in_1(filePath);
    FileData.filename = in_1.baseName();
    FileData.path = in_1.absoluteDir().absolutePath();
    QTextStream in(&inf);
    in.setCodec("UTF-8");

    //Find level header part
    do{
    str_count++;line = in.readLine();
    }while((line!="HEAD") && (!line.isNull()));

    QStringList header;
    str_count++;line = in.readLine();
    bool closed=false;
    while((line!="HEAD_END") && (!line.isNull()))
    {
        header.push_back(line);
        str_count++;line = in.readLine();
        if(line=="HEAD_END") closed=true;
    }
    if(!closed) goto badfile;

    foreach(QString header_line, header)
    {
        QList<QStringList >data = PGEFile::splitDataLine(header_line, &valid);

        for(int i=0;i<data.size();i++)
        {
            if(data[i].size()!=2) goto badfile;
            if(data[i][0]=="TL") //Level Title
            {
                if(PGEFile::IsQStr(data[i][1]))
                    FileData.LevelName = PGEFile::X2STR(data[i][1]);
                else
                    goto badfile;
            }
            else
            if(data[i][0]=="SZ") //Starz number
            {
                if(PGEFile::IsIntU(data[i][1]))
                    FileData.stars = data[i][1].toInt();
                else
                    goto badfile;
            }
        }
    }

    if(!closed)
        goto badfile;

    FileData.CurSection=0;
    FileData.playmusic=0;

    FileData.ReadFileValid=true;

    return FileData;
badfile:
    FileData.ReadFileValid=false;
    return FileData;
}

LevelData FileFormats::ReadExtendedLvlFile(QString RawData, QString filePath, bool sielent)
{
    FileStringList in;
    in.addData( RawData );

    int str_count=0;        //Line Counter

    QString line;           //Current Line data
    LevelData FileData;
    FileData = dummyLvlDataArray();

    //Add path data
    if(!filePath.isEmpty())
    {
        QFileInfo in_1(filePath);
        FileData.filename = in_1.baseName();
        FileData.path = in_1.absoluteDir().absolutePath();
    }

    FileData.untitled = false;
    FileData.modified = false;

    LevelSection lvl_section;

    PlayerPoint player;
    LevelBlock block;
    LevelBGO bgodata;
    LevelNPC npcdata;
    LevelDoors door;
    LevelPhysEnv physiczone;
    LevelLayers layer;
    LevelEvents event;

    QString errorString;

    ///////////////////////////////////////Begin file///////////////////////////////////////
    PGEFile pgeX_Data(RawData);
    if( !pgeX_Data.buildTreeFromRaw() )
    {
        errorString = pgeX_Data.lastError();
        goto badfile;
    }

    for(int section=0; section<pgeX_Data.dataTree.size(); section++) //look sections
    {
        ///////////////////JOKES//////////////////////
        if(pgeX_Data.dataTree[section].name=="JOKES")
        {
            #ifndef PGE_ENGINE
            if(!pgeX_Data.dataTree[section].data.isEmpty())
                if(!pgeX_Data.dataTree[section].data[0].values.isEmpty())
                    QMessageBox::information(nullptr, "Jokes",
                            pgeX_Data.dataTree[section].data[0].values[0].value,
                            QMessageBox::Ok);
            #endif
        }//jokes
        else ///////////////////HEADER//////////////////////
        if(pgeX_Data.dataTree[section].name=="HEAD")
        {
            if(pgeX_Data.dataTree[section].type!=PGEFile::PGEX_Struct)
            {
                errorString=QString("Wrong section data syntax:\nSection [%1]").arg(pgeX_Data.dataTree[section].name);
                goto badfile;
            }

            for(int sdata=0;sdata<pgeX_Data.dataTree[section].data.size();sdata++)
            {
                if(pgeX_Data.dataTree[section].data[sdata].type!=PGEFile::PGEX_Struct)
                {
                    errorString=QString("Wrong data item syntax:\nSection [%1]\nData line %2")
                            .arg(pgeX_Data.dataTree[section].name)
                            .arg(sdata);
                    goto badfile;
                }
                PGEFile::PGEX_Item x = pgeX_Data.dataTree[section].data[sdata];

                for(int sval=0;sval<x.values.size();sval++) //Look markers and values
                {
                    PGEFile::PGEX_Val v = x.values[sval];
                    errorString=QString("Wrong value syntax\nSection [%1]\nData line %2\nMarker %3\nValue %4")
                            .arg(pgeX_Data.dataTree[section].name)
                            .arg(sdata)
                            .arg(v.marker)
                            .arg(v.value);

                    if(v.marker=="TL") //Level Title
                    {
                        if(PGEFile::IsQStr(v.value))
                            FileData.LevelName = PGEFile::X2STR(v.value);
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="SZ") //Starz number
                    {
                        if(PGEFile::IsIntU(v.value))
                            FileData.stars = v.value.toInt();
                        else
                            goto badfile;
                    }
                }
            }
        }//HEADER
        ///////////////////////////////MetaDATA/////////////////////////////////////////////
        else
        if(pgeX_Data.dataTree[section].name=="META_BOOKMARKS")
        {
            if(pgeX_Data.dataTree[section].type!=PGEFile::PGEX_Struct)
            {
                errorString=QString("Wrong section data syntax:\nSection [%1]").arg(pgeX_Data.dataTree[section].name);
                goto badfile;
            }

            for(int sdata=0;sdata<pgeX_Data.dataTree[section].data.size();sdata++)
            {
                if(pgeX_Data.dataTree[section].data[sdata].type!=PGEFile::PGEX_Struct)
                {
                    errorString=QString("Wrong data item syntax:\nSection [%1]\nData line %2")
                            .arg(pgeX_Data.dataTree[section].name)
                            .arg(sdata);
                    goto badfile;
                }

                PGEFile::PGEX_Item x = pgeX_Data.dataTree[section].data[sdata];

                Bookmark meta_bookmark;
                meta_bookmark.bookmarkName = "";
                meta_bookmark.x = 0;
                meta_bookmark.y = 0;

                for(int sval=0;sval<x.values.size();sval++) //Look markers and values
                {
                    PGEFile::PGEX_Val v = x.values[sval];
                    errorString=QString("Wrong value syntax\nSection [%1]\nData line %2\nMarker %3\nValue %4")
                            .arg(pgeX_Data.dataTree[section].name)
                            .arg(sdata)
                            .arg(v.marker)
                            .arg(v.value);

                      if(v.marker=="BM") //Bookmark name
                      {
                          if(PGEFile::IsQStr(v.value))
                              meta_bookmark.bookmarkName = PGEFile::X2STR(v.value);
                          else
                              goto badfile;
                      }
                      else
                      if(v.marker=="X") // Position X
                      {
                          if(PGEFile::IsIntS(v.value))
                              meta_bookmark.x = v.value.toInt();
                          else
                              goto badfile;
                      }
                      else
                      if(v.marker=="Y") //Position Y
                      {
                          if(PGEFile::IsIntS(v.value))
                              meta_bookmark.y = v.value.toInt();
                          else
                              goto badfile;
                      }
                }
                FileData.metaData.bookmarks.push_back(meta_bookmark);
            }
        }//meta bookmarks
        #ifdef PGE_EDITOR
        else
        if(pgeX_Data.dataTree[section].name=="META_SCRIPT_EVENTS")
        {
            if(pgeX_Data.dataTree[section].type!=PGEFile::PGEX_Struct)
            {
                errorString=QString("Wrong section data syntax:\nSection [%1]").
                            arg(pgeX_Data.dataTree[section].name);
                goto badfile;
            }

            if(pgeX_Data.dataTree[section].subTree.size()>0)
            {
                if(!FileData.metaData.script)
                    FileData.metaData.script = new ScriptHolder();
            }

            //Read subtree
            for(int subtree=0;subtree<pgeX_Data.dataTree[section].subTree.size();subtree++)
            {
                if(pgeX_Data.dataTree[section].subTree[subtree].name=="EVENT")
                {
                    if(pgeX_Data.dataTree[section].type!=PGEFile::PGEX_Struct)
                    {
                        errorString=QString("Wrong section data syntax:\nSection [%1]\nSubtree [%2]").
                                    arg(pgeX_Data.dataTree[section].name).
                                    arg(pgeX_Data.dataTree[section].subTree[subtree].name);
                        goto badfile;
                    }

                    EventCommand * event = new EventCommand(EventCommand::EVENTTYPE_LOAD);

                    for(int sdata=0;sdata<pgeX_Data.dataTree[section].subTree[subtree].data.size();sdata++)
                    {
                        if(pgeX_Data.dataTree[section].subTree[subtree].data[sdata].type!=PGEFile::PGEX_Struct)
                        {
                            errorString=QString("Wrong data item syntax:\nSubtree [%1]\nData line %2")
                                    .arg(pgeX_Data.dataTree[section].subTree[subtree].name)
                                    .arg(sdata);
                            goto badfile;
                        }

                        PGEFile::PGEX_Item x = pgeX_Data.dataTree[section].subTree[subtree].data[sdata];

                        //Get values
                        for(int sval=0;sval<x.values.size();sval++) //Look markers and values
                        {
                            PGEFile::PGEX_Val v = x.values[sval];
                            errorString=QString("Wrong value syntax\nSection [%1]\nData line %2\nMarker %3\nValue %4")
                                    .arg(pgeX_Data.dataTree[section].name)
                                    .arg(sdata)
                                    .arg(v.marker)
                                    .arg(v.value);

                            if(v.marker=="TL") //Marker
                            {
                                if(PGEFile::IsQStr(v.value))
                                    event->setMarker(PGEFile::X2STR(v.value));
                                else
                                    goto badfile;
                            }
                            else
                            if(v.marker=="ET") //Event type
                            {
                                if(PGEFile::IsIntS(v.value))
                                    event->setEventType( (EventCommand::EventType)v.value.toInt() );
                                else
                                    goto badfile;
                            }
                        }
                    }//Event header

                    //Basic commands subtree
                    if(event->eventType()==EventCommand::EVENTTYPE_LOAD)
                    {
                        for(int subtree2=0;subtree2<pgeX_Data.dataTree[section].subTree[subtree].subTree.size();subtree2++)
                        {
                            if(pgeX_Data.dataTree[section].subTree[subtree2].subTree[subtree2].name=="BASIC_COMMANDS")
                            {
                                if(pgeX_Data.dataTree[section].type!=PGEFile::PGEX_Struct)
                                {
                                    errorString=QString("Wrong section data syntax:\nSection [%1]\nSubtree [%2]\nSubtree [%2]").
                                                arg(pgeX_Data.dataTree[section].name).
                                                arg(pgeX_Data.dataTree[section].subTree[subtree].name).
                                                arg(pgeX_Data.dataTree[section].subTree[subtree].subTree[subtree2].name);
                                    goto badfile;
                                }

                                for(int sdata=0;sdata<pgeX_Data.dataTree[section].subTree[subtree].subTree[subtree2].data.size();sdata++)
                                {
                                    if(pgeX_Data.dataTree[section].subTree[subtree].subTree[subtree2].data[sdata].type!=PGEFile::PGEX_Struct)
                                    {
                                        errorString=QString("Wrong data item syntax:\nSubtree [%1]\nData line %2")
                                                .arg(pgeX_Data.dataTree[section].subTree[subtree].subTree[subtree2].name)
                                                .arg(sdata);
                                        goto badfile;
                                    }

                                    PGEFile::PGEX_Item x = pgeX_Data.dataTree[section].subTree[subtree].subTree[subtree2].data[sdata];

                                    QString name="";
                                    QString commandType="";
                                    int hx=0;
                                    int ft=0;
                                    double vf=0.0;

                                    //Get values
                                    for(int sval=0;sval<x.values.size();sval++) //Look markers and values
                                    {
                                        PGEFile::PGEX_Val v = x.values[sval];
                                        errorString=QString("Wrong value syntax\nSubtree [%1]\nData line %2\nMarker %3\nValue %4")
                                                .arg(pgeX_Data.dataTree[section].subTree[subtree2].name)
                                                .arg(sdata)
                                                .arg(v.marker)
                                                .arg(v.value);

                                        if(v.marker=="N") //Command name
                                        {
                                            if(PGEFile::IsQStr(v.value))
                                                name = PGEFile::X2STR(v.value);
                                            else
                                                goto badfile;
                                        }
                                        else
                                        if(v.marker=="CT") //Command type
                                        {
                                            if(PGEFile::IsQStr(v.value))
                                                commandType = PGEFile::X2STR(v.value);
                                            else
                                                goto badfile;
                                        }


                                        //MemoryCommand specific values
                                        else
                                        if(v.marker=="HX") //Heximal value
                                        {
                                            if(PGEFile::IsIntS(v.value))
                                                hx = v.value.toInt();
                                            else
                                                goto badfile;
                                        }
                                        else
                                        if(v.marker=="FT") //Field type
                                        {
                                            if(PGEFile::IsIntS(v.value))
                                                ft = v.value.toInt();
                                            else
                                                goto badfile;
                                        }
                                        else
                                        if(v.marker=="V") //Value
                                        {
                                            if(PGEFile::IsFloat(v.value))
                                                vf = v.value.toDouble();
                                            else
                                                goto badfile;
                                        }
                                    }

                                    if(commandType=="MEMORY")
                                    {
                                        MemoryCommand *cmd =
                                           new MemoryCommand(hx,(MemoryCommand::FieldType)ft, vf);
                                        cmd->setMarker(name);
                                        event->addBasicCommand(cmd);
                                    }
                                }//commands list
                            }//Basic command subtree
                        }//subtrees
                    }

                    FileData.metaData.script->revents() << event;
                }//EVENT tree

            }//META_SCRIPT_EVENTS subtree
        }//META_SCRIPT_EVENTS Section
        #endif
        ///////////////////////////////MetaDATA//End////////////////////////////////////////

        else ///////////////////SECTION//////////////////////
        if(pgeX_Data.dataTree[section].name=="SECTION")
        {
            if(pgeX_Data.dataTree[section].type!=PGEFile::PGEX_Struct)
            {
                errorString=QString("Wrong section data syntax:\nSection [%1]").arg(pgeX_Data.dataTree[section].name);
                goto badfile;
            }

            for(int sdata=0;sdata<pgeX_Data.dataTree[section].data.size();sdata++)
            {
                if(pgeX_Data.dataTree[section].data[sdata].type!=PGEFile::PGEX_Struct)
                {
                    errorString=QString("Wrong data item syntax:\nSection [%1]\nData line %2")
                            .arg(pgeX_Data.dataTree[section].name)
                            .arg(sdata);
                    goto badfile;
                }
                PGEFile::PGEX_Item x = pgeX_Data.dataTree[section].data[sdata];
                lvl_section = dummyLvlSection();

                for(int sval=0;sval<x.values.size();sval++) //Look markers and values
                {
                    PGEFile::PGEX_Val v = x.values[sval];
                    errorString=QString("Wrong value syntax\nSection [%1]\nData line %2\nMarker %3\nValue %4")
                            .arg(pgeX_Data.dataTree[section].name)
                            .arg(sdata)
                            .arg(v.marker)
                            .arg(v.value);

                    if(v.marker=="SC") //Section ID
                    {
                        if(PGEFile::IsIntU(v.value))
                            lvl_section.id = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="L") //Left side
                    {
                        if(PGEFile::IsIntS(v.value))
                        {
                            lvl_section.size_left= v.value.toInt();
                            lvl_section.PositionX=v.value.toInt()-10;
                        }
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="R")//Right side
                    {
                        if(PGEFile::IsIntS(v.value))
                            lvl_section.size_right= v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="T") //Top side
                    {
                        if(PGEFile::IsIntS(v.value))
                        {
                            lvl_section.size_top= v.value.toInt();
                            lvl_section.PositionY=v.value.toInt()-10;
                        }
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="B")//Bottom side
                    {
                        if(PGEFile::IsIntS(v.value))
                            lvl_section.size_bottom= v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="MZ")//Stuff music ID
                    {
                        if(PGEFile::IsIntU(v.value))
                            lvl_section.music_id= v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="BG")//Stuff music ID
                    {
                        if(PGEFile::IsIntU(v.value))
                            lvl_section.background= v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="MF")//External music file path
                    {
                        if(PGEFile::IsQStr(v.value))
                            lvl_section.music_file=PGEFile::X2STR(v.value);
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="CS")//Connect sides
                    {
                        if(PGEFile::IsBool(v.value))
                            lvl_section.IsWarp=(bool)v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="OE")//Offscreen exit
                    {
                        if(PGEFile::IsBool(v.value))
                            lvl_section.OffScreenEn=(bool)v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="SR")//Right-way scroll only (No Turn-back)
                    {
                        if(PGEFile::IsBool(v.value))
                            lvl_section.noback=(bool)v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="UW")//Underwater bit
                    {
                        if(PGEFile::IsBool(v.value))
                            lvl_section.underwater=(bool)v.value.toInt();
                        else
                            goto badfile;
                    }
                }

                //add captured value into array
                bool found=false;
                int q=0;

                if(lvl_section.id >= FileData.sections.size())
                {
                    int needToAdd = (FileData.sections.size()-1) - lvl_section.id;
                    while(needToAdd > 0)
                    {
                        LevelSection dummySct = dummyLvlSection();
                        dummySct.id = FileData.sections.size();
                        FileData.sections.push_back(dummySct);
                        needToAdd--;
                    }
                }

                for(q=0; q<FileData.sections.size();q++)
                {
                    if(FileData.sections[q].id==lvl_section.id){found=true; break;}
                }
                if(found)
                    FileData.sections[q] = lvl_section;
                else
                {
                    FileData.sections.push_back(lvl_section);
                }
            }
        }//SECTION
        else ///////////////////STARTPOINT//////////////////////
        if(pgeX_Data.dataTree[section].name=="STARTPOINT")
        {
            if(pgeX_Data.dataTree[section].type!=PGEFile::PGEX_Struct)
            {
                errorString=QString("Wrong section data syntax:\nSection [%1]").arg(pgeX_Data.dataTree[section].name);
                goto badfile;
            }

            for(int sdata=0;sdata<pgeX_Data.dataTree[section].data.size();sdata++)
            {
                if(pgeX_Data.dataTree[section].data[sdata].type!=PGEFile::PGEX_Struct)
                {
                    errorString=QString("Wrong data item syntax:\nSection [%1]\nData line %2")
                            .arg(pgeX_Data.dataTree[section].name)
                            .arg(sdata);
                    goto badfile;
                }
                PGEFile::PGEX_Item x = pgeX_Data.dataTree[section].data[sdata];
                player = dummyLvlPlayerPoint();

                for(int sval=0;sval<x.values.size();sval++) //Look markers and values
                {
                    PGEFile::PGEX_Val v = x.values[sval];
                    errorString=QString("Wrong value syntax\nSection [%1]\nData line %2\nMarker %3\nValue %4")
                            .arg(pgeX_Data.dataTree[section].name)
                            .arg(sdata)
                            .arg(v.marker)
                            .arg(v.value);

                    if(v.marker=="ID") //ID of player point
                    {
                        if(PGEFile::IsIntU(v.value))
                            player.id = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="X")
                    {
                        if(PGEFile::IsIntS(v.value))
                            player.x = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="Y")
                    {
                        if(PGEFile::IsIntS(v.value))
                            player.y = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="D")
                    {
                        if(PGEFile::IsIntS(v.value))
                            player.direction = v.value.toInt();
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
            }
        }//STARTPOINT
        else ///////////////////BLOCK//////////////////////
        if(pgeX_Data.dataTree[section].name=="BLOCK")
        {
            if(pgeX_Data.dataTree[section].type!=PGEFile::PGEX_Struct)
            {
                errorString=QString("Wrong section data syntax:\nSection [%1]").arg(pgeX_Data.dataTree[section].name);
                goto badfile;
            }

            for(int sdata=0;sdata<pgeX_Data.dataTree[section].data.size();sdata++)
            {
                if(pgeX_Data.dataTree[section].data[sdata].type!=PGEFile::PGEX_Struct)
                {
                    errorString=QString("Wrong data item syntax:\nSection [%1]\nData line %2")
                            .arg(pgeX_Data.dataTree[section].name)
                            .arg(sdata);
                    goto badfile;
                }
                PGEFile::PGEX_Item x = pgeX_Data.dataTree[section].data[sdata];
                block = dummyLvlBlock();

                for(int sval=0;sval<x.values.size();sval++) //Look markers and values
                {
                    PGEFile::PGEX_Val v = x.values[sval];
                    errorString=QString("Wrong value syntax\nSection [%1]\nData line %2\nMarker %3\nValue %4")
                            .arg(pgeX_Data.dataTree[section].name)
                            .arg(sdata)
                            .arg(v.marker)
                            .arg(v.value);

                    if(v.marker=="ID") //Block ID
                    {
                        if(PGEFile::IsIntU(v.value))
                            block.id = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="X") // Position X
                    {
                        if(PGEFile::IsIntS(v.value))
                            block.x = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="Y") //Position Y
                    {
                        if(PGEFile::IsIntS(v.value))
                            block.y = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="W") //Width
                    {
                        if(PGEFile::IsIntU(v.value))
                            block.w = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="H") //Height
                    {
                        if(PGEFile::IsIntU(v.value))
                            block.h = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="CN") //Contains (coins/NPC)
                    {
                        if(PGEFile::IsIntS(v.value))
                            block.npc_id = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="IV") //Invisible
                    {
                        if(PGEFile::IsBool(v.value))
                            block.invisible = (bool)v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="SL") //Slippery
                    {
                        if(PGEFile::IsBool(v.value))
                            block.slippery = (bool)v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="LR") //Layer name
                    {
                        if(PGEFile::IsQStr(v.value))
                            block.layer = PGEFile::X2STR(v.value);
                        else
                            goto badfile;
                    }
                }

                block.array_id = FileData.blocks_array_id++;
                block.index = FileData.blocks.size();
                FileData.blocks.push_back(block);
            }
        }//BLOCK
        else ///////////////////BGO//////////////////////
        if(pgeX_Data.dataTree[section].name=="BGO")
        {
            if(pgeX_Data.dataTree[section].type!=PGEFile::PGEX_Struct)
            {
                errorString=QString("Wrong section data syntax:\nSection [%1]").arg(pgeX_Data.dataTree[section].name);
                goto badfile;
            }

            for(int sdata=0;sdata<pgeX_Data.dataTree[section].data.size();sdata++)
            {
                if(pgeX_Data.dataTree[section].data[sdata].type!=PGEFile::PGEX_Struct)
                {
                    errorString=QString("Wrong data item syntax:\nSection [%1]\nData line %2")
                            .arg(pgeX_Data.dataTree[section].name)
                            .arg(sdata);
                    goto badfile;
                }
                PGEFile::PGEX_Item x = pgeX_Data.dataTree[section].data[sdata];
                bgodata = dummyLvlBgo();

                for(int sval=0;sval<x.values.size();sval++) //Look markers and values
                {
                    PGEFile::PGEX_Val v = x.values[sval];
                    errorString=QString("Wrong value syntax\nSection [%1]\nData line %2\nMarker %3\nValue %4")
                            .arg(pgeX_Data.dataTree[section].name)
                            .arg(sdata)
                            .arg(v.marker)
                            .arg(v.value);

                    if(v.marker=="ID") //BGO ID
                    {
                        if(PGEFile::IsIntU(v.value))
                            bgodata.id = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="X") //X Position
                    {
                        if(PGEFile::IsIntS(v.value))
                            bgodata.x = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="Y") //Y Position
                    {
                        if(PGEFile::IsIntS(v.value))
                            bgodata.y = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="ZO") //Z Offset
                    {
                        if(PGEFile::IsFloat(v.value))
                            bgodata.z_offset = v.value.toDouble();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="ZP") //Z Position
                    {
                        if(PGEFile::IsIntS(v.value))
                            bgodata.z_mode = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="SP") //SMBX64 Sorting priority
                    {
                        if(PGEFile::IsIntS(v.value))
                            bgodata.smbx64_sp = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="LR") //Layer name
                    {
                        if(PGEFile::IsQStr(v.value))
                            bgodata.layer = PGEFile::X2STR(v.value);
                        else
                            goto badfile;
                    }
                }

                bgodata.array_id = FileData.bgo_array_id++;
                bgodata.index = FileData.bgo.size();
                FileData.bgo.push_back(bgodata);
            }
        }//BGO
        else ///////////////////NPC//////////////////////
        if(pgeX_Data.dataTree[section].name=="NPC")
        {
            if(pgeX_Data.dataTree[section].type!=PGEFile::PGEX_Struct)
            {
                errorString=QString("Wrong section data syntax:\nSection [%1]").arg(pgeX_Data.dataTree[section].name);
                goto badfile;
            }

            for(int sdata=0;sdata<pgeX_Data.dataTree[section].data.size();sdata++)
            {
                if(pgeX_Data.dataTree[section].data[sdata].type!=PGEFile::PGEX_Struct)
                {
                    errorString=QString("Wrong data item syntax:\nSection [%1]\nData line %2")
                            .arg(pgeX_Data.dataTree[section].name)
                            .arg(sdata);
                    goto badfile;
                }
                PGEFile::PGEX_Item x = pgeX_Data.dataTree[section].data[sdata];
                npcdata = dummyLvlNpc();

                for(int sval=0;sval<x.values.size();sval++) //Look markers and values
                {
                    PGEFile::PGEX_Val v = x.values[sval];
                    errorString=QString("Wrong value syntax\nSection [%1]\nData line %2\nMarker %3\nValue %4")
                            .arg(pgeX_Data.dataTree[section].name)
                            .arg(sdata)
                            .arg(v.marker)
                            .arg(v.value);

                    if(v.marker=="ID") //NPC ID
                    {
                        if(PGEFile::IsIntU(v.value))
                            npcdata.id = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="X") //X position
                    {
                        if(PGEFile::IsIntS(v.value))
                            npcdata.x = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="Y") //Y position
                    {
                        if(PGEFile::IsIntS(v.value))
                            npcdata.y = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="D") //Direction
                    {
                        if(PGEFile::IsIntS(v.value))
                            npcdata.direct = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="S1") //Special value 1
                    {
                        if(PGEFile::IsIntS(v.value))
                            npcdata.special_data = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="S2") //Special value 2
                    {
                        if(PGEFile::IsIntS(v.value))
                            npcdata.special_data2 = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="GE") //Generator
                    {
                        if(PGEFile::IsBool(v.value))
                            npcdata.generator = (bool)v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="GT") //Generator type
                    {
                        if(PGEFile::IsIntS(v.value))
                            npcdata.generator = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="GD") //Generator direction
                    {
                        if(PGEFile::IsIntS(v.value))
                            npcdata.generator_direct = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="GM") //Generator period
                    {
                        if(PGEFile::IsIntU(v.value))
                            npcdata.generator_period = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="MG") //Message
                    {
                        if(PGEFile::IsQStr(v.value))
                            npcdata.msg = PGEFile::X2STR(v.value);
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="FD") //Friendly
                    {
                        if(PGEFile::IsBool(v.value))
                            npcdata.friendly = (bool)v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="NM") //Don't move
                    {
                        if(PGEFile::IsBool(v.value))
                            npcdata.nomove = (bool)v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="BS") //Boss algorithm
                    {
                        if(PGEFile::IsBool(v.value))
                            npcdata.legacyboss = (bool)v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="LR") //Layer
                    {
                        if(PGEFile::IsQStr(v.value))
                            npcdata.layer = PGEFile::X2STR(v.value);
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="LA") //Attach Layer
                    {
                        if(PGEFile::IsQStr(v.value))
                            npcdata.attach_layer = PGEFile::X2STR(v.value);
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="EA") //Event slot "Activated"
                    {
                        if(PGEFile::IsQStr(v.value))
                            npcdata.event_activate = PGEFile::X2STR(v.value);
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="ED") //Event slot "Death/Take/Destroy"
                    {
                        if(PGEFile::IsQStr(v.value))
                            npcdata.event_die = PGEFile::X2STR(v.value);
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="ET") //Event slot "Talk"
                    {
                        if(PGEFile::IsQStr(v.value))
                            npcdata.event_talk = PGEFile::X2STR(v.value);
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="EE") //Event slot "Layer is empty"
                    {
                        if(PGEFile::IsQStr(v.value))
                            npcdata.event_nomore = PGEFile::X2STR(v.value);
                        else
                            goto badfile;
                    }
                }

                npcdata.array_id = FileData.npc_array_id++;
                npcdata.index = FileData.npc.size();
                FileData.npc.push_back(npcdata);
            }
        }//TILES
        else ///////////////////PHYSICS//////////////////////
        if(pgeX_Data.dataTree[section].name=="PHYSICS")
        {
            if(pgeX_Data.dataTree[section].type!=PGEFile::PGEX_Struct)
            {
                errorString=QString("Wrong section data syntax:\nSection [%1]").arg(pgeX_Data.dataTree[section].name);
                goto badfile;
            }

            for(int sdata=0;sdata<pgeX_Data.dataTree[section].data.size();sdata++)
            {
                if(pgeX_Data.dataTree[section].data[sdata].type!=PGEFile::PGEX_Struct)
                {
                    errorString=QString("Wrong data item syntax:\nSection [%1]\nData line %2")
                            .arg(pgeX_Data.dataTree[section].name)
                            .arg(sdata);
                    goto badfile;
                }
                PGEFile::PGEX_Item x = pgeX_Data.dataTree[section].data[sdata];
                physiczone = dummyLvlPhysEnv();

                for(int sval=0;sval<x.values.size();sval++) //Look markers and values
                {
                    PGEFile::PGEX_Val v = x.values[sval];
                    errorString=QString("Wrong value syntax\nSection [%1]\nData line %2\nMarker %3\nValue %4")
                            .arg(pgeX_Data.dataTree[section].name)
                            .arg(sdata)
                            .arg(v.marker)
                            .arg(v.value);

                    if(v.marker=="ET") //Environment type
                    {
                        if(PGEFile::IsIntU(v.value))
                            physiczone.quicksand = (bool)v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="X") //X position
                    {
                        if(PGEFile::IsIntS(v.value))
                            physiczone.x = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="Y") //Y position
                    {
                        if(PGEFile::IsIntS(v.value))
                            physiczone.y = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="W") //Width
                    {
                        if(PGEFile::IsIntU(v.value))
                            physiczone.w = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="H") //Height
                    {
                        if(PGEFile::IsIntU(v.value))
                            physiczone.h = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="LR") //Layer
                    {
                        if(PGEFile::IsQStr(v.value))
                            physiczone.layer = PGEFile::X2STR(v.value);
                        else
                            goto badfile;
                    }
                }

                physiczone.array_id = FileData.physenv_array_id++;
                physiczone.index = FileData.physez.size();
                FileData.physez.push_back(physiczone);
            }
        }//PHYSICS
        else ///////////////////DOORS//////////////////////
        if(pgeX_Data.dataTree[section].name=="DOORS")
        {
            if(pgeX_Data.dataTree[section].type!=PGEFile::PGEX_Struct)
            {
                errorString=QString("Wrong section data syntax:\nSection [%1]").arg(pgeX_Data.dataTree[section].name);
                goto badfile;
            }

            for(int sdata=0;sdata<pgeX_Data.dataTree[section].data.size();sdata++)
            {
                if(pgeX_Data.dataTree[section].data[sdata].type!=PGEFile::PGEX_Struct)
                {
                    errorString=QString("Wrong data item syntax:\nSection [%1]\nData line %2")
                            .arg(pgeX_Data.dataTree[section].name)
                            .arg(sdata);
                    goto badfile;
                }
                PGEFile::PGEX_Item x = pgeX_Data.dataTree[section].data[sdata];
                door = dummyLvlDoor();

                for(int sval=0;sval<x.values.size();sval++) //Look markers and values
                {
                    PGEFile::PGEX_Val v = x.values[sval];
                    errorString=QString("Wrong value syntax\nSection [%1]\nData line %2\nMarker %3\nValue %4")
                            .arg(pgeX_Data.dataTree[section].name)
                            .arg(sdata)
                            .arg(v.marker)
                            .arg(v.value);

                    if(v.marker=="IX") //Input point
                    {
                      if(PGEFile::IsIntS(v.value))
                          door.ix = v.value.toInt();
                      else
                          goto badfile;
                    }
                    else
                    if(v.marker=="IY") //Input point
                    {
                      if(PGEFile::IsIntS(v.value))
                          door.iy = v.value.toInt();
                      else
                          goto badfile;
                    }
                    else
                    if(v.marker=="OX") //Output point
                    {
                      if(PGEFile::IsIntS(v.value))
                          door.ox = v.value.toInt();
                      else
                          goto badfile;
                    }
                    else
                    if(v.marker=="OY") //Output point
                    {
                      if(PGEFile::IsIntS(v.value))
                          door.oy = v.value.toInt();
                      else
                          goto badfile;
                    }
                    else
                    if(v.marker=="DT") //Input point
                    {
                      if(PGEFile::IsIntU(v.value))
                          door.type = v.value.toInt();
                      else
                          goto badfile;
                    }
                    else
                    if(v.marker=="ID") //Input direction
                    {
                      if(PGEFile::IsIntU(v.value))
                          door.idirect = v.value.toInt();
                      else
                          goto badfile;
                    }
                    else
                    if(v.marker=="OD") //Output direction
                    {
                      if(PGEFile::IsIntU(v.value))
                          door.odirect = v.value.toInt();
                      else
                          goto badfile;
                    }
                    else
                    if(v.marker=="WX") //Target world map point
                    {
                      if(PGEFile::IsIntS(v.value))
                          door.world_x = v.value.toInt();
                      else
                          goto badfile;
                    }
                    else
                    if(v.marker=="WY") //Target world map point
                    {
                      if(PGEFile::IsIntS(v.value))
                          door.world_y = v.value.toInt();
                      else
                          goto badfile;
                    }
                    else
                    if(v.marker=="LF") //Target level file
                    {
                      if(PGEFile::IsQStr(v.value))
                          door.lname = PGEFile::X2STR(v.value);
                      else
                          goto badfile;
                    }
                    else
                    if(v.marker=="LI") //Target level file's input warp
                    {
                      if(PGEFile::IsIntU(v.value))
                          door.warpto = v.value.toInt();
                      else
                          goto badfile;
                    }
                    else
                    if(v.marker=="ET") //Level Entrance
                    {
                      if(PGEFile::IsBool(v.value))
                          door.lvl_i = (bool)v.value.toInt();
                      else
                          goto badfile;
                    }
                    else
                    if(v.marker=="EX") //Level exit
                    {
                      if(PGEFile::IsBool(v.value))
                          door.lvl_o = (bool)v.value.toInt();
                      else
                          goto badfile;
                    }
                    else
                    if(v.marker=="SL") //Stars limit
                    {
                      if(PGEFile::IsIntU(v.value))
                          door.stars = v.value.toInt();
                      else
                          goto badfile;
                    }
                    else
                    if(v.marker=="NV") //No Vehicles
                    {
                      if(PGEFile::IsBool(v.value))
                          door.novehicles = (bool)v.value.toInt();
                      else
                          goto badfile;
                    }
                    else
                    if(v.marker=="AI") //Allow grabbed items
                    {
                      if(PGEFile::IsBool(v.value))
                          door.allownpc = (bool)v.value.toInt();
                      else
                          goto badfile;
                    }
                    else
                    if(v.marker=="LC") //Door is locked
                    {
                      if(PGEFile::IsBool(v.value))
                          door.locked = (bool)v.value.toInt();
                      else
                          goto badfile;
                    }
                    else
                    if(v.marker=="LR") //Layer
                    {
                      if(PGEFile::IsQStr(v.value))
                          door.layer = PGEFile::X2STR(v.value);
                      else
                          goto badfile;
                    }
                }

                door.isSetIn = ( !door.lvl_i );
                door.isSetOut = ( !door.lvl_o || (door.lvl_i));

                door.array_id = FileData.doors_array_id++;
                door.index = FileData.doors.size();
                FileData.doors.push_back(door);
            }
        }//DOORS

        else ///////////////////LAYERS//////////////////////
        if(pgeX_Data.dataTree[section].name=="LAYERS")
        {
            if(pgeX_Data.dataTree[section].type!=PGEFile::PGEX_Struct)
            {
                errorString=QString("Wrong section data syntax:\nSection [%1]").arg(pgeX_Data.dataTree[section].name);
                goto badfile;
            }

            for(int sdata=0;sdata<pgeX_Data.dataTree[section].data.size();sdata++)
            {
                if(pgeX_Data.dataTree[section].data[sdata].type!=PGEFile::PGEX_Struct)
                {
                    errorString=QString("Wrong data item syntax:\nSection [%1]\nData line %2")
                            .arg(pgeX_Data.dataTree[section].name)
                            .arg(sdata);
                    goto badfile;
                }
                PGEFile::PGEX_Item x = pgeX_Data.dataTree[section].data[sdata];
                layer = dummyLvlLayer();

                for(int sval=0;sval<x.values.size();sval++) //Look markers and values
                {
                    PGEFile::PGEX_Val v = x.values[sval];
                    errorString=QString("Wrong value syntax\nSection [%1]\nData line %2\nMarker %3\nValue %4")
                            .arg(pgeX_Data.dataTree[section].name)
                            .arg(sdata)
                            .arg(v.marker)
                            .arg(v.value);

                    if(v.marker=="LR") //Layer name
                    {
                        if(PGEFile::IsQStr(v.value))
                            layer.name = PGEFile::X2STR(v.value);
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="HD") //Hidden
                    {
                        if(PGEFile::IsIntU(v.value))
                            layer.hidden = (bool)v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="LC") //Locked
                    {
                        if(PGEFile::IsIntU(v.value))
                            layer.locked = (bool)v.value.toInt();
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
            }
        }//LAYERS

        //EVENTS comming soon
//                else
//                if(sct.first=="EVENTS_CLASSIC") //Action-styled events
//                {
//                    foreach(QStringList value, sectData) //Look markers and values
//                    {
//                            //  if(v.marker=="TL") //Level Title
//                            //  {
//                            //      if(PGEFile::IsQStr(v.value))
//                            //          FileData.LevelName = PGEFile::X2STR(v.value);
//                            //      else
//                            //          goto badfile;
//                            //  }
//                            //  else
//                            //  if(v.marker=="SZ") //Starz number
//                            //  {
//                            //      if(PGEFile::IsIntU(v.value))
//                            //          FileData.stars = v.value.toInt();
//                            //      else
//                            //          goto badfile;
//                            //  }
//                    }
//                }//EVENTS

        else ///////////////////EVENTS_CLASSIC//////////////////////
        if(pgeX_Data.dataTree[section].name=="EVENTS_CLASSIC")
        {
            if(pgeX_Data.dataTree[section].type!=PGEFile::PGEX_Struct)
            {
                errorString=QString("Wrong section data syntax:\nSection [%1]").arg(pgeX_Data.dataTree[section].name);
                goto badfile;
            }

            for(int sdata=0;sdata<pgeX_Data.dataTree[section].data.size();sdata++)
            {
                if(pgeX_Data.dataTree[section].data[sdata].type!=PGEFile::PGEX_Struct)
                {
                    errorString=QString("Wrong data item syntax:\nSection [%1]\nData line %2")
                            .arg(pgeX_Data.dataTree[section].name)
                            .arg(sdata);
                    goto badfile;
                }
                PGEFile::PGEX_Item x = pgeX_Data.dataTree[section].data[sdata];
                event = dummyLvlEvent();

                for(int sval=0;sval<x.values.size();sval++) //Look markers and values
                {
                    PGEFile::PGEX_Val v = x.values[sval];
                    errorString=QString("Wrong value syntax\nSection [%1]\nData line %2\nMarker %3\nValue %4")
                            .arg(pgeX_Data.dataTree[section].name)
                            .arg(sdata)
                            .arg(v.marker)
                            .arg(v.value);

                    if(v.marker=="ET") //Event Title
                    {
                        if(PGEFile::IsQStr(v.value))
                            event.name = PGEFile::X2STR(v.value);
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="MG") //Event Message
                    {
                        if(PGEFile::IsQStr(v.value))
                            event.msg = PGEFile::X2STR(v.value);
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="SD") //Play Sound ID
                    {
                        if(PGEFile::IsIntU(v.value))
                            event.sound_id = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="EG") //End game algorithm
                    {
                        if(PGEFile::IsIntU(v.value))
                            event.end_game = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="LH") //Hide layers
                    {
                        if(PGEFile::IsStringArray(v.value))
                            event.layers_hide = PGEFile::X2STRArr(v.value);
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="LS") //Show layers
                    {
                        if(PGEFile::IsStringArray(v.value))
                            event.layers_show = PGEFile::X2STRArr(v.value);
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="LT") //Toggle layers
                    {
                        if(PGEFile::IsStringArray(v.value))
                            event.layers_toggle = PGEFile::X2STRArr(v.value);
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="SM") //Switch music
                    {
                        if(PGEFile::IsStringArray(v.value))
                        {
                            QStringList musicSets = PGEFile::X2STRArr(v.value);
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
                    if(v.marker=="SB") //Switch background
                    {
                        if(PGEFile::IsStringArray(v.value))
                        {
                            QStringList bgSets = PGEFile::X2STRArr(v.value);
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
                    if(v.marker=="SS") //Section Size
                    {
                        if(PGEFile::IsStringArray(v.value))
                        {
                            QStringList bgSets = PGEFile::X2STRArr(v.value);
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
                    if(v.marker=="TE") //Trigger event
                    {
                        if(PGEFile::IsQStr(v.value))
                            event.trigger = PGEFile::X2STR(v.value);
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="TD") //Trigger delay
                    {
                        if(PGEFile::IsIntU(v.value))
                            event.trigger_timer = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="DS") //Disable smoke
                    {
                        if(PGEFile::IsBool(v.value))
                            event.nosmoke = (bool)v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="AU") //Auto start
                    {
                        if(PGEFile::IsBool(v.value))
                            event.autostart = (bool)v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="AU") //Auto start
                    {
                        if(PGEFile::IsBool(v.value))
                            event.autostart = (bool)v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="PC") //Player controls
                    {
                        if(PGEFile::IsBoolArray(v.value))
                        {
                            QList<bool > controls = PGEFile::X2BollArr(v.value);
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
                    if(v.marker=="ML") //Move layer
                    {
                        if(PGEFile::IsQStr(v.value))
                            event.movelayer = PGEFile::X2STR(v.value);
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="MX") //Layer motion speed X
                    {
                        if(PGEFile::IsFloat(v.value))
                            event.layer_speed_x = v.value.toDouble();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="MY") //Layer motion speed Y
                    {
                        if(PGEFile::IsFloat(v.value))
                            event.layer_speed_y = v.value.toDouble();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="AS") //Autoscroll section ID
                    {
                        if(PGEFile::IsIntS(v.value))
                            event.scroll_section = v.value.toInt();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="AX") //Autoscroll speed X
                    {
                        if(PGEFile::IsFloat(v.value))
                            event.move_camera_x = v.value.toDouble();
                        else
                            goto badfile;
                    }
                    else
                    if(v.marker=="AY") //Autoscroll speed Y
                    {
                        if(PGEFile::IsFloat(v.value))
                            event.move_camera_y = v.value.toDouble();
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
            }
        }//EVENTS_CLASSIC
    }
    ///////////////////////////////////////EndFile///////////////////////////////////////

    errorString.clear(); //If no errors, clear string;
    FileData.ReadFileValid=true;
    return FileData;

    badfile:    //If file format is not correct
    if(!sielent)
        BadFileMsg(filePath+"\nError message: "+errorString, str_count, line);
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
    if( (!FileData.LevelName.isEmpty())||(FileData.stars>0) )
    {
        TextData += "HEAD\n";
        TextData += PGEFile::value("TL", PGEFile::qStrS(FileData.LevelName)); // Level title
        TextData += PGEFile::value("SZ", PGEFile::IntS(FileData.stars));      // Stars number
        TextData += "\n";
        TextData += "HEAD_END\n";
    }

    //////////////////////////////////////MetaData////////////////////////////////////////////////
    //Bookmarks
    if(!FileData.metaData.bookmarks.isEmpty())
    {
        TextData += "META_BOOKMARKS\n";
        for(i=0;i<FileData.metaData.bookmarks.size(); i++)
        {
            //Bookmark name
            TextData += PGEFile::value("BM", PGEFile::qStrS(FileData.metaData.bookmarks[i].bookmarkName));
            TextData += PGEFile::value("X", PGEFile::IntS(FileData.metaData.bookmarks[i].x));
            TextData += PGEFile::value("Y", PGEFile::IntS(FileData.metaData.bookmarks[i].y));
            TextData += "\n";
        }
        TextData += "META_BOOKMARKS_END\n";

        #ifdef PGE_EDITOR
        if(FileData.metaData.script)
        {
            if(!FileData.metaData.script->events().isEmpty())
            {
                TextData += "META_SCRIPT_EVENTS\n";
                foreach(EventCommand* x, FileData.metaData.script->events())
                {
                    TextData += "EVENT\n";
                    TextData += PGEFile::value("TL", PGEFile::qStrS( x->marker() ) );
                    TextData += PGEFile::value("ET", PGEFile::IntS( (int)x->eventType() ) );
                    TextData += "\n";

                    if(x->basicCommands().size()>0)
                    {
                        TextData += "BASIC_COMMANDS\n";
                        foreach(BasicCommand *y, x->basicCommands())
                        {
                            TextData += PGEFile::value("N", PGEFile::qStrS( y->marker() ) );
                            if(QString(y->metaObject()->className())=="MemoryCommand")
                            {
                                MemoryCommand *z = dynamic_cast<MemoryCommand*>(y);
                                TextData += PGEFile::value("CT", PGEFile::qStrS( "MEMORY" ) );
                                TextData += PGEFile::value("HX", PGEFile::IntS( z->hexValue() ) );
                                TextData += PGEFile::value("FT", PGEFile::IntS( (int)z->fieldType() ) );
                                TextData += PGEFile::value("V", PGEFile::FloatS( z->getValue() ) );
                            }
                            TextData += "\n";
                        }
                        TextData += "BASIC_COMMANDS_END\n";
                    }
                    TextData += "EVENT_END\n";
                }
                TextData += "META_SCRIPT_EVENTS_END\n";
            }
        }
        #endif
    }
    //////////////////////////////////////MetaData///END//////////////////////////////////////////


    //SECTION section

    //Count available level sections
    int totalSections=0;
    for(i=0; i< FileData.sections.size(); i++)
    {
        if(
                (FileData.sections[i].size_bottom==0) &&
                (FileData.sections[i].size_left==0) &&
                (FileData.sections[i].size_right==0) &&
                (FileData.sections[i].size_top==0)
           )
            continue; //Skip unitialized sections
        totalSections++;
    }

    //Don't store section data entry if no data to add
    if(totalSections>0)
    {
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
    }

    //STARTPOINT section
    int totalPlayerPoints=0;
    for(i=0; i< FileData.players.size(); i++)
    {
        if((FileData.players[i].w==0)&&
           (FileData.players[i].h==0))
            continue; //Skip empty points
        totalPlayerPoints++;
    }

    //Don't store section data entry if no data to add
    if(totalPlayerPoints>0)
    {
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
    }

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
