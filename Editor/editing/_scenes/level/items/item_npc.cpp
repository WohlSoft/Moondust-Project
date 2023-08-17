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
#include <qglobal.h>
#include <pge_qt_compat.h>

static int randomDirection()
{
    int r = Q_RANDNEW() % 2;
    return ((r == 0) ? -1 : 1);
}

#include <mainwindow.h>
#include <common_features/logger.h>
#include <editing/_dialogs/itemselectdialog.h>
#include <editing/_dialogs/user_data_edit.h>
#include <PGE_File_Formats/file_formats.h>
#include <common_features/util.h>

#include "../lvl_history_manager.h"
#include "../number_limiter.h"
#include "../itemmsgbox.h"
#include "../newlayerbox.h"

#include <editing/_components/history/settings/lvl_npc_userdata.hpp>


ItemNPC::ItemNPC(bool noScene, QGraphicsItem *parent)
    : LvlBaseItem(parent)
{
    m_DisableScene = noScene;
    construct();
}

ItemNPC::ItemNPC(LvlScene *parentScene, QGraphicsItem *parent)
    : LvlBaseItem(parentScene, parent)
{
    m_DisableScene = false;
    construct();
    if(!parentScene) return;
    setScenePoint(parentScene);
    m_scene->addItem(this);
    setLocked(m_scene->m_lockNpc);
}

void ItemNPC::construct()
{
    m_offset_x = 0;
    m_offset_y = 0;

    m_generatorArrow = nullptr;
    m_randomDirection = nullptr;
    m_includedNPC = nullptr;
    m_animated = false;
    m_direction = -1;
    m_gridSize = 1;

    m_extAnimator = false;
    m_animatorID = -1;
    m_imageSize = QRectF(0, 0, 10, 10);

    m_imgOffsetX = 0;
    m_imgOffsetY = 0;

    _internal_animator = nullptr;

    setData(ITEM_TYPE, "NPC");
}


ItemNPC::~ItemNPC()
{
    if(m_includedNPC != nullptr) delete m_includedNPC;
    if(m_randomDirection != nullptr) delete m_randomDirection;
    if(m_generatorArrow != nullptr) delete m_generatorArrow;
    if(m_grp != nullptr) delete m_grp;
    if(!m_DisableScene) m_scene->unregisterElement(this);
    if(_internal_animator) delete _internal_animator;
}

void ItemNPC::contextMenu(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(m_DisableScene) return;

    m_scene->m_contextMenuIsOpened = true; //bug protector

    //Remove selection from non-block items
    if(!this->isSelected())
    {
        m_scene->clearSelection();
        this->setSelected(true);
    }

    this->setSelected(true);
    QMenu ItemMenu;

    /*************Layers*******************/
    QMenu *layerName =         ItemMenu.addMenu(tr("Layer: ") + QString("[%1]").arg(m_data.layer).replace("&", "&&"));
    QAction *setLayer;
    QList<QAction *> layerItems;
    QAction *newLayer =        layerName->addAction(tr("Add to new layer..."));
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

    QString NPCpath1 = m_scene->m_data->meta.path + QString("/npc-%1.txt").arg(m_data.id);
    QString NPCpath2 = m_scene->m_data->meta.path + "/" + m_scene->m_data->meta.filename + QString("/npc-%1.txt").arg(m_data.id);

    QAction *newNpc;
    if((!m_scene->m_data->meta.untitled) && ((QFile().exists(NPCpath2)) || (QFile().exists(NPCpath1))))
        newNpc =            ItemMenu.addAction(tr("Edit NPC-Configuration"));
    else
        newNpc =            ItemMenu.addAction(tr("New NPC-Configuration"));
    newNpc->setEnabled(!m_scene->m_data->meta.untitled);
    ItemMenu.addSeparator();

    /*************Direction*******************/
    QMenu *chDir =             ItemMenu.addMenu(tr("Set %1").arg((!m_localProps.setup.direct_alt_title.isEmpty()) ? m_localProps.setup.direct_alt_title : tr("Direction")));
    QAction *setLeft =          chDir->addAction((!m_localProps.setup.direct_alt_left.isEmpty()) ? m_localProps.setup.direct_alt_left : tr("Left"));
    setLeft->setCheckable(true);
    setLeft->setChecked(m_data.direct == -1);

    QAction *setRand =          chDir->addAction((!m_localProps.setup.direct_alt_rand.isEmpty()) ? m_localProps.setup.direct_alt_rand : tr("Random"));
    setRand->setVisible(!m_localProps.setup.direct_disable_random);
    setRand->setCheckable(true);
    setRand->setChecked(m_data.direct == 0);

    QAction *setRight =         chDir->addAction((!m_localProps.setup.direct_alt_right.isEmpty()) ? m_localProps.setup.direct_alt_right : tr("Right"));
    setRight->setCheckable(true);
    setRight->setChecked(m_data.direct == 1);
    ItemMenu.addSeparator();
    /*************Direction**end**************/

    QAction *fri =              ItemMenu.addAction(tr("Friendly"));
    fri->setCheckable(true);
    fri->setChecked(m_data.friendly);

    QAction *stat =             ItemMenu.addAction(tr("Doesn't move"));
    stat->setCheckable(true);
    stat->setChecked(m_data.nomove);

    QAction *msg =              ItemMenu.addAction(tr("Set message..."));
    ItemMenu.addSeparator();

    QAction *boss =             ItemMenu.addAction(tr("Set as Boss"));
    boss->setCheckable(true);
    boss->setChecked(m_data.is_boss);
    ItemMenu.addSeparator();

    QAction *transform =        ItemMenu.addAction(tr("Transform into"));
    QAction *transform_all_s =  ItemMenu.addAction(tr("Transform all %1 in this section into").arg("NPC-%1").arg(m_data.id));
    QAction *transform_all =    ItemMenu.addAction(tr("Transform all %1 into").arg("NPC-%1").arg(m_data.id));
    ItemMenu.addSeparator();

    QAction *chNPC = nullptr;
    if(m_localProps.setup.container)
    {
        chNPC =                 ItemMenu.addAction(tr("Change included NPC..."));
        ItemMenu.addSeparator();
    }

    /*************Copy Preferences*******************/
    QMenu *copyPreferences =   ItemMenu.addMenu(tr("Copy preferences"));
    QAction *copyArrayID =      copyPreferences->addAction(tr("Array-ID: %1").arg(m_data.meta.array_id));
    QAction *copyItemID =       copyPreferences->addAction(tr("NPC-ID: %1").arg(m_data.id));
    QAction *copyPosXY =        copyPreferences->addAction(tr("Position: X, Y"));
    QAction *copyPosXYWH =      copyPreferences->addAction(tr("Position: X, Y, Width, Height"));
    QAction *copyPosLTRB =      copyPreferences->addAction(tr("Position: Left, Top, Right, Bottom"));
    /*************Copy Preferences*end***************/

    QAction *copyNpc =          ItemMenu.addAction(tr("Copy"));
    QAction *cutNpc =           ItemMenu.addAction(tr("Cut"));
    ItemMenu.addSeparator();
    QAction *remove =           ItemMenu.addAction(tr("Remove"));
    QAction *remove_all_s =     ItemMenu.addAction(tr("Remove all %1 in this section").arg("NPC-%1").arg(m_data.id));
    QAction *remove_all =       ItemMenu.addAction(tr("Remove all %1").arg("NPC-%1").arg(m_data.id));

    QAction *rawUserData =      ItemMenu.addAction(tr("Edit raw user data..."));
    ItemMenu.addSeparator();

    ItemMenu.addSeparator();
    QAction *props =            ItemMenu.addAction(tr("Properties..."));

    /*****************Waiting for answer************************/
    QAction *selected = ItemMenu.exec(mouseEvent->screenPos());
    /***********************************************************/

    if(!selected)
        return;


    if(selected == cutNpc)
        m_scene->m_mw->on_actionCut_triggered();
    else if(selected == copyNpc)
        m_scene->m_mw->on_actionCopy_triggered();
    else if((selected == transform) || (selected == transform_all) || (selected == transform_all_s))
    {
        LevelData oldData;
        LevelData newData;

        int transformTO;
        ItemSelectDialog *npcList = new ItemSelectDialog(m_scene->m_configs, ItemSelectDialog::TAB_NPC, 0, 0, 0, 0, 0, 0, 0, 0, 0, m_scene->m_subWindow, ItemSelectDialog::TAB_NPC);
        util::DialogToCenter(npcList, true);

        if(npcList->exec() == QDialog::Accepted)
        {
            QList<QGraphicsItem *> our_items;
            bool sameID = false;
            transformTO = npcList->npcID;
            unsigned long oldID = m_data.id;

            if(selected == transform)
                our_items = m_scene->selectedItems();
            else if(selected == transform_all)
            {
                our_items = m_scene->items();
                sameID = true;
            }
            else if(selected == transform_all_s)
            {
                bool ok = false;
                long mg = QInputDialog::getInt(m_scene->m_subWindow, tr("Margin of section"),
                                               tr("Please select how far items can travel beyond the section boundaries (in pixels) before they are removed."),
                                               32, 0, 214948, 1, &ok);
                if(!ok) goto cancelTransform;
                LevelSection &s = m_scene->m_data->sections[m_scene->m_data->CurSection];
                QRectF section;
                section.setLeft(s.size_left - mg);
                section.setTop(s.size_top - mg);
                section.setRight(s.size_right + mg);
                section.setBottom(s.size_bottom + mg);
                our_items = m_scene->items(section, Qt::IntersectsItemShape);
                sameID = true;
            }

            for(QGraphicsItem *SelItem : our_items)
            {
                if(SelItem->data(ITEM_TYPE).toString() == "NPC")
                {
                    ItemNPC *sItem = (ItemNPC *)SelItem;
                    if((!sameID) || (sItem->m_data.id == oldID))
                    {
                        oldData.npc.push_back(sItem->m_data);
                        sItem->transformTo(transformTO);
                        newData.npc.push_back(sItem->m_data);
                    }
                }
            }
cancelTransform:
            ;
        }
        delete npcList;
        if(!newData.npc.isEmpty())
            m_scene->m_history->addTransform(newData, oldData);
    }
    else if(selected == chNPC)
    {
        LevelData selData;
        ItemSelectDialog *npcList = new ItemSelectDialog(m_scene->m_configs, ItemSelectDialog::TAB_NPC, 0, 0, 0,
                m_data.contents,
                0, 0, 0, 0, 0, m_scene->m_subWindow);
        npcList->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
        npcList->setGeometry(util::alignToScreenCenter(npcList->size()));
        if(npcList->exec() == QDialog::Accepted)
        {
            //apply to all selected items.
            int selected_npc = 0;
            if(npcList->npcID != 0)
                selected_npc = npcList->npcID;

            for(QGraphicsItem *SelItem : m_scene->selectedItems())
            {
                if(SelItem->data(ITEM_TYPE).toString() == "NPC")
                {
                    selData.npc.push_back(((ItemNPC *) SelItem)->m_data);
                    ((ItemNPC *) SelItem)->setIncludedNPC(selected_npc);
                }
            }
            m_scene->m_history->addChangeSettings(selData, HistorySettings::SETTING_CHANGENPC, QVariant(selected_npc));
        }
        delete npcList;
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
            .arg(m_localProps.setup.width)
            .arg(m_localProps.setup.height)
        );
        m_scene->m_mw->showStatusMsg(tr("Preferences have been copied: %1").arg(QApplication::clipboard()->text()));
    }
    else if(selected == copyPosLTRB)
    {
        QApplication::clipboard()->setText(
            QString("Left=%1; Top=%2; Right=%3; Bottom=%4;")
            .arg(m_data.x)
            .arg(m_data.y)
            .arg(m_data.x + m_localProps.setup.width)
            .arg(m_data.y + m_localProps.setup.height)
        );
        m_scene->m_mw->showStatusMsg(tr("Preferences have been copied: %1").arg(QApplication::clipboard()->text()));
    }
    else if(selected == newNpc)
    {
        LogDebug(QString("NPC.txt path 1: %1").arg(NPCpath1));
        LogDebug(QString("NPC.txt path 2: %1").arg(NPCpath2));
        if((!m_scene->m_data->meta.untitled) && (QFileInfo(NPCpath2).exists()))
            m_scene->m_mw->OpenFile(NPCpath2);
        else if((!m_scene->m_data->meta.untitled) && (QFileInfo(NPCpath1).exists()))
            m_scene->m_mw->OpenFile(NPCpath1);
        else
        {
            NpcEdit *child = m_scene->m_mw->createNPCChild();
            child->newFile(m_data.id,
                           m_scene->m_data->meta.path + "/" + m_scene->m_data->meta.filename);
            child->show();
        }
    }
    else if(selected == fri)
    {
        //apply to all selected items.
        LevelData selData;
        for(QGraphicsItem *SelItem : m_scene->selectedItems())
        {
            if(SelItem->data(ITEM_TYPE).toString() == "NPC")
            {
                selData.npc.push_back(((ItemNPC *) SelItem)->m_data);
                ((ItemNPC *) SelItem)->setFriendly(fri->isChecked());
            }
        }
        m_scene->m_history->addChangeSettings(selData, HistorySettings::SETTING_FRIENDLY, QVariant(fri->isChecked()));
    }
    else if(selected == stat)
    {
        //apply to all selected items.
        LevelData selData;
        for(QGraphicsItem *SelItem : m_scene->selectedItems())
        {
            if(SelItem->data(ITEM_TYPE).toString() == "NPC")
            {
                selData.npc.push_back(((ItemNPC *) SelItem)->m_data);
                ((ItemNPC *) SelItem)->setNoMovable(stat->isChecked());
            }
        }
        m_scene->m_history->addChangeSettings(selData, HistorySettings::SETTING_NOMOVEABLE, QVariant(stat->isChecked()));
    }
    else if(selected == msg)
    {
        LevelData selData;

        ItemMsgBox msgBox(Opened_By::NPC, m_data.msg, m_data.friendly, "", "", m_scene->m_mw);
        util::DialogToCenter(&msgBox, true);
        if(msgBox.exec() == QDialog::Accepted)
        {
            //apply to all selected items.
            for(QGraphicsItem *SelItem : m_scene->selectedItems())
            {
                if(SelItem->data(ITEM_TYPE).toString() == "NPC")
                {
                    selData.npc.push_back(((ItemNPC *) SelItem)->m_data);
                    ((ItemNPC *) SelItem)->setMsg(msgBox.currentText);
                    ((ItemNPC *) SelItem)->setFriendly(msgBox.isFriendlyChecked());
                }
            }
            m_scene->m_history->addChangeSettings(selData, HistorySettings::SETTING_MESSAGE, QVariant(msgBox.currentText));
            m_scene->m_history->addChangeSettings(selData, HistorySettings::SETTING_FRIENDLY, QVariant(msgBox.isFriendlyChecked()));
        }
    }
    else if(selected == boss)
    {
        //apply to all selected items.
        LevelData selData;
        for(QGraphicsItem *SelItem : m_scene->selectedItems())
        {
            if(SelItem->data(ITEM_TYPE).toString() == "NPC")
            {
                selData.npc.push_back(((ItemNPC *) SelItem)->m_data);
                ((ItemNPC *) SelItem)->setLegacyBoss(boss->isChecked());
            }
        }
        m_scene->m_history->addChangeSettings(selData, HistorySettings::SETTING_BOSS, QVariant(boss->isChecked()));
    }
    else if(selected == setLeft)
    {
        LevelData selData;
        for(QGraphicsItem *SelItem : m_scene->selectedItems())
        {
            if(SelItem->data(ITEM_TYPE).toString() == "NPC")
            {
                selData.npc.push_back(((ItemNPC *) SelItem)->m_data);
                ((ItemNPC *) SelItem)->changeDirection(-1);
            }
        }
        m_scene->m_history->addChangeSettings(selData, HistorySettings::SETTING_DIRECTION, QVariant(-1));
    }
    if(selected == setRand)
    {
        LevelData selData;
        for(QGraphicsItem *SelItem : m_scene->selectedItems())
        {
            if(SelItem->data(ITEM_TYPE).toString() == "NPC")
            {
                selData.npc.push_back(((ItemNPC *) SelItem)->m_data);
                ((ItemNPC *) SelItem)->changeDirection(0);
            }
        }
        m_scene->m_history->addChangeSettings(selData, HistorySettings::SETTING_DIRECTION, QVariant(0));
    }
    if(selected == setRight)
    {
        LevelData selData;
        for(QGraphicsItem *SelItem : m_scene->selectedItems())
        {
            if(SelItem->data(ITEM_TYPE).toString() == "NPC")
            {
                selData.npc.push_back(((ItemNPC *) SelItem)->m_data);
                ((ItemNPC *) SelItem)->changeDirection(1);
            }
        }
        m_scene->m_history->addChangeSettings(selData, HistorySettings::SETTING_DIRECTION, QVariant(1));
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
            if(SelItem->data(ITEM_TYPE).toString() == "NPC")
            {
                if(((ItemNPC *) SelItem)->m_data.id == oldID)
                    selectedList.push_back(SelItem);
            }
        }
        if(!selectedList.isEmpty())
        {
            m_scene->removeLvlItems(selectedList);
            m_scene->Debugger_updateItemList();
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
                if(SelItem->data(ITEM_TYPE).toString() == "NPC")
                {
                    selData.npc.push_back(((ItemNPC *)SelItem)->m_data);
                    ((ItemNPC *) SelItem)->m_data.meta.custom_params = ch;
                    ((ItemNPC *) SelItem)->arrayApply();
                }
            }
            m_scene->m_history->addChangeSettings(selData, new NPCHistory_UserData(), ch);
        }
    }
    else if(selected == props)
        m_scene->openProps();
    else if(selected == newLayer)
        m_scene->setLayerToSelected();
    else
    {
        //Fetch layers menu
        for(QAction *lItem : layerItems)
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

void ItemNPC::setMetaSignsVisibility(bool visible)
{
    if(m_includedNPC && !m_localProps.setup.container_show_contents)
        m_includedNPC->setVisible(visible);

    if(m_randomDirection) // Sign of random
        m_randomDirection->setVisible(visible);

    if(m_data.generator) //Generator NPCs are meta-signs by theme selves
        setVisible(visible);
}

//Change arrtibutes
void ItemNPC::setFriendly(bool fri)
{
    m_data.friendly = fri;
    arrayApply(); //Apply changes into array
}

void ItemNPC::setMsg(QString message)
{
    m_data.msg = message;
    arrayApply();//Apply changes into array
}

void ItemNPC::setNoMovable(bool stat)
{
    m_data.nomove = stat;
    arrayApply();//Apply changes into array
}

void ItemNPC::setLegacyBoss(bool boss)
{
    m_data.is_boss = boss;
    arrayApply();//Apply changes into array
}

void ItemNPC::changeDirection(int dir)
{
    m_data.direct = dir;

    if(m_randomDirection != nullptr)
    {
        m_grp->removeFromGroup(m_randomDirection);
        m_scene->removeItem(m_randomDirection);
        delete m_randomDirection;
        m_randomDirection = nullptr;
    }

    if(dir == 0) //if direction=random
    {
        dir = randomDirection(); //set randomly 1 or -1
        m_randomDirection = new QGraphicsPixmapItem;
        m_randomDirection->setPixmap(QPixmap(":/npc/random_direction.png"));
        m_scene->addItem(m_randomDirection);
        m_randomDirection->setOpacity(qreal(0.9));
        m_randomDirection->setPos(
            this->scenePos().x() + ((qreal(m_localProps.setup.width) - 40.0) / 2.0),
            this->scenePos().y() - 32.0
        );
        m_grp->addToGroup(m_randomDirection);
    }

    m_direction = dir;
    refreshOffsets();
    update();

    arrayApply();
}

void ItemNPC::transformTo(long target_id)
{
    if(!m_scene)
        return;

    if(target_id < 1) return;
    if((!m_scene->m_localConfigNPCs.contains(target_id))) return;

    obj_npc &mergedSet = m_scene->m_localConfigNPCs[target_id];
    long animator = mergedSet.animator_id;

    m_data.id = target_id;

    setNpcData(m_data, &mergedSet, &animator, true);
    arrayApply();

    if(!m_scene->m_opts.animationEnabled)
        m_scene->update();
}

void ItemNPC::setIncludedNPC(int npcID, bool init)
{
    if(m_DisableScene)
        return;

    if(m_includedNPC != nullptr)
    {
        m_grp->removeFromGroup(m_includedNPC);
        m_scene->removeItem(m_includedNPC);
        delete m_includedNPC;
        m_includedNPC = nullptr;
    }
    if(npcID == 0 || !m_localProps.setup.container)
    {
        if(!init && (m_data.contents != 0))
        {
            m_data.contents = 0;
            arrayApply();
        }
        return;
    }

    QPixmap npcImg = QPixmap(m_scene->getNPCimg(npcID));
    m_includedNPC = m_scene->addPixmap(npcImg);

    double containerAlignXTo = scenePos().x() +
                               qreal((qreal(m_localProps.setup.width) - qreal(npcImg.width())) / 2.0);

    double containerAlignYTo = 0.0;
    switch(m_localProps.setup.container_align_contents)
    {
    default:
    case 0:
        containerAlignYTo = scenePos().y() +
                            qreal((qreal(m_localProps.setup.height) - qreal(npcImg.height()))
                                  / 2.0);
        break;
    case 1:
        containerAlignYTo = scenePos().y();
        break;
    case 2:
        containerAlignYTo = scenePos().y() + qreal(m_localProps.setup.height);
        break;
    }

    //Default included NPC pos
    m_includedNPC->setPos(containerAlignXTo, containerAlignYTo);

    m_includedNPC->setOpacity(m_localProps.setup.container_show_contents ? qreal(1.0) : qreal(0.4));
    m_includedNPC->setZValue(this->zValue() + m_localProps.setup.container_content_z_offset);
    m_grp->addToGroup(m_includedNPC);

    if(!init) m_data.contents = npcID;
    if(!init) arrayApply();
}

void ItemNPC::setGenerator(bool enable, int direction, int type, bool init)
{
    if(m_DisableScene)
        return;

    if(!init) m_data.generator = enable;

    if(m_generatorArrow != nullptr)
    {
        m_grp->removeFromGroup(m_generatorArrow);
        m_scene->removeItem(m_generatorArrow);
        delete m_generatorArrow;
        m_generatorArrow = nullptr;
    }

    if(!enable)
    {
        if(!init) arrayApply();
        m_gridSize = m_localProps.setup.grid;
        return;
    }
    else
    {
        m_generatorArrow = new QGraphicsPixmapItem;
        switch(type)
        {
        case 2:
            m_generatorArrow->setPixmap(QPixmap(":/npc/proj.png"));
            break;
        case 1:
        default:
            m_generatorArrow->setPixmap(QPixmap(":/npc/warp.png"));
            break;
        }
        if(!init) m_data.generator_type = type;

        m_scene->addItem(m_generatorArrow);

        m_gridSize = 16;

        m_generatorArrow->setOpacity(qreal(0.6));

        QPointF offset = QPoint(0, 0);

        switch(direction)
        {
        case LevelNPC::NPC_GEN_LEFT:
            m_generatorArrow->setRotation(270);
            offset.setY(32);
            if(!init) m_data.generator_direct = LevelNPC::NPC_GEN_LEFT;
            break;
        case LevelNPC::NPC_GEN_DOWN:
            m_generatorArrow->setRotation(180);
            offset.setX(32);
            offset.setY(32);
            if(!init) m_data.generator_direct = LevelNPC::NPC_GEN_DOWN;
            break;
        case LevelNPC::NPC_GEN_RIGHT:
            m_generatorArrow->setRotation(90);
            offset.setX(32);
            if(!init) m_data.generator_direct = LevelNPC::NPC_GEN_RIGHT;
            break;
        case LevelNPC::NPC_GEN_UP:
        default:
            m_generatorArrow->setRotation(0);
            if(!init) m_data.generator_direct = LevelNPC::NPC_GEN_UP;
            break;
        }

        m_generatorArrow->setZValue(this->zValue() + 0.0000015);

        //Default Generator arrow NPC pos
        m_generatorArrow->setPos(
            offset.x() + this->scenePos().x() + ((qreal(m_localProps.setup.width) - 32.0) / 2.0),
            offset.y() + this->scenePos().y() + ((qreal(m_localProps.setup.height) - 32.0) / 2.0)
        );

        m_grp->addToGroup(m_generatorArrow);

        if(!init) arrayApply();
    }
}

QString ItemNPC::getLayerName()
{
    return m_data.layer;
}

void ItemNPC::setLayer(QString layer)
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

///////////////////MainArray functions/////////////////////////////
void ItemNPC::arrayApply()
{
    if(m_DisableScene)
        return;

    bool found = false;

    m_data.x = qRound(this->scenePos().x());
    m_data.y = qRound(this->scenePos().y());

    if(m_data.meta.index < (unsigned int)m_scene->m_data->npc.size())
    {
        //Check index
        if(m_data.meta.array_id == m_scene->m_data->npc[m_data.meta.index].meta.array_id)
            found = true;
    }

    //Apply current data in main array
    if(found)
    {
        //directlry
        m_scene->m_data->npc[m_data.meta.index] = m_data; //apply current npcData
    }
    else
    {
        for(int i = 0; i < m_scene->m_data->npc.size(); i++)
        {
            //after find it into array
            if(m_scene->m_data->npc[i].meta.array_id == m_data.meta.array_id)
            {
                m_data.meta.index = i;
                m_scene->m_data->npc[i] = m_data;
                break;
            }
        }
    }

    //Mark level as modified
    m_scene->m_data->meta.modified = true;

    //Update R-tree innex
    m_scene->unregisterElement(this);
    m_scene->registerElement(this);
}


void ItemNPC::removeFromArray()
{
    if(m_DisableScene)
        return;

    bool found = false;
    if(m_data.meta.index < (unsigned int)m_scene->m_data->npc.size())
    {
        //Check index
        if(m_data.meta.array_id == m_scene->m_data->npc[m_data.meta.index].meta.array_id)
            found = true;
    }

    if(found) //directlry
        m_scene->m_data->npc.removeAt(m_data.meta.index);
    else
    {
        // Find in the list
        for(int i = 0; i < m_scene->m_data->npc.size(); i++)
        {
            if(m_scene->m_data->npc[i].meta.array_id == m_data.meta.array_id)
            {
                m_scene->m_data->npc.removeAt(i);
                break;
            }
        }
    }

    //Mark level as modified
    m_scene->m_data->meta.modified = true;
}


void ItemNPC::returnBack()
{
    this->setPos(m_data.x, m_data.y);
}

QPoint ItemNPC::sourcePos()
{
    return QPoint(m_data.x, m_data.y);
}


void ItemNPC::setMainPixmap(const QPixmap &pixmap)
{
    if(_internal_animator)
        _internal_animator->buildAnimator((QPixmap &)pixmap, m_localProps);
    m_imageSize = pixmap.rect();
    refreshOffsets();
}

void ItemNPC::setNpcData(LevelNPC inD, obj_npc *mergedSet, long *animator_id, bool isTransform)
{
    m_data = inD;

    if(m_DisableScene)
    {
        if(mergedSet)
            m_localProps = (*mergedSet);
        if(m_localProps.cur_image)
        {
            if(!_internal_animator)
                _internal_animator = new AdvNpcAnimator(*m_localProps.cur_image, m_localProps);
            else
                _internal_animator->buildAnimator(*m_localProps.cur_image, m_localProps);
            _internal_animator->start();
            QRect frameRect = _internal_animator->frameRect(-1);
            m_imageSize = QRectF(0, 0, frameRect.width(), frameRect.height());
            m_animated = true;
            m_extAnimator = false;
        }
        changeDirection(m_data.direct);
        return;
    }

    if(!m_scene) return;

    if(mergedSet)
    {
        obj_npc oldProps = m_localProps;
        m_localProps = (*mergedSet);
        if(m_localProps.setup.foreground)
            setZValue(m_scene->Z_npcFore);
        else if(m_localProps.setup.background)
            setZValue(m_scene->Z_npcBack);
        else
            setZValue(m_scene->Z_npcStd);

        if(isTransform) // Do work only if it's a transforming
        {
            // Zero containers are not used (for LVLX)
            if(!m_localProps.setup.container)
                m_data.contents = 0;

            // Zero custom values are not used (for LVLX)
            if(!m_localProps.setup.special_option)
                m_data.special_data = 0;
            else
            {
                // When settings of special value aren't matching
                if(oldProps.isValid &&
                   (m_localProps.setup.special_name != oldProps.setup.special_name ||
                    m_localProps.setup.special_type != oldProps.setup.special_type ||
                    m_localProps.setup.special_combobox_opts != oldProps.setup.special_combobox_opts ||
                    m_localProps.setup.special_spin_min != oldProps.setup.special_spin_min ||
                    m_localProps.setup.special_spin_max != oldProps.setup.special_spin_max ||
                    m_localProps.setup.special_spin_value_offset != oldProps.setup.special_spin_value_offset)
                  )
                {
                    if(m_localProps.setup.default_special) // When default value is set
                        m_data.special_data = m_localProps.setup.default_special_value; //Put it!
                    else // Or calculate it automatically
                    {
                        if(m_localProps.setup.special_type == 0 || m_localProps.setup.special_type == 2)
                            m_data.special_data = 0;//Combobox and NPC-id
                        else if(m_localProps.setup.special_type == 1)//Spinbox with min-max
                        {
                            m_data.special_data = 0;
                            NumberLimiter::apply(m_data.special_data,
                                                 static_cast<long>(m_localProps.setup.special_spin_min),
                                                 static_cast<long>(m_localProps.setup.special_spin_max));
                        }
                    }
                }
                else
                {
                    // Be sure input data are valid
                    if(m_localProps.setup.special_type == 0)
                    {
                        const long rangeMin = 0;
                        const long rangeMax = static_cast<long>(m_localProps.setup.special_combobox_opts.size() - 1);
                        if((m_data.special_data < rangeMin) || (m_data.special_data > rangeMax))
                        {
                            if(m_localProps.setup.default_special) // When default value is set
                                m_data.special_data = m_localProps.setup.default_special_value; //Put it!
                            else
                                NumberLimiter::apply(m_data.special_data, rangeMin, rangeMax);
                        }
                    }
                    else if(m_localProps.setup.special_type == 1)
                    {
                        const long rangeMin = static_cast<long>(m_localProps.setup.special_spin_min);
                        const long rangeMax = static_cast<long>(m_localProps.setup.special_spin_max);
                        if((m_data.special_data < rangeMin) || (m_data.special_data > rangeMax))
                        {
                            if(m_localProps.setup.default_special) // When default value is set
                                m_data.special_data = m_localProps.setup.default_special_value; //Put it!
                            else
                            {
                                m_data.special_data = 0;
                                NumberLimiter::apply(m_data.special_data,
                                                     static_cast<long>(m_localProps.setup.special_spin_min),
                                                     static_cast<long>(m_localProps.setup.special_spin_max));
                            }
                        }
                    }
                }
            }
        }

        if((m_localProps.setup.container) && (m_data.contents > 0))
            setIncludedNPC(int(m_data.contents), true);

        m_data.is_star = m_localProps.setup.is_star;

        m_gridOffsetX = m_localProps.setup.grid_offset_x;
        m_gridOffsetY = m_localProps.setup.grid_offset_y;
        m_gridSize =  int(m_localProps.setup.grid);
    }

    if(animator_id)
        setAnimator(*animator_id);

    setPos(m_data.x, m_data.y);

    changeDirection(m_data.direct);

    setGenerator(m_data.generator,
                 m_data.generator_direct,
                 m_data.generator_type, true);

    setData(ITEM_ID, QString::number(m_data.id));
    setData(ITEM_ARRAY_ID, QString::number(m_data.meta.array_id));

    setData(ITEM_NPC_BLOCK_COLLISION,  QString::number((int)m_localProps.setup.collision_with_blocks));
    setData(ITEM_NPC_NO_NPC_COLLISION, QString::number((int)m_localProps.setup.no_npc_collisions));

    setData(ITEM_WIDTH,  QString::number(m_localProps.setup.width));  //width
    setData(ITEM_HEIGHT, QString::number(m_localProps.setup.height));  //height

    setData(ITEM_IS_META, m_localProps.setup.is_meta_object);

    m_scene->unregisterElement(this);
    m_scene->registerElement(this);
}


QRectF ItemNPC::boundingRect() const
{
    double x, y, r, b, xP, yP, rP, bP;

    x = -5.0 + m_imgOffsetX + (-((double)m_localProps.setup.gfx_offset_x) * m_direction);
    y = -5.0 + m_imgOffsetY;
    r = x + 10.0 + (m_animated ? (m_localProps.setup.gfx_w) : (m_imageSize.width()));
    b = y + 10.0 + (m_animated ? (m_localProps.setup.gfx_h) : (m_imageSize.height()));

    xP = -5.0;
    yP = -5.0;
    rP = xP + data(ITEM_WIDTH).toReal() + 10.0;
    bP = yP + data(ITEM_HEIGHT).toReal() + 10.0;

    x = std::min(x, xP);
    y = std::min(y, yP);
    r = std::max(r, rP);
    b = std::max(b, bP);

    QRectF rect;
    rect.setTopLeft(QPointF(x, y));
    rect.setBottomRight(QPointF(r, b));
    return rect;
}


void ItemNPC::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if(!m_extAnimator)
    {
        if(_internal_animator)
            painter->drawPixmap(m_offseted,
                                _internal_animator->wholeImage(),
                                _internal_animator->frameRect(m_direction));
        else
            painter->drawRect(QRect(0, 0, 32, 32));
    }
    else
    {
        if(m_animatorID < 0)
        {
            painter->drawRect(QRect(m_imgOffsetX, m_imgOffsetY, 1, 1));
            return;
        }

        if(m_scene->m_animatorsNPC.size() > m_animatorID)
            painter->drawPixmap(m_offseted,
                                m_scene->m_animatorsNPC[int(m_animatorID)]->wholeImage(),
                                m_scene->m_animatorsNPC[int(m_animatorID)]->frameRect(m_direction));
        else
            painter->drawRect(QRect(0, 0, 32, 32));

        if(this->isSelected())
        {
            painter->setPen(QPen(QBrush(Qt::black), 2, Qt::SolidLine));
            painter->drawRect(1, 1,
                              int(m_localProps.setup.width - 2), int(m_localProps.setup.height - 2));
            painter->setPen(QPen(QBrush(Qt::magenta), 2, Qt::DotLine));
            painter->drawRect(1, 1,
                              int(m_localProps.setup.width - 2), int(m_localProps.setup.height - 2));
        }
    }
}

void ItemNPC::setScenePoint(LvlScene *theScene)
{
    LvlBaseItem::setScenePoint(theScene);
    m_grp = new QGraphicsItemGroup(this);
}


void ItemNPC::setAnimator(long aniID)
{
    if(m_DisableScene) return;

    if(aniID < m_scene->m_animatorsNPC.size())
    {
        QRect frameRect = m_scene->m_animatorsNPC[int(aniID)]->frameRect(-1);
        m_imageSize = QRectF(0, 0, frameRect.width(), frameRect.height());
    }

    //this->setData(ITEM_WIDTH, QString::number(qRound(m_imageSize.width())));  //width
    //this->setData(ITEM_HEIGHT, QString::number(qRound(m_imageSize.height())));  //height

    m_animatorID = aniID;
    m_extAnimator = true;
    m_animated = true;

    //setPixmap(QPixmap(imageSize.width(), imageSize.height()));

    refreshOffsets();
}

bool ItemNPC::itemTypeIsLocked()
{
    if(!m_scene)
        return false;
    return m_scene->m_lockNpc;
}

void ItemNPC::refreshOffsets()
{
    m_imgOffsetX = (int)round(- (((double)m_localProps.setup.gfx_w - (double)m_localProps.setup.width) / 2));
    m_imgOffsetY = (int)round(- (double)m_localProps.setup.gfx_h + (double)m_localProps.setup.height + (double)m_localProps.setup.gfx_offset_y);

    m_offset_x = m_imgOffsetX + (-((double)m_localProps.setup.gfx_offset_x) * m_direction);
    m_offset_y = m_imgOffsetY;

    m_offseted = m_imageSize;
    m_offseted.setLeft(m_offseted.left() + m_offset_x);
    m_offseted.setTop(m_offseted.top() + m_offset_y);
    m_offseted.setRight(m_offseted.right() + m_offset_x);
    m_offseted.setBottom(m_offseted.bottom() + m_offset_y);
}
