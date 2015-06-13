/*
 * SMBX64 Playble Character Sprite Calibrator, a free tool for playable srite design
 * This is a part of the Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2015 Vitaly Novichkov <admin@wohlnet.ru>
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


#include "../calibrationmain.h"
#include "ui_calibrationmain.h"
#include "globals.h"
#include "graphics.h"
#include "app_path.h"


void CalibrationMain::OpenFile(QString fileName)
{
    QList<QString > tmp;
    QString imgFileM;
    QFileInfo ourFile(fileName);
    currentFile = fileName;

    LastOpenDir = ourFile.absoluteDir().path();

    tmp = ourFile.fileName().split(".", QString::SkipEmptyParts);
    if(tmp.size()==2)
        imgFileM = tmp[0] + "m." + tmp[1];
    else
        imgFileM = "";
    //mask = ;

    QImage maskImg;

    if(QFile::exists(ourFile.absoluteDir().path() + "/" + imgFileM))
        maskImg = Graphics::loadQImage( ourFile.absoluteDir().path() + "/" + imgFileM );
    else
        maskImg = QImage();

    x_imageSprite = QPixmap::fromImage(
                Graphics::setAlphaMask(
                    Graphics::loadQImage( fileName )
                    , maskImg )
                );

    loadConfig(fileName);

    initScene();
    updateControls();
    updateScene();
}



//Made templates for test calibration
void CalibrationMain::on_MakeTemplateB_clicked()
{
    QGraphicsScene * temp1 = new QGraphicsScene;
    QGraphicsScene * temp2 = new QGraphicsScene;

    createDirs();

    QFileInfo ourFile(currentFile);
    QString targetFile =  AppPathManager::userAppDir() + "/calibrator/templates/" + ourFile.baseName() + ".gif";
    QString targetFile2 =  AppPathManager::userAppDir() + "/calibrator/templates/" + ourFile.baseName() + "m.gif";

    temp1->setBackgroundBrush(QBrush(Qt::black));
    temp2->setBackgroundBrush(QBrush(Qt::white));

    for(int i=0; i<10; i++)
        for(int j=0; j<10; j++)
        {
            if(framesX[i][j].used)
            {
                temp1->addRect(framesX[i][j].offsetX + 100*i, framesX[i][j].offsetY + 100 * j,
                              frameWidth-1, (framesX[i][j].isDuck?frameHeightDuck:frameHeight)-1,
                               QPen(Qt::yellow, 1),Qt::transparent);
                temp2->addRect(framesX[i][j].offsetX + 100*i, framesX[i][j].offsetY + 100 * j,
                              frameWidth-1, (framesX[i][j].isDuck?frameHeightDuck:frameHeight)-1,
                               QPen(Qt::black, 1),Qt::transparent);
            }
        }

    QImage img1(1000,1000,QImage::Format_ARGB32_Premultiplied);
    QImage img2(1000,1000,QImage::Format_ARGB32_Premultiplied);
    QPainter p1(&img1);
    QPainter p2(&img2);
    temp1->render(&p1, QRectF(0,0,1000,1000),QRectF(0,0,1000,1000));
    temp2->render(&p2, QRectF(0,0,1000,1000),QRectF(0,0,1000,1000));
    p1.end();
    p2.end();

    QApplication::setOverrideCursor(Qt::WaitCursor);
    Graphics::toGif(img1, targetFile);
    Graphics::toGif(img2, targetFile2);
    //img1.save(targetFile);
    //img2.save(targetFile2);
    QApplication::restoreOverrideCursor();

    QMessageBox::information(this, tr("Saved"), tr("Sprite drawing templates saved in:\n")
                             +targetFile+"\n"+targetFile2);

}
