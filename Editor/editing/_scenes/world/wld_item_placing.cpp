/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <PGE_File_Formats/file_formats.h>
#include <editing/edit_world/world_edit.h>

#include "wld_scene.h"
#include "items/item_tile.h"
#include "items/item_scene.h"
#include "items/item_path.h"
#include "items/item_level.h"
#include "items/item_music.h"
#include "wld_item_placing.h"

/*
    static LevelNPC dummyLvlNpc();
    static LevelBlock dummyLvlBlock();
    static LevelBGO dummyLvlBgo();
    static LevelWater dummyLvlWater();
    static LevelEvents dummyLvlEvent();
*/

//Default dataSets

WorldTerrainTile WldPlacingItems::terrainSet = FileFormats::CreateWldTile();
WorldScenery WldPlacingItems::sceneSet = FileFormats::CreateWldScenery();
WorldPathTile WldPlacingItems::pathSet = FileFormats::CreateWldPath();
WorldLevelTile WldPlacingItems::levelSet = FileFormats::CreateWldLevel();
WorldMusicBox WldPlacingItems::musicSet = FileFormats::CreateWldMusicbox();

int WldPlacingItems::c_offset_x = 0;
int WldPlacingItems::c_offset_y = 0;

int WldPlacingItems::itemW = 0;
int WldPlacingItems::itemH = 0;

int WldPlacingItems::gridSz = 1;
QPoint WldPlacingItems::gridOffset = QPoint(0, 0);

bool WldPlacingItems::overwriteMode = false;
WldPlacingItems::PlaceMode WldPlacingItems::placingMode = WldPlacingItems::PMODE_Brush;

QList<QPair<int, QVariant> > WldPlacingItems::flags;

void WldScene::setItemPlacer(int itemType, unsigned long itemID)
{
    if(m_cursorItemImg)
    {
        delete m_cursorItemImg;
        m_cursorItemImg = nullptr;
    }

    QPixmap tImg;
    LogDebug(QString("ItemPlacer -> set to type-%1 for ID-%2").arg(itemType).arg(itemID));

    switch(itemType)
    {
    case 0: //Terrain tiles
    {
        obj_w_tile &tileConf = m_localConfigTerrain[itemID];
        Items::getItemGFX(&tileConf, tImg, false);
        if(tImg.isNull())
        {
            tImg = m_dummyTerrainImg;
        }
        if(!tileConf.isValid)
        {
            tileConf = m_configs->main_wtiles[1];
            tileConf.image = m_dummyTerrainImg;
        }

        WldPlacingItems::gridSz = tileConf.setup.grid;
        WldPlacingItems::gridOffset = QPoint(0, 0);

        WldPlacingItems::terrainSet.id = itemID;

        // Clean up previously added extra settings
        WldPlacingItems::terrainSet.meta.custom_params.clear();

        long w = tImg.width();
        long h = tImg.height();

        WldPlacingItems::itemW = int(w);
        WldPlacingItems::itemH = int(h);


        WldPlacingItems::flags.clear();
        QPair<int, QVariant> flag;

        flag.first = ITEM_TYPE;
        flag.second = "TILE";
        WldPlacingItems::flags.push_back(flag);

        flag.first = ITEM_ID;
        flag.second = QString::number(itemID);
        WldPlacingItems::flags.push_back(flag);

        flag.first = ITEM_WIDTH;
        flag.second = QString::number(w);
        WldPlacingItems::flags.push_back(flag);

        flag.first = ITEM_HEIGHT;
        flag.second = QString::number(h);
        WldPlacingItems::flags.push_back(flag);

        flag.first = ITEM_IS_CURSOR;
        flag.second = "CURSOR";
        WldPlacingItems::flags.push_back(flag);

        if(WldPlacingItems::placingMode == WldPlacingItems::PMODE_Rect)
        {
            setRectDrawer();
            return;
        }
        if(WldPlacingItems::placingMode == WldPlacingItems::PMODE_Circle)
        {
            setCircleDrawer();
            return;
        }

        WldPlacingItems::c_offset_x = qRound(qreal(w) / 2);
        WldPlacingItems::c_offset_y = qRound(qreal(h) / 2);

        if(WldPlacingItems::placingMode == WldPlacingItems::PMODE_Line)
        {
            setLineDrawer();
            return;
        }

        m_cursorItemImg = addPixmap(tImg);

        //set data flags
        for(const dataFlag_w &itemFlag : WldPlacingItems::flags)
            m_cursorItemImg->setData(itemFlag.first, itemFlag.second);

        m_cursorItemImg->setZValue(7000);
        m_cursorItemImg->setOpacity(0.8);
        m_cursorItemImg->setVisible(false);
        m_cursorItemImg->setEnabled(true);

        m_placingItemType = PLC_Terrain;
        WldPlacingItems::terrainSet.id = itemID;

        //flood fill uses 'item' cursor
        if(WldPlacingItems::placingMode == WldPlacingItems::PMODE_FloodFill)
        {
            setFloodFiller();
            return;
        }

        SwitchEditingMode(MODE_PlacingNew);

        break;
    }
    case 1: //Sceneries
    {
        obj_w_scenery &sceneConf = m_localConfigScenery[itemID];
        Items::getItemGFX(&sceneConf, tImg, false);
        if(tImg.isNull())
        {
            tImg = m_dummySceneryImg;
        }
        if(!sceneConf.isValid)
        {
            sceneConf = m_configs->main_wscene[1];
            sceneConf.image = m_dummySceneryImg;
        }

        WldPlacingItems::gridSz = sceneConf.setup.grid;
        WldPlacingItems::gridOffset = QPoint(0, 0);

        WldPlacingItems::sceneSet.id = itemID;

        // Clean up previously added extra settings
        WldPlacingItems::sceneSet.meta.custom_params.clear();

        long w = WldPlacingItems::gridSz;
        long h = WldPlacingItems::gridSz;

        WldPlacingItems::itemW = int(w);
        WldPlacingItems::itemH = int(h);

        WldPlacingItems::flags.clear();
        QPair<int, QVariant> flag;

        flag.first = ITEM_TYPE;
        flag.second = "SCENERY";
        WldPlacingItems::flags.push_back(flag);

        flag.first = ITEM_ID;
        flag.second = QString::number(itemID);
        WldPlacingItems::flags.push_back(flag);

        flag.first = ITEM_WIDTH;
        flag.second = QString::number(w);
        WldPlacingItems::flags.push_back(flag);

        flag.first = ITEM_HEIGHT;
        flag.second = QString::number(h);
        WldPlacingItems::flags.push_back(flag);

        flag.first = ITEM_IS_CURSOR;
        flag.second = "CURSOR";
        WldPlacingItems::flags.push_back(flag);


        if(WldPlacingItems::placingMode == WldPlacingItems::PMODE_Rect)
        {
            setRectDrawer();
            return;
        }

        WldPlacingItems::c_offset_x = qRound(qreal(tImg.width()) / 2);
        WldPlacingItems::c_offset_y = qRound(qreal(tImg.height()) / 2);

        if(WldPlacingItems::placingMode == WldPlacingItems::PMODE_Line)
        {
            setLineDrawer();
            return;
        }
        if(WldPlacingItems::placingMode == WldPlacingItems::PMODE_Circle)
        {
            setCircleDrawer();
            return;
        }

        m_cursorItemImg = addPixmap(tImg);

        //set data flags
        for(const dataFlag_w &itemFlag : WldPlacingItems::flags)
            m_cursorItemImg->setData(itemFlag.first, itemFlag.second);

        m_cursorItemImg->setData(ITEM_IS_CURSOR, "CURSOR");
        m_cursorItemImg->setZValue(7000);
        m_cursorItemImg->setOpacity(0.8);
        m_cursorItemImg->setVisible(false);
        m_cursorItemImg->setEnabled(true);

        m_placingItemType = PLC_Scene;
        WldPlacingItems::sceneSet.id = itemID;

        //flood fill uses 'item' cursor
        if(WldPlacingItems::placingMode == WldPlacingItems::PMODE_FloodFill)
        {
            setFloodFiller();
            return;
        }

        SwitchEditingMode(MODE_PlacingNew);

        break;
    }
    case 2: //Path
    {
        obj_w_path &pathConf = m_localConfigPaths[itemID];
        Items::getItemGFX(&pathConf, tImg, false);
        if(tImg.isNull())
        {
            tImg = m_dummyPathImg;
        }
        if(!pathConf.isValid)
        {
            pathConf = m_configs->main_wpaths[1];
            pathConf.image = m_dummyPathImg;
        }

        WldPlacingItems::gridSz = pathConf.setup.grid;
        WldPlacingItems::gridOffset = QPoint(0, 0);

        WldPlacingItems::pathSet.id = itemID;

        // Clean up previously added extra settings
        WldPlacingItems::pathSet.meta.custom_params.clear();

        long w = tImg.width();
        long h = tImg.height();

        WldPlacingItems::itemW = int(w);
        WldPlacingItems::itemH = int(h);

        WldPlacingItems::flags.clear();
        QPair<int, QVariant> flag;

        flag.first = ITEM_TYPE;
        flag.second = "PATH";
        WldPlacingItems::flags.push_back(flag);

        flag.first = ITEM_ID;
        flag.second = QString::number(itemID);
        WldPlacingItems::flags.push_back(flag);

        flag.first = ITEM_WIDTH;
        flag.second = QString::number(w);
        WldPlacingItems::flags.push_back(flag);

        flag.first = ITEM_HEIGHT;
        flag.second = QString::number(h);
        WldPlacingItems::flags.push_back(flag);

        flag.first = ITEM_IS_CURSOR;
        flag.second = "CURSOR";
        WldPlacingItems::flags.push_back(flag);

        if(WldPlacingItems::placingMode == WldPlacingItems::PMODE_Rect)
        {
            setRectDrawer();
            return;
        }
        if(WldPlacingItems::placingMode == WldPlacingItems::PMODE_Circle)
        {
            setCircleDrawer();
            return;
        }

        WldPlacingItems::c_offset_x = qRound(qreal(w) / 2);
        WldPlacingItems::c_offset_y = qRound(qreal(h) / 2);

        if(WldPlacingItems::placingMode == WldPlacingItems::PMODE_Line)
        {
            setLineDrawer();
            return;
        }

        m_cursorItemImg = addPixmap(tImg);

        //set data flags
        for(const dataFlag_w &itemFlag : WldPlacingItems::flags)
            m_cursorItemImg->setData(itemFlag.first, itemFlag.second);

        m_cursorItemImg->setData(ITEM_IS_CURSOR, "CURSOR");
        m_cursorItemImg->setZValue(7000);
        m_cursorItemImg->setOpacity(0.8);
        m_cursorItemImg->setVisible(false);
        m_cursorItemImg->setEnabled(true);

        m_placingItemType = PLC_Path;
        WldPlacingItems::pathSet.id = itemID;

        //flood fill uses 'item' cursor
        if(WldPlacingItems::placingMode == WldPlacingItems::PMODE_FloodFill)
        {
            setFloodFiller();
            return;
        }

        SwitchEditingMode(MODE_PlacingNew);

        break;
    }

    case 3: //Level
    {
        obj_w_level &wlevelConf = m_localConfigLevels[itemID];
        Items::getItemGFX(&wlevelConf, tImg, false);
        if(tImg.isNull())
        {
            tImg = m_dummyLevelImg;
        }
        if(!wlevelConf.isValid)
        {
            wlevelConf = m_configs->main_wlevels[0];
            wlevelConf.image = m_dummyLevelImg;
        }

        WldPlacingItems::gridSz = wlevelConf.setup.grid;
        WldPlacingItems::gridOffset = QPoint(0, 0);

        WldPlacingItems::levelSet.id = itemID;

        // Clean up previously added extra settings
        WldPlacingItems::levelSet.meta.custom_params.clear();

        long w = WldPlacingItems::gridSz;
        long h = WldPlacingItems::gridSz;

        WldPlacingItems::itemW = int(w);
        WldPlacingItems::itemH = int(h);

        WldPlacingItems::flags.clear();
        QPair<int, QVariant> flag;

        flag.first = ITEM_TYPE;
        flag.second = "LEVEL";
        WldPlacingItems::flags.push_back(flag);

        flag.first = ITEM_ID;
        flag.second = QString::number(itemID);
        WldPlacingItems::flags.push_back(flag);

        flag.first = ITEM_WIDTH;
        flag.second = QString::number(w);
        WldPlacingItems::flags.push_back(flag);

        flag.first = ITEM_HEIGHT;
        flag.second = QString::number(h);
        WldPlacingItems::flags.push_back(flag);

        flag.first = ITEM_IS_CURSOR;
        flag.second = "CURSOR";
        WldPlacingItems::flags.push_back(flag);


        if(WldPlacingItems::placingMode == WldPlacingItems::PMODE_Rect)
        {
            setRectDrawer();
            return;
        }
        if(WldPlacingItems::placingMode == WldPlacingItems::PMODE_Circle)
        {
            setCircleDrawer();
            return;
        }

        WldPlacingItems::c_offset_x = qRound(qreal(w) / 2);
        WldPlacingItems::c_offset_y = qRound(qreal(h) / 2);

        if(WldPlacingItems::placingMode == WldPlacingItems::PMODE_Line)
        {
            setLineDrawer();
            return;
        }

        m_cursorItemImg = addPixmap(tImg);

        int imgOffsetX = (int) qRound(-(qreal(tImg.width()) - qreal(WldPlacingItems::gridSz)) / 2);
        int imgOffsetY = (int) qRound(-qreal(tImg.height()) + WldPlacingItems::gridSz);

        ((QGraphicsPixmapItem *) m_cursorItemImg)->setOffset(imgOffsetX, imgOffsetY);

        //set data flags
        for(const dataFlag_w &itemFlag : WldPlacingItems::flags)
            m_cursorItemImg->setData(itemFlag.first, itemFlag.second);

        m_cursorItemImg->setData(ITEM_IS_CURSOR, "CURSOR");
        m_cursorItemImg->setZValue(7000);
        m_cursorItemImg->setOpacity(0.8);
        m_cursorItemImg->setVisible(false);
        m_cursorItemImg->setEnabled(true);

        m_placingItemType = PLC_Level;
        WldPlacingItems::levelSet.id = itemID;

        //flood fill uses 'item' cursor
        if(WldPlacingItems::placingMode == WldPlacingItems::PMODE_FloodFill)
        {
            setFloodFiller();
            return;
        }
        SwitchEditingMode(MODE_PlacingNew);
        break;
    }

    case 4: //MusicBox
    {
        m_placingItemType = PLC_Musicbox;
        WldPlacingItems::musicSet.id = itemID;

        WldPlacingItems::gridSz = m_configs->defaultGrid.general;
        WldPlacingItems::gridOffset = QPoint(0, 0);

        WldPlacingItems::c_offset_x = m_configs->defaultGrid.general / 2;
        WldPlacingItems::c_offset_y = m_configs->defaultGrid.general / 2;

        m_cursorItemImg = addRect(0, 0, m_configs->defaultGrid.general, m_configs->defaultGrid.general);
        m_cursorItemImg->setData(ITEM_TYPE, "MUSICBOX");
        m_cursorItemImg->setData(ITEM_ID, QString::number(itemID));
        m_cursorItemImg->setData(ITEM_WIDTH, QString::number(m_configs->defaultGrid.general));
        m_cursorItemImg->setData(ITEM_HEIGHT, QString::number(m_configs->defaultGrid.general));
        auto cursor = qgraphicsitem_cast<QGraphicsRectItem *>(m_cursorItemImg);
        Q_ASSERT(cursor);
        cursor->setBrush(QBrush(Qt::yellow));
        cursor->setPen(QPen(Qt::yellow, 2, Qt::SolidLine));
        m_cursorItemImg->setData(ITEM_IS_CURSOR, "CURSOR");
        m_cursorItemImg->setZValue(7000);
        m_cursorItemImg->setOpacity(0.8);
        m_cursorItemImg->setVisible(false);
        m_cursorItemImg->setEnabled(true);

        SwitchEditingMode(MODE_PlacingNew);

        break;
    }
    case 5: //Get point from a world map
    {
        m_placingItemType = MODE_SetPoint;
        WldPlacingItems::musicSet.id = itemID;

        WldPlacingItems::gridSz = m_configs->defaultGrid.general;
        WldPlacingItems::gridOffset = QPoint(0, 0);

        WldPlacingItems::c_offset_x = m_configs->defaultGrid.general / 2;
        WldPlacingItems::c_offset_y = m_configs->defaultGrid.general / 2;

        m_cursorItemImg = addRect(0, 0, m_configs->defaultGrid.general, m_configs->defaultGrid.general);
        m_cursorItemImg->setData(ITEM_TYPE, "WorldMapPoint");
        m_cursorItemImg->setData(ITEM_ID, QString::number(itemID));
        m_cursorItemImg->setData(ITEM_WIDTH, QString::number(m_configs->defaultGrid.general));
        m_cursorItemImg->setData(ITEM_HEIGHT, QString::number(m_configs->defaultGrid.general));
        auto cursor = qgraphicsitem_cast<QGraphicsRectItem *>(m_cursorItemImg);
        Q_ASSERT(cursor);
        cursor->setBrush(QBrush(Qt::yellow));
        cursor->setPen(QPen(Qt::yellow, 2, Qt::SolidLine));
        m_cursorItemImg->setData(ITEM_IS_CURSOR, "CURSOR");
        m_cursorItemImg->setZValue(7000);
        m_cursorItemImg->setOpacity(0.8);
        m_cursorItemImg->setVisible(false);
        m_cursorItemImg->setEnabled(true);

        SwitchEditingMode(MODE_SetPoint);

        // restore last point
        if(!m_pointSelector.m_pointNotPlaced)
            m_pointSelector.setPoint(m_pointSelector.m_pointCoord);

        break;
    }
    default:
        break;
    }
    if(itemType != 5) SwitchEditingMode(MODE_PlacingNew);
    m_contextMenuIsOpened = false;
}


void WldScene::setRectDrawer()
{
    if(m_cursorItemImg)
    {
        delete m_cursorItemImg;
        m_cursorItemImg = nullptr;
    }

    QPen pen;
    QBrush brush;


    pen = QPen(Qt::gray, 2);
    brush = QBrush(Qt::darkGray);

    //Align width and height to fit into item aligning
    long addW = WldPlacingItems::gridSz - WldPlacingItems::itemW % WldPlacingItems::gridSz;
    long addH = WldPlacingItems::gridSz - WldPlacingItems::itemH % WldPlacingItems::gridSz;
    if(addW == WldPlacingItems::gridSz) addW = 0;
    if(addH == WldPlacingItems::gridSz) addH = 0;
    WldPlacingItems::itemW = int(WldPlacingItems::itemW + addW);
    WldPlacingItems::itemH = int(WldPlacingItems::itemH + addH);

    QPixmap oneCell(WldPlacingItems::itemW, WldPlacingItems::itemH);
    oneCell.fill(QColor(0xFF, 0xFF, 0x00, 128));
    QPainter p(&oneCell);
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(Qt::yellow, 2, Qt::SolidLine));
    p.drawRect(0, 0, WldPlacingItems::itemW, WldPlacingItems::itemH);
    brush.setTexture(oneCell);

    m_cursorItemImg = addRect(0, 0, 1, 1, pen, brush);

    //set data flags
    for(const dataFlag_w &itemFlag : WldPlacingItems::flags)
        m_cursorItemImg->setData(itemFlag.first, itemFlag.second);

    m_cursorItemImg->setData(0, "Square");
    m_cursorItemImg->setData(25, "CURSOR");
    m_cursorItemImg->setZValue(7000);
    m_cursorItemImg->setOpacity(0.5);
    m_cursorItemImg->setVisible(false);
    m_cursorItemImg->setEnabled(true);

    SwitchEditingMode(MODE_DrawRect);
}

void WldScene::setCircleDrawer()
{
    if(m_cursorItemImg)
    {
        delete m_cursorItemImg;
        m_cursorItemImg = nullptr;
    }

    QPen pen;
    QBrush brush;


    pen = QPen(Qt::gray, 2);
    brush = QBrush(Qt::darkGray);

    //Align width and height to fit into item aligning
    long addW = WldPlacingItems::gridSz - WldPlacingItems::itemW % WldPlacingItems::gridSz;
    long addH = WldPlacingItems::gridSz - WldPlacingItems::itemH % WldPlacingItems::gridSz;
    if(addW == WldPlacingItems::gridSz) addW = 0;
    if(addH == WldPlacingItems::gridSz) addH = 0;
    WldPlacingItems::itemW = int(WldPlacingItems::itemW + addW);
    WldPlacingItems::itemH = int(WldPlacingItems::itemH + addH);

    QPixmap oneCell(WldPlacingItems::itemW, WldPlacingItems::itemH);
    oneCell.fill(QColor(0xFF, 0xFF, 0x00, 128));
    QPainter p(&oneCell);
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(Qt::yellow, 2, Qt::SolidLine));
    p.drawRect(0, 0, WldPlacingItems::itemW, WldPlacingItems::itemH);
    brush.setTexture(oneCell);

    m_cursorItemImg = addEllipse(0, 0, 1, 1, pen, brush);

    //set data flags
    for(const dataFlag_w &itemFlag : WldPlacingItems::flags)
        m_cursorItemImg->setData(itemFlag.first, itemFlag.second);

    m_cursorItemImg->setData(0, "Circle");
    m_cursorItemImg->setData(25, "CURSOR");
    m_cursorItemImg->setZValue(7000);
    m_cursorItemImg->setOpacity(0.5);
    m_cursorItemImg->setVisible(false);
    m_cursorItemImg->setEnabled(true);

    SwitchEditingMode(MODE_DrawCircle);
}

void WldScene::setLineDrawer()
{
    if(m_cursorItemImg)
    {
        delete m_cursorItemImg;
        m_cursorItemImg = nullptr;
    }

    QPen pen;

    pen = QPen(Qt::transparent, 2);

    //Align width and height to fit into item aligning
    long addW = WldPlacingItems::gridSz - WldPlacingItems::itemW % WldPlacingItems::gridSz;
    long addH = WldPlacingItems::gridSz - WldPlacingItems::itemH % WldPlacingItems::gridSz;
    if(addW == WldPlacingItems::gridSz) addW = 0;
    if(addH == WldPlacingItems::gridSz) addH = 0;
    WldPlacingItems::itemW = int(WldPlacingItems::itemW + addW);
    WldPlacingItems::itemH = int(WldPlacingItems::itemH + addH);

    m_cursorItemImg = addLine(0, 0, 1, 1, pen);

    //set data flags
    for(const dataFlag_w &itemFlag : WldPlacingItems::flags)
        m_cursorItemImg->setData(itemFlag.first, itemFlag.second);

    m_cursorItemImg->setData(0, "Line");
    m_cursorItemImg->setData(25, "CURSOR");
    m_cursorItemImg->setZValue(7000);
    m_cursorItemImg->setOpacity(0.5);
    m_cursorItemImg->setVisible(false);
    m_cursorItemImg->setEnabled(true);

    SwitchEditingMode(MODE_Line);

}

void WldScene::setFloodFiller()
{
    SwitchEditingMode(MODE_Fill);
}

void WldScene::resetCursor()
{
    if(m_cursorItemImg)
    {
        delete m_cursorItemImg;
        m_cursorItemImg = nullptr;
    }

    m_busyMode = false;
    QPixmap cur(QSize(1, 1));
    cur.fill(Qt::black);
    m_cursorItemImg = addPixmap(QPixmap(cur));
    m_cursorItemImg->setZValue(1000);
    m_cursorItemImg->hide();
}

void WldScene::setMessageBoxItem(bool show, QPointF pos, QString text)
{
    if(m_labelBox)
    {
        if(!show)
        {
            delete m_labelBox;
            m_labelBox = nullptr;
            return;
        }

        if(text != m_labelBox->text())
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
