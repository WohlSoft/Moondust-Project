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


#include "world_edit.h"
#include <ui_world_edit.h>

#include "../common_features/app_path.h"

WorldEdit::WorldEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WorldEdit)
{
    sceneCreated = false;
    FileType = 0;
    currentMusic = 0;
    setAttribute(Qt::WA_DeleteOnClose);
    isUntitled = true;
    latest_export_path = ApplicationPath;
    setWindowIcon(QIcon(QPixmap(":/images/world16.png")));
    ui->setupUi(this);
    updateTimer=NULL;

    ui->graphicsView->setOptimizationFlags(QGraphicsView::DontClipPainter);
    ui->graphicsView->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    ui->graphicsView->setOptimizationFlags(QGraphicsView::DontAdjustForAntialiasing);

    ui->graphicsView->horizontalScrollBar()->setSingleStep(32);
    ui->graphicsView->horizontalScrollBar()->setTracking(true);
    ui->graphicsView->verticalScrollBar()->setSingleStep(32);
    ui->graphicsView->verticalScrollBar()->setTracking(true);
}

void WorldEdit::focusInEvent(QFocusEvent *event)
{
    ui->graphicsView->setFocus();
    QWidget::focusInEvent(event);
}


void WorldEdit::updateScene()
{
        if(scene->opts.animationEnabled)
        {
            QRect viewport_rect(0, 0, ui->graphicsView->viewport()->width(), ui->graphicsView->viewport()->height());
            scene->update( ui->graphicsView->mapToScene(viewport_rect).boundingRect() );
        }
}

void WorldEdit::setAutoUpdateTimer(int ms)
{
    if(updateTimer!=NULL)
        delete updateTimer;
    updateTimer = new QTimer;
    connect(
                updateTimer, SIGNAL(timeout()),
                this,
                SLOT( updateScene()) );
    updateTimer->start(ms);
}

void WorldEdit::stopAutoUpdateTimer()
{
    if(updateTimer!=NULL)
    {
        updateTimer->stop();
        delete updateTimer;
    }
}

