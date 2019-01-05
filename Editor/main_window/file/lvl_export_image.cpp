/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QGraphicsItem>
#include <QPixmap>
#include <QGraphicsScene>
#include <QProgressDialog>
#include <QDesktopWidget>
#include <QStandardPaths>

#include <common_features/main_window_ptr.h>
#include <common_features/app_path.h>

#include "lvl_export_image.h"
#include "ui_lvl_export_image.h"
#include <editing/edit_level/level_edit.h>
#include <editing/_scenes/level/items/item_block.h>
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

    qreal zoom = 1.0;
    if(QString(ui->graphicsView->metaObject()->className()) == "GraphicsWorkspace")
    {
        zoom = static_cast<GraphicsWorkspace *>(ui->graphicsView)->zoom();
    }

    scene->captutedSize.setX(qRound(qreal(ui->graphicsView->horizontalScrollBar()->value()) / zoom) + 10);
    scene->captutedSize.setY(qRound(qreal(ui->graphicsView->verticalScrollBar()->value()) / zoom) + 10);
    if(GlobalSettings::screenGrab.sizeType == SETTINGS_ScreenGrabSettings::GRAB_Fit)
    {
        scene->captutedSize.setWidth(qRound(qreal(ui->graphicsView->viewport()->width()) / zoom) - 20);
        scene->captutedSize.setHeight(qRound(qreal(ui->graphicsView->viewport()->height()) / zoom) - 20);
    }
    else
    {
        scene->captutedSize.setWidth(GlobalSettings::screenGrab.width);
        scene->captutedSize.setHeight(GlobalSettings::screenGrab.height);
    }

    scene->setScreenshotSelector(true);
}

void LevelEdit::ExportingReady() //slot
{
    if(!sceneCreated) return;
    if(!scene) return;

    long x, y, h, w, th, tw;

    bool keepAspectRatio;
    bool forceTiled = false;
    bool gridWasShown = false;
    QString inifile = AppPathManager::settingsFile();
    QSettings settings(inifile, QSettings::IniFormat);
    settings.beginGroup("Main");
    m_recentExportPath = settings.value("export-path", QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)).toString();
    keepAspectRatio = settings.value("export-keep-aspect-ratio", true).toBool();
    keepAspectRatio = settings.value("export-proportions", keepAspectRatio).toBool();
    settings.endGroup();

    if(scene->m_captureFullSection)
    {
        x = LvlData.sections[LvlData.CurSection].size_left;
        y = LvlData.sections[LvlData.CurSection].size_top;
        w = LvlData.sections[LvlData.CurSection].size_right;
        h = LvlData.sections[LvlData.CurSection].size_bottom;
        w = labs(x - w);
        h = labs(y - h);
    }
    else
    {
        x = qRound(scene->captutedSize.x());
        y = qRound(scene->captutedSize.y());
        w = qRound(scene->captutedSize.width());
        h = qRound(scene->captutedSize.height());
    }

    tw = w;
    th = h;
    QVector<long> imgSize;

    imgSize.push_back(th);
    imgSize.push_back(tw);
    imgSize.push_back((int)keepAspectRatio);

    ExportToImage imageExportDialog(imgSize, MainWinConnect::pMainWin);
    imageExportDialog.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    imageExportDialog.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, imageExportDialog.size(), qApp->desktop()->availableGeometry()));
    if(imageExportDialog.exec() != QDialog::Rejected)
        imgSize = imageExportDialog.imageSize;
    else return;

    if(imgSize.size() >= 3)
        if((imgSize[0] < 0) || (imgSize[1] < 0))
            return;

    QString fileName = QFileDialog::getSaveFileName(this, tr("Export current section to image"),
                       m_recentExportPath + "/" +
                       QString("%1_Section_%2%3.png").arg(QFileInfo(curFile).baseName())
                       .arg(LvlData.CurSection + 1)
                       .arg(scene->m_captureFullSection ? "" : ("_" + QString::number(qrand()))),
                       tr("PNG Image (*.png)"));
    if(fileName.isEmpty())
        return;

    forceTiled = imageExportDialog.tiledBackground();

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
    if(imageExportDialog.hideWatersAndWarps())
        scene->hideWarpsAndDoors(false);
    if(imageExportDialog.hideMetaSigns())
        scene->setMetaSignsVisibility(false);
    if(forceTiled)
        scene->setTiledBackground(true);
    QList<QGraphicsItem*> invisibleBlocks;
    if(imageExportDialog.hideInvisibleBlocks())
    {
        QList<QGraphicsItem*> allBlocks = scene->items();
        for(QGraphicsItem* it : allBlocks)
        {
            if(it->data(ITEM_TYPE).toString() != "Block")
                continue;
            //Exclude already hidden elements
            if(!it->isVisible())
                continue;
            ItemBlock *blk = dynamic_cast<ItemBlock*>(it);
            if(blk && blk->m_data.invisible)
            {
                it->setVisible(false);
                invisibleBlocks.push_back(it);
            }
        }
    }
    if(imageExportDialog.hideGrid())
    {
        gridWasShown = scene->m_opts.grid_show;
        scene->m_opts.grid_show = false;
    }

    if(!progress.wasCanceled())
        progress.setValue(10);
    scene->invalidate();
    scene->update();
    qApp->processEvents();
    scene->clearSelection(); // Clear selection on export

    m_recentExportPath = exported.absoluteDir().path();
    keepAspectRatio = imgSize[2];

    th = imgSize[0];
    tw = imgSize[1];

    qApp->processEvents();
    QImage img((int)tw, (int)th, QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::transparent);

    if(!progress.wasCanceled()) progress.setValue(20);

    qApp->processEvents();
    QPainter p(&img);

    if(!progress.wasCanceled()) progress.setValue(30);
    qApp->processEvents();
    scene->render(&p, QRectF(0, 0, tw, th), QRectF(x, y, w, h));

    qApp->processEvents();
    p.end();

    if(!progress.wasCanceled()) progress.setValue(40);
    qApp->processEvents();
    img.save(fileName);

    qApp->processEvents();
    if(!progress.wasCanceled()) progress.setValue(90);

    qApp->processEvents();
    if(scene->m_opts.animationEnabled)
        scene->startAnimation(); // Restart animation
    if(imageExportDialog.hideWatersAndWarps())
        scene->hideWarpsAndDoors(true);
    if(imageExportDialog.hideMetaSigns())
        scene->setMetaSignsVisibility(true);
    if(forceTiled)
        scene->setTiledBackground(false);
    if(gridWasShown)
        scene->m_opts.grid_show = true;
    if(imageExportDialog.hideInvisibleBlocks())
    {
        for(QGraphicsItem *it : invisibleBlocks)
            it->setVisible(true);
    }
    scene->invalidate();
    scene->update();

    if(!progress.wasCanceled()) progress.setValue(100);
    if(!progress.wasCanceled())
        progress.close();

    settings.beginGroup("Main");
    settings.setValue("export-path", m_recentExportPath);
    settings.setValue("export-keep-aspect-ratio", keepAspectRatio);
    settings.endGroup();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////


ExportToImage::ExportToImage(QVector<long> &imgSize, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportToImage)
{
    imageSize = imgSize;
    ui->setupUi(this);
    if(imageSize.size() >= 3)
    {
        ui->imgHeight->setValue((int)imageSize[0]);
        ui->imgWidth->setValue((int)imageSize[1]);
        ui->keepAspectRatio->setChecked((bool)imageSize[2]);
    }
}

ExportToImage::~ExportToImage()
{
    delete ui;
}

bool ExportToImage::hideWatersAndWarps()
{
    return ui->HideWarpsWaters->isChecked();
}

bool ExportToImage::hideMetaSigns()
{
    return ui->hideMetaSigns->isChecked();
}

bool ExportToImage::hideInvisibleBlocks()
{
    return ui->hideInvisibleBlocks->isChecked();
}

bool ExportToImage::hideGrid()
{
    return ui->hideGrid->isChecked();
}

bool ExportToImage::tiledBackground()
{
    return ui->tiledBackround->isChecked();
}


void ExportToImage::on_imgHeight_valueChanged(int arg1)
{
    if((ui->keepAspectRatio->isChecked()) && (ui->imgHeight->hasFocus()))
        ui->imgWidth->setValue((int)round((float)arg1 / ((float)imageSize[0] / (float)imageSize[1])));

}

void ExportToImage::on_imgWidth_valueChanged(int arg1)
{
    if((ui->keepAspectRatio->isChecked()) && (ui->imgWidth->hasFocus()))
        ui->imgHeight->setValue((int)round((float)arg1 / ((float)imageSize[1] / (float)imageSize[0])));
}

void ExportToImage::on_buttonBox_accepted()
{
    imageSize[0] = ui->imgHeight->value();
    imageSize[1] = ui->imgWidth->value();
    imageSize[2] = (int)ui->keepAspectRatio->isChecked();
    accept();
}

void ExportToImage::on_SaveProportion_toggled(bool checked)
{
    if(checked)
    {
        ui->imgWidth->setValue((int)imageSize[1]);
        ui->imgHeight->setValue((int)imageSize[0]);
    }
}

void ExportToImage::on_buttonBox_rejected()
{
    reject();
}

void ExportToImage::on_size1x_clicked()
{
    ui->imgWidth->setValue((int)imageSize[1]);
    ui->imgHeight->setValue((int)imageSize[0]);
}

void ExportToImage::on_size05x_clicked()
{
    ui->imgWidth->setValue((int)imageSize[1] / 2);
    ui->imgHeight->setValue((int)imageSize[0] / 2);
}

void ExportToImage::on_size2x_clicked()
{
    ui->imgWidth->setValue((int)imageSize[1] * 2);
    ui->imgHeight->setValue((int)imageSize[0] * 2);
}
