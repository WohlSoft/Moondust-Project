/*
 * SMBX64 Playble Character Sprite Calibrator, a free tool for playable srite design
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

#include <QPixmap>
//#include <QtDebug>
#include <QSettings>

#include "calibrationmain.h"
#include <ui_calibrationmain.h>

#include "about/about.h"
#include "frame_matrix/matrix.h"
#include "animator/animate.h"
#include "animator/aniFrames.h"
#include "image_calibration/image_calibrator.h"

#include "main/globals.h"
#include "main/graphics.h"
#include "main/app_path.h"

#include "main/mw.h"

CalibrationMain::CalibrationMain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CalibrationMain)
{
    MW::p = this;

    buffer.W=100;
    buffer.H=100;
    buffer.offsetX=0;
    buffer.offsetY=0;
    buffer.used=true;
    buffer.isDuck=true;
    buffer.isRightDir=false;
    wasCanceled=false;
    lockControls=false;

    frmX=0;
    frmY=0;

    currentFile = "";

    ui->setupUi(this);

    QString inifile = AppPathManager::settingsFile();
    QSettings settings(inifile, QSettings::IniFormat);
        settings.beginGroup("Main");
          LastOpenDir = settings.value("lastpath", ".").toString();
          currentFile = settings.value("lastfile", currentFile).toString();
          frmX=settings.value("last-frame-x", ui->FrameX->value()).toInt();
          frmY=settings.value("last-frame-y", ui->FrameY->value()).toInt();
          lockControls=true;
          ui->FrameX->setValue(frmX);
          ui->FrameY->setValue(frmY);
          lockControls=false;
        settings.endGroup();

    QVector<frameOpts > framesY;
    frameOpts spriteData;
    // Write default values
    for(int i=0; i<10; i++)
    {
        framesY.clear();
        for(int j=0; j<10; j++)
        {
            spriteData=buffer;
            framesY.push_back(spriteData);
        }
        framesX.push_back(framesY);
    }


    Scene = new QGraphicsScene(ui->PreviewGraph);

    ui->PreviewGraph->setScene(Scene);
    QGraphicsScene *sc = ui->PreviewGraph->scene();

    currentPixmap = QPixmap(100,100);
    currentPixmap.fill(Qt::transparent);
    currentImageItem.setPixmap(currentPixmap);
    currentImageItem.setZValue(0);

    FrameBox_gray.setRect(0,0,100,100);
    FrameBox_gray.setPen(QPen(Qt::gray, 1));
    FrameBox_gray.setBrush(Qt::transparent);
    FrameBox_gray.setZValue(-10);

    CollisionBox_green.setRect(0,0,frameWidth,frameHeight);
    CollisionBox_green.setPen(QPen(Qt::green));
    CollisionBox_green.setZValue(3);

    grabLineX.setPen(QPen(Qt::red));
    grabLineY.setPen(QPen(Qt::red));

    grabLineX.setZValue(10);
    grabLineY.setZValue(10);

    sc->addItem(&FrameBox_gray);
    sc->addItem(&currentImageItem);
    sc->addItem(&CollisionBox_green);
    sc->addItem(&grabLineX);
    sc->addItem(&grabLineY);

    if(currentFile.isEmpty())
    {
        if(!on_OpenSprite_clicked())
        {
            wasCanceled=true;
            return;
        }
    }
    else
        OpenFile(currentFile);

}

void CalibrationMain::closeEvent(QCloseEvent *event)
{
    QString config = AppPathManager::settingsFile();
    QSettings opts(config, QSettings::IniFormat);
    opts.beginGroup("Main");
        opts.setValue("lastpath", LastOpenDir);
        opts.setValue("lastfile", currentFile);
        opts.setValue("last-frame-x", frmX);
        opts.setValue("last-frame-y", frmY);
    opts.endGroup();

    event->accept();
}


CalibrationMain::~CalibrationMain()
{    delete ui;   }


void CalibrationMain::on_FrameX_valueChanged(int)
{ if(lockControls) return; initScene(); updateControls(); updateScene(); }

void CalibrationMain::on_FrameY_valueChanged(int)
{ if(lockControls) return; initScene(); updateControls(); updateScene(); }



void CalibrationMain::on_Height_valueChanged(int arg1)
{
    if(lockControls) return;
    framesX[frmX][frmY].H = arg1;
    frameHeight = arg1;
    updateScene();
}


void CalibrationMain::on_Height_duck_valueChanged(int arg1)
{
    if(lockControls) return;
    frameHeightDuck=arg1;
    updateScene();
}


void CalibrationMain::on_Width_valueChanged(int arg1)
{
    if(lockControls) return;
    framesX[frmX][frmY].W = arg1;
    frameWidth = arg1;
    updateScene();
}


void CalibrationMain::on_OffsetX_valueChanged(int arg1)
{
    if(lockControls) return;
    framesX[frmX][frmY].offsetX = arg1;
    updateScene();
}


void CalibrationMain::on_OffsetY_valueChanged(int arg1)
{
    if(lockControls) return;
    framesX[frmX][frmY].offsetY = arg1;
    updateScene();
}


void CalibrationMain::on_CopyButton_clicked()
{
    buffer = framesX[frmX][frmY];
}


void CalibrationMain::on_PasteButton_clicked()
{
    framesX[frmX][frmY] = buffer;

    updateControls();
    updateScene();
}

void CalibrationMain::on_isDuckFrame_clicked(bool checked)
{
    framesX[frmX][frmY].isDuck = checked;
    updateScene();
}



bool CalibrationMain::on_OpenSprite_clicked()
{
     QString fileName_DATA = QFileDialog::getOpenFileName(this,
            trUtf8("Open sprite file"),(LastOpenDir.isEmpty()? AppPathManager::userAppDir() : LastOpenDir),
            tr("SMBX playble sprite (mario-*.gif peach-*.gif toad-*.gif luigi-*.gif link-*.gif);;"
               "GIF images (*.gif);;"
               "PNG images (*.png);;"
                "All Files (*.*)"));

     if(fileName_DATA==NULL) return false;

     OpenFile(fileName_DATA);
     return true;
}


void CalibrationMain::on_AboutButton_clicked()
{
    about dialog;
    dialog.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    dialog.exec();
}


void CalibrationMain::on_SaveConfigButton_clicked()
{
    saveConfig(currentFile);
}


// Copy current sizes and offsets to ALL frames
void CalibrationMain::on_applyToAll_clicked()
{
    buffer.H=ui->Height->value();
    buffer.W=ui->Width->value();
    buffer.offsetX=ui->OffsetX->value();
    buffer.offsetY=ui->OffsetY->value();
    for(int i=0; i<10; i++)
        for(int j=0; j<10; j++)
            framesX[i][j] = buffer;

}

//Set using this frame on template
void CalibrationMain::on_EnableFrame_clicked(bool checked)
{
    if(ui->EnableFrame->hasFocus())
    {
        framesX[frmX][frmY].used = checked;
    }
}

void CalibrationMain::on_isRightDirect_clicked(bool checked)
{
    if(lockControls) return;
    framesX[frmX][frmY].isRightDir = checked;
    updateScene();
}

void CalibrationMain::on_showGrabItem_clicked(bool checked)
{
    if(lockControls) return;
    framesX[frmX][frmY].showGrabItem = checked;
    updateScene();
}



void CalibrationMain::on_Matrix_clicked()
{
    Matrix dialog;
    this->hide();
    dialog.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    dialog.setFrame(frmX, frmY);
    dialog.show();
    dialog.raise();
    if(dialog.exec()==QDialog::Accepted)
    {
        ui->FrameX->setValue(dialog.frameX);
        ui->FrameY->setValue(dialog.frameY);
    }
    this->show();
    this->raise();

    framesX = dialog.FrameConfig;
    initScene(); updateControls(); updateScene();
}


void CalibrationMain::on_AnimatorButton_clicked()
{
    this->hide();
    Animate dialog;
    dialog.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    dialog.exec();
    this->show();
    this->raise();
}

void CalibrationMain::on_editSizes_clicked()
{
    QMessageBox::StandardButton reply;
      reply = QMessageBox::question(this, "Warning",
        "This is a physical settings for a sprite, this need only for creation of new character sprite\nIf you want to use this sprite in SMBX, please, don't edit this settings.\nDo you want to continue?",
                                    QMessageBox::Yes|QMessageBox::No);
      if (reply == QMessageBox::Yes)
      {

            ui->Height->setEnabled(true);
            ui->Height_duck->setEnabled(true);
            ui->Width->setEnabled(true);
            ui->grabOffsetX->setEnabled(true);
            ui->grabOffsetY->setEnabled(true);

            ui->EnableFrame->setEnabled(true);
            ui->isDuckFrame->setEnabled(true);
            ui->isRightDirect->setEnabled(true);
            ui->showGrabItem->setEnabled(true);

            ui->grabType->setEnabled(true);

            ui->OffsetX->setEnabled(true);
            ui->OffsetY->setEnabled(true);

            ui->CopyButton->setEnabled(true);
            ui->PasteButton->setEnabled(true);
            ui->applyToAll->setEnabled(true);
            ui->editSizes->setVisible(false);
      }
}


void CalibrationMain::on_calibrateImage_clicked()
{
    ImageCalibrator imgCalibrator;
    imgCalibrator.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    if(!imgCalibrator.init(currentFile)) return;

    imgCalibrator.Scene = Scene;

    this->hide();
    imgCalibrator.exec();
    this->show();
    this->raise();
    OpenFile(imgCalibrator.targetPath);
}



void CalibrationMain::on_grabOffsetX_valueChanged(int arg1)
{   if(ui->grabOffsetX->hasFocus())
    {   frameGrabOffsetX = arg1; updateScene();  }
}

void CalibrationMain::on_grabOffsetY_valueChanged(int arg1)
{   if(ui->grabOffsetY->hasFocus())
      {   frameGrabOffsetY = arg1; updateScene();    }
}

void CalibrationMain::updateControls()
{
    frmX = ui->FrameX->value();
    frmY = ui->FrameY->value();
    lockControls=true;
    ui->Height->setValue(frameHeight);
    ui->Height_duck->setValue(frameHeightDuck);
    ui->Width->setValue(frameWidth);
    ui->OffsetX->setValue(framesX[frmX][frmY].offsetX);
    ui->OffsetY->setValue(framesX[frmX][frmY].offsetY);

    ui->grabOffsetX->setValue(frameGrabOffsetX);
    ui->grabOffsetY->setValue(frameGrabOffsetY);

    if(frameOverTopGrab)
        ui->grabTop->setChecked(true);
    else
        ui->grabSide->setChecked(true);

    ui->EnableFrame->setChecked(framesX[frmX][frmY].used);
    ui->isDuckFrame->setChecked(framesX[frmX][frmY].isDuck);
    ui->isRightDirect->setChecked(framesX[frmX][frmY].isRightDir);
    ui->showGrabItem->setChecked(framesX[frmX][frmY].showGrabItem);
    lockControls=false;
}


void CalibrationMain::updateScene()
{
    int x,y,h,w, relX, relY;
    bool isRight;
    x = framesX[frmX][frmY].offsetX;
    y = framesX[frmX][frmY].offsetY;
    h = framesX[frmX][frmY].isDuck?frameHeightDuck:frameHeight;
    w = frameWidth;
    isRight = framesX[frmX][frmY].isRightDir;

    relX = FrameBox_gray.scenePos().x();
    relY = FrameBox_gray.scenePos().y();

    CollisionBox_green.setPos(relX+x,
                              relY+y);
    CollisionBox_green.setRect(0,0,w,h);

    relX = CollisionBox_green.scenePos().x();
    relY = CollisionBox_green.scenePos().y();

    if(frameOverTopGrab)
    {
        grabLineX.setLine(relX,
                          relY-1,
                          relX+w, relY-1);

        grabLineY.setLine(relX+w/2,
                          relY-1,

                          relX+w/2,
                                relY-21);

        grabLineX.setVisible( framesX[frmX][frmY].showGrabItem );
        grabLineY.setVisible( framesX[frmX][frmY].showGrabItem );

    }
    else
    {
        grabLineX.setLine(relX+(isRight?0:w)+frameGrabOffsetX*(isRight?1:-1),
                          relY+h/2+frameGrabOffsetY,
                          relX+(isRight?0:w)+frameGrabOffsetX*(isRight?1:-1)+(isRight?20:-20),
                                                relY+h/2+frameGrabOffsetY );

        grabLineY.setLine(relX+(isRight?0:w)+frameGrabOffsetX*(isRight?1:-1),
                          relY+h/2+frameGrabOffsetY,

                          relX+(isRight?0:w)+frameGrabOffsetX*(isRight?1:-1),
                                relY+h/2+frameGrabOffsetY-20);

        grabLineX.setVisible( framesX[frmX][frmY].showGrabItem );
        grabLineY.setVisible( framesX[frmX][frmY].showGrabItem );
    }

}

void CalibrationMain::initScene()
{
    frmX = ui->FrameX->value();
    frmY = ui->FrameY->value();

    m_FramePos.setX( frmX * 100 );
    m_FramePos.setY( frmY * 100 );

    QRectF rect = ui->PreviewGraph->scene()->sceneRect();

    FrameBox_gray.setPos(rect.width()/2-50, rect.height()/2-50);
    currentPixmap = x_imageSprite.copy(QRect(m_FramePos.x(), m_FramePos.y(), 100, 100));
    currentImageItem.setPixmap(QPixmap(currentPixmap) );
    currentImageItem.setPos(FrameBox_gray.scenePos().x(), FrameBox_gray.scenePos().y());
}



void CalibrationMain::on_grabTop_clicked()
{
    frameOverTopGrab=true;
    updateScene();
}

void CalibrationMain::on_grabSide_clicked()
{
    frameOverTopGrab=false;
    updateScene();
}
