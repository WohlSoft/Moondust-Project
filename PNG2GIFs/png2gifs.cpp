/*
 * LazyFixTool, a free tool for fix lazily-made image masks
 * and also, convert all BMPs into GIF
 * This is a part of the Platformer Game Engine by Wohlstand, a free platform for game making
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

#include <QApplication>
#include <QImage>
#include <QDir>
#include <QDirIterator>
#include <QString>
#include <QTextStream>
#include <QFileInfo>
#include "version.h"
#include "png2gifs_gui.h"
#include "png2gifs_converter.h"

int main(int argc, char *argv[])
{
    QApplication::addLibraryPath( QFileInfo(argv[0]).dir().path() );
    QApplication a(argc, argv);

    QStringList filters;
    QDir imagesDir;
    QString path;
    QString OPath;
    QStringList fileList;

    bool walkSubDirs=false;
    bool nopause=false;
    bool cOpath=false;
    bool singleFiles=false;

    QString argPath;
    QString argOPath;

    QTextStream(stdout) <<"============================================================================\n";
    QTextStream(stdout) <<"PNG to GIFs with masks converter by Wohlstand. Version "<< _FILE_VERSION << _FILE_RELEASE << "\n";
    QTextStream(stdout) <<"============================================================================\n";
    QTextStream(stdout) <<"This program is distributed under the GNU GPLv3 license\n";
    QTextStream(stdout) <<"============================================================================\n";

    QRegExp isPng = QRegExp("*.png");
    isPng.setPatternSyntax(QRegExp::Wildcard);

    if(a.arguments().size()==1)
    {
        PNG2GIFsGUI png2gifs_gui;
        png2gifs_gui.exec();
        return 0;
    }

    for(int arg=0; arg<a.arguments().size(); arg++)
    {
        if(a.arguments().at(arg)=="--help")
        {
            goto DisplayHelp;
        }
        else
        if(a.arguments().at(arg)=="-R")
        {
            removeSource=true;
        }
        else
        if(a.arguments().at(arg)=="-W")
        {
            walkSubDirs=true;
        }
        else
        if(a.arguments().at(arg)=="--nopause")
        {
            nopause=true;
        }
        else
        {
            //if begins from "-O"
            if(a.arguments().at(arg).size()>=2 && a.arguments().at(arg).at(0)=='-'&& a.arguments().at(arg).at(1)=='O')
              {  argOPath=a.arguments().at(arg); argOPath.remove(0,2); }
            else
            {
                if(isPng.exactMatch(a.arguments().at(arg)))
                {
                    fileList << a.arguments().at(arg);
                    singleFiles=true;
                }
                else
                    argPath=a.arguments().at(arg);
            }
        }
    }


    if(!singleFiles)
    {
        if(argPath.isEmpty()) goto WrongInputPath;
        if(!QDir(argPath).exists()) goto WrongInputPath;

        imagesDir.setPath(argPath);
        filters << "*.png" << "*.PNG";
        imagesDir.setSorting(QDir::Name);
        imagesDir.setNameFilters(filters);

        path = imagesDir.absolutePath() + "/";
    }

    if(!argOPath.isEmpty())
    {
        OPath = argOPath;
        if(!QFileInfo(OPath).isDir())
            goto WrongOutputPath;

        OPath = QDir(OPath).absolutePath() + "/";
    }
    else
    {
        cOpath=true;
        OPath=path;
    }

    QTextStream(stdout) <<"============================================================================\n";
    QTextStream(stdout) <<"Converting images...\n";
    QTextStream(stdout) <<"============================================================================\n";

    if(!singleFiles)
     QTextStream(stdout) << QString("Input path:  "+path+"\n");
    QTextStream(stdout) << QString("Output path: "+OPath+"\n");
    QTextStream(stdout) <<"============================================================================\n";

    if(singleFiles) //By files
    {
        foreach(QString q, fileList)
        {
            path=QFileInfo(q).absoluteDir().path()+"/";
            QString fname = QFileInfo(q).fileName();
            if(cOpath) OPath=path;
            doMagicIn(path, fname, OPath);
        }
    }
    else
    {
        fileList << imagesDir.entryList(filters);

    if(!walkSubDirs)
        foreach(QString q, fileList)
        {
            doMagicIn(path, q, OPath);
        }
        else
        {
            QDirIterator dirsList(imagesDir.absolutePath(), filters,
                                      QDir::Files|QDir::NoSymLinks|QDir::NoDotAndDotDot,
                                  QDirIterator::Subdirectories);

            while(dirsList.hasNext())
              {
                    dirsList.next();
                    if(QFileInfo(dirsList.filePath()).dir().dirName()=="_backup") //Skip Backup dirs
                        continue;

                    if(cOpath) OPath = QFileInfo(dirsList.filePath()).dir().absolutePath()+"/";

                    //qDebug()<< QFileInfo(dirsList.filePath()).dir().absolutePath();
                    doMagicIn(QFileInfo(dirsList.filePath()).dir().absolutePath()+"/", dirsList.fileName(), OPath);
              }


        }
    }

    QTextStream(stdout) <<"============================================================================\n";
    QTextStream(stdout) <<"Done!\n\n";

    if(!nopause) getchar();

    return 0;

DisplayHelp:
    QTextStream(stdout) <<"============================================================================\n";
    QTextStream(stdout) <<"This utility will convert PNG images into GIF with masks format:\n";
    QTextStream(stdout) <<"============================================================================\n";
    QTextStream(stdout) <<"Syntax:\n\n";
    QTextStream(stdout) <<"   PNG2GIFs [--help] [-R] file1.png [file2.png] [...] [-O/path/to/out]\n";
    QTextStream(stdout) <<"   PNG2GIFs [--help] [-W] [-R] /path/to/folder [-O/path/to/out]\n\n";
    QTextStream(stdout) <<" --help              - Display this help\n";
    QTextStream(stdout) <<" /path/to/folder     - path to a directory with PNG files\n";
    QTextStream(stdout) <<" -O/path/to/out      - path to a directory where the pairs of GIF images will be saved\n";
    QTextStream(stdout) <<" -R                  - Remove source images after successful conversion\n";
    QTextStream(stdout) <<" -W                  - Also look for images in subdirectories\n";
    QTextStream(stdout) <<"\n\n";

    getchar();

    exit(0);
    return 0;
WrongInputPath:
    QTextStream(stdout) <<"============================================================================\n";
    QTextStream(stdout) <<"Wrong input path!\n";
    goto DisplayHelp;
WrongOutputPath:
    QTextStream(stdout) <<"============================================================================\n";
    QTextStream(stdout) <<"Wrong output path!\n";
    goto DisplayHelp;
}
