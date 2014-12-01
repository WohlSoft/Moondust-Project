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


MetaData FileFormats::ReadNonSMBX64MetaData(QString RawData, QString filePath)
{

    FileStringList in;
    in.addData( RawData );

    int str_count=0;        //Line Counter
    int i;                  //counters
    //int file_format=0;        //File format number
    QString line;           //Current Line data


    MetaData FileData;

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
                    #ifndef PGE_ENGINE
                    QMessageBox::information(nullptr, "Jokes", sct.second[i], QMessageBox::Ok);
                    #endif
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
                if(sct.first=="META_BOOKMARKS") // Bookmarks
                {
                    Bookmark meta_bookmark;
                    meta_bookmark.bookmarkName = "";
                    meta_bookmark.x = 0;
                    meta_bookmark.y = 0;

                    foreach(QStringList value, sectData) //Look markers and values
                    {
                          if(value[0]=="BM") //Bookmark name
                          {
                              if(PGEFile::IsQStr(value[1]))
                                  meta_bookmark.bookmarkName = PGEFile::X2STR(value[1]);
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="X") // Position X
                          {
                              if(PGEFile::IsIntS(value[1]))
                                  meta_bookmark.x = value[1].toInt();
                              else
                                  goto badfile;
                          }
                          else
                          if(value[0]=="Y") //Position Y
                          {
                              if(PGEFile::IsIntS(value[1]))
                                  meta_bookmark.y = value[1].toInt();
                              else
                                  goto badfile;
                          }
                    }

                    FileData.bookmarks.push_back(meta_bookmark);
                }//Bookmarks

            }
    }


    ///////////////////////////////////////EndFile///////////////////////////////////////

    return FileData;

    badfile:    //If file format is not correct
    BadFileMsg(filePath+"\nError message: "+errorString, str_count, line);
    FileData.bookmarks.clear();

    return FileData;
}


QString FileFormats::WriteNonSMBX64MetaData(MetaData metaData)
{
    QString TextData;
    long i;

    //Bookmarks
    if(!metaData.bookmarks.isEmpty())
    {
        TextData += "META_BOOKMARKS\n";
        for(i=0;i<metaData.bookmarks.size(); i++)
        {
            //Bookmark name
            TextData += PGEFile::value("BM", PGEFile::qStrS(metaData.bookmarks[i].bookmarkName));
            TextData += PGEFile::value("X", PGEFile::IntS(metaData.bookmarks[i].x));
            TextData += PGEFile::value("Y", PGEFile::IntS(metaData.bookmarks[i].y));
            TextData += "\n";
        }
        TextData += "META_BOOKMARKS_END\n";
    }
    return TextData;
}

