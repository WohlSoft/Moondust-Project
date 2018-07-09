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

#include <QClipboard>

#include <editing/_dialogs/itemselectdialog.h>
#include <common_features/util.h>
#include <common_features/main_window_ptr.h>
#include <common_features/logger.h>

#include "item_tile.h"
#include "../wld_history_manager.h"

ItemTile::ItemTile(QGraphicsItem *parent)
    : WldBaseItem(parent)
{
    construct();

}

ItemTile::ItemTile(WldScene *parentScene, QGraphicsItem *parent)
    : WldBaseItem(parentScene, parent)
{
    construct();
    if(!parentScene) return;
    m_scene->addItem(this);
    setZValue(m_scene->Z_Terrain);
}

void ItemTile::construct()
{
    setData(ITEM_TYPE, "TILE");
}

ItemTile::~ItemTile()
{
    m_scene->unregisterElement(this);
}

void ItemTile::contextMenu( QGraphicsSceneMouseEvent * mouseEvent )
{
    m_scene->m_contextMenuIsOpened = true; //bug protector
    //Remove selection from non-bgo items
    if(!this->isSelected())
    {
        m_scene->clearSelection();
        this->setSelected(true);
    }

    setSelected(true);
    QMenu ItemMenu;

    QMenu * copyPreferences =   ItemMenu.addMenu(tr("Copy preferences"));
    QAction *copyItemID =       copyPreferences->addAction(tr("Tile-ID: %1").arg(m_data.id));
    QAction *copyPosXY =        copyPreferences->addAction(tr("Position: X, Y"));
    QAction *copyPosXYWH =      copyPreferences->addAction(tr("Position: X, Y, Width, Height"));
    QAction *copyPosLTRB =      copyPreferences->addAction(tr("Position: Left, Top, Right, Bottom"));

    QAction *copyTile =         ItemMenu.addAction(tr("Copy"));
    QAction *cutTile =          ItemMenu.addAction(tr("Cut"));
                                ItemMenu.addSeparator();
    QAction *transform =        ItemMenu.addAction(tr("Transform into"));
    QAction *transform_all =    ItemMenu.addAction(tr("Transform all %1 into").arg("TILE-%1").arg(m_data.id));
                                ItemMenu.addSeparator();
    QAction *remove =           ItemMenu.addAction(tr("Remove"));
    QAction *remove_all =       ItemMenu.addAction(tr("Remove all %1").arg("TILE-%1").arg(m_data.id));

    QAction *selected =         ItemMenu.exec(mouseEvent->screenPos());

    if(!selected) return;

    if(selected==copyItemID)
    {
        QApplication::clipboard()->setText(QString("%1").arg(m_data.id));
        MainWinConnect::pMainWin->showStatusMsg(tr("Preferences have been copied: %1").arg(QApplication::clipboard()->text()));
    }
    else
    if(selected==copyPosXY)
    {
        QApplication::clipboard()->setText(
                            QString("X=%1; Y=%2;")
                               .arg(m_data.x)
                               .arg(m_data.y)
                               );
        MainWinConnect::pMainWin->showStatusMsg(tr("Preferences have been copied: %1").arg(QApplication::clipboard()->text()));
    }
    else
    if(selected==copyPosXYWH)
    {
        QApplication::clipboard()->setText(
                            QString("X=%1; Y=%2; W=%3; H=%4;")
                               .arg(m_data.x)
                               .arg(m_data.y)
                               .arg(m_imageSize.width())
                               .arg(m_imageSize.height())
                               );
        MainWinConnect::pMainWin->showStatusMsg(tr("Preferences have been copied: %1").arg(QApplication::clipboard()->text()));
    }
    else
    if(selected==copyPosLTRB)
    {
        QApplication::clipboard()->setText(
                            QString("Left=%1; Top=%2; Right=%3; Bottom=%4;")
                               .arg(m_data.x)
                               .arg(m_data.y)
                               .arg(m_data.x+m_imageSize.width())
                               .arg(m_data.y+m_imageSize.height())
                               );
        MainWinConnect::pMainWin->showStatusMsg(tr("Preferences have been copied: %1").arg(QApplication::clipboard()->text()));
    }
    else
    if(selected==cutTile)
    {
        MainWinConnect::pMainWin->on_actionCut_triggered();
    }
    else
    if(selected==copyTile)
    {
        MainWinConnect::pMainWin->on_actionCopy_triggered();
    }
    else
    if((selected==transform)||(selected==transform_all))
    {
        WorldData oldData;
        WorldData newData;
        int transformTO;

        ItemSelectDialog * itemList = new ItemSelectDialog(m_scene->m_configs, ItemSelectDialog::TAB_TILE,0,0,0,0,0,0,0,0,0, m_scene->m_subWindow,ItemSelectDialog::TAB_TILE);
        util::DialogToCenter(itemList, true);

        if(itemList->exec()==QDialog::Accepted)
        {
            QList<QGraphicsItem *> our_items;
            bool sameID=false;
            transformTO = itemList->tileID;
            unsigned long oldID = m_data.id;

            if(selected==transform)
                our_items=m_scene->selectedItems();
            else
            if(selected==transform_all)
            {
                our_items=m_scene->items();
                sameID=true;
            }

            foreach(QGraphicsItem * SelItem, our_items )
            {
                if(SelItem->data(ITEM_TYPE).toString()=="TILE")
                {
                    if((!sameID)||(((ItemTile *) SelItem)->m_data.id==oldID))
                    {
                        oldData.tiles.push_back( ((ItemTile *) SelItem)->m_data );
                        ((ItemTile *) SelItem)->transformTo(transformTO);
                        newData.tiles.push_back( ((ItemTile *) SelItem)->m_data );
                    }
                }
            }
        }
        delete itemList;
        if(!newData.tiles.isEmpty())
            m_scene->m_history->addTransformHistory(newData, oldData);
    }
    else
    if(selected==remove)
    {
       m_scene->removeSelectedWldItems();
    }
    else
    if(selected==remove_all)
    {
        QList<QGraphicsItem *> our_items;
        QList<QGraphicsItem *> selectedList;
        unsigned long oldID = m_data.id;
        our_items = m_scene->items();

        foreach(QGraphicsItem * SelItem, our_items )
        {
            if(SelItem->data(ITEM_TYPE).toString()=="TILE")
            {
                if( ((ItemTile*) SelItem)->m_data.id == oldID)
                {
                    selectedList.push_back(SelItem);
                }
            }
        }
        if(!selectedList.isEmpty())
        {
            m_scene->removeWldItems(selectedList);
            m_scene->Debugger_updateItemList();
        }
    }
}


///////////////////MainArray functions/////////////////////////////
//void ItemTile::setLayer(QString layer)
//{
//    foreach(LevelLayers lr, scene->WldData->layers)
//    {
//        if(lr.name==layer)
//        {
//            tileData.layer = layer;
//            this->setVisible(!lr.hidden);
//            arrayApply();
//        break;
//        }
//    }
//}

void ItemTile::transformTo(long target_id)
{
    if(target_id<1) return;

    if(!m_scene->m_localConfigTerrain.contains(target_id))
        return;

    obj_w_tile &mergedSet = m_scene->m_localConfigTerrain[target_id];
    long animator=mergedSet.animator_id;

    m_data.id = target_id;
    setTileData(m_data, &mergedSet, &animator);
    arrayApply();

    if(!m_scene->m_opts.animationEnabled)
        m_scene->update();
}

void ItemTile::arrayApply()
{
    bool found=false;
    m_data.x = qRound(this->scenePos().x());
    m_data.y = qRound(this->scenePos().y());

    if(m_data.meta.index < (unsigned int)m_scene->m_data->tiles.size())
    { //Check index
        if(m_data.meta.array_id == m_scene->m_data->tiles[m_data.meta.index].meta.array_id)
        {
            found=true;
        }
    }

    //Apply current data in main array
    if(found)
    { //directlry
        m_scene->m_data->tiles[m_data.meta.index] = m_data; //apply current tileData
    }
    else
    for(int i=0; i<m_scene->m_data->tiles.size(); i++)
    { //after find it into array
        if(m_scene->m_data->tiles[i].meta.array_id == m_data.meta.array_id)
        {
            m_data.meta.index = i;
            m_scene->m_data->tiles[i] = m_data;
            break;
        }
    }

    //Mark world map as modified
    m_scene->m_data->meta.modified = true;

    m_scene->unregisterElement(this);
    m_scene->registerElement(this);
}

void ItemTile::removeFromArray()
{
    bool found=false;
    if(m_data.meta.index < (unsigned int)m_scene->m_data->tiles.size())
    { //Check index
        if(m_data.meta.array_id == m_scene->m_data->tiles[m_data.meta.index].meta.array_id)
        {
            found=true;
        }
    }

    if(found)
    { //directlry
        m_scene->m_data->tiles.removeAt(m_data.meta.index);
    }
    else
    for(int i=0; i<m_scene->m_data->tiles.size(); i++)
    {
        if(m_scene->m_data->tiles[i].meta.array_id == m_data.meta.array_id)
        {
            m_scene->m_data->tiles.removeAt(i); break;
        }
    }

    //Mark world map as modified
    m_scene->m_data->meta.modified = true;
}

void ItemTile::returnBack()
{
    setPos(m_data.x, m_data.y);
}

QPoint ItemTile::sourcePos()
{
    return QPoint(m_data.x, m_data.y);
}

bool ItemTile::itemTypeIsLocked()
{
    return m_scene->m_lockTerrain;
}


void ItemTile::setTileData(WorldTerrainTile inD, obj_w_tile *mergedSet, long *animator_id)
{
    m_data = inD;
    setPos(m_data.x, m_data.y);

    if(mergedSet)
    {
        m_localProps = *mergedSet;
        m_gridSize = m_localProps.setup.grid;
    }
    if(animator_id)
        setAnimator(*animator_id);

    setData(ITEM_ID, QString::number(m_data.id) );
    setData(ITEM_ARRAY_ID, QString::number(m_data.meta.array_id) );

    m_scene->unregisterElement(this);
    m_scene->registerElement(this);
}


QRectF ItemTile::boundingRect() const
{
    return m_imageSize;
}

void ItemTile::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if(m_animatorID<0)
    {
        painter->drawRect(QRect(0,0,1,1));
        return;
    }
    if(m_scene->m_animatorsTerrain.size()>m_animatorID)
        painter->drawPixmap(m_imageSize,
                            m_scene->m_animatorsTerrain[m_animatorID]->wholeImage(),
                            m_scene->m_animatorsTerrain[m_animatorID]->frameRect());
    else
        painter->drawRect(QRect(0,0,32,32));

    if(this->isSelected())
    {
        painter->setPen(QPen(QBrush(Qt::black), 2, Qt::SolidLine));
        painter->drawRect(1,1,m_imageSize.width()-2,m_imageSize.height()-2);
        painter->setPen(QPen(QBrush(Qt::green), 2, Qt::DotLine));
        painter->drawRect(1,1,m_imageSize.width()-2,m_imageSize.height()-2);
    }
}

////////////////Animation///////////////////

void ItemTile::setAnimator(long aniID)
{
    if(aniID<m_scene->m_animatorsTerrain.size())
    {
        QRect frameRect = m_scene->m_animatorsTerrain[aniID]->frameRect();
        m_imageSize = QRectF(0,0, frameRect.width(), frameRect.height() );
    }

    setData(ITEM_WIDTH, QString::number(qRound(m_imageSize.width())) ); //width
    setData(ITEM_HEIGHT, QString::number(qRound(m_imageSize.height())) ); //height
    //WriteToLog(QtDebugMsg, QString("Tile Animator ID: %1").arg(aniID));

    m_animatorID = aniID;
}
