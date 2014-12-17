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

#include <common_features/mainwinconnect.h>
#include <common_features/item_rectangles.h>
#include <common_features/themes.h>

#include "wld_mode_fill.h"
#include "../wld_scene.h"
#include "../wld_item_placing.h"
#include "../items/item_tile.h"
#include "../items/item_scene.h"
#include "../items/item_path.h"
#include "../items/item_level.h"

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

void WLD_ModeFill::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);
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

void WLD_ModeFill::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    s->clearSelection();

    if(mouseEvent->modifiers() & Qt::ControlModifier )
        s->setMessageBoxItem(true, mouseEvent->scenePos(),
                               (s->cursor?
                                    (
                               QString::number( s->cursor->scenePos().toPoint().x() ) + "x" +
                               QString::number( s->cursor->scenePos().toPoint().y() )
                                    )
                                        :""));
    else
        s->setMessageBoxItem(false);

    if(s->cursor)
    {
               s->cursor->setPos( QPointF(s->applyGrid( mouseEvent->scenePos().toPoint()-
                                                   QPoint(WldPlacingItems::c_offset_x,
                                                          WldPlacingItems::c_offset_y),
                                                 WldPlacingItems::gridSz,
                                                 WldPlacingItems::gridOffset)));
               s->cursor->show();
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

    backUpPos = scene->cursor->scenePos();

    switch(scene->placingItem)
    {
    case WldScene::PLC_Tile:
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
                        //place tile if collision test
                        WldPlacingItems::TileSet.x = scene->cursor->scenePos().x();
                        WldPlacingItems::TileSet.y = scene->cursor->scenePos().y();

                        scene->WldData->tile_array_id++;
                        WldPlacingItems::TileSet.array_id = scene->WldData->tile_array_id;

                        scene->WldData->tiles.push_back(WldPlacingItems::TileSet);
                        scene->placeTile(WldPlacingItems::TileSet, true);
                        historyBuffer.tiles.push_back(WldPlacingItems::TileSet);

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
                        //place scenery if collision test
                        WldPlacingItems::SceneSet.x = scene->cursor->scenePos().x();
                        WldPlacingItems::SceneSet.y = scene->cursor->scenePos().y();

                        scene->WldData->scene_array_id++;
                        WldPlacingItems::SceneSet.array_id = scene->WldData->scene_array_id;

                        scene->WldData->scenery.push_back(WldPlacingItems::SceneSet);
                        scene->placeScenery(WldPlacingItems::SceneSet, true);
                        historyBuffer.scenery.push_back(WldPlacingItems::SceneSet);

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
                        //place path if collision test
                        WldPlacingItems::PathSet.x = scene->cursor->scenePos().x();
                        WldPlacingItems::PathSet.y = scene->cursor->scenePos().y();

                        scene->WldData->path_array_id++;
                        WldPlacingItems::PathSet.array_id = scene->WldData->path_array_id;

                        scene->WldData->paths.push_back(WldPlacingItems::PathSet);
                        scene->placePath(WldPlacingItems::PathSet, true);
                        historyBuffer.paths.push_back(WldPlacingItems::PathSet);

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
                        //place Level if collision test
                        WldPlacingItems::LevelSet.x = scene->cursor->scenePos().x();
                        WldPlacingItems::LevelSet.y = scene->cursor->scenePos().y();

                        scene->WldData->level_array_id++;
                        WldPlacingItems::LevelSet.array_id = scene->WldData->level_array_id;

                        scene->WldData->levels.push_back(WldPlacingItems::LevelSet);
                        scene->placeLevel(WldPlacingItems::LevelSet, true);
                        historyBuffer.levels.push_back(WldPlacingItems::LevelSet);

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

    scene->cursor->setPos(backUpPos);

    if(
       historyBuffer.tiles.size()>0||
       historyBuffer.scenery.size()>0||
       historyBuffer.paths.size()>0||
       historyBuffer.levels.size()>0
            )
        scene->addPlaceHistory(historyBuffer);
}
