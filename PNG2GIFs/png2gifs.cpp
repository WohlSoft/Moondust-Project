/*
 * LazyFixTool, a free tool for fix lazily-made image masks
 * and also, convert all BMPs into GIF
 * This is a part of the Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2015 Vitaly Novichkov <admin@wohlnet.ru>
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
    QApplication::addLibraryPath( "." );
    QApplication::addLibraryPath( QFileInfo(QString::fromUtf8(argv[0])).dir().path() );
    QApplication::addLibraryPath( QFileInfo(QString::fromLocal8Bit(argv[0])).dir().path() );

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
    bool useWindowsArgStyle=false;

    QString argPath;
    QString argOPath;

    QTextStream(stdout) <<"============================================================================\n";
    QTextStream(stdout) <<"PNG to GIFs with masks converter by Wohlstand. Version "<< _FILE_VERSION << _FILE_RELEASE << "\n";
    QTextStream(stdout) <<"============================================================================\n";
    QTextStream(stdout) <<"This program is distributed under the GNU GPLv3 license\n";
    QTextStream(stdout) <<"============================================================================\n";

    QRegExp isPng = QRegExp("*.png");
    isPng.setPatternSyntax(QRegExp::Wildcard);

    //If we are running on windows, we want the "help" screen to display the arg options in the Windows style
    //to be consistent with native Windows applications (using '/' instead of '-' before single-letter args)
#ifdef Q_OS_WIN
    useWindowsArgStyle=true;
#endif

    if(a.arguments().size()==1)
    {
        PNG2GIFsGUI png2gifs_gui;
        png2gifs_gui.exec();
        return 0;
    }

    for(int arg=0; arg<a.arguments().size(); arg++)
    {
        if(QString(a.arguments().at(arg)).toLower()=="--help")
        {
            goto DisplayHelp;
        }
        else
        if((QString(a.arguments().at(arg)).toLower()==("-r")) | (QString(a.arguments().at(arg)).toLower()=="/r"))
        {
            removeSource=true;
        }
        else
        if((QString(a.arguments().at(arg)).toLower()=="-d") | (QString(a.arguments().at(arg)).toLower()=="/d"))
        {
            walkSubDirs=true;
        }
        else
        if(QString(a.arguments().at(arg)).toLower()=="--nopause")
        {
            nopause=true;
        }
        else
        {
            //if begins from "-O"
            if(a.arguments().at(arg).size()>=2 && a.arguments().at(arg).at(0)=='-' && QChar(a.arguments().at(arg).at(1)).toLower()=='o')
            {
                argOPath=a.arguments().at(arg);
                argOPath.remove(0,2);
                //check if user put a space between "-O" and the path and remove it
                if(a.arguments().at(arg).at(0)==' ')
                    argOPath.remove(0,1);
            }
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
    if(useWindowsArgStyle)
    {
        QTextStream(stdout) <<"   PNG2GIFs [--help] [/R] file1.png [file2.png] [...] /O [output path]\n";
        QTextStream(stdout) <<"   PNG2GIFs [--help] [/D] [/R] [input path] /O [output path]\n\n";
        QTextStream(stdout) <<" --help              - Display this help\n";
        QTextStream(stdout) <<" [input path]        - path to an image directory or a PNG file\n";
        QTextStream(stdout) <<" /O [output path]    - path to a directory where the pairs of GIF images will be saved\n";
        QTextStream(stdout) <<" /R                  - Remove source images after successful conversion\n";
        QTextStream(stdout) <<" /D                  - Look for images in subdirectories\n";
    }
    else
    {
        QTextStream(stdout) <<"   PNG2GIFs [--help] [-R] file1.png [file2.png] [...] -O [output path]\n";
        QTextStream(stdout) <<"   PNG2GIFs [--help] [-D] [-R] [input path] -O [output path]\n\n";
        QTextStream(stdout) <<" --help              - Display this help\n";
        QTextStream(stdout) <<" [input path]        - path to an image directory or a PNG file\n";
        QTextStream(stdout) <<" -O [output path]    - path to a directory where the pairs of GIF images will be saved\n";
        QTextStream(stdout) <<" -R                  - Remove source images after successful conversion\n";
        QTextStream(stdout) <<" -D                  - Look for images in subdirectories\n";
    }
    QTextStream(stdout) <<"\n\n";

    getchar();

    exit(0);
    return 0;
WrongInputPath:
    QTextStream(stdout) <<"============================================================================\n";
    QTextStream(stdout) <<"Invalid input path!\n";
    goto DisplayHelp;
WrongOutputPath:
    QTextStream(stdout) <<"============================================================================\n";
    QTextStream(stdout) <<"Invalid output path!\n";
    goto DisplayHelp;
}
