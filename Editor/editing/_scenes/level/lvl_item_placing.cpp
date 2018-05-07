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

#include <common_features/items.h>
#include <common_features/themes.h>
#include <editing/edit_level/level_edit.h>
#include <PGE_File_Formats/file_formats.h>

#include "lvl_scene.h"
#include "items/item_block.h"
#include "items/item_bgo.h"
#include "items/item_npc.h"
#include "items/item_water.h"
#include "lvl_item_placing.h"

/*
    static LevelNPC dummyLvlNpc();
    static LevelBlock dummyLvlBlock();
    static LevelBGO dummyLvlBgo();
    static LevelWater dummyLvlWater();
    static LevelEvents dummyLvlEvent();
*/

//Default dataSets
LevelNPC    LvlPlacingItems::npcSet=FileFormats::CreateLvlNpc();
long        LvlPlacingItems::npcGfxOffsetX1=0;
long        LvlPlacingItems::npcGfxOffsetX2=0;
long        LvlPlacingItems::npcGfxOffsetY=0;
long        LvlPlacingItems::npcGrid=0;
bool        LvlPlacingItems::npcSpecialAutoIncrement=false;
long        LvlPlacingItems::npcSpecialAutoIncrement_begin=0;
LevelBlock  LvlPlacingItems::blockSet=FileFormats::CreateLvlBlock();
LevelBGO    LvlPlacingItems::bgoSet=FileFormats::CreateLvlBgo();
long        LvlPlacingItems::itemW = 0;
long        LvlPlacingItems::itemH = 0;

LevelPhysEnv  LvlPlacingItems::waterSet=FileFormats::CreateLvlPhysEnv();

int LvlPlacingItems::doorType=LvlPlacingItems::DOOR_Entrance;
long LvlPlacingItems::doorArrayId = 0;

int LvlPlacingItems::c_offset_x=0;
int LvlPlacingItems::c_offset_y=0;

int LvlPlacingItems::waterType=0; //0 - Water, 1 - QuickSand
int LvlPlacingItems::playerID=0;

int LvlPlacingItems::gridSz=1;
QPoint LvlPlacingItems::gridOffset=QPoint(0,0);

bool LvlPlacingItems::sizableBlock=false;

bool LvlPlacingItems::overwriteMode=false;
bool LvlPlacingItems::noOutSectionFlood=true;

LvlPlacingItems::PlaceMode LvlPlacingItems::placingMode = LvlPlacingItems::PMODE_Brush;

QString LvlPlacingItems::layer="";

QList<QPair<int, QVariant > > LvlPlacingItems::flags;

void LvlScene::setItemPlacer(int itemType, unsigned long itemID, int dType)
{
    if(m_cursorItemImg)
        {delete m_cursorItemImg;
        m_cursorItemImg=NULL;}

    LogDebug(QString("ItemPlacer -> set to type-%1 for ID-%2").arg(itemType).arg(itemID));

    QPixmap tImg;

    LvlPlacingItems::sizableBlock=false;

    switch(itemType)
    {
    case 0: //blocks
        {
            obj_block &blockC = m_localConfigBlocks[itemID];
            Items::getItemGFX(&blockC, tImg, false);
            if(tImg.isNull())
            {
                tImg = m_dummyBlockImg;
            }
            if(!blockC.isValid)
            {
                blockC = m_configs->main_block[1];
                blockC.image = m_dummyBlockImg;
            }

            LvlPlacingItems::gridSz = blockC.setup.grid;
            LvlPlacingItems::gridOffset = QPoint(0, 0);

            if( (itemID != LvlPlacingItems::blockSet.id) || (m_placingItemType!=PLC_Block) )
                LvlPlacingItems::blockSet.layer = "Default";
            LvlPlacingItems::layer = LvlPlacingItems::blockSet.layer;
            LvlPlacingItems::blockSet.id = itemID;

            LvlPlacingItems::blockSet.w = tImg.width();
            LvlPlacingItems::blockSet.h = tImg.height();

            m_placingItemType=PLC_Block;

            //Place sizable blocks in the square fill mode
            if(blockC.setup.sizable)
            {
                LvlPlacingItems::sizableBlock=true;
                LvlPlacingItems::placingMode = LvlPlacingItems::PMODE_Brush;
                //Turn off autoscale for sizable blocks to escape a bug in SMBX-38A
                LvlPlacingItems::blockSet.autoscale = false;
                setRectDrawer(); return;
            }

            LvlPlacingItems::itemW = LvlPlacingItems::blockSet.w;
            LvlPlacingItems::itemH = LvlPlacingItems::blockSet.h;

            LvlPlacingItems::flags.clear();
            QPair<int, QVariant > flag;

                flag.first=ITEM_TYPE;
                flag.second="Block";
            LvlPlacingItems::flags.push_back(flag);

                flag.first=ITEM_ID;
                flag.second=QString::number(LvlPlacingItems::blockSet.id);
            LvlPlacingItems::flags.push_back(flag);

            if(blockC.setup.sizable)
            {
                flag.first=ITEM_BLOCK_IS_SIZABLE;
                flag.second = "sizable";
            LvlPlacingItems::flags.push_back(flag);
            }

                flag.first = ITEM_BLOCK_SHAPE;
                flag.second = QString::number(blockC.setup.phys_shape);
            LvlPlacingItems::flags.push_back(flag);

                flag.first = ITEM_WIDTH;
                flag.second = QString::number(LvlPlacingItems::blockSet.w);
            LvlPlacingItems::flags.push_back(flag);

                flag.first = ITEM_HEIGHT;
                flag.second = QString::number(LvlPlacingItems::blockSet.h);
            LvlPlacingItems::flags.push_back(flag);

                flag.first = ITEM_IS_CURSOR;
                flag.second = "CURSOR";
            LvlPlacingItems::flags.push_back(flag);


            //Rectangular fill mode (uses own cursor item)
            if(LvlPlacingItems::placingMode == LvlPlacingItems::PMODE_Rect)
            {
                setRectDrawer(); return;
            }

            //Rectangular fill mode (uses own cursor item)
            if(LvlPlacingItems::placingMode == LvlPlacingItems::PMODE_Circle)
            {
                setCircleDrawer(); return;
            }

            //Offset relative to item center
            LvlPlacingItems::c_offset_x= qRound(qreal(LvlPlacingItems::blockSet.w) / 2);
            LvlPlacingItems::c_offset_y= qRound(qreal(LvlPlacingItems::blockSet.h) / 2);

            //Line mode (uses own cursor item)
            if(LvlPlacingItems::placingMode == LvlPlacingItems::PMODE_Line)
            {
                setLineDrawer(); return;
            }

            //Single item placing
            m_cursorItemImg = addPixmap(tImg);

            //set data flags
            foreach(dataFlag flag, LvlPlacingItems::flags)
                m_cursorItemImg->setData(flag.first, flag.second);

            m_cursorItemImg->setZValue(7000);
            m_cursorItemImg->setOpacity( 0.8 );
            m_cursorItemImg->setVisible(false);
            m_cursorItemImg->setEnabled(true);

            //flood fill uses 'item' cursor
                //if(LvlPlacingItems::floodFillingMode)
            if(LvlPlacingItems::placingMode == LvlPlacingItems::PMODE_FloodFill)
            {
                setFloodFiller(); return;
            }

            SwitchEditingMode(MODE_PlacingNew);

            break;
        }
    case 1: //bgos
    {
        obj_bgo& bgoC = m_localConfigBGOs[itemID];
        Items::getItemGFX(&bgoC, tImg, false);
        if(tImg.isNull())
        {
            tImg=m_dummyBgoImg;
        }
        if(!bgoC.isValid)
        {
            bgoC = m_configs->main_bgo[1];
            bgoC.image = m_dummyBgoImg;
        }


        LvlPlacingItems::gridSz = bgoC.setup.grid;
        LvlPlacingItems::gridOffset = QPoint(bgoC.setup.offsetX,
                                             bgoC.setup.offsetY);

        if( (itemID != LvlPlacingItems::bgoSet.id) || (m_placingItemType!=PLC_BGO) )
            LvlPlacingItems::bgoSet.layer = "Default";
        LvlPlacingItems::layer = LvlPlacingItems::bgoSet.layer;
        LvlPlacingItems::bgoSet.id = itemID;

        long w = tImg.width();
        long h = tImg.height();//( (bgoC.animated)?bgoC.frames:1);

        LvlPlacingItems::itemW = w;
        LvlPlacingItems::itemH = h;

        m_placingItemType=PLC_BGO;

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
        if(LvlPlacingItems::placingMode==LvlPlacingItems::PMODE_Rect)
        {
            setRectDrawer(); return;
        }

        //Rectangular fill mode (uses own cursor item)
        if(LvlPlacingItems::placingMode == LvlPlacingItems::PMODE_Circle)
        {
            setCircleDrawer(); return;
        }

        LvlPlacingItems::c_offset_x= qRound(qreal(w) / 2);
        LvlPlacingItems::c_offset_y= qRound(qreal(h) / 2);

        //Line mode
        if(LvlPlacingItems::placingMode==LvlPlacingItems::PMODE_Line)
        {
            setLineDrawer(); return;
        }

        //Single item placing
        m_cursorItemImg = addPixmap( tImg );

        //set data flags
        foreach(dataFlag flag, LvlPlacingItems::flags)
            m_cursorItemImg->setData(flag.first, flag.second);

        m_cursorItemImg->setZValue(7000);
        m_cursorItemImg->setOpacity( 0.8 );
        m_cursorItemImg->setVisible(false);
        m_cursorItemImg->setEnabled(true);

        //flood fill uses 'item' cursor
        if(LvlPlacingItems::placingMode==LvlPlacingItems::PMODE_FloodFill)
        {
            setFloodFiller(); return;
        }

        SwitchEditingMode(MODE_PlacingNew);
        break;
    }
    case 2: //npcs
    {
        obj_npc &mergedSet = m_localConfigNPCs[itemID];
        tImg = getNPCimg(itemID, LvlPlacingItems::npcSet.direct);
        if(!mergedSet.isValid)
        {
            mergedSet = m_configs->main_npc[1];
            mergedSet.image = m_dummyNpcImg;
        }

        if( (itemID != LvlPlacingItems::npcSet.id) || (m_placingItemType!=PLC_NPC) )
            LvlPlacingItems::npcSet.layer = "Default";
        LvlPlacingItems::layer = LvlPlacingItems::npcSet.layer;
        LvlPlacingItems::npcSet.id = itemID;

        if(LvlPlacingItems::npcSet.generator)
            LvlPlacingItems::gridSz = (m_configs->defaultGrid.general/2);
        else
            LvlPlacingItems::gridSz = mergedSet.setup.grid;

        LvlPlacingItems::npcSet.is_star = mergedSet.setup.is_star;

        LvlPlacingItems::npcGrid = mergedSet.setup.grid;

        LvlPlacingItems::gridOffset = QPoint(mergedSet.setup.grid_offset_x,
                                             mergedSet.setup.grid_offset_y);

        LvlPlacingItems::flags.clear();
        QPair<int, QVariant > flag;

        long imgOffsetX = (int)round( - ( ( (double)mergedSet.setup.gfx_w -
                                            (double)mergedSet.setup.width ) / 2 ) );

        long imgOffsetY = (int)round( - (double)mergedSet.setup.gfx_h +
                                      (double)mergedSet.setup.height +
                                      (double)mergedSet.setup.gfx_offset_y );

        LvlPlacingItems::npcGfxOffsetX1 = imgOffsetX;
        LvlPlacingItems::npcGfxOffsetX2 = (-((double)mergedSet.setup.gfx_offset_x));
        LvlPlacingItems::npcGfxOffsetY = imgOffsetY;

        LvlPlacingItems::itemW = mergedSet.setup.width;
        LvlPlacingItems::itemH = mergedSet.setup.height;

        LvlPlacingItems::c_offset_x= qRound(qreal(mergedSet.setup.width) / 2);
        LvlPlacingItems::c_offset_y= qRound(qreal(mergedSet.setup.height) / 2);

        m_placingItemType = PLC_NPC;

            flag.first=ITEM_TYPE;
            flag.second="NPC";
        LvlPlacingItems::flags.push_back(flag);

            flag.first=ITEM_ID;
            flag.second=QString::number(itemID);
        LvlPlacingItems::flags.push_back(flag);

            flag.first=ITEM_NPC_BLOCK_COLLISION;
            flag.second=QString::number((int)mergedSet.setup.collision_with_blocks);
        LvlPlacingItems::flags.push_back(flag);

            flag.first=ITEM_NPC_NO_NPC_COLLISION;
            flag.second=QString::number((int)mergedSet.setup.no_npc_collisions);
        LvlPlacingItems::flags.push_back(flag);

            flag.first=ITEM_WIDTH;
            flag.second=QString::number(mergedSet.setup.width);
        LvlPlacingItems::flags.push_back(flag);

            flag.first=ITEM_HEIGHT;
            flag.second=QString::number(mergedSet.setup.height);
        LvlPlacingItems::flags.push_back(flag);

            flag.first=ITEM_IS_CURSOR;
            flag.second="CURSOR";
        LvlPlacingItems::flags.push_back(flag);

        //Line mode
        if(LvlPlacingItems::placingMode==LvlPlacingItems::PMODE_Line)
        {
            setLineDrawer(); return;
        }

        m_cursorItemImg = addPixmap(tImg);

        //set data flags
        foreach(dataFlag flag, LvlPlacingItems::flags)
            m_cursorItemImg->setData(flag.first, flag.second);

        ((QGraphicsPixmapItem *)m_cursorItemImg)->setOffset(
                    ( LvlPlacingItems::npcGfxOffsetX1 +
                    ( LvlPlacingItems::npcGfxOffsetX2 *
                      ((LvlPlacingItems::npcSet.direct==0)?-1:LvlPlacingItems::npcSet.direct))),
                    LvlPlacingItems::npcGfxOffsetY );

        m_cursorItemImg->setZValue(7000);
        m_cursorItemImg->setOpacity( 0.8 );
        m_cursorItemImg->setVisible(false);
        m_cursorItemImg->setEnabled(true);

        break;
    }
    case 3: //water
        m_placingItemType=PLC_Water;
        LvlPlacingItems::waterType = itemID;
        LvlPlacingItems::gridSz = 16;
        LvlPlacingItems::gridOffset = QPoint(0,0);
        LvlPlacingItems::c_offset_x= 0;
        LvlPlacingItems::c_offset_y= 0;
        LvlPlacingItems::waterSet.layer = LvlPlacingItems::layer.isEmpty()? "Default" : LvlPlacingItems::layer;
        setRectDrawer(); return;
        break;
    case 4: //doorPoint
        m_placingItemType=PLC_Door;
        LvlPlacingItems::doorType = dType;
        LvlPlacingItems::doorArrayId = itemID;

        LvlPlacingItems::gridSz = 16;
        LvlPlacingItems::gridOffset = QPoint(0,0);

        LvlPlacingItems::c_offset_x = 16;
        LvlPlacingItems::c_offset_y = 16;

        LvlPlacingItems::layer = "";

        m_cursorItemImg = addRect(0,0, 32, 32);

        ((QGraphicsRectItem *)m_cursorItemImg)->setBrush(QBrush(QColor(qRgb(0xff,0x00,0x7f))));
        ((QGraphicsRectItem *)m_cursorItemImg)->setPen(QPen(QColor(qRgb(0xff,0x00,0x7f)), 2,Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
        m_cursorItemImg->setData(ITEM_IS_CURSOR, "CURSOR");
        m_cursorItemImg->setZValue(7000);
        m_cursorItemImg->setOpacity( 0.8 );
        m_cursorItemImg->setVisible(false);
        m_cursorItemImg->setEnabled(true);

        break;
    case 5: //PlayerPoint
        {
        m_placingItemType = PLC_PlayerPoint;
        LvlPlacingItems::playerID = itemID;
        LvlPlacingItems::gridSz = 16;

        LvlPlacingItems::layer = "";

        QPixmap playerPixmap;
        switch( itemID + 1 )
        {
            case 1:
                playerPixmap = Themes::Image(Themes::player1); break;
            case 2:
                playerPixmap = Themes::Image(Themes::player2); break;
            default:
                playerPixmap = Themes::Image(Themes::player_point); break;
        }

        PlayerPoint x = FileFormats::CreateLvlPlayerPoint(itemID + 1);

        int grid_offset_x = 0;
        int grid_offset_y = 0;
        int grid = LvlPlacingItems::gridSz;
        if(((int)x.w >= grid))
            grid_offset_x = -1 * qRound( qreal((int)x.w % grid) / 2 );
        else
            grid_offset_x = qRound( qreal( grid - (int)x.w ) / 2 );
        grid_offset_x += (grid / 2);

        grid_offset_y = -x.h % grid;

        LvlPlacingItems::gridOffset = QPoint(grid_offset_x, grid_offset_y);

        m_cursorItemImg = addPixmap(playerPixmap);
        int w = playerPixmap.width();
        int h = playerPixmap.height();

        LvlPlacingItems::c_offset_x = qRound(qreal(x.w - w) / 2.0);
        LvlPlacingItems::c_offset_y = (int)x.h - h;

        dynamic_cast<QGraphicsPixmapItem *>(m_cursorItemImg)->setOffset(qRound(qreal(x.w-w)/2.0), x.h-h);
        m_cursorItemImg->setData(ITEM_IS_CURSOR, "CURSOR");
        m_cursorItemImg->setZValue(7000);
        m_cursorItemImg->setOpacity( 0.8 );
        m_cursorItemImg->setVisible(true);
        m_cursorItemImg->setEnabled(true);

        break;
        }
        default: break;
    }

    SwitchEditingMode(MODE_PlacingNew);
    m_busyMode=true;
    m_contextMenuIsOpened=false;
}



void LvlScene::setRectDrawer()
{
    if(m_cursorItemImg)
        {delete m_cursorItemImg;
        m_cursorItemImg=NULL;}

    QPen pen;
    QBrush brush;

    switch(m_placingItemType)
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

    //Align width and height to fit into item aligning
    long addW=LvlPlacingItems::gridSz-LvlPlacingItems::itemW%LvlPlacingItems::gridSz;
    long addH=LvlPlacingItems::gridSz-LvlPlacingItems::itemH%LvlPlacingItems::gridSz;
    if(addW==LvlPlacingItems::gridSz) addW=0;
    if(addH==LvlPlacingItems::gridSz) addH=0;
    LvlPlacingItems::itemW = LvlPlacingItems::itemW+addW;
    LvlPlacingItems::itemH = LvlPlacingItems::itemH+addH;

    if((m_placingItemType != PLC_Water) && (!LvlPlacingItems::sizableBlock))
    {
        QPixmap oneCell(LvlPlacingItems::itemW, LvlPlacingItems::itemH);
        oneCell.fill(QColor(0xFF, 0xFF, 0x00, 128));
        QPainter p(&oneCell);
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(Qt::yellow, 2, Qt::SolidLine));
        p.drawRect(0,0, LvlPlacingItems::itemW, LvlPlacingItems::itemH);
        brush.setTexture(oneCell);
    }

    m_cursorItemImg = addRect(0,0,1,1, pen, brush);

    //set data flags
    foreach(dataFlag flag, LvlPlacingItems::flags)
        m_cursorItemImg->setData(flag.first, flag.second);

    m_cursorItemImg->setData(ITEM_TYPE, "Square");

    m_cursorItemImg->setData(ITEM_IS_CURSOR, "CURSOR");
    m_cursorItemImg->setZValue(7000);
    m_cursorItemImg->setOpacity( 0.5 );
    m_cursorItemImg->setVisible(false);
    m_cursorItemImg->setEnabled(true);

    SwitchEditingMode(MODE_DrawRect);
    m_busyMode=true;
}

void LvlScene::setCircleDrawer()
{
    if(m_cursorItemImg)
        {delete m_cursorItemImg;
        m_cursorItemImg=NULL;}

    QPen pen;
    QBrush brush;

    switch(m_placingItemType)
    {
    case PLC_Block:
    case PLC_BGO:
    default:
        pen = QPen(Qt::gray, 2);
        brush = QBrush(Qt::darkGray);
        break;
    }

    //Align width and height to fit into item aligning
    long addW=LvlPlacingItems::gridSz-LvlPlacingItems::itemW%LvlPlacingItems::gridSz;
    long addH=LvlPlacingItems::gridSz-LvlPlacingItems::itemH%LvlPlacingItems::gridSz;
    if(addW==LvlPlacingItems::gridSz) addW=0;
    if(addH==LvlPlacingItems::gridSz) addH=0;
    LvlPlacingItems::itemW = LvlPlacingItems::itemW+addW;
    LvlPlacingItems::itemH = LvlPlacingItems::itemH+addH;

    if((m_placingItemType != PLC_Water) && (!LvlPlacingItems::sizableBlock))
    {
        QPixmap oneCell(LvlPlacingItems::itemW, LvlPlacingItems::itemH);
        oneCell.fill(QColor(0xFF, 0xFF, 0x00, 128));
        QPainter p(&oneCell);
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(Qt::yellow, 2, Qt::SolidLine));
        p.drawRect(0,0, LvlPlacingItems::itemW, LvlPlacingItems::itemH);
        brush.setTexture(oneCell);
    }

    m_cursorItemImg = addEllipse(0,0,1,1, pen, brush);

    //set data flags
    foreach(dataFlag flag, LvlPlacingItems::flags)
        m_cursorItemImg->setData(flag.first, flag.second);

    m_cursorItemImg->setData(ITEM_TYPE, "Circle");

    m_cursorItemImg->setData(ITEM_IS_CURSOR, "CURSOR");
    m_cursorItemImg->setZValue(7000);
    m_cursorItemImg->setOpacity( 0.5 );
    m_cursorItemImg->setVisible(false);
    m_cursorItemImg->setEnabled(true);

    SwitchEditingMode(MODE_DrawCircle);
    m_busyMode=true;
}


void LvlScene::setLineDrawer()
{
    if(m_cursorItemImg)
        {delete m_cursorItemImg;
        m_cursorItemImg=NULL;}

    QPen pen;

    switch(m_placingItemType)
    {
    case PLC_Block:
    case PLC_BGO:
    case PLC_NPC:
    default:
        pen = QPen(Qt::transparent, 2);
        break;
    }

    //Align width and height to fit into item aligning
    long addW=LvlPlacingItems::gridSz-LvlPlacingItems::itemW%LvlPlacingItems::gridSz;
    long addH=LvlPlacingItems::gridSz-LvlPlacingItems::itemH%LvlPlacingItems::gridSz;
    if(addW==LvlPlacingItems::gridSz) addW=0;
    if(addH==LvlPlacingItems::gridSz) addH=0;
    LvlPlacingItems::itemW = LvlPlacingItems::itemW+addW;
    LvlPlacingItems::itemH = LvlPlacingItems::itemH+addH;

    m_cursorItemImg = addLine(0,0,1,1, pen);

    //set data flags
    foreach(dataFlag flag, LvlPlacingItems::flags)
        m_cursorItemImg->setData(flag.first, flag.second);

    m_cursorItemImg->setData(ITEM_TYPE, "LineDrawer");

    m_cursorItemImg->setData(ITEM_IS_CURSOR, "CURSOR");
    m_cursorItemImg->setZValue(7000);
    m_cursorItemImg->setOpacity( 0.5 );
    m_cursorItemImg->setVisible(false);
    m_cursorItemImg->setEnabled(true);

    SwitchEditingMode(MODE_Line);
}

void LvlScene::setFloodFiller()
{
    SwitchEditingMode(MODE_Fill);
}


void LvlScene::updateCursoredNpcDirection()
{
    if(!m_cursorItemImg) return;
    if(m_cursorItemImg->data(ITEM_TYPE).toString()!="NPC") return;

    ((QGraphicsPixmapItem *)m_cursorItemImg)->setPixmap(getNPCimg(LvlPlacingItems::npcSet.id, LvlPlacingItems::npcSet.direct));
    ((QGraphicsPixmapItem *)m_cursorItemImg)->setOffset(
                ( LvlPlacingItems::npcGfxOffsetX1 +
                ( LvlPlacingItems::npcGfxOffsetX2 *
                  ((LvlPlacingItems::npcSet.direct==0)?-1:LvlPlacingItems::npcSet.direct))),
                LvlPlacingItems::npcGfxOffsetY );
}


void LvlScene::resetCursor()
{
    if(m_cursorItemImg)
        {delete m_cursorItemImg;
        m_cursorItemImg=NULL;}

    m_busyMode=false;
    QPixmap cur(QSize(5,5));
    cur.fill(Qt::transparent);
    m_cursorItemImg = addPixmap(QPixmap(cur));
    ((QGraphicsPixmapItem*)m_cursorItemImg)->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
    m_cursorItemImg->setZValue(1000);
    m_cursorItemImg->hide();
}

void LvlScene::setLabelBoxItem(bool show, QPointF pos, QString text)
{
    if(m_labelBox)
    {
        if(!show)
        {
            delete m_labelBox;
            m_labelBox = NULL;
            return;
        }

        if(text!=m_labelBox->text())
            m_labelBox->setText(text);
        m_labelBox->setPos(pos);
    }
    else
    {
        if(!show)
            return;

        QFont font;
        font.setFamily("Times");
        font.setWeight(99);
        font.setPointSize(25);
        m_labelBox = new QGraphicsSimpleTextItem(text);
        m_labelBox->setPen(QPen(QBrush(Qt::black), 2));
        m_labelBox->setBrush(QBrush(Qt::white));
        m_labelBox->setBoundingRegionGranularity(1);
        m_labelBox->setZValue(10000);
        m_labelBox->setFont(font);
        this->addItem(m_labelBox);
        m_labelBox->setPos(pos);
    }

}
