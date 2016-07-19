/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <mainwindow.h>
#include <common_features/themes.h>
#include <common_features/item_rectangles.h>

#include "mode_circle.h"
#include "../lvl_history_manager.h"
#include "../lvl_item_placing.h"

LVL_ModeCircle::LVL_ModeCircle(QGraphicsScene *parentScene, QObject *parent)
    : EditMode("Circle", parentScene, parent)
{
    drawStartPos = QPointF(0,0);
}

LVL_ModeCircle::~LVL_ModeCircle()
{}

void LVL_ModeCircle::set()
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    s->clearSelection();
    s->resetResizers();

    s->m_eraserIsEnabled=false;
    s->m_pastingMode=false;
    s->m_busyMode=true;
    s->m_disableMoveItems=false;

    s->m_viewPort->setInteractive(true);
    s->m_viewPort->setCursor(Themes::Cursor(Themes::cursor_square_fill));
    s->m_viewPort->setDragMode(QGraphicsView::NoDrag);
    s->m_viewPort->setRenderHint(QPainter::Antialiasing, true);
    s->m_viewPort->viewport()->setMouseTracking(true);
}

void LVL_ModeCircle::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);
    MainWindow* mw = s->m_mw;

    if( mouseEvent->buttons() & Qt::RightButton )
    {
        item_rectangles::clearArray();
        QMetaObject::invokeMethod(mw, "on_actionSelect_triggered");
        dontCallEvent = true;
        s->m_mouseIsMovedAfterKey = true;
        return;
    }

    s->m_lastBlockArrayID=s->m_data->blocks_array_id;
    s->m_lastBgoArrayID=s->m_data->bgo_array_id;
    s->m_lastNpcArrayID=s->m_data->npc_array_id;

    LogDebug(QString("Circle mode %1").arg(s->m_editMode));
    if(s->m_cursorItemImg)
    {
        drawStartPos = QPointF(s->applyGrid( mouseEvent->scenePos().toPoint(),
                                          LvlPlacingItems::gridSz,
                                          LvlPlacingItems::gridOffset));
        s->m_cursorItemImg->setPos( drawStartPos );
        s->m_cursorItemImg->setVisible(true);

        QPoint hw = s->applyGrid( mouseEvent->scenePos().toPoint(),
                               LvlPlacingItems::gridSz,
                               LvlPlacingItems::gridOffset);

        QSize hs = QSize( (long)fabs(drawStartPos.x() - hw.x()),  (long)fabs( drawStartPos.y() - hw.y() ) );
        dynamic_cast<QGraphicsEllipseItem *>(s->m_cursorItemImg)->setRect(0,0, hs.width(), hs.height());
    }
}

void LVL_ModeCircle::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    if(!LvlPlacingItems::layer.isEmpty() && LvlPlacingItems::layer!="Default")
        s->setLabelBoxItem(true, mouseEvent->scenePos(), LvlPlacingItems::layer + ", " +
                     QString::number( mouseEvent->scenePos().toPoint().x() ) + "x" +
                     QString::number( mouseEvent->scenePos().toPoint().y() )
                      );
    else
        s->setLabelBoxItem(false);

    if(s->m_cursorItemImg && s->m_cursorItemImg->isVisible())
    {
        QGraphicsEllipseItem * cur = dynamic_cast<QGraphicsEllipseItem *>(s->m_cursorItemImg);
        QPoint hw = s->applyGrid( mouseEvent->scenePos().toPoint(),
                               LvlPlacingItems::gridSz,
                               LvlPlacingItems::gridOffset);

        QSize hs = QSize( (long)fabs(drawStartPos.x() - hw.x()),  (long)fabs( drawStartPos.y() - hw.y() ) );


        cur->setRect(0,0, hs.width(), hs.height());
        cur->setPos(((hw.x() < drawStartPos.x() )? hw.x() : drawStartPos.x()),
                    ((hw.y() < drawStartPos.y() )? hw.y() : drawStartPos.y()));
    }

}

void LVL_ModeCircle::mouseRelease(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent);

    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    if(s->m_cursorItemImg)
    {
        QGraphicsEllipseItem * cur = dynamic_cast<QGraphicsEllipseItem *>(s->m_cursorItemImg);

        // /////////// Don't draw with zero width or height //////////////
        if( (cur->rect().width()==0) || (cur->rect().height()==0))
        {
            s->m_cursorItemImg->hide();
            dontCallEvent = true;
            return;
        }
        // ///////////////////////////////////////////////////////////////

        if( ((s->m_placingItemType==LvlScene::PLC_Block)&&(!LvlPlacingItems::sizableBlock))||
                (s->m_placingItemType==LvlScene::PLC_BGO))
        {
            item_rectangles::drawRound(s, QRect(cur->x(), cur->y(), cur->rect().width(), cur->rect().height()),
                                                QSize(LvlPlacingItems::itemW, LvlPlacingItems::itemH) );
        }

        switch(s->m_placingItemType)
        {
        case LvlScene::PLC_Block:
            {
                //LvlPlacingItems::waterSet.quicksand = (LvlPlacingItems::waterType==1);
                if(LvlPlacingItems::sizableBlock)
                {
                    LvlPlacingItems::blockSet.x = s->m_cursorItemImg->scenePos().x();
                    LvlPlacingItems::blockSet.y = s->m_cursorItemImg->scenePos().y();
                    LvlPlacingItems::blockSet.w = cur->rect().width();
                    LvlPlacingItems::blockSet.h = cur->rect().height();
                    //here define placing water item.
                    s->m_data->blocks_array_id++;

                    LvlPlacingItems::blockSet.array_id = s->m_data->blocks_array_id;
                    s->m_data->blocks.push_back(LvlPlacingItems::blockSet);

                    s->placeBlock(LvlPlacingItems::blockSet, true);
                    LevelData plSzBlock;
                    plSzBlock.blocks.push_back(LvlPlacingItems::blockSet);
                    s->m_history->addPlace(plSzBlock);
                    s->Debugger_updateItemList();
                    break;
                }
                else
                {
                    s->placeItemsByRectArray();
                    //LogDebug("clear collision buffer");
                    s->m_emptyCollisionCheck = false;
                    s->collisionCheckBuffer.clear();
                    #ifdef _DEBUG_
                    LogDebug("Done");
                    #endif
                    s->Debugger_updateItemList();
                    break;
                }
            }
        case LvlScene::PLC_BGO:
            {
                s->placeItemsByRectArray();

                s->m_emptyCollisionCheck = false;
                s->collisionCheckBuffer.clear();

                s->Debugger_updateItemList();
             break;
            }
        }
        s->m_data->modified = true;

        s->m_cursorItemImg->hide();
    }
}

void LVL_ModeCircle::keyPress(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent);
}

void LVL_ModeCircle::keyRelease(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent);
    switch(keyEvent->key())
    {
        case (Qt::Key_Escape):
        {
            item_rectangles::clearArray();
            LvlScene *s = dynamic_cast<LvlScene *>(scene);
            if(s) s->m_mw->on_actionSelect_triggered();
            break;
        }
        default:
            break;
    }
}
