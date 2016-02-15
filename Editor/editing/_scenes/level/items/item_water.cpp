/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <common_features/mainwinconnect.h>
#include <common_features/logger.h>

#include "item_block.h"
#include "item_bgo.h"
#include "item_npc.h"
#include "item_water.h"
#include "item_door.h"
#include "../newlayerbox.h"

ItemWater::ItemWater(QGraphicsItem *parent)
    : LvlBaseItem(parent)
{
    construct();
}

ItemWater::ItemWater(LvlScene *parentScene, QGraphicsItem *parent)
    : LvlBaseItem(parentScene, parent)
{
    construct();
    if(!parentScene) return;
    setScenePoint(parentScene);
    scene->addItem(this);
    gridSize = scene->pConfigs->default_grid/2;
    setZValue(scene->Z_sys_PhysEnv);
    setLocked(scene->lock_water);
}

void ItemWater::construct()
{
    waterSize = QSize(32,32);
    penWidth=2;

    _color=QColor(Qt::darkBlue);
    _pen = QPen(_color, penWidth, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
    _pen.setWidth(penWidth);
    _pen.setCapStyle(Qt::FlatCap);
    _pen.setJoinStyle(Qt::MiterJoin);

    //this->setPen(_pen);
    //this->setBrush(QBrush(Qt::NoBrush));

    gridSize=16;

    waterData.w=32;
    waterData.h=32;
    waterData.x=this->pos().x();
    waterData.y=this->pos().y();
    waterData.env_type = LevelPhysEnv::ENV_WATER;

    setData(ITEM_TYPE, "Water");
    setData(ITEM_BLOCK_IS_SIZABLE, "sizable");
    setData(ITEM_WIDTH, (int)waterData.w);
    setData(ITEM_HEIGHT, (int)waterData.h);
}

ItemWater::~ItemWater()
{
    scene->unregisterElement(this);
}


void ItemWater::contextMenu( QGraphicsSceneMouseEvent * mouseEvent )
{
    scene->contextMenuOpened = true; //bug protector

    //Remove selection from non-bgo items
    if(!this->isSelected())
    {
        scene->clearSelection();
        this->setSelected(true);
    }

    this->setSelected(1);
    QMenu ItemMenu;

    QMenu * LayerName = ItemMenu.addMenu(tr("Layer: ")+QString("[%1]").arg(waterData.layer).replace("&", "&&&"));
    LayerName->deleteLater();

    QAction *setLayer;
    QList<QAction *> layerItems;

    QAction * newLayer = LayerName->addAction(tr("Add to new layer..."));
        LayerName->addSeparator()->deleteLater();
        newLayer->deleteLater();

    foreach(LevelLayer layer, scene->LvlData->layers)
    {
        //Skip system layers
        if((layer.name=="Destroyed Blocks")||(layer.name=="Spawned NPCs")) continue;

        setLayer = LayerName->addAction( layer.name.replace("&", "&&&")+((layer.hidden)?" [hidden]":"") );
        setLayer->setData(layer.name);
        setLayer->setCheckable(true);
        setLayer->setEnabled(true);
        setLayer->setChecked( layer.name==waterData.layer );
        setLayer->deleteLater();
        layerItems.push_back(setLayer);
    }

    ItemMenu.addSeparator();

    QMenu * WaterType = ItemMenu.addMenu(tr("Environment type"));
        WaterType->deleteLater();

    #define CONTEXT_MENU_ITEM_CHK(name, enable, label, checked_condition)\
        name = WaterType->addAction(tr(label));\
        name->setCheckable(true);\
        name->setEnabled(enable);\
        name->setChecked(checked_condition);\
        name->deleteLater(); typeID++;

    QAction *envTypes[13]; int typeID=0;

    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], true,                          "Water",        waterData.env_type==LevelPhysEnv::ENV_WATER);
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], true,                          "Quicksand",    waterData.env_type==LevelPhysEnv::ENV_QUICKSAND);
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], !scene->LvlData->smbx64strict, "Custom liquid", waterData.env_type==LevelPhysEnv::ENV_CUSTOM_LIQUID);
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], !scene->LvlData->smbx64strict, "Gravitational Field", waterData.env_type==LevelPhysEnv::ENV_GRAVITATIONAL_FIELD);
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], !scene->LvlData->smbx64strict, "Touch Event (Once)", waterData.env_type==LevelPhysEnv::ENV_TOUCH_EVENT_ONCE_PLAYER);
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], !scene->LvlData->smbx64strict, "Touch Event (Every frame)", waterData.env_type==LevelPhysEnv::ENV_TOUCH_EVENT_PLAYER);
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], !scene->LvlData->smbx64strict, "NPC Touch Event (Once)", waterData.env_type==LevelPhysEnv::ENV_TOUCH_EVENT_ONCE_NPC);
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], !scene->LvlData->smbx64strict, "NPC Touch Event (Every frame)", waterData.env_type==LevelPhysEnv::ENV_TOUCH_EVENT_NPC);
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], !scene->LvlData->smbx64strict, "Mouse click Event", waterData.env_type==LevelPhysEnv::ENV_CLICK_EVENT);
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], !scene->LvlData->smbx64strict, "Collision script", waterData.env_type==LevelPhysEnv::ENV_COLLISION_SCRIPT);
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], !scene->LvlData->smbx64strict, "Mouse click Script", waterData.env_type==LevelPhysEnv::ENV_CLICK_SCRIPT);
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], !scene->LvlData->smbx64strict, "Collision Event", waterData.env_type==LevelPhysEnv::ENV_COLLISION_EVENT);
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], !scene->LvlData->smbx64strict, "Air chamber", waterData.env_type==LevelPhysEnv::ENV_AIR);

    ItemMenu.addSeparator()->deleteLater();

    QMenu * copyPreferences = ItemMenu.addMenu(tr("Copy preferences"));
        copyPreferences->deleteLater();

            QAction *copyPosXYWH = copyPreferences->addAction(tr("Position: X, Y, Width, Height"));
                copyPosXYWH->deleteLater();
            QAction *copyPosLTRB = copyPreferences->addAction(tr("Position: Left, Top, Right, Bottom"));
                copyPosLTRB->deleteLater();

    ItemMenu.addSeparator()->deleteLater();

    QAction *resize = ItemMenu.addAction(tr("Resize"));
        resize->deleteLater();

    ItemMenu.addSeparator()->deleteLater();
    QAction *copyWater = ItemMenu.addAction(tr("Copy"));
        copyWater->deleteLater();
    QAction *cutWater = ItemMenu.addAction(tr("Cut"));
        cutWater->deleteLater();

    ItemMenu.addSeparator()->deleteLater();
    QAction *remove = ItemMenu.addAction(tr("Remove"));
        remove->deleteLater();

QAction *selected = ItemMenu.exec(mouseEvent->screenPos());

    if(!selected)
    {
        WriteToLog(QtDebugMsg, "Context Menu <- NULL");
        return;
    }

    if(selected==cutWater)
    {
        //scene->doCut = true ;
        MainWinConnect::pMainWin->on_actionCut_triggered();
    }
    else
    if(selected==copyWater)
    {
        //scene->doCopy = true ;
        MainWinConnect::pMainWin->on_actionCopy_triggered();
    }
    else
    if(selected==copyPosXYWH)
    {
        QApplication::clipboard()->setText(
                            QString("X=%1; Y=%2; W=%3; H=%4;")
                               .arg(waterData.x)
                               .arg(waterData.y)
                               .arg(waterData.w)
                               .arg(waterData.h)
                               );
        MainWinConnect::pMainWin->showStatusMsg(tr("Preferences has been copied: %1").arg(QApplication::clipboard()->text()));
    }
    else
    if(selected==copyPosLTRB)
    {
        QApplication::clipboard()->setText(
                            QString("Left=%1; Top=%2; Right=%3; Bottom=%4;")
                               .arg(waterData.x)
                               .arg(waterData.y)
                               .arg(waterData.x+waterData.w)
                               .arg(waterData.y+waterData.h)
                               );
        MainWinConnect::pMainWin->showStatusMsg(tr("Preferences has been copied: %1").arg(QApplication::clipboard()->text()));
    }
    else
    if(selected==resize)
    {
        scene->setPhysEnvResizer(this, true);
    }
    else
    if(selected==remove)
    {
        scene->removeSelectedLvlItems();
    }
    else
    if(selected==newLayer)
    {
        scene->setLayerToSelected();
    }
    else
    {
        bool found=false;
        //Fetch layers menu
        foreach(QAction * lItem, layerItems)
        {
            if(selected==lItem)
            {
                //FOUND!!!
                scene->setLayerToSelected(lItem->data().toString());
                found=true;
                break;
            }//Find selected layer's item
        }

        if(!found)
        {
            for(int i=0; i<typeID; i++)
            {
                if(selected==envTypes[i])
                {
                    LevelData modData;
                    foreach(QGraphicsItem * SelItem, scene->selectedItems() )
                    {
                        if(SelItem->data(ITEM_TYPE).toString()=="Water")
                        {
                            modData.physez.push_back(((ItemWater *)SelItem)->waterData);
                            ((ItemWater *)SelItem)->setType(i);
                        }
                    }
                    scene->addChangeSettingsHistory(modData, HistorySettings::SETTING_WATERTYPE, QVariant(true));
                    found = true;
                    break;
                }
            }
        }
    }
}

///////////////////MainArray functions/////////////////////////////
void ItemWater::setLayer(QString layer)
{
    foreach(LevelLayer lr, scene->LvlData->layers)
    {
        if(lr.name==layer)
        {
            waterData.layer = layer;
            this->setVisible(!lr.hidden);
            arrayApply();
        break;
        }
    }
}

void ItemWater::arrayApply()
{
    bool found=false;

    waterData.x = qRound(this->scenePos().x());
    waterData.y = qRound(this->scenePos().y());
    this->setData(ITEM_WIDTH, (int)waterData.w);
    this->setData(ITEM_HEIGHT, (int)waterData.h);

    if(waterData.index < (unsigned int)scene->LvlData->physez.size())
    { //Check index
        if(waterData.array_id == scene->LvlData->physez[waterData.index].array_id)
        {
            found=true;
        }
    }

    //Apply current data in main array
    if(found)
    { //directlry
        scene->LvlData->physez[waterData.index] = waterData; //apply current bgoData
    }
    else
    for(int i=0; i<scene->LvlData->physez.size(); i++)
    { //after find it into array
        if(scene->LvlData->physez[i].array_id == waterData.array_id)
        {
            waterData.index = i;
            scene->LvlData->physez[i] = waterData;
            break;
        }
    }

    //Update R-tree innex
    scene->unregisterElement(this);
    scene->registerElement(this);
}

void ItemWater::removeFromArray()
{
    bool found=false;
    if(waterData.index < (unsigned int)scene->LvlData->physez.size())
    { //Check index
        if(waterData.array_id == scene->LvlData->physez[waterData.index].array_id)
        {
            found=true;
        }
    }

    if(found)
    { //directlry
        scene->LvlData->physez.removeAt(waterData.index);
    }
    else
    for(int i=0; i<scene->LvlData->physez.size(); i++)
    {
        if(scene->LvlData->physez[i].array_id == waterData.array_id)
        {
            scene->LvlData->physez.removeAt(i); break;
        }
    }
}


void ItemWater::returnBack()
{
    this->setPos(waterData.x, waterData.y);
}

QPoint ItemWater::gridOffset()
{
    return QPoint(0, 0);
}

int ItemWater::getGridSize()
{
    return gridSize;
}

QPoint ItemWater::sourcePos()
{
    return QPoint(waterData.x, waterData.y);
}

void ItemWater::updateColor()
{
    switch(waterData.env_type)
    {
    case LevelPhysEnv::ENV_WATER:
    default:
        _color=QColor(Qt::green);
        break;
    case LevelPhysEnv::ENV_QUICKSAND:
        _color=QColor(Qt::yellow);
        break;
    case LevelPhysEnv::ENV_CUSTOM_LIQUID:
        _color=QColor(Qt::darkGreen);
        break;
    case LevelPhysEnv::ENV_AIR:
        _color=QColor(Qt::blue);
        break;
    case LevelPhysEnv::ENV_GRAVITATIONAL_FIELD:
        _color=QColor(Qt::cyan);
        break;
    case LevelPhysEnv::ENV_TOUCH_EVENT_ONCE_PLAYER:
        _color=QColor(Qt::darkRed);
        break;
    case LevelPhysEnv::ENV_TOUCH_EVENT_PLAYER:
        _color=QColor(Qt::red);
        break;
    case LevelPhysEnv::ENV_TOUCH_EVENT_ONCE_NPC:
        _color=QColor(Qt::darkMagenta);
        break;
    case LevelPhysEnv::ENV_TOUCH_EVENT_NPC:
        _color=QColor(Qt::magenta);
        break;
    case LevelPhysEnv::ENV_CLICK_EVENT:
        _color=QColor(Qt::darkCyan);
        break;
    case LevelPhysEnv::ENV_CLICK_SCRIPT:
        _color=QColor(Qt::darkCyan);
        break;
    case LevelPhysEnv::ENV_COLLISION_EVENT:
        _color=QColor(Qt::darkCyan);
        break;
    case LevelPhysEnv::ENV_COLLISION_SCRIPT:
        _color=QColor(Qt::darkCyan);
        break;
    }
    _pen.setColor(_color);
}

bool ItemWater::itemTypeIsLocked()
{
    if(!scene)
        return false;
    return scene->lock_water;
}

void ItemWater::setType(int tp)
{
    waterData.env_type=tp;
    updateColor();
    //this->setPen(_pen);
    arrayApply();
}

void ItemWater::setRectSize(QRect rect)
{
    waterData.x = rect.x();
    waterData.y = rect.y();
    waterData.w = rect.width();
    waterData.h = rect.height();
    waterSize = rect.size();
    setPos(waterData.x, waterData.y);
    drawWater();
    arrayApply();
}

void ItemWater::setSize(QSize sz)
{
    waterSize = sz;
    waterData.w = sz.width();
    waterData.h = sz.height();
    drawWater();
    arrayApply();
}


void ItemWater::setWaterData(LevelPhysEnv inD)
{
    waterData = inD;
    waterSize = QSize(waterData.w, waterData.h);
    setPos(waterData.x, waterData.y);
    setData(ITEM_ID, QString::number(0) );
    setData(ITEM_ARRAY_ID, QString::number(waterData.array_id) );
    updateColor();
    drawWater();
    scene->unregisterElement(this);
    scene->registerElement(this);
}

void ItemWater::drawWater()
{
//    long x, y, h, w;

//    x = 1;
//    y = 1;
//    w = waterData.w-penWidth;
//    h = waterData.h-penWidth;
    setData(ITEM_WIDTH,  (int)waterData.w);
    setData(ITEM_HEIGHT, (int)waterData.h);
    _pen=QPen(_color, penWidth, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
    //setPen(_pen);
//    QVector<QPointF > points;
//    points.clear();

//    points.push_back(QPointF(x+3, y));
//    points.push_back(QPointF(x+w, y));
//    points.push_back(QPointF(x+w,y+h));
//    points.push_back(QPointF(x, y+h));
//    points.push_back(QPointF(x, y+3));

//    points.push_back(QPointF(x, y+h));
//    points.push_back(QPointF(x+w,y+h));
//    points.push_back(QPointF(x+w, y));
//    points.push_back(QPointF(x+3, y));

//    this->setPolygon( QPolygonF(points) );
}

QRectF ItemWater::boundingRect() const
{
    return QRectF(-1,-1,
                  waterSize.width()+penWidth,
                  waterSize.height()+penWidth);
}

void ItemWater::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setPen(_pen);
    painter->setBrush(Qt::NoBrush);

    long h, w;
    w = waterData.w-penWidth;
    h = waterData.h-penWidth;

    painter->drawRect(1, 1, w, h);
    if(this->isSelected())
    {
        painter->setPen(QPen(QBrush(Qt::black), 2, Qt::SolidLine));
        painter->drawRect(1,1,w,h);
        painter->setPen(QPen(QBrush(Qt::white), 2, Qt::DotLine));
        painter->drawRect(1,1,w,h);
    }
}


