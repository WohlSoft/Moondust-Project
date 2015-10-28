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
#include <common_features/mainwinconnect.h>
#include <common_features/logger.h>
#include <audio/music_player.h>

#include "item_music.h"

ItemMusic::ItemMusic(QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    construct();
}

ItemMusic::ItemMusic(WldScene *parentScene, QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    construct();
    if(!parentScene) return;
    setScenePoint(parentScene);
    scene->addItem(this);
    this->setZValue(scene->musicZ);
    gridSize = scene->pConfigs->default_grid;
    imageSize = QRectF(0, 0, gridSize, gridSize);
    setData(ITEM_WIDTH, QString::number( gridSize ) ); //width
    setData(ITEM_HEIGHT, QString::number( gridSize ) ); //height
}

void ItemMusic::construct()
{
    gridSize=32;
    gridOffsetX=0;
    gridOffsetY=0;
    isLocked=false;

    animatorID=-1;
    musicTitle = "";
    scene=NULL;
    imageSize = QRectF(0, 0, gridSize, gridSize);

    setData(ITEM_TYPE, "MUSICBOX");
    setData(ITEM_IS_ITEM, 1);

    setData(ITEM_WIDTH, QString::number( gridSize ) ); //width
    setData(ITEM_HEIGHT, QString::number( gridSize ) ); //height

    mouseLeft=false;
    mouseMid=false;
    mouseRight=false;
}

ItemMusic::~ItemMusic()
{
    //WriteToLog(QtDebugMsg, "!<-BGO destroyed->!");
    //if(timer) delete timer;
    scene->unregisterElement(this);
}



void ItemMusic::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent )
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

void ItemMusic::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
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
        if((!scene->lock_musbox)&&(!scene->DrawMode)&&(!isLocked))
        {
            contextMenu(mouseEvent);
        }

    }
}

void ItemMusic::contextMenu( QGraphicsSceneMouseEvent * mouseEvent )
{
    scene->contextMenuOpened = true;
    //Remove selection from non-bgo items
    if(!this->isSelected())
    {
        scene->clearSelection();
        this->setSelected(true);
    }

    this->setSelected(true);
    QMenu ItemMenu;

    if(!musicTitle.isEmpty())
    {
        QAction *title = ItemMenu.addAction(QString("[%1]").arg(musicTitle));
        title->setEnabled(false);
    }else if(musicData.id==0)
    {
        QAction *title = ItemMenu.addAction(QString("[%1]").arg(tr("<Silence>")));
        title->setEnabled(false);
    }
    QAction *play = ItemMenu.addAction(tr("Play this"));
        ItemMenu.addSeparator();
    QAction *copyTile = ItemMenu.addAction(tr("Copy"));
    QAction *cutTile = ItemMenu.addAction(tr("Cut"));
        ItemMenu.addSeparator();
    QAction *transform = ItemMenu.addAction(tr("Transform into"));
    QAction *transform_all = ItemMenu.addAction(tr("Transform all %1 into").arg("MUSIC-%1%2")
                                                .arg(musicData.id)
                                                .arg( musicData.music_file.isEmpty()?"":" ("+musicData.music_file+")" ) );
        ItemMenu.addSeparator();
    QAction *remove = ItemMenu.addAction(tr("Remove"));

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
        scene->_edit->currentMusic = musicData.id;
        LvlMusPlay::setMusic(LvlMusPlay::WorldMusic, musicData.id, musicData.music_file);
        LvlMusPlay::updatePlayerState(true);
        MainWinConnect::pMainWin->setMusicButton(true);
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

        ItemSelectDialog * itemList = new ItemSelectDialog(scene->pConfigs, ItemSelectDialog::TAB_MUSIC,0,0,0,0,0,0,0,0,0,MainWinConnect::pMainWin);
        itemList->removeEmptyEntry(ItemSelectDialog::TAB_MUSIC);
        util::DialogToCenter(itemList, true);

        if(itemList->exec()==QDialog::Accepted)
        {
            QList<QGraphicsItem *> our_items;
            bool sameID=false;
            transformTO   = itemList->musicID;
            transformTO_file = itemList->musicFile;
            unsigned long oldID = musicData.id;

            if(selected==transform)
                our_items=scene->selectedItems();
            else
            if(selected==transform_all)
            {
                our_items=scene->items();
                sameID=true;
            }

            foreach(QGraphicsItem * SelItem, our_items )
            {
                if(SelItem->data(ITEM_TYPE).toString()=="MUSICBOX")
                {
                    if((!sameID)||(((ItemMusic *) SelItem)->musicData.id==oldID))
                    {
                        oldData.music.push_back( ((ItemMusic *) SelItem)->musicData );
                        ((ItemMusic *) SelItem)->transformTo(transformTO, transformTO_file);
                        newData.music.push_back( ((ItemMusic *) SelItem)->musicData );
                    }
                }
            }
        }
        delete itemList;
        if(!newData.music.isEmpty())
            scene->addTransformHistory(newData, oldData);
    }
    else
    if(selected==remove)
    {
        scene->removeSelectedWldItems();
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
    musicData.x = qRound(this->scenePos().x());
    musicData.y = qRound(this->scenePos().y());

    if(musicData.index < (unsigned int)scene->WldData->music.size())
    { //Check index
        if(musicData.array_id == scene->WldData->music[musicData.index].array_id)
        {
            found=true;
        }
    }

    //Apply current data in main array
    if(found)
    { //directlry
        scene->WldData->music[musicData.index] = musicData; //apply current musicData
    }
    else
    for(int i=0; i<scene->WldData->music.size(); i++)
    { //after find it into array
        if(scene->WldData->music[i].array_id == musicData.array_id)
        {
            musicData.index = i;
            scene->WldData->music[i] = musicData;
            break;
        }
    }
    scene->unregisterElement(this);
    scene->registerElement(this);
}

void ItemMusic::removeFromArray()
{
    bool found=false;
    if(musicData.index < (unsigned int)scene->WldData->music.size())
    { //Check index
        if(musicData.array_id == scene->WldData->music[musicData.index].array_id)
        {
            found=true;
        }
    }

    if(found)
    { //directlry
        scene->WldData->music.removeAt(musicData.index);
    }
    else
    for(int i=0; i<scene->WldData->music.size(); i++)
    {
        if(scene->WldData->music[i].array_id == musicData.array_id)
        {
            scene->WldData->music.removeAt(i); break;
        }
    }
}

void ItemMusic::returnBack()
{
    setPos(musicData.x, musicData.y);
}

int ItemMusic::getGridSize()
{
    return gridSize;
}

QPoint ItemMusic::sourcePos()
{
    return QPoint(musicData.x, musicData.y);
}

void ItemMusic::setMusicData(WorldMusic inD)
{
    musicData = inD;
    setPos(musicData.x, musicData.y);
    setData(ITEM_ID, QString::number(musicData.id) );
    setData(ITEM_ARRAY_ID, QString::number(musicData.array_id) );

    scene->unregisterElement(this);
    scene->registerElement(this);
}


QRectF ItemMusic::boundingRect() const
{
    return imageSize;
}

void ItemMusic::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if(scene==NULL)
    {
        painter->setPen(QPen(QBrush(Qt::yellow), 2, Qt::SolidLine));
        painter->setBrush(Qt::yellow);
        painter->setOpacity(0.5);
        painter->drawRect(1,1,imageSize.width()-2,imageSize.height()-2);
        painter->setOpacity(1);
        painter->setBrush(Qt::transparent);
        painter->drawRect(1,1,imageSize.width()-2,imageSize.height()-2);
    }
    else
    {
       painter->drawPixmap(scene->musicBoxImg.rect(), scene->musicBoxImg, scene->musicBoxImg.rect());
    }

    if(this->isSelected())
    {
        painter->setPen(QPen(QBrush(Qt::black), 2, Qt::SolidLine));
        painter->drawRect(1,1,imageSize.width()-2,imageSize.height()-2);
        painter->setPen(QPen(QBrush(Qt::white), 2, Qt::DotLine));
        painter->drawRect(1,1,imageSize.width()-2,imageSize.height()-2);
    }
}

void ItemMusic::setScenePoint(WldScene *theScene)
{
    scene = theScene;
}

void ItemMusic::transformTo(int musicID, QString musicFile)
{
    int j = scene->pConfigs->getMusWldI(musicID);
    if(j>=0)
        {
            musicTitle =
                    (scene->pConfigs->music_w_custom_id==(unsigned)musicID) ?
                        musicFile:
                        scene->pConfigs->main_music_wld[j].name;

            musicData.id         = musicID;
            musicData.music_file = musicFile;
            arrayApply();
        }
}

