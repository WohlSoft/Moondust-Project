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
#include <audio/music_player.h>

#include "item_music.h"
#include "../wld_history_manager.h"

ItemMusic::ItemMusic(QGraphicsItem *parent)
    : WldBaseItem(parent)
{
    construct();
}

ItemMusic::ItemMusic(WldScene *parentScene, QGraphicsItem *parent)
    : WldBaseItem(parentScene, parent)
{
    construct();
    if(!parentScene) return;
    setScenePoint(parentScene);
    m_scene->addItem(this);
    this->setZValue(m_scene->Z_MusicBoxes);
    m_gridSize = m_scene->m_configs->defaultGrid.general;
    m_imageSize = QRectF(0, 0, m_gridSize, m_gridSize);
    setData(ITEM_WIDTH,  QString::number( m_gridSize ) ); //width
    setData(ITEM_HEIGHT, QString::number( m_gridSize ) ); //height
}

void ItemMusic::construct()
{
    m_musicTitle = "";
    m_imageSize = QRectF(0, 0, m_gridSize, m_gridSize);

    setData(ITEM_TYPE, "MUSICBOX");
    setData(ITEM_WIDTH, QString::number( m_gridSize ) ); //width
    setData(ITEM_HEIGHT, QString::number( m_gridSize ) ); //height
}

ItemMusic::~ItemMusic()
{
    m_scene->unregisterElement(this);
}

void ItemMusic::contextMenu( QGraphicsSceneMouseEvent * mouseEvent )
{
    m_scene->m_contextMenuIsOpened = true;
    //Remove selection from non-bgo items
    if(!this->isSelected())
    {
        m_scene->clearSelection();
        this->setSelected(true);
    }

    this->setSelected(true);
    QMenu ItemMenu;

    if(!m_musicTitle.isEmpty())
    {
        QAction *title = ItemMenu.addAction(QString("[%1]").arg(m_musicTitle));
        title->setEnabled(false);
    }else if(m_data.id==0)
    {
        QAction *title = ItemMenu.addAction(QString("[%1]").arg(tr("<Silence>")));
        title->setEnabled(false);
    }
    QAction *play = ItemMenu.addAction(tr("Play this"));
        ItemMenu.addSeparator();

    QMenu * copyPreferences = ItemMenu.addMenu(tr("Copy preferences"));
        copyPreferences->deleteLater();
            QAction *copyItemID = copyPreferences->addAction(tr("World-Music-ID: %1").arg(m_data.id));
                copyItemID->deleteLater();
            QAction *copyPosXY = copyPreferences->addAction(tr("Position: X, Y"));
                copyPosXY->deleteLater();
            QAction *copyPosXYWH = copyPreferences->addAction(tr("Position: X, Y, Width, Height"));
                copyPosXYWH->deleteLater();
            QAction *copyPosLTRB = copyPreferences->addAction(tr("Position: Left, Top, Right, Bottom"));
                copyPosLTRB->deleteLater();

    QAction *copyTile = ItemMenu.addAction(tr("Copy"));
    QAction *cutTile = ItemMenu.addAction(tr("Cut"));
        ItemMenu.addSeparator();
    QAction *transform = ItemMenu.addAction(tr("Transform into"));
    QAction *transform_all = ItemMenu.addAction(tr("Transform all %1 into").arg("MUSIC-%1%2")
                                                .arg(m_data.id)
                                                .arg( m_data.music_file.isEmpty()?"":" ("+m_data.music_file+")" ) );
        ItemMenu.addSeparator();
    QAction *remove =       ItemMenu.addAction(tr("Remove"));
    QAction *remove_all =   ItemMenu.addAction(tr("Remove all %1").arg("MUSIC-%1").arg(m_data.id));

QAction *selected = ItemMenu.exec(mouseEvent->screenPos());

    if(!selected)
    {
        #ifdef _DEBUG_
        WriteToLog(QtDebugMsg, "Context Menu <- NULL");
        #endif
        return;
    }

    if(selected==play)
    {
        m_scene->m_subWindow->currentMusic = m_data.id;
        LvlMusPlay::setMusic(LvlMusPlay::WorldMusic, m_data.id, m_data.music_file);
        LvlMusPlay::updatePlayerState(true);
        MainWinConnect::pMainWin->setMusicButton(true);
    }
    else
    if(selected==copyItemID)
    {
        QApplication::clipboard()->setText(QString("%1").arg(m_data.id));
        MainWinConnect::pMainWin->showStatusMsg(tr("Preferences has been copied: %1").arg(QApplication::clipboard()->text()));
    }
    else
    if(selected==copyPosXY)
    {
        QApplication::clipboard()->setText(
                            QString("X=%1; Y=%2;")
                               .arg(m_data.x)
                               .arg(m_data.y)
                               );
        MainWinConnect::pMainWin->showStatusMsg(tr("Preferences has been copied: %1").arg(QApplication::clipboard()->text()));
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
        MainWinConnect::pMainWin->showStatusMsg(tr("Preferences has been copied: %1").arg(QApplication::clipboard()->text()));
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
        MainWinConnect::pMainWin->showStatusMsg(tr("Preferences has been copied: %1").arg(QApplication::clipboard()->text()));
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
        QString transformTO_file;

        ItemSelectDialog * itemList = new ItemSelectDialog(m_scene->m_configs, ItemSelectDialog::TAB_MUSIC,0,0,0,0,0,0,0,0,0, m_scene->m_subWindow);
        itemList->removeEmptyEntry(ItemSelectDialog::TAB_MUSIC);
        util::DialogToCenter(itemList, true);

        if(itemList->exec()==QDialog::Accepted)
        {
            QList<QGraphicsItem *> our_items;
            bool sameID=false;
            transformTO   = itemList->musicID;
            transformTO_file = itemList->musicFile;
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
                if(SelItem->data(ITEM_TYPE).toString()=="MUSICBOX")
                {
                    if((!sameID)||(((ItemMusic *) SelItem)->m_data.id==oldID))
                    {
                        oldData.music.push_back( ((ItemMusic *) SelItem)->m_data );
                        ((ItemMusic *) SelItem)->transformTo(transformTO, transformTO_file);
                        newData.music.push_back( ((ItemMusic *) SelItem)->m_data );
                    }
                }
            }
        }
        delete itemList;
        if(!newData.music.isEmpty())
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
            if(SelItem->data(ITEM_TYPE).toString()=="MUSICBOX")
            {
                if( ((ItemMusic*) SelItem)->m_data.id == oldID)
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
//void ItemMusic::setLayer(QString layer)
//{
//    foreach(LevelLayers lr, scene->WldData->layers)
//    {
//        if(lr.name==layer)
//        {
//            musicData.layer = layer;
//            this->setVisible(!lr.hidden);
//            arrayApply();
//        break;
//        }
//    }
//}

void ItemMusic::arrayApply()
{
    bool found=false;
    m_data.x = qRound(this->scenePos().x());
    m_data.y = qRound(this->scenePos().y());

    if(m_data.meta.index < (unsigned int)m_scene->m_data->music.size())
    { //Check index
        if(m_data.meta.array_id == m_scene->m_data->music[m_data.meta.index].meta.array_id)
        {
            found=true;
        }
    }

    //Apply current data in main array
    if(found)
    { //directlry
        m_scene->m_data->music[m_data.meta.index] = m_data; //apply current musicData
    }
    else
    for(int i=0; i<m_scene->m_data->music.size(); i++)
    { //after find it into array
        if(m_scene->m_data->music[i].meta.array_id == m_data.meta.array_id)
        {
            m_data.meta.index = i;
            m_scene->m_data->music[i] = m_data;
            break;
        }
    }

    //Mark world map as modified
    m_scene->m_data->meta.modified = true;

    m_scene->unregisterElement(this);
    m_scene->registerElement(this);
}

void ItemMusic::removeFromArray()
{
    bool found=false;
    if(m_data.meta.index < (unsigned int)m_scene->m_data->music.size())
    { //Check index
        if(m_data.meta.array_id == m_scene->m_data->music[m_data.meta.index].meta.array_id)
        {
            found=true;
        }
    }

    if(found)
    { //directlry
        m_scene->m_data->music.removeAt(m_data.meta.index);
    }
    else
    for(int i=0; i<m_scene->m_data->music.size(); i++)
    {
        if(m_scene->m_data->music[i].meta.array_id == m_data.meta.array_id)
        {
            m_scene->m_data->music.removeAt(i); break;
        }
    }

    //Mark world map as modified
    m_scene->m_data->meta.modified = true;
}

void ItemMusic::returnBack()
{
    setPos(m_data.x, m_data.y);
}

QPoint ItemMusic::sourcePos()
{
    return QPoint(m_data.x, m_data.y);
}

bool ItemMusic::itemTypeIsLocked()
{
    return m_scene->m_lockMusicBox;
}

void ItemMusic::setMusicData(WorldMusicBox inD)
{
    m_data = inD;
    setPos(m_data.x, m_data.y);
    setData(ITEM_ID, QString::number(m_data.id) );
    setData(ITEM_ARRAY_ID, QString::number(m_data.meta.array_id) );

    m_scene->unregisterElement(this);
    m_scene->registerElement(this);
}


QRectF ItemMusic::boundingRect() const
{
    return m_imageSize;
}

void ItemMusic::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if(m_scene==NULL)
    {
        painter->setPen(QPen(QBrush(Qt::yellow), 2, Qt::SolidLine));
        painter->setBrush(Qt::yellow);
        painter->setOpacity(0.5);
        painter->drawRect(1,1,m_imageSize.width()-2,m_imageSize.height()-2);
        painter->setOpacity(1);
        painter->setBrush(Qt::transparent);
        painter->drawRect(1,1,m_imageSize.width()-2,m_imageSize.height()-2);
    }
    else
    {
       painter->drawPixmap(m_scene->m_musicBoxImg.rect(),
                           m_scene->m_musicBoxImg,
                           m_scene->m_musicBoxImg.rect());
    }

    if(this->isSelected())
    {
        painter->setPen(QPen(QBrush(Qt::black), 2, Qt::SolidLine));
        painter->drawRect(1,1,m_imageSize.width()-2,m_imageSize.height()-2);
        painter->setPen(QPen(QBrush(Qt::white), 2, Qt::DotLine));
        painter->drawRect(1,1,m_imageSize.width()-2,m_imageSize.height()-2);
    }
}

void ItemMusic::transformTo(int musicID, QString musicFile)
{
    int j = m_scene->m_configs->getMusWldI(musicID);
    if(j>=0)
    {
        m_musicTitle =
                (m_scene->m_configs->music_w_custom_id==(unsigned)musicID) ?
                    musicFile:
                    m_scene->m_configs->main_music_wld[j].name;

        m_data.id         = musicID;
        m_data.music_file = musicFile;
        arrayApply();
    }
}

