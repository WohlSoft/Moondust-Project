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

#include <QElapsedTimer>

#include <mainwindow.h>
#include <common_features/themes.h>
#include <common_features/item_rectangles.h>

#include "mode_fill.h"
#include "../lvl_item_placing.h"
#include "../lvl_history_manager.h"

LVL_ModeFill::LVL_ModeFill(QGraphicsScene *parentScene, QObject *parent)
    : EditMode("Fill", parentScene, parent)
{

}

LVL_ModeFill::~LVL_ModeFill()
{}

void LVL_ModeFill::set()
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
    s->m_viewPort->setCursor(Themes::Cursor(Themes::cursor_flood_fill));
    s->m_viewPort->setDragMode(QGraphicsView::NoDrag);
    s->m_viewPort->setRenderHint(QPainter::Antialiasing, true);
    s->m_viewPort->viewport()->setMouseTracking(true);
}

void LVL_ModeFill::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);
    if( mouseEvent->buttons() & Qt::RightButton )
    {
        s->m_mw->on_actionSelect_triggered();
        dontCallEvent = true;
        s->m_mouseIsMovedAfterKey = true;
        return;
    }
    if(! (mouseEvent->buttons()&Qt::LeftButton) )
        return;

    if(s->m_cursorItemImg)
    {
        attemptFlood(s);
        s->Debugger_updateItemList();
    }

}

void LVL_ModeFill::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    s->clearSelection();

    if((!LvlPlacingItems::layer.isEmpty() && LvlPlacingItems::layer!="Default")||
         (mouseEvent->modifiers() & Qt::ControlModifier) )
        s->setLabelBoxItem(true, mouseEvent->scenePos(),
         ((!LvlPlacingItems::layer.isEmpty() && LvlPlacingItems::layer!="Default")?
            LvlPlacingItems::layer + ", ":"") +
                                   (s->m_cursorItemImg?
                                        (
                                   QString::number( s->m_cursorItemImg->scenePos().toPoint().x() ) + "x" +
                                   QString::number( s->m_cursorItemImg->scenePos().toPoint().y() )
                                        )
                                            :"")
                                   );
    else
        s->setLabelBoxItem(false);

    if(s->m_cursorItemImg)
    {
               s->m_cursorItemImg->setPos( QPointF(s->applyGrid( QPointF(mouseEvent->scenePos()-
                                                   QPointF(LvlPlacingItems::c_offset_x,
                                                          LvlPlacingItems::c_offset_y)).toPoint(),
                                                 LvlPlacingItems::gridSz,
                                                 LvlPlacingItems::gridOffset)));
               s->m_cursorItemImg->show();
    }
}

void LVL_ModeFill::mouseRelease(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent);
}

void LVL_ModeFill::keyPress(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent);
}

void LVL_ModeFill::keyRelease(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent);
    switch(keyEvent->key())
    {
        case (Qt::Key_Escape):
        {
            LvlScene *s = dynamic_cast<LvlScene *>(scene);
            if(s) s->m_mw->on_actionSelect_triggered();
            break;
        }
        default:
            break;
    }

}

void LVL_ModeFill::attemptFlood(LvlScene *scene)
{
    typedef QPair<qreal, qreal> CoorPair;

    //Stack overflow protection
    QElapsedTimer timer;
    timer.start();
    qint64 timeout=3000;

    LevelData historyBuffer;

    QPointF backUpPos;

    backUpPos = scene->m_cursorItemImg->scenePos();

    switch(scene->m_placingItemType)
    {
    case LvlScene::PLC_Block:
        {
            QList<CoorPair> blackList; //items which don't pass the test anymore
            QList<CoorPair> nextList; //items to be checked next
            nextList << qMakePair<qreal, qreal>(scene->m_cursorItemImg->x(),scene->m_cursorItemImg->y());
            while(true)
            {
                QList<CoorPair> newList; //items to be checked next in the next loop
                foreach (CoorPair coor, nextList)
                {
                    if(blackList.contains(coor)) //don't check block in blacklist
                        continue;

                    scene->m_cursorItemImg->setPos(coor.first, coor.second);

                    if(!scene->itemCollidesWith(scene->m_cursorItemImg))
                    {
                        if(LvlPlacingItems::noOutSectionFlood)
                        {
                            if(!scene->isInSection(coor.first,
                                                   coor.second,
                                                   LvlPlacingItems::blockSet.w,
                                                   LvlPlacingItems::blockSet.h,
                                                   scene->m_data->CurSection, -1))
                            {
                                blackList << coor;
                                continue;
                            }
                        }
                        //place block if collision test
                        LvlPlacingItems::blockSet.x = coor.first;
                        LvlPlacingItems::blockSet.y = coor.second;
                        scene->m_data->blocks_array_id++;

                        LvlPlacingItems::blockSet.meta.array_id = scene->m_data->blocks_array_id;
                        scene->m_data->blocks.push_back(LvlPlacingItems::blockSet);

                        scene->placeBlock(LvlPlacingItems::blockSet, true);
                        historyBuffer.blocks.push_back(LvlPlacingItems::blockSet);
                        //expand on all sides
                        newList << qMakePair<qreal, qreal>(coor.first - LvlPlacingItems::blockSet.w,coor.second);
                        newList << qMakePair<qreal, qreal>(coor.first,coor.second - LvlPlacingItems::blockSet.h);
                        newList << qMakePair<qreal, qreal>(coor.first,coor.second + LvlPlacingItems::blockSet.h);
                        newList << qMakePair<qreal, qreal>(coor.first + LvlPlacingItems::blockSet.w,coor.second);
                    }
                    blackList << coor; //add current item to black list as it passed the test.
                }

                if(timer.elapsed() > timeout) break; //abort loop on time out

                if(newList.empty()) //if no blocks to add then break;
                    break;

                nextList = newList; //update next list
            }
        }
        break;
    case LvlScene::PLC_BGO:
        {
            QList<CoorPair> blackList; //items which don't pass the test anymore
            QList<CoorPair> nextList; //items to be checked next
            nextList << qMakePair<qreal, qreal>(scene->m_cursorItemImg->x(),scene->m_cursorItemImg->y());
            while(true)
            {
                QList<CoorPair> newList; //items to be checked next in the next loop
                foreach (CoorPair coor, nextList)
                {
                    if(blackList.contains(coor)) //don't check block in blacklist
                        continue;

                    scene->m_cursorItemImg->setPos(coor.first, coor.second);

                    if(!scene->itemCollidesWith(scene->m_cursorItemImg))
                    {
                        if(LvlPlacingItems::noOutSectionFlood){
                            if(!scene->isInSection(coor.first, coor.second, LvlPlacingItems::itemW, LvlPlacingItems::itemH, scene->m_data->CurSection)){
                                blackList << coor;
                                continue;
                            }
                        }

                        //place BGO if collision test
                        LvlPlacingItems::bgoSet.x = coor.first;
                        LvlPlacingItems::bgoSet.y = coor.second;
                        scene->m_data->bgo_array_id++;

                        LvlPlacingItems::bgoSet.meta.array_id = scene->m_data->bgo_array_id;
                        scene->m_data->bgo.push_back(LvlPlacingItems::bgoSet);

                        scene->placeBGO(LvlPlacingItems::bgoSet);
                        historyBuffer.bgo.push_back(LvlPlacingItems::bgoSet);
                        //expand on all sides
                        newList << qMakePair<qreal, qreal>(coor.first - LvlPlacingItems::itemW,coor.second);
                        newList << qMakePair<qreal, qreal>(coor.first,coor.second - LvlPlacingItems::itemH);
                        newList << qMakePair<qreal, qreal>(coor.first,coor.second + LvlPlacingItems::itemH);
                        newList << qMakePair<qreal, qreal>(coor.first + LvlPlacingItems::itemW,coor.second);
                    }
                    blackList << coor; //add current item to black list as it passed the test.
                }

                if(timer.elapsed() > timeout) break; //abort loop on time out

                if(newList.empty()) //if no blocks to add then break;
                    break;

                nextList = newList; //update next list
            }
        }
        break;
    default:
        break;
    }

    scene->m_cursorItemImg->setPos(backUpPos);

    if(
       historyBuffer.blocks.size()>0||
       historyBuffer.bgo.size()>0
            )

        scene->m_history->addPlace(historyBuffer);

}
