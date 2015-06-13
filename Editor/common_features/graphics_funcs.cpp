/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QPixmap>
#include <QImage>
#include <QFile>
#include <QTextStream>
#include <QtDebug>
#include <memory>

#include <common_features/logger.h>

#include "graphics_funcs.h"

#include <EasyBMP/EasyBMP.h>
extern "C"{
#include <giflib/gif_lib.h>
}



bool GraphicsHelps::EnableVBEmulate=true;

QPixmap GraphicsHelps::setAlphaMask(QPixmap image, QPixmap mask)
{
    if(mask.isNull())
        return image;

    if(image.isNull())
        return image;

    QImage target = image.toImage();
    QImage newmask = mask.toImage();

    if(target.size()!= newmask.size())
    {
        newmask = newmask.copy(0,0, target.width(), target.height());
    }

    if(EnableVBEmulate)
        target = setAlphaMask_VB(target, newmask);
    else
        {
                newmask.invertPixels();
                target.setAlphaChannel(newmask);
        }
    return QPixmap::fromImage(target);
}

//Implementation of VB similar transparency function
QImage GraphicsHelps::setAlphaMask_VB(QImage image, QImage mask)
{
    if(mask.isNull())
        return image;

    if(image.isNull())
        return image;

    bool isWhiteMask = true;

    QImage target;

    target = QImage(image.width(), image.height(), QImage::Format_ARGB32);
    target.fill(qRgb(128,128,128));

    QImage newmask = mask.convertToFormat(QImage::Format_ARGB32);
    //newmask = newmask.convertToFormat(QImage::Format_ARGB32);

    if(target.size()!= newmask.size())
    {
        newmask = newmask.copy(0, 0, target.width(), target.height());
    }

    QImage alphaChannel = image.alphaChannel();

    //vbSrcAnd
    for(int y=0; y< image.height(); y++ )
        for(int x=0; x < image.width(); x++ )
        {
            QColor Fpix = QColor(image.pixel(x,y));
            QColor Dpix = QColor(target.pixel(x,y));
            QColor Spix = QColor(newmask.pixel(x,y));
            QColor Npix;

            Npix.setAlpha(255);
            //AND
            Npix.setRed( Dpix.red() & Spix.red());
            Npix.setGreen( Dpix.green() & Spix.green());
            Npix.setBlue( Dpix.blue() & Spix.blue());
            //OR
            Npix.setRed( Fpix.red() | Npix.red());
            Npix.setGreen( Fpix.green() | Npix.green());
            Npix.setBlue( Fpix.blue() | Npix.blue());

            target.setPixel(x, y, Npix.rgba());

            isWhiteMask &= ( (Spix.red()>240) //is almost White
                             &&(Spix.green()>240)
                             &&(Spix.blue()>240));

            //Calculate alpha-channel level
            int newAlpha = 255-((Spix.red() + Spix.green() + Spix.blue())/3);
            if( (Spix.red()>240) //is almost White
                            &&(Spix.green()>240)
                            &&(Spix.blue()>240))
            {
                newAlpha = 0;
            }

            newAlpha = newAlpha+((Fpix.red() + Fpix.green() + Fpix.blue())/3);
            if(newAlpha>255) newAlpha=255;

            alphaChannel.setPixel(x,y, newAlpha);
        }
//    //vbSrcPaint
//    for(int y=0; y< image.height(); y++ )
//        for(int x=0; x < image.width(); x++ )
//        {
//            QColor Dpix = QColor(image.pixel(x,y));
//            QColor Spix = QColor(target.pixel(x,y));
//            QColor Npix;

//            Npix.setAlpha(255);
//            Npix.setRed( Dpix.red() | Spix.red());
//            Npix.setGreen( Dpix.green() | Spix.green());
//            Npix.setBlue( Dpix.blue() | Spix.blue());
//            target.setPixel(x, y, Npix.rgba());

//            //QColor curAlpha;
//            int curAlpha = QColor(alphaChannel.pixel(x,y)).red();
//            int newAlpha = curAlpha+((Dpix.red() + Dpix.green() + Dpix.blue())/3);

//            if(newAlpha>255) newAlpha=255;
//            alphaChannel.setPixel(x,y, newAlpha);
//        }

    target.setAlphaChannel(alphaChannel);

    return target;
}


void GraphicsHelps::loadMaskedImage(QString rootDir, QString in_imgName, QString &out_maskName, QPixmap &out_Img, QPixmap &out_Mask, QString &out_errStr)
{
    if( in_imgName.isEmpty() )
    {
        out_errStr = "Image filename isn't defined";
        return;
    }

    if(!QFile(rootDir+in_imgName).exists())
    {
        out_errStr="image file is not exist: "+rootDir+in_imgName;
        return;
    }

    out_maskName=in_imgName;
    int i = out_maskName.size()-1;
    for( ;i>0; i--)
    {
        if(out_maskName[i]=='.')
        {
            out_maskName.insert(i, 'm');
            break;
        }
    }

    if(i==0)
    {
        out_maskName = "";
        out_Mask = QPixmap();
    }
    else
        out_Mask = QPixmap(rootDir + out_maskName);

    out_Img = GraphicsHelps::setAlphaMask(QPixmap(rootDir+in_imgName), out_Mask);
    if(out_Img.isNull())
    {
        out_errStr="Broken image file "+rootDir+in_imgName;
        return;
    }

    out_errStr = "";
}


QImage GraphicsHelps::fromBMP(QString &file)
{
    QImage errImg;

    BMP tarBMP;
    if(!tarBMP.ReadFromFile( file.toLocal8Bit().data() )){
        WriteToLog(QtCriticalMsg, QString("Error: File does not exsist"));
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

QImage GraphicsHelps::fromGIF(QString &file)
{
    /*Slow and creepy code :P */

    int errcode;

    if(!QFile(file).exists())
        return QImage();

    GifFileType* t = DGifOpenFileName(file.toLocal8Bit().data(), &errcode);
    if(!t)
    {
        qWarning()  << "Can't open GIF "<<file<< GifErrorString(errcode);
        DGifCloseFile(t, &errcode);
        return QImage();
    }

    QImage tarImg=QImage(t->SWidth, t->SHeight, QImage::Format_Indexed8);
    GifRecordType RecordType;
    do {
        if (DGifGetRecordType(t, &RecordType) == GIF_ERROR)
        {
            qWarning()  << "GIF error 2"<< GifErrorString(errcode);
            DGifCloseFile(t, &errcode);
            return QImage();
        }

        switch (RecordType)
        {
           case IMAGE_DESC_RECORD_TYPE:
                {
                    errcode=DGifGetImageDesc(t);
                    GifWord Width = t->Image.Width;
                    GifWord Height = t->Image.Height;
                    ColorMapObject* colrMap = (t->Image.ColorMap
                                             ? t->Image.ColorMap
                                             : t->SColorMap);
                    QVector<QRgb> clTab;
                    int padding = ((Width % 4)!=0 ? 4 - (Width % 4) : 0);
                    for(int c=0; c<colrMap->ColorCount; c++)
                    {
                        QRgb pix=0;
                        // 00000000 R24-00000000 G16-00000000 B8-00000000
                        pix=(((unsigned int)colrMap->Colors[c].Red)<<16)|pix;
                        pix=(((unsigned int)colrMap->Colors[c].Green)<<8)|pix;
                        pix=(((unsigned int)colrMap->Colors[c].Blue))|pix;
                    }
                    tarImg.setColorTable(clTab);

                    if (t->Image.Left + t->Image.Width > t->SWidth ||
                         t->Image.Top + t->Image.Height > t->SHeight) {
                          qWarning()<<"Image %d is not confined to screen dimension, aborted.\n" << GifErrorString(errcode);
                          DGifCloseFile(t, &errcode);
                          return QImage();
                    }

                    std::shared_ptr<GifPixelType>ptr(new GifPixelType[Width+padding]);
                    GifPixelType *gifPixelLine=  ptr.get();
                    if (t->Image.Interlace) {
                        /* Need to perform 4 passes on the images: */
                        for (int i = 0; i < 4; i++)
                            for (int j = 0, pixH=0; j < Height;
                                       j++,pixH++) {
                                if(DGifGetLine(t, gifPixelLine, Width) == GIF_ERROR)
                                {
                                  qWarning()<<"Gif Error 3"<< GifErrorString(errcode);
                                  DGifCloseFile(t, &errcode);
                                  return QImage();
                                }
                                for(int pixW=0; pixW<Width; pixW++)
                                    tarImg.setPixel(pixW, pixH, qGray(gifPixelLine[pixW]));
                            }
                        }
                        else {
                            for (int i = 0, pixH=0; i < Height; i++,pixH++) {
                              if (DGifGetLine(t, gifPixelLine,
                                  Width) == GIF_ERROR) {
                                  qWarning()<<"Gif Error 4"<< GifErrorString(errcode);
                                  DGifCloseFile(t, &errcode);
                                  return QImage();
                              }
                              for(int pixW=0; pixW<Width; pixW++)
                                    tarImg.setPixel(pixW, pixH, qGray(gifPixelLine[pixW]));
                            }
                        }
                }
                break;
           case EXTENSION_RECORD_TYPE:
               break;
           case TERMINATE_RECORD_TYPE:
               break;
           default:            /* Should be traps by DGifGetRecordType. */
               break;
       }
    }
    while (RecordType != TERMINATE_RECORD_TYPE);

    DGifCloseFile(t, &errcode);

    return tarImg;
}

QPixmap GraphicsHelps::loadPixmap(QString file)
{
    return QPixmap::fromImage(loadQImage(file));
}

QImage GraphicsHelps::loadQImage(QString file)
{
    QFile img(file);
    if(!img.exists())
        return QImage();
    if(!img.open(QIODevice::ReadOnly))
        return QImage();

    QByteArray mg=img.read(5); //Get magic number!
    if(mg.size()<2) // too short!
        return QImage();
    char *magic = mg.data();
    img.close();

    //Detect PNG 89 50 4e 47
    if( (magic[0]=='\x89')&&(magic[1]=='\x50')&&(magic[2]=='\x4e')&&(magic[3]=='\x47') )
    {
        QImage image = QImage( file );
        return image;
    } else
    //Detect GIF 47 49 46 38
    if( (magic[0]=='\x47')&&(magic[1]=='\x49')&&(magic[2]=='\x46')&&(magic[3]=='\x38') )
    {
        QImage image = QImage( file );
        //QImage image = fromGIF( file );
        return image;
    }
    else
    //Detect BMP 42 4d
    if( (magic[0]=='\x42')&&(magic[1]=='\x4d') )
    {
        QImage image = fromBMP( file );
        return image;
    }

    //Another formats, supported by Qt :P
    QImage image = QImage( file );
    return image;
}

QPixmap GraphicsHelps::squareImage(QPixmap image, QSize targetSize=QSize(0,0) )
{
    QPixmap target = QPixmap(targetSize);
    target.fill(Qt::transparent);
    QPixmap source;

    if( ( targetSize.width() < image.width() ) || ( targetSize.height() < image.height() ))
        source = image.scaled(targetSize, Qt::KeepAspectRatio);
    else
        source = image;

    QPainter p(&target);

    int targetX = qRound( ( ( qreal(target.width()) - qreal(source.width()) ) / 2 ) );
    int targetY = qRound( ( ( qreal(target.height()) - qreal(source.height()) ) / 2 ) );

    p.drawPixmap( targetX, targetY,source.width(),source.height(), source );

    p.end();
    return target;
}

QPixmap GraphicsHelps::drawDegitFont(int number)
{
    QPixmap font=QPixmap(":/fonts/degits.png");
    QString text=QString::number(number);
    QPixmap img(text.size()*18, 16);

    img.fill(Qt::transparent);
    QPainter p(&img);

    for(int i=0; i<text.size(); i++)
    {
        p.drawPixmap(QRect(18*i, 0, 16,16), font, QRect(0, QString(text[i]).toInt()*16, 16,16));
    }
    p.end();
    return img;
}

bool GraphicsHelps::toGif(QImage& img, QString& path)
{
    int errcode;

    if(QFile(path).exists()) // Remove old file
        QFile::remove(path);

    GifFileType* t = EGifOpenFileName(path.toLocal8Bit().data(),true, &errcode);
    if(!t)
    {
        EGifCloseFile(t, &errcode);
        qWarning()  << "Can't open\n";
        return false;
    }

    EGifSetGifVersion(t, true);
    std::shared_ptr<GifColorType>ptr(new GifColorType[256]);
    GifColorType* colorArr = ptr.get();
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
        qWarning() << "Unfinished\n";
        return false;
    }


    for(int l = tarQImg.colorCount(); l < 256; l++){
        tarQImg.setColor(l,0);
    }

    if(tarQImg.colorTable().size() != 256){
        EGifCloseFile(t, &errcode);
        qWarning()  << "A lot of colors\n";
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
        qWarning()  << "EGifPutScreenDesc error 1\n";
        return false;
    }

    errcode = EGifPutImageDesc(t, 0, 0, img.width(), img.height(), false, 0);
    if(errcode != GIF_OK){
        EGifCloseFile(t, &errcode);
        qWarning()  << "EGifPutImageDesc error 2\n";
        return false;
    }

    //gen byte array
    GifByteType* byteArr = tarQImg.bits();

    for(int h = 0; h < tarQImg.height(); h++){
        errcode = EGifPutLine(t, byteArr, tarQImg.width());
        if(errcode != GIF_OK){
            EGifCloseFile(t, &errcode);
            qWarning()  << "EGifPutLine error 3\n";
            return false;
        }

        byteArr += tarQImg.width();
        byteArr += ((tarQImg.width() % 4)!=0 ? 4 - (tarQImg.width() % 4) : 0);
    }

    if(errcode != GIF_OK){
        qWarning()  << "GIF error 4\n";
        return false;
    }
    EGifCloseFile(t, &errcode);

    return true;
}
