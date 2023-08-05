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

#include "../lvl_history_manager.h"
#include "../newlayerbox.h"

#include <editing/_components/history/settings/lvl_bgo_userdata.hpp>

static inline double bgoZ(const LevelBGO &b)
{
    return (static_cast<double>(b.meta.array_id) * 0.000000000001);
}

ItemBGO::ItemBGO(QGraphicsItem *parent) : LvlBaseItem(parent)
{
    construct();
}

ItemBGO::ItemBGO(LvlScene *parentScene, QGraphicsItem *parent)
    : LvlBaseItem(parentScene, parent)
{
    construct();
    if(!parentScene) return;
    setScenePoint(parentScene);
    m_scene->addItem(this);
    setLocked(m_scene->m_lockBgo);
}

void ItemBGO::construct()
{
    m_gridSize = 32;
    m_zLayer = LevelBGO::ZDefault;
    setData(ITEM_TYPE, "BGO");
}


ItemBGO::~ItemBGO()
{
    m_scene->unregisterElement(this);
}


void ItemBGO::contextMenu(QGraphicsSceneMouseEvent *mouseEvent)
{
    m_scene->m_contextMenuIsOpened = true;
    //Remove selection from non-bgo items
    if(!this->isSelected())
    {
        m_scene->clearSelection();
        this->setSelected(true);
    }

    this->setSelected(1);
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
        if((layer.name == "Destroyed Blocks") || (layer.name == "Spawned NPCs")) continue;

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

    bool isLvlx = !m_scene->m_data->meta.smbx64strict;

    QAction *ZOffset =          ItemMenu.addAction(tr("Change Z-Offset..."));
    ZOffset->setEnabled(isLvlx);
    QMenu *ZMode =              ItemMenu.addMenu(tr("Z-Layer"));
    ZMode->setEnabled(isLvlx);

    QAction *ZMode_bg2 =        ZMode->addAction(tr("Background-2"));
    ZMode_bg2->setCheckable(true);
    ZMode_bg2->setChecked(m_data.z_mode == LevelBGO::Background2);
    QAction *ZMode_bg1 =        ZMode->addAction(tr("Background"));
    ZMode_bg1->setCheckable(true);
    ZMode_bg1->setChecked(m_data.z_mode == LevelBGO::Background1);
    QAction *ZMode_def =        ZMode->addAction(tr("Default"));
    ZMode_def->setCheckable(true);
    ZMode_def->setChecked(m_data.z_mode == LevelBGO::ZDefault);
    QAction *ZMode_fg1 =        ZMode->addAction(tr("Foreground"));
    ZMode_fg1->setCheckable(true);
    ZMode_fg1->setChecked(m_data.z_mode == LevelBGO::Foreground1);
    QAction *ZMode_fg2 =        ZMode->addAction(tr("Foreground-2"));
    ZMode_fg2->setCheckable(true);
    ZMode_fg2->setChecked(m_data.z_mode == LevelBGO::Foreground2);
    ItemMenu.addSeparator();

    QMenu   *transforms = ItemMenu.addMenu(tr("Transform into"));
    QAction *transform =        transforms->addAction(tr("Transform into"));
    QAction *transform_all_s =  transforms->addAction(tr("Transform all %1 in this section into").arg("BGO-%1").arg(m_data.id));
    QAction *transform_all =    transforms->addAction(tr("Transform all %1 into").arg("BGO-%1").arg(m_data.id));
    ItemMenu.addSeparator();

    /*************Copy Preferences*******************/
    QMenu *copyPreferences =   ItemMenu.addMenu(tr("Copy preferences"));
    QAction *copyArrayID =      copyPreferences->addAction(tr("Array-ID: %1").arg(m_data.meta.array_id));
    QAction *copyItemID =       copyPreferences->addAction(tr("BGO-ID: %1").arg(m_data.id));
    QAction *copyPosXY =        copyPreferences->addAction(tr("Position: X, Y"));
    QAction *copyPosXYWH =      copyPreferences->addAction(tr("Position: X, Y, Width, Height"));
    QAction *copyPosLTRB =      copyPreferences->addAction(tr("Position: Left, Top, Right, Bottom"));
    /*************Copy Preferences*end***************/

    QAction *copyBGO =          ItemMenu.addAction(tr("Copy"));
    QAction *cutBGO =           ItemMenu.addAction(tr("Cut"));
    ItemMenu.addSeparator();

    QAction *remove =           ItemMenu.addAction(tr("Remove"));
    QAction *remove_all_s =     ItemMenu.addAction(tr("Remove all %1 in this section").arg("BGO-%1").arg(m_data.id));
    QAction *remove_all =       ItemMenu.addAction(tr("Remove all %1").arg("BGO-%1").arg(m_data.id));
    ItemMenu.addSeparator();

    QAction *rawUserData =      ItemMenu.addAction(tr("Edit raw user data..."));
    ItemMenu.addSeparator();

    QAction *props =            ItemMenu.addAction(tr("Properties..."));
    props->deleteLater();

    /*****************Waiting for answer************************/
    QAction *selected = ItemMenu.exec(mouseEvent->screenPos());
    /***********************************************************/

    if(!selected)
        return;


    if(selected == cutBGO)
        m_scene->m_mw->on_actionCut_triggered();
    else if(selected == copyBGO)
        m_scene->m_mw->on_actionCopy_triggered();
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
        foreach(QGraphicsItem *SelItem, our_items)
        {
            if(SelItem->data(ITEM_TYPE).toString() == "BGO")
            {
                if(((ItemBGO *) SelItem)->m_data.id == oldID)
                    selectedList.push_back(SelItem);
            }
        }
        if(!selectedList.isEmpty())
        {
            LvlScene    *scene = m_scene;
            scene->removeLvlItems(selectedList);
            scene->Debugger_updateItemList();
        }
cancelRemoveSSS:
        ;
    }
    else if(selected == ZMode_bg2)
    {
        LevelData selData;
        foreach(QGraphicsItem *SelItem, m_scene->selectedItems())
        {
            if(SelItem->data(ITEM_TYPE).toString() == "BGO")
            {
                selData.bgo.push_back(((ItemBGO *)SelItem)->m_data);
                ((ItemBGO *) SelItem)->setZMode(LevelBGO::Background2, ((ItemBGO *) SelItem)->m_data.z_offset);
            }
        }
        m_scene->m_history->addChangeSettings(selData, HistorySettings::SETTING_Z_LAYER, QVariant(LevelBGO::Background2));
    }
    else if(selected == ZMode_bg1)
    {
        LevelData selData;
        foreach(QGraphicsItem *SelItem, m_scene->selectedItems())
        {
            if(SelItem->data(ITEM_TYPE).toString() == "BGO")
            {
                selData.bgo.push_back(((ItemBGO *)SelItem)->m_data);
                ((ItemBGO *) SelItem)->setZMode(LevelBGO::Background1, ((ItemBGO *) SelItem)->m_data.z_offset);
            }
        }
        m_scene->m_history->addChangeSettings(selData, HistorySettings::SETTING_Z_LAYER, QVariant(LevelBGO::Background1));
    }
    else if(selected == ZMode_def)
    {
        LevelData selData;
        foreach(QGraphicsItem *SelItem, m_scene->selectedItems())
        {
            if(SelItem->data(ITEM_TYPE).toString() == "BGO")
            {
                selData.bgo.push_back(((ItemBGO *)SelItem)->m_data);
                ((ItemBGO *) SelItem)->setZMode(LevelBGO::ZDefault, ((ItemBGO *) SelItem)->m_data.z_offset);
            }
        }
        m_scene->m_history->addChangeSettings(selData, HistorySettings::SETTING_Z_LAYER, QVariant(LevelBGO::ZDefault));
    }
    else if(selected == ZMode_fg1)
    {
        LevelData selData;
        foreach(QGraphicsItem *SelItem, m_scene->selectedItems())
        {
            if(SelItem->data(ITEM_TYPE).toString() == "BGO")
            {
                selData.bgo.push_back(((ItemBGO *)SelItem)->m_data);
                ((ItemBGO *) SelItem)->setZMode(LevelBGO::Foreground1, ((ItemBGO *) SelItem)->m_data.z_offset);
            }
        }
        m_scene->m_history->addChangeSettings(selData, HistorySettings::SETTING_Z_LAYER, QVariant(LevelBGO::Foreground1));
    }
    else if(selected == ZMode_fg2)
    {
        LevelData selData;
        foreach(QGraphicsItem *SelItem, m_scene->selectedItems())
        {
            if(SelItem->data(ITEM_TYPE).toString() == "BGO")
            {
                selData.bgo.push_back(((ItemBGO *)SelItem)->m_data);
                ((ItemBGO *) SelItem)->setZMode(LevelBGO::Foreground2, ((ItemBGO *) SelItem)->m_data.z_offset);
            }
        }
        m_scene->m_history->addChangeSettings(selData, HistorySettings::SETTING_Z_LAYER, QVariant(LevelBGO::Foreground2));
    }
    else if(selected == ZOffset)
    {
        bool ok;
        qreal newzOffset = QInputDialog::getDouble(m_scene->m_subWindow, tr("Z-Offset"),
                           tr("Please enter the Z-value offset:"),
                           m_data.z_offset,
                           -500, 500, 7, &ok);
        if(ok)
        {
            LevelData selData;
            foreach(QGraphicsItem *SelItem, m_scene->selectedItems())
            {
                if(SelItem->data(ITEM_TYPE).toString() == "BGO")
                {
                    selData.bgo.push_back(((ItemBGO *)SelItem)->m_data);
                    ((ItemBGO *) SelItem)->setZMode(((ItemBGO *) SelItem)->m_data.z_mode, newzOffset);
                }
            }
            m_scene->m_history->addChangeSettings(selData, HistorySettings::SETTING_Z_OFFSET, QVariant(newzOffset));
        }
    }
    else if((selected == transform) || (selected == transform_all) || (selected == transform_all_s))
    {
        LvlScene    *scene = m_scene;
        LevelData   *ldata = scene->m_data;
        LevelData oldData;
        LevelData newData;

        int transformTO;
        int transformToBlock;
        int tabType;
        ItemSelectDialog *blockList = new ItemSelectDialog(m_scene->m_configs,
                ItemSelectDialog::TAB_BGO | ItemSelectDialog::TAB_BLOCK,
                0, 0, 0, 0, 0, 0, 0, 0, 0, m_scene->m_mw,
                ItemSelectDialog::TAB_BGO);
        blockList->setTabsOrder({ItemSelectDialog::TAB_BGO, ItemSelectDialog::TAB_BLOCK});
        util::DialogToCenter(blockList, true);

        if(blockList->exec() == QDialog::Accepted)
        {
            QList<QGraphicsItem *> our_items;
            bool sameID = false;
            transformTO = blockList->bgoID;
            transformToBlock = blockList->blockID;
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

            //Change ID of each BGO
            if(tabType == ItemSelectDialog::TAB_BGO)
            {
                foreach(QGraphicsItem *SelItem, our_items)
                {
                    if(SelItem->data(ITEM_TYPE).toString() == "BGO")
                    {
                        ItemBGO *item = (ItemBGO *)SelItem;
                        if((!sameID) || (item->m_data.id == oldID))
                        {
                            oldData.bgo.push_back(item->m_data);
                            item->transformTo(transformTO);
                            newData.bgo.push_back(item->m_data);
                        }
                    }
                }
            }
            //Transform every BGO into block
            else if(tabType == ItemSelectDialog::TAB_BLOCK)
            {
                for(QGraphicsItem *SelItem : our_items)
                {
                    if(SelItem->data(ITEM_TYPE).toString() == "BGO")
                    {
                        ItemBGO *item = (ItemBGO *)SelItem;
                        if((!sameID) || (item->m_data.id == oldID))
                        {
                            oldData.bgo.push_back(item->m_data);
                            LevelBlock block;
                            block.id = transformToBlock;
                            block.x = item->m_data.x;
                            block.y = item->m_data.y;
                            block.layer = item->m_data.layer;
                            block.meta = item->m_data.meta;
                            block.meta.array_id = (ldata->blocks_array_id)++;
                            ItemBlock *blockItem = scene->placeBlock(block);
                            block.w = blockItem->m_data.w;
                            block.h = blockItem->m_data.h;
                            ldata->blocks.push_back(block);
                            newData.blocks.push_back(block);
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

        if(!newData.bgo.isEmpty() || !newData.blocks.isEmpty())
            scene->m_history->addTransform(newData, oldData);
    }
    else if(selected == copyArrayID)
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
                if(SelItem->data(ITEM_TYPE).toString() == "BGO")
                {
                    selData.bgo.push_back(((ItemBGO *)SelItem)->m_data);
                    ((ItemBGO *) SelItem)->m_data.meta.custom_params = ch;
                    ((ItemBGO *) SelItem)->arrayApply();
                }
            }
            m_scene->m_history->addChangeSettings(selData, new BgoHistory_UserData(), ch);
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


QString ItemBGO::getLayerName() {
    return m_data.layer;
}

///////////////////MainArray functions/////////////////////////////
void ItemBGO::setLayer(QString layer)
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

void ItemBGO::arrayApply()
{
    bool found = false;
    m_data.x = qRound(this->scenePos().x());
    m_data.y = qRound(this->scenePos().y());
    if(m_data.meta.index < (unsigned int)m_scene->m_data->bgo.size())
    {
        //Check index
        if(m_data.meta.array_id == m_scene->m_data->bgo[m_data.meta.index].meta.array_id)
            found = true;
    }

    //Apply current data in main array
    if(found)
    {
        //directlry
        m_scene->m_data->bgo[m_data.meta.index] = m_data; //apply current bgoData
    }
    else
        for(int i = 0; i < m_scene->m_data->bgo.size(); i++)
        {
            //after find it into array
            if(m_scene->m_data->bgo[i].meta.array_id == m_data.meta.array_id)
            {
                m_data.meta.index = i;
                m_scene->m_data->bgo[i] = m_data;
                break;
            }
        }

    //Mark level as modified
    m_scene->m_data->meta.modified = true;

    //Update R-tree innex
    m_scene->unregisterElement(this);
    m_scene->registerElement(this);
}

void ItemBGO::removeFromArray()
{
    bool found = false;
    if(m_data.meta.index < (unsigned int)m_scene->m_data->bgo.size())
    {
        //Check index
        if(m_data.meta.array_id == m_scene->m_data->bgo[m_data.meta.index].meta.array_id)
            found = true;
    }

    if(found)
    {
        //directlry
        m_scene->m_data->bgo.removeAt(m_data.meta.index);
    }
    else
        for(int i = 0; i < m_scene->m_data->bgo.size(); i++)
        {
            if(m_scene->m_data->bgo[i].meta.array_id == m_data.meta.array_id)
            {
                m_scene->m_data->bgo.removeAt(i);
                break;
            }
        }

    //Mark level as modified
    m_scene->m_data->meta.modified = true;
}

void ItemBGO::returnBack()
{
    setPos(m_data.x, m_data.y);
}

QPoint ItemBGO::sourcePos()
{
    return QPoint(m_data.x, m_data.y);
}

bool ItemBGO::itemTypeIsLocked()
{
    if(!m_scene)
        return false;
    return m_scene->m_lockBgo;
}

void ItemBGO::setBGOData(LevelBGO inD, obj_bgo *mergedSet, long *animator_id)
{
    m_data = inD;

    if(mergedSet)
    {
        m_localProps = (*mergedSet);
        m_zValueOverride = m_localProps.setup.zValueOverride;
        m_zValue         = m_localProps.setup.zValue;
        m_zLayer         = m_localProps.setup.zLayer;
        m_zOffset        = m_localProps.setup.zOffset;
        m_gridSize    = m_localProps.setup.grid;
        m_gridOffsetX = m_localProps.setup.grid_offset_x;
        m_gridOffsetY = m_localProps.setup.grid_offset_y;
        setZMode(m_data.z_mode, m_data.z_offset, true);
        setData(ITEM_IS_META, m_localProps.setup.is_meta_object);
    }

    if(animator_id)
        setAnimator((*animator_id));

    setPos(m_data.x, m_data.y);

    setData(ITEM_ID, QString::number(m_data.id));
    setData(ITEM_ARRAY_ID, QString::number(m_data.meta.array_id));

    m_scene->unregisterElement(this);
    m_scene->registerElement(this);
}

void ItemBGO::setZMode(int mode, qreal offset, bool init)
{
    m_data.z_mode = mode;
    m_data.z_offset = offset;

    qreal targetZ = 0;

    if(m_zValueOverride)
        targetZ = m_zValue;
    else
    {
        switch(m_zLayer)
        {
        case -1:
            targetZ = m_scene->Z_BGOBack2;
            break;
        case 1:
            targetZ = m_scene->Z_BGOFore1;
            break;
        case 2:
            targetZ = m_scene->Z_BGOFore2;
            break;
        case 0:
        default:
            targetZ = m_scene->Z_BGOBack1;
        }
        targetZ += m_zOffset + m_data.z_offset;
    }

    switch(m_data.z_mode)
    {
    case LevelBGO::Background2:
        targetZ = m_scene->Z_BGOBack2 + m_zOffset + m_data.z_offset;
        break;
    case LevelBGO::Background1:
        targetZ = m_scene->Z_BGOBack1 + m_zOffset + m_data.z_offset;
        break;
    case LevelBGO::Foreground1:
        targetZ = m_scene->Z_BGOFore1 + m_zOffset + m_data.z_offset;
        break;
    case LevelBGO::Foreground2:
        targetZ = m_scene->Z_BGOFore2 + m_zOffset + m_data.z_offset;
        break;
    default:
        break;
    }

    setZValue(targetZ + bgoZ(m_data));

    if(!init) arrayApply();
}

void ItemBGO::transformTo(long target_id)
{
    if(target_id < 1) return;

    if(!m_scene->m_localConfigBGOs.contains(target_id))
        return;//Don't transform, target item is not found

    obj_bgo &mergedSet = m_scene->m_localConfigBGOs[target_id];
    long animator = mergedSet.animator_id;

    m_data.id = target_id;
    setBGOData(m_data, &mergedSet, &animator);
    arrayApply();

    if(!m_scene->m_opts.animationEnabled)
        m_scene->update();
}

QRectF ItemBGO::boundingRect() const
{
    return m_imageSize;
}

void ItemBGO::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if(m_animatorID < 0)
    {
        painter->drawRect(QRect(0, 0, 1, 1));
        return;
    }
    if(m_scene->m_animatorsBGO.size() > m_animatorID)
        painter->drawPixmap(m_imageSize,
                            m_scene->m_animatorsBGO[m_animatorID]->wholeImage(),
                            m_scene->m_animatorsBGO[m_animatorID]->frameRect());
    else
        painter->drawRect(QRect(0, 0, 32, 32));

    if(this->isSelected())
    {
        painter->setPen(QPen(QBrush(Qt::black), 2, Qt::SolidLine));
        painter->drawRect(1, 1, m_imageSize.width() - 2, m_imageSize.height() - 2);
        painter->setPen(QPen(QBrush(Qt::red), 2, Qt::DotLine));
        painter->drawRect(1, 1, m_imageSize.width() - 2, m_imageSize.height() - 2);
    }
}

////////////////Animation///////////////////

void ItemBGO::setAnimator(long aniID)
{
    if(aniID < m_scene->m_animatorsBGO.size())
    {
        QRect frameRect = m_scene->m_animatorsBGO[aniID]->frameRect();
        m_imageSize = QRectF(0, 0, frameRect.width(), frameRect.height());
    }

    this->setData(ITEM_WIDTH,  QString::number(qRound(m_imageSize.width())));  //width
    this->setData(ITEM_HEIGHT, QString::number(qRound(m_imageSize.height())));  //height

    m_animatorID = aniID;
}
