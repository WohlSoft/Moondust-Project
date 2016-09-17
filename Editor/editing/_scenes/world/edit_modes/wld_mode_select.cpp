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

#include <common_features/main_window_ptr.h>
#include <common_features/item_rectangles.h>
#include <common_features/themes.h>

#include "wld_mode_select.h"
#include "../wld_scene.h"
#include "../wld_history_manager.h"

WLD_ModeSelect::WLD_ModeSelect(QGraphicsScene *parentScene, QObject *parent)
    : EditMode("Select", parentScene, parent)
{
    sourcePos=QPoint(0,0);
    gridSize=0;
    offsetX=0;
    offsetY=0;//, gridX, gridY, i=0;
}

WLD_ModeSelect::~WLD_ModeSelect()
{}


void WLD_ModeSelect::set()
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    s->resetCursor();
    s->resetResizers();
    s->m_pointSelector.unserPointSelector();

    s->m_eraserIsEnabled=false;
    s->m_pastingMode=false;
    s->m_busyMode=false;
    s->m_disableMoveItems=false;

    s->m_viewPort->setInteractive(true);
    s->m_viewPort->setCursor(Themes::Cursor(Themes::cursor_normal));
    s->m_viewPort->setDragMode(QGraphicsView::RubberBandDrag);
}


void WLD_ModeSelect::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    if(s->m_editMode == WldScene::MODE_PasteFromClip)
    {
        if( mouseEvent->buttons() & Qt::RightButton )
        {
            MainWinConnect::pMainWin->on_actionSelect_triggered();
            dontCallEvent = true;
            s->m_mouseIsMovedAfterKey = true;
            return;
        }
        s->m_pastingMode = true;
        dontCallEvent = true;
        s->m_mouseIsMovedAfterKey = true;
        return;
    }

    if((s->m_disableMoveItems) && (mouseEvent->buttons() & Qt::LeftButton)
        && (Qt::ControlModifier != QApplication::keyboardModifiers()))
    { return; }

    if( mouseEvent->buttons() & Qt::MiddleButton )
    {
        if(GlobalSettings::MidMouse_allowSwitchToPlace)
        {
            if(s->selectedItems().size()==1)
            {
                QGraphicsItem * it = s->selectedItems().first();
                QString itp = it->data(0).toString();
                long itd = it->data(1).toInt();
                if(itp=="TILE")
                {MainWinConnect::pMainWin->SwitchPlacingItem(ItemTypes::WLD_Tile, itd); return;}
                else if(itp=="SCENERY")
                {MainWinConnect::pMainWin->SwitchPlacingItem(ItemTypes::WLD_Scenery, itd); return;}
                else if(itp=="PATH")
                {MainWinConnect::pMainWin->SwitchPlacingItem(ItemTypes::WLD_Path, itd); return;}
                else if(itp=="LEVEL")
                {MainWinConnect::pMainWin->SwitchPlacingItem(ItemTypes::WLD_Level, itd); return;}
                else if(itp=="MUSICBOX")
                {MainWinConnect::pMainWin->SwitchPlacingItem(ItemTypes::WLD_MusicBox, itd); return;}
            }
        }
        if(GlobalSettings::MidMouse_allowDuplicate)
        {
            if(!s->selectedItems().isEmpty())
            {
                s->WldBuffer = s->copy();
                s->m_pastingMode=true;
            }
        }
    }
}


void WLD_ModeSelect::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    if(!( mouseEvent->buttons() & Qt::LeftButton )) return;
    if(s->m_cursorItemImg) s->m_cursorItemImg->setPos(mouseEvent->scenePos());
}

void WLD_ModeSelect::mouseRelease(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    if( mouseEvent->button() != Qt::LeftButton )
    {
        dontCallEvent = true;
        return;
    }

    s->m_cursorItemImg->hide();

    //s->haveSelected = false;

    QString ObjType;
    bool collisionPassed = false;

    //History
    WorldData historyBuffer;// bool deleted=false;
    WorldData historySourceBuffer;

    if(s->m_pastingMode)
    {
        s->paste( s->WldBuffer, mouseEvent->scenePos().toPoint() );
        s->Debugger_updateItemList();
        s->m_pastingMode = false;
        s->m_mouseIsMoved=false;
        MainWinConnect::pMainWin->on_actionSelect_triggered();
    }

    QList<QGraphicsItem*> selectedList = s->selectedItems();

    // check for grid snap
    if ((!selectedList.isEmpty())&&(s->m_mouseIsMoved))
    {

        //Set Grid Size/Offset, sourcePosition
        setItemSourceData(selectedList.first(), selectedList.first()->data(0).toString());
        //Check first selected element is it was moved
        if( (sourcePos == QPoint(
                 (long)(selectedList.first()->scenePos().x()),
                 ((long)selectedList.first()->scenePos().y())
                 ) ) )
        {
            s->m_mouseIsMoved=false;
            return; //break fetch when items is not moved
        }


        s->applyGroupGrid(selectedList);

        // Check collisions
        //Only if collision ckecking enabled
        if(!s->m_pastingMode)
        {
            if(s->m_opts.collisionsEnabled && s->checkGroupCollisions(&selectedList))
            {
                collisionPassed = false;
                s->returnItemBackGroup(selectedList);
            }
            else
            {
                collisionPassed = true;
                //applyArrayForItemGroup(selectedList);
                s->m_data->meta.modified=true;
            }
        }


        if((collisionPassed) || (!s->m_opts.collisionsEnabled))
        for (QList<QGraphicsItem*>::iterator it = selectedList.begin();
             it != selectedList.end(); it++)
        { ////////////////////////SECOND FETCH///////////////////////
           ObjType = (*it)->data(0).toString();

           /////////////////////////GET DATA///////////////
           setItemSourceData((*it), (*it)->data(0).toString()); //Set Grid Size/Offset, sourcePosition
           /////////////////////////GET DATA/////////////////////

           //Check position
           if( (sourcePos == QPoint((long)((*it)->scenePos().x()), ((long)(*it)->scenePos().y()))))
           {
               s->m_mouseIsMoved=false;
               break; //break fetch when items is not moved
           }

           if( ObjType == "TILE")
           {
               //Applay move into main array
               historySourceBuffer.tiles.push_back(((ItemTile *)(*it))->m_data);
               ((ItemTile *)(*it))->m_data.x = (long)(*it)->scenePos().x();
               ((ItemTile *)(*it))->m_data.y = (long)(*it)->scenePos().y();
               ((ItemTile *)(*it))->arrayApply();
               historyBuffer.tiles.push_back(((ItemTile *)(*it))->m_data);
               s->m_data->meta.modified = true;
           }
           else
           if( ObjType == "SCENERY")
           {
               //Applay move into main array
               historySourceBuffer.scenery.push_back(((ItemScene *)(*it))->m_data);
               ((ItemScene *)(*it))->m_data.x = (long)(*it)->scenePos().x();
               ((ItemScene *)(*it))->m_data.y = (long)(*it)->scenePos().y();
               ((ItemScene *)(*it))->arrayApply();
               historyBuffer.scenery.push_back(((ItemScene *)(*it))->m_data);
               s->m_data->meta.modified = true;
           }
           else
           if( ObjType == "PATH")
           {
               //Applay move into main array
               historySourceBuffer.paths.push_back(((ItemPath*)(*it))->m_data);
               ((ItemPath *)(*it))->m_data.x = (long)(*it)->scenePos().x();
               ((ItemPath *)(*it))->m_data.y = (long)(*it)->scenePos().y();
               ((ItemPath *)(*it))->arrayApply();
               historyBuffer.paths.push_back(((ItemPath *)(*it))->m_data);
               s->m_data->meta.modified = true;
           }
           else
           if( ObjType == "LEVEL")
           {
               //Applay move into main array
               historySourceBuffer.levels.push_back(((ItemLevel *)(*it))->m_data);
               ((ItemLevel *)(*it))->m_data.x = (long)(*it)->scenePos().x();
               ((ItemLevel *)(*it))->m_data.y = (long)(*it)->scenePos().y();
               ((ItemLevel *)(*it))->arrayApply();
               historyBuffer.levels.push_back(((ItemLevel *)(*it))->m_data);
               s->m_data->meta.modified = true;
           }
           else
           if( ObjType == "MUSICBOX")
           {
               //Applay move into main array
               historySourceBuffer.music.push_back(((ItemMusic *)(*it))->m_data);
               ((ItemMusic *)(*it))->m_data.x = (long)(*it)->scenePos().x();
               ((ItemMusic *)(*it))->m_data.y = (long)(*it)->scenePos().y();
               ((ItemMusic *)(*it))->arrayApply();
               historyBuffer.music.push_back(((ItemMusic *)(*it))->m_data);
               s->m_data->meta.modified = true;
           }

        }////////////////////////SECOND FETCH///////////////////////

        if(s->m_mouseIsMoved)
            s->m_history->addMoveHistory(historySourceBuffer, historyBuffer);

        s->m_mouseIsMoved = false;
    }
}

void WLD_ModeSelect::keyPress(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent);
}

void WLD_ModeSelect::keyRelease(QKeyEvent *keyEvent)
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    switch(keyEvent->key())
    {
        case (Qt::Key_Delete): //Delete action
            s->removeSelectedWldItems();
            break;
        case (Qt::Key_Escape):
            if(!s->m_mouseIsMoved)
                s->clearSelection();
            break;
        default:
            break;
    }
}



void WLD_ModeSelect::setItemSourceData(QGraphicsItem * it, QString ObjType)
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    gridSize = s->m_configs->default_grid;
    offsetX = 0;
    offsetY = 0;

    if( ObjType == "TILE" ||
        ObjType == "SCENERY" ||
        ObjType == "PATH" ||
        ObjType == "LEVEL" ||
        ObjType == "MUSICBOX" )
    {
        sourcePos = ((WldBaseItem *)it)->sourcePos();
        gridSize =  ((WldBaseItem *)it)->getGridSize();
        QPoint p =  ((WldBaseItem *)it)->gridOffset();
        offsetX = p.x();
        offsetY = p.y();
    }
}
