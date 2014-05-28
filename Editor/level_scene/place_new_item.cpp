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
#include "../edit_level/leveledit.h"

#include "item_block.h"
#include "item_bgo.h"
#include "item_npc.h"
#include "item_water.h"

#include "lvl_item_placing.h"


//Default dataSets
LevelNPC    LvlPlacingItems::npcSet={
    0, 0, 0,  0, 0,false, 0, 0, 0, "", false,false,
    false,"Default","", "","","", "",0,0
};
LevelBlock  LvlPlacingItems::blockSet=
{
    0,0,0,0,0,0,false,false,
    "Default","","","",0,0
};
LevelBGO    LvlPlacingItems::bgoSet=
{
    0,0,0,"Default",0,0,0
};
LevelWater  LvlPlacingItems::waterSet=
{
    0,0,0,0,0,false,"Default",0,0
};

int LvlPlacingItems::doorType=LvlPlacingItems::DOOR_Entrance;
int LvlPlacingItems::waterType=0; //0 - Water, 1 - QuickSand
int LvlPlacingItems::playerID=0;

int LvlPlacingItems::gridSz=1;
QPoint LvlPlacingItems::gridOffset=QPoint(0,0);


void LvlScene::setItemPlacer(int itemType, unsigned long itemID, int dType)
{
    if(cursor)
        {delete cursor;
        cursor=NULL;}

    WriteToLog(QtDebugMsg, QString("ItemPlacer -> set to type-%1 for ID-%2").arg(itemType).arg(itemID));

    switch(itemType)
    {
    case 0: //blocks
        {
            placingItem=PLC_Block;

            bool noimage=true, found=false;
            bool isUser=false;
            int j;

            noimage=true;
            isUser=false;

            //Check Index exists
            if(itemID < (unsigned int)index_blocks.size())
            {
                j = index_blocks[itemID].i;

                if(j<pConfigs->main_block.size())
                {
                if(pConfigs->main_block[j].id == itemID)
                    found=true;
                }
            }

            //if Index found
            if(found)
            {   //get neccesary element directly
                WriteToLog(QtDebugMsg, QString("ItemPlacer -> Found by Index %1").arg(itemID));
                if(index_blocks[itemID].type==1)
                {
                    isUser=true;
                    noimage=false;
                    tImg = uBlocks[index_blocks[itemID].i].image;
                }
                else
                {
                    tImg = pConfigs->main_block[index_blocks[itemID].i].image;
                    noimage=false;
                }
            }
            else
            {
                //found neccesary element in arrays and select
                for(j=0;j<uBlocks.size();j++)
                {
                    if(uBlocks[j].id == itemID)
                    {
                        isUser=true;
                        noimage=false;
                        tImg = uBlocks[j].image;
                        break;
                    }
                }

                for(j=0;j<pConfigs->main_block.size();j++)
                {
                    if(pConfigs->main_block[j].id==itemID)
                    {
                        noimage=false;
                        if(!isUser)
                            tImg = pConfigs->main_block[j].image; break;
                    }
                }
                WriteToLog(QtDebugMsg, QString("ItemPlacer -> Found by Fetch %1").arg(j));
            }

            if((noimage)||(tImg.isNull()))
            {
                tImg = uBlockImg;
            }

            LvlPlacingItems::gridSz=32;
            LvlPlacingItems::gridOffset = QPoint(0, 0);

            LvlPlacingItems::blockSet.id = itemID;

            LvlPlacingItems::blockSet.w = tImg.width();
            LvlPlacingItems::blockSet.h = tImg.height()/
                            ( (pConfigs->main_block[j].animated)?
                                pConfigs->main_block[j].frames:1
                            );
            LvlPlacingItems::blockSet.layer = "Default";

            if(pConfigs->main_block[j].sizable)
            {
                setSquareDrawer(); return;
            }

            cursor = addPixmap(tImg.copy(0,0,
                                         LvlPlacingItems::blockSet.w,
                                         LvlPlacingItems::blockSet.h
                                         )
                               );

            cursor->setData(0, "Block");
            if(pConfigs->main_block[j].sizable) cursor->setData(3, "sizable");
            cursor->setData(9, QString::number(LvlPlacingItems::blockSet.w));
            cursor->setData(10, QString::number(LvlPlacingItems::blockSet.h));
            cursor->setData(25, "CURSOR");
            cursor->setZValue(7000);
            cursor->setOpacity( 0.8 );
            cursor->setVisible(true);
            cursor->setEnabled(true);
            break;
        }
    case 1: //bgos
    {
        int j;
        bool noimage=true, found=false;
        bool isUser=false;

        noimage=true;
        isUser=false;

        //Check Index exists
        if(itemID < (unsigned int)index_bgo.size())
        {
            j = index_bgo[itemID].i;

            if(j<pConfigs->main_bgo.size())
            {
            if(pConfigs->main_bgo[j].id == itemID)
                found=true;
            }
        }

        //if Index found
        if(found)
        {   //get neccesary element directly
            if(index_bgo[itemID].type==1)
            {
                isUser=true;
                noimage=false;
                tImg = uBGOs[index_bgo[itemID].i].image;
            }
            else
            {
                tImg = pConfigs->main_bgo[index_bgo[itemID].i].image;
                noimage=false;
            }
        }
        else
        {
            //fetching arrays
            for(j=0;j<uBGOs.size();j++)
            {
                if(uBGOs[j].id==itemID)
                {
                    isUser=true;
                    noimage=false;
                    tImg = uBGOs[j].image;
                    break;
                }
            }

            for(j=0;j<pConfigs->main_bgo.size();j++)
            {
                if(pConfigs->main_bgo[j].id==itemID)
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


        LvlPlacingItems::gridSz=pConfigs->main_bgo[j].grid;
        LvlPlacingItems::gridOffset = QPoint(pConfigs->main_bgo[j].offsetX,
                                             pConfigs->main_bgo[j].offsetY);

        LvlPlacingItems::bgoSet.id = itemID;


        long w = tImg.width();
        long h = tImg.height()/( (pConfigs->main_bgo[j].animated)?pConfigs->main_bgo[j].frames:1);

        cursor = addPixmap(tImg.copy(0,0,w,h));

        cursor->setData(0, "BGO");
        cursor->setData(1, QString::number(itemID));
        cursor->setData(9, QString::number(w));
        cursor->setData(10, QString::number(h));
        cursor->setData(25, "CURSOR");
        cursor->setZValue(7000);
        cursor->setOpacity( 0.8 );
        cursor->setVisible(true);
        cursor->setEnabled(true);

        placingItem=PLC_BGO;
        LvlPlacingItems::bgoSet.id = itemID;
        break;
    }
    case 2: //npcs
    {
        int j;
        bool found=false;
        bool isUserTxt=false;
        obj_npc mergedSet;

        placingItem=PLC_NPC;
        LvlPlacingItems::npcSet.id = itemID;

        if(itemID < (unsigned int)index_npc.size())
        {
            j = index_npc[itemID].gi;

            if(j<pConfigs->main_npc.size())
            {
            if(pConfigs->main_npc[j].id == itemID)
                found=true;
            }
        }

        //if Index found
        if(found)
        {   //get neccesary element directly
            if(index_npc[itemID].type==1)
            {
                if(uNPCs[index_npc[itemID].i].withTxt)
                {
                    isUserTxt=true;
                    mergedSet = uNPCs[index_npc[itemID].i].merged;
                }
                else
                    mergedSet = pConfigs->main_npc[index_npc[itemID].gi];
            }
        }
        else
        {
            //fetching arrays
            for(j=0;j<uNPCs.size();j++)
            {
                if(uNPCs[j].id==itemID)
                {
                    if(uNPCs[j].withTxt)
                    {
                        isUserTxt=true;
                        mergedSet = uNPCs[j].merged;
                    }
                    break;
                }
            }

            for(j=0;j<pConfigs->main_npc.size();j++)
            {
                if(pConfigs->main_npc[j].id==itemID)
                {
                    if(!isUserTxt)
                        mergedSet = pConfigs->main_npc[j];
                    break;
                }
            }
        }

        tImg = getNPCimg(itemID);

        LvlPlacingItems::gridSz=mergedSet.grid;
        LvlPlacingItems::gridOffset = QPoint(mergedSet.grid_offset_x,
                                             mergedSet.grid_offset_y);

        cursor = addPixmap(tImg);

        long imgOffsetX = (int)round( - ( ( (double)mergedSet.gfx_w -
                                            (double)mergedSet.width ) / 2 ) );

        long imgOffsetY = (int)round( - (double)mergedSet.gfx_h +
                                      (double)mergedSet.height +
                                      (double)mergedSet.gfx_offset_y );

        ((QGraphicsPixmapItem *)cursor)->setOffset(
                    imgOffsetX+(-((double)mergedSet.gfx_offset_x)*
                                LvlPlacingItems::npcSet.direct), imgOffsetY );

        cursor->setData(0, "NPC");
        cursor->setData(1, QString::number(itemID));
        cursor->setData(9, QString::number(mergedSet.width));
        cursor->setData(10, QString::number(mergedSet.height));
        cursor->setData(25, "CURSOR");
        cursor->setZValue(7000);
        cursor->setOpacity( 0.8 );
        cursor->setVisible(true);
        cursor->setEnabled(true);

        break;
    }
    case 3: //water
        placingItem=PLC_Water;
        LvlPlacingItems::waterType = itemID;
        LvlPlacingItems::gridSz = 16;
        LvlPlacingItems::gridOffset = QPoint(0,0);
        setSquareDrawer(); return;
        break;
    case 4: //doorPoint
        placingItem=PLC_Door;
        LvlPlacingItems::doorType = dType;
        break;
    case 5: //PlayerPoint
        placingItem=PLC_PlayerPoint;
        LvlPlacingItems::playerID = itemID;

        LvlPlacingItems::gridSz=2;
        LvlPlacingItems::gridOffset = QPoint(0,2);

        cursor = addPixmap(QString(":/player%1.png").arg(itemID+1));
        cursor->setData(25, "CURSOR");
        cursor->setZValue(7000);
        cursor->setOpacity( 0.8 );
        cursor->setVisible(true);
        cursor->setEnabled(true);

        break;
        default: break;
    }
    EditingMode = MODE_PlacingNew;
    DrawMode=true;
    contextMenuOpened=false;
}



void LvlScene::setSquareDrawer()
{
    if(cursor)
        {delete cursor;
        cursor=NULL;}

    QPen pen;
    QBrush brush;

    switch(placingItem)
    {
    case PLC_Water:
        if(LvlPlacingItems::waterType==1)
        {
            pen = QPen(Qt::yellow, 2);
            brush = QBrush(Qt::darkYellow);
        }
        else
        {
            pen = QPen(Qt::green, 2);
            brush = QBrush(Qt::darkGreen);
        }
        break;
    case PLC_Block:
    default:
        pen = QPen(Qt::gray, 2);
        brush = QBrush(Qt::darkGray);
        break;
    }

    cursor = addRect(0,0,1,1, pen, brush);

    cursor->setData(0, "Square");
    cursor->setData(25, "CURSOR");
    cursor->setZValue(7000);
    cursor->setOpacity( 0.5 );
    cursor->setVisible(false);
    cursor->setEnabled(true);

    EditingMode = MODE_DrawSquare;
    DrawMode=true;
}


void LvlScene::resetCursor()
{
    if(cursor)
        {delete cursor;
        cursor=NULL;}

    DrawMode=false;
    QPixmap cur(QSize(1,1));
    cur.fill(Qt::black);
    cursor = addPixmap(QPixmap(cur));
    cursor->setZValue(1000);
    cursor->hide();
}
