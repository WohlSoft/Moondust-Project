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

#include "../file_formats/lvl_filedata.h"
#include "../level_scene/lvlscene.h"
#include "../common_features/logger.h"

#include "../common_features/mainwinconnect.h"

#include <QDebug>


void leveledit::mouseReleaseEvent( QMouseEvent * event )
{
    /*
    if(scene->PasteFromBuffer)
    {
       changeCursor(0);
       scene->PasteFromBuffer=false;
    }*/
    QWidget::mouseReleaseEvent( event );
}

void leveledit::leaveEvent(QEvent * leaveEvent)
{
   // scene->setSectionResizer(false, false);
    leaveEvent->accept();
}

leveledit::~leveledit()
{
    //free(scene);
    delete ui;
}



void leveledit::ResetPosition()
{
    LvlData.sections[LvlData.CurSection].PositionX =
            LvlData.sections[LvlData.CurSection].size_left;
    LvlData.sections[LvlData.CurSection].PositionY =
            LvlData.sections[LvlData.CurSection].size_bottom-602;

    ui->graphicsView->verticalScrollBar()->setValue(LvlData.sections[LvlData.CurSection].size_bottom-602);
    ui->graphicsView->horizontalScrollBar()->setValue(LvlData.sections[LvlData.CurSection].size_left);
}

void leveledit::setCurrentSection(int scId)
{
    bool sIsNew=false;
    QPointF center = ui->graphicsView->viewport()->rect().center();
    center = ui->graphicsView->mapToScene( center.toPoint() );

    WriteToLog(QtDebugMsg, QString("Save current position %1 %2")
               .arg(ui->graphicsView->horizontalScrollBar()->value())
               .arg(ui->graphicsView->verticalScrollBar()->value())
               );

    //Save currentPosition on Section
    LvlData.sections[LvlData.CurSection].PositionX =
            ui->graphicsView->horizontalScrollBar()->value();
    LvlData.sections[LvlData.CurSection].PositionY =
            ui->graphicsView->verticalScrollBar()->value();

    //Change Current Section
    LvlData.CurSection = scId;

    //allocate new section zone if empty
    if(
        (LvlData.sections[LvlData.CurSection].size_left==0) &&
        (LvlData.sections[LvlData.CurSection].size_top==0) &&
        (LvlData.sections[LvlData.CurSection].size_bottom==0) &&
        (LvlData.sections[LvlData.CurSection].size_right==0)
      )
    {
        scene->InitSection(LvlData.CurSection);
        sIsNew=true;
    }

    scene->drawSpace();

    WriteToLog(QtDebugMsg, QString("Move to current section position"));

    //Move to new section position
    if(sIsNew)
    {
        ui->graphicsView->centerOn(center);
        ui->graphicsView->verticalScrollBar()->setValue(LvlData.sections[LvlData.CurSection].size_top);
        ui->graphicsView->horizontalScrollBar()->setValue(LvlData.sections[LvlData.CurSection].size_left);
    }
    else
    {
        ui->graphicsView->verticalScrollBar()->setValue(LvlData.sections[LvlData.CurSection].PositionY);
        ui->graphicsView->horizontalScrollBar()->setValue(LvlData.sections[LvlData.CurSection].PositionX);
    }

    WriteToLog(QtDebugMsg, QString("Call to Draw intersection space"));

    scene->update();
    ui->graphicsView->update();

    update();

    if(sIsNew) MainWinConnect::pMainWin->on_actionReset_position_triggered();

}


void leveledit::changeCursor(int mode)
{
    switch(mode)
    {
    case (-1): // Hand Dragger
        ui->graphicsView->setCursor(Qt::ArrowCursor);
        ui->graphicsView->setInteractive(false);
        ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
        if(sceneCreated) scene->resetCursor();
        break;
    case 0:    // Selector
        ui->graphicsView->setInteractive(true);
        ui->graphicsView->setCursor(Qt::ArrowCursor);
        ui->graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
        if(sceneCreated) scene->resetCursor();
        break;
    case 1:    // Eriser
        ui->graphicsView->setInteractive(true);
        ui->graphicsView->setCursor(QCursor(QPixmap(":/cur_rubber.png"), 0, 0));
        ui->graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
        if(sceneCreated) scene->resetCursor();
        break;
    case 2:    // place New item
        ui->graphicsView->setInteractive(true);
        ui->graphicsView->setCursor(Qt::CrossCursor);
        ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
        ui->graphicsView->setRenderHint(QPainter::Antialiasing, true);
        ui->graphicsView->viewport()->setMouseTracking(true);
        break;
    case 3:    // Draw water zones
        ui->graphicsView->setInteractive(false);
        ui->graphicsView->setCursor(Qt::CrossCursor);
        ui->graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
        if(sceneCreated) scene->resetCursor();
        break;
    case 4:    // paste from Buffer
        scene->clearSelection();
        ui->graphicsView->setInteractive(true);
        ui->graphicsView->setCursor(QCursor(QPixmap(":/cur_pasta.png"), 0, 0));
        ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
        if(sceneCreated) scene->resetCursor();
        break;
    case 5:    // Resizing mode
        ui->graphicsView->setInteractive(true);
        ui->graphicsView->setCursor(Qt::ArrowCursor);
        ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
        if(sceneCreated) scene->resetCursor();
        break;
    default:
        break;
    }
}
