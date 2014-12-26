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

#include <editing/_dialogs/itemselectdialog.h>
#include <common_features/mainwinconnect.h>
#include <common_features/logger.h>

#include "item_block.h"
#include "item_bgo.h"
#include "item_npc.h"
#include "item_water.h"
#include "item_door.h"
#include "../newlayerbox.h"

ItemBlock::ItemBlock(QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    construct();
}

ItemBlock::ItemBlock(LvlScene *parentScene, QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    construct();
    if(!parentScene) return;
    setScenePoint(parentScene);
    parentScene->addItem(this);
    setLocked(scene->lock_block);
}

void ItemBlock::construct()
{
    animated = false;
    animatorID=-1;
    imageSize = QRectF(0,0,10,10);

    gridSize=32;

    isLocked=false;

    mouseLeft=false;
    mouseMid=false;
    mouseRight=false;

    setData(ITEM_TYPE, "Block");
    setData(ITEM_IS_ITEM, 1);
}


ItemBlock::~ItemBlock()
{
   // WriteToLog(QtDebugMsg, "!<-Block destroyed->!");

    if(includedNPC!=NULL) delete includedNPC;
    if(grp!=NULL) delete grp;
    //if(timer) delete timer;
}


void ItemBlock::mousePressEvent ( QGraphicsSceneMouseEvent * mouseEvent )
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
        mouseEvent->accept();
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

void ItemBlock::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
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
        if((!scene->lock_block)&&(!scene->DrawMode)&&(!isLocked))
        {
            scene->contextMenuOpened=true;

            //Remove selection from non-block items
            if(!this->isSelected())
            {
                scene->clearSelection();
                this->setSelected(true);
            }

            this->setSelected(true);
            QMenu ItemMenu;
            QMenu * LayerName = ItemMenu.addMenu(tr("Layer: ")+QString("[%1]").arg(blockData.layer).replace("&", "&&&"));

            QAction *setLayer;
            QList<QAction *> layerItems;

            QAction * newLayer = LayerName->addAction(tr("Add to new layer..."));
            LayerName->addSeparator()->deleteLater();;

            foreach(LevelLayers layer, scene->LvlData->layers)
            {
                //Skip system layers
                if((layer.name=="Destroyed Blocks")||(layer.name=="Spawned NPCs")) continue;

                setLayer = LayerName->addAction( layer.name.replace("&", "&&&")+((layer.hidden)?tr(" [hidden]"):"") );
                setLayer->setData(layer.name);
                setLayer->setCheckable(true);
                setLayer->setEnabled(true);
                setLayer->setChecked( layer.name==blockData.layer );
                layerItems.push_back(setLayer);
            }

                ItemMenu.addSeparator();

            QAction *invis = ItemMenu.addAction(tr("Invisible"));
                invis->setCheckable(1);
                invis->setChecked( blockData.invisible );

            QAction *slipp = ItemMenu.addAction(tr("Slippery"));
                slipp->setCheckable(1);
                slipp->setChecked( blockData.slippery );

            QAction *resize = ItemMenu.addAction(tr("Resize"));
                resize->setVisible( (this->data(ITEM_BLOCK_IS_SIZABLE).toString()=="sizable") );

                ItemMenu.addSeparator();
            QAction *chNPC = ItemMenu.addAction(tr("Change included NPC..."));
                ItemMenu.addSeparator();
            QAction *transform = ItemMenu.addAction(tr("Transform into"));
                ItemMenu.addSeparator();
            QAction *copyBlock = ItemMenu.addAction( tr("Copy") );
            QAction *cutBlock = ItemMenu.addAction( tr("Cut") );
                ItemMenu.addSeparator();
            QAction *remove = ItemMenu.addAction( tr("Remove") );
                ItemMenu.addSeparator();
            QAction *props = ItemMenu.addAction(tr("Properties..."));

     QAction *selected = ItemMenu.exec(mouseEvent->screenPos());

            if(!selected)
            {
                #ifdef _DEBUG_
                    WriteToLog(QtDebugMsg, "Context Menu <- NULL");
                #endif
                return;
            }
            //mouseEvent->accept();

            //WriteToLog(QtDebugMsg, QString("Block ContextMenu");

            if(selected==cutBlock)
            {
                MainWinConnect::pMainWin->on_actionCut_triggered();
            }
            else
            if(selected==copyBlock)
            {
                MainWinConnect::pMainWin->on_actionCopy_triggered();
            }
            else
            if(selected==transform)
            {
                int transformTO;
                ItemSelectDialog * blockList = new ItemSelectDialog(scene->pConfigs, ItemSelectDialog::TAB_BLOCK);
                blockList->removeEmptyEntry(ItemSelectDialog::TAB_BLOCK);
                blockList->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
                blockList->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, blockList->size(), qApp->desktop()->availableGeometry()));
                if(blockList->exec()==QDialog::Accepted)
                {
                    transformTO = blockList->blockID;
                    foreach(QGraphicsItem * SelItem, scene->selectedItems() )
                    {
                        if(SelItem->data(ITEM_TYPE).toString()=="Block")
                        {
                            ((ItemBlock *) SelItem)->transformTo(transformTO);
                        }
                    }
                }
                delete blockList;
            }
            else
            if(selected==invis)
            {
                //apply to all selected items.
                LevelData selData;
                foreach(QGraphicsItem * SelItem, scene->selectedItems() )
                {
                    if(SelItem->data(ITEM_TYPE).toString()=="Block")
                    {
                        selData.blocks.push_back(((ItemBlock *) SelItem)->blockData);
                        ((ItemBlock *) SelItem)->setInvisible(invis->isChecked());
                    }
                }
                scene->addChangeSettingsHistory(selData, LvlScene::SETTING_INVISIBLE, QVariant(invis->isChecked()));
            }
            else
            if(selected==slipp)
            {
                //apply to all selected items.
                LevelData selData;
                foreach(QGraphicsItem * SelItem, scene->selectedItems() )
                {
                    if(SelItem->data(ITEM_TYPE).toString()=="Block")
                    {
                        selData.blocks.push_back(((ItemBlock *) SelItem)->blockData);
                        ((ItemBlock *) SelItem)->setSlippery(slipp->isChecked());
                    }
                }
                scene->addChangeSettingsHistory(selData, LvlScene::SETTING_SLIPPERY, QVariant(invis->isChecked()));
            }
            else
            if(selected==resize)
            {
                scene->setBlockResizer(this, true);
            }
            else
            if(selected==chNPC)
            {
                LevelData selData;
                ItemSelectDialog * npcList = new ItemSelectDialog(scene->pConfigs, ItemSelectDialog::TAB_NPC,
                                                           ItemSelectDialog::NPCEXTRA_WITHCOINS | (blockData.npc_id < 0 && blockData.npc_id != 0 ? ItemSelectDialog::NPCEXTRA_ISCOINSELECTED : 0),0,0,
                                                           (blockData.npc_id <0 && blockData.npc_id != 0 ? blockData.npc_id *-1 : blockData.npc_id)
                                                                  );
                npcList->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
                npcList->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, npcList->size(), qApp->desktop()->availableGeometry()));
                if(npcList->exec()==QDialog::Accepted)
                {
                    //apply to all selected items.
                    int selected_npc=0;
                    if(npcList->npcID!=0){
                        if(npcList->isCoin)
                            selected_npc = npcList->npcID * -1;
                        else
                            selected_npc = npcList->npcID;
                    }

                    foreach(QGraphicsItem * SelItem, scene->selectedItems() )
                    {
                        if(SelItem->data(ITEM_TYPE).toString()=="Block")
                        {
                            //((ItemBlock *) SelItem)->blockData.npc_id = selected_npc;
                            //((ItemBlock *) SelItem)->arrayApply();
                            selData.blocks.push_back(((ItemBlock *) SelItem)->blockData);
                            ((ItemBlock *) SelItem)->setIncludedNPC(selected_npc);
                        }
                    }
                    scene->addChangeSettingsHistory(selData, LvlScene::SETTING_CHANGENPC, QVariant(selected_npc));
                }
                delete npcList;
            }
            else
            if(selected==remove)
            {
                //scene->contextMenuOpened = false;
                scene->removeSelectedLvlItems();
            }
            else
            if(selected==props)
            {
                scene->openProps();
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
    }
}

void ItemBlock::contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
{
    QGraphicsItem::contextMenuEvent(event);
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

void ItemBlock::setIncludedNPC(int npcID, bool init)
{
    if(includedNPC!=NULL)
    {
        grp->removeFromGroup(includedNPC);
        scene->removeItem(includedNPC);
        delete includedNPC;
        includedNPC = NULL;
    }
    if(npcID==0)
    {
        if(!init) blockData.npc_id = 0;
        if(!init) arrayApply();
        return;
    }

    QPixmap npcImg = QPixmap( scene->getNPCimg( ((npcID > 0)? (npcID) : scene->pConfigs->marker_npc.coin_in_block ) ) );
    includedNPC = scene->addPixmap( npcImg );

    includedNPC->setPos(
                (
                    blockData.x+((blockData.w-npcImg.width())/2)
                 ),
                (
                    blockData.y+((blockData.h-npcImg.height())/2)
                 ));
    includedNPC->setZValue(scene->Z_Block + 10);
    includedNPC->setOpacity(qreal(0.6));
    grp->addToGroup(includedNPC);

    if(!init) blockData.npc_id = npcID;
    if(!init) arrayApply();
}

void ItemBlock::transformTo(long target_id)
{
    if(target_id<1) return;

    bool noimage=true;
    long item_i=0;
    long animator=0;
    obj_block mergedSet;

    //Get block settings
    scene->getConfig_Block(target_id, item_i, animator, mergedSet, &noimage);

    if(noimage)
        return;//Don't transform, target item is not found

    blockData.id = target_id;
    setBlockData(blockData, &mergedSet, &animator);
    arrayApply();

    if(!scene->opts.animationEnabled)
        scene->update();
}

///////////////////MainArray functions/////////////////////////////
void ItemBlock::arrayApply()
{
    bool found=false;
    blockData.x = qRound(this->scenePos().x());
    blockData.y = qRound(this->scenePos().y());
    if(this->data(ITEM_BLOCK_IS_SIZABLE).toString()=="sizable")
        this->setZValue( scene->Z_blockSizable + ((double)blockData.y / (double) 100000000000) + 1 - ((double)blockData.w * (double)0.0000000000000001) );
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

void ItemBlock::returnBack()
{
    this->setPos(blockData.x, blockData.y);
}

QPoint ItemBlock::gridOffset()
{
    return QPoint(0,0);
}

int ItemBlock::getGridSize()
{
    return gridSize;
}

QPoint ItemBlock::sourcePos()
{
    return QPoint(blockData.x, blockData.y);
}


void ItemBlock::setMainPixmap() // Init Sizable block
{
    if(sizable)
    {
        currentImage = drawSizableBlock(blockData.w, blockData.h, scene->animates_Blocks[animatorID]->wholeImage());
        imageSize = QRectF(0,0, blockData.w, blockData.h);
    }
}

void ItemBlock::setBlockSize(QRect rect)
{
    if(sizable)
    {
        blockData.x = rect.x();
        blockData.y = rect.y();
        blockData.w = rect.width();
        blockData.h = rect.height();
        currentImage = drawSizableBlock(blockData.w, blockData.h, scene->animates_Blocks[animatorID]->wholeImage());
        this->setPos(blockData.x, blockData.y);
    }
    imageSize = QRectF(0,0, blockData.w, blockData.h);
    this->setData(ITEM_WIDTH, QVariant((int)blockData.w));
    this->setData(ITEM_HEIGHT, QVariant((int)blockData.h));
    setIncludedNPC(blockData.npc_id);
    arrayApply();
    scene->update();
}


void ItemBlock::setBlockData(LevelBlock inD, obj_block *mergedSet, long *animator)
{
    if(!scene) return;

    blockData = inD;

    if(mergedSet)
    {
        localProps = (*mergedSet);
        sizable = localProps.sizable;
        gridSize = localProps.grid;

        if(localProps.sizable)
        {
            setZValue( scene->Z_blockSizable + ((double)blockData.y/(double)100000000000)
                     + 1 - ((double)blockData.w * (double)0.0000000000000001) ); // applay sizable block Z

            setData(ITEM_BLOCK_IS_SIZABLE, "sizable" );
        }
        else
        {
            if(localProps.view==1)
                setZValue(scene->Z_BlockFore); // applay lava block Z
            else
                setZValue(scene->Z_Block); // applay standart block Z

            setData(ITEM_BLOCK_IS_SIZABLE, "standart" );
        }
        setData(ITEM_BLOCK_SHAPE, localProps.phys_shape);
    }

    if(animator)
        setAnimator((*animator));

    setPos(blockData.x, blockData.y);

    if(blockData.invisible)
        setOpacity(qreal(0.5));

    setIncludedNPC(blockData.npc_id, true);

    imageSize = QRectF(0,0,blockData.w, blockData.h);

    setData(ITEM_ID, QString::number(blockData.id) );
    setData(ITEM_ARRAY_ID, QString::number(blockData.array_id) );
    setData(ITEM_WIDTH, QString::number(blockData.w) ); //width
    setData(ITEM_HEIGHT, QString::number(blockData.h) ); //height
}


QRectF ItemBlock::boundingRect() const
{
    return imageSize;
}

void ItemBlock::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{
    if(animatorID<0)
    {
        painter->setPen(QPen(QBrush(Qt::red), 1, Qt::DotLine));
        painter->drawRect(QRect(0,0,1,1));
        return;
    }

    if(scene->animates_Blocks.size()>animatorID)
    {
        if(sizable)
            painter->drawPixmap(imageSize, currentImage, imageSize);
        else
            painter->drawPixmap(imageSize, scene->animates_Blocks[animatorID]->image(), imageSize);
    }
    else
        painter->drawRect(QRect(0,0,32,32));

    if(this->isSelected())
    {
        painter->setPen(QPen(QBrush(Qt::black), 2, Qt::SolidLine));
        painter->drawRect(1,1,imageSize.width()-2,imageSize.height()-2);
        painter->setPen(QPen(QBrush(Qt::green), 2, Qt::DotLine));
        painter->drawRect(1,1,imageSize.width()-2,imageSize.height()-2);
    }

}

void ItemBlock::setAnimator(long aniID)
{
    if(aniID<scene->animates_Blocks.size())
    imageSize = QRectF(0,0,
                scene->animates_Blocks[aniID]->image().width(),
                scene->animates_Blocks[aniID]->image().height()
                );
    if(!sizable)
    {
        blockData.w = qRound(imageSize.width()); //width
        blockData.h = qRound(imageSize.height()); //height
        setData(ITEM_WIDTH, QVariant((int)blockData.w));
        setData(ITEM_HEIGHT, QVariant((int)blockData.h));
    }
    animatorID = aniID;
    setMainPixmap();
}


//global Pointers
void ItemBlock::setScenePoint(LvlScene *theScene)
{
    scene = theScene;
    grp = new QGraphicsItemGroup(this);
    includedNPC = NULL;
}



void ItemBlock::setLocked(bool lock)
{
    this->setFlag(QGraphicsItem::ItemIsSelectable, !lock);
    this->setFlag(QGraphicsItem::ItemIsMovable, !lock);
    isLocked = lock;
}

//sizable Block formula
QPixmap ItemBlock::drawSizableBlock(int w, int h, QPixmap srcimg)
{
    int x,y, i, j;
    int hc, wc;

    QPixmap img(w, h);

    x = qRound(qreal(srcimg.width())/3);  // Width of one piece
    y = qRound(qreal(srcimg.height())/3); // Height of one piece

    img.fill(Qt::transparent);
    QPainter szblock(&img);

    int fLnt = 0; // Free Lenght
    int fWdt = 0; // Free Width

    int dX=0; //Draw Offset. This need for crop junk on small sizes
    int dY=0;

    if(w < 2*x) dX = (2*x-w)/2; else dX=0;
    if(h < 2*y) dY = (2*y-h)/2; else dY=0;

    //L Draw left border
    if(h > 2*y)
    {
        hc=0;
        for(i=0; i<((h-2*y) / y); i++ )
        {
            szblock.drawPixmap(0, x+hc, x-dX, y, srcimg.copy(0, y, x-dX, y));
                hc+=x;
        }
            fLnt = (h-2*y)%y;
            if( fLnt != 0) szblock.drawPixmap(0, x+hc, x-dX, fLnt, srcimg.copy(0, y, x-dX, fLnt) );
    }

    //T Draw top border
    if(w > 2*x)
    {
        hc=0;
        for(i=0; i<( (w-2*x) / x); i++ )
        {
            szblock.drawPixmap(x+hc, 0, x, y-dY, srcimg.copy(x, 0, x, y-dY) );
                hc+=x;
        }
            fLnt = (w-2*x)%x;
            if( fLnt != 0) szblock.drawPixmap(x+hc, 0, fLnt, y-dY, srcimg.copy(x, 0, fLnt, y-dY) );
    }

    //B Draw bottom border
    if(w > 2*x)
    {
        hc=0;
        for(i=0; i< ( (w-2*x) / x); i++ )
        {
            szblock.drawPixmap(x+hc, h-y+dY, x, y-dY, srcimg.copy(x, srcimg.width()-y+dY, x, y-dY) );
                hc+=x;
        }
            fLnt = (w-2*x)%x;
            if( fLnt != 0) szblock.drawPixmap(x+hc, h-y+dY, fLnt, y-dY, srcimg.copy(x, srcimg.width()-y+dY, fLnt, y-dY) );
    }

    //R Draw right border
    if(h > 2*y)
    {
        hc=0;
        for(i=0; i<((h-2*y) / y); i++ )
        {
            szblock.drawPixmap(w-x+dX, y+hc, x-dX, y, srcimg.copy(srcimg.width()-x+dX, y, x-dX, y));
                hc+=x;
        }
            fLnt = (h-2*y)%y;
            if( fLnt != 0) szblock.drawPixmap(w-x+dX, y+hc, x-dX, fLnt, srcimg.copy(srcimg.width()-x+dX, y, x-dX, fLnt));
    }

    //C Draw center
    if( w > 2*x && h > 2*y)
    {
        hc=0;
        wc=0;
        for(i=0; i<((h-2*y) / y); i++ )
        {
            hc=0;
            for(j=0; j<((w-2*x) / x); j++ )
            {
            szblock.drawPixmap(x+hc, y+wc, x, y, srcimg.copy(x, y, x, y));
                hc+=x;
            }
                fLnt = (w-2*x)%x;
                if(fLnt != 0 ) szblock.drawPixmap(x+hc, y+wc, fLnt, y, srcimg.copy(x, y, fLnt, y));
            wc+=y;
        }

        fWdt = (h-2*y)%y;
        if(fWdt !=0)
        {
            hc=0;
            for(j=0; j<((w-2*x) / x); j++ )
            {
            szblock.drawPixmap(x+hc, y+wc, x, fWdt, srcimg.copy(x, y, x, fWdt));
                hc+=x;
            }
                fLnt = (w-2*x)%x;
                if(fLnt != 0 ) szblock.drawPixmap(x+hc, y+wc, fLnt, fWdt, srcimg.copy(x, y, fLnt, fWdt));

        }

    }

    //Draw corners
     //1 Left-top
    szblock.drawPixmap(0,0,x-dX,y-dY, srcimg.copy(QRect(0,0,x-dX, y-dY)));
     //2 Right-top
    szblock.drawPixmap(w-x+dX, 0, x-dX, y-dY, srcimg.copy(QRect(srcimg.width()-x+dX, 0, x-dX, y-dY)) );
     //3 Right-bottom
    szblock.drawPixmap(w-x+dX, h-y+dY, x-dX, y-dY, srcimg.copy(QRect(srcimg.width()-x+dX, srcimg.height()-y+dY, x-dX, y-dY)) );
     //4 Left-bottom
    szblock.drawPixmap(0, h-y+dY, x-dX, y-dY, srcimg.copy(QRect(0, srcimg.height()-y+dY, x-dX, y-dY)) );

    szblock.end();
    return img;
}
