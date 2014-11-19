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

#include "wld_mode_select.h"
#include "../wld_scene.h"
#include "../../common_features/mainwinconnect.h"
#include "../../common_features/item_rectangles.h"

#include "../item_tile.h"
#include "../item_scene.h"
#include "../item_path.h"
#include "../item_level.h"
#include "../item_music.h"

#include "../../common_features/themes.h"

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

    s->EraserEnabled=false;
    s->PasteFromBuffer=false;
    s->DrawMode=false;
    s->disableMoveItems=false;

    s->resetCursor();
    s->resetResizers();
    s->unserPointSelector();

    s->_viewPort->setInteractive(true);
    s->_viewPort->setCursor(Themes::Cursor(Themes::cursor_normal));
    s->_viewPort->setDragMode(QGraphicsView::RubberBandDrag);
}


void WLD_ModeSelect::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    if(s->EditingMode == WldScene::MODE_PasteFromClip)
    {
        if( mouseEvent->buttons() & Qt::RightButton )
        {
            MainWinConnect::pMainWin->on_actionSelect_triggered();
            dontCallEvent = true;
            return;
        }
        s->PasteFromBuffer = true;
        dontCallEvent = true;
        return;
    }

    if((s->disableMoveItems) && (mouseEvent->buttons() & Qt::LeftButton)
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
                s->PasteFromBuffer=true;
            }
        }
    }
}


void WLD_ModeSelect::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    WldScene *s = dynamic_cast<WldScene *>(scene);

    if(!( mouseEvent->buttons() & Qt::LeftButton )) return;
    if(s->cursor) s->cursor->setPos(mouseEvent->scenePos());
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

    s->cursor->hide();

    s->haveSelected = false;

    QString ObjType;
    bool collisionPassed = false;

    //History
    WorldData historyBuffer;// bool deleted=false;
    WorldData historySourceBuffer;

    if(s->PasteFromBuffer)
    {
        s->paste( s->WldBuffer, mouseEvent->scenePos().toPoint() );
        s->Debugger_updateItemList();
        s->PasteFromBuffer = false;
        s->mouseMoved=false;
        MainWinConnect::pMainWin->on_actionSelect_triggered();
    }

    QList<QGraphicsItem*> selectedList = s->selectedItems();

    // check for grid snap
    if ((!selectedList.isEmpty())&&(s->mouseMoved))
    {

        //Set Grid Size/Offset, sourcePosition
        setItemSourceData(selectedList.first(), selectedList.first()->data(0).toString());
        //Check first selected element is it was moved
        if( (sourcePos == QPoint(
                 (long)(selectedList.first()->scenePos().x()),
                 ((long)selectedList.first()->scenePos().y())
                 ) ) )
        {
            s->mouseMoved=false;
            return; //break fetch when items is not moved
        }


        s->applyGroupGrid(selectedList);

        // Check collisions
        //Only if collision ckecking enabled
        if(!s->PasteFromBuffer)
        {
            if(s->opts.collisionsEnabled && s->checkGroupCollisions(&selectedList))
            {
                collisionPassed = false;
                s->returnItemBackGroup(selectedList);
            }
            else
            {
                collisionPassed = true;
                //applyArrayForItemGroup(selectedList);
                s->WldData->modified=true;
            }
        }


        if((collisionPassed) || (!s->opts.collisionsEnabled))
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
               s->mouseMoved=false;
               break; //break fetch when items is not moved
           }

           if( ObjType == "TILE")
           {
               //Applay move into main array
               historySourceBuffer.tiles.push_back(((ItemTile *)(*it))->tileData);
               ((ItemTile *)(*it))->tileData.x = (long)(*it)->scenePos().x();
               ((ItemTile *)(*it))->tileData.y = (long)(*it)->scenePos().y();
               ((ItemTile *)(*it))->arrayApply();
               historyBuffer.tiles.push_back(((ItemTile *)(*it))->tileData);
               s->WldData->modified = true;
           }
           else
           if( ObjType == "SCENERY")
           {
               //Applay move into main array
               historySourceBuffer.scenery.push_back(((ItemScene *)(*it))->sceneData);
               ((ItemScene *)(*it))->sceneData.x = (long)(*it)->scenePos().x();
               ((ItemScene *)(*it))->sceneData.y = (long)(*it)->scenePos().y();
               ((ItemScene *)(*it))->arrayApply();
               historyBuffer.scenery.push_back(((ItemScene *)(*it))->sceneData);
               s->WldData->modified = true;
           }
           else
           if( ObjType == "PATH")
           {
               //Applay move into main array
               historySourceBuffer.paths.push_back(((ItemPath*)(*it))->pathData);
               ((ItemPath *)(*it))->pathData.x = (long)(*it)->scenePos().x();
               ((ItemPath *)(*it))->pathData.y = (long)(*it)->scenePos().y();
               ((ItemPath *)(*it))->arrayApply();
               historyBuffer.paths.push_back(((ItemPath *)(*it))->pathData);
               s->WldData->modified = true;
           }
           else
           if( ObjType == "LEVEL")
           {
               //Applay move into main array
               historySourceBuffer.levels.push_back(((ItemLevel *)(*it))->levelData);
               ((ItemLevel *)(*it))->levelData.x = (long)(*it)->scenePos().x();
               ((ItemLevel *)(*it))->levelData.y = (long)(*it)->scenePos().y();
               ((ItemLevel *)(*it))->arrayApply();
               historyBuffer.levels.push_back(((ItemLevel *)(*it))->levelData);
               s->WldData->modified = true;
           }
           else
           if( ObjType == "MUSICBOX")
           {
               //Applay move into main array
               historySourceBuffer.music.push_back(((ItemMusic *)(*it))->musicData);
               ((ItemMusic *)(*it))->musicData.x = (long)(*it)->scenePos().x();
               ((ItemMusic *)(*it))->musicData.y = (long)(*it)->scenePos().y();
               ((ItemMusic *)(*it))->arrayApply();
               historyBuffer.music.push_back(((ItemMusic *)(*it))->musicData);
               s->WldData->modified = true;
           }

        }////////////////////////SECOND FETCH///////////////////////

        if(s->mouseMoved)
            s->addMoveHistory(historySourceBuffer, historyBuffer);

        s->mouseMoved = false;
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
            if(!s->mouseMoved)
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

    gridSize = s->pConfigs->default_grid;
    offsetX = 0;
    offsetY = 0;

    if( ObjType == "TILE")
    {
        sourcePos = QPoint(  ((ItemTile *)it)->tileData.x, ((ItemTile *)it)->tileData.y);
        gridSize = ((ItemTile *)it)->gridSize;
        offsetX = 0;
        offsetY = 0;
    }
    else
    if( ObjType == "SCENERY")
    {
        sourcePos = QPoint(  ((ItemScene *)it)->sceneData.x, ((ItemScene *)it)->sceneData.y);
        gridSize = ((ItemScene *)it)->gridSize;
        offsetX = 0;
        offsetY = 0;
    }
    else
    if( ObjType == "PATH")
    {
        sourcePos = QPoint(  ((ItemPath *)it)->pathData.x, ((ItemPath *)it)->pathData.y);
        gridSize = ((ItemPath *)it)->gridSize;
        offsetX = 0;
        offsetY = 0;
    }
    else
    if( ObjType == "LEVEL")
    {
        sourcePos = QPoint(  ((ItemLevel *)it)->levelData.x, ((ItemLevel*)it)->levelData.y);
        gridSize = ((ItemLevel *)it)->gridSize;
        offsetX = 0;
        offsetY = 0;
    }
    else
    if( ObjType == "MUSICBOX")
    {
        sourcePos = QPoint(  ((ItemMusic *)it)->musicData.x, ((ItemMusic*)it)->musicData.y);
        gridSize = ((ItemMusic *)it)->gridSize;
        offsetX = 0;
        offsetY = 0;
    }
}
