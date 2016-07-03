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

#include <common_features/themes.h>
#include <common_features/main_window_ptr.h>
#include <common_features/item_rectangles.h>

#include "mode_circle.h"
#include "../lvl_scene.h"
#include "../lvl_item_placing.h"
#include "../items/item_bgo.h"
#include "../items/item_block.h"
#include "../items/item_npc.h"
#include "../items/item_water.h"
#include "../items/item_playerpoint.h"
#include "../items/item_door.h"

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

    s->EraserEnabled=false;
    s->PasteFromBuffer=false;
    s->DrawMode=true;
    s->disableMoveItems=false;

    s->_viewPort->setInteractive(true);
    s->_viewPort->setCursor(Themes::Cursor(Themes::cursor_square_fill));
    s->_viewPort->setDragMode(QGraphicsView::NoDrag);
    s->_viewPort->setRenderHint(QPainter::Antialiasing, true);
    s->_viewPort->viewport()->setMouseTracking(true);
}

void LVL_ModeCircle::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    if( mouseEvent->buttons() & Qt::RightButton )
    {
        item_rectangles::clearArray();
        MainWinConnect::pMainWin->on_actionSelect_triggered();
        dontCallEvent = true;
        s->IsMoved = true;
        return;
    }

    s->last_block_arrayID=s->LvlData->blocks_array_id;
    s->last_bgo_arrayID=s->LvlData->bgo_array_id;
    s->last_npc_arrayID=s->LvlData->npc_array_id;

    LogDebug(QString("Circle mode %1").arg(s->EditingMode));
    if(s->cursor)
    {
        drawStartPos = QPointF(s->applyGrid( mouseEvent->scenePos().toPoint(),
                                          LvlPlacingItems::gridSz,
                                          LvlPlacingItems::gridOffset));
        s->cursor->setPos( drawStartPos );
        s->cursor->setVisible(true);

        QPoint hw = s->applyGrid( mouseEvent->scenePos().toPoint(),
                               LvlPlacingItems::gridSz,
                               LvlPlacingItems::gridOffset);

        QSize hs = QSize( (long)fabs(drawStartPos.x() - hw.x()),  (long)fabs( drawStartPos.y() - hw.y() ) );
        dynamic_cast<QGraphicsEllipseItem *>(s->cursor)->setRect(0,0, hs.width(), hs.height());
    }
}

void LVL_ModeCircle::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    if(!LvlPlacingItems::layer.isEmpty() && LvlPlacingItems::layer!="Default")
        s->setMessageBoxItem(true, mouseEvent->scenePos(), LvlPlacingItems::layer + ", " +
                     QString::number( mouseEvent->scenePos().toPoint().x() ) + "x" +
                     QString::number( mouseEvent->scenePos().toPoint().y() )
                      );
    else
        s->setMessageBoxItem(false);

    if(s->cursor && s->cursor->isVisible())
    {
        QGraphicsEllipseItem * cur = dynamic_cast<QGraphicsEllipseItem *>(s->cursor);
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

    if(s->cursor)
    {
        QGraphicsEllipseItem * cur = dynamic_cast<QGraphicsEllipseItem *>(s->cursor);

        // /////////// Don't draw with zero width or height //////////////
        if( (cur->rect().width()==0) || (cur->rect().height()==0))
        {
            s->cursor->hide();
            dontCallEvent = true;
            return;
        }
        // ///////////////////////////////////////////////////////////////

        if( ((s->placingItem==LvlScene::PLC_Block)&&(!LvlPlacingItems::sizableBlock))||
                (s->placingItem==LvlScene::PLC_BGO))
        {
            item_rectangles::drawRound(s, QRect(cur->x(), cur->y(), cur->rect().width(), cur->rect().height()),
                                                QSize(LvlPlacingItems::itemW, LvlPlacingItems::itemH) );
        }

        switch(s->placingItem)
        {
        case LvlScene::PLC_Block:
            {
                //LvlPlacingItems::waterSet.quicksand = (LvlPlacingItems::waterType==1);
                if(LvlPlacingItems::sizableBlock)
                {
                    LvlPlacingItems::blockSet.x = s->cursor->scenePos().x();
                    LvlPlacingItems::blockSet.y = s->cursor->scenePos().y();
                    LvlPlacingItems::blockSet.w = cur->rect().width();
                    LvlPlacingItems::blockSet.h = cur->rect().height();
                    //here define placing water item.
                    s->LvlData->blocks_array_id++;

                    LvlPlacingItems::blockSet.array_id = s->LvlData->blocks_array_id;
                    s->LvlData->blocks.push_back(LvlPlacingItems::blockSet);

                    s->placeBlock(LvlPlacingItems::blockSet, true);
                    LevelData plSzBlock;
                    plSzBlock.blocks.push_back(LvlPlacingItems::blockSet);
                    s->addPlaceHistory(plSzBlock);
                    s->Debugger_updateItemList();
                    break;
                }
                else
                {
                    s->placeItemsByRectArray();
                    //LogDebug("clear collision buffer");
                    s->emptyCollisionCheck = false;
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

                s->emptyCollisionCheck = false;
                s->collisionCheckBuffer.clear();

                s->Debugger_updateItemList();
             break;
            }
        }
        s->LvlData->modified = true;

        s->cursor->hide();
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
            item_rectangles::clearArray();
            MainWinConnect::pMainWin->on_actionSelect_triggered();
            break;
        default:
            break;
    }
}
