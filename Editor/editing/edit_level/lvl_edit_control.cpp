/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <QDebug>

#include <mainwindow.h>
#include <common_features/logger.h>
#include <common_features/themes.h>
#include <editing/_scenes/level/lvl_scene.h>
#include <PGE_File_Formats/file_formats.h>
#include <PGE_File_Formats/lvl_filedata.h>

#include "level_edit.h"
#include <ui_leveledit.h>

void LevelEdit::mouseReleaseEvent(QMouseEvent *event)
{
    /*
    if(scene->PasteFromBuffer)
    {
       changeCursor(0);
       scene->PasteFromBuffer=false;
    }*/
    QWidget::mouseReleaseEvent(event);
}

void LevelEdit::leaveEvent(QEvent *leaveEvent)
{
    // scene->setSectionResizer(false, false);
    leaveEvent->accept();
}

LevelEdit::~LevelEdit()
{
    //free(scene);
    delete ui;
    LogDebug("LevelEdit -> Class destroyed");
}



void LevelEdit::ResetPosition()
{
    LvlData.sections[LvlData.CurSection].PositionX =
        LvlData.sections[LvlData.CurSection].size_left;
    LvlData.sections[LvlData.CurSection].PositionY =
        LvlData.sections[LvlData.CurSection].size_bottom - ui->graphicsView->viewport()->height() + 10;
    goTo(LvlData.sections[LvlData.CurSection].size_left, LvlData.sections[LvlData.CurSection].size_bottom - ui->graphicsView->viewport()->height() + 10, false, QPoint(-10, 10));
}

void LevelEdit::ResetZoom()
{
    if(QString(ui->graphicsView->metaObject()->className()) == "GraphicsWorkspace")
        static_cast<GraphicsWorkspace *>(ui->graphicsView)->setZoom(1.0);
}

void LevelEdit::zoomIn()
{
    if(QString(ui->graphicsView->metaObject()->className()) == "GraphicsWorkspace")
        static_cast<GraphicsWorkspace *>(ui->graphicsView)->zoomIn();
}

void LevelEdit::zoomOut()
{
    if(QString(ui->graphicsView->metaObject()->className()) == "GraphicsWorkspace")
        static_cast<GraphicsWorkspace *>(ui->graphicsView)->zoomOut();
}

QGraphicsView *LevelEdit::getGraphicsView()
{
    return ui->graphicsView;
}

void LevelEdit::setZoom(int percent)
{
    if(QString(ui->graphicsView->metaObject()->className()) == "GraphicsWorkspace")
        static_cast<GraphicsWorkspace *>(ui->graphicsView)->setZoom(qreal(percent) / 100.0);
}

int LevelEdit::getZoom()
{
    if(QString(ui->graphicsView->metaObject()->className()) == "GraphicsWorkspace")
        return qRound(static_cast<GraphicsWorkspace *>(ui->graphicsView)->zoom() * 100.0);
    else
        return 100;
}


static int findNearestSection(LevelData &data, long x, long y)
{
    long lessDistance = 0;
    int  result = 0;

    //Try to find intersecting section first
    for(int i = 0; i < data.sections.size(); i++)
    {
        LevelSection &s = data.sections[i];

        if((x >= s.size_left) && (x <= s.size_right) &&
           (y >= s.size_top)  && (y <= s.size_bottom))
            return i;
    }

    //Find section by nearest center or corner
    for(int i = 0; i < data.sections.size(); i++)
    {
        LevelSection &s = data.sections[i];
        long centerX = s.size_left + std::abs(s.size_left - s.size_right) / 2;
        long centerY = s.size_top  + std::abs(s.size_top  - s.size_bottom) / 2;
        //Find distance to center
        long distanceC = std::sqrt(std::pow(centerX - x, 2) + std::pow(centerY - y, 2));

        auto addDistance = [&](long f)
        {
            long distanceCorner = f;
            if(distanceC > distanceCorner)
                distanceC = distanceCorner;
        };

        //Find distance to left-top corner
        addDistance(std::sqrt(std::pow(s.size_left - x, 2) + std::pow(s.size_top - y, 2)));
        //Find distance to right-top corner
        addDistance(std::sqrt(std::pow(s.size_right - x, 2) + std::pow(s.size_top - y, 2)));
        //Find distance to right-bottom corner
        addDistance(std::sqrt(std::pow(s.size_right - x, 2) + std::pow(s.size_bottom - y, 2)));
        //Find distance to left-bottom corner
        addDistance(std::sqrt(std::pow(s.size_left - x, 2) + std::pow(s.size_bottom - y, 2)));

        //Find distance to nearest vertical edge
        if((x >= s.size_left) && (x <= s.size_right))
        {
            addDistance(std::abs(s.size_top - y));
            addDistance(std::abs(s.size_bottom - y));
        }

        //Find distance to nearest horizontal edge
        if((y >= s.size_top) && (y <= s.size_bottom))
        {
            addDistance(std::abs(s.size_left - x));
            addDistance(std::abs(s.size_right - x));
        }

        if(i == 0)
            lessDistance = distanceC;
        else if(distanceC < lessDistance)
        {
            lessDistance = distanceC;
            result = i;
        }
    }

    return result;
}

void LevelEdit::goTo(long x, long y, bool SwitchToSection, QPoint offset, bool center)
{
    if(center)
        offset = QPoint(-ui->graphicsView->viewport()->width() / 2, -ui->graphicsView->viewport()->height() / 2);

    if(SwitchToSection)
    {
        int section = findNearestSection(LvlData, x, y);
        m_mw->SetCurrentLevelSection(section);
    }

    qreal zoom = 1.0;

    if(QString(ui->graphicsView->metaObject()->className()) == "GraphicsWorkspace")
        zoom = static_cast<GraphicsWorkspace *>(ui->graphicsView)->zoom();

    LogDebug(QString("Pos: %1, zoom %2, scenePos: %3")
             .arg(ui->graphicsView->horizontalScrollBar()->value())
             .arg(zoom).arg(x));
    ui->graphicsView->horizontalScrollBar()->setValue(qRound(qreal(x)*zoom) + offset.x());
    ui->graphicsView->verticalScrollBar()->setValue(qRound(qreal(y)*zoom) + offset.y());
    scene->update();
    ui->graphicsView->update();
    update();
}

void LevelEdit::setCurrentSection(int scId)
{
    bool sIsNew = false;
    QPointF center = ui->graphicsView->viewport()->rect().center();
    center = ui->graphicsView->mapToScene(center.toPoint());
    LogDebug(QString("Save current position %1 %2")
             .arg(ui->graphicsView->horizontalScrollBar()->value())
             .arg(ui->graphicsView->verticalScrollBar()->value())
            );
    qreal zoom = 1.0;

    if(QString(ui->graphicsView->metaObject()->className()) == "GraphicsWorkspace")
        zoom = static_cast<GraphicsWorkspace *>(ui->graphicsView)->zoom();

    QPoint target = QPoint(
                        qRound(qreal(ui->graphicsView->horizontalScrollBar()->value()) / zoom),
                        qRound(qreal(ui->graphicsView->verticalScrollBar()->value()) / zoom)
                    );
    //Save currentPosition on Section
    LvlData.sections[LvlData.CurSection].PositionX = target.x();
    LvlData.sections[LvlData.CurSection].PositionY = target.y();

    //Change Current Section
    while(LvlData.sections.size() <= scId)
    {
        LevelSection newSct = FileFormats::CreateLvlSection();
        newSct.id = LvlData.sections.size();
        LvlData.sections.push_back(newSct);
    }

    LvlData.CurSection = scId;

    //allocate new section zone if empty
    if(
        (LvlData.sections[LvlData.CurSection].size_left == 0) &&
        (LvlData.sections[LvlData.CurSection].size_top == 0) &&
        (LvlData.sections[LvlData.CurSection].size_bottom == 0) &&
        (LvlData.sections[LvlData.CurSection].size_right == 0)
    )
    {
        scene->InitSection(LvlData.CurSection);
        sIsNew = true;
    }

    scene->drawSpace();
    LogDebug(QString("Move to current section position"));

    //Move to new section position
    if(sIsNew)
    {
        ui->graphicsView->centerOn(center);
        goTo(LvlData.sections[LvlData.CurSection].size_left,
             LvlData.sections[LvlData.CurSection].size_top);
    }
    else
    {
        goTo(LvlData.sections[LvlData.CurSection].PositionX,
             LvlData.sections[LvlData.CurSection].PositionY);
    }

    LogDebug(QString("Call to Draw intersection space"));
    scene->update();
    ui->graphicsView->update();
    update();

    if(sIsNew) m_mw->on_actionGotoLeftBottom_triggered();
}


void LevelEdit::changeCursor(int mode)
{
    switch(mode)
    {
    case(MODE_HandDrag):  // Hand Dragger
        ui->graphicsView->setCursor(Qt::ArrowCursor);
        ui->graphicsView->setInteractive(false);
        ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);

        if(sceneCreated) scene->SwitchEditingMode(LvlScene::MODE_HandScroll);

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

        if(sceneCreated) scene->SwitchEditingMode(LvlScene::MODE_DrawRect);

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
