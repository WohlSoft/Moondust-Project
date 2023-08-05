/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2023 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <QClipboard>

#include <mainwindow.h>
#include <editing/_dialogs/itemselectdialog.h>
#include <editing/_dialogs/user_data_edit.h>
#include <common_features/logger.h>
#include <common_features/util.h>
#include <common_features/graphics_funcs.h>
#include <PGE_File_Formats/file_formats.h>

#include "../lvl_history_manager.h"
#include "../itemmsgbox.h"
#include "../newlayerbox.h"

#include <editing/_components/history/settings/lvl_block_userdata.hpp>

static inline double sizableBlockZ(const LevelBlock &b)
{
    return (static_cast<double>(b.y) / 100000000000.0) + 1.0 -
           (static_cast<double>(b.w) * 0.0000000000000001);
}

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
    setLocked(m_scene->m_lockBlock);
}

void ItemBlock::construct()
{
    m_gridSize = 32;
    setData(ITEM_TYPE, "Block");
    m_includedNPC = nullptr;
    m_coinCounter = nullptr;
    m_grp = nullptr;
}


ItemBlock::~ItemBlock()
{
    if(m_includedNPC != nullptr) delete m_includedNPC;
    if(m_coinCounter != nullptr) delete m_coinCounter;
    if(m_grp != nullptr) delete m_grp;
    m_scene->unregisterElement(this);
}

void ItemBlock::contextMenu(QGraphicsSceneMouseEvent *mouseEvent)
{
    m_scene->m_contextMenuIsOpened = true;

    //Remove selection from non-block items
    if(!this->isSelected())
    {
        m_scene->clearSelection();
        this->setSelected(true);
    }

    this->setSelected(true);
    QMenu ItemMenu;

    /*************Layers*******************/
    QMenu *layerName = ItemMenu.addMenu(tr("Layer: ") + QString("[%1]").arg(m_data.layer).replace("&", "&&"));
    QAction *setLayer;
    QList<QAction *> layerItems;
    QAction *newLayer = layerName->addAction(tr("Add to new layer..."));
    layerName->addSeparator()->deleteLater();

    for(const LevelLayer &layer : m_scene->m_data->layers)
    {
        //Skip system layers
        if((layer.name == "Destroyed Blocks") || (layer.name == "Spawned NPCs"))
            continue;

        QString label = layer.name + ((layer.hidden) ? " " + tr("[hidden]") : "");
        setLayer = layerName->addAction(label.replace("&", "&&"));
        setLayer->setData(layer.name);
        setLayer->setCheckable(true);
        setLayer->setEnabled(true);
        setLayer->setChecked(layer.name == m_data.layer);
        layerItems.push_back(setLayer);
    }
    ItemMenu.addSeparator();
    /*************Layers*end***************/

    QAction *invis =           ItemMenu.addAction(tr("Invisible"));
    invis->setCheckable(1);
    invis->setChecked(m_data.invisible);

    QAction *slipp =           ItemMenu.addAction(tr("Slippery"));
    slipp->setCheckable(1);
    slipp->setChecked(m_data.slippery);

    QAction *resize =          ItemMenu.addAction(tr("Resize"));
    resize->setVisible((this->data(ITEM_BLOCK_IS_SIZABLE).toString() == "sizable"));
    ItemMenu.addSeparator();

    QAction *chNPC =           ItemMenu.addAction(tr("Change included NPC..."));
    ItemMenu.addSeparator();
    QMenu   *transforms = ItemMenu.addMenu(tr("Transform into"));
    QAction *transform =       transforms->addAction(tr("Transform into"));
    QAction *transform_all_s = transforms->addAction(tr("Transform all %1 in this section into").arg("BLOCK-%1").arg(m_data.id));
    QAction *transform_all =   transforms->addAction(tr("Transform all %1 into").arg("BLOCK-%1").arg(m_data.id));
    QAction *makemsgevent =    ItemMenu.addAction(tr("Make message box..."));
    ItemMenu.addSeparator();

    /*************Copy Preferences*******************/
    QMenu *copyPreferences =  ItemMenu.addMenu(tr("Copy preferences"));
    QAction *copyArrayID =      copyPreferences->addAction(tr("Array-ID: %1").arg(m_data.meta.array_id));
    QAction *copyItemID =      copyPreferences->addAction(tr("Block-ID: %1").arg(m_data.id));
    QAction *copyPosXY =       copyPreferences->addAction(tr("Position: X, Y"));
    QAction *copyPosXYWH =     copyPreferences->addAction(tr("Position: X, Y, Width, Height"));
    QAction *copyPosLTRB =     copyPreferences->addAction(tr("Position: Left, Top, Right, Bottom"));
    /*************Copy Preferences*end***************/

    QAction *copyBlock = ItemMenu.addAction(tr("Copy"));
    QAction *cutBlock =  ItemMenu.addAction(tr("Cut"));
    ItemMenu.addSeparator();

    QAction *remove =    ItemMenu.addAction(tr("Remove"));
    QAction *remove_all_s =     ItemMenu.addAction(tr("Remove all %1 in this section").arg("BLOCK-%1").arg(m_data.id));
    QAction *remove_all =       ItemMenu.addAction(tr("Remove all %1").arg("BLOCK-%1").arg(m_data.id));
    ItemMenu.addSeparator();

    QAction *rawUserData =      ItemMenu.addAction(tr("Edit raw user data..."));
    ItemMenu.addSeparator();

    QAction *props =     ItemMenu.addAction(tr("Properties..."));

    /*****************Waiting for answer************************/
    QAction *selected =  ItemMenu.exec(mouseEvent->screenPos());
    /***********************************************************/

    if(!selected)
        return;

    if(selected == copyArrayID)
    {
        QApplication::clipboard()->setText(QString("%1").arg(m_data.meta.array_id));
        m_scene->m_mw->showStatusMsg(tr("Preferences have been copied: %1").arg(QApplication::clipboard()->text()));
    }
    else if(selected == copyItemID)
    {
        QApplication::clipboard()->setText(QString("%1").arg(m_data.id));
        m_scene->m_mw->showStatusMsg(tr("Preferences have been copied: %1").arg(QApplication::clipboard()->text()));
    }
    else if(selected == copyPosXY)
    {
        QApplication::clipboard()->setText(
            QString("X=%1; Y=%2;")
            .arg(m_data.x)
            .arg(m_data.y)
        );
        m_scene->m_mw->showStatusMsg(tr("Preferences have been copied: %1").arg(QApplication::clipboard()->text()));
    }
    else if(selected == copyPosXYWH)
    {
        QApplication::clipboard()->setText(
            QString("X=%1; Y=%2; W=%3; H=%4;")
            .arg(m_data.x)
            .arg(m_data.y)
            .arg(m_imageSize.width())
            .arg(m_imageSize.height())
        );
        m_scene->m_mw->showStatusMsg(tr("Preferences have been copied: %1").arg(QApplication::clipboard()->text()));
    }
    else if(selected == copyPosLTRB)
    {
        QApplication::clipboard()->setText(
            QString("Left=%1; Top=%2; Right=%3; Bottom=%4;")
            .arg(m_data.x)
            .arg(m_data.y)
            .arg(m_data.x + m_imageSize.width())
            .arg(m_data.y + m_imageSize.height())
        );
        m_scene->m_mw->showStatusMsg(tr("Preferences have been copied: %1").arg(QApplication::clipboard()->text()));
    }
    else if(selected == cutBlock)
        m_scene->m_mw->on_actionCut_triggered();
    else if(selected == copyBlock)
        m_scene->m_mw->on_actionCopy_triggered();
    else if((selected == transform) || (selected == transform_all) || (selected == transform_all_s))
    {
        LvlScene *scene = m_scene;
        LevelData *ldata = scene->m_data;
        LevelData oldData;
        LevelData newData;

        int transformTO;
        int transformToBGO;
        int tabType;
        ItemSelectDialog *blockList = new ItemSelectDialog(scene->m_configs,
                ItemSelectDialog::TAB_BLOCK | ItemSelectDialog::TAB_BGO,
                0, 0, 0, 0, 0, 0, 0, 0, 0, scene->m_subWindow,
                ItemSelectDialog::TAB_BLOCK | ItemSelectDialog::TAB_BGO);
        util::DialogToCenter(blockList, true);

        if(blockList->exec() == QDialog::Accepted)
        {
            QList<QGraphicsItem *> our_items;
            bool sameID = false;
            transformTO = blockList->blockID;
            transformToBGO = blockList->bgoID;
            tabType = blockList->currentTab;
            unsigned long oldID = m_data.id;

            if(selected == transform)
                our_items = scene->selectedItems();
            else if(selected == transform_all)
            {
                our_items = scene->items();
                sameID = true;
            }
            else if(selected == transform_all_s)
            {
                bool ok = false;
                long mg = QInputDialog::getInt(scene->m_subWindow, tr("Margin of section"),
                                               tr("Please select how far items can travel beyond the section boundaries (in pixels) before they are removed."),
                                               32, 0, 214948, 1, &ok);
                if(!ok) goto cancelTransform;
                LevelSection &s = ldata->sections[ldata->CurSection];
                QRectF section;
                section.setLeft(s.size_left - mg);
                section.setTop(s.size_top - mg);
                section.setRight(s.size_right + mg);
                section.setBottom(s.size_bottom + mg);
                our_items = scene->items(section, Qt::IntersectsItemShape);
                sameID = true;
            }

            //Change ID of each block
            if(tabType == ItemSelectDialog::TAB_BLOCK)
            {
                for(QGraphicsItem *SelItem : our_items)
                {
                    if(SelItem->data(ITEM_TYPE).toString() == "Block")
                    {
                        ItemBlock *item = (ItemBlock *)SelItem;
                        if((!sameID) || (item->m_data.id == oldID))
                        {
                            oldData.blocks.push_back(item->m_data);
                            item->transformTo(transformTO);
                            newData.blocks.push_back(item->m_data);
                        }
                    }
                }
            }
            //Transform every block into BGO
            else if(tabType == ItemSelectDialog::TAB_BGO)
            {
                for(QGraphicsItem *SelItem : our_items)
                {
                    if(SelItem->data(ITEM_TYPE).toString() == "Block")
                    {
                        ItemBlock *item = (ItemBlock *)SelItem;
                        if((!sameID) || (item->m_data.id == oldID))
                        {
                            oldData.blocks.push_back(item->m_data);
                            LevelBGO bgo;
                            bgo.id = transformToBGO;
                            bgo.x = item->m_data.x;
                            bgo.y = item->m_data.y;
                            bgo.layer = item->m_data.layer;
                            bgo.meta = item->m_data.meta;
                            bgo.meta.array_id = (ldata->bgo_array_id)++;
                            scene->placeBGO(bgo);
                            ldata->bgo.push_back(bgo);
                            newData.bgo.push_back(bgo);
                            item->removeFromArray();
                            delete item;
                        }
                    }
                }
            }
cancelTransform:
            ;
        }
        delete blockList;

        if(!newData.blocks.isEmpty() || !newData.bgo.isEmpty())
            scene->m_history->addTransform(newData, oldData);
    }
    else if(selected == makemsgevent)
    {
        QString eventName;
        QString msgText;


        if(m_data.event_hit.isEmpty())
        {
            bool ok = false;
typeEventAgain:
            eventName = QInputDialog::getText(m_scene->m_subWindow, tr("Event name"),
                                              tr("Please enter the name of event:"),
                                              QLineEdit::Normal, QString(), &ok);
            if(eventName.isEmpty() && ok)
                goto typeEventAgain;
            if(ok)
            {
                ItemMsgBox msgBox(Opened_By::BLOCK, "", false,
                                  tr("Please enter the message which will be shown.\n(Max line length is 27 characters)"),
                                  tr("Hit message text"), m_scene->m_subWindow);
                util::DialogToCenter(&msgBox, true);
                if(msgBox.exec() == QDialog::Accepted)
                {
                    msgText = msgBox.currentText;
                    if(m_scene->m_data->eventIsExist(eventName))
                    {
                        int count = 0;
                        while(m_scene->m_data->eventIsExist(QString(eventName + " %1").arg(count)))
                            count++;

                        eventName = QString(eventName + " %1").arg(count);
                    }

                    LevelSMBX64Event msgEvent = FileFormats::CreateLvlEvent();
                    msgEvent.name = eventName;
                    msgEvent.msg = msgText;
                    msgEvent.meta.array_id = ++m_scene->m_data->events_array_id;
                    m_scene->m_data->events.push_back(msgEvent);
                    LevelData historyOldData;
                    historyOldData.blocks.push_back(m_data);
                    m_data.event_hit = eventName;
                    arrayApply();

                    m_scene->m_history->addAddEvent(msgEvent);
                    m_scene->m_history->addChangeSettings(historyOldData, HistorySettings::SETTING_EV_HITED, QVariant(eventName));

                    m_scene->m_mw->setEventsBox();
                    m_scene->m_mw->EventListsSync();

                    QMessageBox::information(m_scene->m_subWindow, tr("Event created"),
                                             tr("Message event created!"), QMessageBox::Ok);
                }
            }
        }
        else
            QMessageBox::warning(m_scene->m_subWindow, tr("'Hit' event slot is used"),
                                 tr("Sorry, but the 'Hit' event slot already used by the event: '%1'.")
                                 .arg(m_data.event_hit), QMessageBox::Ok);

    }
    else if(selected == invis)
    {
        //apply to all selected items.
        LevelData selData;
        for(QGraphicsItem *SelItem : m_scene->selectedItems())
        {
            if(SelItem->data(ITEM_TYPE).toString() == "Block")
            {
                selData.blocks.push_back(((ItemBlock *) SelItem)->m_data);
                ((ItemBlock *) SelItem)->setInvisible(invis->isChecked());
            }
        }
        m_scene->m_history->addChangeSettings(selData, HistorySettings::SETTING_INVISIBLE, QVariant(invis->isChecked()));
    }
    else if(selected == slipp)
    {
        //apply to all selected items.
        LevelData selData;
        for(QGraphicsItem *SelItem : m_scene->selectedItems())
        {
            if(SelItem->data(ITEM_TYPE).toString() == "Block")
            {
                selData.blocks.push_back(((ItemBlock *) SelItem)->m_data);
                ((ItemBlock *) SelItem)->setSlippery(slipp->isChecked());
            }
        }
        m_scene->m_history->addChangeSettings(selData, HistorySettings::SETTING_SLIPPERY, QVariant(invis->isChecked()));
    }
    else if(selected == resize)
        m_scene->setBlockResizer(this, true);
    else if(selected == chNPC)
    {
        LevelData selData;
        ItemSelectDialog *npcList = new ItemSelectDialog(m_scene->m_configs, ItemSelectDialog::TAB_NPC,
                ItemSelectDialog::NPCEXTRA_WITHCOINS | (m_data.npc_id < 0 && m_data.npc_id != 0 ? ItemSelectDialog::NPCEXTRA_ISCOINSELECTED : 0), 0, 0,
                (m_data.npc_id < 0 && m_data.npc_id != 0 ? m_data.npc_id * -1 : m_data.npc_id),
                0, 0, 0, 0, 0, m_scene->m_mw);
        npcList->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
        npcList->setGeometry(util::alignToScreenCenter(npcList->size()));
        if(npcList->exec() == QDialog::Accepted)
        {
            //apply to all selected items.
            int selected_npc = 0;
            if(npcList->npcID != 0)
            {
                if(npcList->isCoin)
                    selected_npc = npcList->npcID * -1;
                else
                    selected_npc = npcList->npcID;
            }

            foreach(QGraphicsItem *SelItem, m_scene->selectedItems())
            {
                if(SelItem->data(ITEM_TYPE).toString() == "Block")
                {
                    //((ItemBlock *) SelItem)->blockData.npc_id = selected_npc;
                    //((ItemBlock *) SelItem)->arrayApply();
                    selData.blocks.push_back(((ItemBlock *) SelItem)->m_data);
                    ((ItemBlock *) SelItem)->setIncludedNPC(selected_npc);
                }
            }
            m_scene->m_history->addChangeSettings(selData, HistorySettings::SETTING_CHANGENPC, QVariant(selected_npc));
        }
        delete npcList;
    }
    else if(selected == remove)
        m_scene->removeSelectedLvlItems();
    else if((selected == remove_all_s) || (selected == remove_all))
    {
        QList<QGraphicsItem *> our_items;
        QList<QGraphicsItem *> selectedList;
        unsigned long oldID = m_data.id;

        if(selected == remove_all)
            our_items = m_scene->items();
        else if(selected == remove_all_s)
        {
            bool ok = false;
            long mg = QInputDialog::getInt(m_scene->m_subWindow, tr("Margin of section"),
                                           tr("Please select how far items can travel beyond the section boundaries (in pixels) before they are removed."),
                                           32, 0, 214948, 1, &ok);
            if(!ok) goto cancelRemoveSSS;
            LevelSection &s = m_scene->m_data->sections[m_scene->m_data->CurSection];
            QRectF section;
            section.setLeft(s.size_left - mg);
            section.setTop(s.size_top - mg);
            section.setRight(s.size_right + mg);
            section.setBottom(s.size_bottom + mg);
            our_items = m_scene->items(section, Qt::IntersectsItemShape);
        }

        for(QGraphicsItem *SelItem : our_items)
        {
            if(SelItem->data(ITEM_TYPE).toString() == "Block")
            {
                if(((ItemBlock *) SelItem)->m_data.id == oldID)
                    selectedList.push_back(SelItem);
            }
        }

        if(!selectedList.isEmpty())
        {
            LvlScene *scene = m_scene;
            scene->removeLvlItems(selectedList);
            scene->Debugger_updateItemList();
        }
cancelRemoveSSS:
        ;
    }
    else if(selected == rawUserData)
    {
        UserDataEdit d(m_data.meta.custom_params, m_scene->m_subWindow);
        int ret = d.exec();
        if(ret == QDialog::Accepted)
        {
            LevelData selData;
            QString ch = d.getText();
            foreach(QGraphicsItem *SelItem, m_scene->selectedItems())
            {
                if(SelItem->data(ITEM_TYPE).toString() == "Block")
                {
                    selData.blocks.push_back(((ItemBlock *)SelItem)->m_data);
                    ((ItemBlock *) SelItem)->m_data.meta.custom_params = ch;
                    ((ItemBlock *) SelItem)->arrayApply();
                }
            }
            m_scene->m_history->addChangeSettings(selData, new BlockHistory_UserData(), ch);
        }
    }
    else if(selected == props)
        m_scene->openProps();
    else if(selected == newLayer)
        m_scene->setLayerToSelected();
    else
    {
        //Fetch layers menu
        foreach(QAction *lItem, layerItems)
        {
            if(selected == lItem)
            {
                //FOUND!!!
                m_scene->setLayerToSelected(lItem->data().toString());
                break;
            }//Find selected layer's item
        }
    }
}

bool ItemBlock::isSizable()
{
    return m_sizable;
}

void ItemBlock::setMetaSignsVisibility(bool visible)
{
    if(m_includedNPC)
        m_includedNPC->setVisible(visible);
    if(m_coinCounter)
        m_coinCounter->setVisible(visible);
}

void ItemBlock::updateSizableBorder(const QPixmap &srcimg)
{
    sizable_border.g = m_localProps.setup.sizable_border_width < 0 ?
                       m_scene->m_configs->defaultBlock.sizable_block_border_size :
                       m_localProps.setup.sizable_border_width;
    sizable_border.l = m_localProps.setup.sizable_border_width_left;
    sizable_border.t = m_localProps.setup.sizable_border_width_top;
    sizable_border.r = m_localProps.setup.sizable_border_width_right;
    sizable_border.b = m_localProps.setup.sizable_border_width_bottom;

    if(sizable_border.l <= 0)
        sizable_border.l = sizable_border.g;
    if(sizable_border.r <= 0)
        sizable_border.r = sizable_border.g;
    if(sizable_border.t <= 0)
        sizable_border.t = sizable_border.g;
    if(sizable_border.b <= 0)
        sizable_border.b = sizable_border.g;

    if((sizable_border.l <= 0) && (sizable_border.g <= 0))
        sizable_border.l = qRound(double(srcimg.width()) / 3);
    if((sizable_border.r <= 0) && (sizable_border.g <= 0))
        sizable_border.r = qRound(double(srcimg.width()) / 3);
    if((sizable_border.b <= 0) && (sizable_border.g <= 0))
        sizable_border.b = qRound(double(srcimg.height()) / 3);
    if((sizable_border.t <= 0) && (sizable_border.g <= 0))
        sizable_border.t = qRound(double(srcimg.height()) / 3);
}

void ItemBlock::setSlippery(bool slip)
{
    m_data.slippery = slip;
    arrayApply(); //Apply changes into array
}

void ItemBlock::setInvisible(bool inv)
{
    m_data.invisible = inv;
    if(inv)
        this->setOpacity(0.5);
    else
        this->setOpacity(1);

    arrayApply();//Apply changes into array

}

QString ItemBlock::getLayerName() {
    return m_data.layer;
}

void ItemBlock::setLayer(QString layer)
{
    foreach(LevelLayer lr, m_scene->m_data->layers)
    {
        if(lr.name == layer)
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
    if(m_includedNPC != nullptr)
    {
        m_grp->removeFromGroup(m_includedNPC);
        m_scene->removeItem(m_includedNPC);
        delete m_includedNPC;
        m_includedNPC = nullptr;
    }
    if(m_coinCounter != nullptr)
    {
        m_grp->removeFromGroup(m_coinCounter);
        m_scene->removeItem(m_coinCounter);
        delete m_coinCounter;
        m_coinCounter = nullptr;
    }

    if(npcID == 0)
    {
        if(!init) m_data.npc_id = 0;
        if(!init) arrayApply();
        return;
    }

    QPixmap npcImg = QPixmap(m_scene->getNPCimg(((npcID > 0) ? (npcID) : m_scene->m_configs->marker_npc.coin_in_block)));
    m_includedNPC = m_scene->addPixmap(npcImg);
    m_includedNPC->setPos(
        (
            m_data.x + ((m_data.w - npcImg.width()) / 2)
        ),
        (
            m_data.y + ((m_data.h - npcImg.height()) / 2)
        ));
    m_includedNPC->setZValue(m_scene->Z_Block + 10);
    m_includedNPC->setOpacity(qreal(0.6));
    m_grp->addToGroup(m_includedNPC);

    if(npcID < 0)
    {
        m_coinCounter = new QGraphicsPixmapItem(GraphicsHelps::drawDegitFont(abs(npcID)));
        m_coinCounter->setPos(x(), y());
        m_coinCounter->setOpacity(1.0);
        m_coinCounter->setZValue(m_scene->Z_Block + 11);
        m_grp->addToGroup(m_coinCounter);
    }

    if(!init) m_data.npc_id = npcID;
    if(!init) arrayApply();
}

void ItemBlock::transformTo(long target_id)
{
    if(target_id < 1) return;
    if(!m_scene->m_localConfigBlocks.contains(target_id))
        return;

    obj_block &mergedSet = m_scene->m_localConfigBlocks[target_id];
    long animator = mergedSet.animator_id;

    m_data.id = target_id;
    setBlockData(m_data, &mergedSet, &animator);
    arrayApply();

    if(!m_scene->m_opts.animationEnabled)
        m_scene->update();
}

///////////////////MainArray functions/////////////////////////////
void ItemBlock::arrayApply()
{
    bool found = false;
    m_data.x = qRound(this->scenePos().x());
    m_data.y = qRound(this->scenePos().y());
    if(this->data(ITEM_BLOCK_IS_SIZABLE).toString() == "sizable")
        this->setZValue(m_scene->Z_blockSizable + sizableBlockZ(m_data));
    if(m_data.meta.index < (unsigned int)m_scene->m_data->blocks.size())
    {
        //Check index
        if(m_data.meta.array_id == m_scene->m_data->blocks[m_data.meta.index].meta.array_id)
            found = true;
    }

    //Apply current data in main array
    if(found)
    {
        //directlry
        m_scene->m_data->blocks[m_data.meta.index] = m_data; //apply current blockdata
    }
    else
        for(int i = 0; i < m_scene->m_data->blocks.size(); i++)
        {
            //after find it into array
            if(m_scene->m_data->blocks[i].meta.array_id == m_data.meta.array_id)
            {
                m_data.meta.index = i;
                m_scene->m_data->blocks[i] = m_data;
                break;
            }
        }

    //Mark level as modified
    m_scene->m_data->meta.modified = true;

    //Update R-tree innex
    m_scene->unregisterElement(this);
    m_scene->registerElement(this);
}

void ItemBlock::removeFromArray()
{
    bool found = false;
    if(m_data.meta.index < (unsigned int)m_scene->m_data->blocks.size())
    {
        //Check index
        if(m_data.meta.array_id == m_scene->m_data->blocks[m_data.meta.index].meta.array_id)
            found = true;
    }
    if(found)
    {
        //directlry
        m_scene->m_data->blocks.removeAt(m_data.meta.index);
    }
    else
        for(int i = 0; i < m_scene->m_data->blocks.size(); i++)
        {
            if(m_scene->m_data->blocks[i].meta.array_id == m_data.meta.array_id)
            {
                m_scene->m_data->blocks.removeAt(i);
                break;
            }
        }

    //Mark level as modified
    m_scene->m_data->meta.modified = true;
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
        const QPixmap &texture = m_scene->m_animatorsBlocks[m_animatorID]->image();
        updateSizableBorder(texture);
        drawSizableBlock(m_data.w, m_data.h, texture);
        m_imageSize = QRectF(0, 0, m_data.w, m_data.h);
    }
}

void ItemBlock::setBlockSize(QRect rect)
{
    if(m_sizable)
    {
        const QPixmap &texture = m_scene->m_animatorsBlocks[m_animatorID]->image();
        m_data.x = rect.x();
        m_data.y = rect.y();
        m_data.w = rect.width();
        m_data.h = rect.height();
        updateSizableBorder(texture);
        drawSizableBlock(m_data.w, m_data.h, texture);
        this->setPos(m_data.x, m_data.y);
    }
    m_imageSize = QRectF(0, 0, m_data.w, m_data.h);
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
        m_sizable = m_localProps.setup.sizable;
        m_gridSize = m_localProps.setup.grid;
        m_gridOffsetX = m_localProps.setup.grid_offset_x;
        m_gridOffsetY = m_localProps.setup.grid_offset_y;

        if(m_localProps.setup.sizable)
        {
            setZValue(m_scene->Z_blockSizable + sizableBlockZ(m_data));  // applay sizable block Z
            setData(ITEM_BLOCK_IS_SIZABLE, "sizable");
        }
        else
        {
            if(m_localProps.setup.z_layer == 1)
                setZValue(m_scene->Z_BlockFore); // applay lava block Z
            else
                setZValue(m_scene->Z_Block); // applay standart block Z

            setData(ITEM_BLOCK_IS_SIZABLE, "standart");
        }
        setData(ITEM_BLOCK_SHAPE, m_localProps.setup.phys_shape);
        setData(ITEM_IS_META, m_localProps.setup.is_meta_object);
    }

    if(animator)
        setAnimator((*animator));

    setPos(m_data.x, m_data.y);

    if(m_data.invisible)
        setOpacity(qreal(0.5));

    setIncludedNPC(m_data.npc_id, true);

    m_imageSize = QRectF(0, 0, m_data.w, m_data.h);

    setData(ITEM_ID, QString::number(m_data.id));
    setData(ITEM_ARRAY_ID, QString::number(m_data.meta.array_id));
    setData(ITEM_WIDTH, QString::number(m_data.w));  //width
    setData(ITEM_HEIGHT, QString::number(m_data.h));  //height

    m_scene->unregisterElement(this);
    m_scene->registerElement(this);
}


QRectF ItemBlock::boundingRect() const
{
    return m_imageSize;
}

void ItemBlock::paint(QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/)
{
    if(m_animatorID < 0)
    {
        painter->setPen(QPen(QBrush(Qt::red), 1, Qt::DotLine));
        painter->drawRect(QRect(0, 0, 1, 1));
        return;
    }

    if(m_scene->m_animatorsBlocks.size() > m_animatorID)
    {
        const QPixmap &image = m_scene->m_animatorsBlocks[m_animatorID]->wholeImage();
        QRect rect = m_scene->m_animatorsBlocks[m_animatorID]->frameRect();
        int frame = m_scene->m_animatorsBlocks[m_animatorID]->frame();
        if(m_sizable)
        {
            if(m_sizablePrevFrame != frame)
            {
                m_sizablePrevFrame = m_scene->m_animatorsBlocks[m_animatorID]->frame();
                drawSizableBlock(m_data.w, m_data.h, m_scene->m_animatorsBlocks[m_animatorID]->image(m_sizablePrevFrame));
            }
            painter->drawPixmap(m_imageSize,
                                m_currentImage,
                                m_imageSize);
        }
        else
            painter->drawPixmap(m_imageSize,
                                image,
                                rect);
    }
    else
        painter->drawRect(QRect(0, 0, 32, 32));

    if(this->isSelected())
    {
        painter->setPen(QPen(QBrush(Qt::black), 2, Qt::SolidLine));
        painter->drawRect(1, 1, m_imageSize.width() - 2, m_imageSize.height() - 2);
        painter->setPen(QPen(QBrush(Qt::green), 2, Qt::DotLine));
        painter->drawRect(1, 1, m_imageSize.width() - 2, m_imageSize.height() - 2);
    }

}

void ItemBlock::setAnimator(long aniID)
{
    if(aniID < m_scene->m_animatorsBlocks.size())
    {
        QRect frameRect = m_scene->m_animatorsBlocks[aniID]->frameRect();
        m_sizablePrevFrame = m_scene->m_animatorsBlocks[aniID]->frame();
        m_imageSize = QRectF(0, 0, frameRect.width(), frameRect.height());
    }
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
    m_includedNPC = nullptr;
    m_coinCounter = nullptr;
}

bool ItemBlock::itemTypeIsLocked()
{
    if(!m_scene)
        return false;
    return m_scene->m_lockBlock;
}

//sizable Block formula
void ItemBlock::drawSizableBlock(int w, int h, const QPixmap &srcimg)
{
    m_currentImage = QPixmap(w, h);

    if((srcimg.width() < 3) || (srcimg.height() < 3))
    {
        // Too small picture for sizable block
        m_currentImage = srcimg.scaled(w, h);
        return;
    }

    //! Width of body
    const int32_t wB = w;
    //! Height of body
    const int32_t hB = h;

    //! Width of texture
    const int32_t wT = srcimg.width();
    //! Height of texture
    const int32_t hT = srcimg.height();

    //! Left border size
    const int32_t xbL = sizable_border.l;
    //! Top border size
    const int32_t ybT = sizable_border.t;
    //! Right border size
    const int32_t xbR = sizable_border.r;
    //! Bottom border size
    const int32_t ybB = sizable_border.b;
    //! Summary of left and right bottom sizes
    const int32_t xB2 = xbL + xbR;
    //! Summary of top and bottom bottom sizes
    const int32_t yB2 = ybT + ybB;
    //! Width of center piece
    const int32_t pWidth = (wT > xB2) ? wT - xB2 : 1;
    //! Height of center piece
    const int32_t pHeight = (hT > yB2) ? hT - yB2 : 1;

    //! Iterator 1
    int32_t i;
    //! Iterator 2
    int32_t j;

    //! Horizontal offset cursor
    int32_t hc;
    //! Vertical offset cursor
    int32_t wc;

    //! Lenght left
    int32_t fLnt = 0;
    //! Width left
    int32_t fWdt = 0;
    //! Draw Offset X. This need for crop junk on small sizes
    int32_t dX = 0;
    //! Draw Offset Y. This need for crop junk on small sizes
    int32_t dY = 0;

    if(wB < xB2)
        dX = (xB2 - wB) >> 1;
    else dX = 0;

    if(hB < yB2)
        dY = (yB2 - hB) >> 1;
    else dY = 0;

    int32_t totalW = ((wB - xB2) / pWidth);
    int32_t totalH = ((hB - yB2) / pHeight);

    m_currentImage.fill(Qt::transparent);
    QPainter szblock(&m_currentImage);

    //L Draw left border
    if(hB > yB2)
    {
        const QPixmap piece = srcimg.copy(0, ybT,      xbL - dX, pHeight);
        hc = 0;

        for(i = 0; i < totalH; i++)
        {
            szblock.drawPixmap(0, ybT + hc, xbL - dX, pHeight, piece);
            hc += pHeight;
        }

        fLnt = (hB - yB2) % pHeight;

        if(fLnt != 0)
            szblock.drawPixmap(0, ybT + hc, xbL - dX, fLnt,
                               srcimg.copy(0, ybT,      xbL - dX, fLnt));
    }

    //T Draw top border
    if(wB > xB2)
    {
        const QPixmap piece = srcimg.copy(xbL,      0, pWidth, ybT - dY);
        hc = 0;

        for(i = 0; i < totalW; i++)
        {
            szblock.drawPixmap(xbL + hc,   0, pWidth, ybT - dY, piece);
            hc += pWidth;
        }

        fLnt = (wB - xB2) % pWidth;

        if(fLnt != 0)
            szblock.drawPixmap(xbL + hc, 0, fLnt, ybT - dY,
                               srcimg.copy(xbL,      0, fLnt, ybT - dY));
    }

    //B Draw bottom border
    if(wB > xB2)
    {
        const QPixmap piece = srcimg.copy(xbL,        hT - ybB + dY, pWidth, ybB - dY);
        hc = 0;

        for(i = 0; i < totalW; i++)
        {
            szblock.drawPixmap(xbL + hc,   hB - ybB + dY, pWidth, ybB - dY, piece);
            hc += pWidth;
        }

        fLnt = (wB - xB2) % pWidth;

        if(fLnt != 0)
            szblock.drawPixmap(xbL + hc,   hB - ybB + dY, fLnt, ybB - dY,
                               srcimg.copy(xbL,        hT - ybB + dY, fLnt, ybB - dY));
    }

    //R Draw right border
    if(hB > yB2)
    {
        const QPixmap piece = srcimg.copy(wT - xbR + dX, ybT, xbR - dX, pHeight);
        hc = 0;

        for(i = 0; i < totalH; i++)
        {
            szblock.drawPixmap(wB - xbR + dX,      ybT + hc, xbR - dX, pHeight, piece);
            hc += pHeight;
        }

        fLnt = (hB - yB2) % pHeight;

        if(fLnt != 0)
            szblock.drawPixmap(wB - xbR + dX,      ybT + hc, xbR - dX, fLnt,
                               srcimg.copy(wT - xbR + dX, ybT, xbR - dX, fLnt));
    }

    //C Draw center
    if((wB > (xB2)) && (hB > (yB2)))
    {
        const QPixmap piece = srcimg.copy(xbL,      ybT,      pWidth, pHeight);
        hc = 0;
        wc = 0;

        for(i = 0; i < totalH; i++)
        {
            hc = 0;

            for(j = 0; j < totalW; j++)
            {
                szblock.drawPixmap(xbL + hc, ybT + wc, pWidth, pHeight, piece);
                hc += pWidth;
            }

            fLnt = (wB - xB2) % pWidth;

            if(fLnt != 0)
                szblock.drawPixmap(xbL + hc, ybT + wc, fLnt, pHeight,
                                   srcimg.copy(xbL,      ybT,      fLnt, pHeight));

            wc += pHeight;
        }

        fWdt = (hB - yB2) % pHeight;

        if(fWdt != 0)
        {
            const QPixmap piece2 = srcimg.copy(xbL,      ybT,      pWidth, fWdt);
            hc = 0;

            for(j = 0; j < totalW; j++)
            {
                szblock.drawPixmap(xbL + hc, ybT + wc, pWidth, fWdt, piece2);
                hc += pWidth;
            }

            fLnt = (wB - xB2) % pWidth;

            if(fLnt != 0)
                szblock.drawPixmap(xbL + hc, ybT + wc, fLnt, fWdt,
                                   srcimg.copy(xbL,      ybT,      fLnt, fWdt));
        }
    }

    //Draw corners
    //1 Left-top
    szblock.drawPixmap(0, 0, xbL - dX, ybT - dY,
                       srcimg.copy(QRect(0, 0, xbL - dX, ybT - dY)));
    //2 Right-top
    szblock.drawPixmap(wB - xbR + dX, 0, xbR - dX, ybT - dY,
                       srcimg.copy(QRect(wT - xbR + dX, 0, xbR - dX, ybT - dY)));
    //3 Right-bottom
    szblock.drawPixmap(wB - xbR + dX, hB - ybB + dY, xbR - dX, ybB - dY,
                       srcimg.copy(QRect(wT - xbR + dX, hT - ybB + dY, xbR - dX, ybB - dY)));
    //4 Left-bottom
    szblock.drawPixmap(0, hB - ybB + dY, xbL - dX, ybB - dY,
                       srcimg.copy(QRect(0, hT - ybB + dY, xbL - dX, ybB - dY)));

    szblock.end();
}
