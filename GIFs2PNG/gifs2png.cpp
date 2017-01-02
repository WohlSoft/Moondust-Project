/*
 * GIFs2PNG, a free tool for merge GIF images with his masks and save into PNG
 * This is a part of the Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QCoreApplication>
#include <QDir>
#include <QDirIterator>
#include <QString>
#include <QTextStream>
#include <QFileInfo>
#include "version.h"

#ifdef _WIN32
#define FREEIMAGE_LIB 1
#endif
#include <FreeImageLite.h>

#include "common_features/config_manager.h"

#include "common_features/file_mapper.h"

FIBITMAP* loadImage(QString file, bool convertTo32bit=true)
{
    #if  defined(__unix__) || defined(_WIN32)
    PGE_FileMapper fileMap;
    if( !fileMap.open_file(file.toUtf8().data()) )
    {
        return NULL;
    }

    FIMEMORY *imgMEM = FreeImage_OpenMemory((unsigned char*)fileMap.data, (unsigned int)fileMap.size);
    FREE_IMAGE_FORMAT formato = FreeImage_GetFileTypeFromMemory(imgMEM);
    if(formato  == FIF_UNKNOWN) { return NULL; }
    FIBITMAP* img = FreeImage_LoadFromMemory(formato, imgMEM, 0);
    FreeImage_CloseMemory(imgMEM);
    fileMap.close_file();
    if(!img) {
        return NULL;
    }
    #else
    FREE_IMAGE_FORMAT formato = FreeImage_GetFileType(file.toUtf8().data(), 0);
    if(formato  == FIF_UNKNOWN) { return NULL; }
    FIBITMAP* img = FreeImage_Load(formato, file.toUtf8().data());
    if(!img) { return NULL; }
    #endif
    if(convertTo32bit)
    {
        FIBITMAP* temp;
        temp = FreeImage_ConvertTo32Bits(img);
        if(!temp) { return NULL; }
        FreeImage_Unload(img);
        img = temp;
    }
    return img;
}

void mergeBitBltToRGBA(FIBITMAP *image, QString pathToMask)
{
    if(!image) return;
    if(!QFileInfo(pathToMask).exists()) return; //Nothing to do
    FIBITMAP* mask = loadImage( pathToMask );
    if(!mask) return;//Nothing to do

    unsigned int img_w = FreeImage_GetWidth(image);
    unsigned int img_h = FreeImage_GetHeight(image);
    unsigned int mask_w = FreeImage_GetWidth(mask);
    unsigned int mask_h = FreeImage_GetHeight(mask);

    for(unsigned int y=0; (y<img_h) && (y<mask_h); y++)
    {
        for(unsigned int x=0; (x<img_w) && (x<mask_w); x++ )
        {
            RGBQUAD Fpix;
            FreeImage_GetPixelColor(image, x, y, &Fpix);

            RGBQUAD Dpix = {0x7F, 0x7F, 0x7F, 0xFF};

            RGBQUAD Spix;
            FreeImage_GetPixelColor(mask, x, y, &Spix);

            RGBQUAD Npix = {0x0, 0x0, 0x0, 0xff};

            Npix.rgbRed = (Dpix.rgbRed & Spix.rgbRed);
            Npix.rgbGreen = (Dpix.rgbGreen & Spix.rgbGreen);
            Npix.rgbBlue = (Dpix.rgbBlue & Spix.rgbBlue);
            Npix.rgbRed = (Npix.rgbRed | Fpix.rgbRed);
            Npix.rgbGreen = (Npix.rgbGreen | Fpix.rgbGreen);
            Npix.rgbBlue = (Npix.rgbBlue | Fpix.rgbBlue);
            int newAlpha= 255-
                      ( ( int(Spix.rgbRed)+
                          int(Spix.rgbGreen)+
                          int(Spix.rgbBlue) ) / 3);
            if(  (Spix.rgbRed>240u) //is almost White
               &&(Spix.rgbGreen>240u)
               &&(Spix.rgbBlue>240u))
            {
                newAlpha = 0;
            }

            newAlpha= newAlpha+( ( int(Fpix.rgbRed)+
                                   int(Fpix.rgbGreen)+
                                   int(Fpix.rgbBlue) ) / 3);
            if(newAlpha > 255) newAlpha=255;
            Npix.rgbReserved = newAlpha;

            FreeImage_SetPixelColor(image, x, y, &Npix);
        }
    }
    FreeImage_Unload(mask);
}

void doMagicIn(QString path, QString q, QString OPath, bool removeMode, ConfigPackMiniManager &cnf)
{
    QTextStream pout(stdout);
    QRegExp isMask = QRegExp("*m.gif");
    isMask.setPatternSyntax(QRegExp::Wildcard);

    QRegExp isBackupDir = QRegExp("*/_backup/");
    isBackupDir.setPatternSyntax(QRegExp::Wildcard);

    if(isBackupDir.exactMatch(path))
        return; //Skip backup directories

    if(isMask.exactMatch(q))
        return;

    QString imgFileM;
    QStringList tmp = q.split(".", QString::SkipEmptyParts);
    if(tmp.size()==2)
        imgFileM = tmp[0] + "m." + tmp[1];
    else
        return;

    QString maskPath = cnf.getFile(imgFileM, path);

    FIBITMAP* image=loadImage(path+q);
    if(!image)
    {
        QTextStream(stderr) << path+q+" - CAN'T OPEN!\n";
        return;
    }

    if(QFileInfo(maskPath).exists())
    {
        //mask.load(maskPath);
        mergeBitBltToRGBA(image, maskPath);
    }

    if(image)
    {
        pout << path+q <<"\n";
        if(FreeImage_Save(FIF_PNG, image, QString(OPath+tmp[0]+".png").toLocal8Bit().data()))
        {
            if(removeMode) // Detele old files
            {
                QFile::remove( path+q );
                QFile::remove( path+imgFileM );
            }
            pout << OPath+tmp[0]+".png" <<"\n";
        }
        FreeImage_Unload(image);
    }
    else
    {
        QTextStream(stderr) << path+q+" - WRONG!\n";
    }
}

bool isQuotesdString(QString in) // QUOTED STRING
{
    //This is INVERTED validator. If false - good, true - bad.
    #define QStrGOOD true
    #define QStrBAD false
    int i=0;
    for(i=0; i<(signed)in.size();i++)
    {
        if(i==0)
        {
            if(in[i]!='"') return QStrBAD;
        } else if(i==(signed)in.size()-1) {
            if(in[i]!='"') return QStrBAD;
        } else if(in[i]=='"') return QStrBAD;
        else if(in[i]=='"') return QStrBAD;
    }
    if(i==0) return QStrBAD; //This is INVERTED validator. If false - good, true - bad.
    return QStrGOOD;
}

QString removeQuotes(QString str)
{
    QString target = str;
    if(target.isEmpty())
        return target;
    if(target[0]==QChar('\"'))
        target.remove(0,1);
    if((!target.isEmpty()) && (target[target.size()-1]==QChar('\"')))
        target.remove(target.size()-1,1);
    return target;
}

int main(int argc, char *argv[])
{
    QCoreApplication::addLibraryPath( "." );
    QCoreApplication::addLibraryPath( QFileInfo(QString::fromUtf8(argv[0])).dir().path() );
    QCoreApplication::addLibraryPath( QFileInfo(QString::fromLocal8Bit(argv[0])).dir().path() );

    QCoreApplication a(argc, argv);
    QStringList filters;
    QDir imagesDir;
    QString path;
    QString OPath;
    bool removeMode=false;
    QStringList fileList;

    QTextStream pout(stdout);

    FreeImage_Initialise();

    ConfigPackMiniManager config;

    bool nopause=false;
    bool walkSubDirs=false;
    bool cOpath=false;
    bool singleFiles=false;
    bool skipBackground2=false;

    QString argPath;
    QString argOPath;

    QString configPath;

    pout <<"============================================================================\n";
    pout <<"Pair of GIFs to PNG converter tool by Wohlstand. Version "<<_FILE_VERSION<<_FILE_RELEASE<<"\n";
    pout <<"============================================================================\n";
    pout <<"This program is distributed under the GNU GPLv3 license \n";
    pout <<"============================================================================\n";
    pout.flush();

    QRegExp isGif("*.gif");
    isGif.setPatternSyntax(QRegExp::Wildcard);
    isGif.setCaseSensitivity(Qt::CaseInsensitive);

    QRegExp isMask("*m.gif");
    isMask.setPatternSyntax(QRegExp::Wildcard);
    isMask.setCaseSensitivity(Qt::CaseInsensitive);

    QRegExp bg2("background2\\-\\d+\\.gif");
    bg2.setPatternSyntax(QRegExp::RegExp);
    bg2.setCaseSensitivity(Qt::CaseInsensitive);

    if(a.arguments().size()==1)
    {
        goto DisplayHelp;
    }

    for(int arg=0; arg<a.arguments().size(); arg++)
    {
        if(QString(a.arguments().at(arg)).toLower()=="--help")
        {
            goto DisplayHelp;
        }
        else
        if( (QString(a.arguments().at(arg)).toLower()==("-r"))||
            (QString(a.arguments().at(arg)).toLower()=="/r"))
        {
            removeMode=true;
        }
        else
        if( (QString(a.arguments().at(arg)).toLower()==("-b"))||
            (QString(a.arguments().at(arg)).toLower()=="/b"))
        {
            skipBackground2=true;
        }
        else
        if( (QString(a.arguments().at(arg)).toLower()=="-d")||
            (QString(a.arguments().at(arg)).toLower()=="/d")||
            (QString(a.arguments().at(arg)).toLower()=="-w")||
            (QString(a.arguments().at(arg)).toLower()=="/w") )
        {
            walkSubDirs=true;
        }
        else
        if(QString(a.arguments().at(arg)).toLower()=="--nopause")
        {
            nopause=true;
        }
        else
        if(QString(a.arguments().at(arg)).toLower().startsWith("--config="))
        {
            QStringList tmp;
            tmp = a.arguments().at(arg).split('=');
            if(tmp.size()>1)
            {
                configPath = tmp.last();
                if(isQuotesdString(configPath))
                {
                    configPath = removeQuotes(configPath);
                    configPath = QDir(configPath).absolutePath();
                }
            }
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
                if(isMask.exactMatch(a.arguments().at(arg)))
                    continue;
                else
                    if(isGif.exactMatch(a.arguments().at(arg)))
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
        filters << "*.gif" << "*.GIF";
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
        OPath=path;
        cOpath=true;
    }

    pout <<"============================================================================\n";
    pout <<"Converting images...\n";
    pout <<"============================================================================\n";
    pout.flush();

    config.setConfigDir(configPath);

    if(!singleFiles)
      pout << QString("Input path:  "+path+"\n");
    pout << QString("Output path: "+OPath+"\n");
    pout <<"============================================================================\n";
    pout.flush();
    if(!configPath.isEmpty())
    {
        pout <<"============================================================================\n";
        pout <<QString("Used config pack: "+configPath+"\n");
        pout <<"============================================================================\n";
        pout.flush();
    }
    if(singleFiles) //By files
    {
        foreach(QString q, fileList)
        {
            path=QFileInfo(q).absoluteDir().path()+"/";
            QString fname = QFileInfo(q).fileName();
            if(skipBackground2 && (bg2.indexIn(fname) != -1))
                continue;
            if(cOpath) OPath=path;
            doMagicIn(path, fname , OPath, removeMode, config);
        }
    }
    else
    {
        fileList << imagesDir.entryList(filters);
        if(!walkSubDirs) //By directories
        {
            foreach(QString q, fileList)
            {
                if(skipBackground2 && (bg2.indexIn(q) != -1))
                    continue;
                doMagicIn(path, q, OPath, removeMode, config);
            }
        }
        else
        {
            QDirIterator dirsList(imagesDir.absolutePath(), filters,
                                      QDir::Files|QDir::NoSymLinks|QDir::NoDotAndDotDot,
                                  QDirIterator::Subdirectories);

            while(dirsList.hasNext())
            {
                dirsList.next();
                if(QFileInfo(dirsList.filePath()).dir().dirName()=="_backup") //Skip LazyFix's Backup dirs
                    continue;

                if(skipBackground2 && (bg2.indexIn(dirsList.filePath()) != -1))
                    continue;

                if(cOpath) OPath = QFileInfo(dirsList.filePath()).dir().absolutePath()+"/";

                doMagicIn(QFileInfo(dirsList.filePath()).dir().absolutePath()+"/", dirsList.fileName(), OPath, removeMode, config);
            }
        }
    }

    pout <<"============================================================================\n";
    pout <<"Done!\n\n";
    pout.flush();

    if(!nopause)
    {
        pout <<"Press any key to exit...\n";
        pout.flush();
        getchar();
    }

    return 0;

DisplayHelp:
    //If we are running on windows, we want the "help" screen to display the arg options in the Windows style
    //to be consistent with native Windows applications (using '/' instead of '-' before single-letter args)

    pout <<"============================================================================\n";
    pout <<"This utility will merge GIF images and their masks into solid PNG images:\n";
    pout <<"============================================================================\n";
    pout <<"Syntax:\n\n";
#ifdef Q_OS_WIN
#define ARGSIGN "/"
#else
#define ARGSIGN "-"
#endif
    pout <<"   GIFs2PNG [--help] [" ARGSIGN "R] file1.gif [file2.gif] [...] [" ARGSIGN "O path/to/output]\n";
    pout <<"   GIFs2PNG [--help] [" ARGSIGN "D] [" ARGSIGN "R] path/to/input [" ARGSIGN "O path/to/output]\n\n";
    pout <<" --help              - Display this help\n";
    pout <<" path/to/input       - path to a directory with pairs of GIF files\n";
    pout <<" " ARGSIGN "O path/to/output   - path to a directory where the PNG images will be saved\n";
    pout <<" " ARGSIGN "R                  - Remove source images after a succesful conversion\n";
    pout <<" " ARGSIGN "D                  - Look for images in subdirectories\n";
    pout <<" " ARGSIGN "B                  - Skip all \"background2-*.gif\" sprites (due a bug in the LunaLUA)\n";

    pout <<"\n";
    pout <<" --config=/path/to/config/pack\n";
    pout <<"                     - Allow usage of default masks from specific PGE config pack\n";
    pout <<"                       (Useful for cases where the GFX designer didn't make a mask image)\n";
    pout <<" --nopause           - Don't pause application after processing finishes (useful for script integration)\n";
    pout <<"\n\n";
    pout.flush();

    getchar();

    exit(0);
    return 0;
WrongInputPath:
    pout <<"============================================================================\n";
    QTextStream(stderr) <<"Wrong input path!\n";
    goto DisplayHelp;
WrongOutputPath:
    pout <<"============================================================================\n";
    pout.flush();
    QTextStream(stderr) <<"Wrong output path!\n";
    goto DisplayHelp;
}
