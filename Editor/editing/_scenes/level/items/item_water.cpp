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

ItemWater::ItemWater(QGraphicsPolygonItem *parent)
    : QGraphicsPolygonItem(parent)
{
    construct();
}

ItemWater::ItemWater(LvlScene *parentScene, QGraphicsPolygonItem *parent)
    : QGraphicsPolygonItem(parent)
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
    isLocked=false;
    waterSize = QSize(32,32);
    penWidth=2;

    _pen = QPen(Qt::darkBlue, penWidth, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
    _pen.setWidth(penWidth);
    _pen.setCapStyle(Qt::FlatCap);
    _pen.setJoinStyle(Qt::MiterJoin);

    this->setPen(_pen);
    this->setBrush(QBrush(Qt::NoBrush));

    gridSize=16;

    waterData.w=32;
    waterData.h=32;
    waterData.x=this->pos().x();
    waterData.y=this->pos().y();
    waterData.quicksand=false;

    setData(ITEM_TYPE, "Water");
    setData(ITEM_IS_ITEM, 1);
    setData(ITEM_BLOCK_IS_SIZABLE, "sizable");
    setData(ITEM_WIDTH, (int)waterData.w);
    setData(ITEM_HEIGHT, (int)waterData.h);

    mouseLeft=false;
    mouseMid=false;
    mouseRight=false;
}

ItemWater::~ItemWater()
{
    scene->unregisterElement(this);
}


void ItemWater::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
    if((this->flags()&QGraphicsItem::ItemIsSelectable)==0)
    {
        QGraphicsItem::mousePressEvent(mouseEvent); return;
    }

    if(scene->DrawMode)
    {
        unsetCursor();
        ungrabMouse();
        this->setSelected(false);
        return;
    }

    //Discard multi-mouse keys
    if((mouseLeft)||(mouseMid)||(mouseRight))
    {
        mouseEvent->accept();
        return;
    }

    if( mouseEvent->buttons() & Qt::LeftButton )
        mouseLeft=true;
    if( mouseEvent->buttons() & Qt::MiddleButton )
        mouseMid=true;
    if( mouseEvent->buttons() & Qt::RightButton )
        mouseRight=true;


    QGraphicsPolygonItem::mousePressEvent(mouseEvent);
}

void ItemWater::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    int multimouse=0;
    bool callContext=false;
    if(((mouseMid)||(mouseRight))&&( mouseLeft^(mouseEvent->buttons() & Qt::LeftButton) ))
        multimouse++;
    if( (((mouseLeft)||(mouseRight)))&&( mouseMid^(mouseEvent->buttons() & Qt::MiddleButton) ))
        multimouse++;
    if((((mouseLeft)||(mouseMid)))&&( mouseRight^(mouseEvent->buttons() & Qt::RightButton) ))
        multimouse++;
    if(multimouse>0)
    {
        mouseEvent->accept(); return;
    }

    if( mouseLeft^(mouseEvent->buttons() & Qt::LeftButton) )
        mouseLeft=false;

    if( mouseMid^(mouseEvent->buttons() & Qt::MiddleButton) )
        mouseMid=false;

    if( mouseRight^(mouseEvent->buttons() & Qt::RightButton) )
    {
        if(!scene->IsMoved) callContext=true;
        mouseRight=false;
    }

    QGraphicsItem::mouseReleaseEvent(mouseEvent);

    /////////////////////////CONTEXT MENU:///////////////////////////////
    if((callContext)&&(!scene->contextMenuOpened))
    {
        if((!scene->lock_water)&&(!isLocked))
        {
            contextMenu(mouseEvent);
        }
    }

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

    QAction *setAsWater = WaterType->addAction(tr("Water"));
        setAsWater->setCheckable(true);
        setAsWater->setChecked(!waterData.quicksand);
        setAsWater->deleteLater();

    QAction *setAsQuicksand = WaterType->addAction(tr("Quicksand"));
        setAsQuicksand->setCheckable(true);
        setAsQuicksand->setChecked(waterData.quicksand);
        setAsQuicksand->deleteLater();


    QAction *showRectangleValues = ItemMenu.addAction(tr("Show zone range data"));
        showRectangleValues->deleteLater();

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
    if(selected==setAsWater)
    {
        LevelData modData;
        foreach(QGraphicsItem * SelItem, scene->selectedItems() )
        {
            if(SelItem->data(ITEM_TYPE).toString()=="Water")
            {
                modData.physez.push_back(((ItemWater *)SelItem)->waterData);
                ((ItemWater *)SelItem)->setType(0);
            }
        }
        scene->addChangeSettingsHistory(modData, HistorySettings::SETTING_WATERTYPE, QVariant(true));
    }
    else
    if(selected==setAsQuicksand)
    {
        LevelData modData;
        foreach(QGraphicsItem * SelItem, scene->selectedItems() )
        {
            if(SelItem->data(ITEM_TYPE).toString()=="Water")
            {
                modData.physez.push_back(((ItemWater *)SelItem)->waterData);
                ((ItemWater *)SelItem)->setType(1);
            }
        }
        scene->addChangeSettingsHistory(modData, HistorySettings::SETTING_WATERTYPE, QVariant(false));
    }
    else
    if(selected==showRectangleValues)
    {
        QInputDialog rectangle;
        rectangle.setInputMode(QInputDialog::TextInput);
        rectangle.setOption(QInputDialog::NoButtons, true);
        rectangle.setWindowTitle(tr("Physical environment range"));
        rectangle.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter,
              QSize(400, 150), qApp->desktop()->availableGeometry()));
        rectangle.setLabelText(tr("Rectangle X(left), Y(top), Width, Height, Right, Bottom"));
        rectangle.setTextEchoMode(QLineEdit::Normal);
        rectangle.setTextValue(
                            QString("X=%1; Y=%2; W=%3; H=%4; Right: %5; Bottom %6;")
                               .arg(waterData.x)
                               .arg(waterData.y)
                               .arg(waterData.w)
                               .arg(waterData.h)
                               .arg(waterData.x+waterData.w)
                               .arg(waterData.y+waterData.h)
                               );
        rectangle.exec();

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
        //Fetch layers menu
        foreach(QAction * lItem, layerItems)
        {
            if(selected==lItem)
            {
                //FOUND!!!
                scene->setLayerToSelected(lItem->data().toString());
                break;
            }//Find selected layer's item
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


void ItemWater::setType(int tp)
{
    switch(tp)
    {
    case 1://Quicksand
        waterData.quicksand=true;
        _pen.setColor(Qt::yellow);
        this->setPen(_pen);
        break;
    case 0://Water
    default:
        waterData.quicksand=false;
        _pen.setColor(Qt::green);
        this->setPen(_pen);
        break;
    }
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
    drawWater();
    scene->unregisterElement(this);
    scene->registerElement(this);
}

void ItemWater::drawWater()
{
    long x, y, h, w;

    x = 1;
    y = 1;
    w = waterData.w-penWidth;
    h = waterData.h-penWidth;

    setData(ITEM_WIDTH, (int)waterData.w);
    setData(ITEM_HEIGHT, (int)waterData.h);

    setPen(QPen(((waterData.quicksand)?Qt::yellow:Qt::green), penWidth, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));

    QVector<QPointF > points;
    points.clear();

    points.push_back(QPointF(x+3, y));
    points.push_back(QPointF(x+w, y));
    points.push_back(QPointF(x+w,y+h));
    points.push_back(QPointF(x, y+h));
    points.push_back(QPointF(x, y+3));

    points.push_back(QPointF(x, y+h));
    points.push_back(QPointF(x+w,y+h));
    points.push_back(QPointF(x+w, y));
    points.push_back(QPointF(x+3, y));

    this->setPolygon( QPolygonF(points) );
}


void ItemWater::setLocked(bool lock)
{
    this->setFlag(QGraphicsItem::ItemIsSelectable, !lock);
    this->setFlag(QGraphicsItem::ItemIsMovable, !lock);
    isLocked = lock;
}


QRectF ItemWater::boundingRect() const
{
    return QRectF(-1,-1,waterSize.width()+penWidth,waterSize.height()+penWidth);
}

void ItemWater::setScenePoint(LvlScene *theScene)
{
    scene = theScene;
}

