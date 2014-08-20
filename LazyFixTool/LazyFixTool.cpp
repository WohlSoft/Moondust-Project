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

#include "libs/EasyBMP/EasyBMP.h"
extern "C"{
#include "libs/giflib/gif_lib.h"
}

bool noBackUp=false;
bool DarkGray=false;

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

    GifFileType* t = EGifOpenFileName(path.toStdString().c_str(),true, &errcode);
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

    QRegExp isBackupDir = QRegExp("*/_backup/");
    isBackupDir.setPatternSyntax(QRegExp::Wildcard);

    if(isBackupDir.exactMatch(path))
        return; //Skip backup directories

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
    {
        QString saveTo;

        QImage image = loadQImage(path+q);
        if(image.isNull()) return;

        QTextStream(stdout) << path+q+"\n";

        saveTo = QString(OPath+(tmp[0].toLower())+".gif");
        //overwrite source image (convert BMP to GIF)
        if(toGif( image, saveTo ) ) //Write gif
        {
            QTextStream(stdout) <<"GIF-1 only\n";
        }
        else
        {
            QTextStream(stdout) <<"BMP-1 only\n";
            image.save(saveTo, "BMP"); //If failed, write BMP
        }
        return;
    }

    if(!noBackUp)
    {
        //create backup dir
        QDir backup(path+"_backup/");
        if(!backup.exists())
        {
            QTextStream(stdout) << QString("Create backup with path %1\n").arg(path+"_backup");
            if(!backup.mkdir("."))
                QTextStream(stderr) << QString("WARNING! Can't create backup directory %1\n").arg(path+"_backup");

        }

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
        QTextStream(stdout) << path+q+"\n";


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
            if(DarkGray)
            {
                //fill white-gray to dark-gray
                if((target.pixel(w,h) < qRgb(0xFF,0xFF,0xFF)) && (target.pixel(w,h) >= qRgb(0x44,0x44,0x44)))
                    target.setPixel(w,h, qRgb(0x44,0x44,0x44));

                //fill black-gray to black
                if((target.pixel(w,h) > qRgb(0x00,0x00,0x00)) && (target.pixel(w,h) < qRgb(0x44,0x44,0x44)))
                    target.setPixel(w,h, qRgb(0x00,0x00,0x00));
            }
        }
    else // If white not exist
    for(int w=0; w< target.width(); w++)
        for(int h=0; h < target.height(); h++)
        {
            //fill white-gray to white
            if(target.pixel(w,h) > qRgb(0xF3,0xF3,0xF3))
                target.setPixel(w,h, qRgb(0xFF,0xFF,0xFF));

            //fill black-gray to black
            if((target.pixel(w,h) > qRgb(0x00,0x00,0x00)) && (target.pixel(w,h) < qRgb(0x44,0x44,0x44)))
                target.setPixel(w,h, qRgb(0x00,0x00,0x00));
        }

    mask = target;

    target = setAlphaMask(image, mask);

    //Save after fix
    //target.save(OPath+tmp[0]+"_after.bmp", "BMP");
    QString saveTo;


    saveTo = QString(OPath+(tmp[0].toLower())+".gif");
    //overwrite source image (convert BMP to GIF)
    if(toGif(image, saveTo ) ) //Write gif
    {
        QTextStream(stdout) <<"GIF-1 ";
    }
    else
    {
        QTextStream(stdout) <<"BMP-1 ";
        image.save(saveTo, "BMP"); //If failed, write BMP
    }

    saveTo = QString(OPath+(tmp[0].toLower())+"m.gif");

    //overwrite mask image
    if( toGif(mask, saveTo ) ) //Write gif
    {
        QTextStream(stdout) <<"GIF-2\n";
    }
    else
    {
        mask.save(saveTo, "BMP"); //If failed, write BMP
        QTextStream(stdout) <<"BMP-2\n";
    }

    }
    else
    QTextStream(stderr) << path+q+" - WRONG!\n";
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
    QTextStream(stdout) <<"Lazily-made image masks fix tool by Wohlstand. Version "<< _FILE_VERSION << _FILE_RELEASE << "\n";
    QTextStream(stdout) <<"============================================================================\n";
    QTextStream(stdout) <<"This program is distributed under the GNU GPLv3 license\n";
    QTextStream(stdout) <<"============================================================================\n";

    QRegExp isGif = QRegExp("*.gif");
    isGif.setPatternSyntax(QRegExp::Wildcard);

    QRegExp isMask = QRegExp("*m.gif");
    isMask.setPatternSyntax(QRegExp::Wildcard);

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
        if(a.arguments().at(arg)=="-N")
        {
            noBackUp=true;
        }
        else
        if(a.arguments().at(arg)=="-W")
        {
            walkSubDirs=true;
        }
        else
        if(a.arguments().at(arg)=="-G")
        {
            DarkGray=true;
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

                    if(cOpath) OPath=QFileInfo(dirsList.filePath()).dir().absolutePath()+"/";

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
    QTextStream(stdout) <<"This utility will fix lazily-made image masks:\n";
    QTextStream(stdout) <<"============================================================================\n";
    QTextStream(stdout) <<"Syntax:\n\n";
    QTextStream(stdout) <<"   LazyFixTool [--help] [-N] [-G] file1.gif [file2.gif] [...] [-O/path/to/out]\n";
    QTextStream(stdout) <<"   LazyFixTool [--help] [-W] [-N] [-G] /path/to/folder [-O/path/to/out]\n\n";
    QTextStream(stdout) <<" --help              - Display this help\n";
    QTextStream(stdout) <<" /path/to/folder     - path to a directory with a pair of GIF files\n";
    QTextStream(stdout) <<" -O/path/to/out      - path to a directory where the new images will be saved\n";
    QTextStream(stdout) <<" -W                  - Walk in subdirectores\n";
    QTextStream(stdout) <<" -N                  - Don't create backup\n";
    QTextStream(stdout) <<" -G                  - Make gray shades on masks is more dark\n";
    QTextStream(stdout) <<"\n\n";

    getchar();

    exit(0);
    return 0;
WrongInputPath:
    QTextStream(stdout) <<"============================================================================\n";
    QTextStream(stdout) <<"Wrong output path!\n";
    goto DisplayHelp;
WrongOutputPath:
    QTextStream(stdout) <<"============================================================================\n";
    QTextStream(stdout) <<"Wrong output path!\n";
    goto DisplayHelp;
}
