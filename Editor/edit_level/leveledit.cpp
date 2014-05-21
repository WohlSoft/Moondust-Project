/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include <QtWidgets>
#include <QGraphicsItem>
#include <QPixmap>
#include <QGraphicsScene>
#include <QProgressDialog>

#include "leveledit.h"
#include "../ui_leveledit.h"

#include "../file_formats/file_formats.h"
#include "../level_scene/lvlscene.h"
#include "saveimage.h"
#include "../common_features/logger.h"


#include <QDebug>

leveledit::leveledit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::leveledit)
{
    sceneCreated = false;
    FileType = 0;
    setAttribute(Qt::WA_DeleteOnClose);
    isUntitled = true;
    isModified = false;
    latest_export_path = QApplication::applicationDirPath();
    setWindowIcon(QIcon(QPixmap(":/lvl16.png")));
    ui->setupUi(this);

    ui->graphicsView->setOptimizationFlags(QGraphicsView::DontClipPainter);
    ui->graphicsView->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    ui->graphicsView->setOptimizationFlags(QGraphicsView::DontAdjustForAntialiasing);

            /*
             * 	setOptimizationFlags(QGraphicsView::DontClipPainter);
        setOptimizationFlags(QGraphicsView::DontSavePainterState);
        setOptimizationFlags(QGraphicsView::DontAdjustForAntialiasing);*/
}



