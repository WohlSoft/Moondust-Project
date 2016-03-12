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
#include <QFileInfo>
#include <QTextStream>
#include <memory>

#include <common_features/logger.h>

#include "graphics_funcs.h"

#include <common_features/file_mapper.h>

#include <EasyBMP/EasyBMP.h>
extern "C"{
#include <giflib/gif_lib.h>
}

#ifdef _WIN32
#define FREEIMAGE_LIB 1
#endif
#include <FreeImageLite.h>

#include <QtDebug>


FIBITMAP* GraphicsHelps::loadImage(QString file, bool convertTo32bit)
{
    #if  defined(__unix__) || defined(__APPLE__) || defined(_WIN32)
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

void GraphicsHelps::mergeWithMask(FIBITMAP *image, QString pathToMask)
{
    if(!image) return;
    if(!QFileInfo(pathToMask).exists()) return; //Nothing to do
    FIBITMAP* mask = loadImage( pathToMask, true );
    if(!mask) return;//Nothing to do

    unsigned int img_w = FreeImage_GetWidth(image);
    unsigned int img_h = FreeImage_GetHeight(image);
    unsigned int mask_w = FreeImage_GetWidth(mask);
    unsigned int mask_h = FreeImage_GetHeight(mask);

    BYTE *img_bits  = FreeImage_GetBits(image);
    BYTE *mask_bits = FreeImage_GetBits(mask);

    BYTE *FPixP = img_bits;
    BYTE *SPixP = mask_bits;

    RGBQUAD Npix = {0x0, 0x0, 0x0, 0xff};   //Destination pixel color
    short newAlpha=255;//Calculated destination alpha-value

    for(unsigned int y=0; (y<img_h) && (y<mask_h); y++ )
    {
        for(unsigned int x=0; (x<img_w) && (x<mask_w); x++ )
        {
            Npix.rgbBlue =  ((SPixP[FI_RGBA_BLUE] & 0x7F) | FPixP[FI_RGBA_BLUE]);
            Npix.rgbGreen = ((SPixP[FI_RGBA_GREEN] & 0x7F) | FPixP[FI_RGBA_GREEN]);
            Npix.rgbRed =   ((SPixP[FI_RGBA_RED] & 0x7F) | FPixP[FI_RGBA_RED]);

            newAlpha= 255-( ( short(SPixP[FI_RGBA_RED])+
                          short(SPixP[FI_RGBA_GREEN])+
                          short(SPixP[FI_RGBA_BLUE]) ) / 3);

            if(  (SPixP[FI_RGBA_RED]>240u) //is almost White
               &&(SPixP[FI_RGBA_GREEN]>240u)
               &&(SPixP[FI_RGBA_BLUE]>240u))
            {
                newAlpha = 0;
            }

            newAlpha += ( ( short(FPixP[FI_RGBA_RED])+
                                   short(FPixP[FI_RGBA_GREEN])+
                                   short(FPixP[FI_RGBA_BLUE])) / 3);

            if(newAlpha > 255) newAlpha=255;
            FPixP[FI_RGBA_BLUE]  = Npix.rgbBlue;
            FPixP[FI_RGBA_GREEN] = Npix.rgbGreen;
            FPixP[FI_RGBA_RED]   = Npix.rgbRed;
            FPixP[FI_RGBA_ALPHA] = (BYTE)newAlpha;
            FPixP+=4; SPixP+=4;
        }
    }
    FreeImage_Unload(mask);
}






QPixmap GraphicsHelps::mergeToRGBA(QPixmap image, QImage mask)
{
    if(mask.isNull())
        return image;

    if(image.isNull())
        return image;

    QImage target = image.toImage();
    if(target.format()!=QImage::Format_ARGB32)
        target=target.convertToFormat(QImage::Format_ARGB32);
    mergeToRGBA_BitWise(target, mask);
    return QPixmap::fromImage(target);
}

void GraphicsHelps::mergeToRGBA(QPixmap &img, QImage &mask, QString path, QString maskpath)
{
    if(path.isNull())
        return;

    QImage target;
    target=loadQImage(path);
    if(target.format()!=QImage::Format_ARGB32)
        target=target.convertToFormat(QImage::Format_ARGB32);

    if( maskpath.isNull() )
    {
        img.convertFromImage(target);
        return;
    }
    mask=loadQImage(maskpath);
    mergeToRGBA_BitWise(target, mask);
    img.convertFromImage(target);
}

//Implementation of Bitwise merging of bit-mask to RGBA image
void GraphicsHelps::mergeToRGBA_BitWise(QImage &image, QImage mask)
{
    if(mask.isNull())
        return;

    if(image.isNull())
        return;

    //bool isWhiteMask = true;
    //QImage target(image.width(), image.height(), QImage::Format_ARGB32);

    //QImage newmask = mask.convertToFormat(QImage::Format_ARGB32);
    //newmask = newmask.convertToFormat(QImage::Format_ARGB32);
//    if(target.size()!= mask.size())
//    {
//        mask = mask.copy(0, 0, target.width(), target.height());
//    }

    for(int y=0; (y< image.height()) && (y < mask.height()); y++ )
        for(int x=0; (x < image.width()) && (x<mask.width()); x++ )
        {
            QColor Fpix = QColor(image.pixel(x,y));
            QColor Dpix = QColor(qRgb(128,128,128));
            QColor Spix = QColor(mask.pixel(x,y));
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

//            isWhiteMask &= ( (Spix.red()>240) //is almost White
//                             &&(Spix.green()>240)
//                             &&(Spix.blue()>240));

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
            Npix.setAlpha(newAlpha);

            image.setPixel(x, y, Npix.rgba());
        }
}

void GraphicsHelps::loadMaskedImage(QString rootDir, QString in_imgName, QString &out_maskName, QPixmap &out_Img, QImage&, QString &out_errStr){
    loadMaskedImage(rootDir, in_imgName, out_maskName, out_Img, out_errStr);
}

void GraphicsHelps::loadMaskedImage(QString rootDir, QString in_imgName, QString &out_maskName, QPixmap &out_Img, QString &out_errStr)
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

    out_maskName = in_imgName;
    int i = out_maskName.size()-1;
    for( ;i>0; i--)
    {
        if(out_maskName[i]=='.')
        {
            out_maskName.insert(i, 'm');
            break;
        }
    }

    QImage target;

    if(i==0)
    {
        out_maskName = "";
        loadQImage(target, rootDir+in_imgName);
    } else {
        loadQImage(target, rootDir+in_imgName, rootDir+out_maskName);
    }
    //GraphicsHelps::mergeToRGBA(out_Img, out_Mask, rootDir+in_imgName, rootDir + out_maskName);
    out_Img = std::move(QPixmap::fromImage(target));

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

    for(int x = 0; x < tarBMP.TellWidth(); x++) {
        for(int y = 0; y < tarBMP.TellHeight(); y++) {
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
    if(file.startsWith(':'))
    {//Load from resources!

        QImage image = QImage( file );
        return image;

    } else {//Load via FreeImage

        FIBITMAP* img = loadImage(file, true);
        if(img)
        {
            BYTE *bits = FreeImage_GetBits(img);
            int width = FreeImage_GetWidth(img);
            int height = FreeImage_GetHeight(img);
            QImage target(width, height, QImage::Format_ARGB32);

            for(int y = height-1; y >=0; y--) {
                for(int x = 0; x < width; x++) {
                    target.setPixel(x, y, qRgba(bits[FI_RGBA_RED],
                                                bits[FI_RGBA_GREEN],
                                                bits[FI_RGBA_BLUE],
                                                bits[FI_RGBA_ALPHA]));
                    bits += 4;
                }
            }

            FreeImage_Unload(img);
            return target;

        } else {
            return QImage();
        }
    }
    return QImage();
}

void GraphicsHelps::loadQImage(QImage &target, QString file, QString maskPath)
{
    if(file.startsWith(':'))
    {//Load from resources!

        target = QImage( file );
        return;

    } else {//Load via FreeImage

        FIBITMAP* img = loadImage(file, true);
        if(img)
        {
            mergeWithMask(img, maskPath);
            BYTE *bits = FreeImage_GetBits(img);
            int width = FreeImage_GetWidth(img);
            int height = FreeImage_GetHeight(img);
            target = QImage(width, height, QImage::Format_ARGB32);
            for(int y = height-1; y >=0; y--) {
                for(int x = 0; x < width; x++) {
                    target.setPixel(x, y, qRgba(bits[FI_RGBA_RED],
                                                bits[FI_RGBA_GREEN],
                                                bits[FI_RGBA_BLUE],
                                                bits[FI_RGBA_ALPHA]));
                    bits += 4;
                }
            }
            FreeImage_Unload(img);
            return;

        } else {
            return;
        }
    }
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
