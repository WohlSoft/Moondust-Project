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

#include <editing/_dialogs/itemselectdialog.h>
#include <common_features/util.h>
#include <common_features/logger.h>
#include <common_features/mainwinconnect.h>

#include "item_level.h"

ItemLevel::ItemLevel(QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    construct();
}

ItemLevel::ItemLevel(WldScene *parentScene, QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    construct();
    if(!parentScene) return;
    setScenePoint(parentScene);
    scene->addItem(this);
    setZValue(scene->levelZ);
}

void ItemLevel::construct()
{
    gridSize=32;
    gridOffsetX=0;
    gridOffsetY=0;
    isLocked=false;
    imgOffsetX=0;
    imgOffsetY=0;

    animatorID=-1;
    imageSize = QRectF(0,0,10,10);

    mouseLeft=false;
    mouseMid=false;
    mouseRight=false;

    setData(ITEM_TYPE, "LEVEL");
    setData(ITEM_IS_ITEM, 1);
}

ItemLevel::~ItemLevel()
{}



void ItemLevel::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent )
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

    QGraphicsItem::mousePressEvent(mouseEvent);
}

void ItemLevel::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
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
        if((!scene->lock_level)&&(!scene->DrawMode)&&(!isLocked))
        {
            contextMenu(mouseEvent);
        }
    }
}

void ItemLevel::contextMenu( QGraphicsSceneMouseEvent * mouseEvent )
{
    //Remove selection from non-bgo items
    if(!this->isSelected())
    {
        scene->clearSelection();
        this->setSelected(true);
    }

    this->setSelected(1);
    QMenu ItemMenu;

    QAction *LvlTitle = ItemMenu.addAction(QString("[%1]").arg(levelData.title));
        LvlTitle->setEnabled(false);
        LvlTitle->setVisible(!levelData.title.isEmpty());

    QAction *openLvl = ItemMenu.addAction(tr("Open target file: %1").arg(levelData.lvlfile));
        openLvl->setVisible( (!levelData.lvlfile.isEmpty()) && (QFile(scene->WldData->path + "/" + levelData.lvlfile).exists()) );
        ItemMenu.addSeparator();

    QAction *setPathBG = ItemMenu.addAction(tr("Path background"));
        setPathBG->setCheckable(true);
        setPathBG->setChecked(levelData.pathbg);

    QAction *setBigPathBG = ItemMenu.addAction(tr("Big Path background"));
        setBigPathBG->setCheckable(true);
        setBigPathBG->setChecked(levelData.bigpathbg);

    QAction *setAlVis = ItemMenu.addAction(tr("Always Visible"));
        setAlVis->setCheckable(true);
        setAlVis->setChecked(levelData.alwaysVisible);
        ItemMenu.addSeparator();
    QAction *copyTile = ItemMenu.addAction(tr("Copy"));
    QAction *cutTile = ItemMenu.addAction(tr("Cut"));
        ItemMenu.addSeparator();
    QAction *transform = ItemMenu.addAction(tr("Transform into"));
    QAction *transform_all = ItemMenu.addAction(tr("Transform all %1 into").arg("LEVEL-%1").arg(levelData.id));
        ItemMenu.addSeparator();
    QAction *remove = ItemMenu.addAction(tr("Remove"));
        ItemMenu.addSeparator();
    QAction *props = ItemMenu.addAction(tr("Properties..."));

    scene->contextMenuOpened = true; //bug protector
QAction *selected = ItemMenu.exec(mouseEvent->screenPos());

    if(!selected)
    {
        #ifdef _DEBUG_
        WriteToLog(QtDebugMsg, "Context Menu <- NULL");
        #endif
        return;
    }

    if(selected==openLvl)
    {
        MainWinConnect::pMainWin->OpenFile(scene->WldData->path + "/" + levelData.lvlfile);
        scene->contextMenuOpened = false;
    }
    else
    if(selected==setPathBG)
    {
        scene->contextMenuOpened = false;
        WorldData selData;
        foreach(QGraphicsItem * SelItem, scene->selectedItems() )
        {
            if(SelItem->data(0).toString()=="LEVEL")
            {
                selData.levels << ((ItemLevel *)SelItem)->levelData;
                ((ItemLevel *)SelItem)->setPath( setPathBG->isChecked() );
            }
        }
        scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_PATHBACKGROUND, QVariant(setPathBG->isChecked()));
    }
    else
    if(selected==setBigPathBG)
    {
        scene->contextMenuOpened = false;
        WorldData selData;
        foreach(QGraphicsItem * SelItem, scene->selectedItems() )
        {
            if(SelItem->data(0).toString()=="LEVEL")
            {
                selData.levels << ((ItemLevel *)SelItem)->levelData;
                ((ItemLevel *)SelItem)->setbPath( setBigPathBG->isChecked() );
            }
        }
        scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_BIGPATHBACKGROUND, QVariant(setPathBG->isChecked()));
    }
    else
    if(selected==setAlVis)
    {
        scene->contextMenuOpened = false;
        WorldData selData;
        foreach(QGraphicsItem * SelItem, scene->selectedItems() )
        {
            if(SelItem->data(0).toString()=="LEVEL")
            {
                selData.levels << ((ItemLevel *)SelItem)->levelData;
                ((ItemLevel *)SelItem)->alwaysVisible( setAlVis->isChecked() );
            }
        }
        scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_ALWAYSVISIBLE, QVariant(setPathBG->isChecked()));
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
    if(selected==transform)
    {
        int transformTO;
        ItemSelectDialog * itemList = new ItemSelectDialog(scene->pConfigs, ItemSelectDialog::TAB_LEVEL);
        itemList->removeEmptyEntry(ItemSelectDialog::TAB_LEVEL);
        util::DialogToCenter(itemList, true);
        if(itemList->exec()==QDialog::Accepted)
        {
            transformTO = itemList->levelID;
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(ITEM_TYPE).toString()=="LEVEL")
                {
                    ((ItemLevel *) SelItem)->transformTo(transformTO);
                }
            }
        }
        delete itemList;
    }
    else
    if(selected==transform_all)
    {
        int transformTO;
        ItemSelectDialog * itemList = new ItemSelectDialog(scene->pConfigs, ItemSelectDialog::TAB_LEVEL);
        itemList->removeEmptyEntry(ItemSelectDialog::TAB_LEVEL);
        util::DialogToCenter(itemList, true);
        if(itemList->exec()==QDialog::Accepted)
        {
            transformTO = itemList->levelID;
            unsigned long oldID = levelData.id;
            foreach(QGraphicsItem * SelItem, scene->items() )
            {
                if(SelItem->data(ITEM_TYPE).toString()=="LEVEL")
                {
                    if(((ItemLevel *) SelItem)->levelData.id==oldID)
                        ((ItemLevel *) SelItem)->transformTo(transformTO);
                }
            }
        }
        delete itemList;
    }
    else
    if(selected==remove)
    {
        scene->removeSelectedWldItems();
    }
    else
    if(selected==props)
    {
        scene->openProps();
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

void ItemLevel::transformTo(long target_id)
{
    if(target_id<1) return;

    bool noimage=true;
    long item_i=0;
    long animator=0;
    obj_w_level mergedSet;

    //Get Level settings
    scene->getConfig_Level(target_id, item_i, animator, mergedSet, &noimage);

    if(noimage)
        return;//Don't transform, target item is not found

    levelData.id = target_id;
    setLevelData(levelData, &mergedSet, &animator);
    arrayApply();

    if(!scene->opts.animationEnabled)
        scene->update();
}

void ItemLevel::arrayApply()
{
    bool found=false;
    levelData.x = qRound(this->scenePos().x());
    levelData.y = qRound(this->scenePos().y());

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

void ItemLevel::returnBack()
{
    setPos(levelData.x, levelData.y);
}

int ItemLevel::getGridSize()
{
    return gridSize;
}

QPoint ItemLevel::sourcePos()
{
    return QPoint(levelData.x, levelData.y);
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

void ItemLevel::setLevelData(WorldLevels inD, obj_w_level *mergedSet,
                             long *animator_id, long *path_id, long *bPath_id)
{
    levelData = inD;

    setPos(levelData.x, levelData.y);

    setData(ITEM_ID, QString::number(levelData.id) );
    setData(ITEM_ARRAY_ID, QString::number(levelData.array_id) );

    if(mergedSet)
    {
        localProps = *mergedSet;
        gridSize = localProps.grid;
    }

    if(animator_id && path_id && bPath_id)
        setAnimator(*animator_id, *path_id, *bPath_id);

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

    this->setData(ITEM_WIDTH, QString::number( gridSize ) ); //width
    this->setData(ITEM_HEIGHT, QString::number( gridSize ) ); //height
    //WriteToLog(QtDebugMsg, QString("Tile Animator ID: %1").arg(aniID));

    pathID = path;
    bPathID = bPath;
    animatorID = aniID;
}

