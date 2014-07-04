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

#include "item_block.h"
#include "../common_features/logger.h"

#include "../item_select_dialog/itemselectdialog.h"
#include "newlayerbox.h"

#include "../common_features/mainwinconnect.h"

ItemBlock::ItemBlock(QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    animated = false;
    animatorID=-1;
    imageSize = QRectF(0,0,10,10);

    gridSize=32;

    isLocked=false;
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
    if(scene->DrawMode)
    {
        unsetCursor();
        ungrabMouse();
        this->setSelected(false);
        return;
    }
    QGraphicsItem::mousePressEvent(mouseEvent);
}

void ItemBlock::contextMenuEvent( QGraphicsSceneContextMenuEvent * event )
{
    if((!scene->lock_block)&&(!scene->DrawMode)&&(!isLocked))
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
        LayerName->addSeparator()->deleteLater();;
        newLayer->deleteLater();

        foreach(LevelLayers layer, scene->LvlData->layers)
        {
            //Skip system layers
            if((layer.name=="Destroyed Blocks")||(layer.name=="Spawned NPCs")) continue;

            setLayer = LayerName->addAction( layer.name+((layer.hidden)?tr(" [hidden]"):"") );
            setLayer->setData(layer.name);
            setLayer->setCheckable(true);
            setLayer->setEnabled(true);
            setLayer->setChecked( layer.name==blockData.layer );
            newLayer->deleteLater();
            layerItems.push_back(setLayer);
        }

        ItemMenu->addSeparator()->deleteLater();;

        QAction *invis = ItemMenu->addAction(tr("Invisible"));
            invis->setCheckable(1);
            invis->setChecked( blockData.invisible );
            invis->deleteLater();

        QAction *slipp = ItemMenu->addAction(tr("Slippery"));
            slipp->setCheckable(1);
            slipp->setChecked( blockData.slippery );
            slipp->deleteLater();

        QAction *resize = ItemMenu->addAction(tr("Resize"));
            resize->setVisible( (this->data(3).toString()=="sizable") );
            resize->deleteLater();

        ItemMenu->addSeparator()->deleteLater();;
        QAction *chNPC = ItemMenu->addAction(tr("Change included NPC..."));
        chNPC->deleteLater();

        ItemMenu->addSeparator()->deleteLater();;
        QAction *copyBlock = ItemMenu->addAction( tr("Copy") );
        copyBlock->deleteLater();
        QAction *cutBlock = ItemMenu->addAction( tr("Cut") );
        cutBlock->deleteLater();
        ItemMenu->addSeparator()->deleteLater();;
        QAction *remove = ItemMenu->addAction( tr("Remove") );
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
            goto delItems;
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
                {
                    selData.blocks.push_back(((ItemBlock *) SelItem)->blockData);
                    ((ItemBlock *) SelItem)->setInvisible(invis->isChecked());
                }
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
                {
                    selData.blocks.push_back(((ItemBlock *) SelItem)->blockData);
                    ((ItemBlock *) SelItem)->setSlippery(slipp->isChecked());
                }
            }
            scene->addChangeSettingsHistory(selData, LvlScene::SETTING_SLIPPERY, QVariant(invis->isChecked()));
            scene->contextMenuOpened = false;
        }
        else
        if(selected==resize)
        {
            scene->setBlockResizer(this, true);
            scene->contextMenuOpened = false;
        }
        else
        if(selected==chNPC)
        {
            scene->contextMenuOpened = false;
            LevelData selData;
            ItemSelectDialog * npcList = new ItemSelectDialog(scene->pConfigs, ItemSelectDialog::TAB_NPC,
                                                       ItemSelectDialog::NPCEXTRA_WITHCOINS | (blockData.npc_id < 1000 && blockData.npc_id != 0 ? ItemSelectDialog::NPCEXTRA_ISCOINSELECTED : 0),0,0,
                                                       (blockData.npc_id < 1000 && blockData.npc_id != 0 ? blockData.npc_id : blockData.npc_id-1000));
            npcList->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
            npcList->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, npcList->size(), qApp->desktop()->availableGeometry()));
            if(npcList->exec()==QDialog::Accepted)
            {
                //apply to all selected items.
                int selected_npc=0;
                if(npcList->npcID!=0){
                    if(npcList->isCoin)
                        selected_npc = npcList->npcID;
                    else
                        selected_npc = npcList->npcID+1000;
                }

                foreach(QGraphicsItem * SelItem, scene->selectedItems() )
                {
                    if(SelItem->data(0).toString()=="Block")
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
            LevelData removedItems;
            bool deleted=false;
            scene->contextMenuOpened = false;
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(0).toString()=="Block")
                {
                    removedItems.blocks.push_back(((ItemBlock *)SelItem)->blockData);
                    ((ItemBlock *)SelItem)->removeFromArray();
                    scene->removeItem(SelItem);
                    delete SelItem;
                    deleted=true;
                }
            }
            if(deleted) MainWinConnect::pMainWin->activeLvlEditWin()->scene->addRemoveHistory( removedItems );
        }
        else
        if(selected==props)
        {
            scene->openProps();
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
                    MainWinConnect::pMainWin->setLayerToolsLocked(true);
                    MainWinConnect::pMainWin->setLayersBox();
                    MainWinConnect::pMainWin->setLayerToolsLocked(false);
                }
                delete layerBox;
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
                        if(selected==newLayer){
                            scene->addChangedNewLayerHistory(modData, lr);
                        }
                        break;
                    }
                }//Find layer's settings
                if(selected!=newLayer){
                    scene->addChangedLayerHistory(modData, lName);
                }
                scene->contextMenuOpened = false;
            }

        }
        delItems:;
    }
    else
    {
        QGraphicsItem::contextMenuEvent(event);
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

    if(!init) blockData.npc_id = npcID;
    if(!init) arrayApply();
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

void ItemBlock::setMainPixmap(/*const QPixmap &pixmap*/) // Init Sizable block
{
    //currentImage = pixmap;
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
    setIncludedNPC(blockData.npc_id);
    arrayApply();
    scene->update();
}


void ItemBlock::setBlockData(LevelBlock inD, bool is_sz)
{
    blockData = inD;
    sizable = is_sz;
    imageSize = QRectF(0,0,blockData.w, blockData.h);
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
    //this->setPixmap(scene->animates_Blocks[aniID]->image());
    this->setData(9, QString::number(qRound(imageSize.width())) ); //width
    this->setData(10, QString::number(qRound(imageSize.height())) ); //height

    //WriteToLog(QtDebugMsg, QString("BGO Animator ID: %1").arg(aniID));
    animatorID = aniID;
}

void ItemBlock::setContextMenu(QMenu &menu)
{
    ItemMenu = &menu;
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
    //QPixmap * sizableImage;
    //QPainter * szblock;
    x=32;
    y=32;

    //sizableImage = new QPixmap(QSize(w, h));
    img.fill(Qt::transparent);
    QPainter szblock(&img);

    //L
    hc=0;
    for(i=0; i<((h-2*y) / y); i++ )
    {
        szblock.drawPixmap(0, x+hc, x, y, srcimg.copy(QRect(0, y, x, y)));
            hc+=x;
    }

    //T
    hc=0;
    for(i=0; i<( (w-2*x) / x); i++ )
    {
        szblock.drawPixmap(x+hc, 0, x, y, srcimg.copy(QRect(x, 0, x, y)) );
            hc+=x;
    }

    //B
    hc=0;
    for(i=0; i< ( (w-2*x) / x); i++ )
    {
        szblock.drawPixmap(x+hc, h-y, x, y, srcimg.copy(QRect(x, srcimg.width()-y, x, y )) );
            hc+=x;
    }

    //R
    hc=0;
    for(i=0; i<((h-2*y) / y); i++ )
    {
        szblock.drawPixmap(w-x, y+hc, x, y, srcimg.copy(QRect(srcimg.width()-x, y, x, y)));
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
        szblock.drawPixmap(x+hc, y+wc, x, y, srcimg.copy(QRect(x, y, x, y)));
            hc+=x;
        }
        wc+=y;
    }

    //Applay sizable formula
     //1
    szblock.drawPixmap(0,0,y,x, srcimg.copy(QRect(0,0,y,x)));
     //2
    szblock.drawPixmap(w-y, 0, y, x, srcimg.copy(QRect(srcimg.width()-y, 0, y, x)) );
     //3
    szblock.drawPixmap(w-y, h-x, y, x, srcimg.copy(QRect(srcimg.width()-y, srcimg.height()-x, y, x)) );
     //4
    szblock.drawPixmap(0, h-x, y, x, srcimg.copy(QRect(0, srcimg.height()-x, y, x)) );

    //img = QPixmap( * sizableImage);
    //delete sizableImage;
    //delete szblock;
    return img;
}
