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

#include <QCoreApplication>
#include <QImage>
#include <QDir>
#include <QDirIterator>
#include <QString>
#include <QTextStream>
#include <QFileInfo>
#include "version.h"

extern "C"{
	#include "../_Libs/giflib/gif_lib.h"
}

bool removeSource=false;

QImage setAlphaMask(QImage image, QImage mask)
{
    if(mask.isNull())
        return image;

    if(image.isNull())
        return image;

    QImage target = image;
    QImage newmask = mask;

    if(target.size()!= newmask.size())
    {
        newmask = newmask.copy(0,0, target.width(), target.height());
    }

    newmask.invertPixels();

    target.setAlphaChannel(newmask);

    return target;
}

bool toGif(QImage& img, QString& path){
    int errcode;

    if(QFile(path).exists()) // Remove old file
        QFile::remove(path);

    GifFileType* t = EGifOpenFileName(path.toLocal8Bit().data(),true, &errcode);
    if(!t){
        EGifCloseFile(t, &errcode);
        QTextStream(stdout)  << "Can't open\n";
        return false;
    }

    EGifSetGifVersion(t, true);

    GifColorType* colorArr = new GifColorType[256];
    ColorMapObject* cmo = GifMakeMapObject(256, colorArr);

    bool unfinished = false;
    QImage tarQImg(img.width(), img.height(), QImage::Format_Indexed8);
    QVector<QRgb> table;
    for(int y = 0; y < img.height(); y++){
        for(int x = 0; x < img.width(); x++){
            if(table.size() >= 256){
                unfinished = true;
                break;
            }
            QRgb pix;
            if(!table.contains(pix = img.pixel(x,y))){
                table.push_back(pix);
                tarQImg.setColor(tarQImg.colorCount(), pix);
            }
            tarQImg.setPixel(x,y,table.indexOf(pix));
        }
        if(table.size() >= 256){
            unfinished = true;
            break;
        }
    }

    if(unfinished){
        EGifCloseFile(t, &errcode);
        QTextStream(stdout)  << "Unfinished\n";
        return false;
    }


    for(int l = tarQImg.colorCount(); l < 256; l++){
        tarQImg.setColor(l,0);
    }

    if(tarQImg.colorTable().size() != 256){
        EGifCloseFile(t, &errcode);
        QTextStream(stdout)  << "A lot of colors\n";
        return false;
    }

    QVector<QRgb> clTab = tarQImg.colorTable();

    for(int i = 0; i < 255; i++){
        QRgb rgb = clTab[i];
        colorArr[i].Red = qRed(rgb);
        colorArr[i].Green = qGreen(rgb);
        colorArr[i].Blue = qBlue(rgb);
    }
    cmo->Colors = colorArr;

    errcode = EGifPutScreenDesc(t, img.width(), img.height(), 256, 0, cmo);
    if(errcode != GIF_OK){
        EGifCloseFile(t, &errcode);
        QTextStream(stdout)  << "EGifPutScreenDesc error 1\n";
        return false;
    }

    errcode = EGifPutImageDesc(t, 0, 0, img.width(), img.height(), false, 0);
    if(errcode != GIF_OK){
        EGifCloseFile(t, &errcode);
        QTextStream(stdout)  << "EGifPutImageDesc error 2\n";
        return false;
    }

    //gen byte array
    GifByteType* byteArr = tarQImg.bits();

    for(int h = 0; h < tarQImg.height(); h++){
        errcode = EGifPutLine(t, byteArr, tarQImg.width());
        if(errcode != GIF_OK){
            EGifCloseFile(t, &errcode);
            QTextStream(stdout)  << "EGifPutLine error 3\n";
            return false;
        }

        byteArr += tarQImg.width();
        byteArr += ((tarQImg.width() % 4)!=0 ? 4 - (tarQImg.width() % 4) : 0);
    }

    if(errcode != GIF_OK){
        QTextStream(stdout)  << "GIF error 4\n";
        return false;
    }
    EGifCloseFile(t, &errcode);

    return true;
}

QImage loadQImage(QString file)
{
    QImage image = QImage(file);
    return image;
}


void doMagicIn(QString path, QString q, QString OPath)
{
    //skip unexists pairs
    if(!QFile(path+q).exists())
        return;

    QImage ImgSrc;
    QImage image;
    QImage mask;

    ImgSrc = loadQImage(path+q);

    if(ImgSrc.isNull()) //Skip null images
        return;
    QString bname = QFileInfo(path+q).baseName();

    QString saveToImg = QString(OPath+(bname.toLower())+".gif");
    QString saveToMask = QString(OPath+(bname.toLower())+"m.gif");

    QTextStream(stdout) << path+q+"\n";

    mask = ImgSrc.alphaChannel();
    mask.invertPixels();

    //Write mask image
    if( toGif(mask, saveToMask ) ) //Write gif
    {
        QTextStream(stdout) <<"GIF-1\n";
    }
    else
    {
        mask.save(saveToMask, "BMP"); //If failed, write BMP
        QTextStream(stdout) <<"BMP-1\n";
    }



    image = ImgSrc.convertToFormat(QImage::Format_RGB32);
    mask = ImgSrc.alphaChannel();
    mask.invertPixels();
    for(int w=0; w < mask.width(); w++)
        for(int h=0; h < mask.height(); h++)
        {
            if(mask.pixel(w,h)==qRgb(0xFF,0xFF,0xFF)) //Fill white pixel into black on image
            {
                image.setPixel(w,h, qRgb(0,0,0));
            }
            else
            if(mask.pixel(w,h)!=qRgb(0,0,0)) //Fill non-black pixel color into black on mask
            {
                mask.setPixel(w,h, qGray(0,0,0));
            }
        }
    mask.invertPixels();
    image.setAlphaChannel(mask);

    //Write mask image
    if( toGif(image, saveToImg ) ) //Write gif
    {
        QTextStream(stdout) <<"GIF-2\n";
    }
    else
    {
        image.save(saveToImg, "BMP"); //If failed, write BMP
        QTextStream(stdout) <<"BMP-2\n";
    }

    if(removeSource) QFile(path+q).remove();
}

int main(int argc, char *argv[])
{
    QCoreApplication::addLibraryPath(".");
    QCoreApplication a(argc, argv);

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
        goto DisplayHelp;
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
