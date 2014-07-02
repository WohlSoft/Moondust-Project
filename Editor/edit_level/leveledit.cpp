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

//#include <QGLWidget>


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
    updateTimer=NULL;

    hMover=NULL;
    vMover=NULL;

    hMove=32;
    vMove=32;

    ui->graphicsView->setOptimizationFlags(QGraphicsView::DontClipPainter);
    ui->graphicsView->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    ui->graphicsView->setOptimizationFlags(QGraphicsView::DontAdjustForAntialiasing);

    ui->graphicsView->horizontalScrollBar()->setSingleStep(32);
    ui->graphicsView->horizontalScrollBar()->setTracking(true);
    ui->graphicsView->verticalScrollBar()->setSingleStep(32);
    ui->graphicsView->verticalScrollBar()->setTracking(true);

    //ui->graphicsView->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers | QGL::DirectRendering)));
    //ui->graphicsView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

            /*
             * 	setOptimizationFlags(QGraphicsView::DontClipPainter);
        setOptimizationFlags(QGraphicsView::DontSavePainterState);
        setOptimizationFlags(QGraphicsView::DontAdjustForAntialiasing);*/
    //connect(ui->graphicsView, SIGNAL(keyPressEvent(QKeyEvent *)), this, SLOT(gViewKeyPress(QKeyEvent *)));
}

int keyTime=75;

void leveledit::keyPressEvent( QKeyEvent * event )
{
    switch(event->key())
    {
    case Qt::Key_Left:
        if(hMover) delete hMover;
        hMover = new QTimer();
        connect(hMover, SIGNAL(timeout()), this, SLOT(moveH_slot()));
        hMove = -fabs(hMove);
        hMover->start(keyTime);
    break;
    case Qt::Key_Right:
        if(hMover) delete hMover;
        hMover = new QTimer();
        connect(hMover, SIGNAL(timeout()), this, SLOT(moveH_slot()));
        hMove = fabs(hMove);
        hMover->start(keyTime);
    break;
    case Qt::Key_Shift:
        keyTime=25;
        if(hMover) {hMover->stop(); hMover->start(keyTime);}
        break;
    default:
        QWidget::keyPressEvent(event);
        break;
    }
}

void leveledit::keyReleaseEvent( QKeyEvent * event )
{

    switch(event->key())
    {
    case Qt::Key_Right:
    case Qt::Key_Left:
        if(hMover)
        {
            hMover->stop();
            delete hMover;
            hMover=NULL;
        }
        break;
    case Qt::Key_Shift:
        keyTime=75;
        if(hMover) {hMover->stop(); hMover->start(keyTime);}
        break;
    default:
        QWidget::keyReleaseEvent(event);
        break;
    }
}

void leveledit::moveH_slot()
{
    moveH(hMove);
}

void leveledit::moveV_slot()
{
    moveV(vMove);
}

void leveledit::moveH(int step)
{
    ui->graphicsView->horizontalScrollBar()->setValue(
                ui->graphicsView->horizontalScrollBar()->value() + step);
}

void leveledit::moveV(int step)
{
    ui->graphicsView->verticalScrollBar()->setValue(
                ui->graphicsView->verticalScrollBar()->value() + step);
}



void leveledit::updateScene()
{
    if(scene->opts.animationEnabled)
        scene->update(
                                     ui->graphicsView->horizontalScrollBar()->value(),
                                     ui->graphicsView->verticalScrollBar()->value(),
                                     ui->graphicsView->width(),
                                     ui->graphicsView->height()
                                     );
}

void leveledit::setAutoUpdateTimer(int ms)
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

void leveledit::stopAutoUpdateTimer()
{
    if(updateTimer!=NULL)
    {
        updateTimer->stop();
        delete updateTimer;
    }
}
