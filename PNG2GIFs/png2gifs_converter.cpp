
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include "png2gifs_converter.h"
extern "C"{
    #include "../_Libs/giflib/gif_lib.h"
}


bool removeSource=false;

P2G_Converter::P2G_Converter()
{}

P2G_Converter::~P2G_Converter()
{}



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

bool toGif(QImage& img, QString& path)
{
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


int doMagicIn(QString path, QString q, QString OPath)
{
    //skip unexists pairs
    if(!QFile(path+q).exists())
        return CNV_SKIPPED;

    QImage ImgSrc;
    QImage image;
    QImage mask;

    ImgSrc = loadQImage(path+q);

    if(ImgSrc.isNull()) //Skip null images
        return CNV_FAILED;
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

    return CNV_SUCCESS;
}
