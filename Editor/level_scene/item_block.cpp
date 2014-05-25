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

#include "item_block.h"
#include "../common_features/logger.h"

#include "../npc_dialog/npcdialog.h"
#include "newlayerbox.h"

#include "../common_features/mainwinconnect.h"

ItemBlock::ItemBlock(QGraphicsPixmapItem *parent)
    : QGraphicsPixmapItem(parent)
{
    animated = false;
    frameFirst=0; //from first frame
    frameLast=-1; //to unlimited frameset
    //image = new QGraphicsPixmapItem;
    isLocked=false;
}


ItemBlock::~ItemBlock()
{
 //   WriteToLog(QtDebugMsg, "!<-Block destroyed->!");
}

void ItemBlock::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent )
{
    if(scene->DrawMode)
    {
        unsetCursor();
        ungrabMouse();
        this->setSelected(false);
        return;
    }
    QGraphicsPixmapItem::mousePressEvent(mouseEvent);
}

void ItemBlock::contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
{
    if((!scene->lock_block)&&(!isLocked))
    {
        //Remove selection from non-block items
        if(this->isSelected())
        {
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(0).toString()!="Block") SelItem->setSelected(false);
            }
        }
        else
        {
            scene->clearSelection();
            this->setSelected(true);
        }


        this->setSelected(1);
        ItemMenu->clear();
        QMenu * LayerName = ItemMenu->addMenu(tr("Layer: ")+QString("[%1]").arg(blockData.layer));

        QAction *setLayer;
        QList<QAction *> layerItems;

        QAction * newLayer = LayerName->addAction(tr("Add to new layer..."));
        LayerName->addSeparator();

        foreach(LevelLayers layer, scene->LvlData->layers)
        {
            //Skip system layers
            if((layer.name=="Destroyed Blocks")||(layer.name=="Spawned NPCs")) continue;

            setLayer = LayerName->addAction( layer.name+((layer.hidden)?tr(" [hidden]"):"") );
            setLayer->setData(layer.name);
            setLayer->setCheckable(true);
            setLayer->setEnabled(true);
            setLayer->setChecked( layer.name==blockData.layer );
            layerItems.push_back(setLayer);
        }

        ItemMenu->addSeparator();

        QAction *invis = ItemMenu->addAction(tr("Invisible"));
            invis->setCheckable(1);
            invis->setChecked( blockData.invisible );

        QAction *slipp = ItemMenu->addAction(tr("Slippery"));
            slipp->setCheckable(1);
            slipp->setChecked( blockData.slippery );

        QAction *resize = ItemMenu->addAction(tr("Resize"));
            resize->setVisible( (this->data(3).toString()=="sizable") );

        ItemMenu->addSeparator();
        QAction *chNPC = ItemMenu->addAction(tr("Change included NPC..."));

        ItemMenu->addSeparator();
        QAction *copyBlock = ItemMenu->addAction( tr("Copy") );
        QAction *cutBlock = ItemMenu->addAction( tr("Cut") );
        ItemMenu->addSeparator();
        QAction *remove = ItemMenu->addAction( tr("Remove") );

        scene->contextMenuOpened = true; //bug protector
QAction *selected = ItemMenu->exec(event->screenPos());

        if(!selected)
        {
            WriteToLog(QtDebugMsg, "Context Menu <- NULL");
            scene->contextMenuOpened = true;
            return;
        }
        event->accept();

        //WriteToLog(QtDebugMsg, QString("Block ContextMenu");

        if(selected==cutBlock)
        {
            //scene->doCut = true ;
            MainWinConnect::pMainWin->on_actionCut_triggered();
            scene->contextMenuOpened = false;
        }
        else
        if(selected==copyBlock)
        {
            //scene->doCopy = true ;
            MainWinConnect::pMainWin->on_actionCopy_triggered();
            scene->contextMenuOpened = false;
        }
        else
        if(selected==invis)
        {
            //apply to all selected items.
            LevelData selData;
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(0).toString()=="Block")
                    ((ItemBlock *) SelItem)->setInvisible(invis->isChecked());
                selData.blocks.push_back(((ItemBlock *) SelItem)->blockData);
            }
            scene->addChangeSettingsHistory(selData, LvlScene::SETTING_INVISIBLE, QVariant(invis->isChecked()));
            scene->contextMenuOpened = false;
        }
        else
        if(selected==slipp)
        {
            //apply to all selected items.
            LevelData selData;
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(0).toString()=="Block")
                    ((ItemBlock *) SelItem)->setSlippery(slipp->isChecked());
                selData.blocks.push_back(((ItemBlock *) SelItem)->blockData);
            }
            scene->addChangeSettingsHistory(selData, LvlScene::SETTING_SLIPPERY, QVariant(invis->isChecked()));
            scene->contextMenuOpened = false;
        }
        else
        if(selected==chNPC)
        {
            scene->contextMenuOpened = false;
            LevelData selData;
            QList<QVariant> modNPC;
            modNPC.push_back(QVariant((int)blockData.npc_id));
            NpcDialog * npcList = new NpcDialog(scene->pConfigs);
            npcList->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
            npcList->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, npcList->size(), qApp->desktop()->availableGeometry()));
            npcList->setState(blockData.npc_id);
            if(npcList->exec()==QDialog::Accepted)
            {
                //apply to all selected items.
                int selected_npc=0;
                if(npcList->isEmpty)
                    selected_npc = 0;
                else
                if(npcList->isCoin)
                    selected_npc = npcList->coins;
                else
                    selected_npc = npcList->selectedNPC+1000;

                foreach(QGraphicsItem * SelItem, scene->selectedItems() )
                {
                    if(SelItem->data(0).toString()=="Block")
                    {
                        ((ItemBlock *) SelItem)->blockData.npc_id = selected_npc;
                        ((ItemBlock *) SelItem)->arrayApply();
                        ((ItemBlock *) SelItem)->setIncludedNPC(selected_npc);
                        selData.blocks.push_back(((ItemBlock *) SelItem)->blockData);
                    }
                }
                modNPC.push_back(QVariant(selected_npc));
            }
            scene->addChangeSettingsHistory(selData, LvlScene::SETTING_CHANGENPC, QVariant(modNPC));
        }
        else
        if(selected==remove)
        {
            LevelData removedItems;
            bool deleted=false;
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(0).toString()=="Block")
                {
                    removedItems.blocks.push_back(((ItemBlock *)SelItem)->blockData);
                    ((ItemBlock *)SelItem)->removeFromArray();
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
                LevelData modData;
                foreach(LevelLayers lr, scene->LvlData->layers)
                { //Find layer's settings
                    if(lr.name==lName)
                    {
                        foreach(QGraphicsItem * SelItem, scene->selectedItems() )
                        {

                            if(SelItem->data(0).toString()=="Block")
                            {
                                modData.blocks.push_back(((ItemBlock*) SelItem)->blockData);
                                ((ItemBlock *) SelItem)->blockData.layer = lr.name;
                                ((ItemBlock *) SelItem)->setVisible(!lr.hidden);
                                ((ItemBlock *) SelItem)->arrayApply();
                            }
                        }
                    break;
                    }
                }//Find layer's settings
                scene->addChangedLayerHistory(modData, lName);
             scene->contextMenuOpened = false;
            }

        }
    }
    else
    {
        QGraphicsPixmapItem::contextMenuEvent(event);
    }
}

void ItemBlock::setSlippery(bool slip)
{
    blockData.slippery=slip;
    arrayApply(); //Apply changes into array
}

void ItemBlock::setInvisible(bool inv)
{
    blockData.invisible=inv;
    if(inv)
        this->setOpacity(0.5);
    else
        this->setOpacity(1);

    arrayApply();//Apply changes into array

}

void ItemBlock::setLayer(QString layer)
{
    foreach(LevelLayers lr, scene->LvlData->layers)
    {
        if(lr.name==layer)
        {
            blockData.layer = layer;
            this->setVisible(!lr.hidden);
            arrayApply();
        break;
        }
    }

}

void ItemBlock::setIncludedNPC(int npcID)
{
    if(includedNPC!=NULL)
    {
        grp->removeFromGroup(includedNPC);
        scene->removeItem(includedNPC);
        free(includedNPC);
        includedNPC = NULL;
    }
    if(npcID==0) return;

    QPixmap npcImg = QPixmap( scene->getNPCimg( ((npcID > 1000)? (npcID-1000) : scene->pConfigs->marker_npc.coin_in_block ) ) );
    includedNPC = scene->addPixmap( npcImg );

    includedNPC->setPos(
                (
                    blockData.x+((blockData.w-npcImg.width())/2)
                 ),
                (
                    blockData.y+((blockData.h-npcImg.height())/2)
                 ));
    includedNPC->setZValue(scene->blockZ + 10);
    includedNPC->setOpacity(qreal(0.6));
    grp->addToGroup(includedNPC);
}

///////////////////MainArray functions/////////////////////////////
void ItemBlock::arrayApply()
{
    bool found=false;
    if(this->data(3).toString()=="sizable")
        this->setZValue( scene->blockZs + ((double)blockData.y / (double) 100000000000) + 1 - ((double)blockData.w * (double)0.0000000000000001) );
    if(blockData.index < (unsigned int)scene->LvlData->blocks.size())
    { //Check index
        if(blockData.array_id == scene->LvlData->blocks[blockData.index].array_id)
            found=true;
    }

    //Apply current data in main array
    if(found)
    { //directlry
        scene->LvlData->blocks[blockData.index] = blockData; //apply current blockdata
    }
    else
    for(int i=0; i<scene->LvlData->blocks.size(); i++)
    { //after find it into array
        if(scene->LvlData->blocks[i].array_id == blockData.array_id)
        {
            blockData.index = i;
            scene->LvlData->blocks[i] = blockData;
            break;
        }
    }
}

void ItemBlock::removeFromArray()
{
    bool found=false;
    if(blockData.index < (unsigned int)scene->LvlData->blocks.size())
    { //Check index
        if(blockData.array_id == scene->LvlData->blocks[blockData.index].array_id)
            found=true;
    }
    if(found)
    { //directlry
        scene->LvlData->blocks.remove(blockData.index);
    }
    else
    for(int i=0; i<scene->LvlData->blocks.size(); i++)
    {
        if(scene->LvlData->blocks[i].array_id == blockData.array_id)
        {
            scene->LvlData->blocks.remove(i); break;
        }
    }
}

/*
void ItemBlock::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    int gridSize=32, offsetX=0, offsetY=0, gridX, gridY, i;
    QPoint sourcePos;

    sourcePos=QPoint(blockData.x, blockData.y);
    QPointF itemPos = this->scenePos();

    if((!isSelected())||(sourcePos == itemPos))
    { QGraphicsPixmapItem::mouseReleaseEvent(event); return;}

    if(scene->grid)
    { //ATTACH TO GRID
        gridX = ((int)itemPos.x() - (int)itemPos.x() % gridSize);
        gridY = ((int)itemPos.y() - (int)itemPos.y() % gridSize);

        if((int)itemPos.x()<0)
        {
            if( (int)itemPos.x() < gridX - (int)(gridSize/2) )
                gridX -= gridSize;
        }
        else
        {
            if( (int)itemPos.x() > gridX + (int)(gridSize/2) )
                gridX += gridSize;
        }

        if((int)itemPos.y()<0)
        {if( (int)itemPos.y() < gridY - (int)(gridSize/2) )
            gridY -= gridSize;
        }
        else {if( (int)itemPos.y() > gridY + (int)(gridSize/2) )
         gridY += gridSize;
        }

        this->setPos(QPointF(offsetX+gridX, offsetY+gridY));
    }

    //Check collision
    if( scene->itemCollidesWith(this) )
    {
        this->setPos(QPointF(sourcePos));
        this->setSelected(false);
        WriteToLog(QtDebugMsg, QString("Moved back %1 %2")
                   .arg((long)this->scenePos().x())
                   .arg((long)this->scenePos().y()) );
    }
    else
    {
        blockData.x=(long)this->scenePos().x();
        blockData.y=(long)this->scenePos().y();

         for (i=0;i<scene->LvlData->blocks.size();i++)
            {
                if(scene->LvlData->blocks[i].array_id == blockData.array_id)
                {
                    //Applay move into main array
                    scene->LvlData->blocks[i].x = (long)this->scenePos().x();
                    scene->LvlData->blocks[i].y = (long)this->scenePos().y();
                    scene->LvlData->modyfied = true;
                    break;
                }
            }
    }

}*/

void ItemBlock::setMainPixmap(const QPixmap &pixmap)
{
    mainImage = pixmap;
    if(!sizable)
        this->setPixmap(mainImage);
    else
    {
        frameWidth = blockData.w;
        frameSize = blockData.h;
        frameHeight = blockData.h;
        currentImage = drawSizableBlock(blockData.w, blockData.h, mainImage);
        this->setPixmap(currentImage);
    }
}

void ItemBlock::setBlockData(LevelBlock inD, bool is_sz)
{
    blockData = inD;
    sizable = is_sz;
}


QRectF ItemBlock::boundingRect() const
{
    if((!animated)&&(!sizable))
        return QRectF(0,0,mainImage.width(),mainImage.height());
    else
        return QRectF(0,0,frameWidth,frameSize);
}

void ItemBlock::setContextMenu(QMenu &menu)
{
    ItemMenu = &menu;
}


//global Pointers
void ItemBlock::setScenePoint(LvlScene *theScene)
{
    scene = theScene;
}

void ItemBlock::setGroupPoint(QGraphicsItemGroup *theGrp)
{
    grp = theGrp;
}

void ItemBlock::setNPCItemPoint(QGraphicsItem *includedNPCPnt)
{
    includedNPC = includedNPCPnt;
}


////////////////Animation///////////////////


void ItemBlock::setAnimation(int frames, int framespeed, int algorithm)
{
    animated = true;
    framesQ = frames;
    frameSpeed = framespeed;

    frameSize = (int)round(mainImage.height()/frames);
    frameWidth = mainImage.width();
    frameHeight = mainImage.height();

    framePos = QPoint(0,0);
    draw();

    if(algorithm == 1) // Invisible block
    {
        frameFirst = 5;
        frameLast = 6;
    }
    else if(algorithm == 3) //Player's character block
    {
        frameFirst = 0;
        frameLast = 1;
    }
    else if(algorithm == 4) //Player's character switch
    {
        frameFirst = 0;
        frameLast = 4;
    }
    else //Default block
    {
        frameFirst = 0;
        frameLast = -1;
    }

    setFrame(frameFirst);

    timer = new QTimer(this);
    connect(
                timer, SIGNAL(timeout()),
                this,
                SLOT( nextFrame() ) );
}

void ItemBlock::AnimationStart()
{
    if(!animated) return;
    timer->start(frameSpeed);
}

void ItemBlock::AnimationStop()
{
    if(!animated) return;
    timer->stop();
    setFrame(frameFirst);
}

void ItemBlock::draw()
{
    currentImage =  mainImage.copy(QRect(framePos.x(), framePos.y(), frameWidth, frameSize ));
}

QPoint ItemBlock::fPos() const
{
    return framePos;
}

void ItemBlock::setFrame(int y)
{
    frameCurrent = frameSize * y;
    if ( ((frameCurrent >= frameHeight )&&(frameLast==-1)) ||
         ((frameCurrent >= frameLast*frameSize )&&(frameLast>-1)) )
        {
        frameCurrent = frameFirst*frameSize;
        framePos.setY( frameFirst * frameSize );
        }
    else
    framePos.setY( frameCurrent );
    draw();
    this->setPixmap(QPixmap(currentImage));
}

void ItemBlock::setLocked(bool lock)
{
    this->setFlag(QGraphicsItem::ItemIsSelectable, !lock);
    this->setFlag(QGraphicsItem::ItemIsMovable, !lock);
    isLocked = lock;
}

void ItemBlock::nextFrame()
{
    frameCurrent += frameSize;
    if ( ((frameCurrent >= frameHeight )&&(frameLast==-1)) ||
         ((frameCurrent >= frameLast*frameSize )&&(frameLast>-1)) )
        {
        frameCurrent = frameFirst*frameSize;
        framePos.setY( frameFirst * frameSize );
        }
    else
    framePos.setY( framePos.y() + frameSize );
    draw();
    this->setPixmap(QPixmap(currentImage));
}



//sizable Block formula
QPixmap ItemBlock::drawSizableBlock(int w, int h, QPixmap srcimg)
{
    int x,y, i, j;
    int hc, wc;
    QPixmap img;
    QPixmap * sizableImage;
    QPainter * szblock;
    x=32;
    y=32;

    sizableImage = new QPixmap(QSize(w, h));
    sizableImage->fill(Qt::transparent);
    szblock = new QPainter(sizableImage);

    //L
    hc=0;
    for(i=0; i<((h-2*y) / y); i++ )
    {
        szblock->drawPixmap(0, x+hc, x, y, srcimg.copy(QRect(0, y, x, y)));
            hc+=x;
    }

    //T
    hc=0;
    for(i=0; i<( (w-2*x) / x); i++ )
    {
        szblock->drawPixmap(x+hc, 0, x, y, srcimg.copy(QRect(x, 0, x, y)) );
            hc+=x;
    }

    //B
    hc=0;
    for(i=0; i< ( (w-2*x) / x); i++ )
    {
        szblock->drawPixmap(x+hc, h-y, x, y, srcimg.copy(QRect(x, srcimg.width()-y, x, y )) );
            hc+=x;
    }

    //R
    hc=0;
    for(i=0; i<((h-2*y) / y); i++ )
    {
        szblock->drawPixmap(w-x, y+hc, x, y, srcimg.copy(QRect(srcimg.width()-x, y, x, y)));
            hc+=x;
    }

    //C
    hc=0;
    wc=0;
    for(i=0; i<((h-2*y) / y); i++ )
    {
        hc=0;
        for(j=0; j<((w-2*x) / x); j++ )
        {
        szblock->drawPixmap(x+hc, y+wc, x, y, srcimg.copy(QRect(x, y, x, y)));
            hc+=x;
        }
        wc+=y;
    }

    //Applay sizable formula
     //1
    szblock->drawPixmap(0,0,y,x, srcimg.copy(QRect(0,0,y,x)));
     //2
    szblock->drawPixmap(w-y, 0, y, x, srcimg.copy(QRect(srcimg.width()-y, 0, y, x)) );
     //3
    szblock->drawPixmap(w-y, h-x, y, x, srcimg.copy(QRect(srcimg.width()-y, srcimg.height()-x, y, x)) );
     //4
    szblock->drawPixmap(0, h-x, y, x, srcimg.copy(QRect(0, srcimg.height()-x, y, x)) );

    img = QPixmap( * sizableImage);
    return img;
}
