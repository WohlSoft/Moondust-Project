/*
 * LazyFixTool, a free tool for fix lazily-made image masks
 * and also, convert all BMPs into GIF
 * This is a part of the Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
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

static QTextStream qout(stdout);

int main(int argc, char *argv[])
{
    QApplication::addLibraryPath(".");
    QApplication::addLibraryPath(QFileInfo(QString::fromUtf8(argv[0])).dir().path());
    QApplication::addLibraryPath(QFileInfo(QString::fromLocal8Bit(argv[0])).dir().path());
    QApplication a(argc, argv);
    QStringList filters;
    QDir imagesDir;
    QString path;
    QString OPath;
    QStringList fileList;
    bool walkSubDirs = false;
    //bool nopause=false;
    bool cOpath = false;
    bool singleFiles = false;
    QString argPath;
    QString argOPath;
    qout << "============================================================================\n";
    qout << "PNG to GIFs with masks converter by Wohlstand. Version " << _FILE_VERSION << _FILE_RELEASE << "\n";
    qout << "============================================================================\n";
    qout << "This program is distributed under the GNU GPLv3 license\n";
    qout << "============================================================================\n";
    qout.flush();
    QRegExp isPng = QRegExp("*.png");
    isPng.setPatternSyntax(QRegExp::Wildcard);

    if(a.arguments().size() == 1)
    {
        PNG2GIFsGUI png2gifs_gui;
        png2gifs_gui.exec();
        return 0;
    }

    for(int arg = 0; arg < a.arguments().size(); arg++)
    {
        QString argC = a.arguments().at(arg);

        if(argC.compare("--help", Qt::CaseInsensitive) == 0)
            goto DisplayHelp;
        else if((argC.compare("-r", Qt::CaseInsensitive) == 0) ||
                (argC.compare("/r", Qt::CaseInsensitive) == 0))
            removeSource = true;
        else if((argC.compare("-d", Qt::CaseInsensitive) == 0) ||
                (argC.compare("/d", Qt::CaseInsensitive) == 0) ||
                (argC.compare("-w", Qt::CaseInsensitive) == 0) ||
                (argC.compare("/w", Qt::CaseInsensitive) == 0))
            walkSubDirs = true;
        else if(argC.compare("--nopause", Qt::CaseInsensitive) == 0)
        {
            //nopause=true;
        }
        else
        {
            //if begins from "-O"
            if(argC.size() >= 2 && argC.at(0) == '-' && QChar(argC.at(1)).toLower() == 'o')
            {
                argOPath = argC.mid(2, -1);

                //check if user put a space between "-O" and the path and remove it
                if(argC.at(0) == ' ')
                    argC.remove(0, 1);
            }
            else
            {
                if(isPng.exactMatch(a.arguments().at(arg)))
                {
                    fileList << argC;
                    singleFiles = true;
                }
                else
                    argPath = argC;
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
        cOpath = true;
        OPath = path;
    }

    qout << "============================================================================\n";
    qout << "Converting images...\n";
    qout << "============================================================================\n";

    if(!singleFiles)
        qout << QString("Input path:  " + path + "\n");

    qout << QString("Output path: " + OPath + "\n");
    qout << "============================================================================\n";
    qout.flush();

    if(singleFiles) //By files
    {
        foreach(QString q, fileList)
        {
            path = QFileInfo(q).absoluteDir().path() + "/";
            QString fname = QFileInfo(q).fileName();

            if(cOpath) OPath = path;

            doMagicIn(path, fname, OPath);
        }
    }
    else
    {
        fileList << imagesDir.entryList(filters);

        if(!walkSubDirs)
            foreach(QString q, fileList)
                doMagicIn(path, q, OPath);
        else
        {
            QDirIterator dirsList(imagesDir.absolutePath(), filters,
                                  QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot,
                                  QDirIterator::Subdirectories);

            while(dirsList.hasNext())
            {
                dirsList.next();

                if(QFileInfo(dirsList.filePath()).dir().dirName() == "_backup") //Skip Backup dirs
                    continue;

                if(cOpath) OPath = QFileInfo(dirsList.filePath()).dir().absolutePath() + "/";

                //qDebug()<< QFileInfo(dirsList.filePath()).dir().absolutePath();
                doMagicIn(QFileInfo(dirsList.filePath()).dir().absolutePath() + "/", dirsList.fileName(), OPath);
            }
        }
    }

    qout << "============================================================================\n";
    qout << "Done!\n\n";
    qout.flush();
    //if(!nopause)
    //    getchar();
    return 0;
DisplayHelp:
    //If we are running on windows, we want the "help" screen to display the arg options in the Windows style
    //to be consistent with native Windows applications (using '/' instead of '-' before single-letter args)
    qout << "============================================================================\n";
    qout << "This utility will convert PNG images into GIF with masks format:\n";
    qout << "============================================================================\n";
    qout << "Syntax:\n\n";
#ifdef Q_OS_WIN
    qout << "   PNG2GIFs [--help] [/R] file1.png [file2.png] [...] /O [output path]\n";
    qout << "   PNG2GIFs [--help] [/D] [/R] [input path] /O [output path]\n\n";
    qout << " --help              - Display this help\n";
    qout << " [input path]        - path to an image directory or a PNG file\n";
    qout << " /O [output path]    - path to a directory where the pairs of GIF images will be saved\n";
    qout << " /R                  - Remove source images after successful conversion\n";
    qout << " /D                  - Look for images in subdirectories\n";
#else
    qout << "   PNG2GIFs [--help] [-R] file1.png [file2.png] [...] -O [output path]\n";
    qout << "   PNG2GIFs [--help] [-D] [-R] [input path] -O [output path]\n\n";
    qout << " --help              - Display this help\n";
    qout << " [input path]        - path to an image directory or a PNG file\n";
    qout << " -O [output path]    - path to a directory where the pairs of GIF images will be saved\n";
    qout << " -R                  - Remove source images after successful conversion\n";
    qout << " -D                  - Look for images in subdirectories\n";
#endif
    qout << "\n\n";
    qout.flush();
    return 0;
WrongInputPath:
    qout << "============================================================================\n";
    qout << "Invalid input path!\n";
    goto DisplayHelp;
WrongOutputPath:
    qout << "============================================================================\n";
    qout << "Invalid output path!\n";
    goto DisplayHelp;
}
