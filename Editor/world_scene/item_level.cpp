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

#include "item_level.h"
#include "../common_features/logger.h"

#include "../common_features/mainwinconnect.h"


ItemLevel::ItemLevel(QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    gridSize=32;
    gridOffsetX=0;
    gridOffsetY=0;
    isLocked=false;
    imgOffsetX=0;
    imgOffsetY=0;

    animatorID=-1;
    imageSize = QRectF(0,0,10,10);
}


ItemLevel::~ItemLevel()
{
    //WriteToLog(QtDebugMsg, "!<-BGO destroyed->!");
    //if(timer) delete timer;
}

void ItemLevel::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
    if(scene->DrawMode)
    {
        unsetCursor();
        ungrabMouse();
        this->setSelected(false);
        return;
    }
    QGraphicsItem::mousePressEvent(mouseEvent);
}

void ItemLevel::contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
{
    if((!scene->lock_level)&&(!scene->DrawMode)&&(!isLocked))
    {
        //Remove selection from non-bgo items
        if(this->isSelected())
        {
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(0).toString()!="LEVEL") SelItem->setSelected(false);
            }
        }
        else
        {
            scene->clearSelection();
            this->setSelected(true);
        }

        this->setSelected(1);
        ItemMenu->clear();
        QAction *setPathBG = ItemMenu->addAction(tr("Path background"));
        setPathBG->setCheckable(true);
        setPathBG->setChecked(levelData.pathbg);
        setPathBG->deleteLater();

        QAction *setBigPathBG = ItemMenu->addAction(tr("Big Path background"));
        setBigPathBG->setCheckable(true);
        setBigPathBG->setChecked(levelData.bigpathbg);
        setBigPathBG->deleteLater();

        QAction *setAlVis = ItemMenu->addAction(tr("Always Visible"));
        setAlVis->setCheckable(true);
        setAlVis->setChecked(levelData.alwaysVisible);
        setAlVis->deleteLater();

        ItemMenu->addSeparator()->deleteLater();;
        QAction *copyTile = ItemMenu->addAction(tr("Copy"));
        copyTile->deleteLater();
        QAction *cutTile = ItemMenu->addAction(tr("Cut"));
        cutTile->deleteLater();
        ItemMenu->addSeparator()->deleteLater();
        QAction *remove = ItemMenu->addAction(tr("Remove"));
        remove->deleteLater();
        ItemMenu->addSeparator()->deleteLater();;
        QAction *props = ItemMenu->addAction(tr("Properties..."));
        props->deleteLater();

        scene->contextMenuOpened = true; //bug protector
QAction *selected = ItemMenu->exec(event->screenPos());

        if(!selected)
        {
            #ifdef _DEBUG_
            WriteToLog(QtDebugMsg, "Context Menu <- NULL");
            #endif
            scene->contextMenuOpened = true;
            return;
        }
        event->accept();

        if(selected==setPathBG)
        {
            scene->contextMenuOpened = false;
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(0).toString()=="LEVEL")
                {
                    ((ItemLevel *)SelItem)->setPath( setPathBG->isChecked() );
                }
            }
        }
        else
        if(selected==setBigPathBG)
        {
            scene->contextMenuOpened = false;
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(0).toString()=="LEVEL")
                {
                    ((ItemLevel *)SelItem)->setbPath( setBigPathBG->isChecked() );
                }
            }
        }
        else
        if(selected==setAlVis)
        {
            scene->contextMenuOpened = false;
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(0).toString()=="LEVEL")
                {
                    ((ItemLevel *)SelItem)->alwaysVisible( setAlVis->isChecked() );
                }
            }
        }
        else
        if(selected==cutTile)
        {
            //scene->doCut = true ;
            MainWinConnect::pMainWin->on_actionCut_triggered();
            scene->contextMenuOpened = false;
        }
        else
        if(selected==copyTile)
        {
            //scene->doCopy = true ;
            MainWinConnect::pMainWin->on_actionCopy_triggered();
            scene->contextMenuOpened = false;
        }
        else
        if(selected==remove)
        {
            WorldData removedItems;
            //bool deleted=false;
            scene->contextMenuOpened = false;
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(0).toString()=="LEVEL")
                {
                    removedItems.levels.push_back(((ItemLevel *)SelItem)->levelData);
                    ((ItemLevel *)SelItem)->removeFromArray();
                    scene->removeItem(SelItem);
                    delete SelItem;
                    //deleted=true;
                }
            }
            //if(deleted) MainWinConnect::pMainWin->activeLvlEditWin()->scene->addRemoveHistory( removedItems );
        }
        else
        if(selected==props)
        {
            scene->openProps();
        }
//        else
//        {
//            bool itemIsFound=false;
//            QString lName;
//            if(selected==newLayer)
//            {
//                scene->contextMenuOpened = false;
//                ToNewLayerBox * layerBox = new ToNewLayerBox(scene->WldData);
//                layerBox->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
//                layerBox->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, layerBox->size(), qApp->desktop()->availableGeometry()));
//                if(layerBox->exec()==QDialog::Accepted)
//                {
//                    itemIsFound=true;
//                    lName = layerBox->lName;

//                    //Store new layer into array
//                    LevelLayers nLayer;
//                    nLayer.name = lName;
//                    nLayer.hidden = layerBox->lHidden;
//                    scene->WldData->layers_array_id++;
//                    nLayer.array_id = scene->WldData->layers_array_id;
//                    scene->WldData->layers.push_back(nLayer);

//                    //scene->SyncLayerList=true; //Refresh layer list
//                    MainWinConnect::pMainWin->setLayerToolsLocked(true);
//                    MainWinConnect::pMainWin->setLayersBox();
//                    MainWinConnect::pMainWin->setLayerToolsLocked(false);
//                }
//                delete layerBox;
//            }
//            else
//            foreach(QAction * lItem, layerItems)
//            {
//                if(selected==lItem)
//                {
//                    itemIsFound=true;
//                    lName = lItem->data().toString();
//                    //FOUND!!!
//                 break;
//                }//Find selected layer's item
//            }

//            if(itemIsFound)
//            {
//                LevelData modData;
//                foreach(LevelLayers lr, scene->WldData->layers)
//                { //Find layer's settings
//                    if(lr.name==lName)
//                    {
//                        foreach(QGraphicsItem * SelItem, scene->selectedItems() )
//                        {

//                            if(SelItem->data(0).toString()=="BGO")
//                            {
//                                modData.bgo.push_back(((ItemLevel*) SelItem)->levelData);
//                                ((ItemLevel *) SelItem)->levelData.layer = lr.name;
//                                ((ItemLevel *) SelItem)->setVisible(!lr.hidden);
//                                ((ItemLevel *) SelItem)->arrayApply();
//                            }
//                        }
//                        if(selected==newLayer){
//                            scene->addChangedNewLayerHistory(modData, lr);
//                        }
//                        break;
//                    }
//                }//Find layer's settings
//                if(selected!=newLayer){
//                    scene->addChangedLayerHistory(modData, lName);
//                }
//                scene->contextMenuOpened = false;
//            }
//        }
    }
    else
    {
        QGraphicsItem::contextMenuEvent(event);
    }
}


///////////////////MainArray functions/////////////////////////////
//void ItemLevel::setLayer(QString layer)
//{
//    foreach(LevelLayers lr, scene->WldData->layers)
//    {
//        if(lr.name==layer)
//        {
//            levelData.layer = layer;
//            this->setVisible(!lr.hidden);
//            arrayApply();
//        break;
//        }
//    }
//}

void ItemLevel::arrayApply()
{
    bool found=false;
    if(levelData.index < (unsigned int)scene->WldData->levels.size())
    { //Check index
        if(levelData.array_id == scene->WldData->levels[levelData.index].array_id)
        {
            found=true;
        }
    }

    //Apply current data in main array
    if(found)
    { //directlry
        scene->WldData->levels[levelData.index] = levelData; //apply current levelData
    }
    else
    for(int i=0; i<scene->WldData->levels.size(); i++)
    { //after find it into array
        if(scene->WldData->levels[i].array_id == levelData.array_id)
        {
            levelData.index = i;
            scene->WldData->levels[i] = levelData;
            break;
        }
    }
}

void ItemLevel::removeFromArray()
{
    bool found=false;
    if(levelData.index < (unsigned int)scene->WldData->levels.size())
    { //Check index
        if(levelData.array_id == scene->WldData->levels[levelData.index].array_id)
        {
            found=true;
        }
    }

    if(found)
    { //directlry
        scene->WldData->levels.remove(levelData.index);
    }
    else
    for(int i=0; i<scene->WldData->levels.size(); i++)
    {
        if(scene->WldData->levels[i].array_id == levelData.array_id)
        {
            scene->WldData->levels.remove(i); break;
        }
    }
}


void ItemLevel::alwaysVisible(bool v)
{
    levelData.alwaysVisible = v;
    arrayApply();
}

void ItemLevel::setPath(bool p)
{
    levelData.pathbg=p;
    arrayApply();
    this->update();
}

void ItemLevel::setbPath(bool p)
{
    levelData.bigpathbg=p;
    arrayApply();

    this->update();

    imageSizeTarget=imageSize;

    if(p)
    {
        if(imageSizeTarget.left() > imageSizeBP.left())
            imageSizeTarget.setLeft( imageSizeBP.left() );
        if(imageSizeTarget.right() < imageSizeBP.right())
            imageSizeTarget.setRight( imageSizeBP.right() );
        if(imageSizeTarget.top() > imageSizeBP.top())
            imageSizeTarget.setTop( imageSizeBP.top() );
        if(imageSizeTarget.bottom() < imageSizeBP.bottom())
            imageSizeTarget.setBottom( imageSizeBP.bottom() );
    }
    scene->update();
}

void ItemLevel::setLevelData(WorldLevels inD)
{
    levelData = inD;
}


QRectF ItemLevel::boundingRect() const
{
    return imageSizeTarget;
}

void ItemLevel::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if(animatorID<0)
    {
        painter->drawRect(QRect(imgOffsetX,imgOffsetY,1,1));
        return;
    }

    if(levelData.bigpathbg)
    {
        if(scene->animates_Levels.size()>bPathID)
            painter->drawPixmap(imageSizeBP, scene->animates_Levels[bPathID]->image(), scene->animates_Levels[bPathID]->image().rect());
        else
            painter->drawRect(QRect(0,0,32,32));
    }


    if(levelData.pathbg)
    {
        if(scene->animates_Levels.size()>pathID)
            painter->drawPixmap(imageSizeP, scene->animates_Levels[pathID]->image(), scene->animates_Levels[pathID]->image().rect());
        else
            painter->drawRect(QRect(0,0,32,32));
    }

    if(scene->animates_Levels.size()>animatorID)
        painter->drawPixmap(imageSize, scene->animates_Levels[animatorID]->image(), scene->animates_Levels[animatorID]->image().rect());
    else
        painter->drawRect(QRect(0,0,32,32));

    if(this->isSelected())
    {
        painter->setPen(QPen(QBrush(Qt::black), 2, Qt::SolidLine));
        painter->drawRect(imgOffsetX+1,imgOffsetY+1,imageSize.width()-2,imageSize.height()-2);
        painter->setPen(QPen(QBrush(Qt::magenta), 2, Qt::DotLine));
        painter->drawRect(imgOffsetX+1,imgOffsetY+1,imageSize.width()-2,imageSize.height()-2);
    }
}

void ItemLevel::setContextMenu(QMenu &menu)
{
    ItemMenu = &menu;
}

void ItemLevel::setScenePoint(WldScene *theScene)
{
    scene = theScene;
}


////////////////Animation///////////////////

void ItemLevel::setAnimator(long aniID, long path, long bPath)
{
    if(aniID < scene->animates_Levels.size())
    {
    imgOffsetX = (int)qRound( -( qreal(scene->animates_Levels[aniID]->image().width() - gridSize)  / 2 ) );
    imgOffsetY = (int)qRound( -qreal(scene->animates_Levels[aniID]->image().height()) + gridSize );
    imageSize = QRectF(imgOffsetX,imgOffsetY,
                scene->animates_Levels[aniID]->image().width(),
                scene->animates_Levels[aniID]->image().height()
                );
    }

    if(path < scene->animates_Levels.size())
    {
    imgOffsetXp = (int)qRound( -( qreal(scene->animates_Levels[path]->image().width() - gridSize)  / 2 ) );
    imgOffsetYp = (int)qRound( -qreal(scene->animates_Levels[path]->image().height()) + gridSize );
    imageSizeP = QRectF(imgOffsetXp,imgOffsetYp,
                scene->animates_Levels[path]->image().width(),
                scene->animates_Levels[path]->image().height()
                );
    }

    if(bPath < scene->animates_Levels.size())
    {
    imgOffsetXbp = (int)qRound( -( qreal(scene->animates_Levels[bPath]->image().width() - gridSize)  / 2 ) );
    imgOffsetYbp = (int)qRound( -qreal(scene->animates_Levels[bPath]->image().height()) + qreal(gridSize)*1.25);
    imageSizeBP = QRectF(imgOffsetXbp,imgOffsetYbp,
                scene->animates_Levels[bPath]->image().width(),
                scene->animates_Levels[bPath]->image().height()
                );
    }


    imageSizeTarget=imageSize;

    if(levelData.bigpathbg)
    {
        if(imageSizeTarget.left() > imageSizeBP.left())
            imageSizeTarget.setLeft( imageSizeBP.left() );
        if(imageSizeTarget.right() < imageSizeBP.right())
            imageSizeTarget.setRight( imageSizeBP.right() );
        if(imageSizeTarget.top() > imageSizeBP.top())
            imageSizeTarget.setTop( imageSizeBP.top() );
        if(imageSizeTarget.bottom() < imageSizeBP.bottom())
            imageSizeTarget.setBottom( imageSizeBP.bottom() );
    }

    this->setData(9, QString::number( gridSize ) ); //width
    this->setData(10, QString::number( gridSize ) ); //height
    //WriteToLog(QtDebugMsg, QString("Tile Animator ID: %1").arg(aniID));

    pathID = path;
    bPathID = bPath;
    animatorID = aniID;
}
