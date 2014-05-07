/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsItemAnimation>
#include <QKeyEvent>
#include <QBitmap>
#include <QPainter>
#include <QMessageBox>
#include <QApplication>
#include <QGraphicsItem>
#include <QProgressDialog>
#include <QtCore>
#include <QDebug>

#include "lvlscene.h"
#include "lvl_filedata.h"
#include "dataconfigs.h"
#include "logger.h"

#include "item_block.h"
#include "item_bgo.h"

#include "leveledit.h"

LvlScene::LvlScene(dataconfigs &configs, LevelData &FileData, QObject *parent) : QGraphicsScene(parent)
{
    //Set the background GraphicsItem's points
    for(int i=0;i<22;i++)
        BgItem.push_back(new QGraphicsPixmapItem);

    pConfigs = &configs; // Pointer to Main Configs
    opts.animationEnabled = true;
    opts.collisionsEnabled = true;

    index_blocks = pConfigs->index_blocks; //Applaying blocks indexes
    index_bgo = pConfigs->index_bgo;

    LvlData = &FileData; //Ad pointer to level data
    grid = true;
    EditingMode = 0;
    EraserEnabled = false;
    PasteFromBuffer = false;
    wasPasted = false;
    IsMoved = false;
    haveSelected = false;

    //sbZ = 0;

    QPixmap cur(QSize(1,1));
    cur.fill(Qt::black);
    cursor = addPixmap(QPixmap(cur));
    cursor->setZValue(1000);
    cursor->hide();

    uBlockImg = QPixmap(QApplication::applicationDirPath() + "/" + "data/unknown_block.gif");
    npcmask = QBitmap(QApplication::applicationDirPath() + "/" + "data/unknown_npcm.gif");
    uNpcImg = QPixmap(QApplication::applicationDirPath() + "/" + "data/unknown_npc.gif");
    uNpcImg.setMask(npcmask);

    uBgoImg = QPixmap(QApplication::applicationDirPath() + "/" + "data/unknown_bgo.gif");

    //setZ Indexes
    bgZ = -1000;
    blockZs = -150; // sizable blocks
    bgoZb = -100; // backround BGO
    npcZb = -50; // standart NPC

    blockZ = 1; // standart block
    playerZ = 5; //player Point

    bgoZf = 50; // foreground BGO

    blockZl = 100;
    npcZf = 150; // foreground NPC
    waterZ = 500;
    doorZ = 700;
    spaceZ1 = 1000; // interSection space layer
    spaceZ2 = 1020; // section Border

    setItemIndexMethod(NoIndex);

    lock_bgo=false;
    lock_block=false;
    lock_npc=false;
    lock_door=false;
    lock_water=false;
    //bgoMenu->addAction("BGO");
    //npcMenu->addAction("NPC");
    //waterMenu->addAction("Water");
    //DoorMenu->addAction("Door");
}

LvlScene::~LvlScene()
{
    uBGs.clear();
    uBGOs.clear();
    uBlocks.clear();

    foreach(QGraphicsPixmapItem * it, BgItem)
        free(it);
}

void LvlScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
        //QList<QGraphicsItem*> selectedList = selectedItems();

        cursor->setPos(mouseEvent->scenePos());
        cursor->show();

        haveSelected=true;

        if(EditingMode==1) // if Editing Mode = Esaising
        {
            EraserEnabled = true;
        }
        else
        if(EditingMode==4)
        {
            PasteFromBuffer = true;
        }
        /* if (!selectedList.isEmpty())
        {

            for (QList<QGraphicsItem*>::iterator it = selectedList.begin(); it != selectedList.end(); it++)
            {
                // Z.push_back((*it)->zValue());
                // (*it)->setZValue(1000);
            }

        }*/
        QGraphicsScene::mousePressEvent(mouseEvent);
}


void LvlScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    cursor->setPos(mouseEvent->scenePos());
    QGraphicsItem * findItem;
    bool removeIt=true;

    if(haveSelected)
        IsMoved = true;

    if (EraserEnabled) { // Remove All items, placed under Cursor
        findItem = itemCollidesCursor(cursor);
        if(findItem)
        {
            if((findItem->data(0).toString()=="Block")&&(lock_block))
                removeIt=false;
            else
            if((findItem->data(0).toString()=="BGO")&&(lock_bgo))
                removeIt=false;
            else
            if((findItem->data(0).toString()=="NPC")&&(lock_npc))
                removeIt=false;
            else
            if((findItem->data(0).toString()=="Water")&&(lock_water))
                removeIt=false;
            else
            if(((findItem->data(0).toString()=="Door_enter")||(findItem->data(0).toString()=="Door_exit"))&&
                    (lock_door))
                removeIt=false;

            if(removeIt)
            {
                //remove data from main array before deletion item from scene
                if( findItem->data(0).toString()=="Block" )
                {
                    ((ItemBlock *)findItem)->removeFromArray();
                }
                else
                if( findItem->data(0).toString()=="BGO" )
                {
                    ((ItemBGO *)findItem)->removeFromArray();
                }
                removeItem(findItem);
            }
        }
        QGraphicsScene::mouseMoveEvent(mouseEvent);
    } else
        QGraphicsScene::mouseMoveEvent(mouseEvent);
}



void LvlScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
    {
            int gridSize=32, offsetX=0, offsetY=0, gridX, gridY;//, i=0;
            QPoint sourcePos;

            cursor->hide();

            haveSelected = false;

            QString ObjType;
            int collisionPassed = false;

            if(PasteFromBuffer)
            {
                paste( LvlBuffer, mouseEvent->scenePos().toPoint() );
                EditingMode = 0;
                PasteFromBuffer = false;
                IsMoved=true;
                wasPasted = true; //Set flag for reset pasta cursor to normal select
            }

            QList<QGraphicsItem*> selectedList = selectedItems();

            // check for grid snap
            if ((!selectedList.isEmpty())&&(IsMoved))
            {
                IsMoved = false;
                // correct selected items' coordinates
                for (QList<QGraphicsItem*>::iterator it = selectedList.begin(); it != selectedList.end(); it++)
                {
                    if(EraserEnabled)
                    {
                        //remove data from main array before deletion item from scene
                        if( (*it)->data(0).toString()=="Block" )
                        {
                            ((ItemBlock *)(*it))->removeFromArray();
                        }
                        else
                        if( (*it)->data(0).toString()=="BGO" )
                        {
                            ((ItemBGO *)(*it))->removeFromArray();
                        }

                        removeItem((*it)); continue;
                    }

                    gridSize = 32;
                    ObjType = (*it)->data(0).toString();

                    //(*it)->setZValue(Z);
                    if( ObjType == "NPC")
                    {
                        gridSize = 1;
                    }
                    else
                    if( ObjType == "BGO")
                    {
                        gridSize = ((ItemBGO *)(*it))->gridSize;
                        offsetX = ((ItemBGO *)(*it))->gridOffsetX;
                        offsetY = ((ItemBGO *)(*it))->gridOffsetY;
                    }else
                    if( ObjType == "Water")
                    {
                        gridSize = 16;
                    }else
                    if( ObjType == "Door_enter")
                        gridSize = 16 ;
                    else
                    if( ObjType == "Door_exit")
                        gridSize = 16 ;
                    else
                    if( ObjType == "player1")
                    {
                        offsetY = 2;
                        gridSize = 2 ;
                    }
                    else
                    if( ObjType == "player2")
                    {
                        offsetY = 2;
                        gridSize = 2 ;
                    }

                    QPointF itemPos;

                    itemPos = (*it)->scenePos();

                    if(grid)
                    { //ATTACH TO GRID
                        gridX = ((int)itemPos.x() - (int)itemPos.x() % gridSize);
                        gridY = ((int)itemPos.y() - (int)itemPos.y() % gridSize);

                        if((int)itemPos.x()<0)
                        {
                            if( (int)itemPos.x() < offsetX+gridX - (int)(gridSize/2) )
                                gridX -= gridSize;
                        }
                        else
                        {
                            if( (int)itemPos.x() > offsetX+gridX + (int)(gridSize/2) )
                                gridX += gridSize;
                        }

                        if((int)itemPos.y()<0)
                        {if( (int)itemPos.y() < offsetY+gridY - (int)(gridSize/2) )
                            gridY -= gridSize;
                        }
                        else {if( (int)itemPos.y() > offsetY+gridY + (int)(gridSize/2) )
                         gridY += gridSize;
                        }

                        if(ObjType=="Block")
                        {
                            (*it)->setPos(QPointF(gridX, gridY));
                        }
                            else
                        (*it)->setPos(QPointF(offsetX+gridX, offsetY+gridY));
                    }
                }
                EraserEnabled = false;


                // Check collisions
                //Only if collision ckecking enabled
                if(!PasteFromBuffer)

                for (QList<QGraphicsItem*>::iterator it = selectedList.begin(); it != selectedList.end(); it++)
                {
                    ObjType = (*it)->data(0).toString();

                    WriteToLog(QtDebugMsg, QString(" >>Check collision with \"%1\"").arg(ObjType));

                    if( ObjType == "NPC")
                    {
                        foreach (LevelNPC findInArr, LvlData->npc)
                        {
                            if(findInArr.array_id==(unsigned)(*it)->data(2).toInt())
                            {
                                sourcePos = QPoint(findInArr.x, findInArr.y); break;
                            }
                        }
                    }
                    else
                    if( ObjType == "Block")
                    {
                        sourcePos = QPoint(  ((ItemBlock *)(*it))->blockData.x, ((ItemBlock *)(*it))->blockData.y);
                        //WriteToLog(QtDebugMsg, QString(" >>Check collision for Block"));
                    }
                    else
                    if( ObjType == "BGO")
                    {
                        sourcePos = QPoint(  ((ItemBGO *)(*it))->bgoData.x, ((ItemBGO *)(*it))->bgoData.y);
                        /*
                        foreach (LevelBGO findInArr, LvlData->bgo)
                        {
                            if(findInArr.array_id==(unsigned)(*it)->data(2).toInt())
                            {
                                sourcePos = QPoint(findInArr.x, findInArr.y); break;
                            }
                        }*/
                    }

                    //Check position
                    if( sourcePos == QPoint((long)((*it)->scenePos().x()), ((long)(*it)->scenePos().y())))
                    {
                        WriteToLog(QtDebugMsg, QString(" >>Collision skiped, posSource=posCurrent"));
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
                            ((ItemBlock *)(*it))->blockData.x = (long)(*it)->scenePos().x();
                            ((ItemBlock *)(*it))->blockData.y = (long)(*it)->scenePos().y();
                            ((ItemBlock *)(*it))->arrayApply();
                            LvlData->modified = true;
                        }
                        else
                        if( ObjType == "BGO")
                        {
                            //Applay move into main array
                            ((ItemBGO *)(*it))->bgoData.x = (long)(*it)->scenePos().x();
                            ((ItemBGO *)(*it))->bgoData.y = (long)(*it)->scenePos().y();
                            ((ItemBGO *)(*it))->arrayApply();
                            LvlData->modified = true;
                        }
                    }
                }


                QGraphicsScene::mouseReleaseEvent(mouseEvent);
                return;
            }
            EraserEnabled = false;
            QGraphicsScene::mouseReleaseEvent(mouseEvent);
    }



//Copy selected items into clipboard
LevelData LvlScene::copy()
{

    //Get Selected Items
    QList<QGraphicsItem*> selectedList = selectedItems();

    LevelData copyData;

    if (!selectedList.isEmpty())
    {
        for (QList<QGraphicsItem*>::iterator it = selectedList.begin(); it != selectedList.end(); it++)
        {
            QString ObjType = (*it)->data(0).toString();

            if( ObjType == "Block")
            {
                copyData.blocks.push_back(((ItemBlock *)(*it))->blockData);
            }
            else
            if( ObjType == "BGO")
            {
                copyData.bgo.push_back(((ItemBGO *)(*it))->bgoData);
            }
            else
            if( ObjType == "NPC")
            {
                foreach (LevelNPC findInArr, LvlData->npc)
                {
                    if(findInArr.array_id==(unsigned)(*it)->data(2).toInt())
                    {
                        copyData.npc.push_back(findInArr);
                        break;
                    }
                }
            }

        }//fetch selected items
    }

    return copyData;
}

void LvlScene::paste(LevelData BufferIn, QPoint pos)
{
    long baseX, baseY;
    //set first base
    if(!BufferIn.blocks.isEmpty()){
        baseX = BufferIn.blocks[0].x;
        baseY = BufferIn.blocks[0].y;
    }else if(!BufferIn.bgo.isEmpty()){
        baseX = BufferIn.bgo[0].x;
        baseY = BufferIn.bgo[0].y;
    }else if(!BufferIn.npc.isEmpty()){
        baseX = BufferIn.npc[0].x;
        baseY = BufferIn.npc[0].y;
    }else{
        //nothing to paste
        return;
    }

    foreach (LevelBlock block, BufferIn.blocks) {
        if(block.x<baseX){
            baseX = block.x;
        }
        if(block.y<baseY){
            baseY = block.y;
        }
    }
    foreach (LevelBGO bgo, BufferIn.bgo){
        if(bgo.x<baseX){
            baseX = bgo.x;
        }
        if(bgo.y<baseY){
            baseY = bgo.y;
        }
    }
    foreach (LevelNPC npc, BufferIn.npc){
        if(npc.x<baseX){
            baseX = npc.x;
        }
        if(npc.y<baseY){
            baseY = npc.y;
        }
    }

    foreach (LevelBlock block, BufferIn.blocks){
        //Gen Copy of Block
        LevelBlock dumpBlock = block;
        dumpBlock.x = (long)pos.x() + block.x - baseX;
        dumpBlock.y = (long)pos.y() + block.y - baseY;
        LvlData->blocks_array_id++;
        dumpBlock.array_id = LvlData->blocks_array_id;
        placeBlock(dumpBlock);
    }
    foreach (LevelBGO bgo, BufferIn.bgo){
        //Gen Copy of BGO
        LevelBGO dumpBGO = bgo;
        dumpBGO.x = (long)pos.x() + bgo.x - baseX;
        dumpBGO.y = (long)pos.y() + bgo.y - baseY;
        LvlData->bgo_array_id++;
        dumpBGO.array_id = LvlData->bgo_array_id;
        placeBGO(dumpBGO);
    }
    foreach (LevelNPC npc, BufferIn.npc){
        //Gen Copy of NPC
        LevelNPC dumpNPC = npc;
        dumpNPC.x = (long)pos.x() + npc.x - baseX;
        dumpNPC.y = (long)pos.y() + npc.y - baseY;
        LvlData->npc_array_id++;
        dumpNPC.array_id = LvlData->npc_array_id;
        placeNPC(dumpNPC);
    }

}


QGraphicsItem * LvlScene::itemCollidesWith(QGraphicsItem * item)
{
    qlonglong leftA, leftB;
    qlonglong rightA, rightB;
    qlonglong topA, topB;
    qlonglong bottomA, bottomB;
    //qreal betweenZ;

    QList<QGraphicsItem *> collisions = collidingItems(item, Qt::IntersectsItemBoundingRect);
    foreach (QGraphicsItem * it, collisions) {
            if (it == item)
                 continue;
            if(item->data(0).toString()=="Water")
                return NULL;
            if(item->data(0).toString()=="Door_exit")
                return NULL;
            if(item->data(0).toString()=="Door_enter")
                return NULL;

            leftA = item->scenePos().x();
            rightA = item->scenePos().x()+item->data(9).toLongLong();
            topA = item->scenePos().y();
            bottomA = item->scenePos().y()+item->data(10).toLongLong();

            leftB = it->scenePos().x();
            rightB = it->scenePos().x()+it->data(9).toLongLong();
            topB = it->scenePos().y();
            bottomB = it->scenePos().y()+it->data(10).toLongLong();

            if(it->data(0).toString()=="Block")
                WriteToLog(QtDebugMsg, QString(" >>Collision with block detected"));

              if((item->data(0).toString()=="Block")||(item->data(0).toString()=="NPC")
                      ||(item->data(0).toString()=="BGO"))
              {
                  if(item->data(0).toString()!=it->data(0).toString()) continue;

                  if(item->data(3).toString()=="sizable")
                  {//sizable Block
                      WriteToLog(QtDebugMsg, QString("sizable block") );
                      //Change berween Z-Values

                      /* This is trash, because exist better method for Z-Value sorting
                      if(it->data(3).toString()=="sizable")
                      {
                          WriteToLog(QtDebugMsg, QString("Colliding with sizable Z: %1 %2")
                                     .arg(item->zValue()).arg(it->zValue()));

                          if( (item->scenePos().y() > it->scenePos().y()) &&
                          ( item->zValue() <= it->zValue() ) )
                              {
                                betweenZ = it->zValue();
                                it->setZValue(item->zValue());
                                item->setZValue(betweenZ);

                                if(item->zValue() == it->zValue()) item->setZValue(item->zValue() + 0.0000000001);
                               // betweenZ+=0.0000000001;
                                //item->setZValue(it->zValue() + 0.0000000001);

                                WriteToLog(QtDebugMsg, QString("sizable block changed Z-") );
                              }
                          else
                          if( (item->scenePos().y() < it->scenePos().y() ) &&
                          ( item->zValue() >= it->zValue() ) )
                              {
                                betweenZ = it->zValue();
                                it->setZValue(item->zValue());
                                item->setZValue(betweenZ);

                                if(item->zValue() == it->zValue()) item->setZValue(item->zValue() - 0.00000000001);
                               // item->setZValue(it->zValue() + 0.0000000001);

                                WriteToLog(QtDebugMsg, QString("sizable block changed Z+") );
                              }
                      } */
                      continue;
                  }//sizable Block

                  if(item->data(0).toString()=="BGO")
                    if(item->data(1).toInt()!=it->data(1).toInt()) continue;

                     if( bottomA <= topB )
                     { continue; }
                     if( topA >= bottomB )
                     { continue; }
                     if( rightA <= leftB )
                     { continue; }
                     if( leftA >= rightB )
                     { continue; }

                     if(it->data(3).toString()!="sizable")
                        return it;
              }

    }
    return NULL;
}

QGraphicsItem * LvlScene::itemCollidesCursor(QGraphicsItem * item)
{
    QList<QGraphicsItem *> collisions = collidingItems(item);
    foreach (QGraphicsItem * it, collisions) {
            if (it == item)
                 continue;
            if(
                    (it->data(0).toString()=="Block")||
                    (it->data(0).toString()=="BGO")||
                    (it->data(0).toString()=="NPC")||
                    (it->data(0).toString()=="door_exit")||
                    (it->data(0).toString()=="door_enter")||
                    (it->data(0).toString()=="water")
              )
                return it;
    }
    return NULL;
}

/*
QGraphicsItem * LvlScene::itemCollidesMouse(QGraphicsItem * item)
{
    QList<QGraphicsItem *> collisions = collidingItems(item, Qt::ContainsItemShape);
    foreach (QGraphicsItem * it, collisions) {
            if (it == item)
                 continue;
            if(item->data(0).toString()=="Water")
                return NULL;
            if(item->data(0).toString()=="Door_exit")
                return NULL;
            if(item->data(0).toString()=="Door_enter")
                return NULL;

        if( item->data(0).toString() ==  it->data(0).toString() )
            return it;
    }
    return NULL;
}
*/


//Search and load custom User's files
void LvlScene::loadUserData(LevelData FileData, QProgressDialog &progress)
{
    int i, total=0;

    UserBGs uBG;
    UserBlocks uBlock;
    UserBGOs uBGO;

    bool loaded1, loaded2;
    QString uLVLDs = FileData.path + "/" + FileData.filename + "/";
    QString uLVLD = FileData.path + "/" + FileData.filename;
    QString uLVLs = FileData.path + "/";

    //Load Backgrounds
    for(i=0; i<pConfigs->main_bg.size(); i++) //Add user images
        {
        if(!progress.wasCanceled())
            progress.setLabelText("Search User Backgrounds "+QString::number(i)+"/"+QString::number(pConfigs->main_bg.size()));

            loaded1 = false;
            loaded2 = false;

            //check for first image
            if((QFile::exists(uLVLD) ) &&
                  (QFile::exists(uLVLDs + pConfigs->main_bg[i].image_n)) )
            {
                uBG.image = QPixmap( uLVLDs + pConfigs->main_bg[i].image_n );
                uBG.id = pConfigs->main_bg[i].id;
                loaded1 = true;
            }
            else
            if(QFile::exists(uLVLs + pConfigs->main_bg[i].image_n) )
            {
                uBG.image = QPixmap( uLVLs + pConfigs->main_bg[i].image_n );
                uBG.id = pConfigs->main_bg[i].id;
                loaded1 = true;
            }

            if((loaded1)&&(pConfigs->main_bg[i].animated) )
            {
                uBG.image=uBG.image.copy(0, 0, uBG.image.width(), (int)round(uBG.image.height()/pConfigs->main_bg[i].frames));
            }

            //check for second image
            if(pConfigs->main_bg[i].type == 1)
            {
                if((QFile::exists(uLVLD) ) &&
                      (QFile::exists(uLVLDs + pConfigs->main_bg[i].second_image_n )) )
                {
                    uBG.second_image = QPixmap( uLVLDs + pConfigs->main_bg[i].second_image_n );
                    uBG.id = pConfigs->main_bg[i].id;
                    loaded2 = true;
                }
                else
                if(QFile::exists(uLVLs + pConfigs->main_bg[i].second_image_n) )
                {
                    uBG.second_image = QPixmap( uLVLs + pConfigs->main_bg[i].second_image_n );
                    uBG.id = pConfigs->main_bg[i].id;
                    loaded2 = true;
                }
            }
            if((loaded1)&&(!loaded2)) uBG.q = 0;
            if((!loaded1)&&(loaded2)) uBG.q = 1;
            if((loaded1)&&(loaded2)) uBG.q = 2;

            //If user images found and loaded
            if( (loaded1) || (loaded2) )
                uBGs.push_back(uBG);

        total++;
        if(!progress.wasCanceled())
            progress.setValue(progress.value()+1);
        else return;
        }


    //Load Blocks
    for(i=0; i<pConfigs->main_block.size(); i++) //Add user images
    {

        if(!progress.wasCanceled())
            progress.setLabelText("Search User Blocks "+QString::number(i)+"/"+QString::number(pConfigs->main_block.size()));

            if((QFile::exists(uLVLD) ) &&
                  (QFile::exists(uLVLDs + pConfigs->main_block[i].image_n)) )
            {
                if(QFile::exists(uLVLDs + pConfigs->main_block[i].mask_n))
                    uBlock.mask = QBitmap(uLVLDs + pConfigs->main_block[i].mask_n );
                else
                    uBlock.mask = pConfigs->main_block[i].mask;

                uBlock.image = QPixmap(uLVLDs + pConfigs->main_block[i].image_n );

                if((uBlock.image.height()!=uBlock.mask.height())||(uBlock.image.width()!=uBlock.mask.width()))
                    uBlock.mask = uBlock.mask.copy(0,0,uBlock.image.width(),uBlock.image.height());
                uBlock.image.setMask(uBlock.mask);
                uBlock.id = pConfigs->main_block[i].id;
                uBlocks.push_back(uBlock);

                //Apply index;
                if(uBlock.id < (unsigned int)index_blocks.size())
                {
                    index_blocks[uBlock.id].type = 1;
                    index_blocks[uBlock.id].i = (uBlocks.size()-1);
                }
            }
            else
            if(QFile::exists(uLVLs + pConfigs->main_block[i].image_n) )
            {
                if(QFile::exists(uLVLs + pConfigs->main_block[i].mask_n))
                    uBlock.mask = QBitmap(uLVLs + pConfigs->main_block[i].mask_n );
                else
                    uBlock.mask = pConfigs->main_block[i].mask;

                uBlock.image = QPixmap(uLVLs + pConfigs->main_block[i].image_n );

                if((uBlock.image.height()!=uBlock.mask.height())||(uBlock.image.width()!=uBlock.mask.width()))
                    uBlock.mask = uBlock.mask.copy(0,0,uBlock.image.width(),uBlock.image.height());

                uBlock.image.setMask(uBlock.mask);
                uBlock.id = pConfigs->main_block[i].id;
                uBlocks.push_back(uBlock);

                //Apply index;
                if(uBlock.id < (unsigned int)index_blocks.size())
                {
                    index_blocks[uBlock.id].type = 1;
                    index_blocks[uBlock.id].i = (uBlocks.size()-1);
                }
            }

    if(!progress.wasCanceled())
        progress.setValue(progress.value()+1);
    else return;
    }

    //Load BGO
    for(i=0; i<pConfigs->main_bgo.size(); i++) //Add user images
    {
        if(!progress.wasCanceled())
            progress.setLabelText("Search User BGOs "+QString::number(i)+"/"+QString::number(pConfigs->main_bgo.size()));

            if((QFile::exists(uLVLD) ) &&
                  (QFile::exists(uLVLDs + pConfigs->main_bgo[i].image_n)) )
            {
                if(QFile::exists(uLVLDs + pConfigs->main_bgo[i].mask_n))
                    uBGO.mask = QBitmap(uLVLDs + pConfigs->main_bgo[i].mask_n );
                else
                    uBGO.mask = pConfigs->main_bgo[i].mask;

                uBGO.image = QPixmap(uLVLDs + pConfigs->main_bgo[i].image_n );

                if((uBGO.image.height()!=uBGO.mask.height())||(uBGO.image.width()!=uBGO.mask.width()))
                    uBGO.mask = uBGO.mask.copy(0,0,uBGO.image.width(),uBGO.image.height());
                uBGO.image.setMask(uBGO.mask);
                uBGO.id = pConfigs->main_bgo[i].id;
                uBGOs.push_back(uBGO);
            }
            else
            if(QFile::exists(uLVLs + pConfigs->main_bgo[i].image_n) )
            {
                if(QFile::exists(uLVLs + pConfigs->main_bgo[i].mask_n))
                    uBGO.mask = QBitmap(uLVLs + pConfigs->main_bgo[i].mask_n );
                else
                    uBGO.mask = pConfigs->main_bgo[i].mask;

                uBGO.image = QPixmap(uLVLs + pConfigs->main_bgo[i].image_n );

                if((uBGO.image.height()!=uBGO.mask.height())||(uBGO.image.width()!=uBGO.mask.width()))
                    uBGO.mask = uBGO.mask.copy(0,0,uBGO.image.width(),uBGO.image.height());

                uBGO.image.setMask(uBGO.mask);
                uBGO.id = pConfigs->main_bgo[i].id;
                uBGOs.push_back(uBGO);
            }
    if(!progress.wasCanceled())
        progress.setValue(progress.value()+1);
    else return;
    }

}

void LvlScene::drawSpace(LevelData FileData/*, dataconfigs &configs*/)
{
    WriteToLog(QtDebugMsg, QString("Draw intersection space-> Find and remove current"));
    foreach(QGraphicsItem * spaceItem, items())
    {
        if(spaceItem->data(0).toString()=="Space")
        {
            removeItem(spaceItem);
            break;
        }
    }
    foreach(QGraphicsItem * spaceItem, items())
    {
        if(spaceItem->data(0).toString()=="SectionBorder")
        {
            removeItem(spaceItem);
            break;
        }
    }

    QPolygon bigSpace;
    QGraphicsItem * item;
    QGraphicsItem * item2;
    QVector<QPoint > drawing;

    int i, j;
    long l, r, t, b;
         //x, y, h, w;

    WriteToLog(QtDebugMsg, QString("Find minimal"));
    j=-1;
    do
    {
        j++;
        l = FileData.sections[j].size_left;
        r = FileData.sections[j].size_right;
        t = FileData.sections[j].size_top;
        b = FileData.sections[j].size_bottom;
    }
    while(
          ((FileData.sections[j].size_left==0) &&
          (FileData.sections[j].size_right==0) &&
          (FileData.sections[j].size_top==0) &&
          (FileData.sections[j].size_bottom==0)) && (j<FileData.sections.size())
    );

    for(i=0;i<FileData.sections.size(); i++)
    {

        if(
                (FileData.sections[i].size_left==0) &&
                (FileData.sections[i].size_right==0) &&
                (FileData.sections[i].size_top==0) &&
                (FileData.sections[i].size_bottom==0))
            continue;

        if(FileData.sections[i].size_left < l)
            l = FileData.sections[i].size_left;
        if(FileData.sections[i].size_right > r)
            r = FileData.sections[i].size_right;
        if(FileData.sections[i].size_top < t)
            t = FileData.sections[i].size_top;
        if(FileData.sections[i].size_bottom > b)
            b = FileData.sections[i].size_bottom;
    }

    WriteToLog(QtDebugMsg, QString("Draw polygon"));

    drawing.clear();
    drawing.push_back(QPoint(l-1000, t-1000));
    drawing.push_back(QPoint(r+1000, t-1000));
    drawing.push_back(QPoint(r+1000, b+1000));
    drawing.push_back(QPoint(l-1000, b+1000));
    drawing.push_back(QPoint(l-1000, t+1000));

    bigSpace = QPolygon(drawing);


    l = FileData.sections[FileData.CurSection].size_left;
    r = FileData.sections[FileData.CurSection].size_right;
    t = FileData.sections[FileData.CurSection].size_top;
    b = FileData.sections[FileData.CurSection].size_bottom;


    WriteToLog(QtDebugMsg, QString("Draw editing hole"));
    drawing.clear();
    drawing.push_back(QPoint(l-1, t-1));
    drawing.push_back(QPoint(r+1, t-1));
    drawing.push_back(QPoint(r+1, b+1));
    drawing.push_back(QPoint(l-1, b+1));
    drawing.push_back(QPoint(l-1, t-1));

    bigSpace = bigSpace.subtracted(QPolygon(drawing));

    WriteToLog(QtDebugMsg, QString("add polygon to Item"));
    item = addPolygon(bigSpace, QPen(Qt::NoPen), QBrush(Qt::black));//Add inactive space
    item2 = addPolygon(QPolygon(drawing), QPen(Qt::red, 2));
    item->setZValue(spaceZ1);
    item2->setZValue(spaceZ2);
    item->setOpacity(qreal(0.4));
    item->setData(0, "Space");
    item2->setData(0, "SectionBorder");

}

///////////////////////////////BACKGROUND IMAGE/////////////////////////////////////////
void LvlScene::makeSectionBG(LevelData FileData, QProgressDialog &progress)
//void LvlScene::makeSectionBG(int x, int y, int w, int h)
{
    int i, total=0;
    WriteToLog(QtDebugMsg, QString("Applay Backgrounds"));

    //Load Backgrounds
    for(i=0; i<FileData.sections.size(); i++)
    {
        setSectionBG(FileData.sections[i]);

        total++;

        if(!progress.wasCanceled())
            progress.setValue(progress.value()+1);
        else return;
    }

}

void LvlScene::setSectionBG(LevelSection section)
{
    QGraphicsItem * item;
    QBrush brush;
    QPen pen;
    QPixmap image = QPixmap(QApplication::applicationDirPath() + "/" + "data/nobg.gif");
    QPixmap img;
    QPixmap img2; //Second image buffer
    //need a BGitem

    bool isUser1=false, isUser2=false, noimage=false;
    long x,y,h,w, j;

    if(
        (section.size_left!=0) ||
        (section.size_top!=0)||
        (section.size_bottom!=0)||
        (section.size_right!=0)
      )
    {
        x=section.size_left;
        y=section.size_top;
        w=section.size_right;
        h=section.size_bottom;

        WriteToLog(QtDebugMsg, "Check for user images");

        isUser1=false; // user's images are exist
        isUser2=false; // user's images are exist
        noimage=true;
        j = 0;
        if(section.background != 0 )
        {
            //Find user image
            for(j=0;j<uBGs.size();j++)
            {
                if(uBGs[j].id==section.background)
                {
                    noimage=false;
                    if((uBGs[j].q==0)||(uBGs[j].q==2)) //set first image
                        {img = uBGs[j].image; isUser1=true;}
                    if((uBGs[j].q>=1)){ // set Second image
                        img2 = uBGs[j].second_image; isUser2=true;}
                    break;
                }
            } //If not exist, will be used default

            for(j=0;j<pConfigs->main_bg.size();j++)
            {
                if(pConfigs->main_bg[j].id==section.background)
                {
                    noimage=false;
                      if(!isUser1)
                          img = pConfigs->main_bg[j].image;
                      if(!isUser2)
                          img2 = pConfigs->main_bg[j].second_image;
                    break;
                }
            }
            if((noimage)&&(!isUser1))
            {
                WriteToLog(QtWarningMsg, "Image not found");
                img=image;
            }
        }
        else noimage=true;

        //pConfigs->main_bg[j].type;

        brush = QBrush(QColor(0, 0, 0));
        //QBrush brush(QColor(255, 255, 255));
        pen = QPen(Qt::NoPen);
        //for (int i = 0; i < 11; i++) {

        //item = addRect(QRectF(x, y, , ), pen, brush);

        if((!noimage)&&(!img.isNull()))
        {
            item = addPixmap(image);
            DrawBG(x, y, w, h, img, img2, pConfigs->main_bg[j], BgItem[section.id]);
            BgItem[section.id]->setParentItem(item);
            item->setPos(x, y);
        }
        else
            item = addRect(x, y, (long)fabs(x-w), (long)fabs(y-h), pen, brush);

        WriteToLog(QtDebugMsg, QString("Item placed to x=%1 y=%2 h=%3 w=%4").arg(x).arg(y)
                   .arg((long)fabs(x-w)).arg((long)fabs(y-h)));
        //

        item->setData(0, "BackGround"+QString::number(section.id) );
        item->setZValue(bgZ);
    }

}

void LvlScene::DrawBG(int x, int y, int w, int h, QPixmap srcimg, QPixmap srcimg2, obj_BG &bgsetup, QGraphicsPixmapItem * &BgItem)
{
    QPixmap BackImg;
    QPainter * BGPaint;
    QPixmap img;
    //int i, j;
    int si_attach, attach;
    long toX, toY, bgH, bgW;

    WriteToLog(QtDebugMsg, "Draw BG Image");
    BackImg = QPixmap(QSize( (long)fabs(x-w), (long)fabs(y-h) ));
    attach = bgsetup.attached;
    long px = 0;

    if((bgsetup.type==0)&&(!bgsetup.editing_tiled))
    {   //SingleRow BG

        WriteToLog(QtDebugMsg, "SingleRow BG");
        if(attach==0)
            BackImg.fill( srcimg.toImage().pixel(0,0) );
        else
            BackImg.fill( srcimg.toImage().pixel(0,(srcimg.height()-1)) );
        BGPaint = new QPainter(&BackImg);

        px=0;
        if(attach==0)
            toY = (long)fabs(y-h)-srcimg.height();
        else
            toY = 0;
        bgW = srcimg.width();
        bgH = srcimg.height();

        do
        { //Draw row
            BGPaint->drawPixmap(px, toY, bgW, bgH, srcimg);
            px += srcimg.width();
        }
        while( px < (long)fabs(x-w) );

    }
    else if((bgsetup.type==1)&&(!bgsetup.editing_tiled))
    {   //Double BG
        WriteToLog(QtDebugMsg, "DoubleRow BG");

        si_attach = bgsetup.second_attached; // Second image attach

        //Fill empty space
        if((!srcimg2.isNull()) && (si_attach==0))
            BackImg.fill( srcimg2.toImage().pixel(0,0) );
        else
            BackImg.fill( srcimg.toImage().pixel(0,0) );

        BGPaint = new QPainter(&BackImg);

        px=0;

        toY = (long)fabs(y-h)-srcimg.height();
        bgW = srcimg.width();
        bgH = srcimg.height();

        WriteToLog(QtDebugMsg, QString("Draw first row, params: "));

        //Draw first row
            do{
                BGPaint->drawPixmap(px, toY, bgW, bgH, srcimg);
                px += srcimg.width();
            } while( px < (long)fabs(x-w) );

        WriteToLog(QtDebugMsg, "Draw second row");
        px=0;

        if(si_attach==0) // over first
            toY = (long)fabs(y-h)-srcimg.height()-srcimg2.height();
        else
        if(si_attach==1) // bottom
            toY = (long)fabs(y-h)-srcimg2.height();

        bgW = srcimg2.width();
        bgH = srcimg2.height();

        if(!srcimg2.isNull())
        {
            //Draw seconf row if it no null
            do {
                BGPaint->drawPixmap(px, toY, bgW, bgH, srcimg2);
                px += srcimg2.width();
            } while( px < (long)fabs(x-w) );
        } else WriteToLog(QtDebugMsg, "second image is Null");
    }
    else
    { // Black
        WriteToLog(QtDebugMsg, "Tiled");
        BackImg.fill( Qt::black );
        BGPaint = new QPainter(&BackImg);

        px=0;
        if(attach==0)
            toY = (long)fabs(y-h)-srcimg.height();
        else
            toY = 0;

        bgW = srcimg.width();
        bgH = srcimg.height();

        toX = 0;

        do{
            px=0;
            do
            { //Draw row
                BGPaint->drawPixmap(px, toY + toX*((attach==0)?(-1):1), bgW, bgH, srcimg);
                px += srcimg.width();
            }
            while( px < (long)fabs(x-w) );
            toX+=srcimg.height();
        }   while( toX < (long)fabs(y-h) + srcimg.height() );


    }
    BGPaint->end();

    WriteToLog(QtDebugMsg, "Drawed");

    if(!BackImg.isNull())
        img = BackImg.copy(BackImg.rect());
    else
    {
        WriteToLog(QtDebugMsg, "Drawed PixMap is null");
        img.fill( Qt::red );
    }
    WriteToLog(QtDebugMsg, "Added to QPixmap");

    BgItem->setPixmap(QPixmap(img));
    //return img;
}


void LvlScene::ChangeSectionBG(int BG_Id, LevelData &FileData)
{


    foreach (QGraphicsItem * findBG, items() )
    {
        if(findBG->data(0)=="BackGround"+QString::number(FileData.CurSection) )
        {
            WriteToLog(QtDebugMsg, QString("Remove item BackGround"+QString::number(FileData.CurSection)) );
            removeItem(findBG); break;
        }
    }
    FileData.sections[FileData.CurSection].background = BG_Id;
    FileData.modified = true;

    WriteToLog(QtDebugMsg, "set Background to "+QString::number(BG_Id));
    setSectionBG(FileData.sections[FileData.CurSection]);
}



//////////////////Block////////////////////////////////////////////////////////////////////////////////////////

/* //The Trach
void LvlScene::placeBox(float x, float y)
{
    QGraphicsItem *	box = addPixmap(QPixmap(QApplication::applicationDirPath() + "/" + "data/graphics/level/block/block-8.gif"));
    box->setPos(x, y);
    if (itemCollidesWith(box))
        removeItem(box);
    else
        box->setData(0, "Box");
}
*/

void LvlScene::placeBlock(LevelBlock &block)
{
    bool noimage=true, found=false;
    bool isUser=false;
    int j;

    QGraphicsItem *npc;
    QGraphicsItemGroup *includedNPC;
    ItemBlock *BlockImage = new ItemBlock;

    noimage=true;
    isUser=false;

    //Check Index exists
    if(block.id < (unsigned int)index_blocks.size())
    {
        j = index_blocks[block.id].i;

        if(pConfigs->main_block[j].id == block.id)
            found=true;
    }

    //if Index found
    if(found)
    {   //get neccesary element directly
        if(index_blocks[block.id].type==1)
        {
            isUser=true;
            noimage=false;
            tImg = uBlocks[index_blocks[block.id].i].image;
        }
        else
        {
            tImg = pConfigs->main_block[index_blocks[block.id].i].image;
            noimage=false;
        }
    }
    else
    {
        //found neccesary element in arrays and select
        for(j=0;j<uBlocks.size();j++)
        {
            if(uBlocks[j].id == block.id)
            {
                isUser=true;
                noimage=false;
                tImg = uBlocks[j].image;
                break;
            }
        }

        for(j=0;j<pConfigs->main_block.size();j++)
        {
            if(pConfigs->main_block[j].id==block.id)
            {
                noimage=false;
                if(!isUser)
                    tImg = pConfigs->main_block[j].image; break;
            }
        }
    }

    if((noimage)||(tImg.isNull()))
    {
        //if(block.id==89) WriteToLog(QtDebugMsg, QString("Block 89 is %1, %2").arg(noimage).arg(tImg.isNull()));
        tImg = uBlockImg;
    }

    BlockImage->setBlockData(block, pConfigs->main_block[j].sizable);
    BlockImage->setMainPixmap(tImg);
    addItem(BlockImage);

    BlockImage->setContextMenu(blockMenu);

    if((!noimage) && (pConfigs->main_block[j].animated))
    {
        BlockImage->setAnimation(pConfigs->main_block[j].frames, pConfigs->main_block[j].framespeed, pConfigs->main_block[j].algorithm);
        BlockImage->setData(4, "animated");
    }

    includedNPC = new QGraphicsItemGroup(BlockImage);

    if(block.invisible)
        BlockImage->setOpacity(qreal(0.5));

    BlockImage->setPos(block.x, block.y);

    if(block.npc_id != 0)
    {
        npc = addPixmap( QPixmap(uNpcImg) );
        npc->setPos(block.x, block.y);
        npc->setZValue(blockZ);
        npc->setOpacity(qreal(0.4));
        includedNPC->addToGroup(npc);
    }

    if(pConfigs->main_block[j].sizable)
    {
        BlockImage->setZValue( blockZs + ((double)block.y/(double)100000000000) + 1 - ((double)block.w * (double)0.0000000000000001) ); // applay sizable block Z
        //sbZ += 0.0000000001;
    }
    else
    {
        if(pConfigs->main_block[j].view==1)
            BlockImage->setZValue(blockZl); // applay lava block Z
        else
            BlockImage->setZValue(blockZ); // applay standart block Z
    }

    BlockImage->setFlag(QGraphicsItem::ItemIsSelectable, (!lock_block));
    BlockImage->setFlag(QGraphicsItem::ItemIsMovable, (!lock_block));

    BlockImage->setData(0, "Block");
    BlockImage->setData(1, QString::number(block.id) );
    BlockImage->setData(2, QString::number(block.array_id) );


    if(pConfigs->main_block[j].sizable)
    {
        BlockImage->setData(3, "sizable" );
    }
    else
        BlockImage->setData(3, "standart" );

    BlockImage->setData(9, QString::number(block.w) ); //width
    BlockImage->setData(10, QString::number(block.h) ); //height
    BlockImage->setScenePoint(this);
    if(PasteFromBuffer) BlockImage->setSelected(true);
}


void LvlScene::placeBGO(LevelBGO &bgo)
{
    int j;
    bool noimage=true, found=false;

    ItemBGO *BGOItem = new ItemBGO;
    bool isUser=false;

    noimage=true;
    isUser=false;

    //Check Index exists
    if(bgo.id < (unsigned int)index_bgo.size())
    {
        j = index_bgo[bgo.id].i;

        if(pConfigs->main_bgo[j].id == bgo.id)
            found=true;
    }

    //if Index found
    if(found)
    {   //get neccesary element directly
        if(index_bgo[bgo.id].type==1)
        {
            isUser=true;
            noimage=false;
            tImg = uBGOs[index_bgo[bgo.id].i].image;
        }
        else
        {
            tImg = pConfigs->main_bgo[index_bgo[bgo.id].i].image;
            noimage=false;
        }
    }
    else
    {
        for(j=0;j<uBGOs.size();j++)
        {
            if(uBGOs[j].id==bgo.id)
            {
                isUser=true;
                noimage=false;
                tImg = uBGOs[j].image;
                break;
            }
        }

        for(j=0;j<pConfigs->main_bgo.size();j++)
        {
            if(pConfigs->main_bgo[j].id==bgo.id)
            {
                noimage=false;
                if(!isUser)
                tImg = pConfigs->main_bgo[j].image; break;
            }
        }
    }

    if((noimage)||(tImg.isNull()))
    {
        tImg=uBgoImg;
    }

    BGOItem->setBGOData(bgo);
        BGOItem->gridSize = pConfigs->main_bgo[j].grid;
        BGOItem->gridOffsetX = pConfigs->main_bgo[j].offsetX;
        BGOItem->gridOffsetY = pConfigs->main_bgo[j].offsetY;
    BGOItem->setMainPixmap(tImg);
    BGOItem->setContextMenu(bgoMenu);
    addItem(BGOItem);

    BGOItem->setPos(bgo.x, bgo.y);

    if((!noimage) && (pConfigs->main_bgo[j].animated))
    {
        //tImg=tImg.copy(0, 0, tImg.width(), (int)round(tImg.height()/pConfigs->main_bgo[j].frames));
        BGOItem->setAnimation(pConfigs->main_bgo[j].frames, pConfigs->main_bgo[j].framespeed);
        BGOItem->setData(4, "animated");
    }

    BGOItem->setFlag(QGraphicsItem::ItemIsSelectable, (!lock_bgo));
    BGOItem->setFlag(QGraphicsItem::ItemIsMovable, (!lock_bgo));

    BGOItem->setData(0, "BGO");
    BGOItem->setData(1, QString::number(bgo.id) );
    BGOItem->setData(2, QString::number(bgo.array_id) );

    BGOItem->setData(9, QString::number(tImg.width()) ); //width
    BGOItem->setData(10, QString::number(tImg.height()) ); //height

    if(pConfigs->main_bgo[j].view!=0)
        BGOItem->setZValue(bgoZf + pConfigs->main_bgo[j].zOffset);
        //bgoback->addToGroup(box);
    else
        BGOItem->setZValue(bgoZb + pConfigs->main_bgo[j].zOffset);
        //bgofore->addToGroup(box);

    BGOItem->setScenePoint(this);

    if(PasteFromBuffer) BGOItem->setSelected(true);
}

void LvlScene::placeNPC(LevelNPC &npc)
{
    QGraphicsItem *	box;
    box = addPixmap(QPixmap(uNpcImg));
    box->setPos(npc.x, npc.y);

    box->setFlag(QGraphicsItem::ItemIsSelectable, (!lock_npc));
    box->setFlag(QGraphicsItem::ItemIsMovable, (!lock_npc));

    //npcfore->addToGroup(box);
    if(npc.id==91)
        box->setZValue(npcZf);
    else
        box->setZValue(npcZb);

    box->setData(0, "NPC"); // ObjType
    box->setData(1, QString::number(npc.id) );
    box->setData(2, QString::number(npc.array_id) );

    box->setData(9, QString::number(uNpcImg.width()) ); //width
    box->setData(10, QString::number(uNpcImg.height()) ); //height

    if(PasteFromBuffer) box->setSelected(true);
}

void LvlScene::sortBlockArray(QVector<LevelBlock > &blocks)
{
    LevelBlock tmp1;
    int total = blocks.size();
    long i;
    long ymin;
    long ymini;
    long sorted = 0;


        while(sorted < blocks.size())
        {
            ymin = blocks[sorted].y;
            ymini = sorted;

            for(i = sorted; i < total; i++)
            {
                if( blocks[i].y < ymin )
                {
                    ymin = blocks[i].y; ymini = i;
                }
            }
            tmp1 = blocks[ymini];
            blocks[ymini] = blocks[sorted];
            blocks[sorted] = tmp1;
            sorted++;
        }
}

void LvlScene::sortBGOArray(QVector<LevelBGO > &bgos)
{
    LevelBGO tmp1;
    int total = bgos.size();
    long i;
    unsigned long ymin;
    unsigned long ymini;
    long sorted = 0;

        while(sorted < bgos.size())
        {
            ymin = bgos[sorted].id;
            ymini = sorted;

            for(i = sorted; i < total; i++)
            {
                if( bgos[i].id < ymin )
                {
                    ymin = bgos[i].id; ymini = i;
                }
            }
            tmp1 = bgos[ymini];
            bgos[ymini] = bgos[sorted];
            bgos[sorted] = tmp1;
            sorted++;
        }
}

/////////////////////SET Block Objects/////////////////////////////////////////////
void LvlScene::setBlocks(LevelData FileData, QProgressDialog &progress)
{
    int i=0;

    //Sort block by Y
    sortBlockArray(FileData.blocks);

    //Applay images to objects
    for(i=0; i<FileData.blocks.size(); i++)
    {
        //  this makes loading slow!!!
        //if(!progress.wasCanceled())
        //    progress.setLabelText("Applayng Blocks "+QString::number(i)+"/"+QString::number(FileData.blocks.size()));

        //Add block to scene
        placeBlock(FileData.blocks[i]);

        if(!progress.wasCanceled())
            progress.setValue(progress.value()+1);
        else return;
    }
}


/////////////////////SET BackGround Objects/////////////////////////////////////////////
void LvlScene::setBGO(LevelData FileData, QProgressDialog &progress)
{
    int i=0;

    //sortBGOArray(FileData.bgo); //Sort BGOs

    //Applay images to objects
    for(i=0; i<FileData.bgo.size(); i++)
    {
        //  this makes loading slow!!!
        //if(!progress.wasCanceled())
        //    progress.setLabelText("Applayng BGOs "+QString::number(i)+"/"+QString::number(FileData.bgo.size()));

        placeBGO(FileData.bgo[i]);

        if(!progress.wasCanceled())
            progress.setValue(progress.value()+1);
        else return;
    }

}




/////////////////////////SET NonPlayble Characters and Items/////////////////////////////////
void LvlScene::setNPC(LevelData FileData, QProgressDialog &progress)
{
    int i=0;
    //QGraphicsItem *	box;

    for(i=0; i<FileData.npc.size(); i++)
    {
        placeNPC(FileData.npc[i]);

        if(!progress.wasCanceled())
            progress.setValue(progress.value()+1);
        else return;
    }

}

/////////////////////////SET Waters/////////////////////////////////
void LvlScene::setWaters(LevelData FileData, QProgressDialog &progress)
{
    int i=0;
    long x, y, h, w;
    QGraphicsItem *	box;

    for(i=0; i<FileData.water.size(); i++)
    {
        //if(!progress.wasCanceled())
        //    progress.setLabelText("Applayng water "+QString::number(i)+"/"+QString::number(FileData.water.size()));

        x = FileData.water[i].x;
        y = FileData.water[i].y;
        h = FileData.water[i].h;
        w = FileData.water[i].w;

        //box = addRect(x, y, w, h, QPen(((FileData.water[i].quicksand)?Qt::yellow:Qt::green), 4), Qt::NoBrush);

        QVector<QPoint > points;
        // {{x, y},{x+w, y},{x+w,y+h},{x, y+h}}
        points.push_back(QPoint(x, y));
        points.push_back(QPoint(x+w, y));
        points.push_back(QPoint(x+w,y+h));
        points.push_back(QPoint(x, y+h));
        points.push_back(QPoint(x, y));

        points.push_back(QPoint(x, y+h));
        points.push_back(QPoint(x+w,y+h));
        points.push_back(QPoint(x+w, y));
        points.push_back(QPoint(x, y));

        box = addPolygon(QPolygon(points), QPen(((FileData.water[i].quicksand)?Qt::yellow:Qt::green), 4));

        box->setFlag(QGraphicsItem::ItemIsSelectable, (!lock_water));
        box->setFlag(QGraphicsItem::ItemIsMovable, (!lock_water));

        box->setZValue(waterZ);

        box->setData(0, "Water"); // ObjType
        box->setData(1, QString::number(0) );
        box->setData(2, QString::number(FileData.water[i].array_id) );

        if(!progress.wasCanceled())
            progress.setValue(progress.value()+1);
        else return;
    }

}


void LvlScene::placeDoor(LevelDoors &door)
{
    long ix, iy, ox, oy, h, w;
    QGraphicsItem *	enter;
    QGraphicsItem *	exit;
    QGraphicsItemGroup *enterId, *exitId;
    QGraphicsTextItem *enterTxt, *enterTxt_l2;
    QGraphicsTextItem *exitTxt, *exitTxt_l2;

    ix = door.ix;
    iy = door.iy;
    ox = door.ox;
    oy = door.oy;
    h = 32;
    w = 32;
    QFont font1, font2;
    font1.setWeight(50);
    font1.setBold(1);
    font1.setPointSize(14);

    font2.setWeight(14);
    font2.setBold(0);
    font2.setPointSize(12);

    //font.setStyle(QFont::Times);
    //font.setStyle();

    QColor cEnter(Qt::magenta);
    QColor cExit(Qt::darkMagenta);
    cEnter.setAlpha(50);
    cExit.setAlpha(50);

    if( ((!door.lvl_o) && (!door.lvl_i)) || ((door.lvl_o) && (!door.lvl_i)) )
    {
        enter = addRect(ix, iy, w, h, QPen(Qt::magenta, 2,Qt::SolidLine), QBrush(cEnter));
        enterId = new QGraphicsItemGroup(enter);

        enterTxt = new QGraphicsTextItem(QString::number(door.array_id));
        enterTxt->setDefaultTextColor(Qt::black);
        enterTxt->setFont(font1);
        enterTxt->setPos(ix-5, iy-2);
        enterTxt_l2 = new QGraphicsTextItem(QString::number(door.array_id));
        enterTxt_l2->setDefaultTextColor(Qt::white);
        enterTxt_l2->setFont(font2);
        enterTxt_l2->setPos(ix-3, iy);

        enterId->addToGroup(enterTxt);
        enterId->addToGroup(enterTxt_l2);
        enter->setFlag(QGraphicsItem::ItemIsSelectable, (!lock_door));
        enter->setFlag(QGraphicsItem::ItemIsMovable, (!lock_door));
        enter->setZValue(doorZ);

        enterTxt->setZValue(doorZ+0.0000001);
        enterTxt_l2->setZValue(doorZ+0.0000002);
        enter->setData(0, "Door_enter"); // ObjType
        enter->setData(1, QString::number(0) );
        enter->setData(2, QString::number(door.array_id) );
    }

    if( ((!door.lvl_o) && (!door.lvl_i)) || ((door.lvl_i)) )
    {
        exit = addRect(ox, oy, w, h, QPen(Qt::darkMagenta, 2,Qt::SolidLine), QBrush(cExit));
        exitId = new QGraphicsItemGroup(exit);

        exitTxt = new QGraphicsTextItem(QString::number(door.array_id));
        exitTxt->setDefaultTextColor(Qt::black);
        exitTxt->setFont(font1);
        exitTxt->setPos(ox+10, oy+8);
        exitTxt_l2 = new QGraphicsTextItem(QString::number(door.array_id));
        exitTxt_l2->setDefaultTextColor(Qt::white);
        exitTxt_l2->setFont(font2);
        exitTxt_l2->setPos(ox+12, oy+10);

        exitId->addToGroup(exitTxt);
        exitId->addToGroup(exitTxt_l2);
        exit->setFlag(QGraphicsItem::ItemIsSelectable, (!lock_door));
        exit->setFlag(QGraphicsItem::ItemIsMovable, (!lock_door));
        exit->setZValue(doorZ);
        exitTxt->setZValue(doorZ+0.0000001);
        exitTxt_l2->setZValue(doorZ+0.0000002);
        exit->setData(0, "Door_exit"); // ObjType
        exit->setData(1, QString::number(0) );
        exit->setData(2, QString::number(door.array_id) );
    }


}

/////////////////////////SET Doors/////////////////////////////////
void LvlScene::setDoors(LevelData FileData, QProgressDialog &progress)
{
    int i=0;


    for(i=0; i<FileData.doors.size(); i++)
    {
        //if(!progress.wasCanceled())
        //    progress.setLabelText("Applayng doors "+QString::number(i)+"/"+QString::number(FileData.doors.size()));

           placeDoor(FileData.doors[i]);

        if(!progress.wasCanceled())
            progress.setValue(progress.value()+1);
        else
            return;
    }

}

//////////////////////////SET Player Points/////////////////////////////////
void LvlScene::setPlayerPoints()
{
    int i=0;

    QGraphicsItem *	player;
    PlayerPoint plr;

    for(i=0; i<LvlData->players.size(); i++)
    {
        plr = LvlData->players[i];
        if((plr.h!=0)||(plr.w!=0)||(plr.x!=0)||(plr.y!=0))
        {
            player = addPixmap(QPixmap(":/player"+QString::number(i+1)+".png"));
            player->setPos(plr.x, plr.y);
            player->setZValue(playerZ);
            player->setData(0, "player"+QString::number(i+1) );
            player->setFlag(QGraphicsItem::ItemIsSelectable, true);
            player->setFlag(QGraphicsItem::ItemIsMovable, true);
        }

    }

}



////////////////////////////////////Animator////////////////////////////////
void LvlScene::startBlockAnimation()
{
    QList<QGraphicsItem*> ItemList = items();
    QGraphicsItem *tmp;
    for (QList<QGraphicsItem*>::iterator it = ItemList.begin(); it != ItemList.end(); it++)
    {
        if(((*it)->data(0)=="Block")&&((*it)->data(4)=="animated"))
        {
            tmp = (*it);
            ((ItemBlock *)tmp)->AnimationStart();
        }
        else
        if(((*it)->data(0)=="BGO")&&((*it)->data(4)=="animated"))
        {
            tmp = (*it);
            ((ItemBGO *)tmp)->AnimationStart();
        }
    }

}

void LvlScene::stopAnimation()
{
    QList<QGraphicsItem*> ItemList = items();
    QGraphicsItem *tmp;
    for (QList<QGraphicsItem*>::iterator it = ItemList.begin(); it != ItemList.end(); it++)
    {
        if(((*it)->data(0)=="Block")&&((*it)->data(4)=="animated"))
        {
            tmp = (*it);
            ((ItemBlock *)tmp)->AnimationStop();
        }
        else
        if(((*it)->data(0)=="BGO")&&((*it)->data(4)=="animated"))
        {
            tmp = (*it);
            ((ItemBGO *)tmp)->AnimationStop();
        }
    }

}




/////////////////////////////////////////////Locks////////////////////////////////
void LvlScene::setLocked(int type, bool lock)
{
    QList<QGraphicsItem*> ItemList = items();
    // setLock
    switch(type)
    {
    case 1://Block
        lock_block = lock;
        break;
    case 2://BGO
        lock_bgo = lock;
        break;
    case 3://NPC
        lock_npc = lock;
        break;
    case 4://Water
        lock_water = lock;
        break;
    case 5://Doors
        lock_door = lock;
        break;
    default: break;
    }

    for (QList<QGraphicsItem*>::iterator it = ItemList.begin(); it != ItemList.end(); it++)
    {
        switch(type)
        {
        case 1://Block
            if((*it)->data(0).toString()=="Block")
            {
                (*it)->setFlag(QGraphicsItem::ItemIsSelectable, (!lock));
                (*it)->setFlag(QGraphicsItem::ItemIsMovable, (!lock));
            }
            break;
        case 2://BGO
            if((*it)->data(0).toString()=="BGO")
            {
                (*it)->setFlag(QGraphicsItem::ItemIsSelectable, (!lock));
                (*it)->setFlag(QGraphicsItem::ItemIsMovable, (!lock));
            }
            break;
        case 3://NPC
            if((*it)->data(0).toString()=="NPC")
            {
                (*it)->setFlag(QGraphicsItem::ItemIsSelectable, (!lock));
                (*it)->setFlag(QGraphicsItem::ItemIsMovable, (!lock));
            }
            break;
        case 4://Water
            if((*it)->data(0).toString()=="Water")
            {
                (*it)->setFlag(QGraphicsItem::ItemIsSelectable, (!lock));
                (*it)->setFlag(QGraphicsItem::ItemIsMovable, (!lock));
            }
            break;
        case 5://Doors
            if(((*it)->data(0).toString()=="Door_enter")||((*it)->data(0).toString()=="Door_exit"))
            {
                (*it)->setFlag(QGraphicsItem::ItemIsSelectable, (!lock));
                (*it)->setFlag(QGraphicsItem::ItemIsMovable, (!lock));
            }
            break;
        default: break;
        }
    }

}

