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

     in.setAutoDetectUnicode(true);
     in.setLocale(QLocale::system());
     in.setCodec(QTextCodec::codecForLocale());

     int str_count=0;        //Line Counter
     int i;                  //counters
     QString line;           //Current Line data

     WorldData FileData = dummyWldDataArray();

     FileData.untitled = false;
     FileData.modified = false;

     WorldTiles tile;
     WorldScenery scen;
     WorldPaths pathitem;
     WorldLevels lvlitem;
     WorldMusic musicbox;


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


                 //READING MARKERS HERE

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


    //GENERATING TEXT DATA HERE


    return TextData;
}
