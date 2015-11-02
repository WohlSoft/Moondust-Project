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

#include <QtWidgets>
#include <QGraphicsItem>
#include <QPixmap>
#include <QGraphicsScene>
#include <QProgressDialog>

#include <common_features/mainwinconnect.h>
#include <common_features/app_path.h>

#include "lvl_export_image.h"
#include "ui_lvl_export_image.h"
#include <editing/edit_level/level_edit.h>
#include <ui_leveledit.h>

//Export whole section
void LevelEdit::ExportToImage_fn()
{
    if(!sceneCreated) return;
    if(!scene) return;

    scene->resetResizers();
    scene->setScreenshotSelector();
}


//Export piece
void LevelEdit::ExportToImage_fn_piece()
{
    if(!sceneCreated) return;
    if(!scene) return;

    scene->resetResizers();
    MainWinConnect::pMainWin->on_actionSelect_triggered();

    qreal zoom=1.0;
    if(QString(ui->graphicsView->metaObject()->className())=="GraphicsWorkspace")
    {
        zoom = static_cast<GraphicsWorkspace *>(ui->graphicsView)->zoom();
    }

    scene->captutedSize.setX(qRound(qreal(ui->graphicsView->horizontalScrollBar()->value())/zoom)+10 );
    scene->captutedSize.setY(qRound(qreal(ui->graphicsView->verticalScrollBar()->value())/zoom)+10 );
    scene->captutedSize.setWidth(qRound(qreal(ui->graphicsView->viewport()->width())/zoom)-20);
    scene->captutedSize.setHeight(qRound(qreal(ui->graphicsView->viewport()->height())/zoom)-20);

    scene->setScreenshotSelector(true);
}


void LevelEdit::ExportingReady() //slot
{
    if(!sceneCreated) return;
    if(!scene) return;

        long x, y, h, w, th, tw;

        bool proportion;
        bool forceTiled=false;
        QString inifile = AppPathManager::settingsFile();
        QSettings settings(inifile, QSettings::IniFormat);
        settings.beginGroup("Main");
        latest_export_path = settings.value("export-path", AppPathManager::userAppDir()).toString();
        proportion = settings.value("export-proportions", true).toBool();
        settings.endGroup();


        if(scene->isFullSection)
        {
            x=LvlData.sections[LvlData.CurSection].size_left;
            y=LvlData.sections[LvlData.CurSection].size_top;
            w=LvlData.sections[LvlData.CurSection].size_right;
            h=LvlData.sections[LvlData.CurSection].size_bottom;
            w=labs(x-w);
            h=labs(y-h);
        }
        else
        {
            x=qRound(scene->captutedSize.x());
            y=qRound(scene->captutedSize.y());
            w=qRound(scene->captutedSize.width());
            h=qRound(scene->captutedSize.height());
        }

        tw=w;
        th=h;
        QVector<long> imgSize;

        imgSize.push_back(th);
        imgSize.push_back(tw);
        imgSize.push_back((int)proportion);

        ExportToImage ExportImage(imgSize, MainWinConnect::pMainWin);
        ExportImage.setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
        ExportImage.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, ExportImage.size(), qApp->desktop()->availableGeometry()));
        if(ExportImage.exec()!=QDialog::Rejected)
            imgSize = ExportImage.imageSize;
        else return;

        if(imgSize.size()>=3)
            if((imgSize[0]<0)||(imgSize[1]<0))
                return;

        QString fileName = QFileDialog::getSaveFileName(this, tr("Export current section to image"),
            latest_export_path + "/" +
            QString("%1_Section_%2%3.png").arg( QFileInfo(curFile).baseName() )
                                                        .arg(LvlData.CurSection+1)
                                                        .arg(scene->isFullSection?"":("_"+QString::number(qrand()))),
                                                        tr("PNG Image (*.png)"));
        if (fileName.isEmpty())
            return;

        forceTiled = ExportImage.TiledBackground();

        QFileInfo exported(fileName);

        QProgressDialog progress(tr("Saving section image..."), tr("Abort"), 0, 100, MainWinConnect::pMainWin);
        progress.setWindowTitle(tr("Please wait..."));
        progress.setWindowModality(Qt::WindowModal);
        progress.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
        progress.setFixedSize(progress.size());
        progress.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, progress.size(), qApp->desktop()->availableGeometry()));
        progress.setCancelButton(0);
        progress.setMinimumDuration(0);

        //progress.show();

        if(!progress.wasCanceled()) progress.setValue(0);

        qApp->processEvents();
        scene->stopAnimation(); //Reset animation to 0 frame
        if(ExportImage.HideWatersAndDoors()) scene->hideWarpsAndDoors(false);
        if(forceTiled) scene->setTiledBackground(true);

        if(!progress.wasCanceled()) progress.setValue(10);
        qApp->processEvents();
        scene->clearSelection(); // Clear selection on export

        latest_export_path = exported.absoluteDir().path();
        proportion = imgSize[2];

        th=imgSize[0];
        tw=imgSize[1];

        qApp->processEvents();
        QImage img(tw,th,QImage::Format_ARGB32_Premultiplied);

        if(!progress.wasCanceled()) progress.setValue(20);

        qApp->processEvents();
        QPainter p(&img);

        if(!progress.wasCanceled()) progress.setValue(30);
        qApp->processEvents();
        scene->render(&p, QRectF(0,0,tw,th),QRectF(x,y,w,h));

        qApp->processEvents();
        p.end();

        if(!progress.wasCanceled()) progress.setValue(40);
        qApp->processEvents();
        img.save(fileName);

        qApp->processEvents();
        if(!progress.wasCanceled()) progress.setValue(90);

        qApp->processEvents();
        if(scene->opts.animationEnabled) scene->startAnimation(); // Restart animation
        if(ExportImage.HideWatersAndDoors()) scene->hideWarpsAndDoors(true);
        if(forceTiled) scene->setTiledBackground(false);

        if(!progress.wasCanceled()) progress.setValue(100);
        if(!progress.wasCanceled())
            progress.close();

        settings.beginGroup("Main");
            settings.setValue("export-path", latest_export_path);
            settings.setValue("export-proportions", proportion);
        settings.endGroup();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////


ExportToImage::ExportToImage(QVector<long> &imgSize, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportToImage)
{
    imageSize = imgSize;
    ui->setupUi(this);
    if(imageSize.size()>=3)
    {
        ui->imgHeight->setValue(imageSize[0]);
        ui->imgWidth->setValue(imageSize[1]);
        ui->SaveProportion->setChecked((bool)imageSize[2]);
    }

}

ExportToImage::~ExportToImage()
{
    delete ui;
}

bool ExportToImage::HideWatersAndDoors()
{
    return ui->HideWarpsWaters->isChecked();
}

bool ExportToImage::TiledBackground()
{
    return ui->tiledBackround->isChecked();
}

void ExportToImage::on_imgHeight_valueChanged(int arg1)
{
    if( (ui->SaveProportion->isChecked()) && (ui->imgHeight->hasFocus()) )
        ui->imgWidth->setValue( (int)round((float)arg1 / ((float)imageSize[0]/(float)imageSize[1])) );

}

void ExportToImage::on_imgWidth_valueChanged(int arg1)
{
    if( (ui->SaveProportion->isChecked()) && (ui->imgWidth->hasFocus()) )
        ui->imgHeight->setValue( (int)round((float)arg1 / ((float)imageSize[1]/(float)imageSize[0])) );
}

void ExportToImage::on_buttonBox_accepted()
{
    imageSize[0]=ui->imgHeight->value();
    imageSize[1]=ui->imgWidth->value();
    imageSize[2]= (int)ui->SaveProportion->isChecked();
    accept();
}

void ExportToImage::on_SaveProportion_toggled(bool checked)
{
    if(checked)
    {
        ui->imgWidth->setValue( imageSize[1]);
        ui->imgHeight->setValue( imageSize[0]);
    }
}

void ExportToImage::on_buttonBox_rejected()
{
    reject();
}

void ExportToImage::on_size1x_clicked()
{
    ui->imgWidth->setValue( imageSize[1]);
    ui->imgHeight->setValue( imageSize[0]);
}

void ExportToImage::on_size05x_clicked()
{
    ui->imgWidth->setValue( imageSize[1]/2);
    ui->imgHeight->setValue( imageSize[0]/2);
}

void ExportToImage::on_size2x_clicked()
{
    ui->imgWidth->setValue( imageSize[1]*2);
    ui->imgHeight->setValue( imageSize[0]*2);
}
