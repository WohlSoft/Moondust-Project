/*
 * LazyFixTool, a free tool for fix lazily-made image masks
 * This is a part of Platformer Game Engine by Wohlstand, a free platform for game making
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

#include <iostream>

#include <QCoreApplication>
#include <QImage>
#include <QDir>
#include <QDirIterator>
#include <QString>
#include <QTextStream>
#include <QtDebug>
#include <QFileInfo>

#include "libs/giflib/gif_lib.h"
#include "libs/EasyBMP/EasyBMP.h"

bool noBackUp=false;
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
    GifFileType* t = EGifOpenFileName(path.toStdString().c_str(),true, &errcode);
    if(!t){
        EGifCloseFile(t, &errcode);
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
        return false;
    }


    for(int l = tarQImg.colorCount(); l < 256; l++){
        tarQImg.setColor(l,0);
    }

    if(tarQImg.colorTable().size() != 256){
        EGifCloseFile(t, &errcode);
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
        return false;
    }

    errcode = EGifPutImageDesc(t, 0, 0, img.width(), img.height(), false, 0);
    if(errcode != GIF_OK){
        EGifCloseFile(t, &errcode);
        return false;
    }

    //gen byte array
    GifByteType* byteArr = tarQImg.bits();

    for(int h = 0; h < tarQImg.height(); h++){
        errcode = EGifPutLine(t, byteArr, tarQImg.width());
        if(errcode != GIF_OK){
            EGifCloseFile(t, &errcode);
            return false;
        }

        byteArr += tarQImg.width();
        byteArr += ((tarQImg.width() % 4)!=0 ? 4 - (tarQImg.width() % 4) : 0);
    }





    EGifCloseFile(t, &errcode);
    if(errcode != GIF_OK){
        return false;
    }
    return true;
}

QImage fromBMP(QString &file)
{
    QImage errImg;

    BMP tarBMP;
    if(!tarBMP.ReadFromFile(file.toStdString().c_str())){
        //WriteToLog(QtCriticalMsg, QString("Error: File does not exsist"));
        return errImg; //Check if empty with errImg.isNull();
    }

    QImage bmpImg(tarBMP.TellWidth(),tarBMP.TellHeight(),QImage::Format_RGB666);

    for(int x = 0; x < tarBMP.TellWidth(); x++){
        for(int y = 0; y < tarBMP.TellHeight(); y++){
            RGBApixel pixAt = tarBMP.GetPixel(x,y);
            bmpImg.setPixel(x,y,qRgb(pixAt.Red, pixAt.Green, pixAt.Blue));
        }
    }

    return bmpImg;
}

QImage loadQImage(QString file)
{
    QImage image = QImage(file);
    if(image.isNull())
        image = fromBMP(file);
    return image;
}


void doMagicIn(QString path, QString q, QString OPath)
{
    QRegExp isMask = QRegExp("*m.gif");
    isMask.setPatternSyntax(QRegExp::Wildcard);

    if(isMask.exactMatch(q))
        return;

    QImage target;
    QString imgFileM;
    QStringList tmp = q.split(".", QString::SkipEmptyParts);
    if(tmp.size()==2)
        imgFileM = tmp[0] + "m." + tmp[1];
    else
        return;

    //skip unexists pairs
    if(!QFile(path+q).exists())
        return;
    if(!QFile(path+imgFileM).exists())
        return;

    if(!noBackUp)
    {
        //create backup dir
        if(!QDir(path+"_backup").exists())
            QDir().mkdir(path+"_backup");

        //create Back UP of source images
        if(!QFile(path+"_backup/"+q).exists())
            QFile::copy(path+q, path+"_backup/"+q);
        if(!QFile(path+"_backup/"+imgFileM).exists())
            QFile::copy(path+imgFileM, path+"_backup/"+imgFileM);
    }

    QImage image = loadQImage(path+q);
    QImage mask = loadQImage(path+imgFileM);

    if(mask.isNull()) //Skip null masks
        return;

    target = setAlphaMask(image, mask);

    if(!target.isNull())
    {
        //Save before fix
        //target.save(OPath+tmp[0]+"_before.png");
        //mask.save(OPath+tmp[0]+"_mask_before.png");
        qDebug() << path+q;


    //fix
    if(image.size()!= mask.size())
        mask = mask.copy(0,0, image.width(), image.height());
    target = mask;

    QList<QRgb > colortable;

    for(int w=0; w< target.width(); w++)
        for(int h=0; h < target.height(); h++)
        {
            bool cFind=false;
            foreach(QRgb c, colortable) //Store color into color table
            { if(target.pixel(w,h)==c) { cFind=true;break;} }
            if(!cFind) colortable.push_back(target.pixel(w,h));

            if(target.pixel(w,h)==image.pixel(w,h)) //Fill cased pixel color into black
                target.setPixel(w,h, qRgb(0,0,0));
        }

    bool WhiteExist=false;
    foreach(QRgb c, colortable) // Find White color in table
    { if(c==qRgb(0xFF,0xFF,0xFF)) { WhiteExist=true;break;} }

    if(WhiteExist)
    for(int w=0; w< target.width(); w++)
        for(int h=0; h < target.height(); h++)
        {
            if((target.pixel(w,h) < qRgb(0xFF,0xFF,0xFF)) && (target.pixel(w,h) >= qRgb(0x44,0x44,0x44)))
                target.setPixel(w,h, qRgb(0x44,0x44,0x44));

            if((target.pixel(w,h) > qRgb(0x00,0x00,0x00)) && (target.pixel(w,h) < qRgb(0x44,0x44,0x44)))
                target.setPixel(w,h, qRgb(0x00,0x00,0x00));
        }
    else // If white not exist - fill white-gray to white
    for(int w=0; w< target.width(); w++)
        for(int h=0; h < target.height(); h++)
        {
            if(target.pixel(w,h) > qRgb(0xF3,0xF3,0xF3))
                target.setPixel(w,h, qRgb(0xFF,0xFF,0xFF));
        }

    mask = target;

    target = setAlphaMask(image, mask);

    //Save after fix
    //target.save(OPath+tmp[0]+"_after.bmp", "BMP");
    mask.save(OPath+tmp[0]+"m.gif", "BMP"); //overwrite mask image

    }
    else
    qDebug() << path+q+" - WRONG!";
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

    std::cout<<"============================================================================\n";
    std::cout<<"Lazily-made image masks fix tool by Wohlstand\n";
    std::cout<<"============================================================================\n";
    std::cout<<"This software distributed under GNU GPLv3\n";
    std::cout<<"============================================================================\n";

    if(a.arguments().size()==1)
    {
        goto DisplayHelp;
    }

    if(a.arguments().filter("--help", Qt::CaseInsensitive).size()>0)
    {
        goto DisplayHelp;
    }

    if(a.arguments().filter("-W", Qt::CaseInsensitive).size()>0)
    {
        walkSubDirs=true;
    }

    if(a.arguments().filter("-N", Qt::CaseInsensitive).size()>0)
    {
       noBackUp=true;
    }

    imagesDir.setPath(a.arguments().at(1));
    filters << "*.gif" << "*.GIF";
    imagesDir.setSorting(QDir::Name);
    imagesDir.setNameFilters(filters);

    path = imagesDir.absolutePath() + "/";

    if(a.arguments().filter("-O", Qt::CaseSensitive).size()>0)
    {
        OPath = a.arguments().filter("-O", Qt::CaseSensitive).first().remove(0,2);
        if(!QFileInfo(OPath).isDir())
            goto WrongOutputPath;

        OPath = QDir(OPath).absolutePath() + "/";
    }
    else
        OPath=path;

    std::cout<<"============================================================================\n";
    std::cout<<"Converting images...\n";
    std::cout<<"============================================================================\n";

    std::cout<< QString("Input path:  "+path+"\n").toStdString();
    std::cout<< QString("Output path: "+OPath+"\n").toStdString();
    std::cout<<"============================================================================\n";
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
                //qDebug()<< QFileInfo(dirsList.filePath()).dir().absolutePath();
                doMagicIn(QFileInfo(dirsList.filePath()).dir().absolutePath()+"/", dirsList.fileName(), QFileInfo(dirsList.filePath()).dir().absolutePath()+"/");
          }


    }

    std::cout<<"============================================================================\n";
    std::cout<<"Done!\n\n";

    getchar();

    exit(0);
    return a.exec();

DisplayHelp:
    std::cout<<"============================================================================\n";
    std::cout<<"This utility will fix lazily-made image masks:\n";
    std::cout<<"============================================================================\n";
    std::cout<<"Syntax:\n\n";
    std::cout<<"   LazyFixTool [--help] /path/to/folder [-O/path/to/out] [-W] [-N]\n\n";
    std::cout<<" --help              - Display this help\n";
    std::cout<<" /path/to/folder     - path to directory with pair of GIF files\n";
    std::cout<<" -O/path/to/out      - path to directory where will be saved new images\n";
    std::cout<<"                       Note: (with -W flag will be ingored)\n";
    std::cout<<" -W                  - Walk in subdirectores\n";
    std::cout<<" -N                  - Don't create backup\n";
    std::cout<<"\n\n";

    getchar();

    exit(0);
    return a.exec();
WrongOutputPath:
    std::cout<<"============================================================================\n";
    std::cout<<"Wrong output path!\n";
    goto DisplayHelp;
}
