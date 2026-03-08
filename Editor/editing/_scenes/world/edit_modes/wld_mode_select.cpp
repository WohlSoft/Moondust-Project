/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2026 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <common_features/main_window_ptr.h>
#include <common_features/item_rectangles.h>
#include <common_features/themes.h>

#include "wld_mode_select.h"
#include "../wld_scene.h"
#include "../wld_history_manager.h"

WLD_ModeSelect::WLD_ModeSelect(QGraphicsScene *parentScene, QObject *parent)
    : EditMode("Select", parentScene, parent)
{
    sourcePos = QPoint(0, 0);
    gridSize = 0;
    offsetX = 0;
    offsetY = 0; //, gridX, gridY, i=0;
}

WLD_ModeSelect::~WLD_ModeSelect()
{}


void WLD_ModeSelect::set()
{
    set(MoondustBaseScene::MODE_Selecting);
}

void WLD_ModeSelect::set(int editMode)
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    s->resetCursor();
    s->resetResizers();
    s->m_pointSelector.unserPointSelector();

    s->setEditFlagEraser(false);
    s->setEditFlagPasteMode(false);
    s->setEditFlagBusyMode(false);
    s->setEditFlagNoMoveItems(false);

    auto *vp = s->curViewPort();
    if(editMode == MoondustBaseScene::MODE_PasteFromClip)
    {
        s->clearSelection();
        vp->setInteractive(true);
        vp->setCursor(Themes::Cursor(Themes::cursor_pasting));
        vp->setDragMode(QGraphicsView::NoDrag);
        s->setEditFlagNoMoveItems(true);
    }
    else
    {
        vp->setInteractive(true);
        vp->setCursor(Themes::Cursor(Themes::cursor_normal));
        vp->setDragMode(QGraphicsView::RubberBandDrag);
    }

    if(editMode == MoondustBaseScene::MODE_SelectingOnly)
        s->setEditFlagNoMoveItems(true);
}


void WLD_ModeSelect::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene)
        return;

    WldScene *s = dynamic_cast<WldScene *>(scene);

    if(s->editMode() == MoondustBaseScene::MODE_PasteFromClip)
    {
        if(mouseEvent->buttons() & Qt::RightButton)
        {
            MainWinConnect::pMainWin->on_actionSelect_triggered();
            dontCallEvent = true;
            s->m_mouseIsMovedAfterKey = true;
            return;
        }
        s->setEditFlagPasteMode(true);
        dontCallEvent = true;
        s->m_mouseIsMovedAfterKey = true;
        return;
    }

    if(s->getEditFlagNoMoveItems() && (mouseEvent->buttons() & Qt::LeftButton)
       && (Qt::ControlModifier != QApplication::keyboardModifiers()))
        return;

    if(mouseEvent->buttons() & Qt::MiddleButton)
    {
        if(GlobalSettings::MidMouse_allowSwitchToPlace)
        {
            if(s->selectedItems().size() == 1)
            {
                const auto &ss = s->selectedItems();
                QGraphicsItem *it = ss.first();
                int itp = it->data(WldScene::ITEM_TYPE_INT).toInt();

                unsigned long itd = (unsigned long)it->data(WldScene::ITEM_ID).toULongLong();
                if(itp == ItemTypes::WLD_Tile)
                {
                    MainWinConnect::pMainWin->SwitchPlacingItem(ItemTypes::WLD_Tile, itd);
                    return;
                }
                else if(itp == ItemTypes::WLD_Scenery)
                {
                    MainWinConnect::pMainWin->SwitchPlacingItem(ItemTypes::WLD_Scenery, itd);
                    return;
                }
                else if(itp == ItemTypes::WLD_Path)
                {
                    MainWinConnect::pMainWin->SwitchPlacingItem(ItemTypes::WLD_Path, itd);
                    return;
                }
                else if(itp == ItemTypes::WLD_Level)
                {
                    MainWinConnect::pMainWin->SwitchPlacingItem(ItemTypes::WLD_Level, itd);
                    return;
                }
                else if(itp == ItemTypes::WLD_MusicBox)
                {
                    MainWinConnect::pMainWin->SwitchPlacingItem(ItemTypes::WLD_MusicBox, itd);
                    return;
                }
            }
        }

        if(GlobalSettings::MidMouse_allowDuplicate)
        {
            if(!s->selectedItems().isEmpty())
            {
                s->WldBuffer = s->copy();
                s->setEditFlagPasteMode(true);
            }
        }
    }
}


void WLD_ModeSelect::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    if(!(mouseEvent->buttons() & Qt::LeftButton)) return;
    if(s->m_cursorItemImg) s->m_cursorItemImg->setPos(mouseEvent->scenePos());
}

void WLD_ModeSelect::mouseRelease(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    if(mouseEvent->button() != Qt::LeftButton)
    {
        dontCallEvent = true;
        return;
    }

    s->m_cursorItemImg->hide();

    //s->haveSelected = false;

    int ObjType;
    bool collisionPassed = false;

    //History
    WorldData historyBuffer;// bool deleted=false;
    WorldData historySourceBuffer;

    if(s->getEditFlagPasteMode())
    {
        s->paste(s->WldBuffer, mouseEvent->scenePos().toPoint());
        s->Debugger_updateItemList();
        s->setEditFlagPasteMode(false);
        s->m_mouseIsMoved = false;
        MainWinConnect::pMainWin->on_actionSelect_triggered();
    }

    MoondustBaseScene::PGE_ItemList selectedList = s->selectedItems();

    // check for grid snap
    if((!selectedList.isEmpty()) && (s->m_mouseIsMoved))
    {
        //Set Grid Size/Offset, sourcePosition
        setItemSourceData(selectedList.first(), selectedList.first()->data(WldScene::ITEM_TYPE_INT).toInt());
        //Check first selected element is it was moved
        if((sourcePos == QPoint(
                (long)(selectedList.first()->scenePos().x()),
                ((long)selectedList.first()->scenePos().y())
            )))
        {
            s->m_mouseIsMoved = false;
            return; //break fetch when items is not moved
        }


        s->applyGroupGrid(selectedList);

        // Check collisions
        //Only if collision ckecking enabled
        if(!s->getEditFlagPasteMode())
        {
            if(s->m_opts.collisionsEnabled && s->checkGroupCollisions(selectedList))
            {
                collisionPassed = false;
                s->returnItemBackGroup(selectedList);
            }
            else
            {
                collisionPassed = true;
                //applyArrayForItemGroup(selectedList);
                s->m_data->meta.modified = true;
            }
        }


        if(collisionPassed || !s->m_opts.collisionsEnabled)
        {
            for(QList<QGraphicsItem *>::iterator it = selectedList.begin(); it != selectedList.end(); it++)
            {
                ////////////////////////SECOND FETCH///////////////////////
                ObjType = (*it)->data(WldScene::ITEM_TYPE_INT).toInt();

                /////////////////////////GET DATA///////////////
                setItemSourceData((*it), ObjType); //Set Grid Size/Offset, sourcePosition
                /////////////////////////GET DATA/////////////////////

                //Check position
                if((sourcePos == QPoint((long)((*it)->scenePos().x()), ((long)(*it)->scenePos().y()))))
                {
                    s->m_mouseIsMoved = false;
                    break; //break fetch when items is not moved
                }

                if(ObjType == ItemTypes::WLD_Tile)
                {
                    auto *item = dynamic_cast<ItemTile *>(*it);
                    Q_ASSERT(item);
                    //Applay move into main array
                    historySourceBuffer.tiles.push_back(item->m_data);
                    // item->m_data.x = (long)item->scenePos().x();
                    // item->m_data.y = (long)item->scenePos().y();
                    item->arrayApply();
                    historyBuffer.tiles.push_back(item->m_data);
                    s->m_data->meta.modified = true;
                }
                else if(ObjType == ItemTypes::WLD_Scenery)
                {
                    auto *item = dynamic_cast<ItemScene *>(*it);
                    Q_ASSERT(item);
                    //Applay move into main array
                    historySourceBuffer.scenery.push_back(item->m_data);
                    // item->m_data.x = (long)item->scenePos().x();
                    // item->m_data.y = (long)item->scenePos().y();
                    item->arrayApply();
                    historyBuffer.scenery.push_back(item->m_data);
                    s->m_data->meta.modified = true;
                }
                else if(ObjType == ItemTypes::WLD_Path)
                {
                    auto *item = dynamic_cast<ItemPath *>(*it);
                    Q_ASSERT(item);
                    //Applay move into main array
                    historySourceBuffer.paths.push_back(item->m_data);
                    // item->m_data.x = (long)item->scenePos().x();
                    // item->m_data.y = (long)item->scenePos().y();
                    item->arrayApply();
                    historyBuffer.paths.push_back(item->m_data);
                    s->m_data->meta.modified = true;
                }
                else if(ObjType == ItemTypes::WLD_Level)
                {
                    auto *item = dynamic_cast<ItemLevel *>(*it);
                    Q_ASSERT(item);
                    //Applay move into main array
                    historySourceBuffer.levels.push_back(item->m_data);
                    // item->m_data.x = (long)item->scenePos().x();
                    // item->m_data.y = (long)item->scenePos().y();
                    item->arrayApply();
                    historyBuffer.levels.push_back(item->m_data);
                    s->m_data->meta.modified = true;
                }
                else if(ObjType == ItemTypes::WLD_MusicBox)
                {
                    auto *item = dynamic_cast<ItemMusic *>(*it);
                    Q_ASSERT(item);
                    //Applay move into main array
                    historySourceBuffer.music.push_back(item->m_data);
                    // item->m_data.x = (long)item->scenePos().x();
                    // item->m_data.y = (long)item->scenePos().y();
                    item->arrayApply();
                    historyBuffer.music.push_back(item->m_data);
                    s->m_data->meta.modified = true;
                }

            }////////////////////////SECOND FETCH///////////////////////
        }

        if(s->m_mouseIsMoved)
            s->m_history->addMoveHistory(historySourceBuffer, historyBuffer);

        s->m_mouseIsMoved = false;
    }
}

void WLD_ModeSelect::keyPress(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent)
}

void WLD_ModeSelect::keyRelease(QKeyEvent *keyEvent)
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    switch(keyEvent->key())
    {
    case(Qt::Key_Delete):  //Delete action
        s->removeSelectedWldItems();
        break;
    case(Qt::Key_Escape):
        if(!s->m_mouseIsMoved)
            s->clearSelection();
        break;
    default:
        break;
    }
}



void WLD_ModeSelect::setItemSourceData(QGraphicsItem *it, int ObjType)
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    gridSize = s->m_configs->defaultGrid.general;
    offsetX = 0;
    offsetY = 0;

    if(ObjType == ItemTypes::WLD_Tile ||
       ObjType == ItemTypes::WLD_Scenery ||
       ObjType == ItemTypes::WLD_Path ||
       ObjType == ItemTypes::WLD_Level ||
       ObjType == ItemTypes::WLD_MusicBox)
    {
        sourcePos = ((WldBaseItem *)it)->sourcePos();
        gridSize = ((WldBaseItem *)it)->getGridSize();
        QPoint p = ((WldBaseItem *)it)->gridOffset();
        offsetX = p.x();
        offsetY = p.y();
    }
}
