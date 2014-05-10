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

#include "lvlscene.h"
#include "../leveledit.h"

#include "item_block.h"
#include "item_bgo.h"


QPoint LvlScene::applyGrid(QPoint source, int gridSize, QPoint gridOffset)
{
    int gridX, gridY;
    if(grid)
    { //ATTACH TO GRID
        gridX = ((int)source.x() - (int)source.x() % gridSize);
        gridY = ((int)source.y() - (int)source.y() % gridSize);

        if((int)source.x()<0)
        {
            if( (int)source.x() < gridOffset.x()+gridX - (int)(gridSize/2) )
                gridX -= gridSize;
        }
        else
        {
            if( (int)source.x() > gridOffset.x()+gridX + (int)(gridSize/2) )
                gridX += gridSize;
        }

        if((int)source.y()<0)
        {if( (int)source.y() < gridOffset.y()+gridY - (int)(gridSize/2) )
            gridY -= gridSize;
        }
        else {if( (int)source.y() > gridOffset.y()+gridY + (int)(gridSize/2) )
         gridY += gridSize;
        }

        return QPoint(gridOffset.x()+gridX, gridOffset.y()+gridY);

    }
    else
        return source;
}





// //////////////////////////////// Place new ////////////////////////////////

void LvlScene::placeBlock(LevelBlock &block, bool toGrid)
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

    QPoint newPos = QPoint(block.x, block.y);
    if(toGrid)
    {
        newPos = applyGrid(QPoint(block.x, block.y), 32);
        block.x = newPos.x();
        block.y = newPos.y();
    }

    BlockImage->setPos(QPointF(newPos));

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


void LvlScene::placeBGO(LevelBGO &bgo, bool toGrid)
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



    QPoint newPos = QPoint(bgo.x, bgo.y);
    if(toGrid)
    {
        newPos = applyGrid(QPoint(bgo.x, bgo.y), pConfigs->main_bgo[j].grid,
                           QPoint(pConfigs->main_bgo[j].offsetX, pConfigs->main_bgo[j].offsetY));
        bgo.x = newPos.x();
        bgo.y = newPos.y();
    }

    BGOItem->setPos( QPointF(newPos) );

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

void LvlScene::placeNPC(LevelNPC &npc, bool toGrid)
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


void LvlScene::placeDoor(LevelDoors &door, bool toGrid)
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


void LvlScene::placeWater(LevelWater &water, bool toGrid)
{
    long x, y, h, w;
    QGraphicsItem *	box;
    //if(!progress.wasCanceled())
    //    progress.setLabelText("Applayng water "+QString::number(i)+"/"+QString::number(FileData.water.size()));

    QPoint newPos = QPoint(water.x, water.y);

    if(toGrid)
    {
        newPos = applyGrid(QPoint(water.x, water.y), 16);
        water.x = newPos.x();
        water.y = newPos.y();
    }

    x = newPos.x();
    y = newPos.y();
    h = water.h;
    w = water.w;

    //box = addRect(x, y, w, h, QPen(((water.quicksand)?Qt::yellow:Qt::green), 4), Qt::NoBrush);

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

    box = addPolygon(QPolygon(points), QPen(((water.quicksand)?Qt::yellow:Qt::green), 4));

    box->setFlag(QGraphicsItem::ItemIsSelectable, (!lock_water));
    box->setFlag(QGraphicsItem::ItemIsMovable, (!lock_water));

    box->setZValue(waterZ);

    box->setData(0, "Water"); // ObjType
    box->setData(1, QString::number(0) );
    box->setData(2, QString::number(water.array_id) );

}
