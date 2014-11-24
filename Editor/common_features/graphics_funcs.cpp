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

#include <QPixmap>
#include <QImage>
#include "graphics_funcs.h"
#include "../../_Libs/EasyBMP/EasyBMP.h"
#include "logger.h"

bool GraphicsHelps::EnableVBEmulate=false;

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

    QImage newmask = mask;
    target.convertToFormat(QImage::Format_ARGB32);

    if(target.size()!= newmask.size())
    {
        newmask = newmask.copy(0, 0, target.width(), target.height());
    }

    QImage alphaChannel = image.alphaChannel();

    //vbSrcAnd
    for(int y=0; y< image.height(); y++ )
        for(int x=0; x < image.width(); x++ )
        {
            QColor Dpix = QColor(target.pixel(x,y));
            QColor Spix = QColor(newmask.pixel(x,y));
            QColor Npix;

            Npix.setAlpha(255);
            Npix.setRed( Dpix.red() & Spix.red());
            Npix.setGreen( Dpix.green() & Spix.green());
            Npix.setBlue( Dpix.blue() & Spix.blue());
            target.setPixel(x, y, Npix.rgba());

            isWhiteMask &= ( (Spix.red()>240) //is almost White
                             &&(Spix.green()>240)
                             &&(Spix.blue()>240));
        }

    //vbSrcPaint
    for(int y=0; y< image.height(); y++ )
        for(int x=0; x < image.width(); x++ )
        {
            QColor Dpix = QColor(image.pixel(x,y));
            QColor Spix = QColor(target.pixel(x,y));
            QColor Npix;

            Npix.setAlpha(255);
            Npix.setRed( Dpix.red() | Spix.red());
            Npix.setGreen( Dpix.green() | Spix.green());
            Npix.setBlue( Dpix.blue() | Spix.blue());
            target.setPixel(x, y, Npix.rgba());
        }

    //Apply alpha-channel
    for(int y=0; y< image.height(); y++ )
        for(int x=0; x < image.width(); x++ )
        {
            QColor Dpix = QColor(target.pixel(x,y));
            QColor Spix = QColor(newmask.pixel(x,y));

            int newAlpha = 255-((Spix.red() + Spix.green() + Spix.blue())/3);

            //if pixels equal
            if( Dpix.red() == Spix.red() &&
                Dpix.green() == Spix.green() &&
                Dpix.blue() == Spix.blue() ) newAlpha=255;
            else

            //if color is not black and white mask
            if( ( (Dpix.red()>5) //Is not black
                &&(Dpix.green()>5)
                &&(Dpix.blue()>5))
                    &&
                isWhiteMask )
                newAlpha = 128;
            else
                if( (Spix.red()>240) //is almost White
                                &&(Spix.green()>240)
                                &&(Spix.blue()>240))
                {
                    newAlpha = 0;
                }


            alphaChannel.setPixel(x,y, newAlpha);
        }
    target.setAlphaChannel(alphaChannel);

    return target;
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

QPixmap GraphicsHelps::loadPixmap(QString file)
{
    return QPixmap::fromImage(loadQImage(file));
}

QImage GraphicsHelps::loadQImage(QString file)
{
    QImage image = QImage( file );
    if(image.isNull())
        image = fromBMP( file);
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
