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

#include <common_features/app_path.h>
#include <common_features/util.h>
#include <common_features/mainwinconnect.h>
#include <editing/edit_world/world_edit.h>
#include <ui_world_edit.h>

#include "wld_export_image.h"
#include <ui_wld_export_image.h>

void WorldEdit::ExportToImage_fn()
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

void WorldEdit::ExportingReady() //slot
{
        long th, tw;

        bool proportion;
        bool hideMusic;
        bool hidePathLevels;
        QString inifile = AppPathManager::settingsFile();
        QSettings settings(inifile, QSettings::IniFormat);
        settings.beginGroup("Main");
        latest_export_path = settings.value("export-path", AppPathManager::userAppDir()).toString();
        proportion = settings.value("export-proportions", true).toBool();
        settings.endGroup();

        QSize imgSize;
        WldSaveImage ExportImage(scene->captutedSize.toRect(),
                                 scene->captutedSize.size().toSize(),
                                 proportion);
        ExportImage.setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
        ExportImage.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, ExportImage.size(), qApp->desktop()->availableGeometry()));
        if(ExportImage.exec()!=QDialog::Rejected)
        {
            WriteToLog(QtDebugMsg, "ImageExport -> accepted");
            imgSize = ExportImage.imageSize;
            WriteToLog(QtDebugMsg, QString("ImageExport -> Image size %1x%2").arg(imgSize.width()).arg(imgSize.height()));
        }
        else {
            WriteToLog(QtDebugMsg, "ImageExport -> Rejected");
            return;
        }

        proportion = ExportImage.saveProportion;
        hideMusic =  ExportImage.hideMusBoxes;
        hidePathLevels =  ExportImage.hidePaths;

        if((imgSize.width()<0)||(imgSize.height()<0))
            return;

        WriteToLog(QtDebugMsg, "ImageExport -> Open file dialog");

        QString fileName = QFileDialog::getSaveFileName(this, tr("Export selected area to image"),
            latest_export_path + "/" +
            QString("%1_x%2_y%3.png").arg( (WldData.EpisodeTitle.isEmpty())? QFileInfo(curFile).baseName() : util::filePath(WldData.EpisodeTitle.replace(QChar(' '), QChar('_'))) )
                                        .arg(scene->captutedSize.x())
                                        .arg(scene->captutedSize.y()), tr("PNG Image (*.png)"));

        WriteToLog(QtDebugMsg, "ImageExport -> Check file dialog...");
        if (fileName.isEmpty())
            return;

        WriteToLog(QtDebugMsg, "ImageExport -> Start exporting...");
        QFileInfo exported(fileName);

        QProgressDialog progress(tr("Saving section image..."), tr("Abort"), 0, 100, this);
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
        if(scene->opts.animationEnabled) scene->stopAnimation(); //Reset animation to 0 frame
        if(hideMusic) scene->hideMusicBoxes(false);
        if(hidePathLevels) scene->hidePathAndLevels(false);

        if(!progress.wasCanceled()) progress.setValue(10);
        qApp->processEvents();
        scene->clearSelection(); // Clear selection on export

        latest_export_path = exported.absoluteDir().path();

        th=imgSize.height();
        tw=imgSize.width();

        qApp->processEvents();
        QImage img(tw,th,QImage::Format_ARGB32_Premultiplied);
        img.fill(Qt::black);

        if(!progress.wasCanceled()) progress.setValue(20);

        qApp->processEvents();
        QPainter p(&img);

        if(!progress.wasCanceled()) progress.setValue(30);
        qApp->processEvents();
        scene->render(&p, QRectF(0,0,tw,th), scene->captutedSize);

        qApp->processEvents();
        p.end();

        if(!progress.wasCanceled()) progress.setValue(40);
        qApp->processEvents();
        img.save(fileName);

        qApp->processEvents();
        if(!progress.wasCanceled()) progress.setValue(90);

        qApp->processEvents();
        if(hideMusic) scene->hideMusicBoxes(true);
        if(hidePathLevels) scene->hidePathAndLevels(true);

        if(scene->opts.animationEnabled) scene->startAnimation(); // Restart animation


        if(!progress.wasCanceled()) progress.setValue(100);
        if(!progress.wasCanceled())
            progress.close();

        settings.beginGroup("Main");
            settings.setValue("export-path", latest_export_path);
            settings.setValue("export-proportions", proportion);
        settings.endGroup();
}




bool WldSaveImage_lock=false;

WldSaveImage::WldSaveImage(QRect sourceRect, QSize targetSize, bool proportion, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WldSaveImage)
{
    getRect = sourceRect;
    imageSize = targetSize;
    saveProportion = proportion;
    hideMusBoxes = false;
    ui->setupUi(this);

    WldSaveImage_lock=true;

    ui->imgWidth->setValue(imageSize.width());
    ui->imgHeight->setValue(imageSize.height());
    ui->SaveProportion->setChecked(saveProportion);

    ui->ExportRect->setText(tr("Will be exported:\nTop:\t%1\nLeft:\t%2\nRight:\t%3\nBottom:\t%4")
                            .arg(getRect.top())
                            .arg(getRect.left())
                            .arg(getRect.right())
                            .arg(getRect.bottom()));

    WldSaveImage_lock=false;
}

WldSaveImage::~WldSaveImage()
{
    delete ui;
}

void WldSaveImage::on_imgHeight_valueChanged(int arg1)
{
    if(WldSaveImage_lock) return;

    if( (ui->SaveProportion->isChecked()) && (ui->imgHeight->hasFocus()) )
        ui->imgWidth->setValue( qRound(qreal(arg1) / ( qreal(imageSize.height())/qreal(imageSize.width()) ) ) );
}

void WldSaveImage::on_imgWidth_valueChanged(int arg1)
{
    if(WldSaveImage_lock) return;

    if( (ui->SaveProportion->isChecked()) && (ui->imgWidth->hasFocus()) )
        ui->imgHeight->setValue( qRound(qreal(arg1) / ( qreal(imageSize.width())/qreal(imageSize.height())  )) );

}

void WldSaveImage::on_SaveProportion_clicked(bool checked)
{
    if(WldSaveImage_lock) return;

    WldSaveImage_lock=true;

    if(checked)
    {
        ui->imgWidth->setValue( imageSize.width());
        ui->imgHeight->setValue( imageSize.height());
    }

    WldSaveImage_lock=false;
}

void WldSaveImage::on_HidePaths_clicked(bool checked)
{
    if(WldSaveImage_lock) return;
    hidePaths = checked;
}

void WldSaveImage::on_buttonBox_accepted()
{
    imageSize.setWidth(ui->imgWidth->value());
    imageSize.setHeight(ui->imgHeight->value());
    saveProportion = ui->SaveProportion->isChecked();
    hideMusBoxes=ui->HideMusBoxes->isChecked();
    hidePaths = ui->HidePaths->isChecked();
    accept();
}

void WldSaveImage::on_buttonBox_rejected()
{
    reject();
}

void WldSaveImage::on_HideMusBoxes_clicked(bool checked)
{
    hideMusBoxes = checked;
}
