#include <QtGui>
#include <QPixmap>
#include <QBitmap>
#include <QSettings>
#include <QProgressDialog>
#include <QApplication>
#include "dataconfigs.h"
#include "mainwindow.h"
#include <QDebug>

dataconfigs::dataconfigs()
{

}

/*
[background-1]
name="Smallest bush"		;background name, default="background-%n"
type="scenery"			;Background type, default="Scenery"
grid=32				; 32 | 16 Default="32"
view=background			; background | foreground, default="background"
image="background-1.gif"	;Image file with level file ; the image mask will be have *m.gif name.
climbing=0			; default = 0
animated = 0			; default = 0 - no
frames = 1			; default = 1
frame-speed=125			; default = 125 ms, etc. 8 frames per sec
*/

void dataconfigs::loadconfigs()
{
    obj_bgo sbgo;
    long bgo_total=0, i, prgs=0;
    QString bgo_ini = QApplication::applicationDirPath() + "/" +  "configs/SMBX/" + "lvl_bgo.ini";
    QSettings bgoset(bgo_ini, QSettings::IniFormat);

    main_bgo.clear();

    QBitmap mask;
    //QPixmap image;

    QString imgFile, imgFileM;

    QString bgoPath = QApplication::applicationDirPath() + "/" + "data/graphics/level/background/";

    QStringList tmp;

    bgoset.beginGroup("background-main");
        bgo_total = bgoset.value("total", "0").toInt();
    bgoset.endGroup();

    QProgressDialog progress("Loading BGO Data", "Abort", 0, bgo_total);
         progress.setWindowTitle("Loading config...");
         //progress.setWindowModality(Qt::WindowModal);
         progress.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
         progress.setCancelButton(0);

    bool debug = false;

    for(i=1; i<=bgo_total; i++)
    {
        if(debug)qDebug() << i << "\n";
        bgoset.beginGroup( QString("background-"+QString::number(i)) );
            sbgo.name = bgoset.value("name", "").toString();
            if(debug)qDebug() << "name" << "\n";
            sbgo.type = bgoset.value("type", "other").toString();
            if(debug)qDebug() << "type" << "\n";
            sbgo.grid = bgoset.value("grid", "32").toInt();
            if(debug)qDebug() << "grid" << "\n";
            sbgo.view = (bgoset.value("view", "background").toString()=="foreground");
            if(debug)qDebug() << "view" << "\n";
            imgFile = bgoset.value("image", "").toString();
            if(debug)qDebug() << "imgfile" << "\n";
            if( (imgFile!="") )
            {
                tmp = imgFile.split(".", QString::SkipEmptyParts);
                if(tmp.size()==2)
                    imgFileM = tmp[0] + "m." + tmp[1];
                else
                    imgFileM = "";
                if(debug)qDebug() << "tmp" << "\n";
                if(tmp.size()==2) mask = QBitmap(bgoPath + imgFileM);
                sbgo.mask = mask;
                if(debug)qDebug() << "mask" << "\n";
                sbgo.image = QPixmap(bgoPath + imgFile);
                if(debug)qDebug() << "set image" << "\n";
                if(tmp.size()==2) sbgo.image.setMask(mask);
                if(debug)qDebug() << "set mask" << "\n";
                //sbgo.image = image;
            }
            else
            sbgo.image = QPixmap(QApplication::applicationDirPath() + "/" + "data/unknown_bgo.gif");
            if(debug)qDebug() << "image to array" << "\n";
            sbgo.climbing = (bgoset.value("climbing", "0").toString()=="1");
            if(debug)qDebug() << "climb" << "\n";
            sbgo.animated = (bgoset.value("animated", "0").toString()=="1");
            if(debug)qDebug() << "anim" << "\n";
            sbgo.frames = bgoset.value("frames", "1").toInt();
            if(debug)qDebug() << "frames" << "\n";
            sbgo.framespeed = bgoset.value("frame-speed", "125").toInt();
            if(debug)qDebug() << "fr-speed" << "\n";
            sbgo.id = i;

            main_bgo.push_back(sbgo);
            if(debug)qDebug() << "add to array" << "\n";
        bgoset.endGroup();

        prgs++;
        progress.setValue(prgs);
    }

    progress.close();

}
