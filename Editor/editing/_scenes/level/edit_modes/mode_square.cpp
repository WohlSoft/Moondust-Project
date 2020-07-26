/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "mode_square.h"
#include "../lvl_history_manager.h"
#include "../lvl_item_placing.h"

LVL_ModeSquare::LVL_ModeSquare(QGraphicsScene *parentScene, QObject *parent)
    : EditMode("Square", parentScene, parent)
{
    drawStartPos = QPointF(0, 0);
}

LVL_ModeSquare::~LVL_ModeSquare()
{}

void LVL_ModeSquare::set()
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    s->clearSelection();
    s->resetResizers();

    s->m_eraserIsEnabled = false;
    s->m_pastingMode = false;
    s->m_busyMode = true;
    s->m_disableMoveItems = false;

    s->m_viewPort->setInteractive(true);
    s->m_viewPort->setCursor(Themes::Cursor(Themes::cursor_square_fill));
    s->m_viewPort->setDragMode(QGraphicsView::NoDrag);
    s->m_viewPort->setRenderHint(QPainter::Antialiasing, true);
    s->m_viewPort->viewport()->setMouseTracking(true);
}

void LVL_ModeSquare::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    if(mouseEvent->buttons() & Qt::RightButton)
    {
        item_rectangles::clearArray();
        s->m_mw->on_actionSelect_triggered();
        dontCallEvent = true;
        s->m_mouseIsMovedAfterKey = true;
        return;
    }

    if(!(mouseEvent->buttons() & Qt::LeftButton)) // Deny any other mouse buttons
        return;

    s->m_lastBlockArrayID = s->m_data->blocks_array_id;
    s->m_lastBgoArrayID = s->m_data->bgo_array_id;
    s->m_lastNpcArrayID = s->m_data->npc_array_id;

    LogDebug(QString("Rectangle mode %1").arg(s->m_editMode));
    if(s->m_cursorItemImg)
    {
        QGraphicsRectItem *cur = dynamic_cast<QGraphicsRectItem *>(s->m_cursorItemImg);
        drawStartPos = QPointF(s->applyGrid(mouseEvent->scenePos().toPoint(),
                                            LvlPlacingItems::gridSz,
                                            LvlPlacingItems::gridOffset));
        s->m_cursorItemImg->setPos(drawStartPos);
        s->m_cursorItemImg->setVisible(true);

        QPoint hw = s->applyGrid(mouseEvent->scenePos().toPoint(),
                                 LvlPlacingItems::gridSz,
                                 LvlPlacingItems::gridOffset);

        QSize hs = QSize((long)fabs(drawStartPos.x() - hw.x()), (long)fabs(drawStartPos.y() - hw.y()));
        cur->setRect(0, 0, hs.width(), hs.height());
    }
}

void LVL_ModeSquare::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    if(!LvlPlacingItems::layer.isEmpty() && LvlPlacingItems::layer != "Default")
        s->setLabelBoxItem(true, mouseEvent->scenePos(), LvlPlacingItems::layer + ", " +
                           QString::number(mouseEvent->scenePos().toPoint().x()) + "x" +
                           QString::number(mouseEvent->scenePos().toPoint().y())
                          );
    else
        s->setLabelBoxItem(false);

    if(s->m_cursorItemImg && s->m_cursorItemImg->isVisible())
    {
        QGraphicsRectItem *cur = dynamic_cast<QGraphicsRectItem *>(s->m_cursorItemImg);
        QPoint hw = s->applyGrid(mouseEvent->scenePos().toPoint(),
                                 LvlPlacingItems::gridSz,
                                 LvlPlacingItems::gridOffset);

        QSize hs = QSize((int)fabs(drawStartPos.x() - hw.x()), (int)fabs(drawStartPos.y() - hw.y()));


        cur->setRect(0, 0, hs.width(), hs.height());
        cur->setPos(
            ((hw.x() < drawStartPos.x()) ? hw.x() : drawStartPos.x()),
            ((hw.y() < drawStartPos.y()) ? hw.y() : drawStartPos.y())
        );
    }

}

void LVL_ModeSquare::mouseRelease(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent)

    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    if(s->m_cursorItemImg)
    {
        QGraphicsRectItem *cur = dynamic_cast<QGraphicsRectItem *>(s->m_cursorItemImg);
        // /////////// Don't draw with zero width or height //////////////
        if((cur->rect().width() == 0.0) || (cur->rect().height() == 0.0))
        {
            s->m_cursorItemImg->hide();
            dontCallEvent = true;
            return;
        }
        // ///////////////////////////////////////////////////////////////

        if(((s->m_placingItemType == LvlScene::PLC_Block) && (!LvlPlacingItems::sizableBlock)) ||
           (s->m_placingItemType == LvlScene::PLC_BGO))
        {
            item_rectangles::drawMatrix(s, QRect(cur->x(), cur->y(), cur->rect().width(), cur->rect().height()),
                                        QSize(LvlPlacingItems::itemW, LvlPlacingItems::itemH));
        }

        switch(s->m_placingItemType)
        {
        case LvlScene::PLC_Water:
        {
            LvlPlacingItems::waterSet.env_type = LvlPlacingItems::waterType;

            LvlPlacingItems::waterSet.x = s->m_cursorItemImg->scenePos().x();
            LvlPlacingItems::waterSet.y = s->m_cursorItemImg->scenePos().y();
            LvlPlacingItems::waterSet.w = cur->rect().width();
            LvlPlacingItems::waterSet.h = cur->rect().height();
            //here define placing water item.
            s->m_data->physenv_array_id++;

            LvlPlacingItems::waterSet.meta.array_id = s->m_data->physenv_array_id;
            s->m_data->physez.push_back(LvlPlacingItems::waterSet);

            s->placeEnvironmentZone(LvlPlacingItems::waterSet, true);
            LevelData plWater;
            plWater.physez.push_back(LvlPlacingItems::waterSet);
            s->m_history->addPlace(plWater);
            s->Debugger_updateItemList();
            break;
        }
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

                LvlPlacingItems::blockSet.meta.array_id = s->m_data->blocks_array_id;
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
        s->m_data->meta.modified = true;

        s->m_cursorItemImg->hide();
    }
}

void LVL_ModeSquare::keyPress(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent)
}

void LVL_ModeSquare::keyRelease(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent)
    switch(keyEvent->key())
    {
    case(Qt::Key_Escape):
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
