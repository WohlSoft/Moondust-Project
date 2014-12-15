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

#include <QElapsedTimer>

#include <common_features/themes.h>
#include <common_features/mainwinconnect.h>
#include <common_features/item_rectangles.h>

#include "mode_fill.h"
#include "../lvl_item_placing.h"
#include "../lvl_scene.h"
#include "../items/item_bgo.h"
#include "../items/item_block.h"
#include "../items/item_npc.h"
#include "../items/item_water.h"
#include "../items/item_playerpoint.h"
#include "../items/item_door.h"

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

    s->EraserEnabled=false;
    s->PasteFromBuffer=false;
    s->DrawMode=true;
    s->disableMoveItems=false;

    s->_viewPort->setInteractive(true);
    s->_viewPort->setCursor(Themes::Cursor(Themes::cursor_flood_fill));
    s->_viewPort->setDragMode(QGraphicsView::NoDrag);
    s->_viewPort->setRenderHint(QPainter::Antialiasing, true);
    s->_viewPort->viewport()->setMouseTracking(true);
}

void LVL_ModeFill::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);
    if( mouseEvent->buttons() & Qt::RightButton )
    {
        MainWinConnect::pMainWin->on_actionSelect_triggered();
        dontCallEvent = true;
        s->IsMoved = true;
        return;
    }
    if(! mouseEvent->buttons() & Qt::LeftButton)
        return;

    if(s->cursor)
    {
        attemptFlood(s);
    }

}

void LVL_ModeFill::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    s->clearSelection();

    if((!LvlPlacingItems::layer.isEmpty() && LvlPlacingItems::layer!="Default")||
         (mouseEvent->modifiers() & Qt::ControlModifier) )
        s->setMessageBoxItem(true, mouseEvent->scenePos(),
         ((!LvlPlacingItems::layer.isEmpty() && LvlPlacingItems::layer!="Default")?
            LvlPlacingItems::layer + ", ":"") +
                                   (s->cursor?
                                        (
                                   QString::number( s->cursor->scenePos().toPoint().x() ) + "x" +
                                   QString::number( s->cursor->scenePos().toPoint().y() )
                                        )
                                            :"")
                                   );
    else
        s->setMessageBoxItem(false);

    if(s->cursor)
    {
               s->cursor->setPos( QPointF(s->applyGrid( QPointF(mouseEvent->scenePos()-
                                                   QPointF(LvlPlacingItems::c_offset_x,
                                                          LvlPlacingItems::c_offset_y)).toPoint(),
                                                 LvlPlacingItems::gridSz,
                                                 LvlPlacingItems::gridOffset)));
               s->cursor->show();
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
            MainWinConnect::pMainWin->on_actionSelect_triggered();
            break;
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

    backUpPos = scene->cursor->scenePos();

    switch(scene->placingItem)
    {
    case LvlScene::PLC_Block:
        {
            QList<CoorPair> blackList; //items which don't pass the test anymore
            QList<CoorPair> nextList; //items to be checked next
            nextList << qMakePair<qreal, qreal>(scene->cursor->x(),scene->cursor->y());
            while(true)
            {
                QList<CoorPair> newList; //items to be checked next in the next loop
                foreach (CoorPair coor, nextList)
                {
                    if(blackList.contains(coor)) //don't check block in blacklist
                        continue;

                    scene->cursor->setPos(coor.first, coor.second);

                    if(!scene->itemCollidesWith(scene->cursor))
                    {
                        if(LvlPlacingItems::noOutSectionFlood){
                            if(!scene->isInSection(coor.first, coor.second, LvlPlacingItems::blockSet.w, LvlPlacingItems::blockSet.h, scene->LvlData->CurSection)){
                                blackList << coor;
                                continue;
                            }
                        }
                        //place block if collision test
                        LvlPlacingItems::blockSet.x = coor.first;
                        LvlPlacingItems::blockSet.y = coor.second;
                        scene->LvlData->blocks_array_id++;

                        LvlPlacingItems::blockSet.array_id = scene->LvlData->blocks_array_id;
                        scene->LvlData->blocks.push_back(LvlPlacingItems::blockSet);

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
            nextList << qMakePair<qreal, qreal>(scene->cursor->x(),scene->cursor->y());
            while(true)
            {
                QList<CoorPair> newList; //items to be checked next in the next loop
                foreach (CoorPair coor, nextList)
                {
                    if(blackList.contains(coor)) //don't check block in blacklist
                        continue;

                    scene->cursor->setPos(coor.first, coor.second);

                    if(!scene->itemCollidesWith(scene->cursor))
                    {
                        if(LvlPlacingItems::noOutSectionFlood){
                            if(!scene->isInSection(coor.first, coor.second, LvlPlacingItems::itemW, LvlPlacingItems::itemH, scene->LvlData->CurSection)){
                                blackList << coor;
                                continue;
                            }
                        }

                        //place BGO if collision test
                        LvlPlacingItems::bgoSet.x = coor.first;
                        LvlPlacingItems::bgoSet.y = coor.second;
                        scene->LvlData->bgo_array_id++;

                        LvlPlacingItems::bgoSet.array_id = scene->LvlData->bgo_array_id;
                        scene->LvlData->bgo.push_back(LvlPlacingItems::bgoSet);

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

    scene->cursor->setPos(backUpPos);

    if(
       historyBuffer.blocks.size()>0||
       historyBuffer.bgo.size()>0
            )

        scene->addPlaceHistory(historyBuffer);

}
