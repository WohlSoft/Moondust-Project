#include "image_calibrator.h"
#include <ui_image_calibrator.h>
#include "../main/graphics.h"
#include "../main/globals.h"
#include "../frame_matrix/matrix.h"
#include <QFileInfo>
#include <QDir>
#include <QSettings>

bool ImgCalibratorLock = false;

ImageCalibrator::ImageCalibrator(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImageCalibrator)
{
    ui->setupUi(this);
}

ImageCalibrator::~ImageCalibrator()
{
    delete ui;
}


bool ImageCalibrator::init(QString imgPath)
{
    QList<QString > tmp;
    QString imgFileM;
    QString imgOrig;
    QFileInfo ourFile(imgPath);
    tmp = ourFile.fileName().split(".", QString::SkipEmptyParts);
    if(tmp.size()==2)
        imgFileM = tmp[0] + "m." + tmp[1];
    else
        imgFileM = "";
    //mask = ;

    imgOrig = ourFile.absoluteDir().path() + "/" + tmp[0] + "_orig.png";

    bool createOrig=false;
    if(!QFile::exists(imgOrig))
    {
        createOrig=true;
    }

    //Scene->mSpriteImage = QPixmap(fileName);
    if(createOrig)
    {
        sprite = QPixmap::fromImage(
                    Graphics::setAlphaMask(
                        Graphics::loadQImage( imgPath )
                        ,Graphics::loadQImage( ourFile.absoluteDir().path() + "/" + imgFileM ))
                    );
        if(sprite.isNull()) return false;
        sprite.save(imgOrig, "PNG");
         // Generate backup image
    }
    else
        sprite = QPixmap::fromImage(
                            Graphics::loadQImage( imgOrig )
                    ); // load original sprite instead current


    if(sprite.isNull()) return false;

    targetPath = imgPath;

    pngPath = ourFile.absoluteDir().path() + "/" + tmp[0] + ".png";
    gifPath = ourFile.absoluteDir().path() + "/" + tmp[0] + ".gif";
    gifPathM = ourFile.absoluteDir().path() + "/" + tmp[0] + "m.gif";
    iniPath =  ourFile.absoluteDir().path() + "/" + tmp[0] + "_orig_calibrates.ini";

    //generate scene
    ui->PreviewGraph->setScene(new QGraphicsScene(ui->PreviewGraph));
    QGraphicsScene * sc = ui->PreviewGraph->scene();

    frmX=0;
    frmY=0;

    frameOpts xyCell;
    xyCell.offsetX=0;
    xyCell.offsetY=0;
    xyCell.W=0;
    xyCell.H=0;
    xyCell.used=false;
    xyCell.isDuck=false;
    xyCell.isRightDir=false;
    xyCell.showGrabItem=false;
    // Write default values
    QVector<frameOpts > xRow;
    for(int i=0; i<10; i++)
    {
        xRow.clear();
        for(int j=0; j<10; j++)
        {
            xRow.push_back(xyCell);
        }
        imgOffsets.push_back(xRow);
    }


    loadCalibrates();

    imgFrame = sc->addPixmap(sprite.copy(frmX*100,frmY*100,100,100));
    imgFrame->setZValue(0);
    imgFrame->setPos(0,0);

    phsFrame = sc->addRect(0,0,99,99, QPen(QBrush(Qt::gray), 1));
    phsFrame->setZValue(-2);
    phsFrame->setOpacity(0.5f);
    phsFrame->setPos(0,0);

    physics = sc->addRect(0,0,99,99, QPen(QBrush(Qt::green), 1));
    physics->setZValue(4);
    physics->setPos(0,0);

    updateScene();

    return true;
}


void ImageCalibrator::on_FrameX_valueChanged(int arg1)
{
    if(ImgCalibratorLock) return;
    frmX = arg1;
    updateControls();
    updateScene();

}

void ImageCalibrator::on_FrameY_valueChanged(int arg1)
{
    if(ImgCalibratorLock) return;
    frmY = arg1;
    updateControls();
    updateScene();
}

void ImageCalibrator::on_OffsetX_valueChanged(int arg1)
{
    if(ImgCalibratorLock) return;
    imgOffsets[frmX][frmY].offsetX = arg1;
    updateScene();
}

void ImageCalibrator::on_OffsetY_valueChanged(int arg1)
{
    if(ImgCalibratorLock) return;
    imgOffsets[frmX][frmY].offsetY = arg1;
    updateScene();
}

void ImageCalibrator::on_CropW_valueChanged(int arg1)
{
    if(ImgCalibratorLock) return;
    imgOffsets[frmX][frmY].W = arg1;
    updateScene();

}

void ImageCalibrator::on_CropH_valueChanged(int arg1)
{
    if(ImgCalibratorLock) return;
    imgOffsets[frmX][frmY].H = arg1;
    updateScene();
}

void ImageCalibrator::on_Matrix_clicked()
{
    Matrix dialog;
    dialog.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    dialog.setFrame(frmX, frmY);
    if(dialog.exec()==QDialog::Accepted)
    {
        frmX = dialog.frameX;
        frmY = dialog.frameY;
        ImgCalibratorLock=true;
        ui->FrameX->setValue(frmX);
        ui->FrameY->setValue(frmY);
        updateControls();
        ImgCalibratorLock=false;
    }
    this->raise();
    updateScene();
}

void ImageCalibrator::on_Reset_clicked()
{
    imgOffsets[frmX][frmY].offsetX = 0;
    imgOffsets[frmX][frmY].offsetY = 0;
    imgOffsets[frmX][frmY].W = 0;
    imgOffsets[frmX][frmY].H = 0;
    updateControls();
    updateScene();
}

void ImageCalibrator::on_WritePNG_GIF_clicked()
{
    QPixmap target = generateTarget();
    target.save(pngPath, "PNG");

    QImage targetGif = target.toImage();
    QImage mask = targetGif.alphaChannel();
    mask.invertPixels();

    Graphics::toGif(targetGif, gifPath);
    Graphics::toGif(mask, gifPathM);

    saveCalibrates();

    QMessageBox::information(this,
         "Image was overwritten",
         "Calibrated sprite was saved in:\n"+pngPath+"\n"+gifPath, QMessageBox::Ok);

}

void ImageCalibrator::on_WritePNG_clicked()
{
    QPixmap target = generateTarget();
    target.save(pngPath, "PNG");

    saveCalibrates();

    QMessageBox::information(this,
         "Image was overwritten",
         "Calibrated sprite was saved in:\n"+pngPath, QMessageBox::Ok);

}

void ImageCalibrator::on_WriteGIF_clicked()
{
    QImage target = generateTarget().toImage();
    QImage mask = target.alphaChannel();
    mask.invertPixels();

    Graphics::toGif(target, gifPath);
    Graphics::toGif(mask, gifPathM);

    saveCalibrates();

    QMessageBox::information(this,
         "Image was overwritten",
         "Calibrated sprite was saved in:\n"+gifPath, QMessageBox::Ok);



}

void ImageCalibrator::updateControls()
{
    ImgCalibratorLock = true;
    ui->OffsetX->setValue(imgOffsets[frmX][frmY].offsetX);
    ui->OffsetY->setValue(imgOffsets[frmX][frmY].offsetY);
    ui->CropW->setValue(imgOffsets[frmX][frmY].W);
    ui->CropH->setValue(imgOffsets[frmX][frmY].H);
    ImgCalibratorLock = false;
}

void ImageCalibrator::updateScene()
{
    imgFrame->setPixmap(
                getFrame(frmX, frmY, imgOffsets[frmX][frmY].offsetX, imgOffsets[frmX][frmY].offsetY,
                         imgOffsets[frmX][frmY].W, imgOffsets[frmX][frmY].H)
                );
    physics->setRect(framesX[frmX][frmY].offsetX, framesX[frmX][frmY].offsetY,
                     frameWidth-1, (framesX[frmX][frmY].isDuck?
                                   frameHeightDuck:frameHeight)-1);
}

void ImageCalibrator::saveCalibrates()
{
    QSettings conf(iniPath, QSettings::IniFormat);

    for(int i=0; i<10; i++)
    {
        for(int j=0; j<10; j++)
        {
            conf.beginGroup(QString::number(i)+"-"+QString::number(j));
            conf.setValue("x", imgOffsets[i][j].offsetX);
            conf.setValue("y", imgOffsets[i][j].offsetY);
            conf.setValue("w", imgOffsets[i][j].W);
            conf.setValue("h", imgOffsets[i][j].H);
            conf.endGroup();
        }
    }

}

void ImageCalibrator::loadCalibrates()
{
    QSettings conf(iniPath, QSettings::IniFormat);

    for(int i=0; i<10; i++)
    {
        for(int j=0; j<10; j++)
        {
            conf.beginGroup(QString::number(i)+"-"+QString::number(j));
            imgOffsets[i][j].offsetX = conf.value("x", 0).toInt();
            imgOffsets[i][j].offsetY = conf.value("y", 0).toInt();
            imgOffsets[i][j].W = conf.value("w", 0).toInt();
            imgOffsets[i][j].H = conf.value("h", 0).toInt();
            conf.endGroup();
        }
    }
}

QPixmap ImageCalibrator::generateTarget()
{
    QPixmap target(1000,1000);
    target.fill(Qt::transparent);
    QPainter x(&target);
    for(int i=0; i<10; i++)
    {
        for(int j=0; j<10; j++)
        {
            x.drawPixmap(i*100, j*100, 100, 100,
                         getFrame(i, j, imgOffsets[i][j].offsetX, imgOffsets[i][j].offsetY,
                                  imgOffsets[i][j].W, imgOffsets[i][j].H)
                         );
        }
    }
    x.end();
    return target;
}

QPixmap ImageCalibrator::getFrame(int x, int y, int oX, int oY, int cW, int cH)
{
    int pX=0, pY=0, pXo=0, pYo=0;
    int X, Y;
    X = x*100 + oX;
    Y = y*100 + oY;

    if(X<0)
    {
        pX = X;
        pXo = -pX;
    }
    else
    if(X+100 > 1000)
        pXo = X+100-1000;


    if(Y<0)
    {
        pY = Y;
        pYo = -pY;
    }
    else
    if(Y+100 > 1000)
        pYo = Y+100-1000;

    qDebug() << pX << 100-pXo;
    qDebug() << pY << 100-pYo;
    qDebug() << X-pX << Y-pY << 100-pXo << 100-pYo;

    QPixmap frame(100,100);
    frame.fill(Qt::transparent);
    QPainter pt(&frame);
    pt.drawPixmap(-pX, -pY, 100-pXo-cW, 100-pYo-cH,
                 sprite.copy(X-pX,
                             Y-pY, 100-pXo-cW,100-pYo-cH));
    pt.end();

    return frame;
}


