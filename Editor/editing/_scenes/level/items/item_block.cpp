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

#include <QInputDialog>
#include <editing/_dialogs/itemselectdialog.h>
#include <common_features/mainwinconnect.h>
#include <common_features/logger.h>
#include <common_features/util.h>
#include <PGE_File_Formats/file_formats.h>

#include "item_block.h"
#include "item_bgo.h"
#include "item_npc.h"
#include "item_water.h"
#include "item_door.h"
#include "../itemmsgbox.h"
#include "../newlayerbox.h"

ItemBlock::ItemBlock(QGraphicsItem *parent)
    : LvlBaseItem(parent)
{
    construct();
}

ItemBlock::ItemBlock(LvlScene *parentScene, QGraphicsItem *parent)
    : LvlBaseItem(parentScene, parent)
{
    construct();
    if(!parentScene) return;
    setScenePoint(parentScene);
    parentScene->addItem(this);
    setLocked(m_scene->lock_block);
}

void ItemBlock::construct()
{
    m_gridSize=32;
    setData(ITEM_TYPE, "Block");
}


ItemBlock::~ItemBlock()
{
    if(m_includedNPC!=NULL) delete m_includedNPC;
    if(m_grp!=NULL) delete m_grp;
    m_scene->unregisterElement(this);
}

void ItemBlock::contextMenu(QGraphicsSceneMouseEvent * mouseEvent)
{
    m_scene->contextMenuOpened=true;

    //Remove selection from non-block items
    if(!this->isSelected())
    {
        m_scene->clearSelection();
        this->setSelected(true);
    }

    this->setSelected(true);
    QMenu ItemMenu;

    /*************Layers*******************/
    QMenu * LayerName = ItemMenu.addMenu(tr("Layer: ")+QString("[%1]").arg(m_data.layer).replace("&", "&&&"));
    QAction *setLayer;
    QList<QAction *> layerItems;
    QAction * newLayer = LayerName->addAction(tr("Add to new layer..."));
                         LayerName->addSeparator()->deleteLater();;

    foreach(LevelLayer layer, m_scene->LvlData->layers)
    {
        //Skip system layers
        if((layer.name=="Destroyed Blocks")||(layer.name=="Spawned NPCs")) continue;

        setLayer = LayerName->addAction( layer.name.replace("&", "&&&")+((layer.hidden)?tr(" [hidden]"):"") );
        setLayer->setData(layer.name);
        setLayer->setCheckable(true);
        setLayer->setEnabled(true);
        setLayer->setChecked( layer.name==m_data.layer );
        layerItems.push_back(setLayer);
    }
    ItemMenu.addSeparator();
    /*************Layers*end***************/

    QAction *invis =           ItemMenu.addAction(tr("Invisible"));
        invis->setCheckable(1);
        invis->setChecked( m_data.invisible );

    QAction *slipp =           ItemMenu.addAction(tr("Slippery"));
        slipp->setCheckable(1);
        slipp->setChecked( m_data.slippery );

    QAction *resize =          ItemMenu.addAction(tr("Resize"));
        resize->setVisible( (this->data(ITEM_BLOCK_IS_SIZABLE).toString()=="sizable") );
                               ItemMenu.addSeparator();

    QAction *chNPC =           ItemMenu.addAction(tr("Change included NPC..."));
                               ItemMenu.addSeparator();
    QAction *transform =       ItemMenu.addAction(tr("Transform into"));
    QAction *transform_all_s = ItemMenu.addAction(tr("Transform all %1 in this section into").arg("BLOCK-%1").arg(m_data.id));
    QAction *transform_all =   ItemMenu.addAction(tr("Transform all %1 into").arg("BLOCK-%1").arg(m_data.id));
    QAction *makemsgevent =    ItemMenu.addAction(tr("Make message box..."));
                               ItemMenu.addSeparator();

    /*************Copy Preferences*******************/
    QMenu * copyPreferences =  ItemMenu.addMenu(tr("Copy preferences"));
    QAction *copyItemID =      copyPreferences->addAction(tr("Block-ID: %1").arg(m_data.id));
    QAction *copyPosXY =       copyPreferences->addAction(tr("Position: X, Y"));
    QAction *copyPosXYWH =     copyPreferences->addAction(tr("Position: X, Y, Width, Height"));
    QAction *copyPosLTRB =     copyPreferences->addAction(tr("Position: Left, Top, Right, Bottom"));
    /*************Copy Preferences*end***************/

    QAction *copyBlock = ItemMenu.addAction( tr("Copy") );
    QAction *cutBlock =  ItemMenu.addAction( tr("Cut") );
                         ItemMenu.addSeparator();
    QAction *remove =    ItemMenu.addAction( tr("Remove") );
                         ItemMenu.addSeparator();
    QAction *props =     ItemMenu.addAction(tr("Properties..."));

    /*****************Waiting for answer************************/
    QAction *selected =  ItemMenu.exec(mouseEvent->screenPos());
    /***********************************************************/

    if(!selected)
        return;


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
    if((selected==transform)||(selected==transform_all)||(selected==transform_all_s))
    {
        LevelData oldData;
        LevelData newData;

        int transformTO;
        ItemSelectDialog * blockList = new ItemSelectDialog(m_scene->pConfigs, ItemSelectDialog::TAB_BLOCK, 0,0,0,0,0,0,0,0,0,MainWinConnect::pMainWin);
        blockList->removeEmptyEntry(ItemSelectDialog::TAB_BLOCK);
        util::DialogToCenter(blockList, true);

        if(blockList->exec()==QDialog::Accepted)
        {
            QList<QGraphicsItem *> our_items;
            bool sameID=false;
            transformTO = blockList->blockID;
            unsigned long oldID = m_data.id;

            if(selected==transform)
                our_items=m_scene->selectedItems();
            else
            if(selected==transform_all)
            {
                our_items=m_scene->items();
                sameID=true;
            }
            else if(selected==transform_all_s)
            {
                bool ok=false;
                long mg = QInputDialog::getInt(NULL, tr("Margin of section"),
                               tr("Please select, how far items out of section should be removed too (in pixels)"),
                               32, 0, 214948, 1, &ok);
                if(!ok) goto cancelTransform;
                LevelSection &s=m_scene->LvlData->sections[m_scene->LvlData->CurSection];
                QRectF section;
                section.setLeft(s.size_left-mg);
                section.setTop(s.size_top-mg);
                section.setRight(s.size_right+mg);
                section.setBottom(s.size_bottom+mg);
                our_items=m_scene->items(section, Qt::IntersectsItemShape);
                sameID=true;
            }


            foreach(QGraphicsItem * SelItem, our_items )
            {
                if(SelItem->data(ITEM_TYPE).toString()=="Block")
                {
                    if((!sameID)||(((ItemBlock *) SelItem)->m_data.id==oldID))
                    {
                        oldData.blocks.push_back( ((ItemBlock *) SelItem)->m_data );
                        ((ItemBlock *) SelItem)->transformTo(transformTO);
                        newData.blocks.push_back( ((ItemBlock *) SelItem)->m_data );
                    }
                }
            }
            cancelTransform:;
        }
        delete blockList;

        if(!newData.blocks.isEmpty())
            m_scene->addTransformHistory(newData, oldData);
    }
    else
    if(selected==makemsgevent)
    {
        QString eventName;
        QString msgText;


        if(m_data.event_hit.isEmpty())
        {
            bool ok=false;
            typeEventAgain:
            eventName = QInputDialog::getText(m_scene->_edit, tr("Event name"),
                                              tr("Please enter the name of event:"),
                                              QLineEdit::Normal, QString(), &ok);
            if(eventName.isEmpty() && ok)
                goto typeEventAgain;
            if(ok)
            {
                ItemMsgBox msgBox(Opened_By::BLOCK, "", false, tr("Please, enter message which will be shown\nMessage limits: max line lenth is 27 characters"), tr("Hit message text"), MainWinConnect::pMainWin);
                util::DialogToCenter(&msgBox, true);
                if(msgBox.exec()==QDialog::Accepted)
                {
                    msgText = msgBox.currentText;
                    if(m_scene->LvlData->eventIsExist(eventName))
                    {
                        int count=0;
                        while(m_scene->LvlData->eventIsExist(QString(eventName+" %1").arg(count)))
                            count++;

                        eventName = QString(eventName+" %1").arg(count);
                    }

                        LevelSMBX64Event msgEvent = FileFormats::CreateLvlEvent();
                        msgEvent.name = eventName;
                        msgEvent.msg = msgText;
                        msgEvent.array_id = ++m_scene->LvlData->events_array_id;
                    m_scene->LvlData->events.push_back(msgEvent);
                        LevelData historyOldData;
                        historyOldData.blocks.push_back(m_data);
                    m_data.event_hit = eventName;
                    arrayApply();

                    m_scene->addAddEventHistory(msgEvent);
                    m_scene->addChangeSettingsHistory(historyOldData, HistorySettings::SETTING_EV_HITED, QVariant(eventName));

                    MainWinConnect::pMainWin->setEventsBox();
                    MainWinConnect::pMainWin->EventListsSync();

                    QMessageBox::information(m_scene->_edit, tr("Event has been created"),
                                 tr("Message event has been created!"), QMessageBox::Ok);
                }
            }
        }
        else
            QMessageBox::warning(m_scene->_edit, tr("'Hit' event slot is used"),
                         tr("Sorry, but 'Hit' event slot already used by '%1' event.")
                                 .arg(m_data.event_hit), QMessageBox::Ok);

    }
    else
    if(selected==invis)
    {
        //apply to all selected items.
        LevelData selData;
        foreach(QGraphicsItem * SelItem, m_scene->selectedItems() )
        {
            if(SelItem->data(ITEM_TYPE).toString()=="Block")
            {
                selData.blocks.push_back(((ItemBlock *) SelItem)->m_data);
                ((ItemBlock *) SelItem)->setInvisible(invis->isChecked());
            }
        }
        m_scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_INVISIBLE, QVariant(invis->isChecked()));
    }
    else
    if(selected==slipp)
    {
        //apply to all selected items.
        LevelData selData;
        foreach(QGraphicsItem * SelItem, m_scene->selectedItems() )
        {
            if(SelItem->data(ITEM_TYPE).toString()=="Block")
            {
                selData.blocks.push_back(((ItemBlock *) SelItem)->m_data);
                ((ItemBlock *) SelItem)->setSlippery(slipp->isChecked());
            }
        }
        m_scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_SLIPPERY, QVariant(invis->isChecked()));
    }
    else
    if(selected==resize)
    {
        m_scene->setBlockResizer(this, true);
    }
    else
    if(selected==chNPC)
    {
        LevelData selData;
        ItemSelectDialog * npcList = new ItemSelectDialog(m_scene->pConfigs, ItemSelectDialog::TAB_NPC,
                                                   ItemSelectDialog::NPCEXTRA_WITHCOINS | (m_data.npc_id < 0 && m_data.npc_id != 0 ? ItemSelectDialog::NPCEXTRA_ISCOINSELECTED : 0),0,0,
                                                   (m_data.npc_id <0 && m_data.npc_id != 0 ? m_data.npc_id *-1 : m_data.npc_id),
                                                          0,0,0,0,0,MainWinConnect::pMainWin);
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

            foreach(QGraphicsItem * SelItem, m_scene->selectedItems() )
            {
                if(SelItem->data(ITEM_TYPE).toString()=="Block")
                {
                    //((ItemBlock *) SelItem)->blockData.npc_id = selected_npc;
                    //((ItemBlock *) SelItem)->arrayApply();
                    selData.blocks.push_back(((ItemBlock *) SelItem)->m_data);
                    ((ItemBlock *) SelItem)->setIncludedNPC(selected_npc);
                }
            }
            m_scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_CHANGENPC, QVariant(selected_npc));
        }
        delete npcList;
    }
    else
    if(selected==remove)
    {
        m_scene->removeSelectedLvlItems();
    }
    else
    if(selected==props)
    {
        m_scene->openProps();
    }
    else
    if(selected==newLayer)
    {
        m_scene->setLayerToSelected();
    }
    else
    {
        //Fetch layers menu
        foreach(QAction * lItem, layerItems)
        {
            if(selected==lItem)
            {
                //FOUND!!!
                m_scene->setLayerToSelected(lItem->data().toString());
                break;
            }//Find selected layer's item
        }
    }
}

void ItemBlock::setSlippery(bool slip)
{
    m_data.slippery=slip;
    arrayApply(); //Apply changes into array
}

void ItemBlock::setInvisible(bool inv)
{
    m_data.invisible=inv;
    if(inv)
        this->setOpacity(0.5);
    else
        this->setOpacity(1);

    arrayApply();//Apply changes into array

}

void ItemBlock::setLayer(QString layer)
{
    foreach(LevelLayer lr, m_scene->LvlData->layers)
    {
        if(lr.name==layer)
        {
            m_data.layer = layer;
            this->setVisible(!lr.hidden);
            arrayApply();
        break;
        }
    }
}

void ItemBlock::setIncludedNPC(int npcID, bool init)
{
    if(m_includedNPC!=NULL)
    {
        m_grp->removeFromGroup(m_includedNPC);
        m_scene->removeItem(m_includedNPC);
        delete m_includedNPC;
        m_includedNPC = NULL;
    }
    if(npcID==0)
    {
        if(!init) m_data.npc_id = 0;
        if(!init) arrayApply();
        return;
    }

    QPixmap npcImg = QPixmap( m_scene->getNPCimg( ((npcID > 0)? (npcID) : m_scene->pConfigs->marker_npc.coin_in_block ) ) );
    m_includedNPC = m_scene->addPixmap( npcImg );

    m_includedNPC->setPos(
                (
                    m_data.x+((m_data.w-npcImg.width())/2)
                 ),
                (
                    m_data.y+((m_data.h-npcImg.height())/2)
                 ));
    m_includedNPC->setZValue(m_scene->Z_Block + 10);
    m_includedNPC->setOpacity(qreal(0.6));
    m_grp->addToGroup(m_includedNPC);

    if(!init) m_data.npc_id = npcID;
    if(!init) arrayApply();
}

void ItemBlock::transformTo(long target_id)
{
    if(target_id<1) return;
    if(!m_scene->uBlocks.contains(target_id))
        return;

    obj_block &mergedSet = m_scene->uBlocks[target_id];
    long animator = mergedSet.animator_id;

    m_data.id = target_id;
    setBlockData(m_data, &mergedSet, &animator);
    arrayApply();

    if(!m_scene->opts.animationEnabled)
        m_scene->update();
}

///////////////////MainArray functions/////////////////////////////
void ItemBlock::arrayApply()
{
    bool found=false;
    m_data.x = qRound(this->scenePos().x());
    m_data.y = qRound(this->scenePos().y());
    if(this->data(ITEM_BLOCK_IS_SIZABLE).toString()=="sizable")
        this->setZValue( m_scene->Z_blockSizable + ((double)m_data.y / (double) 100000000000) + 1 - ((double)m_data.w * (double)0.0000000000000001) );
    if(m_data.index < (unsigned int)m_scene->LvlData->blocks.size())
    { //Check index
        if(m_data.array_id == m_scene->LvlData->blocks[m_data.index].array_id)
            found=true;
    }

    //Apply current data in main array
    if(found)
    { //directlry
        m_scene->LvlData->blocks[m_data.index] = m_data; //apply current blockdata
    }
    else
    for(int i=0; i<m_scene->LvlData->blocks.size(); i++)
    { //after find it into array
        if(m_scene->LvlData->blocks[i].array_id == m_data.array_id)
        {
            m_data.index = i;
            m_scene->LvlData->blocks[i] = m_data;
            break;
        }
    }
    //Update R-tree innex
    m_scene->unregisterElement(this);
    m_scene->registerElement(this);
}

void ItemBlock::removeFromArray()
{
    bool found=false;
    if(m_data.index < (unsigned int)m_scene->LvlData->blocks.size())
    { //Check index
        if(m_data.array_id == m_scene->LvlData->blocks[m_data.index].array_id)
            found=true;
    }
    if(found)
    { //directlry
        m_scene->LvlData->blocks.removeAt(m_data.index);
    }
    else
    for(int i=0; i<m_scene->LvlData->blocks.size(); i++)
    {
        if(m_scene->LvlData->blocks[i].array_id == m_data.array_id)
        {
            m_scene->LvlData->blocks.removeAt(i); break;
        }
    }
}

void ItemBlock::returnBack()
{
    this->setPos(m_data.x, m_data.y);
}

QPoint ItemBlock::sourcePos()
{
    return QPoint(m_data.x, m_data.y);
}


void ItemBlock::setMainPixmap() // Init Sizable block
{
    if(m_sizable)
    {
        drawSizableBlock(m_data.w, m_data.h, m_scene->animates_Blocks[m_animatorID]->wholeImage());
        m_imageSize = QRectF(0,0, m_data.w, m_data.h);
    }
}

void ItemBlock::setBlockSize(QRect rect)
{
    if(m_sizable)
    {
        m_data.x = rect.x();
        m_data.y = rect.y();
        m_data.w = rect.width();
        m_data.h = rect.height();
        drawSizableBlock(m_data.w, m_data.h, m_scene->animates_Blocks[m_animatorID]->wholeImage());
        this->setPos(m_data.x, m_data.y);
    }
    m_imageSize = QRectF(0,0, m_data.w, m_data.h);
    this->setData(ITEM_WIDTH, QVariant((int)m_data.w));
    this->setData(ITEM_HEIGHT, QVariant((int)m_data.h));
    setIncludedNPC(m_data.npc_id);
    arrayApply();
    m_scene->update();
}


void ItemBlock::setBlockData(LevelBlock inD, obj_block *mergedSet, long *animator)
{
    if(!m_scene) return;

    m_data = inD;

    if(mergedSet)
    {
        m_localProps = (*mergedSet);
        m_sizable = m_localProps.sizable;
        m_gridSize = m_localProps.grid;

        if(m_localProps.sizable)
        {
            setZValue( m_scene->Z_blockSizable + ((double)m_data.y/(double)100000000000)
                     + 1 - ((double)m_data.w * (double)0.0000000000000001) ); // applay sizable block Z

            setData(ITEM_BLOCK_IS_SIZABLE, "sizable" );
        }
        else
        {
            if(m_localProps.view==1)
                setZValue(m_scene->Z_BlockFore); // applay lava block Z
            else
                setZValue(m_scene->Z_Block); // applay standart block Z

            setData(ITEM_BLOCK_IS_SIZABLE, "standart" );
        }
        setData(ITEM_BLOCK_SHAPE, m_localProps.phys_shape);
    }

    if(animator)
        setAnimator((*animator));

    setPos(m_data.x, m_data.y);

    if(m_data.invisible)
        setOpacity(qreal(0.5));

    setIncludedNPC(m_data.npc_id, true);

    m_imageSize = QRectF(0,0,m_data.w, m_data.h);

    setData(ITEM_ID, QString::number(m_data.id) );
    setData(ITEM_ARRAY_ID, QString::number(m_data.array_id) );
    setData(ITEM_WIDTH, QString::number(m_data.w) ); //width
    setData(ITEM_HEIGHT, QString::number(m_data.h) ); //height

    m_scene->unregisterElement(this);
    m_scene->registerElement(this);
}


QRectF ItemBlock::boundingRect() const
{
    return m_imageSize;
}

void ItemBlock::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{
    if(m_animatorID<0)
    {
        painter->setPen(QPen(QBrush(Qt::red), 1, Qt::DotLine));
        painter->drawRect(QRect(0,0,1,1));
        return;
    }

    if(m_scene->animates_Blocks.size()>m_animatorID)
    {
        if(m_sizable)
            painter->drawPixmap(m_imageSize, m_currentImage, m_imageSize);
        else
            painter->drawPixmap(m_imageSize, m_scene->animates_Blocks[m_animatorID]->image(), m_imageSize);
    }
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

void ItemBlock::setAnimator(long aniID)
{
    if(aniID<m_scene->animates_Blocks.size())
    m_imageSize = QRectF(0,0,
                m_scene->animates_Blocks[aniID]->image().width(),
                m_scene->animates_Blocks[aniID]->image().height()
                );
    if(!m_sizable)
    {
        m_data.w = qRound(m_imageSize.width()); //width
        m_data.h = qRound(m_imageSize.height()); //height
        setData(ITEM_WIDTH, QVariant((int)m_data.w));
        setData(ITEM_HEIGHT, QVariant((int)m_data.h));
    }
    m_animatorID = aniID;
    setMainPixmap();
}

//global Pointers
void ItemBlock::setScenePoint(LvlScene *theScene)
{
    LvlBaseItem::setScenePoint(theScene);
    m_grp = new QGraphicsItemGroup(this);
    m_includedNPC = NULL;
}

bool ItemBlock::itemTypeIsLocked()
{
    if(!m_scene)
        return false;
    return m_scene->lock_block;
}

//sizable Block formula
void ItemBlock::drawSizableBlock(int w, int h, QPixmap srcimg)
{
    int x,y, i, j;
    int hc, wc;

    m_currentImage=QPixmap(w, h);

    x = qRound(qreal(srcimg.width())/3);  // Width of one piece
    y = qRound(qreal(srcimg.height())/3); // Height of one piece

    int x2 = x<<1; // 2*x
    int y2 = y<<1; // 2*y

    int pWidth = srcimg.width()-x2;//Width of center piece
    int pHeight = srcimg.height()-y2;//Height of center piece

    m_currentImage.fill(Qt::transparent);
    QPainter szblock(&m_currentImage);

    int fLnt = 0; // Free Lenght
    int fWdt = 0; // Free Width

    int dX=0; //Draw Offset. This need for crop junk on small sizes
    int dY=0;

    if(w < x2) dX = (x2-w)/2; else dX=0;
    if(h < y2) dY = (y2-h)/2; else dY=0;

    int totalW = ((w-x2) / x);
    int totalH = ((h-y2) / y);
    //L Draw left border
    if(h > y2)
    {
        hc=0;
        for(i=0; i< totalH; i++ )
        {
            szblock.drawPixmap(0, x+hc, x-dX, pHeight, srcimg.copy(0, y, x-dX, pHeight));
                hc+=pHeight;
        }
            fLnt = (h-y2)%pHeight;
            if( fLnt != 0) szblock.drawPixmap(0, x+hc, x-dX, fLnt, srcimg.copy(0, y, x-dX, fLnt) );
    }

    //T Draw top border
    if(w > x2)
    {
        hc=0;
        for(i=0; i<totalW; i++ )
        {
            szblock.drawPixmap(x+hc, 0, pWidth, y-dY, srcimg.copy(x, 0, pWidth, y-dY) );
                hc+=pWidth;
        }
            fLnt = (w-x2)%pWidth;
            if( fLnt != 0) szblock.drawPixmap(x+hc, 0, fLnt, y-dY, srcimg.copy(x, 0, fLnt, y-dY) );
    }

    //B Draw bottom border
    if(w > x2)
    {
        hc=0;
        for(i=0; i< totalW; i++ )
        {
            szblock.drawPixmap(x+hc, h-y+dY, pWidth, y-dY, srcimg.copy(x, srcimg.height()-y+dY, pWidth, y-dY) );
                hc+=pWidth;
        }
            fLnt = (w-x2)%pWidth;
            if( fLnt != 0) szblock.drawPixmap(x+hc, h-y+dY, fLnt, y-dY, srcimg.copy(x, srcimg.height()-y+dY, fLnt, y-dY) );
    }

    //R Draw right border
    if(h > y2)
    {
        hc=0;
        for(i=0; i<totalH; i++ )
        {
            szblock.drawPixmap(w-x+dX, y+hc, x-dX, pHeight, srcimg.copy(srcimg.width()-x+dX, y, x-dX, pHeight));
                hc+=pHeight;
        }
            fLnt = (h-y2)%pHeight;
            if( fLnt != 0) szblock.drawPixmap(w-x+dX, y+hc, x-dX, fLnt, srcimg.copy(srcimg.width()-x+dX, y, x-dX, fLnt));
    }

    //C Draw center
    if( w > x2 && h > y2)
    {
        hc=0;
        wc=0;
        for(i=0; i<totalH; i++ )
        {
            hc=0;
            for(j=0; j<totalW; j++ )
            {
            szblock.drawPixmap(x+hc, y+wc, pWidth, pHeight, srcimg.copy(x, y, pWidth, pHeight));
                hc+=pWidth;
            }
                fLnt = (w-x2)%pWidth;
                if(fLnt != 0 ) szblock.drawPixmap(x+hc, y+wc, fLnt, pHeight, srcimg.copy(x, y, fLnt, pHeight));
            wc+=y;
        }

        fWdt = (h-y2)%pHeight;
        if(fWdt !=0)
        {
            hc=0;
            for(j=0; j<totalW; j++ )
            {
            szblock.drawPixmap(x+hc, y+wc, pWidth, fWdt, srcimg.copy(x, y, pWidth, fWdt));
                hc+=pWidth;
            }
                fLnt = (w-x2)%pWidth;
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
}
