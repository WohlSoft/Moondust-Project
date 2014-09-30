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

WorldData FileFormats::ReadExtendedWorldFile(QFile &inf)
{
     QTextStream in(&inf);   //Read File
     in.setCodec("UTF-8");

     int str_count=0;        //Line Counter
     int i;                  //counters
     QString line;           //Current Line data

     WorldData FileData = dummyWldDataArray();

     FileData.untitled = false;
     FileData.modified = false;

     WorldTiles tile;
     WorldScenery scen;
     WorldPaths pathitem;
     WorldMusic musicbox;
     WorldLevels lvlitem;


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

         //WriteToLog(QtDebugMsg, QString("Section %1, lines %2, %3")
         //           .arg(PGEXsection.first)
         //           .arg(PGEXsection.second.size())
         //           .arg(sectionOpened?"opened":"closed")
         //           );
     }

     if(sectionOpened)
     {
         errorString=QString("Section [%1] is not closed").arg(PGEXsection.first);
         goto badfile;
     }

     foreach(PGEXSct sct, PGEXTree) //look sections
     {

         //WriteToLog(QtDebugMsg, QString("Section %1")
         //           .arg(sct.first) );

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
                           if(value[0]=="TL") //Episode Title
                           {
                               if(PGEFile::IsQStr(value[1]))
                                   FileData.EpisodeTitle = PGEFile::X2STR(value[1]);
                               else
                                   goto badfile;
                           }
                           else
                           if(value[0]=="DC") //Disabled characters
                           {
                               if(PGEFile::IsBoolArray(value[1]))
                                   FileData.nocharacter = PGEFile::X2BollArr(value[1]);
                               else
                                   goto badfile;
                           }
                           else
                           if(value[0]=="IT") //Intro level
                           {
                               if(PGEFile::IsQStr(value[1]))
                                   FileData.IntroLevel_file = PGEFile::X2STR(value[1]);
                               else
                                   goto badfile;
                           }
                           else
                           if(value[0]=="HB") //Hub Styled
                           {
                               if(PGEFile::IsBool(value[1]))
                                   FileData.HubStyledWorld = (bool)value[1].toInt();
                               else
                                   goto badfile;
                           }
                           else
                           if(value[0]=="RL") //Restart level on fail
                           {
                               if(PGEFile::IsBool(value[1]))
                                   FileData.restartlevel = (bool)value[1].toInt();
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
                           else
                           if(value[0]=="CD") //Credits list
                           {
                               if(PGEFile::IsQStr(value[1]))
                                   FileData.authors = PGEFile::X2STR(value[1]);
                               else
                                   goto badfile;
                           }

                     }
                 }//Header
                 else
                 if(sct.first=="TILES") // TILES
                 {
                     tile = dummyWldTile();
                     foreach(QStringList value, sectData) //Look markers and values
                     {
                               if(value[0]=="ID") //Tile ID
                               {
                                   if(PGEFile::IsIntU(value[1]))
                                       tile.id = value[1].toInt();
                                   else
                                       goto badfile;
                               }
                               else
                               if(value[0]=="X") //X Position
                               {
                                   if(PGEFile::IsIntS(value[1]))
                                       tile.x = value[1].toInt();
                                   else
                                       goto badfile;
                               }
                               else
                               if(value[0]=="Y") //Y Position
                               {
                                   if(PGEFile::IsIntS(value[1]))
                                       tile.y = value[1].toInt();
                                   else
                                       goto badfile;
                               }
                     }

                     tile.array_id = FileData.tile_array_id++;
                     tile.index = FileData.tiles.size();
                     FileData.tiles.push_back(tile);
                 }//TILES

                 else
                 if(sct.first=="SCENERY") // SCENERY
                 {
                     scen = dummyWldScen();
                     foreach(QStringList value, sectData) //Look markers and values
                     {
                               if(value[0]=="ID") //Scenery ID
                               {
                                   if(PGEFile::IsIntU(value[1]))
                                       scen.id = value[1].toInt();
                                   else
                                       goto badfile;
                               }
                               else
                               if(value[0]=="X") //X Position
                               {
                                   if(PGEFile::IsIntS(value[1]))
                                       scen.x = value[1].toInt();
                                   else
                                       goto badfile;
                               }
                               else
                               if(value[0]=="Y") //Y Position
                               {
                                   if(PGEFile::IsIntS(value[1]))
                                       scen.y = value[1].toInt();
                                   else
                                       goto badfile;
                               }
                     }

                     scen.array_id = FileData.scene_array_id++;
                     scen.index = FileData.scenery.size();
                     FileData.scenery.push_back(scen);
                 }//SCENERY

                 else
                 if(sct.first=="PATHS") // SCENERY
                 {
                     pathitem = dummyWldPath();
                     foreach(QStringList value, sectData) //Look markers and values
                     {
                               if(value[0]=="ID") //PATHS ID
                               {
                                   if(PGEFile::IsIntU(value[1]))
                                       pathitem.id = value[1].toInt();
                                   else
                                       goto badfile;
                               }
                               else
                               if(value[0]=="X") //X Position
                               {
                                   if(PGEFile::IsIntS(value[1]))
                                       pathitem.x = value[1].toInt();
                                   else
                                       goto badfile;
                               }
                               else
                               if(value[0]=="Y") //Y Position
                               {
                                   if(PGEFile::IsIntS(value[1]))
                                       pathitem.y = value[1].toInt();
                                   else
                                       goto badfile;
                               }
                     }

                     pathitem.array_id = FileData.path_array_id++;
                     pathitem.index = FileData.paths.size();
                     FileData.paths.push_back(pathitem);
                 }//PATHS

                 else
                 if(sct.first=="MUSICBOXES") // MUSICBOXES
                 {
                     musicbox = dummyWldMusic();
                     foreach(QStringList value, sectData) //Look markers and values
                     {
                               if(value[0]=="ID") //MISICBOX ID
                               {
                                   if(PGEFile::IsIntU(value[1]))
                                       musicbox.id = value[1].toInt();
                                   else
                                       goto badfile;
                               }
                               else
                               if(value[0]=="X") //X Position
                               {
                                   if(PGEFile::IsIntS(value[1]))
                                       musicbox.x = value[1].toInt();
                                   else
                                       goto badfile;
                               }
                               else
                               if(value[0]=="Y") //Y Position
                               {
                                   if(PGEFile::IsIntS(value[1]))
                                       musicbox.y = value[1].toInt();
                                   else
                                       goto badfile;
                               }
                               else
                               if(value[0]=="MF") //Custom music file
                               {
                                   if(PGEFile::IsQStr(value[1]))
                                       musicbox.music_file = PGEFile::X2STR(value[1]);
                                   else
                                       goto badfile;
                               }
                     }

                     musicbox.array_id = FileData.musicbox_array_id++;
                     musicbox.index = FileData.music.size();
                     FileData.music.push_back(musicbox);
                 }//MUSICBOXES

                 else
                 if(sct.first=="LEVELS") // LEVELS
                 {
                     lvlitem = dummyWldLevel();
                     foreach(QStringList value, sectData) //Look markers and values
                     {
                               if(value[0]=="ID") //LEVEL IMAGE ID
                               {
                                   if(PGEFile::IsIntU(value[1]))
                                       lvlitem.id = value[1].toInt();
                                   else
                                       goto badfile;
                               }
                               else
                               if(value[0]=="X") //X Position
                               {
                                   if(PGEFile::IsIntS(value[1]))
                                       lvlitem.x = value[1].toInt();
                                   else
                                       goto badfile;
                               }
                               else
                               if(value[0]=="Y") //Y Position
                               {
                                   if(PGEFile::IsIntS(value[1]))
                                       lvlitem.y = value[1].toInt();
                                   else
                                       goto badfile;
                               }
                               else
                               if(value[0]=="LF") //Target level file
                               {
                                   if(PGEFile::IsQStr(value[1]))
                                       lvlitem.lvlfile = PGEFile::X2STR(value[1]);
                                   else
                                       goto badfile;
                               }
                               else
                               if(value[0]=="LT") //Level title
                               {
                                   if(PGEFile::IsQStr(value[1]))
                                       lvlitem.title = PGEFile::X2STR(value[1]);
                                   else
                                       goto badfile;
                               }
                               else
                               if(value[0]=="EI") //Entrance Warp ID (if 0 - start level from default points)
                               {
                                   if(PGEFile::IsIntU(value[1]))
                                       lvlitem.title = value[1].toInt();
                                   else
                                       goto badfile;
                               }
                               else
                               if(value[0]=="ET") //Open top path on exit type
                               {
                                   if(PGEFile::IsIntS(value[1]))
                                       lvlitem.top_exit = value[1].toInt();
                                   else
                                       goto badfile;
                               }
                               else
                               if(value[0]=="EL") //Open left path on exit type
                               {
                                   if(PGEFile::IsIntS(value[1]))
                                       lvlitem.left_exit = value[1].toInt();
                                   else
                                       goto badfile;
                               }
                               else
                               if(value[0]=="ER") //Open right path on exit type
                               {
                                   if(PGEFile::IsIntS(value[1]))
                                       lvlitem.right_exit = value[1].toInt();
                                   else
                                       goto badfile;
                               }
                               else
                               if(value[0]=="EB") //Open bottom path on exit type
                               {
                                   if(PGEFile::IsIntS(value[1]))
                                       lvlitem.bottom_exit = value[1].toInt();
                                   else
                                       goto badfile;
                               }
                               else
                               if(value[0]=="WX") //Goto world map X
                               {
                                   if(PGEFile::IsIntS(value[1]))
                                       lvlitem.gotox = value[1].toInt();
                                   else
                                       goto badfile;
                               }
                               else
                               if(value[0]=="WY") //Goto world map Y
                               {
                                   if(PGEFile::IsIntS(value[1]))
                                       lvlitem.gotoy = value[1].toInt();
                                   else
                                       goto badfile;
                               }
                               else
                               if(value[0]=="AV") //Always visible
                               {
                                   if(PGEFile::IsBool(value[1]))
                                       lvlitem.alwaysVisible = (bool)value[1].toInt();
                                   else
                                       goto badfile;
                               }
                               else
                               if(value[0]=="SP") //Is Game start point
                               {
                                   if(PGEFile::IsBool(value[1]))
                                       lvlitem.gamestart = (bool)value[1].toInt();
                                   else
                                       goto badfile;
                               }
                               else
                               if(value[0]=="BP") //Path background
                               {
                                   if(PGEFile::IsBool(value[1]))
                                       lvlitem.pathbg = (bool)value[1].toInt();
                                   else
                                       goto badfile;
                               }
                               else
                               if(value[0]=="BG") //Big path background
                               {
                                   if(PGEFile::IsBool(value[1]))
                                       lvlitem.bigpathbg = (bool)value[1].toInt();
                                   else
                                       goto badfile;
                               }
                     }

                     lvlitem.array_id = FileData.level_array_id++;
                     lvlitem.index = FileData.levels.size();
                     FileData.levels.push_back(lvlitem);
                 }//LEVELS
\
                 //PGEFile::X2BollArr(value[1].toInt());
                 //PGEFile::X2STRArr(value[1]);
                 //PGEFile::X2STR(value[1]);
                 //value[1].toInt();
                 //(bool)value[1].toInt();

             }
     }


     ///////////////////////////////////////EndFile///////////////////////////////////////

     FileData.ReadFileValid=true;

     return FileData;

     badfile:    //If file format not corrects
         BadFileMsg(inf.fileName(), str_count, line);
         FileData.ReadFileValid=false;
     return FileData;
}



//*********************************************************
//****************WRITE FILE FORMAT************************
//*********************************************************

QString FileFormats::WriteExtendedWldFile(WorldData FileData)
{
    QString TextData;
    long i;
    bool addArray=false;

    //HEAD section
    TextData += "HEAD\n";
        if(!FileData.EpisodeTitle.isEmpty())
            TextData += PGEFile::value("TL", PGEFile::qStrS(FileData.EpisodeTitle)); // Episode title

        addArray=false;
        foreach(bool x, FileData.nocharacter)
        { if(x) addArray=true; }
        if(addArray)
            TextData += PGEFile::value("DC", PGEFile::BoolArrayS(FileData.nocharacter)); // Disabled characters

        if(!FileData.IntroLevel_file.isEmpty())
            TextData += PGEFile::value("IT", PGEFile::qStrS(FileData.IntroLevel_file)); // Intro level

        if(FileData.HubStyledWorld)
            TextData += PGEFile::value("HB", PGEFile::BoolS(FileData.HubStyledWorld)); // Hub-styled episode

        if(FileData.restartlevel)
            TextData += PGEFile::value("RL", PGEFile::BoolS(FileData.restartlevel)); // Restart on fail
        if(FileData.stars>0)
            TextData += PGEFile::value("SZ", PGEFile::IntS(FileData.stars));      // Total stars number
        if(!FileData.authors.isEmpty())
            TextData += PGEFile::value("CD", PGEFile::qStrS( FileData.authors )); // Credits

    TextData += "\n";
    TextData += "HEAD_END\n";

    if(!FileData.tiles.isEmpty())
    {
        TextData += "TILES\n";

        for(i=0; i<FileData.tiles.size();i++)
        {
            TextData += PGEFile::value("ID", PGEFile::IntS(FileData.tiles[i].id ));
            TextData += PGEFile::value("X", PGEFile::IntS(FileData.tiles[i].x ));
            TextData += PGEFile::value("Y", PGEFile::IntS(FileData.tiles[i].y ));
            TextData += "\n";
        }

        TextData += "TILES_END\n";
    }

    if(!FileData.scenery.isEmpty())
    {
        TextData += "SCENERY\n";

        for(i=0; i<FileData.scenery.size();i++)
        {
            TextData += PGEFile::value("ID", PGEFile::IntS(FileData.scenery[i].id ));
            TextData += PGEFile::value("X", PGEFile::IntS(FileData.scenery[i].x ));
            TextData += PGEFile::value("Y", PGEFile::IntS(FileData.scenery[i].y ));
            TextData += "\n";
        }

        TextData += "SCENERY_END\n";
    }

    if(!FileData.paths.isEmpty())
    {
        TextData += "PATHS\n";

        for(i=0; i<FileData.paths.size();i++)
        {
            TextData += PGEFile::value("ID", PGEFile::IntS(FileData.paths[i].id ));
            TextData += PGEFile::value("X", PGEFile::IntS(FileData.paths[i].x ));
            TextData += PGEFile::value("Y", PGEFile::IntS(FileData.paths[i].y ));
            TextData += "\n";
        }

        TextData += "PATHS_END\n";
    }

    if(!FileData.music.isEmpty())
    {
        TextData += "MUSICBOXES\n";

        for(i=0; i<FileData.music.size();i++)
        {
            TextData += PGEFile::value("ID", PGEFile::IntS(FileData.music[i].id ));
            TextData += PGEFile::value("X", PGEFile::IntS(FileData.music[i].x ));
            TextData += PGEFile::value("Y", PGEFile::IntS(FileData.music[i].y ));
            if(!FileData.music[i].music_file.isEmpty())
                TextData += PGEFile::value("MF", PGEFile::qStrS(FileData.music[i].music_file ));
            TextData += "\n";
        }

        TextData += "MUSICBOXES_END\n";
    }


    if(!FileData.levels.isEmpty())
    {
        TextData += "LEVELS\n";

        WorldLevels defLvl = dummyWldLevel();
        for(i=0; i<FileData.levels.size();i++)
        {
            TextData += PGEFile::value("ID", PGEFile::IntS(FileData.levels[i].id ));
            TextData += PGEFile::value("X", PGEFile::IntS(FileData.levels[i].x ));
            TextData += PGEFile::value("Y", PGEFile::IntS(FileData.levels[i].y ));
            if(!FileData.levels[i].title.isEmpty())
                TextData += PGEFile::value("LT", PGEFile::qStrS(FileData.levels[i].title ));
            if(!FileData.levels[i].lvlfile.isEmpty())
                TextData += PGEFile::value("LF", PGEFile::qStrS(FileData.levels[i].lvlfile ));
            if(FileData.levels[i].entertowarp!=defLvl.entertowarp)
                TextData += PGEFile::value("EI", PGEFile::IntS(FileData.levels[i].entertowarp ));
            if(FileData.levels[i].left_exit!=defLvl.left_exit)
                TextData += PGEFile::value("EL", PGEFile::IntS(FileData.levels[i].left_exit ));
            if(FileData.levels[i].top_exit!=defLvl.top_exit)
                TextData += PGEFile::value("ET", PGEFile::IntS(FileData.levels[i].top_exit ));
            if(FileData.levels[i].right_exit!=defLvl.right_exit)
                TextData += PGEFile::value("ER", PGEFile::IntS(FileData.levels[i].right_exit ));
            if(FileData.levels[i].bottom_exit!=defLvl.bottom_exit)
                TextData += PGEFile::value("EB", PGEFile::IntS(FileData.levels[i].bottom_exit ));
            if(FileData.levels[i].gotox!=defLvl.gotox)
                TextData += PGEFile::value("WX", PGEFile::IntS(FileData.levels[i].gotox ));
            if(FileData.levels[i].gotoy!=defLvl.gotoy)
                TextData += PGEFile::value("WY", PGEFile::IntS(FileData.levels[i].gotoy ));
            if(FileData.levels[i].alwaysVisible)
                TextData += PGEFile::value("AV", PGEFile::BoolS(FileData.levels[i].alwaysVisible ));
            if(FileData.levels[i].gamestart)
                TextData += PGEFile::value("SP", PGEFile::BoolS(FileData.levels[i].gamestart ));
            if(FileData.levels[i].pathbg)
                TextData += PGEFile::value("BP", PGEFile::BoolS(FileData.levels[i].pathbg ));
            if(FileData.levels[i].bigpathbg)
                TextData += PGEFile::value("BG", PGEFile::BoolS(FileData.levels[i].bigpathbg ));
            TextData += "\n";
        }

        TextData += "LEVELS_END\n";
    }



    return TextData;
}
