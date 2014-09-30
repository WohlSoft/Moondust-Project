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
#include "../edit_level/level_edit.h"

#include "../file_formats/file_formats.h"
#include "item_block.h"
#include "item_bgo.h"
#include "item_npc.h"
#include "item_water.h"

#include "lvl_item_placing.h"

#include "../common_features/items.h"
#include "../common_features/themes.h"

/*
    static LevelNPC dummyLvlNpc();
    static LevelBlock dummyLvlBlock();
    static LevelBGO dummyLvlBgo();
    static LevelWater dummyLvlWater();
    static LevelEvents dummyLvlEvent();
*/

//Default dataSets
LevelNPC    LvlPlacingItems::npcSet=FileFormats::dummyLvlNpc();
long        LvlPlacingItems::npcGfxOffsetX1=0;
long        LvlPlacingItems::npcGfxOffsetX2=0;
long        LvlPlacingItems::npcGfxOffsetY=0;
long        LvlPlacingItems::npcGrid=0;
LevelBlock  LvlPlacingItems::blockSet=FileFormats::dummyLvlBlock();
LevelBGO    LvlPlacingItems::bgoSet=FileFormats::dummyLvlBgo();
long        LvlPlacingItems::itemW = 0;
long        LvlPlacingItems::itemH = 0;

LevelPhysEnv  LvlPlacingItems::waterSet=FileFormats::dummyLvlPhysEnv();

int LvlPlacingItems::doorType=LvlPlacingItems::DOOR_Entrance;
long LvlPlacingItems::doorArrayId = 0;

int LvlPlacingItems::c_offset_x=0;
int LvlPlacingItems::c_offset_y=0;

int LvlPlacingItems::waterType=0; //0 - Water, 1 - QuickSand
int LvlPlacingItems::playerID=0;

int LvlPlacingItems::gridSz=1;
QPoint LvlPlacingItems::gridOffset=QPoint(0,0);

bool LvlPlacingItems::sizableBlock=false;
bool LvlPlacingItems::fillingMode=false;
bool LvlPlacingItems::lineMode=false;
bool LvlPlacingItems::overwriteMode=false;

QString LvlPlacingItems::layer="";

QList<QPair<int, QVariant > > LvlPlacingItems::flags;

void LvlScene::setItemPlacer(int itemType, unsigned long itemID, int dType)
{
    if(cursor)
        {delete cursor;
        cursor=NULL;}

    WriteToLog(QtDebugMsg, QString("ItemPlacer -> set to type-%1 for ID-%2").arg(itemType).arg(itemID));

    LvlPlacingItems::sizableBlock=false;

    switch(itemType)
    {
    case 0: //blocks
        {
            placingItem=PLC_Block;

            long j;

            tImg = Items::getItemGFX(ItemTypes::LVL_Block, itemID, false, &j);

            if(tImg.isNull())
            {
                tImg = uBlockImg;
            }

            LvlPlacingItems::gridSz=pConfigs->main_block[j].grid;
            LvlPlacingItems::gridOffset = QPoint(0, 0);

            LvlPlacingItems::blockSet.id = itemID;

            LvlPlacingItems::blockSet.w = tImg.width();
            LvlPlacingItems::blockSet.h = tImg.height();
            LvlPlacingItems::blockSet.layer = "Default";

            //Place sizable blocks in the square fill mode
            if(pConfigs->main_block[j].sizable)
            {
                LvlPlacingItems::sizableBlock=true;
                LvlPlacingItems::fillingMode=false;
                LvlPlacingItems::lineMode=false;
                setSquareDrawer(); return;
            }

            LvlPlacingItems::itemW = LvlPlacingItems::blockSet.w;
            LvlPlacingItems::itemH = LvlPlacingItems::blockSet.h;

            LvlPlacingItems::flags.clear();
            QPair<int, QVariant > flag;

                flag.first=0;
                flag.second="Block";
            LvlPlacingItems::flags.push_back(flag);

                flag.first=1;
                flag.second=QString::number(LvlPlacingItems::blockSet.id);
            LvlPlacingItems::flags.push_back(flag);

            if(pConfigs->main_block[j].sizable)
            {
                flag.first=3;
                flag.second = "sizable";
            LvlPlacingItems::flags.push_back(flag);
            }

                flag.first = 9;
                flag.second = QString::number(LvlPlacingItems::blockSet.w);
            LvlPlacingItems::flags.push_back(flag);

                flag.first = 10;
                flag.second = QString::number(LvlPlacingItems::blockSet.h);
            LvlPlacingItems::flags.push_back(flag);

                flag.first = 25;
                flag.second = "CURSOR";
            LvlPlacingItems::flags.push_back(flag);


            //Square fill mode
            if(LvlPlacingItems::fillingMode)
            {
                setSquareDrawer(); return;
            }

            LvlPlacingItems::c_offset_x= qRound(qreal(LvlPlacingItems::blockSet.w) / 2);
            LvlPlacingItems::c_offset_y= qRound(qreal(LvlPlacingItems::blockSet.h) / 2);

            //Line mode
            if(LvlPlacingItems::lineMode)
            {
                setLineDrawer(); return;
            }

            //Single item placing
            cursor = addPixmap(tImg);

            //set data flags
            foreach(dataFlag flag, LvlPlacingItems::flags)
                cursor->setData(flag.first, flag.second);

            cursor->setZValue(7000);
            cursor->setOpacity( 0.8 );
            cursor->setVisible(false);
            cursor->setEnabled(true);

            SwitchEditingMode(MODE_PlacingNew);

            break;
        }
    case 1: //bgos
    {
//        bool noimage=true, found=false;
//        bool isUser=false;

//        noimage=true;
//        isUser=false;

//        //Check Index exists
//        if(itemID < (unsigned int)index_bgo.size())
//        {
//            j = index_bgo[itemID].i;

//            if(j<pConfigs->main_bgo.size())
//            {
//                if(pConfigs->main_bgo[j].id == itemID)
//                    found=true;
//            }
//        }

//        //if Index found
//        if(found)
//        {   //get neccesary element directly
//            isUser=true;
//            noimage=false;
//            tImg = animates_BGO[index_bgo[itemID].ai]->wholeImage();
//        }
//        else
//        {
//            //fetching arrays
//            for(j=0;j<uBGOs.size();j++)
//            {
//                if(uBGOs[j].id==itemID)
//                {
//                    isUser=true;
//                    noimage=false;
//                    tImg = uBGOs[j].image;
//                    break;
//                }
//            }

//            j=pConfigs->getBgoI(itemID);
//            if(j>=0)
//            {
//                noimage=false;
//                if(!isUser)
//                tImg = pConfigs->main_bgo[j].image;
//            }
//        }

        long j;

        tImg = Items::getItemGFX(ItemTypes::LVL_BGO, itemID, false, &j);
        if(tImg.isNull())
        {
            tImg=uBgoImg;
        }


        LvlPlacingItems::gridSz=pConfigs->main_bgo[j].grid;
        LvlPlacingItems::gridOffset = QPoint(pConfigs->main_bgo[j].offsetX,
                                             pConfigs->main_bgo[j].offsetY);

        LvlPlacingItems::bgoSet.id = itemID;


        long w = tImg.width();
        long h = tImg.height();//( (pConfigs->main_bgo[j].animated)?pConfigs->main_bgo[j].frames:1);

        LvlPlacingItems::itemW = w;
        LvlPlacingItems::itemH = h;


        LvlPlacingItems::flags.clear();
        QPair<int, QVariant > flag;

            flag.first=0;
            flag.second="BGO";
        LvlPlacingItems::flags.push_back(flag);

            flag.first=1;
            flag.second=QString::number(itemID);
        LvlPlacingItems::flags.push_back(flag);

            flag.first = 9;
            flag.second = QString::number(w);
        LvlPlacingItems::flags.push_back(flag);

            flag.first = 10;
            flag.second = QString::number(h);
        LvlPlacingItems::flags.push_back(flag);

            flag.first = 25;
            flag.second = "CURSOR";
        LvlPlacingItems::flags.push_back(flag);

        //Square fill mode
        if(LvlPlacingItems::fillingMode)
        {
            setSquareDrawer(); return;
        }

        LvlPlacingItems::c_offset_x= qRound(qreal(w) / 2);
        LvlPlacingItems::c_offset_y= qRound(qreal(h) / 2);

        //Line mode
        if(LvlPlacingItems::lineMode)
        {
            setLineDrawer(); return;
        }

        //Single item placing
        cursor = addPixmap( tImg );

        //set data flags
        foreach(dataFlag flag, LvlPlacingItems::flags)
            cursor->setData(flag.first, flag.second);

        cursor->setZValue(7000);
        cursor->setOpacity( 0.8 );
        cursor->setVisible(false);
        cursor->setEnabled(true);

        placingItem=PLC_BGO;

        LvlPlacingItems::bgoSet.id = itemID;

        SwitchEditingMode(MODE_PlacingNew);
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

            j=pConfigs->getNpcI(itemID);
            if(j>=0)
            {
                if(!isUserTxt)
                    mergedSet = pConfigs->main_npc[j];
            }

        }

        tImg = getNPCimg(itemID, LvlPlacingItems::npcSet.direct);

        if(LvlPlacingItems::npcSet.generator)
            LvlPlacingItems::gridSz=16;
        else
            LvlPlacingItems::gridSz=mergedSet.grid;

        LvlPlacingItems::npcSet.is_star = mergedSet.is_star;

        LvlPlacingItems::npcGrid=mergedSet.grid;

        LvlPlacingItems::gridOffset = QPoint(mergedSet.grid_offset_x,
                                             mergedSet.grid_offset_y);

        cursor = addPixmap(tImg);

        long imgOffsetX = (int)round( - ( ( (double)mergedSet.gfx_w -
                                            (double)mergedSet.width ) / 2 ) );

        long imgOffsetY = (int)round( - (double)mergedSet.gfx_h +
                                      (double)mergedSet.height +
                                      (double)mergedSet.gfx_offset_y
                                      -((pConfigs->marker_npc.buried == LvlPlacingItems::npcSet.id)? (double)mergedSet.gfx_h : 0) );

        LvlPlacingItems::npcGfxOffsetX1 = imgOffsetX;
        LvlPlacingItems::npcGfxOffsetX2 = (-((double)mergedSet.gfx_offset_x));
        LvlPlacingItems::npcGfxOffsetY = imgOffsetY;
        ((QGraphicsPixmapItem *)cursor)->setOffset(
                    ( LvlPlacingItems::npcGfxOffsetX1 +
                    ( LvlPlacingItems::npcGfxOffsetX2 *
                      ((LvlPlacingItems::npcSet.direct==0)?-1:LvlPlacingItems::npcSet.direct))),
                    LvlPlacingItems::npcGfxOffsetY );

        LvlPlacingItems::itemW = mergedSet.width;
        LvlPlacingItems::itemH = mergedSet.height;

        cursor->setData(0, "NPC");
        cursor->setData(1, QString::number(itemID));
        cursor->setData(7, QString::number((int)mergedSet.collision_with_blocks));
        cursor->setData(8, QString::number((int)mergedSet.no_npc_collions));
        cursor->setData(9, QString::number(mergedSet.width));
        cursor->setData(10, QString::number(mergedSet.height));
        LvlPlacingItems::c_offset_x= qRound(qreal(mergedSet.width) / 2);
        LvlPlacingItems::c_offset_y= qRound(qreal(mergedSet.height) / 2);
        cursor->setData(25, "CURSOR");
        cursor->setZValue(7000);
        cursor->setOpacity( 0.8 );
        cursor->setVisible(false);
        cursor->setEnabled(true);

        break;
    }
    case 3: //water
        placingItem=PLC_Water;
        LvlPlacingItems::waterType = itemID;
        LvlPlacingItems::gridSz = 16;
        LvlPlacingItems::gridOffset = QPoint(0,0);
        LvlPlacingItems::c_offset_x= 0;
        LvlPlacingItems::c_offset_y= 0;
        setSquareDrawer(); return;
        break;
    case 4: //doorPoint
        placingItem=PLC_Door;
        LvlPlacingItems::doorType = dType;
        LvlPlacingItems::doorArrayId = itemID;

        LvlPlacingItems::gridSz=16;
        LvlPlacingItems::gridOffset = QPoint(0,0);

        LvlPlacingItems::c_offset_x = 16;
        LvlPlacingItems::c_offset_y = 16;

        cursor = addRect(0,0, 32, 32);

        ((QGraphicsRectItem *)cursor)->setBrush(QBrush(QColor(qRgb(0xff,0x00,0x7f))));
        ((QGraphicsRectItem *)cursor)->setPen(QPen(QColor(qRgb(0xff,0x00,0x7f)), 2,Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
        cursor->setData(25, "CURSOR");
        cursor->setZValue(7000);
        cursor->setOpacity( 0.8 );
        cursor->setVisible(false);
        cursor->setEnabled(true);

        break;
    case 5: //PlayerPoint
        {
        placingItem=PLC_PlayerPoint;
        LvlPlacingItems::playerID = itemID;

        LvlPlacingItems::gridSz=2;
        LvlPlacingItems::gridOffset = QPoint(0,2);

        LvlPlacingItems::c_offset_x = 16;
        LvlPlacingItems::c_offset_y = 16;

        QPixmap playerPixmap;
        switch(itemID+1)
        {
            case 1:
                playerPixmap = Themes::Image(Themes::player1); break;
            case 2:
                playerPixmap = Themes::Image(Themes::player2); break;
            default:
                playerPixmap = Themes::Image(Themes::player_point); break;
        }

        PlayerPoint x = FileFormats::dummyLvlPlayerPoint(itemID+1);

        cursor = addPixmap(playerPixmap);
        dynamic_cast<QGraphicsPixmapItem *>(cursor)->setOffset(qRound(qreal(x.w-playerPixmap.width())/2.0), x.h-playerPixmap.height() );
        cursor->setData(25, "CURSOR");
        cursor->setZValue(7000);
        cursor->setOpacity( 0.8 );
        cursor->setVisible(true);
        cursor->setEnabled(true);

        break;
        }
        default: break;
    }

    SwitchEditingMode(MODE_PlacingNew);
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
    case PLC_BGO:
    default:
        pen = QPen(Qt::gray, 2);
        brush = QBrush(Qt::darkGray);
        break;
    }

    cursor = addRect(0,0,1,1, pen, brush);

    //set data flags
    foreach(dataFlag flag, LvlPlacingItems::flags)
        cursor->setData(flag.first, flag.second);

    cursor->setData(0, "Square");

    cursor->setData(25, "CURSOR");
    cursor->setZValue(7000);
    cursor->setOpacity( 0.5 );
    cursor->setVisible(false);
    cursor->setEnabled(true);

    SwitchEditingMode(MODE_DrawSquare);
    DrawMode=true;
}


void LvlScene::setLineDrawer()
{
    if(cursor)
        {delete cursor;
        cursor=NULL;}

    QPen pen;

    switch(placingItem)
    {
    case PLC_Block:
    case PLC_BGO:
    default:
        pen = QPen(Qt::transparent, 2);
        break;
    }

    cursor = addLine(0,0,1,1, pen);

    //set data flags
    foreach(dataFlag flag, LvlPlacingItems::flags)
        cursor->setData(flag.first, flag.second);

    cursor->setData(0, "LineDrawer");

    cursor->setData(25, "CURSOR");
    cursor->setZValue(7000);
    cursor->setOpacity( 0.5 );
    cursor->setVisible(false);
    cursor->setEnabled(true);

    SwitchEditingMode(MODE_Line);
}


void LvlScene::updateCursoredNpcDirection()
{
    if(!cursor) return;
    if(cursor->data(0).toString()!="NPC") return;

    ((QGraphicsPixmapItem *)cursor)->setPixmap(getNPCimg(LvlPlacingItems::npcSet.id, LvlPlacingItems::npcSet.direct));
    ((QGraphicsPixmapItem *)cursor)->setOffset(
                ( LvlPlacingItems::npcGfxOffsetX1 +
                ( LvlPlacingItems::npcGfxOffsetX2 *
                  ((LvlPlacingItems::npcSet.direct==0)?-1:LvlPlacingItems::npcSet.direct))),
                LvlPlacingItems::npcGfxOffsetY );
}


void LvlScene::resetCursor()
{
    if(cursor)
        {delete cursor;
        cursor=NULL;}

    DrawMode=false;
    QPixmap cur(QSize(5,5));
    cur.fill(Qt::transparent);
    cursor = addPixmap(QPixmap(cur));
    ((QGraphicsPixmapItem*)cursor)->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
    cursor->setZValue(1000);
    cursor->hide();
}
