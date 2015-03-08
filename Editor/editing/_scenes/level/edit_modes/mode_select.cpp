/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <common_features/mainwinconnect.h>
#include <common_features/item_rectangles.h>

#include "mode_select.h"
#include "../lvl_scene.h"
#include "../items/item_bgo.h"
#include "../items/item_block.h"
#include "../items/item_npc.h"
#include "../items/item_water.h"
#include "../items/item_playerpoint.h"
#include "../items/item_door.h"

LVL_ModeSelect::LVL_ModeSelect(QGraphicsScene *parentScene, QObject *parent)
    : EditMode("Select", parentScene, parent)
{
    sourcePos=QPoint(0,0);
    gridSize=0;
    offsetX=0;
    offsetY=0;//, gridX, gridY, i=0;
}

LVL_ModeSelect::~LVL_ModeSelect()
{}


void LVL_ModeSelect::set()
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    s->resetCursor();
    s->resetResizers();

    s->EraserEnabled=false;
    s->PasteFromBuffer=false;
    s->DrawMode=false;
    s->disableMoveItems=false;

    s->_viewPort->setInteractive(true);
    s->_viewPort->setCursor(Themes::Cursor(Themes::cursor_normal));
    s->_viewPort->setDragMode(QGraphicsView::RubberBandDrag);
}


void LVL_ModeSelect::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    if(s->EditingMode == LvlScene::MODE_PasteFromClip)
    {
        if( mouseEvent->buttons() & Qt::RightButton )
        {
            MainWinConnect::pMainWin->on_actionSelect_triggered();
            dontCallEvent = true;
            s->IsMoved = true;
            return;
        }
        s->PasteFromBuffer = true;
        dontCallEvent = true;
        s->IsMoved = true;
        return;
    }

    if((s->disableMoveItems) && (mouseEvent->buttons() & Qt::LeftButton)
        && (Qt::ControlModifier != QApplication::keyboardModifiers()))
    { dontCallEvent = true; return; }

    if( mouseEvent->buttons() & Qt::MiddleButton )
    {
        if(GlobalSettings::MidMouse_allowSwitchToPlace)
        {
            if(s->selectedItems().size()==1)
            {
                QGraphicsItem * it = s->selectedItems().first();
                QString itp = it->data(ITEM_TYPE).toString();
                long itd = it->data(ITEM_ID).toInt();
                if(itp=="Block")
                {MainWinConnect::pMainWin->SwitchPlacingItem(ItemTypes::LVL_Block, itd); return;}
                else if(itp=="BGO")
                {MainWinConnect::pMainWin->SwitchPlacingItem(ItemTypes::LVL_BGO, itd); return;}
                else if(itp=="NPC")
                {MainWinConnect::pMainWin->SwitchPlacingItem(ItemTypes::LVL_NPC, itd); return;}
            }
        }
        if(GlobalSettings::MidMouse_allowDuplicate)
        {
            if(!s->selectedItems().isEmpty())
            {
                s->LvlBuffer=s->copy();
                s->PasteFromBuffer=true;
            }
        }
    }
}


void LVL_ModeSelect::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    if(!( mouseEvent->buttons() & Qt::LeftButton )) return;
    if(s->cursor) s->cursor->setPos(mouseEvent->scenePos());
}

void LVL_ModeSelect::mouseRelease(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

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
    LevelData historyBuffer;
    LevelData historySourceBuffer;

    if(s->PasteFromBuffer)
    {
        s->paste( s->LvlBuffer, mouseEvent->scenePos().toPoint() );
        s->PasteFromBuffer = false;
        s->IsMoved=false;
        MainWinConnect::pMainWin->on_actionSelect_triggered();
        s->Debugger_updateItemList();
    }

    QList<QGraphicsItem*> selectedList = s->selectedItems();

    // check for grid snap
    if ((!selectedList.isEmpty())&&(s->mouseMoved))
    {
        //Set Grid Size/Offset, sourcePosition
        setItemSourceData(selectedList.first(), selectedList.first()->data(ITEM_TYPE).toString());
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
                s->LvlData->modified=true;
            }
        }


        if((collisionPassed) || (!s->opts.collisionsEnabled))
        for (QList<QGraphicsItem*>::iterator it = selectedList.begin();
             it != selectedList.end(); it++)
        { ////////////////////////SECOND FETCH///////////////////////
           ObjType = (*it)->data(ITEM_TYPE).toString();

           /////////////////////////GET DATA///////////////
           setItemSourceData((*it), (*it)->data(ITEM_TYPE).toString()); //Set Grid Size/Offset, sourcePosition
           /////////////////////////GET DATA/////////////////////

           //Check position
           if( (sourcePos == QPoint((long)((*it)->scenePos().x()), ((long)(*it)->scenePos().y()))))
           {
               s->mouseMoved=false;
               break; //break fetch when items is not moved
           }

           if( ObjType == "Block")
            {
                //WriteToLog(QtDebugMsg, QString(" >>Collision passed"));
                //Applay move into main array
                historySourceBuffer.blocks.push_back(dynamic_cast<ItemBlock *>(*it)->blockData);
                //dynamic_cast<ItemBlock *>(*it)->blockData.x = (long)(*it)->scenePos().x();
                //dynamic_cast<ItemBlock *>(*it)->blockData.y = (long)(*it)->scenePos().y();
                dynamic_cast<ItemBlock *>(*it)->arrayApply();
                historyBuffer.blocks.push_back(dynamic_cast<ItemBlock *>(*it)->blockData);
                s->LvlData->modified = true;
            }
            else
            if( ObjType == "BGO")
            {
                //Applay move into main array
                historySourceBuffer.bgo.push_back(dynamic_cast<ItemBGO *>(*it)->bgoData);
                //dynamic_cast<ItemBGO *>(*it)->bgoData.x = (long)(*it)->scenePos().x();
                //dynamic_cast<ItemBGO *>(*it)->bgoData.y = (long)(*it)->scenePos().y();
                dynamic_cast<ItemBGO *>(*it)->arrayApply();
                historyBuffer.bgo.push_back(dynamic_cast<ItemBGO *>(*it)->bgoData);
                s->LvlData->modified = true;
            }
            else
            if( ObjType == "NPC")
            {
                //Applay move into main array
                historySourceBuffer.npc.push_back(dynamic_cast<ItemNPC *>(*it)->npcData);
                //dynamic_cast<ItemNPC *>(*it)->npcData.x = (long)(*it)->scenePos().x();
                //dynamic_cast<ItemNPC *>(*it)->npcData.y = (long)(*it)->scenePos().y();
                dynamic_cast<ItemNPC *>(*it)->arrayApply();
                historyBuffer.npc.push_back(dynamic_cast<ItemNPC *>(*it)->npcData);
                s->LvlData->modified = true;
            }
            else
            if( ObjType == "Water")
            {
                //Applay move into main array
                historySourceBuffer.physez.push_back(dynamic_cast<ItemWater *>(*it)->waterData);
                //dynamic_cast<ItemWater *>(*it)->waterData.x = (long)(*it)->scenePos().x();
                //dynamic_cast<ItemWater *>(*it)->waterData.y = (long)(*it)->scenePos().y();
                dynamic_cast<ItemWater *>(*it)->arrayApply();
                historyBuffer.physez.push_back(dynamic_cast<ItemWater *>(*it)->waterData);
                s->LvlData->modified = true;
            }
            else
            if( ObjType == "Door_enter")
            {
                //Applay move into main array
                //historySourceBuffer.water.push_back(dynamic_cast<ItemWater *>(*it)->waterData);
                LevelDoor oldDoorData = dynamic_cast<ItemDoor *>(*it)->doorData;
                oldDoorData.isSetIn = true;
                oldDoorData.isSetOut = false;
                historySourceBuffer.doors.push_back(oldDoorData);
                dynamic_cast<ItemDoor *>(*it)->doorData.ix = (long)(*it)->scenePos().x();
                dynamic_cast<ItemDoor *>(*it)->doorData.iy = (long)(*it)->scenePos().y();
                if(( dynamic_cast<ItemDoor *>(*it)->doorData.lvl_i)||
                     dynamic_cast<ItemDoor *>(*it)->doorData.lvl_o)
                {
                    dynamic_cast<ItemDoor *>(*it)->doorData.ox = (long)(*it)->scenePos().x();
                    dynamic_cast<ItemDoor *>(*it)->doorData.oy = (long)(*it)->scenePos().y();
                }

                dynamic_cast<ItemDoor *>(*it)->arrayApply();
                LevelDoor newDoorData = dynamic_cast<ItemDoor *>(*it)->doorData;
                newDoorData.isSetIn = true;
                newDoorData.isSetOut = false;
                historyBuffer.doors.push_back(newDoorData);
                //historyBuffer.water.push_back(dynamic_cast<ItemWater *>(*it)->waterData);
                s->LvlData->modified = true;
            }
            else
            if( ObjType == "Door_exit")
            {
                //Applay move into main array
                //historySourceBuffer.water.push_back(dynamic_cast<ItemWater *>(*it)->waterData);
                LevelDoor oldDoorData = dynamic_cast<ItemDoor *>(*it)->doorData;
                oldDoorData.isSetIn = false;
                oldDoorData.isSetOut = true;
                historySourceBuffer.doors.push_back(oldDoorData);
                dynamic_cast<ItemDoor *>(*it)->doorData.ox = (long)(*it)->scenePos().x();
                dynamic_cast<ItemDoor *>(*it)->doorData.oy = (long)(*it)->scenePos().y();
                if(( dynamic_cast<ItemDoor *>(*it)->doorData.lvl_i)||
                     dynamic_cast<ItemDoor *>(*it)->doorData.lvl_o)
                {
                    dynamic_cast<ItemDoor *>(*it)->doorData.ix = (long)(*it)->scenePos().x();
                    dynamic_cast<ItemDoor *>(*it)->doorData.iy = (long)(*it)->scenePos().y();
                }
                dynamic_cast<ItemDoor *>(*it)->arrayApply();
                LevelDoor newDoorData = dynamic_cast<ItemDoor *>(*it)->doorData;
                newDoorData.isSetIn = false;
                newDoorData.isSetOut = true;
                historyBuffer.doors.push_back(newDoorData);
                //historyBuffer.water.push_back(dynamic_cast<ItemWater *>(*it)->waterData);
                s->LvlData->modified = true;
            }
            else
            if(ObjType == "playerPoint" )
            {
                 historySourceBuffer.players.push_back(dynamic_cast<ItemPlayerPoint *>(*it)->pointData);
                 //dynamic_cast<ItemPlayerPoint *>(*it)->pointData.x =(long)(*it)->scenePos().x();
                 //dynamic_cast<ItemPlayerPoint *>(*it)->pointData.y =(long)(*it)->scenePos().y();
                 dynamic_cast<ItemPlayerPoint *>(*it)->arrayApply();
                 historyBuffer.players.push_back(dynamic_cast<ItemPlayerPoint *>(*it)->pointData);
            }
        }////////////////////////SECOND FETCH///////////////////////

        if(s->mouseMoved)
            s->addMoveHistory(historySourceBuffer, historyBuffer);

        s->mouseMoved = false;
    }
}

void LVL_ModeSelect::keyPress(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent);
}

void LVL_ModeSelect::keyRelease(QKeyEvent *keyEvent)
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    switch(keyEvent->key())
    {
        case (Qt::Key_Delete): //Delete action
            s->removeSelectedLvlItems();
            break;
        case (Qt::Key_Escape):
            if(!s->mouseMoved)
                s->clearSelection();
            break;
        default:
            break;
    }
}



void LVL_ModeSelect::setItemSourceData(QGraphicsItem * it, QString ObjType)
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    gridSize = s->pConfigs->default_grid;
    offsetX = 0;
    offsetY = 0;

    if( ObjType == "NPC")
    {
        sourcePos = QPoint(  dynamic_cast<ItemNPC *>(it)->npcData.x, dynamic_cast<ItemNPC *>(it)->npcData.y);
        gridSize = dynamic_cast<ItemNPC *>(it)->gridSize;
        offsetX = dynamic_cast<ItemNPC *>(it)->localProps.grid_offset_x;
        offsetY = dynamic_cast<ItemNPC *>(it)->localProps.grid_offset_y;
    }
    else
    if( ObjType == "Block")
    {
        sourcePos = QPoint(  dynamic_cast<ItemBlock *>(it)->blockData.x, dynamic_cast<ItemBlock *>(it)->blockData.y);
        gridSize = dynamic_cast<ItemBlock *>(it)->gridSize;
        //WriteToLog(QtDebugMsg, QString(" >>Check collision for Block"));
    }
    else
    if( ObjType == "BGO")
    {
        sourcePos = QPoint(  dynamic_cast<ItemBGO *>(it)->bgoData.x, dynamic_cast<ItemBGO *>(it)->bgoData.y);
        gridSize = dynamic_cast<ItemBGO *>(it)->gridSize;
        offsetX = dynamic_cast<ItemBGO *>(it)->gridOffsetX;
        offsetY = dynamic_cast<ItemBGO *>(it)->gridOffsetY;
    }
    else
    if( ObjType == "Water")
    {
        sourcePos = QPoint(  dynamic_cast<ItemWater *>(it)->waterData.x, dynamic_cast<ItemWater *>(it)->waterData.y);
        gridSize = qRound(qreal(s->pConfigs->default_grid)/2);
    }
    else
    if( ObjType == "Door_enter")
    {
        sourcePos = QPoint(  dynamic_cast<ItemDoor *>(it)->doorData.ix, dynamic_cast<ItemDoor *>(it)->doorData.iy);
        gridSize = qRound(qreal(s->pConfigs->default_grid)/2);
    }
    else
    if( ObjType == "Door_exit"){
        sourcePos = QPoint(  dynamic_cast<ItemDoor *>(it)->doorData.ox, dynamic_cast<ItemDoor *>(it)->doorData.oy);
        gridSize = qRound(qreal(s->pConfigs->default_grid)/2);
    }
    else
    if( ObjType == "playerPoint" )
    {
        gridSize = 2 ;
        sourcePos = QPoint(dynamic_cast<ItemPlayerPoint *>(it)->pointData.x, dynamic_cast<ItemPlayerPoint *>(it)->pointData.y);
    }
}
