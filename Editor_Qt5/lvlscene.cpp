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


LvlScene::LvlScene(dataconfigs &configs, LevelData &FileData, QObject *parent) : QGraphicsScene(parent)
{
    //Set the background GraphicsItem's points
    for(int i=0;i<22;i++)
        BgItem.push_back(new QGraphicsPixmapItem);

    pConfigs = &configs; // Pointer to Main Configs

    LvlData = &FileData;
    grid = true;
    EditingMode = 0;
    EraserEnabled = false;
    sbZ = 0;

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
    blockZs = -150; // Sizeble blocks
    bgoZb = -100; // backround BGO
    npcZb = -50; // standart NPC

    blockZ = 1; // standart block

    bgoZf = 50; // foreground BGO

    blockZl = 100;
    npcZf = 150; // foreground NPC
    waterZ = 500;
    doorZ = 700;
    spaceZ1 = 1000; // interSection space layer
    spaceZ2 = 1020; // section Border

    blockMenu.addAction("Invisible");
    blockMenu.addAction("Slippery");
    blockMenu.addAction("Resize");
    blockMenu.addAction("Properties...");


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
        QList<QGraphicsItem*> selectedList = selectedItems();

        cursor->setPos(mouseEvent->scenePos());
        cursor->show();

        if(EditingMode==1) // if Editing Mode = Esaising
        {
            EraserEnabled = true;
        }
        if (!selectedList.isEmpty())
        {
            /*
            for (QList<QGraphicsItem*>::iterator it = selectedList.begin(); it != selectedList.end(); it++)
            {
                // Z.push_back((*it)->zValue());
                // (*it)->setZValue(1000);
            }
            */
        }
        QGraphicsScene::mousePressEvent(mouseEvent);
}


void LvlScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    cursor->setPos(mouseEvent->scenePos());
    QGraphicsItem * findItem;
    if (EraserEnabled) { // Remove All items, placed under Cursor
        findItem = itemCollidesCursor(cursor);
        if(findItem)
        {
            removeItem(findItem);
        }
        QGraphicsScene::mouseMoveEvent(mouseEvent);
    } else
        QGraphicsScene::mouseMoveEvent(mouseEvent);
}



void LvlScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
    {
            int gridSize=32, offsetX=0, offsetY=0, gridX, gridY, i;
            QPoint sourcePos;

            cursor->hide();

            QList<QGraphicsItem*> selectedList = selectedItems();

            QString ObjType;

            // check for grid snap
            if (!selectedList.isEmpty())
            {
                // correct selected items' coordinates
                for (QList<QGraphicsItem*>::iterator it = selectedList.begin(); it != selectedList.end(); it++)
                {
                    if(EraserEnabled)
                    {
                        removeItem((*it)); continue;
                    }

                    gridSize = 32;
                    ObjType = (*it)->data(0).toString();

                    //(*it)->setZValue(Z);
                    if( ObjType == "NPC")
                    {
                        gridSize = 1;
                    }

                    if( ObjType == "BGO")
                    {
                        foreach(obj_bgo findGrid, pConfigs->main_bgo)
                        {
                            if(findGrid.id == (unsigned)(*it)->data(1).toInt() )
                            {
                                gridSize = findGrid.grid; break;
                            }
                        }
                    }

                    if( (*it)->data(0).toString() == "Door_enter")
                        gridSize = 16 ;

                    if( (*it)->data(0).toString() == "Door_exit")
                        gridSize = 16 ;

                    QPointF itemPos = (*it)->scenePos();

                    if(grid)
                    { //ATTACH TO GRID
                        gridX = ((int)itemPos.x() - (int)itemPos.x() % gridSize);
                        gridY = ((int)itemPos.y() - (int)itemPos.y() % gridSize);

                        if((int)itemPos.x()<0)
                        {
                            if( (int)itemPos.x() < gridX - (int)(gridSize/2) )
                                gridX -= gridSize;
                        }
                        else
                        {
                            if( (int)itemPos.x() > gridX + (int)(gridSize/2) )
                                gridX += gridSize;
                        }

                        if((int)itemPos.y()<0)
                        {if( (int)itemPos.y() < gridY - (int)(gridSize/2) )
                            gridY -= gridSize;
                        }
                        else {if( (int)itemPos.y() > gridY + (int)(gridSize/2) )
                         gridY += gridSize;
                        }

                        (*it)->setPos(QPointF(offsetX+gridX, offsetY+gridY));
                    }

                }
                EraserEnabled = false;

                // Check collisions
                for (QList<QGraphicsItem*>::iterator it = selectedList.begin(); it != selectedList.end(); it++)
                {
                    ObjType = (*it)->data(0).toString();
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

                    if( ObjType == "Block")
                    {
                        foreach (LevelBlock findInArr, LvlData->blocks)
                        {
                            if(findInArr.array_id==(unsigned)(*it)->data(2).toInt())
                            {
                                sourcePos = QPoint(findInArr.x, findInArr.y); break;
                            }
                        }
                    }

                    if( ObjType == "BGO")
                    {
                        foreach (LevelBGO findInArr, LvlData->bgo)
                        {
                            if(findInArr.array_id==(unsigned)(*it)->data(2).toInt())
                            {
                                sourcePos = QPoint(findInArr.x, findInArr.y); break;
                            }
                        }
                    }

                        if( itemCollidesWith((*it)) )
                        {
                            (*it)->setPos(QPointF(sourcePos));
                            (*it)->setSelected(false);
                            WriteToLog(QtDebugMsg, QString("Moved back %1 %2")
                                       .arg((long)(*it)->scenePos().x())
                                       .arg((long)(*it)->scenePos().y()) );
                        }
                        else
                        {
                            if( ObjType == "Block")
                            {
                                for (i=0;i<LvlData->blocks.size();i++)
                                {
                                    if(LvlData->blocks[i].array_id==(unsigned)(*it)->data(2).toInt())
                                    {
                                        //Applay move into main array
                                        LvlData->blocks[i].x = (long)(*it)->scenePos().x();
                                        LvlData->blocks[i].y = (long)(*it)->scenePos().y();
                                        LvlData->modyfied = true;
                                        break;
                                    }
                                }
                            } else
                            if( ObjType == "BGO")
                            {
                                for (i=0;i<LvlData->bgo.size();i++)
                                {
                                    if(LvlData->bgo[i].array_id==(unsigned)(*it)->data(2).toInt())
                                    {
                                        //Applay move into main array
                                        LvlData->bgo[i].x = (long)(*it)->scenePos().x();
                                        LvlData->bgo[i].y = (long)(*it)->scenePos().y();
                                        LvlData->modyfied = true;
                                        break;
                                    }
                                }
                            }
                        }
                }


                QGraphicsScene::mouseReleaseEvent(mouseEvent);
                return;
            }
            EraserEnabled = false;
            QGraphicsScene::mouseReleaseEvent(mouseEvent);
    }


QGraphicsItem * LvlScene::itemCollidesWith(QGraphicsItem * item)
{
    qlonglong leftA, leftB;
    qlonglong rightA, rightB;
    qlonglong topA, topB;
    qlonglong bottomA, bottomB;
    qreal betweenZ;

    QList<QGraphicsItem *> collisions = collidingItems(item);
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

              if((item->data(0).toString()=="Block")||(item->data(0).toString()=="NPC")
                      ||(item->data(0).toString()=="BGO"))
              {
                  if(item->data(0).toString()!=it->data(0).toString()) continue;

                  if(item->data(3).toString()=="sizeble")
                  {//Sizeble Block
                      WriteToLog(QtDebugMsg, QString("Sizeble block") );
                      //Change berween Z-Values
                      if(it->data(3).toString()=="sizeble")
                      {
                          WriteToLog(QtDebugMsg, QString("Colliding with Sizeble Z: %1 %2")
                                     .arg(item->parentItem()->zValue()).arg(it->parentItem()->zValue()));

                          if( (item->scenePos().y() > it->scenePos().y()) &&
                          ( item->parentItem()->zValue() <= it->parentItem()->zValue() ) )
                              {
                                betweenZ = it->parentItem()->zValue();
                                it->parentItem()->setZValue(item->parentItem()->zValue());
                                item->parentItem()->setZValue(betweenZ);

                                if(item->parentItem()->zValue() == it->parentItem()->zValue()) item->parentItem()->setZValue(item->parentItem()->zValue() + 0.0000000001);
                               // betweenZ+=0.0000000001;
                                //item->setZValue(it->zValue() + 0.0000000001);

                                WriteToLog(QtDebugMsg, QString("Sizeble block changed Z-") );
                              }
                          else
                          if( (item->scenePos().y() < it->scenePos().y() ) &&
                          ( item->parentItem()->zValue() >= it->parentItem()->zValue() ) )
                              {
                                betweenZ = it->parentItem()->zValue();
                                it->parentItem()->setZValue(item->parentItem()->zValue());
                                item->parentItem()->setZValue(betweenZ);

                                if(item->parentItem()->zValue() == it->parentItem()->zValue()) item->parentItem()->setZValue(item->parentItem()->zValue() - 0.00000000001);
                               // item->setZValue(it->zValue() + 0.0000000001);

                                WriteToLog(QtDebugMsg, QString("Sizeble block changed Z+") );
                              }
                      }
                      continue;
                  }//Sizeble Block

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

                     if(it->data(3).toString()!="sizeble")
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
void LvlScene::loadUserData(LevelData FileData, QProgressDialog &progress, dataconfigs &configs)
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
    for(i=0; i<configs.main_bg.size(); i++) //Add user images
        {
        if(!progress.wasCanceled())
            progress.setLabelText("Search User Backgrounds "+QString::number(i)+"/"+QString::number(configs.main_bg.size()));

            loaded1 = false;
            loaded2 = false;

            //check for first image
            if((QFile::exists(uLVLD) ) &&
                  (QFile::exists(uLVLDs + configs.main_bg[i].image_n)) )
            {
                uBG.image = QPixmap( uLVLDs + configs.main_bg[i].image_n );
                uBG.id = configs.main_bg[i].id;
                loaded1 = true;
            }
            else
            if(QFile::exists(uLVLs + configs.main_bg[i].image_n) )
            {
                uBG.image = QPixmap( uLVLs + configs.main_bg[i].image_n );
                uBG.id = configs.main_bg[i].id;
                loaded1 = true;
            }

            if((loaded1)&&(configs.main_bg[i].animated) )
            {
                uBG.image=uBG.image.copy(0, 0, uBG.image.width(), (int)round(uBG.image.height()/configs.main_bg[i].frames));
            }

            //check for second image
            if(configs.main_bg[i].type == 1)
            {
                if((QFile::exists(uLVLD) ) &&
                      (QFile::exists(uLVLDs + configs.main_bg[i].second_image_n )) )
                {
                    uBG.second_image = QPixmap( uLVLDs + configs.main_bg[i].second_image_n );
                    uBG.id = configs.main_bg[i].id;
                    loaded2 = true;
                }
                else
                if(QFile::exists(uLVLs + configs.main_bg[i].second_image_n) )
                {
                    uBG.second_image = QPixmap( uLVLs + configs.main_bg[i].second_image_n );
                    uBG.id = configs.main_bg[i].id;
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
        }


    //Load Blocks
    for(i=0; i<configs.main_block.size(); i++) //Add user images
    {

        if(!progress.wasCanceled())
            progress.setLabelText("Search User Blocks "+QString::number(i)+"/"+QString::number(configs.main_block.size()));

            if((QFile::exists(uLVLD) ) &&
                  (QFile::exists(uLVLDs + configs.main_block[i].image_n)) )
            {
                if(QFile::exists(uLVLDs + configs.main_block[i].mask_n))
                    uBlock.mask = QBitmap(uLVLDs + configs.main_block[i].mask_n );
                else
                    uBlock.mask = configs.main_block[i].mask;

                uBlock.image = QPixmap(uLVLDs + configs.main_block[i].image_n );

                if((uBlock.image.height()!=uBlock.mask.height())||(uBlock.image.width()!=uBlock.mask.width()))
                    uBlock.mask = uBlock.mask.copy(0,0,uBlock.image.width(),uBlock.image.height());
                uBlock.image.setMask(uBlock.mask);
                uBlock.id = configs.main_block[i].id;
                uBlocks.push_back(uBlock);
            }
            else
            if(QFile::exists(uLVLs + configs.main_block[i].image_n) )
            {
                if(QFile::exists(uLVLs + configs.main_block[i].mask_n))
                    uBlock.mask = QBitmap(uLVLs + configs.main_block[i].mask_n );
                else
                    uBlock.mask = configs.main_block[i].mask;

                uBlock.image = QPixmap(uLVLs + configs.main_block[i].image_n );

                if((uBlock.image.height()!=uBlock.mask.height())||(uBlock.image.width()!=uBlock.mask.width()))
                    uBlock.mask = uBlock.mask.copy(0,0,uBlock.image.width(),uBlock.image.height());

                uBlock.image.setMask(uBlock.mask);
                uBlock.id = configs.main_block[i].id;
                uBlocks.push_back(uBlock);
            }

    if(!progress.wasCanceled())
        progress.setValue(progress.value()+1);
    }

    //Load BGO
    for(i=0; i<configs.main_bgo.size(); i++) //Add user images
    {
        if(!progress.wasCanceled())
            progress.setLabelText("Search User BGOs "+QString::number(i)+"/"+QString::number(configs.main_bgo.size()));

            if((QFile::exists(uLVLD) ) &&
                  (QFile::exists(uLVLDs + configs.main_bgo[i].image_n)) )
            {
                if(QFile::exists(uLVLDs + configs.main_bgo[i].mask_n))
                    uBGO.mask = QBitmap(uLVLDs + configs.main_bgo[i].mask_n );
                else
                    uBGO.mask = configs.main_bgo[i].mask;

                uBGO.image = QPixmap(uLVLDs + configs.main_bgo[i].image_n );

                if((uBGO.image.height()!=uBGO.mask.height())||(uBGO.image.width()!=uBGO.mask.width()))
                    uBGO.mask = uBGO.mask.copy(0,0,uBGO.image.width(),uBGO.image.height());
                uBGO.image.setMask(uBGO.mask);
                uBGO.id = configs.main_bgo[i].id;
                uBGOs.push_back(uBGO);
            }
            else
            if(QFile::exists(uLVLs + configs.main_bgo[i].image_n) )
            {
                if(QFile::exists(uLVLs + configs.main_bgo[i].mask_n))
                    uBGO.mask = QBitmap(uLVLs + configs.main_bgo[i].mask_n );
                else
                    uBGO.mask = configs.main_bgo[i].mask;

                uBGO.image = QPixmap(uLVLs + configs.main_bgo[i].image_n );

                if((uBGO.image.height()!=uBGO.mask.height())||(uBGO.image.width()!=uBGO.mask.width()))
                    uBGO.mask = uBGO.mask.copy(0,0,uBGO.image.width(),uBGO.image.height());

                uBGO.image.setMask(uBGO.mask);
                uBGO.id = configs.main_bgo[i].id;
                uBGOs.push_back(uBGO);
            }
    if(!progress.wasCanceled())
        progress.setValue(progress.value()+1);
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
    drawing.push_back(QPoint(l, t));
    drawing.push_back(QPoint(r, t));
    drawing.push_back(QPoint(r, b));
    drawing.push_back(QPoint(l, b));
    drawing.push_back(QPoint(l, t));

    bigSpace = bigSpace.subtracted(QPolygon(drawing));

    WriteToLog(QtDebugMsg, QString("add polygon to Item"));
    item = addPolygon(bigSpace, QPen(Qt::NoPen), QBrush(Qt::black));//Add inactive space
    item2 = addPolygon(QPolygon(drawing), QPen(Qt::red, 4));
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

        //configs.main_bg[j].type;

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
    FileData.modyfied = true;

    WriteToLog(QtDebugMsg, "set Background to "+QString::number(BG_Id));
    setSectionBG(FileData.sections[FileData.CurSection]);
}



//////////////////Block////////////////////////////////////////////////////////////////////////////////////////

void LvlScene::placeBox(float x, float y)
{
    QGraphicsItem *	box = addPixmap(QPixmap(QApplication::applicationDirPath() + "/" + "data/graphics/level/block/block-8.gif"));
    box->setPos(x, y);
    if (itemCollidesWith(box))
        removeItem(box);
    else
        box->setData(0, "Box");
}

void LvlScene::placeBlock(LevelBlock block, dataconfigs &configs)
{
    bool noimage=true;//, found=false;
    bool isUser=false;
    int j;
    //QPixmap img;

    QGraphicsItem *box, *npc;
    QGraphicsItemGroup *includedNPC;

    //ItemBlock *BlockImage;
    ItemBlock *BlockImage = new ItemBlock;

    noimage=true;
    isUser=false;

    for(j=0;j<uBlocks.size();j++)
    {
        if(uBlocks[j].id==block.id)
        {
            isUser=true;
            tImg = uBlocks[j].image;
            break;
        }
    }

    for(j=0;j<configs.main_block.size();j++)
    {
        if(configs.main_block[j].id==block.id)
        {
            noimage=false;
            if(!isUser)
            tImg = configs.main_block[j].image; break;
        }
    }
    if((noimage)||(tImg.isNull()))
    {
        tImg=uBlockImg;
    }

    box = addPixmap(QPixmap(QSize(0,0)));

    if(!configs.main_block[j].sizeble)
    {
        BlockImage->setMainPixmap(tImg);
    }
    else
    {
        BlockImage->setMainPixmap( QPixmap( drawSizebleBlock(block.w, block.h, tImg) ));
    }

    //box = new QGraphicsItem;
    BlockImage->setParentItem(box);
    box->update();
    BlockImage->setContextMenu(blockMenu);

    if((!noimage) && (configs.main_block[j].animated))
    {
        BlockImage->setAnimation(tImg, configs.main_block[j].frames);
        /*
        if(configs.main_block[j].algorithm==1)
            tImg = tImg.copy(0, ((int)round(tImg.height()/configs.main_block[j].frames))*4,
                       tImg.width(), (int)round(tImg.height()/configs.main_block[j].frames));

        else*/
            //tImg=tImg.copy(0, 0, tImg.width(), (int)round(tImg.height()/configs.main_block[j].frames));
    }


    includedNPC = new QGraphicsItemGroup(BlockImage);
    //addToGroup(box);

    if(block.invisible)
        BlockImage->setOpacity(qreal(0.5));

    BlockImage->setPos(block.x, block.y);

    //QPainter npc_p(&img);
    if(block.npc_id != 0)
    {
        npc = addPixmap( QPixmap(uNpcImg) );
        npc->setPos(block.x, block.y);
        npc->setZValue(blockZ);
        npc->setOpacity(qreal(0.4));
        includedNPC->addToGroup(npc);
    }

    if(configs.main_block[j].sizeble)
    {
        box->setZValue(blockZs+sbZ); // applay Sizeble block Z
        sbZ += 0.0000000001;
    }
    else
    {
    if(configs.main_block[j].view==1)
        box->setZValue(blockZl); // applay lava block Z
    else
        box->setZValue(blockZ); // applay standart block Z
    }

    /*QList<QGraphicsItem* > blockData;
    blockData.push_back(box);
    blockData.push_back(npc);

    BlockGr = createItemGroup( blockData );
    */
    BlockImage->setFlag(QGraphicsItem::ItemIsSelectable, true);
    BlockImage->setFlag(QGraphicsItem::ItemIsMovable, true);
    //box->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    BlockImage->setData(0, "Block");
    BlockImage->setData(1, QString::number(block.id) );
    BlockImage->setData(2, QString::number(block.array_id) );

    if(configs.main_block[j].sizeble)
    {
        BlockImage->setData(3, "sizeble" );
        //GrpBlocksSz->addToGroup(box);
    }
    else
        BlockImage->setData(3, "standart" );

    BlockImage->setData(9, QString::number(block.w) ); //width
    BlockImage->setData(10, QString::number(block.h) ); //height

    //GrpBlocks->addToGroup(box);

}

//Sizeble Block formula
QPixmap LvlScene::drawSizebleBlock(int w, int h, QPixmap srcimg)
{
    int x,y, i, j;
    int hc, wc;
    QPixmap img;
    QPixmap * sizebleImage;
    QPainter * szblock;
    x=32;
    y=32;

    sizebleImage = new QPixmap(QSize(w, h));
    sizebleImage->fill(Qt::transparent);
    szblock = new QPainter(sizebleImage);

    //L
    hc=0;
    for(i=0; i<((h-2*y) / y); i++ )
    {
        szblock->drawPixmap(0, x+hc, x, y, srcimg.copy(QRect(0, y, x, y)));
            hc+=x;
    }

    //T
    hc=0;
    for(i=0; i<( (w-2*x) / x); i++ )
    {
        szblock->drawPixmap(x+hc, 0, x, y, srcimg.copy(QRect(x, 0, x, y)) );
            hc+=x;
    }

    //B
    hc=0;
    for(i=0; i< ( (w-2*x) / x); i++ )
    {
        szblock->drawPixmap(x+hc, h-y, x, y, srcimg.copy(QRect(x, srcimg.width()-y, x, y )) );
            hc+=x;
    }

    //R
    hc=0;
    for(i=0; i<((h-2*y) / y); i++ )
    {
        szblock->drawPixmap(w-x, y+hc, x, y, srcimg.copy(QRect(srcimg.width()-x, y, x, y)));
            hc+=x;
    }

    //C
    hc=0;
    wc=0;
    for(i=0; i<((h-2*y) / y); i++ )
    {
        hc=0;
        for(j=0; j<((w-2*x) / x); j++ )
        {
        szblock->drawPixmap(x+hc, y+wc, x, y, srcimg.copy(QRect(x, y, x, y)));
            hc+=x;
        }
        wc+=y;
    }

    //Applay Sizeble formula
     //1
    szblock->drawPixmap(0,0,y,x, srcimg.copy(QRect(0,0,y,x)));
     //2
    szblock->drawPixmap(w-y, 0, y, x, srcimg.copy(QRect(srcimg.width()-y, 0, y, x)) );
     //3
    szblock->drawPixmap(w-y, h-x, y, x, srcimg.copy(QRect(srcimg.width()-y, srcimg.height()-x, y, x)) );
     //4
    szblock->drawPixmap(0, h-x, y, x, srcimg.copy(QRect(0, srcimg.height()-x, y, x)) );

    img = QPixmap( * sizebleImage);
    return img;
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
void LvlScene::setBlocks(LevelData FileData, QProgressDialog &progress, dataconfigs &configs)
{
    int i=0;

    //Sort block by Y
    sortBlockArray(FileData.blocks);

    //Applay images to objects
    for(i=0; i<FileData.blocks.size(); i++)
    {
        if(!progress.wasCanceled())
            progress.setLabelText("Applayng Blocks "+QString::number(i)+"/"+QString::number(FileData.blocks.size()));

        //Add block to scene
        placeBlock(FileData.blocks[i], configs);

        if(!progress.wasCanceled())
            progress.setValue(progress.value()+1);
    }
}


/////////////////////SET BackGround Objects/////////////////////////////////////////////
void LvlScene::setBGO(LevelData FileData, QProgressDialog &progress, dataconfigs &configs)
{
    int i=0, j;
    bool noimage=true;//, found=false;
    //QPixmap tImg;

    //QBitmap mask;
    //QBitmap mask = QBitmap(QApplication::applicationDirPath() + "/" + "data/unknown_npcm.gif");
    //QPixmap image = QPixmap(QApplication::applicationDirPath() + "/" + "data/unknown_npc.gif");

    QGraphicsItem *	box;
    bool isUser=false;

    //sortBGOArray(FileData.bgo); //Sort BGOs

    //Applay images to objects
    for(i=0; i<FileData.bgo.size(); i++)
    {
        if(!progress.wasCanceled())
            progress.setLabelText("Applayng BGOs "+QString::number(i)+"/"+QString::number(FileData.bgo.size()));

        noimage=true;
        isUser=false;
        for(j=0;j<uBGOs.size();j++)
        {
            if(uBGOs[j].id==FileData.bgo[i].id)
            {
                isUser=true;
                tImg = uBGOs[j].image;
                break;
            }
        }

        for(j=0;j<configs.main_bgo.size();j++)
        {
            if(configs.main_bgo[j].id==FileData.bgo[i].id)
            {
                noimage=false;
                if(!isUser)
                tImg = configs.main_bgo[j].image; break;
            }
        }
        if((noimage)||(tImg.isNull()))
        {
            tImg=uBgoImg;
        }

        if((!noimage) && (configs.main_bgo[j].animated))
        {
            tImg=tImg.copy(0, 0, tImg.width(), (int)round(tImg.height()/configs.main_bgo[j].frames));
        }

        box = addPixmap(QPixmap(tImg));
        box->setPos(FileData.bgo[i].x, FileData.bgo[i].y);

        box->setFlag(QGraphicsItem::ItemIsSelectable, true);
        box->setFlag(QGraphicsItem::ItemIsMovable, true);

        box->setData(0, "BGO");
        box->setData(1, QString::number(FileData.bgo[i].id) );
        box->setData(2, QString::number(FileData.bgo[i].array_id) );

        box->setData(9, QString::number(tImg.width()) ); //width
        box->setData(10, QString::number(tImg.height()) ); //height

        if(configs.main_bgo[j].view!=0)
            box->setZValue(bgoZf + configs.main_bgo[j].zOffset);
            //bgoback->addToGroup(box);
        else
            box->setZValue(bgoZb + configs.main_bgo[j].zOffset);
            //bgofore->addToGroup(box);

        if(!progress.wasCanceled())
            progress.setValue(progress.value()+1);
    }

}




/////////////////////////SET NonPlayble Characters and Items/////////////////////////////////
void LvlScene::setNPC(LevelData FileData, QProgressDialog &progress)
{
    int i=0;
    QGraphicsItem *	box;

    for(i=0; i<FileData.npc.size(); i++)
    {
        box = addPixmap(QPixmap(uNpcImg));
        box->setPos(FileData.npc[i].x, FileData.npc[i].y);
        box->setFlag(QGraphicsItem::ItemIsSelectable,true);
        box->setFlag(QGraphicsItem::ItemIsMovable, true);
        //npcfore->addToGroup(box);
        if(FileData.npc[i].id==91)
            box->setZValue(npcZf);
        else
            box->setZValue(npcZb);

        box->setData(0, "NPC"); // ObjType
        box->setData(1, QString::number(FileData.npc[i].id) );
        box->setData(2, QString::number(FileData.npc[i].array_id) );

        box->setData(9, QString::number(uNpcImg.width()) ); //width
        box->setData(10, QString::number(uNpcImg.height()) ); //height

        if(!progress.wasCanceled())
            progress.setValue(progress.value()+1);
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
        if(!progress.wasCanceled())
            progress.setLabelText("Applayng water "+QString::number(i)+"/"+QString::number(FileData.water.size()));

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

        box->setFlag(QGraphicsItem::ItemIsSelectable,true);
        box->setFlag(QGraphicsItem::ItemIsMovable, true);

        box->setZValue(waterZ);

        box->setData(0, "Water"); // ObjType
        box->setData(1, QString::number(0) );
        box->setData(2, QString::number(FileData.water[i].array_id) );

        if(!progress.wasCanceled())
            progress.setValue(progress.value()+1);
    }

}

/////////////////////////SET Doors/////////////////////////////////
void LvlScene::setDoors(LevelData FileData, QProgressDialog &progress)
{
    int i=0;
    long ix, iy, ox, oy, h, w;
    QGraphicsItem *	enter;
    QGraphicsItem *	exit;

    for(i=0; i<FileData.doors.size(); i++)
    {
        if(!progress.wasCanceled())
            progress.setLabelText("Applayng doors "+QString::number(i)+"/"+QString::number(FileData.doors.size()));

        ix = FileData.doors[i].ix;
        iy = FileData.doors[i].iy;
        ox = FileData.doors[i].ox;
        oy = FileData.doors[i].oy;
        h = 32;
        w = 32;

        enter = addRect(ix, iy, w, h, QPen(Qt::magenta, 4), Qt::NoBrush);
        exit = addRect(ox, oy, w, h, QPen(Qt::magenta, 4), Qt::NoBrush);

        enter->setFlag(QGraphicsItem::ItemIsSelectable,true);
        enter->setFlag(QGraphicsItem::ItemIsMovable, true);
        exit->setFlag(QGraphicsItem::ItemIsSelectable,true);
        exit->setFlag(QGraphicsItem::ItemIsMovable, true);

        enter->setZValue(doorZ);
        exit->setZValue(doorZ);

        enter->setData(0, "Door_enter"); // ObjType
        enter->setData(1, QString::number(0) );
        enter->setData(2, QString::number(FileData.doors[i].array_id) );

        exit->setData(0, "Door_exit"); // ObjType
        exit->setData(1, QString::number(0) );
        exit->setData(2, QString::number(FileData.doors[i].array_id) );

        if(!progress.wasCanceled())
            progress.setValue(progress.value()+1);
    }

}

