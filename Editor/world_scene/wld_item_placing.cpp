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

#include "wld_scene.h"
#include "../edit_world/world_edit.h"

#include "item_tile.h"
#include "item_scene.h"
#include "item_path.h"
#include "item_level.h"
#include "item_music.h"

#include "../file_formats/file_formats.h"
#include "wld_item_placing.h"

/*
    static LevelNPC dummyLvlNpc();
    static LevelBlock dummyLvlBlock();
    static LevelBGO dummyLvlBgo();
    static LevelWater dummyLvlWater();
    static LevelEvents dummyLvlEvent();
*/

//Default dataSets

WorldTiles WldPlacingItems::TileSet=FileFormats::dummyWldTile();
WorldScenery WldPlacingItems::SceneSet=FileFormats::dummyWldScen();
WorldPaths WldPlacingItems::PathSet=FileFormats::dummyWldPath();
WorldLevels WldPlacingItems::LevelSet=FileFormats::dummyWldLevel();
WorldMusic WldPlacingItems::MusicSet=FileFormats::dummyWldMusic();

int WldPlacingItems::c_offset_x=0;
int WldPlacingItems::c_offset_y=0;

int WldPlacingItems::itemW=0;
int WldPlacingItems::itemH=0;

int WldPlacingItems::gridSz=1;
QPoint WldPlacingItems::gridOffset=QPoint(0,0);

bool WldPlacingItems::overwriteMode=false;
WldPlacingItems::PlaceMode WldPlacingItems::placingMode=WldPlacingItems::PMODE_Brush;

QList<QPair<int, QVariant > > WldPlacingItems::flags;

void WldScene::setItemPlacer(int itemType, unsigned long itemID)
{
    if(cursor)
        {delete cursor;
        cursor=NULL;}

    WriteToLog(QtDebugMsg, QString("ItemPlacer -> set to type-%1 for ID-%2").arg(itemType).arg(itemID));

    switch(itemType)
    {
    case 0: //Tiles
    {
        int j;
        bool noimage=true, found=false;
        bool isUser=false;

        noimage=true;
        isUser=false;

        //Check Index exists
        if(itemID < (unsigned int)index_tiles.size())
        {
            j = index_tiles[itemID].i;

            if(j<pConfigs->main_wtiles.size())
            {
                if(pConfigs->main_wtiles[j].id == itemID)
                    found=true;
            }
        }

        //if Index found
        if(found)
        {   //get neccesary element directly
            isUser=true;
            noimage=false;
            tImg = animates_Tiles[index_tiles[itemID].ai]->wholeImage();
        }
        else
        {
            //fetching arrays
            for(j=0;j<uTiles.size();j++)
            {
                if(uTiles[j].id==itemID)
                {
                    isUser=true;
                    noimage=false;
                    tImg = uTiles[j].image;
                    break;
                }
            }

            j=pConfigs->getTileI(itemID);
            if(j>=0)
            {
                noimage=false;
                if(!isUser)
                tImg = pConfigs->main_wtiles[j].image;
            }
        }

        if((noimage)||(tImg.isNull()))
        {
            tImg=uTileImg;
        }

        WldPlacingItems::gridSz=pConfigs->default_grid;
        WldPlacingItems::gridOffset = QPoint(0, 0);

        WldPlacingItems::TileSet.id = itemID;


        long w = tImg.width();
        long h = tImg.height()/( (pConfigs->main_wtiles[j].animated)?pConfigs->main_wtiles[j].frames:1);

        WldPlacingItems::itemW = w;
        WldPlacingItems::itemH = h;


        WldPlacingItems::flags.clear();
        QPair<int, QVariant > flag;

            flag.first=0;
            flag.second="TILE";
        WldPlacingItems::flags.push_back(flag);

            flag.first=1;
            flag.second=QString::number(itemID);
        WldPlacingItems::flags.push_back(flag);

            flag.first = 9;
            flag.second = QString::number(w);
        WldPlacingItems::flags.push_back(flag);

            flag.first = 10;
            flag.second = QString::number(h);
        WldPlacingItems::flags.push_back(flag);

            flag.first = 25;
            flag.second = "CURSOR";
        WldPlacingItems::flags.push_back(flag);

        if(WldPlacingItems::placingMode==WldPlacingItems::PMODE_Square)
        {
            setSquareDrawer(); return;
        }

        WldPlacingItems::c_offset_x= qRound(qreal(w) / 2);
        WldPlacingItems::c_offset_y= qRound(qreal(h) / 2);

        if(WldPlacingItems::placingMode==WldPlacingItems::PMODE_Line)
        {
            setLineDrawer(); return;
        }

        cursor = addPixmap(tImg.copy(0,h*pConfigs->main_wtiles[j].display_frame,w,h));

        //set data flags
        foreach(dataFlag_w flag, WldPlacingItems::flags)
            cursor->setData(flag.first, flag.second);

        cursor->setZValue(7000);
        cursor->setOpacity( 0.8 );
        cursor->setVisible(false);
        cursor->setEnabled(true);

        placingItem=PLC_Tile;
        WldPlacingItems::TileSet.id = itemID;
        break;
    }
    case 1: //Sceneries
    {
        int j;
        bool noimage=true, found=false;
        bool isUser=false;

        noimage=true;
        isUser=false;

        //Check Index exists
        if(itemID < (unsigned int)index_scenes.size())
        {
            j = index_scenes[itemID].i;

            if(j<pConfigs->main_wscene.size())
            {
                if(pConfigs->main_wscene[j].id == itemID)
                    found=true;
            }
        }

        //if Index found
        if(found)
        {   //get neccesary element directly
            isUser=true;
            noimage=false;
            tImg = animates_Scenery[index_scenes[itemID].ai]->wholeImage();
        }
        else
        {
            //fetching arrays
            for(j=0;j<uScenes.size();j++)
            {
                if(uScenes[j].id==itemID)
                {
                    isUser=true;
                    noimage=false;
                    tImg = uScenes[j].image;
                    break;
                }
            }

            j=pConfigs->getSceneI(itemID);
            if(j>=0)
            {
                noimage=false;
                if(!isUser)
                tImg = pConfigs->main_wscene[j].image;
            }
        }

        if((noimage)||(tImg.isNull()))
        {
            tImg=uSceneImg;
        }


        WldPlacingItems::gridSz=qRound(qreal(pConfigs->default_grid)/2);
        WldPlacingItems::gridOffset = QPoint(0, 0);

        WldPlacingItems::SceneSet.id = itemID;

        long w = WldPlacingItems::gridSz;
        long h = WldPlacingItems::gridSz;

        WldPlacingItems::itemW = w;
        WldPlacingItems::itemH = h;

        long iw = tImg.width();
        long ih = tImg.height()/( (pConfigs->main_wscene[j].animated)?pConfigs->main_wscene[j].frames:1);

        WldPlacingItems::flags.clear();
        QPair<int, QVariant > flag;

            flag.first=0;
            flag.second="SCENERY";
        WldPlacingItems::flags.push_back(flag);

            flag.first=1;
            flag.second=QString::number(itemID);
        WldPlacingItems::flags.push_back(flag);

            flag.first = 9;
            flag.second = QString::number(w);
        WldPlacingItems::flags.push_back(flag);

            flag.first = 10;
            flag.second = QString::number(h);
        WldPlacingItems::flags.push_back(flag);

            flag.first = 25;
            flag.second = "CURSOR";
        WldPlacingItems::flags.push_back(flag);


        if(WldPlacingItems::placingMode==WldPlacingItems::PMODE_Square)
        {
            setSquareDrawer(); return;
        }

        WldPlacingItems::c_offset_x= qRound(qreal(w) / 2);
        WldPlacingItems::c_offset_y= qRound(qreal(h) / 2);

        if(WldPlacingItems::placingMode==WldPlacingItems::PMODE_Line)
        {
            setLineDrawer(); return;
        }

        cursor = addPixmap(tImg.copy(0,ih * pConfigs->main_wscene[j].display_frame ,iw,ih));

        //set data flags
        foreach(dataFlag_w flag, WldPlacingItems::flags)
            cursor->setData(flag.first, flag.second);

        cursor->setData(25, "CURSOR");
        cursor->setZValue(7000);
        cursor->setOpacity( 0.8 );
        cursor->setVisible(false);
        cursor->setEnabled(true);

        placingItem=PLC_Scene;
        WldPlacingItems::SceneSet.id = itemID;
        break;
    }
    case 2: //Path
    {
        int j;
        bool noimage=true, found=false;
        bool isUser=false;

        noimage=true;
        isUser=false;

        //Check Index exists
        if(itemID < (unsigned int)index_paths.size())
        {
            j = index_paths[itemID].i;

            if(j<pConfigs->main_wpaths.size())
            {
                if(pConfigs->main_wpaths[j].id == itemID)
                    found=true;
            }
        }

        //if Index found
        if(found)
        {   //get neccesary element directly
            isUser=true;
            noimage=false;
            tImg = animates_Paths[index_paths[itemID].ai]->wholeImage();
        }
        else
        {
            //fetching arrays
            for(j=0;j<uPaths.size();j++)
            {
                if(uPaths[j].id==itemID)
                {
                    isUser=true;
                    noimage=false;
                    tImg = uPaths[j].image;
                    break;
                }
            }

            j=pConfigs->getBgoI(itemID);
            if(j>=0)
            {
                noimage=false;
                if(!isUser)
                tImg = pConfigs->main_wpaths[j].image;
            }
        }

        if((noimage)||(tImg.isNull()))
        {
            tImg=uPathImg;
        }


        WldPlacingItems::gridSz=pConfigs->default_grid;
        WldPlacingItems::gridOffset = QPoint(0, 0);

        WldPlacingItems::PathSet.id = itemID;


        long w = tImg.width();
        long h = tImg.height()/( (pConfigs->main_wpaths[j].animated)?pConfigs->main_wpaths[j].frames:1);

        WldPlacingItems::itemW = w;
        WldPlacingItems::itemH = h;

        WldPlacingItems::flags.clear();
        QPair<int, QVariant > flag;

            flag.first=0;
            flag.second="PATH";
        WldPlacingItems::flags.push_back(flag);

            flag.first=1;
            flag.second=QString::number(itemID);
        WldPlacingItems::flags.push_back(flag);

            flag.first = 9;
            flag.second = QString::number(w);
        WldPlacingItems::flags.push_back(flag);

            flag.first = 10;
            flag.second = QString::number(h);
        WldPlacingItems::flags.push_back(flag);

            flag.first = 25;
            flag.second = "CURSOR";
        WldPlacingItems::flags.push_back(flag);

        if(WldPlacingItems::placingMode==WldPlacingItems::PMODE_Square)
        {
            setSquareDrawer(); return;
        }

        WldPlacingItems::c_offset_x= qRound(qreal(w) / 2);
        WldPlacingItems::c_offset_y= qRound(qreal(h) / 2);

        if(WldPlacingItems::placingMode==WldPlacingItems::PMODE_Line)
        {
            setLineDrawer(); return;
        }

        cursor = addPixmap(tImg.copy(0,h * pConfigs->main_wpaths[j].display_frame, w ,h));

        //set data flags
        foreach(dataFlag_w flag, WldPlacingItems::flags)
            cursor->setData(flag.first, flag.second);

        cursor->setData(25, "CURSOR");
        cursor->setZValue(7000);
        cursor->setOpacity( 0.8 );
        cursor->setVisible(false);
        cursor->setEnabled(true);

        placingItem=PLC_Path;
        WldPlacingItems::PathSet.id = itemID;
        break;
    }

    case 3: //Level
    {
        int j;
        bool noimage=true, found=false;
        bool isUser=false;

        noimage=true;
        isUser=false;

        //Check Index exists
        if(itemID < (unsigned int)index_levels.size())
        {
            j = index_levels[itemID].i;

            if(j<pConfigs->main_wlevels.size())
            {
                if(pConfigs->main_wlevels[j].id == itemID)
                    found=true;
            }
        }

        //if Index found
        if(found)
        {   //get neccesary element directly
            isUser=true;
            noimage=false;
            tImg = animates_Levels[index_levels[itemID].ai]->wholeImage();
        }
        else
        {
            //fetching arrays
            for(j=0;j<uLevels.size();j++)
            {
                if(uLevels[j].id==itemID)
                {
                    isUser=true;
                    noimage=false;
                    tImg = uLevels[j].image;
                    break;
                }
            }

            j=pConfigs->getWLevelI(itemID);
            if(j>=0)
            {
                noimage=false;
                if(!isUser)
                tImg = pConfigs->main_wlevels[j].image;
            }
        }

        if((noimage)||(tImg.isNull()))
        {
            tImg=uLevelImg;
        }


        WldPlacingItems::gridSz=pConfigs->default_grid;
        WldPlacingItems::gridOffset = QPoint(0, 0);

        WldPlacingItems::LevelSet.id = itemID;


        long w = WldPlacingItems::gridSz;
        long h = WldPlacingItems::gridSz;

        long iw = tImg.width();
        long ih = tImg.height()/( (pConfigs->main_wlevels[j].animated)?pConfigs->main_wlevels[j].frames:1);

        WldPlacingItems::itemW = w;
        WldPlacingItems::itemH = h;

        WldPlacingItems::flags.clear();
        QPair<int, QVariant > flag;

            flag.first=0;
            flag.second="LEVEL";
        WldPlacingItems::flags.push_back(flag);

            flag.first=1;
            flag.second=QString::number(itemID);
        WldPlacingItems::flags.push_back(flag);

            flag.first = 9;
            flag.second = QString::number(w);
        WldPlacingItems::flags.push_back(flag);

            flag.first = 10;
            flag.second = QString::number(h);
        WldPlacingItems::flags.push_back(flag);

            flag.first = 25;
            flag.second = "CURSOR";
        WldPlacingItems::flags.push_back(flag);


        if(WldPlacingItems::placingMode==WldPlacingItems::PMODE_Square)
        {
            setSquareDrawer(); return;
        }

        WldPlacingItems::c_offset_x= qRound(qreal(w) / 2);
        WldPlacingItems::c_offset_y= qRound(qreal(h) / 2);

        if(WldPlacingItems::placingMode==WldPlacingItems::PMODE_Line)
        {
            setLineDrawer(); return;
        }

        cursor = addPixmap(tImg.copy(0, ih * pConfigs->main_wlevels[j].display_frame ,iw,ih));

        int imgOffsetX = (int)qRound( -( qreal(tImg.width()) - qreal(WldPlacingItems::gridSz))  / 2 );
        int imgOffsetY = (int)qRound( -qreal(
                  tImg.height()/( (pConfigs->main_wlevels[j].animated)?pConfigs->main_wlevels[j].frames:1))
                  + WldPlacingItems::gridSz);

        ((QGraphicsPixmapItem*)cursor)->setOffset(imgOffsetX, imgOffsetY );

        //set data flags
        foreach(dataFlag_w flag, WldPlacingItems::flags)
            cursor->setData(flag.first, flag.second);

        cursor->setData(25, "CURSOR");
        cursor->setZValue(7000);
        cursor->setOpacity( 0.8 );
        cursor->setVisible(false);
        cursor->setEnabled(true);

        placingItem=PLC_Level;
        WldPlacingItems::LevelSet.id = itemID;
        break;
    }

    case 4: //MusicBox
        placingItem=PLC_Musicbox;
        WldPlacingItems::MusicSet.id = itemID;

        WldPlacingItems::gridSz=32;
        WldPlacingItems::gridOffset = QPoint(0,0);

        WldPlacingItems::c_offset_x = 16;
        WldPlacingItems::c_offset_y = 16;

        cursor = addRect(0,0, 32, 32);
        cursor->setData(0, "MUSICBOX");
        cursor->setData(1, QString::number(itemID));
        cursor->setData(9, QString::number(32));
        cursor->setData(10, QString::number(32));
        ((QGraphicsRectItem *)cursor)->setBrush(QBrush(Qt::yellow));
        ((QGraphicsRectItem *)cursor)->setPen(QPen(Qt::yellow, 2,Qt::SolidLine));
        cursor->setData(25, "CURSOR");
        cursor->setZValue(7000);
        cursor->setOpacity( 0.8 );
        cursor->setVisible(false);
        cursor->setEnabled(true);

        break;
    case 5: //Get point from a world map
        placingItem=MODE_SetPoint;
        WldPlacingItems::MusicSet.id = itemID;

        WldPlacingItems::gridSz=32;
        WldPlacingItems::gridOffset = QPoint(0,0);

        WldPlacingItems::c_offset_x = 16;
        WldPlacingItems::c_offset_y = 16;

        cursor = addRect(0,0, 32, 32);
        cursor->setData(0, "WorldMapPoint");
        cursor->setData(1, QString::number(itemID));
        cursor->setData(9, QString::number(32));
        cursor->setData(10, QString::number(32));
        ((QGraphicsRectItem *)cursor)->setBrush(QBrush(Qt::yellow));
        ((QGraphicsRectItem *)cursor)->setPen(QPen(Qt::yellow, 2,Qt::SolidLine));
        cursor->setData(25, "CURSOR");
        cursor->setZValue(7000);
        cursor->setOpacity( 0.8 );
        cursor->setVisible(false);
        cursor->setEnabled(true);

        SwitchEditingMode(MODE_SetPoint);

        // restore last point
        if(!selectedPointNotUsed) setPoint(selectedPoint);

        break;
        default: break;
    }
    if(itemType!=5) SwitchEditingMode(MODE_PlacingNew);
    contextMenuOpened=false;
}



void WldScene::setSquareDrawer()
{
    if(cursor)
        {delete cursor;
        cursor=NULL;}

    QPen pen;
    QBrush brush;


    pen = QPen(Qt::gray, 2);
    brush = QBrush(Qt::darkGray);

    cursor = addRect(0,0,1,1, pen, brush);

    //set data flags
    foreach(dataFlag_w flag, WldPlacingItems::flags)
        cursor->setData(flag.first, flag.second);

    cursor->setData(0, "Square");
    cursor->setData(25, "CURSOR");
    cursor->setZValue(7000);
    cursor->setOpacity( 0.5 );
    cursor->setVisible(false);
    cursor->setEnabled(true);

    SwitchEditingMode(MODE_DrawSquare);
}

void WldScene::setLineDrawer()
{
    if(cursor)
        {delete cursor;
        cursor=NULL;}

    QPen pen;

    pen = QPen(Qt::transparent, 2);

    cursor = addLine(0,0,1,1, pen);

    //set data flags
    foreach(dataFlag_w flag, WldPlacingItems::flags)
        cursor->setData(flag.first, flag.second);

    cursor->setData(0, "Line");
    cursor->setData(25, "CURSOR");
    cursor->setZValue(7000);
    cursor->setOpacity( 0.5 );
    cursor->setVisible(false);
    cursor->setEnabled(true);

    SwitchEditingMode(MODE_Line);

}


void WldScene::resetCursor()
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

void WldScene::setMessageBoxItem(bool show, QPointF pos, QString text)
{
    if(messageBox)
    {
        if(!show)
        {
            delete messageBox;
            messageBox = NULL;
            return;
        }

        if(text!=messageBox->text())
            messageBox->setText(text);
        messageBox->setPos(pos);
    }
    else
    {
        if(!show)
            return;

        QFont font;
        font.setFamily("Times");
        font.setWeight(100);
        font.setPointSize(25);
        messageBox = new QGraphicsSimpleTextItem(text);
        messageBox->setPen(QPen(QBrush(Qt::black), 2));
        messageBox->setBrush(QBrush(Qt::white));
        messageBox->setBoundingRegionGranularity(1);
        messageBox->setZValue(10000);
        messageBox->setFont(font);
        this->addItem(messageBox);
        messageBox->setPos(pos);
    }

}
