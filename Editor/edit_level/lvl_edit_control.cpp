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

#include "level_edit.h"
#include "../ui_leveledit.h"

#include "../file_formats/lvl_filedata.h"
#include "../level_scene/lvlscene.h"
#include "../common_features/logger.h"
#include "../common_features/themes.h"

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
            LvlData.sections[LvlData.CurSection].size_bottom-ui->graphicsView->viewport()->height()+25;

    goTo(LvlData.sections[LvlData.CurSection].size_left, LvlData.sections[LvlData.CurSection].size_bottom-ui->graphicsView->viewport()->height()+25, false, QPoint(-10,10));
}

void leveledit::ResetZoom()
{
    if(QString(ui->graphicsView->metaObject()->className())=="GraphicsWorkspace")
    {
        static_cast<GraphicsWorkspace *>(ui->graphicsView)->setZoom(1.0);
    }
}

void leveledit::zoomIn()
{
    if(QString(ui->graphicsView->metaObject()->className())=="GraphicsWorkspace")
    {
        static_cast<GraphicsWorkspace *>(ui->graphicsView)->zoomIn();
    }
}

void leveledit::zoomOut()
{
    if(QString(ui->graphicsView->metaObject()->className())=="GraphicsWorkspace")
    {
        static_cast<GraphicsWorkspace *>(ui->graphicsView)->zoomOut();
    }
}

QGraphicsView *leveledit::getGraphicsView()
{
    return ui->graphicsView;
}
void leveledit::setZoom(int percent)
{
    if(QString(ui->graphicsView->metaObject()->className())=="GraphicsWorkspace")
    {
        static_cast<GraphicsWorkspace *>(ui->graphicsView)->setZoom(qreal(percent)/100.0);
    }
}

int leveledit::getZoom()
{
    if(QString(ui->graphicsView->metaObject()->className())=="GraphicsWorkspace")
    {
        return qRound(static_cast<GraphicsWorkspace *>(ui->graphicsView)->zoom() * 100.0);
    }
    else
    {
        return 100;
    }
}


void leveledit::goTo(long x, long y, bool SwitchToSection, QPoint offset, bool center)
{
    if(center)
        offset= QPoint(-ui->graphicsView->viewport()->width()/2, -ui->graphicsView->viewport()->height()/2);

    if(SwitchToSection)
    {
        int padding=128;
        for(int i=0; i<LvlData.sections.size(); i++)
        {
            if( (x >= LvlData.sections[i].size_left-padding) &&
                (x <= LvlData.sections[i].size_right+padding) &&
                (y >= LvlData.sections[i].size_top-padding) &&
                (y <= LvlData.sections[i].size_bottom+padding) )
            {
                    MainWinConnect::pMainWin->SetCurrentLevelSection(i);
                    break;
            }
        }
    }

    qreal zoom=1.0;
    if(QString(ui->graphicsView->metaObject()->className())=="GraphicsWorkspace")
    {
        zoom = static_cast<GraphicsWorkspace *>(ui->graphicsView)->zoom();
    }

    WriteToLog(QtDebugMsg, QString("Pos: %1, zoom %2, scenePos: %3")
               .arg(ui->graphicsView->horizontalScrollBar()->value())
               .arg(zoom).arg(x));

    ui->graphicsView->horizontalScrollBar()->setValue( qRound(qreal(x)*zoom)+offset.x() );
    ui->graphicsView->verticalScrollBar()->setValue( qRound(qreal(y)*zoom)+offset.y() );

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

    qreal zoom=1.0;
    if(QString(ui->graphicsView->metaObject()->className())=="GraphicsWorkspace")
    {
        zoom = static_cast<GraphicsWorkspace *>(ui->graphicsView)->zoom();
    }

    QPoint target = QPoint(
                qRound(qreal(ui->graphicsView->horizontalScrollBar()->value())/zoom),
                qRound(qreal(ui->graphicsView->verticalScrollBar()->value())/zoom)
                           );

    //Save currentPosition on Section
    LvlData.sections[LvlData.CurSection].PositionX = target.x();
    LvlData.sections[LvlData.CurSection].PositionY = target.y();

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
    case (MODE_HandDrag): // Hand Dragger
        ui->graphicsView->setCursor(Qt::ArrowCursor);
        ui->graphicsView->setInteractive(false);
        ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
        if(sceneCreated) scene->SwitchEditingMode(LvlScene::MODE_Selecting);
        break;
    case MODE_Selecting:    // Selector
        ui->graphicsView->setInteractive(true);
        ui->graphicsView->setCursor(Qt::ArrowCursor);
        ui->graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
        if(sceneCreated) scene->SwitchEditingMode(LvlScene::MODE_Selecting);
        break;
    case MODE_Erasing:    // Eriser
        ui->graphicsView->setInteractive(true);
        ui->graphicsView->setCursor(QCursor(Themes::Image(Themes::cursor_erasing), 0, 0));
        ui->graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
        if(sceneCreated) scene->SwitchEditingMode(LvlScene::MODE_Erasing);
        break;
    case MODE_PlaceItem:    // place New item [any modes (single, square, line)]
        ui->graphicsView->setInteractive(true);
        ui->graphicsView->setCursor(Qt::CrossCursor);
        ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
        ui->graphicsView->setRenderHint(QPainter::Antialiasing, true);
        ui->graphicsView->viewport()->setMouseTracking(true);
        if(sceneCreated) scene->SwitchEditingMode(LvlScene::MODE_PlacingNew);
        break;
    case MODE_DrawSquares:    // Draw water zones
        ui->graphicsView->setInteractive(true);
        ui->graphicsView->setCursor(Qt::CrossCursor);
        ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
        ui->graphicsView->setRenderHint(QPainter::Antialiasing, true);
        ui->graphicsView->viewport()->setMouseTracking(true);
        if(sceneCreated) scene->SwitchEditingMode(LvlScene::MODE_DrawSquare);
        break;
    case MODE_Pasting:    // paste from Buffer
        scene->clearSelection();
        ui->graphicsView->setInteractive(true);
        ui->graphicsView->setCursor(QCursor(Themes::Image(Themes::cursor_pasting), 0, 0));
        ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
        if(sceneCreated) scene->SwitchEditingMode(LvlScene::MODE_PasteFromClip);
        break;
    case MODE_Resizing:    // Resizing mode
        ui->graphicsView->setInteractive(true);
        ui->graphicsView->setCursor(Qt::ArrowCursor);
        ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
        if(sceneCreated) scene->SwitchEditingMode(LvlScene::MODE_Resizing);
        break;
    default:
        break;
    }
}
