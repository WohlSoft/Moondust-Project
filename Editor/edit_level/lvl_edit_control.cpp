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
    MainWinConnect::pMainWin->updateMenus(true);
    WriteToLog(QtDebugMsg, "LevelEdit -> Class destroyed");
}



void leveledit::ResetPosition()
{
    LvlData.sections[LvlData.CurSection].PositionX =
            LvlData.sections[LvlData.CurSection].size_left;
    LvlData.sections[LvlData.CurSection].PositionY =
            LvlData.sections[LvlData.CurSection].size_bottom-602;

    goTo(LvlData.sections[LvlData.CurSection].size_left, LvlData.sections[LvlData.CurSection].size_bottom-602, false, QPoint(-10,10));
}

void leveledit::goTo(long x, long y, bool SwitchToSection, QPoint offset)
{

    if(SwitchToSection)
    {
        for(int i=0; i<LvlData.sections.size(); i++)
        {
            if( (x >= LvlData.sections[i].size_left) &&
                (x <= LvlData.sections[i].size_right) &&
                (y >= LvlData.sections[i].size_top) &&
                (y <= LvlData.sections[i].size_bottom) )
            {
                    MainWinConnect::pMainWin->SetCurrentLevelSection(i);
                    break;
            }
        }
    }

    ui->graphicsView->horizontalScrollBar()->setValue(x + offset.x() );
    ui->graphicsView->verticalScrollBar()->setValue(y + offset.y() );

    scene->update();
    ui->graphicsView->update();

    update();

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
        goTo(LvlData.sections[LvlData.CurSection].size_left,
                LvlData.sections[LvlData.CurSection].size_top);
    }
    else
    {
        goTo( LvlData.sections[LvlData.CurSection].PositionX,
        LvlData.sections[LvlData.CurSection].PositionY );
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
        if(sceneCreated) scene->SwitchEditingMode(LvlScene::MODE_Selecting);
        break;
    case 0:    // Selector
        ui->graphicsView->setInteractive(true);
        ui->graphicsView->setCursor(Qt::ArrowCursor);
        ui->graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
        if(sceneCreated) scene->SwitchEditingMode(LvlScene::MODE_Selecting);
        break;
    case 1:    // Eriser
        ui->graphicsView->setInteractive(true);
        ui->graphicsView->setCursor(QCursor(QPixmap(":/cur_rubber.png"), 0, 0));
        ui->graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
        if(sceneCreated) scene->SwitchEditingMode(LvlScene::MODE_Erasing);
        break;
    case 2:    // place New item
        ui->graphicsView->setInteractive(true);
        ui->graphicsView->setCursor(Qt::CrossCursor);
        ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
        ui->graphicsView->setRenderHint(QPainter::Antialiasing, true);
        ui->graphicsView->viewport()->setMouseTracking(true);
        if(sceneCreated) scene->SwitchEditingMode(LvlScene::MODE_PlacingNew);
        break;
    case 3:    // Draw water zones
        ui->graphicsView->setInteractive(true);
        ui->graphicsView->setCursor(Qt::CrossCursor);
        ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
        ui->graphicsView->setRenderHint(QPainter::Antialiasing, true);
        ui->graphicsView->viewport()->setMouseTracking(true);
        if(sceneCreated) scene->SwitchEditingMode(LvlScene::MODE_DrawSquare);
        break;
    case 4:    // paste from Buffer
        scene->clearSelection();
        ui->graphicsView->setInteractive(true);
        ui->graphicsView->setCursor(QCursor(QPixmap(":/cur_pasta.png"), 0, 0));
        ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
        if(sceneCreated) scene->SwitchEditingMode(LvlScene::MODE_PasteFromClip);
        break;
    case 5:    // Resizing mode
        ui->graphicsView->setInteractive(true);
        ui->graphicsView->setCursor(Qt::ArrowCursor);
        ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
        if(sceneCreated) scene->SwitchEditingMode(LvlScene::MODE_Resizing);
        break;
    default:
        break;
    }
}
