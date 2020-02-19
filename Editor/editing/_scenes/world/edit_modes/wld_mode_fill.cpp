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

#include <common_features/main_window_ptr.h>
#include <common_features/item_rectangles.h>
#include <common_features/themes.h>

#include "wld_mode_fill.h"
#include "../wld_scene.h"
#include "../wld_item_placing.h"
#include "../wld_history_manager.h"

WLD_ModeFill::WLD_ModeFill(QGraphicsScene *parentScene, QObject *parent)
    : EditMode("Fill", parentScene, parent)
{}

WLD_ModeFill::~WLD_ModeFill()
{}

void WLD_ModeFill::set()
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

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

void WLD_ModeFill::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);
    if( mouseEvent->buttons() & Qt::RightButton )
    {
        MainWinConnect::pMainWin->on_actionSelect_triggered();
        dontCallEvent = true;
        s->m_mouseIsMovedAfterKey = true;
        return;
    }
    if(! (mouseEvent->buttons() & Qt::LeftButton) )
        return;

    if(s->m_cursorItemImg)
    {
        attemptFlood(s);
        s->Debugger_updateItemList();
    }

}

void WLD_ModeFill::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    s->clearSelection();

    if(mouseEvent->modifiers() & Qt::ControlModifier )
        s->setMessageBoxItem(true, mouseEvent->scenePos(),
                               (s->m_cursorItemImg?
                                    (
                               QString::number( s->m_cursorItemImg->scenePos().toPoint().x() ) + "x" +
                               QString::number( s->m_cursorItemImg->scenePos().toPoint().y() )
                                    )
                                        :""));
    else
        s->setMessageBoxItem(false);

    if(s->m_cursorItemImg)
    {
               s->m_cursorItemImg->setPos( QPointF(s->applyGrid( mouseEvent->scenePos().toPoint()-
                                                   QPoint(WldPlacingItems::c_offset_x,
                                                          WldPlacingItems::c_offset_y),
                                                 WldPlacingItems::gridSz,
                                                 WldPlacingItems::gridOffset)));
               s->m_cursorItemImg->show();
    }
}

void WLD_ModeFill::mouseRelease(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent);
}

void WLD_ModeFill::keyPress(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent);
}

void WLD_ModeFill::keyRelease(QKeyEvent *keyEvent)
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

void WLD_ModeFill::attemptFlood(WldScene *scene)
{
    typedef QPair<qreal, qreal> CoorPair;

    //Stack overflow protection
    QElapsedTimer timer;
    timer.start();
    qint64 timeout=3000;

    WorldData historyBuffer;

    QPointF backUpPos;

    backUpPos = scene->m_cursorItemImg->scenePos();

    switch(scene->m_placingItemType)
    {
    case WldScene::PLC_Terrain:
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
                        //place tile if collision test
                        WldPlacingItems::terrainSet.x = scene->m_cursorItemImg->scenePos().x();
                        WldPlacingItems::terrainSet.y = scene->m_cursorItemImg->scenePos().y();

                        scene->m_data->tile_array_id++;
                        WldPlacingItems::terrainSet.meta.array_id = scene->m_data->tile_array_id;

                        scene->m_data->tiles.push_back(WldPlacingItems::terrainSet);
                        scene->placeTile(WldPlacingItems::terrainSet, true);
                        historyBuffer.tiles.push_back(WldPlacingItems::terrainSet);

                        //expand on all sides
                        newList << qMakePair<qreal, qreal>(coor.first - WldPlacingItems::itemW,coor.second);
                        newList << qMakePair<qreal, qreal>(coor.first,coor.second - WldPlacingItems::itemH);
                        newList << qMakePair<qreal, qreal>(coor.first,coor.second + WldPlacingItems::itemH);
                        newList << qMakePair<qreal, qreal>(coor.first + WldPlacingItems::itemW,coor.second);
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
    case WldScene::PLC_Scene:
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
                        //place scenery if collision test
                        WldPlacingItems::sceneSet.x = scene->m_cursorItemImg->scenePos().x();
                        WldPlacingItems::sceneSet.y = scene->m_cursorItemImg->scenePos().y();

                        scene->m_data->scene_array_id++;
                        WldPlacingItems::sceneSet.meta.array_id = scene->m_data->scene_array_id;

                        scene->m_data->scenery.push_back(WldPlacingItems::sceneSet);
                        scene->placeScenery(WldPlacingItems::sceneSet, true);
                        historyBuffer.scenery.push_back(WldPlacingItems::sceneSet);

                        //expand on all sides
                        newList << qMakePair<qreal, qreal>(coor.first - WldPlacingItems::itemW,coor.second);
                        newList << qMakePair<qreal, qreal>(coor.first,coor.second - WldPlacingItems::itemH);
                        newList << qMakePair<qreal, qreal>(coor.first,coor.second + WldPlacingItems::itemH);
                        newList << qMakePair<qreal, qreal>(coor.first + WldPlacingItems::itemW,coor.second);
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
    case WldScene::PLC_Path:
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
                        //place path if collision test
                        WldPlacingItems::pathSet.x = scene->m_cursorItemImg->scenePos().x();
                        WldPlacingItems::pathSet.y = scene->m_cursorItemImg->scenePos().y();

                        scene->m_data->path_array_id++;
                        WldPlacingItems::pathSet.meta.array_id = scene->m_data->path_array_id;

                        scene->m_data->paths.push_back(WldPlacingItems::pathSet);
                        scene->placePath(WldPlacingItems::pathSet, true);
                        historyBuffer.paths.push_back(WldPlacingItems::pathSet);

                        //expand on all sides
                        newList << qMakePair<qreal, qreal>(coor.first - WldPlacingItems::itemW,coor.second);
                        newList << qMakePair<qreal, qreal>(coor.first,coor.second - WldPlacingItems::itemH);
                        newList << qMakePair<qreal, qreal>(coor.first,coor.second + WldPlacingItems::itemH);
                        newList << qMakePair<qreal, qreal>(coor.first + WldPlacingItems::itemW,coor.second);
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
    case WldScene::PLC_Level:
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
                        //place Level if collision test
                        WldPlacingItems::levelSet.x = scene->m_cursorItemImg->scenePos().x();
                        WldPlacingItems::levelSet.y = scene->m_cursorItemImg->scenePos().y();

                        scene->m_data->level_array_id++;
                        WldPlacingItems::levelSet.meta.array_id = scene->m_data->level_array_id;

                        scene->m_data->levels.push_back(WldPlacingItems::levelSet);
                        scene->placeLevel(WldPlacingItems::levelSet, true);
                        historyBuffer.levels.push_back(WldPlacingItems::levelSet);

                        //expand on all sides
                        newList << qMakePair<qreal, qreal>(coor.first - WldPlacingItems::itemW,coor.second);
                        newList << qMakePair<qreal, qreal>(coor.first,coor.second - WldPlacingItems::itemH);
                        newList << qMakePair<qreal, qreal>(coor.first,coor.second + WldPlacingItems::itemH);
                        newList << qMakePair<qreal, qreal>(coor.first + WldPlacingItems::itemW,coor.second);
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
       historyBuffer.tiles.size()>0||
       historyBuffer.scenery.size()>0||
       historyBuffer.paths.size()>0||
       historyBuffer.levels.size()>0
            )
        scene->m_history->addPlaceHistory(historyBuffer);
}
