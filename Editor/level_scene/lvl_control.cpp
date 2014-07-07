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

#include "lvlscene.h"
#include "../edit_level/leveledit.h"
#include <QtMath>

#include "item_block.h"
#include "item_bgo.h"
#include "item_npc.h"
#include "item_water.h"
#include "item_door.h"

#include "../common_features/mainwinconnect.h"
#include "../common_features/grid.h"
#include "lvl_item_placing.h"

#include "../file_formats/file_formats.h"


QPoint sourcePos=QPoint(0,0);
int gridSize=0, offsetX=0, offsetY=0;//, gridX, gridY, i=0;


void LvlScene::keyReleaseEvent ( QKeyEvent * keyEvent )
{
    QList<QGraphicsItem*> selectedList = selectedItems();
    LevelData historyBuffer;
    bool deleted=false;

    QString objType;
    switch(keyEvent->key())
    {
    case (Qt::Key_Delete): //Delete action
        if(selectedList.isEmpty()) break;

        for (QList<QGraphicsItem*>::iterator it = selectedList.begin(); it != selectedList.end(); it++)
        {

                objType=(*it)->data(0).toString();

                if(!(*it)->isVisible()) continue;  //Invisible items can't be deleted

                //remove data from main array before deletion item from scene
                if( objType=="Block" )
                {
                    historyBuffer.blocks.push_back(((ItemBlock*)(*it))->blockData);
                    ((ItemBlock *)(*it))->removeFromArray();
                    if((*it)) delete (*it);
                    deleted=true;
                }
                else
                if( objType=="BGO" )
                {
                    historyBuffer.bgo.push_back(((ItemBGO*)(*it))->bgoData);
                    ((ItemBGO *)(*it))->removeFromArray();
                    if((*it)) delete (*it);
                    deleted=true;
                }
                else
                if( objType=="NPC" )
                {
                    historyBuffer.npc.push_back(((ItemNPC*)(*it))->npcData);
                    ((ItemNPC *)(*it))->removeFromArray();
                    if((*it)) delete (*it);
                    deleted=true;
                }
                else
                if( objType=="Water" )
                {
                    historyBuffer.water.push_back(((ItemWater*)(*it))->waterData);
                    ((ItemWater *)(*it))->removeFromArray();
                    if((*it)) delete (*it);
                    deleted=true;
                }
                else
                if(( objType=="Door_enter" )||( objType=="Door_exit" ))
                {
                    //historyBuffer.water.push_back(((ItemWater*)(*it))->waterData);
                    bool isEntrance = (objType=="Door_enter");
                    /*addRemoveDoorHistory((*it)->data(2).toInt(), objType=="Door_enter",
                                         (isEntrance ? ((ItemDoor *)(*it))->doorData.ix :
                                                       ((ItemDoor *)(*it))->doorData.ox),
                                         (isEntrance ? ((ItemDoor *)(*it))->doorData.iy :
                                                       ((ItemDoor *)(*it))->doorData.oy));*/
                    LevelDoors doorData = ((ItemDoor *)(*it))->doorData;
                    if(isEntrance){
                        doorData.isSetIn = true;
                        doorData.isSetOut = false;
                    }else{
                        doorData.isSetIn = false;
                        doorData.isSetOut = true;
                    }
                    historyBuffer.doors.push_back(doorData);
                    ((ItemDoor *)(*it))->removeFromArray();
                    if((*it)) delete (*it);
                    MainWinConnect::pMainWin->setDoorData(-2);
                    deleted=true;
                }
                else
                if(( objType=="player1" )||( objType=="player2" ))
                {
                    unsigned long player=1;

                    if(objType=="player1")
                        player=1;
                    if(objType=="player2")
                        player=2;

                    for(int plr=0; plr<LvlData->players.size(); plr++)
                    {
                     if(LvlData->players[plr].id == player)
                     {
                         historyBuffer.players.push_back(LvlData->players[plr]);
                         LvlData->players[plr].x = 0;
                         LvlData->players[plr].y = 0;
                         LvlData->players[plr].w = 0;
                         LvlData->players[plr].h = 0;
                         deleted=true;
                         if((*it)) delete (*it);
                         break;
                     }
                    }
                }
        }
        if(deleted) addRemoveHistory(historyBuffer);

        break;
    case (Qt::Key_Escape):
        if(!IsMoved)
            this->clearSelection();
        if(pResizer!=NULL )
        {
            switch(pResizer->type)
            {
            case 2:
                setBlockResizer(NULL, false, false);
                break;
            case 1:
                setEventSctSizeResizer(-1, false, false);
                break;
            case 0:
            default:
                MainWinConnect::pMainWin->on_cancelResize_clicked();
            }
        }
            //setSectionResizer(false, false);
        break;
    case (Qt::Key_Enter):
    case (Qt::Key_Return):

        if(pResizer!=NULL )
        {
            switch(pResizer->type)
            {
            case 2:
                setBlockResizer(NULL, false, true);
                break;
            case 1:
                setEventSctSizeResizer(-1, false, true);
                break;
            case 0:
            default:
                MainWinConnect::pMainWin->on_applyResize_clicked();
            }
        }
            //setSectionResizer(false, true);
        break;

    default:
        break;
    }
    QGraphicsScene::keyReleaseEvent(keyEvent);
}

void LvlScene::openProps()
{
    QList<QGraphicsItem * > items = this->selectedItems();
    if(!items.isEmpty())
    {
        if(items.first()->data(0).toString()=="Block")
        {
            MainWinConnect::pMainWin->LvlItemProps(0,
                          ((ItemBlock *)items.first())->blockData,
                          FileFormats::dummyLvlBgo(),
                          FileFormats::dummyLvlNpc(), false);
        }
        else
        if(items.first()->data(0).toString()=="BGO")
        {
            MainWinConnect::pMainWin->LvlItemProps(1,
                              FileFormats::dummyLvlBlock(),
                              ((ItemBGO *)items.first())->bgoData,
                              FileFormats::dummyLvlNpc(), false);
        }
        else
        if(items.first()->data(0).toString()=="NPC")
        {
            MainWinConnect::pMainWin->LvlItemProps(2,
                              FileFormats::dummyLvlBlock(),
                              FileFormats::dummyLvlBgo(),
                              ((ItemNPC *)items.first())->npcData, false);
        }
        else
        MainWinConnect::pMainWin->LvlItemProps(-1,
                                               FileFormats::dummyLvlBlock(),
                                               FileFormats::dummyLvlBgo(),
                                               FileFormats::dummyLvlNpc());
    }
    else
    {
        MainWinConnect::pMainWin->LvlItemProps(-1,
                                               FileFormats::dummyLvlBlock(),
                                               FileFormats::dummyLvlBgo(),
                                               FileFormats::dummyLvlNpc());
    }

    QGraphicsScene::selectionChanged();
}

void LvlScene::selectionChanged()
{
    if(this->selectedItems().isEmpty())
    {
        LevelBlock dummyBlock;
        dummyBlock.array_id=0;

        LevelBGO dummyBgo;
        dummyBgo.array_id=0;

        LevelNPC dummyNPC;
        dummyNPC.array_id=0;

        MainWinConnect::pMainWin->LvlItemProps(-1, dummyBlock, dummyBgo, dummyNPC);
    }

    #ifdef _DEBUG_
    WriteToLog(QtDebugMsg, "Selection Changed!");
    #endif
}

void LvlScene::doorPointsSync(long arrayID, bool remove)
{
    bool doorExist=false;
    bool doorEntranceSynced=false;
    bool doorExitSynced=false;

    int i=0;
    //find doorItem in array
    for(i=0; i<LvlData->doors.size(); i++)
    {
        if(LvlData->doors[i].array_id==(unsigned int)arrayID)
        {
            doorExist=true;
            break;
        }
    }
    if(!doorExist) return;

    //get ItemList
    QList<QGraphicsItem * > items = this->items();

    foreach(QGraphicsItem * item, items)
    {
        if((!LvlData->doors[i].isSetIn)&&(!LvlData->doors[i].isSetOut)) break; //Don't sync door points if not placed

        if((item->data(0).toString()=="Door_enter")&&(item->data(2).toInt()==arrayID))
        {
            if((! (((!LvlData->doors[i].lvl_o) && (!LvlData->doors[i].lvl_i)) ||
                   ((LvlData->doors[i].lvl_o) && (!LvlData->doors[i].lvl_i)))
                )||(remove))
            {
                ((ItemDoor *)item)->doorData = LvlData->doors[i];
                ((ItemDoor *)item)->removeFromArray();
                delete ((ItemDoor *)item);
                doorEntranceSynced = true;
            }
            else
            {
                LvlData->doors[i].isSetIn=true;
                ((ItemDoor *)item)->doorData = LvlData->doors[i];
                doorEntranceSynced = true;
            }
        }

        if((item->data(0).toString()=="Door_exit")&&(item->data(2).toInt()==arrayID))
        {
            if( (! (((!LvlData->doors[i].lvl_o) && (!LvlData->doors[i].lvl_i)) ||
                                      (LvlData->doors[i].lvl_i) ) )||(remove))
            {
                ((ItemDoor *)item)->doorData = LvlData->doors[i];
                ((ItemDoor *)item)->removeFromArray();
                delete ((ItemDoor *)item);
                doorExitSynced = true;
            }
            else
            {
                LvlData->doors[i].isSetOut=true;
                ((ItemDoor *)item)->doorData = LvlData->doors[i];
                doorExitSynced = true;
            }
        }
        if((doorEntranceSynced)&&(doorExitSynced)) break; // stop fetch, because door points was synced
    }


}

static QPointF drawStartPos = QPoint(0,0);

void LvlScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    #ifdef _DEBUG_
    WriteToLog(QtDebugMsg, QString("Mouse pressed -> [%1, %2] contextMenuOpened=%3, DrawMode=%4").arg(mouseEvent->scenePos().x()).arg(mouseEvent->scenePos().y())
               .arg(contextMenuOpened).arg(DrawMode));
    #endif

if(contextMenuOpened) return;

    switch(EditingMode)
    {
        case MODE_PlacingNew:
        {
            if( mouseEvent->buttons() & Qt::RightButton )
            {
                MainWinConnect::pMainWin->on_actionSelect_triggered();
                return;
            }

            if(cursor){
                cursor->setPos( QPointF(applyGrid( mouseEvent->scenePos().toPoint()-
                                                   QPoint(LvlPlacingItems::c_offset_x,
                                                          LvlPlacingItems::c_offset_y),
                                                   LvlPlacingItems::gridSz,
                                                   LvlPlacingItems::gridOffset)));
            }
            placeItemUnderCursor();
            QGraphicsScene::mousePressEvent(mouseEvent);
            return;
            break;
        }
        case MODE_DrawSquare:
        {
            if( mouseEvent->buttons() & Qt::RightButton )
            {
                MainWinConnect::pMainWin->on_actionSelect_triggered();
                return;
            }

            if(cursor){
                drawStartPos = QPointF(applyGrid( mouseEvent->scenePos().toPoint(),
                                                  LvlPlacingItems::gridSz,
                                                  LvlPlacingItems::gridOffset));
                cursor->setPos( drawStartPos );
                cursor->setVisible(true);

                QPoint hw = applyGrid( mouseEvent->scenePos().toPoint(),
                                       LvlPlacingItems::gridSz,
                                       LvlPlacingItems::gridOffset);

                QSize hs = QSize( (long)fabs(drawStartPos.x() - hw.x()),  (long)fabs( drawStartPos.y() - hw.y() ) );
                ((QGraphicsRectItem *)cursor)->setRect(0,0, hs.width(), hs.height());
            }

            QGraphicsScene::mousePressEvent(mouseEvent);
            return;
            break;

        }
        case MODE_Resizing:
        {
            QGraphicsScene::mousePressEvent(mouseEvent);
            return;
        }
        case MODE_Erasing:
        {
            if( mouseEvent->buttons() & Qt::RightButton )
            {
                MainWinConnect::pMainWin->on_actionSelect_triggered();
                return;
            }

            if(cursor){
               cursor->show();
               cursor->setPos(mouseEvent->scenePos());
            }

            QGraphicsScene::mousePressEvent(mouseEvent);

            QList<QGraphicsItem*> selectedList = selectedItems();
            if (!selectedList.isEmpty())
            {
                removeItemUnderCursor();
                EraserEnabled=true;
            }
            break;
        }
        case MODE_PasteFromClip: //Pasta
        {
            if( mouseEvent->buttons() & Qt::RightButton )
            {
                MainWinConnect::pMainWin->on_actionSelect_triggered();
                return;
            }
            PasteFromBuffer = true;
            break;
        }
        default:
        {
            if((disableMoveItems) && (mouseEvent->buttons() & Qt::LeftButton)
                && (Qt::ControlModifier != QApplication::keyboardModifiers()))
            { return; }

            QGraphicsScene::mousePressEvent(mouseEvent);

            break;
        }
    }
    haveSelected=(!selectedItems().isEmpty());

}


void LvlScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    //WriteToLog(QtDebugMsg, QString("Mouse moved -> [%1, %2]").arg(mouseEvent->scenePos().x()).arg(mouseEvent->scenePos().y()));
    if(contextMenuOpened) return;

    switch(EditingMode)
    {
    case MODE_PlacingNew:
        {
            this->clearSelection();
            if(cursor)
            {
                        cursor->setPos( QPointF(applyGrid( mouseEvent->scenePos().toPoint()-
                                                           QPoint(LvlPlacingItems::c_offset_x,
                                                                  LvlPlacingItems::c_offset_y),
                                                         LvlPlacingItems::gridSz,
                                                         LvlPlacingItems::gridOffset)));
                       cursor->show();
            }
            if( mouseEvent->buttons() & Qt::LeftButton ) placeItemUnderCursor();
            QGraphicsScene::mouseMoveEvent(mouseEvent);
            break;
        }
    case MODE_DrawSquare:
        {
            if(cursor)
            {
                if(cursor->isVisible())
                {
                QPoint hw = applyGrid( mouseEvent->scenePos().toPoint(),
                                       LvlPlacingItems::gridSz,
                                       LvlPlacingItems::gridOffset);

                QSize hs = QSize( (long)fabs(drawStartPos.x() - hw.x()),  (long)fabs( drawStartPos.y() - hw.y() ) );


                ((QGraphicsRectItem *)cursor)->setRect(0,0, hs.width(), hs.height());
                ((QGraphicsRectItem *)cursor)->setPos(
                            ((hw.x() < drawStartPos.x() )? hw.x() : drawStartPos.x()),
                            ((hw.y() < drawStartPos.y() )? hw.y() : drawStartPos.y())
                            );
                }
            }
        }
    case MODE_Resizing:
        {
            this->clearSelection();
            QGraphicsScene::mouseMoveEvent(mouseEvent);
            return;
        break;
        }
    case MODE_Erasing:
        {
            if(cursor) cursor->setPos(mouseEvent->scenePos());
            if (EraserEnabled)// Remove All items, placed under Cursor
                removeItemUnderCursor();
            break;
        }
    default:
        if(cursor) cursor->setPos(mouseEvent->scenePos());
        break;
    }

    haveSelected=(!selectedItems().isEmpty());
    if(haveSelected)
    {
        if(!IsMoved)
        {
            IsMoved = true;
        }
    }
    QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void LvlScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(contextMenuOpened)
    {
        contextMenuOpened = false; //bug protector
        QGraphicsScene::mouseReleaseEvent(mouseEvent);
        return;
    }

    switch(EditingMode)
    {
    case MODE_DrawSquare:
        {

        if(cursor)
        {

            // /////////// Don't draw with zero width or height //////////////
            if( (((QGraphicsRectItem *)cursor)->rect().width()==0) ||
              (((QGraphicsRectItem *)cursor)->rect().height()==0))
            {
                cursor->hide();
                break;
            }
            // ///////////////////////////////////////////////////////////////

            switch(placingItem)
            {
            case PLC_Water:
                {
                    LvlPlacingItems::waterSet.quicksand = (LvlPlacingItems::waterType==1);

                    LvlPlacingItems::waterSet.x = cursor->scenePos().x();
                    LvlPlacingItems::waterSet.y = cursor->scenePos().y();
                    LvlPlacingItems::waterSet.w = ((QGraphicsRectItem *)cursor)->rect().width();
                    LvlPlacingItems::waterSet.h = ((QGraphicsRectItem *)cursor)->rect().height();
                    //here define placing water item.
                    LvlData->water_array_id++;

                    LvlPlacingItems::waterSet.array_id = LvlData->water_array_id;
                    LvlData->water.push_back(LvlPlacingItems::waterSet);

                    placeWater(LvlPlacingItems::waterSet, true);
                    LevelData plWater;
                    plWater.water.push_back(LvlPlacingItems::waterSet);
                    addPlaceHistory(plWater);
                    break;
                }
            case PLC_Block:
                {
                    //LvlPlacingItems::waterSet.quicksand = (LvlPlacingItems::waterType==1);
                    if(LvlPlacingItems::sizableBlock)
                    {
                        LvlPlacingItems::blockSet.x = cursor->scenePos().x();
                        LvlPlacingItems::blockSet.y = cursor->scenePos().y();
                        LvlPlacingItems::blockSet.w = ((QGraphicsRectItem *)cursor)->rect().width();
                        LvlPlacingItems::blockSet.h = ((QGraphicsRectItem *)cursor)->rect().height();
                        //here define placing water item.
                        LvlData->blocks_array_id++;

                        LvlPlacingItems::blockSet.array_id = LvlData->blocks_array_id;
                        LvlData->blocks.push_back(LvlPlacingItems::blockSet);

                        placeBlock(LvlPlacingItems::blockSet, true);
                        LevelData plSzBlock;
                        plSzBlock.blocks.push_back(LvlPlacingItems::blockSet);
                        addPlaceHistory(plSzBlock);
                        break;
                    }
                    else
                    {
                        long x = cursor->scenePos().x();
                        long y = cursor->scenePos().y();
                        long width = ((QGraphicsRectItem *)cursor)->rect().width();
                        long height = ((QGraphicsRectItem *)cursor)->rect().height();
                        int repWidth = width/LvlPlacingItems::blockSet.w;
                        int repHeight = height/LvlPlacingItems::blockSet.h;

                        LevelData plSqBlock;
                        for(int i = 0; i < repWidth; i++){
                            for(int j = 0; j < repHeight; j++){
                                LvlPlacingItems::blockSet.x = x + i * LvlPlacingItems::blockSet.w;
                                LvlPlacingItems::blockSet.y = y + j * LvlPlacingItems::blockSet.h;

                                LvlData->blocks_array_id++;

                                LvlPlacingItems::blockSet.array_id = LvlData->blocks_array_id;

                                LvlData->blocks.push_back(LvlPlacingItems::blockSet);
                                placeBlock(LvlPlacingItems::blockSet, true);
                                plSqBlock.blocks.push_back(LvlPlacingItems::blockSet);
                            }
                        }
                        if(plSqBlock.blocks.size() > 0)
                        {
                            addPlaceHistory(plSqBlock);
                            //restart Animation
                            //if(opts.animationEnabled) stopAnimation();
                            //if(opts.animationEnabled) startBlockAnimation();

                        }
                    }
                    break;
                }
            case PLC_BGO:
                {
                    long x = cursor->scenePos().x();
                    long y = cursor->scenePos().y();
                    long width = ((QGraphicsRectItem *)cursor)->rect().width();
                    long height = ((QGraphicsRectItem *)cursor)->rect().height();
                    int repWidth = width/LvlPlacingItems::bgoW;
                    int repHeight = height/LvlPlacingItems::bgoH;

                    LevelData plSqBgo;
                    for(int i = 0; i < repWidth; i++){
                        for(int j = 0; j < repHeight; j++){
                            LvlPlacingItems::bgoSet.x = x + i * LvlPlacingItems::bgoW;
                            LvlPlacingItems::bgoSet.y = y + j * LvlPlacingItems::bgoH;

                            LvlData->bgo_array_id++;

                            LvlPlacingItems::bgoSet.array_id = LvlData->bgo_array_id;

                            LvlData->bgo.push_back(LvlPlacingItems::bgoSet);
                            placeBGO(LvlPlacingItems::bgoSet, true);
                            plSqBgo.bgo.push_back(LvlPlacingItems::bgoSet);
                        }
                    }
                    if(plSqBgo.bgo.size() > 0)
                    {
                        addPlaceHistory(plSqBgo);
                        //restart Animation
                    }
                }
            }

        cursor->hide();
        }
        break;
        }
    case MODE_PlacingNew:
    {
        if(placingItem == PLC_Door)
        {
            MainWinConnect::pMainWin->on_actionSelect_triggered();
            QGraphicsScene::mouseReleaseEvent(mouseEvent);
            return;
        }
    }
    default:
        break;
    }


    if(DrawMode)
    {
        QGraphicsScene::mouseReleaseEvent(mouseEvent);
        return;
    }

            cursor->hide();

            haveSelected = false;

            QString ObjType;
            int collisionPassed = false;

            //History
            LevelData historyBuffer; bool deleted=false;
            LevelData historySourceBuffer;

            if(PasteFromBuffer)
            {
                paste( LvlBuffer, mouseEvent->scenePos().toPoint() );
                PasteFromBuffer = false;
                IsMoved=false;
                MainWinConnect::pMainWin->on_actionSelect_triggered();
            }


            QList<QGraphicsItem*> deleteList;
            deleteList.clear();
            QList<QGraphicsItem*> selectedList = selectedItems();

            // check for grid snap
            if ((!selectedList.isEmpty())&&(IsMoved))
            {

                // correct selected items' coordinates
                for (QList<QGraphicsItem*>::iterator it = selectedList.begin(); it != selectedList.end(); it++)
                { ////////////////////////FIRST FETCH///////////////////////
                    if(EditingMode==MODE_Erasing)
                    {

                        if(!(*it)->isVisible()) continue; //Invisible items can't be deleted

                        //remove data from main array before deletion item from scene
                        if( (*it)->data(0).toString()=="Block" )
                        {
                            historyBuffer.blocks.push_back(((ItemBlock*)(*it))->blockData);
                            ((ItemBlock *)(*it))->removeFromArray();
                            deleted=true;
                        }
                        else
                        if( (*it)->data(0).toString()=="BGO" )
                        {
                            historyBuffer.bgo.push_back(((ItemBGO*)(*it))->bgoData);
                            ((ItemBGO *)(*it))->removeFromArray();
                            deleted=true;
                        }
                        else
                        if( (*it)->data(0).toString()=="NPC" )
                        {
                            historyBuffer.npc.push_back(((ItemNPC*)(*it))->npcData);
                            ((ItemNPC *)(*it))->removeFromArray();
                            deleted=true;
                        }
                        else
                        if( (*it)->data(0).toString()=="Water" )
                        {
                            historyBuffer.water.push_back(((ItemWater*)(*it))->waterData);
                            ((ItemWater *)(*it))->removeFromArray();
                            deleted=true;
                        }
                        else
                        if(( (*it)->data(0).toString()=="Door_enter" )||( (*it)->data(0).toString()=="Door_exit" ))
                        {
                            //historyBuffer.water.push_back(((ItemWater*)(*it))->waterData);
                            LevelDoors tData = ((ItemDoor*)(*it))->doorData;
                            tData.isSetIn = ( (*it)->data(0).toString()=="Door_enter" );
                            tData.isSetOut = ( (*it)->data(0).toString()=="Door_exit" );
                            historyBuffer.doors.push_back(tData);
                            ((ItemDoor *)(*it))->removeFromArray();
                            deleted=true;
                            MainWinConnect::pMainWin->setDoorData(-2);
                        }
                        else
                        if(( (*it)->data(0).toString()=="player1" )||( (*it)->data(0).toString()=="player2" ))
                        {
                            unsigned long player=1;

                            if((*it)->data(0).toString()=="player1")
                                player=1;
                            if((*it)->data(0).toString()=="player2")
                                player=2;

                            for(int plr=0; plr<LvlData->players.size(); plr++)
                            {
                             if(LvlData->players[plr].id == player)
                             {
                                 historyBuffer.players.push_back(LvlData->players[plr]);
                                 LvlData->players[plr].x = 0;
                                 LvlData->players[plr].y = 0;
                                 LvlData->players[plr].w = 0;
                                 LvlData->players[plr].h = 0;
                                 //    Uncomment this after add player point history
                                 deleted=true;
                                 break;
                             }
                            }
                        }
                        removeItem((*it));
                        deleteList.push_back((*it));
                        continue;
                    }

                    /////////////////////////GET DATA///////////////

                    setItemSourceData((*it), (*it)->data(0).toString()); //Set Grid Size/Offset, sourcePosition

                    /////////////////////////GET DATA/////////////////////

                    //Check position
                    if( (sourcePos == QPoint((long)((*it)->scenePos().x()), ((long)(*it)->scenePos().y()))))
                    {
                        ///SKIP NON-MOVED ITEMS
                        IsMoved=false;
                        #ifdef _DEBUG_
                        WriteToLog(QtDebugMsg, QString(" >>Collision skiped, posSource=posCurrent"));
                        #endif
                        continue;
                    }

                    ////////////////////Apply to GRID/////////////////////////////////
                    (*it)->setPos( QPointF(
                                       applyGrid( (*it)->scenePos().toPoint(),
                                                      gridSize,
                                                      QPoint(offsetX, offsetY)
                                                  )
                                           )
                                  );
                    //////////////////////////////////////////////////////////////////

                } ////////////////////////FIRST FETCH///////////////////////

                if(!deleteList.isEmpty())
                {
                    while(!deleteList.isEmpty())
                    {
                        QGraphicsItem* tmp = deleteList.first();
                        deleteList.pop_front();
                        if(tmp!=NULL) delete tmp;
                    }
                }
                selectedList = selectedItems();

                if((EditingMode==MODE_Erasing)&&(deleted))
                {
                    addRemoveHistory(historyBuffer);
                }
                EraserEnabled = false;

                // Check collisions
                //Only if collision ckecking enabled
                if(!PasteFromBuffer)
                for (QList<QGraphicsItem*>::iterator it = selectedList.begin(); it != selectedList.end(); it++)
                { ////////////////////////SECOND FETCH///////////////////////
                    ObjType = (*it)->data(0).toString();

                    #ifdef _DEBUG_
                    WriteToLog(QtDebugMsg, QString(" >>Check collision with \"%1\"").arg(ObjType));
                    #endif

                    setItemSourceData((*it), ObjType); //Set Grid Size/Offset, sourcePosition

                    //Check position
                    if( (sourcePos == QPoint((long)((*it)->scenePos().x()), ((long)(*it)->scenePos().y()))))
                    {
                        ///SKIP NON-MOVED ITEMS
                        IsMoved=false;
                        #ifdef _DEBUG_
                        WriteToLog(QtDebugMsg, QString(" >>Collision skiped, posSource=posCurrent"));
                        #endif
                        continue;
                    }

                    if(opts.collisionsEnabled)
                    { //check Available to collisions checking
                        if( itemCollidesWith((*it)) )
                        {
                            collisionPassed = false;
                            (*it)->setPos(QPointF(sourcePos));
                            (*it)->setSelected(false);
                            /*
                            WriteToLog(QtDebugMsg, QString("Moved back %1 %2")
                                       .arg((long)(*it)->scenePos().x())
                                       .arg((long)(*it)->scenePos().y()) );*/
                        }
                        else
                        {
                            collisionPassed = true;
                        }
                    }

                    if((collisionPassed) || (!opts.collisionsEnabled))
                    {
                        if( ObjType == "Block")
                        {
                            //WriteToLog(QtDebugMsg, QString(" >>Collision passed"));
                            //Applay move into main array
                            historySourceBuffer.blocks.push_back(((ItemBlock *)(*it))->blockData);
                            ((ItemBlock *)(*it))->blockData.x = (long)(*it)->scenePos().x();
                            ((ItemBlock *)(*it))->blockData.y = (long)(*it)->scenePos().y();
                            ((ItemBlock *)(*it))->arrayApply();
                            historyBuffer.blocks.push_back(((ItemBlock *)(*it))->blockData);
                            LvlData->modified = true;
                        }
                        else
                        if( ObjType == "BGO")
                        {
                            //Applay move into main array
                            historySourceBuffer.bgo.push_back(((ItemBGO *)(*it))->bgoData);
                            ((ItemBGO *)(*it))->bgoData.x = (long)(*it)->scenePos().x();
                            ((ItemBGO *)(*it))->bgoData.y = (long)(*it)->scenePos().y();
                            ((ItemBGO *)(*it))->arrayApply();
                            historyBuffer.bgo.push_back(((ItemBGO *)(*it))->bgoData);
                            LvlData->modified = true;
                        }
                        else
                        if( ObjType == "NPC")
                        {
                            //Applay move into main array
                            historySourceBuffer.npc.push_back(((ItemNPC *)(*it))->npcData);
                            ((ItemNPC *)(*it))->npcData.x = (long)(*it)->scenePos().x();
                            ((ItemNPC *)(*it))->npcData.y = (long)(*it)->scenePos().y();
                            ((ItemNPC *)(*it))->arrayApply();
                            historyBuffer.npc.push_back(((ItemNPC *)(*it))->npcData);
                            LvlData->modified = true;
                        }
                        else
                        if( ObjType == "Water")
                        {
                            //Applay move into main array
                            historySourceBuffer.water.push_back(((ItemWater *)(*it))->waterData);
                            ((ItemWater *)(*it))->waterData.x = (long)(*it)->scenePos().x();
                            ((ItemWater *)(*it))->waterData.y = (long)(*it)->scenePos().y();
                            ((ItemWater *)(*it))->arrayApply();
                            historyBuffer.water.push_back(((ItemWater *)(*it))->waterData);
                            LvlData->modified = true;
                        }
                        else
                        if( ObjType == "Door_enter")
                        {
                            //Applay move into main array
                            //historySourceBuffer.water.push_back(((ItemWater *)(*it))->waterData);
                            LevelDoors oldDoorData = ((ItemDoor *)(*it))->doorData;
                            oldDoorData.isSetIn = true;
                            oldDoorData.isSetOut = false;
                            historySourceBuffer.doors.push_back(oldDoorData);
                            ((ItemDoor *)(*it))->doorData.ix = (long)(*it)->scenePos().x();
                            ((ItemDoor *)(*it))->doorData.iy = (long)(*it)->scenePos().y();
                            if((((ItemDoor *)(*it))->doorData.lvl_i)||((ItemDoor *)(*it))->doorData.lvl_o)
                            {
                                ((ItemDoor *)(*it))->doorData.ox = (long)(*it)->scenePos().x();
                                ((ItemDoor *)(*it))->doorData.oy = (long)(*it)->scenePos().y();
                            }

                            ((ItemDoor *)(*it))->arrayApply();
                            LevelDoors newDoorData = ((ItemDoor *)(*it))->doorData;
                            newDoorData.isSetIn = true;
                            newDoorData.isSetOut = false;
                            historyBuffer.doors.push_back(newDoorData);
                            //historyBuffer.water.push_back(((ItemWater *)(*it))->waterData);
                            LvlData->modified = true;
                        }
                        else
                        if( ObjType == "Door_exit")
                        {
                            //Applay move into main array
                            //historySourceBuffer.water.push_back(((ItemWater *)(*it))->waterData);
                            LevelDoors oldDoorData = ((ItemDoor *)(*it))->doorData;
                            oldDoorData.isSetIn = false;
                            oldDoorData.isSetOut = true;
                            historySourceBuffer.doors.push_back(oldDoorData);
                            ((ItemDoor *)(*it))->doorData.ox = (long)(*it)->scenePos().x();
                            ((ItemDoor *)(*it))->doorData.oy = (long)(*it)->scenePos().y();
                            if((((ItemDoor *)(*it))->doorData.lvl_i)||((ItemDoor *)(*it))->doorData.lvl_o)
                            {
                                ((ItemDoor *)(*it))->doorData.ix = (long)(*it)->scenePos().x();
                                ((ItemDoor *)(*it))->doorData.iy = (long)(*it)->scenePos().y();
                            }
                            ((ItemDoor *)(*it))->arrayApply();
                            LevelDoors newDoorData = ((ItemDoor *)(*it))->doorData;
                            newDoorData.isSetIn = false;
                            newDoorData.isSetOut = true;
                            historyBuffer.doors.push_back(newDoorData);
                            //historyBuffer.water.push_back(((ItemWater *)(*it))->waterData);
                            LvlData->modified = true;
                        }
                        else
                        if(( ObjType == "player1" ) || ( ObjType == "player2" ))
                        {
                            int plrId=0;
                            if( ObjType == "player1" )
                                plrId=1;
                            if( ObjType == "player2" )
                                plrId=2;

                            for(int g=0; g<LvlData->players.size(); g++)
                            {
                             if(LvlData->players[g].id == (unsigned int)plrId)
                             {
                                 historySourceBuffer.players.push_back(LvlData->players[g]);
                                 LvlData->players[g].x = (long)(*it)->scenePos().x();
                                 LvlData->players[g].y = (long)(*it)->scenePos().y();
                                 historyBuffer.players.push_back(LvlData->players[g]);
                                 break;
                             }
                            }
                        }
                    }
                }////////////////////////SECOND FETCH///////////////////////

                if((EditingMode==MODE_Selecting)&&(IsMoved)) addMoveHistory(historySourceBuffer, historyBuffer);

                IsMoved = false;

                QGraphicsScene::mouseReleaseEvent(mouseEvent);
                return;
           }
     EraserEnabled = false;
     QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

void LvlScene::setItemSourceData(QGraphicsItem * it, QString ObjType)
{
    gridSize = pConfigs->default_grid;
    offsetX = 0;
    offsetY = 0;

    if( ObjType == "NPC")
    {
        sourcePos = QPoint(  ((ItemNPC *)it)->npcData.x, ((ItemNPC *)it)->npcData.y);
        gridSize = ((ItemNPC *)it)->gridSize;
        offsetX = ((ItemNPC *)it)->localProps.grid_offset_x;
        offsetY = ((ItemNPC *)it)->localProps.grid_offset_y;
    }
    else
    if( ObjType == "Block")
    {
        sourcePos = QPoint(  ((ItemBlock *)it)->blockData.x, ((ItemBlock *)it)->blockData.y);
        gridSize = ((ItemBlock *)it)->gridSize;
        //WriteToLog(QtDebugMsg, QString(" >>Check collision for Block"));
    }
    else
    if( ObjType == "BGO")
    {
        sourcePos = QPoint(  ((ItemBGO *)it)->bgoData.x, ((ItemBGO *)it)->bgoData.y);
        gridSize = ((ItemBGO *)it)->gridSize;
        offsetX = ((ItemBGO *)it)->gridOffsetX;
        offsetY = ((ItemBGO *)it)->gridOffsetY;
    }
    else
    if( ObjType == "Water")
    {
        sourcePos = QPoint(  ((ItemWater *)it)->waterData.x, ((ItemWater *)it)->waterData.y);
        gridSize = qRound(qreal(pConfigs->default_grid)/2);
    }
    else
    if( ObjType == "Door_enter")
    {
        sourcePos = QPoint(  ((ItemDoor *)it)->doorData.ix, ((ItemDoor *)it)->doorData.iy);
        gridSize = qRound(qreal(pConfigs->default_grid)/2);
    }
    else
    if( ObjType == "Door_exit"){
        sourcePos = QPoint(  ((ItemDoor *)it)->doorData.ox, ((ItemDoor *)it)->doorData.oy);
        gridSize = qRound(qreal(pConfigs->default_grid)/2);
    }
    else
    if(( ObjType == "player1" ) || ( ObjType == "player2" ))
    {
        offsetY = 2;
        gridSize = 2 ;
        int plrId=0;
        if( ObjType == "player1" )
            plrId=1;
        if( ObjType == "player2" )
            plrId=2;

        foreach(PlayerPoint pnt, LvlData->players)
        {
         if(pnt.id == (unsigned int)plrId)
         {
             sourcePos = QPoint(pnt.x, pnt.y);
             break;
         }
        }
    }
}


void LvlScene::placeItemUnderCursor()
{
    LevelData newData;
    bool wasPlaced=false;
    if( itemCollidesWith(cursor) )
    {
        return;
    }
    else
    {
        if(placingItem == PLC_Block)
        {
            LvlPlacingItems::blockSet.x = cursor->scenePos().x();
            LvlPlacingItems::blockSet.y = cursor->scenePos().y();

            LvlData->blocks_array_id++;
            LvlPlacingItems::blockSet.array_id = LvlData->blocks_array_id;

            LvlData->blocks.push_back(LvlPlacingItems::blockSet);
            placeBlock(LvlPlacingItems::blockSet, true);
            newData.blocks.push_back(LvlPlacingItems::blockSet);
            wasPlaced=true;
        }
        else
        if(placingItem == PLC_BGO)
        {
            LvlPlacingItems::bgoSet.x = cursor->scenePos().x();
            LvlPlacingItems::bgoSet.y = cursor->scenePos().y();

            LvlData->bgo_array_id++;
            LvlPlacingItems::bgoSet.array_id = LvlData->bgo_array_id;

            LvlData->bgo.push_back(LvlPlacingItems::bgoSet);
            placeBGO(LvlPlacingItems::bgoSet, true);
            newData.bgo.push_back(LvlPlacingItems::bgoSet);
            wasPlaced=true;
        }
        else
        if(placingItem == PLC_NPC)
        {
            LvlPlacingItems::npcSet.x = cursor->scenePos().x();
            LvlPlacingItems::npcSet.y = cursor->scenePos().y();

            LvlData->npc_array_id++;
            LvlPlacingItems::npcSet.array_id = LvlData->npc_array_id;

            LvlData->npc.push_back(LvlPlacingItems::npcSet);

            placeNPC(LvlPlacingItems::npcSet, true);

            newData.npc.push_back(LvlPlacingItems::npcSet);

            if(opts.animationEnabled) stopAnimation();
            if(opts.animationEnabled) startBlockAnimation();

            wasPlaced=true;
        }
        else
        if(placingItem == PLC_PlayerPoint)
        {
            foreach(PlayerPoint pnt, LvlData->players)
            {
             if(pnt.id == (unsigned int)LvlPlacingItems::playerID+1)
             {
                 QList<QVariant> oData;
                 oData.push_back(pnt.id);
                 oData.push_back((qlonglong)pnt.x);
                 oData.push_back((qlonglong)pnt.y);
                 oData.push_back((qlonglong)pnt.w);
                 oData.push_back((qlonglong)pnt.h);
                 pnt.x = cursor->scenePos().x();
                 pnt.y = cursor->scenePos().y();
                 pnt.w = 24;
                 if(LvlPlacingItems::playerID==0)
                    pnt.h = 54;
                 else
                    pnt.h = 60;
                 placePlayerPoint(pnt);

                 addPlacePlayerPointHistory(pnt, QVariant(oData));

                 break;
             }
            }

        }
        else
        if(placingItem == PLC_Door)
        {
            foreach(LevelDoors door, LvlData->doors)
            {
             if(door.array_id == (unsigned int)LvlPlacingItems::doorArrayId)
             {
                if(LvlPlacingItems::doorType==LvlPlacingItems::DOOR_Entrance)
                {
                    if(!door.isSetIn)
                    {
                        door.ix = cursor->scenePos().x();
                        door.iy = cursor->scenePos().y();
                        if((door.lvl_i)||(door.lvl_o))
                        {
                            door.ox = door.ix;
                            door.oy = door.iy;
                        }
                        door.isSetIn=true;
                        addPlaceDoorHistory(door.array_id, true, door.ix, door.iy);
                        placeDoorEnter(door, false, false);
                    }
                }
                else
                {
                    if(!door.isSetOut)
                    {
                        door.ox = cursor->scenePos().x();
                        door.oy = cursor->scenePos().y();
                        if((door.lvl_i)||(door.lvl_o))
                        {
                            door.ix = door.ox;
                            door.iy = door.oy;
                        }
                        door.isSetOut=true;
                        addPlaceDoorHistory(door.array_id, false, door.ox, door.oy);
                        placeDoorExit(door, false, false);
                    }
                }
                MainWinConnect::pMainWin->setDoorData(-2);
                break;
             }
            }

        }
    }
    if(wasPlaced)
    {
        LvlData->modified = true;
        addPlaceHistory(newData);
    }

    //if(opts.animationEnabled) stopAnimation();
    //if(opts.animationEnabled) startBlockAnimation();
}


void LvlScene::removeItemUnderCursor()
{
    if(contextMenuOpened) return;

    QGraphicsItem * findItem;
    bool removeIt=true;
    findItem = itemCollidesCursor(cursor);
    if(findItem)
    {
        if(findItem->data(0).toString()=="Block")
        {
            if((lock_block)|| (((ItemBlock *)findItem)->isLocked) )
            removeIt=false;
        }
        else
        if(findItem->data(0).toString()=="BGO")
        {
            if( (lock_bgo) || ((((ItemBGO *)findItem)->isLocked)) )
            removeIt=false;
        }
        else
        if(findItem->data(0).toString()=="NPC")
        {
            if( (lock_npc) || ((((ItemNPC *)findItem)->isLocked)) )
            removeIt=false;
        }
        else
        if(findItem->data(0).toString()=="Water")
        {
            if( (lock_water) || ((((ItemWater *)findItem)->isLocked)) )
            removeIt=false;
        }
        else
        if(((findItem->data(0).toString()=="Door_enter")||(findItem->data(0).toString()=="Door_exit"))&&
                (lock_door))
            removeIt=false;

        if(!findItem->isVisible()) //Invisible items can't be deleted
            removeIt=false;

        if(removeIt)
        {
            LevelData removedItems;
            bool deleted=false;
            //remove data from main array before deletion item from scene
            if( findItem->data(0).toString()=="Block" )
            {
                removedItems.blocks.push_back(((ItemBlock *)findItem)->blockData);
                ((ItemBlock *)findItem)->removeFromArray();
                deleted=true;
            }
            else
            if( findItem->data(0).toString()=="BGO" )
            {
                removedItems.bgo.push_back(((ItemBGO *)findItem)->bgoData);
                ((ItemBGO *)findItem)->removeFromArray();
                deleted=true;
            }
            else
            if( findItem->data(0).toString()=="NPC" )
            {
                removedItems.npc.push_back(((ItemNPC *)findItem)->npcData);
                ((ItemNPC *)findItem)->removeFromArray();
                deleted=true;
            }
            else
            if( findItem->data(0).toString()=="Water" )
            {
                removedItems.water.push_back(((ItemWater *)findItem)->waterData);
                ((ItemWater *)findItem)->removeFromArray();
                deleted=true;
            }
            else
            if((findItem->data(0).toString()=="Door_enter")||(findItem->data(0).toString()=="Door_exit"))
            {
                LevelDoors tData = ((ItemDoor*)findItem)->doorData;
                                            tData.isSetIn = (findItem->data(0).toString()=="Door_enter");
                                            tData.isSetOut = (findItem->data(0).toString()=="Door_exit");
                                            removedItems.doors.push_back(tData);
                ((ItemDoor *)findItem)->removeFromArray();
                MainWinConnect::pMainWin->setDoorData(-2);
                deleted=true;
            }
            else
            if(( findItem->data(0).toString()=="player1" )||( findItem->data(0).toString()=="player2" ))
            {
                unsigned long player=1;

                if(findItem->data(0).toString()=="player1")
                    player=1;
                if(findItem->data(0).toString()=="player2")
                    player=2;

                for(int plr=0; plr<LvlData->players.size(); plr++)
                {
                 if(LvlData->players[plr].id == player)
                 {
                     removedItems.players.push_back(LvlData->players[plr]);
                     LvlData->players[plr].x = 0;
                     LvlData->players[plr].y = 0;
                     LvlData->players[plr].w = 0;
                     LvlData->players[plr].h = 0;
                     //    Uncomment this after add player point history
                     deleted=true;
                     break;
                 }
                }
            }
            removeItem(findItem);
            delete findItem;
            if(deleted)addRemoveHistory(removedItems);
        }
    }
}


void LvlScene::setSectionResizer(bool enabled, bool accept)
{
    if((enabled)&&(pResizer==NULL))
    {
        int x = LvlData->sections[LvlData->CurSection].size_left;
        int y = LvlData->sections[LvlData->CurSection].size_top;
        int w = LvlData->sections[LvlData->CurSection].size_right;
        int h = LvlData->sections[LvlData->CurSection].size_bottom;

        pResizer = new ItemResizer( QSize((long)fabs(x-w), (long)fabs(y-h)), Qt::green, 32 );
        this->addItem(pResizer);
        pResizer->setPos(x, y);
        pResizer->type=0;
        pResizer->_minSize = QSizeF(800, 600);
        this->setFocus(Qt::ActiveWindowFocusReason);
        //DrawMode=true;
        MainWinConnect::pMainWin->activeLvlEditWin()->changeCursor(5);
    }
    else
    {
        if(pResizer!=NULL)
        {
            if(accept)
            {
                #ifdef _DEBUG_
                WriteToLog(QtDebugMsg, QString("SECTION RESIZE -> to %1 x %2").arg(pResizer->_width).arg(pResizer->_height));
                #endif
                long l = pResizer->pos().x();
                long t = pResizer->pos().y();
                long r = l+pResizer->_width;
                long b = t+pResizer->_height;
                long oldL = LvlData->sections[LvlData->CurSection].size_left;
                long oldR = LvlData->sections[LvlData->CurSection].size_right;
                long oldT = LvlData->sections[LvlData->CurSection].size_top;
                long oldB = LvlData->sections[LvlData->CurSection].size_bottom;
                LvlData->sections[LvlData->CurSection].size_left = l;
                LvlData->sections[LvlData->CurSection].size_right = r;
                LvlData->sections[LvlData->CurSection].size_top = t;
                LvlData->sections[LvlData->CurSection].size_bottom = b;

                addResizeSectionHistory(LvlData->CurSection, oldL, oldT, oldR, oldB, l, t, r, b);

                ChangeSectionBG(LvlData->sections[LvlData->CurSection].background);
                drawSpace();
                LvlData->modified = true;
            }
            delete pResizer;
            pResizer = NULL;
            MainWinConnect::pMainWin->on_actionSelect_triggered();
            //resetResizingSection=true;
        }
        DrawMode=false;
    }
}


static long eventID=0;
void LvlScene::setEventSctSizeResizer(long event, bool enabled, bool accept)
{
    if(event>=0)
        eventID=event;

    if((enabled)&&(pResizer==NULL))
    {
        int x = LvlData->events[eventID].sets[LvlData->CurSection].position_left;
        int y = LvlData->events[eventID].sets[LvlData->CurSection].position_top;
        int w = LvlData->events[eventID].sets[LvlData->CurSection].position_right;
        int h = LvlData->events[eventID].sets[LvlData->CurSection].position_bottom;

        pResizer = new ItemResizer( QSize((long)fabs(x-w), (long)fabs(y-h)), Qt::yellow, 32 );
        this->addItem(pResizer);
        pResizer->setPos(x, y);
        pResizer->type=1;
        pResizer->_minSize = QSizeF(800, 600);
        this->setFocus(Qt::ActiveWindowFocusReason);
        //DrawMode=true;
        MainWinConnect::pMainWin->activeLvlEditWin()->changeCursor(5);
    }
    else
    {
        if(pResizer!=NULL)
        {
            if(accept)
            {
                #ifdef _DEBUG_
                WriteToLog(QtDebugMsg, QString("SECTION RESIZE -> to %1 x %2").arg(pResizer->_width).arg(pResizer->_height));
                #endif
                long l = pResizer->pos().x();
                long t = pResizer->pos().y();
                long r = l+pResizer->_width;
                long b = t+pResizer->_height;
                //long oldL = LvlData->events[eventID].sets[LvlData->CurSection].position_left;
                //long oldR = LvlData->events[eventID].sets[LvlData->CurSection].position_right;
                //long oldT = LvlData->events[eventID].sets[LvlData->CurSection].position_top;
                //long oldB = LvlData->events[eventID].sets[LvlData->CurSection].position_bottom;
                QList<QVariant> sizeData;
                sizeData.push_back((qlonglong)LvlData->CurSection);
                sizeData.push_back((qlonglong)LvlData->events[eventID].sets[LvlData->CurSection].position_top);
                sizeData.push_back((qlonglong)LvlData->events[eventID].sets[LvlData->CurSection].position_right);
                sizeData.push_back((qlonglong)LvlData->events[eventID].sets[LvlData->CurSection].position_bottom);
                sizeData.push_back((qlonglong)LvlData->events[eventID].sets[LvlData->CurSection].position_left);
                sizeData.push_back((qlonglong)t);
                sizeData.push_back((qlonglong)r);
                sizeData.push_back((qlonglong)b);
                sizeData.push_back((qlonglong)l);
                addChangeEventSettingsHistory(LvlData->events[eventID].array_id, LvlScene::SETTING_EV_SECSIZE, QVariant(sizeData));

                LvlData->events[eventID].sets[LvlData->CurSection].position_left = l;
                LvlData->events[eventID].sets[LvlData->CurSection].position_right = r;
                LvlData->events[eventID].sets[LvlData->CurSection].position_top = t;
                LvlData->events[eventID].sets[LvlData->CurSection].position_bottom = b;

                //addResizeSectionHistory(LvlData->CurSection, oldL, oldT, oldR, oldB, l, t, r, b);

                //ChangeSectionBG(LvlData->sections[LvlData->CurSection].background);
                //drawSpace();
                LvlData->modified = true;
                MainWinConnect::pMainWin->eventSectionSettingsSync();
            }
            delete pResizer;
            pResizer = NULL;
            MainWinConnect::pMainWin->on_actionSelect_triggered();
            //resetResizingSection=true;
        }
        DrawMode=false;
    }
}


void LvlScene::setBlockResizer(QGraphicsItem * targetBlock, bool enabled, bool accept)
{
    if((enabled)&&(pResizer==NULL))
    {
        int x = ((ItemBlock *)targetBlock)->blockData.x;
        int y = ((ItemBlock *)targetBlock)->blockData.y;
        int w = ((ItemBlock *)targetBlock)->blockData.w;
        int h = ((ItemBlock *)targetBlock)->blockData.h;

        pResizer = new ItemResizer( QSize(w, h), Qt::blue, 32 );
        this->addItem(pResizer);
        pResizer->setPos(x, y);
        pResizer->type=2;
        pResizer->targetItem = targetBlock;
        pResizer->_minSize = QSizeF(64, 64);
        this->setFocus(Qt::ActiveWindowFocusReason);
        //DrawMode=true;
        MainWinConnect::pMainWin->activeLvlEditWin()->changeCursor(5);
    }
    else
    {
        if(pResizer!=NULL)
        {
            if(accept)
            {
                #ifdef _DEBUG_
                WriteToLog(QtDebugMsg, QString("BLOCK RESIZE -> to %1 x %2").arg(pResizer->_width).arg(pResizer->_height));
                #endif
                long x = pResizer->pos().x();
                long y = pResizer->pos().y();
                long w = pResizer->_width;
                long h = pResizer->_height;
                long oldX = ((ItemBlock *)pResizer->targetItem)->blockData.x;
                long oldY = ((ItemBlock *)pResizer->targetItem)->blockData.y;
                long oldW = ((ItemBlock *)pResizer->targetItem)->blockData.w;
                long oldH = ((ItemBlock *)pResizer->targetItem)->blockData.h;
                ((ItemBlock *)pResizer->targetItem)->blockData.x = x;
                ((ItemBlock *)pResizer->targetItem)->blockData.y = y;
                ((ItemBlock *)pResizer->targetItem)->blockData.w = w;
                ((ItemBlock *)pResizer->targetItem)->blockData.h = h;

                ((ItemBlock *)pResizer->targetItem)->setBlockSize( QRect(x,y,w,h) );
                LvlData->modified = true;

                addResizeBlockHistory(((ItemBlock *)pResizer->targetItem)->blockData, oldX, oldY, oldX+oldW, oldY+oldH, x, y, x+w, y+h);

                //ChangeSectionBG(LvlData->sections[LvlData->CurSection].background);
                //drawSpace();
            }
            delete pResizer;
            pResizer = NULL;
            MainWinConnect::pMainWin->on_actionSelect_triggered();
            //resetResizingSection=true;
        }
        DrawMode=false;
    }
}

void LvlScene::SwitchEditingMode(int EdtMode)
{
    //int EditingMode; // 0 - selecting,  1 - erasing, 2 - placeNewObject
                     // 3 - drawing water/sand zone, 4 - placing from Buffer
    //bool EraserEnabled;
    //bool PasteFromBuffer;

    //bool DrawMode; //Placing/drawing on map, disable selecting and dragging items

    //bool disableMoveItems;

    //bool contextMenuOpened;
    EraserEnabled=false;
    PasteFromBuffer=false;
    DrawMode=false;
    disableMoveItems=false;

    switch(EdtMode)
    {
    case MODE_PlacingNew:
        DrawMode=true;
        setSectionResizer(false, false);

        break;
    case MODE_DrawSquare:
        resetCursor();
        setSectionResizer(false, false);
        DrawMode=true;
        break;

    case MODE_Resizing:
        resetCursor();
        DrawMode=true;
        disableMoveItems=true;
        break;

    case MODE_PasteFromClip:
        resetCursor();
        setSectionResizer(false, false);
        disableMoveItems=true;
        break;

    case MODE_Erasing:
        resetCursor();
        setSectionResizer(false, false);
        break;

    case MODE_SelectingOnly:
        resetCursor();
        setSectionResizer(false, false);
        disableMoveItems=true;
        break;
    case MODE_Selecting:
    default:
        resetCursor();
        setSectionResizer(false, false);
        break;
    }
    EditingMode = EdtMode;

}
