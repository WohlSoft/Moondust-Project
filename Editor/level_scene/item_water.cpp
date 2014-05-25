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

#include "item_water.h"
#include "../common_features/logger.h"

#include "newlayerbox.h"

#include "../common_features/mainwinconnect.h"


ItemWater::ItemWater(QGraphicsPolygonItem *parent)
    : QGraphicsPolygonItem(parent)
{
    isLocked=false;
    waterSize = QSize(32,32);
    penWidth=2;

    _pen.setColor(Qt::darkBlue);
    _pen.setWidth(penWidth);
    _pen.setCapStyle(Qt::SquareCap);
    _pen.setJoinStyle(Qt::MiterJoin);
    _pen.setMiterLimit(0);
    this->setPen(_pen);

    waterData.w=32;
    waterData.h=32;
    waterData.x=this->pos().x();
    waterData.y=this->pos().y();
    waterData.quicksand=false;
}


ItemWater::~ItemWater()
{
 //   WriteToLog(QtDebugMsg, "!<-Water destroyed->!");
}

void ItemWater::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
    if(scene->DrawMode)
    {
        unsetCursor();
        ungrabMouse();
        this->setSelected(false);
        return;
    }
    QGraphicsPolygonItem::mousePressEvent(mouseEvent);
}

void ItemWater::contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
{
    if((!scene->lock_water)&&(!isLocked))
    {
        //Remove selection from non-bgo items
        if(this->isSelected())
        {
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(0).toString()!="Water") SelItem->setSelected(false);
            }
        }
        else
        {
            scene->clearSelection();
            this->setSelected(true);
        }

        this->setSelected(1);
        ItemMenu->clear();

        QMenu * LayerName = ItemMenu->addMenu(tr("Layer: ")+QString("[%1]").arg(waterData.layer));

        QAction *setLayer;
        QList<QAction *> layerItems;

        QAction * newLayer = LayerName->addAction(tr("Add to new layer..."));
            LayerName->addSeparator();

        foreach(LevelLayers layer, scene->LvlData->layers)
        {
            //Skip system layers
            if((layer.name=="Destroyed Blocks")||(layer.name=="Spawned NPCs")) continue;

            setLayer = LayerName->addAction( layer.name+((layer.hidden)?" [hidden]":"") );
            setLayer->setData(layer.name);
            setLayer->setCheckable(true);
            setLayer->setEnabled(true);
            setLayer->setChecked( layer.name==waterData.layer );
            layerItems.push_back(setLayer);
        }

        ItemMenu->addSeparator();

        QMenu * WaterType = ItemMenu->addMenu(tr("Environment type"));

        QAction *setAsWater = WaterType->addAction(tr("Water"));
            setAsWater->setCheckable(true);
            setAsWater->setChecked(!waterData.quicksand);

        QAction *setAsQuicksand = WaterType->addAction(tr("Quicksand"));
            setAsQuicksand->setCheckable(true);
            setAsQuicksand->setChecked(waterData.quicksand);

        ItemMenu->addSeparator();
        QAction *copyWater = ItemMenu->addAction(tr("Copy"));
        QAction *cutWater = ItemMenu->addAction(tr("Cut"));

        ItemMenu->addSeparator();
        QAction *remove = ItemMenu->addAction(tr("Remove"));

        scene->contextMenuOpened = true; //bug protector
QAction *selected = ItemMenu->exec(event->screenPos());

        if(!selected)
        {
            WriteToLog(QtDebugMsg, "Context Menu <- NULL");
            scene->contextMenuOpened = true;
            return;
        }
        event->accept();

        if(selected==cutWater)
        {
            //scene->doCut = true ;
            MainWinConnect::pMainWin->on_actionCut_triggered();
            scene->contextMenuOpened = false;
        }
        else
        if(selected==copyWater)
        {
            //scene->doCopy = true ;
            MainWinConnect::pMainWin->on_actionCopy_triggered();
            scene->contextMenuOpened = false;
        }
        else
        if(selected==setAsWater)
        {
            LevelData modData;
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(0).toString()=="Water")
                {
                    modData.water.push_back(((ItemWater *)SelItem)->waterData);
                    ((ItemWater *)SelItem)->setType(0);
                }
            }
            scene->addChangeSettingsHistory(modData, LvlScene::SETTING_WATERTYPE, QVariant(true));
            scene->contextMenuOpened = false;
        }
        else
        if(selected==setAsQuicksand)
        {
            LevelData modData;
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(0).toString()=="Water")
                {
                    modData.water.push_back(((ItemWater *)SelItem)->waterData);
                    ((ItemWater *)SelItem)->setType(1);
                }
            }
            scene->addChangeSettingsHistory(modData, LvlScene::SETTING_WATERTYPE, QVariant(false));
            scene->contextMenuOpened = false;
        }
        else
        if(selected==remove)
        {
            LevelData removedItems;
            bool deleted=false;
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(0).toString()=="Water")
                {
                    removedItems.water.push_back(((ItemWater *)SelItem)->waterData);
                    ((ItemWater *)SelItem)->removeFromArray();
                    scene->removeItem(SelItem);
                    deleted=true;
                }
            }
            if(deleted) scene->addRemoveHistory( removedItems );
            scene->contextMenuOpened = false;
        }
        else
        {
            bool itemIsFound=false;
            QString lName;
            if(selected==newLayer)
            {
                scene->contextMenuOpened = false;
                ToNewLayerBox * layerBox = new ToNewLayerBox(scene->LvlData);
                layerBox->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
                layerBox->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, layerBox->size(), qApp->desktop()->availableGeometry()));
                if(layerBox->exec()==QDialog::Accepted)
                {
                    itemIsFound=true;
                    lName = layerBox->lName;

                    //Store new layer into array
                    LevelLayers nLayer;
                    nLayer.name = lName;
                    nLayer.hidden = layerBox->lHidden;
                    scene->LvlData->layers_array_id++;
                    nLayer.array_id = scene->LvlData->layers_array_id;
                    scene->LvlData->layers.push_back(nLayer);

                    //scene->SyncLayerList=true; //Refresh layer list
                    MainWinConnect::pMainWin->setLayersBox();
                }
            }
            else
            foreach(QAction * lItem, layerItems)
            {
                if(selected==lItem)
                {
                    itemIsFound=true;
                    lName = lItem->data().toString();
                    //FOUND!!!
                 break;
                }//Find selected layer's item
            }

            if(itemIsFound)
            {
                foreach(LevelLayers lr, scene->LvlData->layers)
                { //Find layer's settings
                    if(lr.name==lName)
                    {
                        foreach(QGraphicsItem * SelItem, scene->selectedItems() )
                        {

                            if(SelItem->data(0).toString()=="Water")
                            {
                            ((ItemWater *) SelItem)->waterData.layer = lr.name;
                            ((ItemWater *) SelItem)->setVisible(!lr.hidden);
                            ((ItemWater *) SelItem)->arrayApply();
                            }
                        }
                    break;
                    }
                }//Find layer's settings
             scene->contextMenuOpened = false;
            }
        }
    }
    else
    {
        QGraphicsPolygonItem::contextMenuEvent(event);
    }
}


///////////////////MainArray functions/////////////////////////////
void ItemWater::setLayer(QString layer)
{
    foreach(LevelLayers lr, scene->LvlData->layers)
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
    if(waterData.index < (unsigned int)scene->LvlData->water.size())
    { //Check index
        if(waterData.array_id == scene->LvlData->water[waterData.index].array_id)
        {
            found=true;
        }
    }

    //Apply current data in main array
    if(found)
    { //directlry
        scene->LvlData->water[waterData.index] = waterData; //apply current bgoData
    }
    else
    for(int i=0; i<scene->LvlData->water.size(); i++)
    { //after find it into array
        if(scene->LvlData->water[i].array_id == waterData.array_id)
        {
            waterData.index = i;
            scene->LvlData->water[i] = waterData;
            break;
        }
    }
}

void ItemWater::removeFromArray()
{
    bool found=false;
    if(waterData.index < (unsigned int)scene->LvlData->water.size())
    { //Check index
        if(waterData.array_id == scene->LvlData->water[waterData.index].array_id)
        {
            found=true;
        }
    }

    if(found)
    { //directlry
        scene->LvlData->water.remove(waterData.index);
    }
    else
    for(int i=0; i<scene->LvlData->water.size(); i++)
    {
        if(scene->LvlData->water[i].array_id == waterData.array_id)
        {
            scene->LvlData->water.remove(i); break;
        }
    }
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


void ItemWater::setSize(QSize sz)
{
    waterSize = sz;
    waterData.w = sz.width();
    waterData.h = sz.height();
    drawWater();
    arrayApply();
}


void ItemWater::setWaterData(LevelWater inD)
{
    waterData = inD;
    waterSize = QSize(waterData.w, waterData.h);
    //drawWater();
}

void ItemWater::drawWater()
{
    long x, y, h, w;

    x = penWidth;//waterData.x;
    y = penWidth;//waterData.y;
    w = waterData.w-penWidth;
    h = waterData.h-penWidth;

    _pen.setColor(((waterData.quicksand)?Qt::yellow:Qt::green));
    this->setPen(_pen);

    //this->setPen(QPen(((waterData.quicksand)?Qt::yellow:Qt::green), 4));
    //this->setBrush(Qt::NoBrush);
    //this->setRect(x, y, w, h);

    QVector<QPointF > points;
    points.clear();
    // {{x, y},{x+w, y},{x+w,y+h},{x, y+h}}
    points.push_back(QPointF(x, y));
    points.push_back(QPointF(x+w, y));
    points.push_back(QPointF(x+w,y+h));
    points.push_back(QPointF(x, y+h));
    points.push_back(QPointF(x, y));

    points.push_back(QPointF(x, y+h));
    points.push_back(QPointF(x+w,y+h));
    points.push_back(QPointF(x+w, y));
    points.push_back(QPointF(x, y));

    this->setPolygon( QPolygonF(points) );
/*
    WriteToLog(QtDebugMsg, "WaterDraw -> ============================");
    WriteToLog(QtDebugMsg, QString("WaterDraw -> x=%1").arg(waterData.x));
    WriteToLog(QtDebugMsg, QString("WaterDraw -> y=%1").arg(waterData.y));
    WriteToLog(QtDebugMsg, QString("WaterDraw -> w=%1").arg(waterData.w));
    WriteToLog(QtDebugMsg, QString("WaterDraw -> h=%1").arg(waterData.h));
    WriteToLog(QtDebugMsg, QString("WaterDraw -> sand %1").arg((int)waterData.quicksand));

    WriteToLog(QtDebugMsg, QString("WaterDraw -> Drawesd x=%1").arg(this->pos().x()));
    WriteToLog(QtDebugMsg, QString("WaterDraw -> Drawesd y=%1").arg(this->pos().y()));
 */
}


QRectF ItemWater::boundingRect() const
{
    return QRectF(0,0,waterSize.width()+penWidth,waterSize.height()+penWidth);
}

void ItemWater::setContextMenu(QMenu &menu)
{
    ItemMenu = &menu;
}

void ItemWater::setScenePoint(LvlScene *theScene)
{
    scene = theScene;
}

