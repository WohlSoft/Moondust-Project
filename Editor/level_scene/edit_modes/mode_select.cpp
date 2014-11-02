#include "mode_select.h"
#include "../lvlscene.h"
#include "../../common_features/mainwinconnect.h"
#include "../../common_features/item_rectangles.h"

#include "../item_bgo.h"
#include "../item_block.h"
#include "../item_npc.h"
#include "../item_water.h"
#include "../item_playerpoint.h"
#include "../item_door.h"

ModeSelect::ModeSelect(QGraphicsScene *parentScene, QObject *parent)
    : EditMode("Select", parentScene, parent)
{
    sourcePos=QPoint(0,0);
    gridSize=0;
    offsetX=0;
    offsetY=0;//, gridX, gridY, i=0;
}

ModeSelect::~ModeSelect()
{}


void ModeSelect::set()
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    s->EraserEnabled=false;
    s->PasteFromBuffer=false;
    s->DrawMode=false;
    s->disableMoveItems=false;

    s->resetCursor();
    s->resetResizers();

    s->_viewPort->setInteractive(true);
    s->_viewPort->setCursor(Qt::ArrowCursor);
    s->_viewPort->setDragMode(QGraphicsView::RubberBandDrag);
}


void ModeSelect::mousePress(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent);
}


void ModeSelect::mouseMove(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    if(!( mouseEvent->buttons() & Qt::LeftButton )) return;
    if(s->cursor) s->cursor->setPos(mouseEvent->scenePos());
}

void ModeSelect::mouseRelease(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(!scene) return;
    LvlScene *s = dynamic_cast<LvlScene *>(scene);

    if( mouseEvent->button() != Qt::LeftButton ) return;

    QString ObjType;
    bool collisionPassed = false;
    //History
    LevelData historyBuffer;
    LevelData historySourceBuffer;

    QList<QGraphicsItem*> selectedList = s->selectedItems();

    // check for grid snap
    if ((!selectedList.isEmpty())&&(s->mouseMoved))
    {
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
           ObjType = (*it)->data(0).toString();

           /////////////////////////GET DATA///////////////
           s->setItemSourceData((*it), (*it)->data(0).toString()); //Set Grid Size/Offset, sourcePosition
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
                LevelDoors oldDoorData = dynamic_cast<ItemDoor *>(*it)->doorData;
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
                LevelDoors newDoorData = dynamic_cast<ItemDoor *>(*it)->doorData;
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
                LevelDoors oldDoorData = dynamic_cast<ItemDoor *>(*it)->doorData;
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
                LevelDoors newDoorData = dynamic_cast<ItemDoor *>(*it)->doorData;
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

void ModeSelect::keyPress(QKeyEvent *keyEvent)
{
    Q_UNUSED(keyEvent);
}

void ModeSelect::keyRelease(QKeyEvent *keyEvent)
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



void ModeSelect::setItemSourceData(QGraphicsItem * it, QString ObjType)
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
