/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2024 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <common_features/logger.h>
#include <Utils/dir_list_ci_qt.h>
#include <mainwindow.h>

#include "item_level.h"
#include "../wld_history_manager.h"

ItemLevel::ItemLevel(QGraphicsItem *parent)
    : WldBaseItem(parent)
{
    construct();
}

ItemLevel::ItemLevel(WldScene *parentScene, QGraphicsItem *parent)
    : WldBaseItem(parentScene, parent)
{
    construct();
    if(!parentScene) return;
    setScenePoint(parentScene);
    m_scene->addItem(this);
    setZValue(m_scene->Z_Levels);
}

void ItemLevel::construct()
{
    setData(ITEM_TYPE, "LEVEL");
}

ItemLevel::~ItemLevel()
{
    m_scene->unregisterElement(this);
}

void ItemLevel::contextMenu(QGraphicsSceneMouseEvent *mouseEvent)
{
    //Remove selection from non-bgo items
    if(!this->isSelected())
    {
        m_scene->clearSelection();
        this->setSelected(true);
    }

    this->setSelected(1);
    QMenu ItemMenu;
    DirListCIQt ci;
    QString lvlDstPath;
    QString lvlDstName;
    if(!m_data.lvlfile.isEmpty())
    {
        ci.setCurDir(m_scene->m_data->meta.path);
        lvlDstName = ci.resolveFileCase(m_data.lvlfile);
        lvlDstPath = m_scene->m_data->meta.path + "/" + lvlDstName;
    }

    QAction *lvlTitle =         ItemMenu.addAction(QString("[%1]").arg(m_data.title));
    lvlTitle->setEnabled(false);
    lvlTitle->setVisible(!m_data.title.isEmpty());

    QAction *openLvl =          ItemMenu.addAction(tr("Open target file: %1").arg(lvlDstName));
    openLvl->setVisible(!lvlDstName.isEmpty() && QFile::exists(lvlDstPath));
    ItemMenu.addSeparator();

    QAction *setPathBG =        ItemMenu.addAction(tr("Path background"));
    setPathBG->setCheckable(true);
    setPathBG->setChecked(m_data.pathbg);

    QAction *setBigPathBG =     ItemMenu.addAction(tr("Big Path background"));
    setBigPathBG->setCheckable(true);
    setBigPathBG->setChecked(m_data.bigpathbg);

    QAction *setAlVis =         ItemMenu.addAction(tr("Always Visible"));
    setAlVis->setCheckable(true);
    setAlVis->setChecked(m_data.alwaysVisible);
    ItemMenu.addSeparator();

    QMenu *copyPreferences =   ItemMenu.addMenu(tr("Copy preferences"));

    QAction *copyArrayID =      copyPreferences->addAction(tr("Array-ID: %1").arg(m_data.meta.array_id));
    QAction *copyItemID =       copyPreferences->addAction(tr("Level-ID: %1").arg(m_data.id));
    QAction *copyPosXY =        copyPreferences->addAction(tr("Position: X, Y"));
    QAction *copyPosXYWH =      copyPreferences->addAction(tr("Position: X, Y, Width, Height"));
    QAction *copyPosLTRB =      copyPreferences->addAction(tr("Position: Left, Top, Right, Bottom"));

    QAction *copyTile =         ItemMenu.addAction(tr("Copy"));
    QAction *cutTile =          ItemMenu.addAction(tr("Cut"));
    ItemMenu.addSeparator();
    QAction *transform =        ItemMenu.addAction(tr("Transform into"));
    QAction *transform_all =    ItemMenu.addAction(tr("Transform all %1 into").arg("LEVEL-%1").arg(m_data.id));
    ItemMenu.addSeparator();
    QAction *remove =           ItemMenu.addAction(tr("Remove"));
    QAction *remove_all =       ItemMenu.addAction(tr("Remove all %1").arg("LEVEL-%1").arg(m_data.id));
    ItemMenu.addSeparator();
    QAction *props =            ItemMenu.addAction(tr("Properties..."));

    m_scene->m_contextMenuIsOpened =  true; //bug protector

    QAction *selected =         ItemMenu.exec(mouseEvent->screenPos());

    if(!selected)
        return;

    if(selected == openLvl)
    {
        m_scene->m_mw ->OpenFile(lvlDstPath);
        m_scene->m_contextMenuIsOpened = false;
    }
    else if(selected == setPathBG)
    {
        m_scene->m_contextMenuIsOpened = false;
        WorldData selData;
        foreach(QGraphicsItem *SelItem, m_scene->selectedItems())
        {
            if(SelItem->data(ITEM_TYPE).toString() == "LEVEL")
            {
                selData.levels << ((ItemLevel *)SelItem)->m_data;
                ((ItemLevel *)SelItem)->setShowSmallPathBG(setPathBG->isChecked());
            }
        }
        m_scene->m_history->addChangeSettingsHistory(selData, HistorySettings::SETTING_PATHBACKGROUND, QVariant(setPathBG->isChecked()));
    }
    else if(selected == setBigPathBG)
    {
        m_scene->m_contextMenuIsOpened = false;
        WorldData selData;
        foreach(QGraphicsItem *SelItem, m_scene->selectedItems())
        {
            if(SelItem->data(ITEM_TYPE).toString() == "LEVEL")
            {
                selData.levels << ((ItemLevel *)SelItem)->m_data;
                ((ItemLevel *)SelItem)->setShowBigPathBG(setBigPathBG->isChecked());
            }
        }
        m_scene->m_history->addChangeSettingsHistory(selData, HistorySettings::SETTING_BIGPATHBACKGROUND, QVariant(setPathBG->isChecked()));
    }
    else if(selected == setAlVis)
    {
        m_scene->m_contextMenuIsOpened = false;
        WorldData selData;
        foreach(QGraphicsItem *SelItem, m_scene->selectedItems())
        {
            if(SelItem->data(ITEM_TYPE).toString() == "LEVEL")
            {
                selData.levels << ((ItemLevel *)SelItem)->m_data;
                ((ItemLevel *)SelItem)->alwaysVisible(setAlVis->isChecked());
            }
        }
        m_scene->m_history->addChangeSettingsHistory(selData, HistorySettings::SETTING_ALWAYSVISIBLE, QVariant(setPathBG->isChecked()));
    }
    else if(selected == cutTile)
    {
        //scene->doCut = true ;
        m_scene->m_mw->on_actionCut_triggered();
        m_scene->m_contextMenuIsOpened = false;
    }
    else if(selected == copyTile)
    {
        //scene->doCopy = true ;
        m_scene->m_mw->on_actionCopy_triggered();
        m_scene->m_contextMenuIsOpened = false;
    }
    else if((selected == transform) || (selected == transform_all))
    {
        WorldData oldData;
        WorldData newData;
        int transformTO;

        ItemSelectDialog *itemList = new ItemSelectDialog(m_scene->m_configs, ItemSelectDialog::TAB_LEVEL, 0, 0, 0, 0, 0, 0, 0, 0, 0, m_scene->m_subWindow, ItemSelectDialog::TAB_LEVEL);
        util::DialogToCenter(itemList, true);

        if(itemList->exec() == QDialog::Accepted)
        {
            QList<QGraphicsItem *> our_items;
            bool sameID = false;
            transformTO = itemList->levelID;
            unsigned long oldID = m_data.id;

            if(selected == transform)
                our_items = m_scene->selectedItems();
            else if(selected == transform_all)
            {
                our_items = m_scene->items();
                sameID = true;
            }

            foreach(QGraphicsItem *SelItem, our_items)
            {
                if(SelItem->data(ITEM_TYPE).toString() == "LEVEL")
                {
                    if((!sameID) || (((ItemLevel *) SelItem)->m_data.id == oldID))
                    {
                        oldData.levels.push_back(((ItemLevel *) SelItem)->m_data);
                        ((ItemLevel *) SelItem)->transformTo(transformTO);
                        newData.levels.push_back(((ItemLevel *) SelItem)->m_data);
                    }
                }
            }
        }
        delete itemList;
        if(!newData.levels.isEmpty())
            m_scene->m_history->addTransformHistory(newData, oldData);
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
    else if(selected == remove)
        m_scene->removeSelectedWldItems();
    else if(selected == remove_all)
    {
        QList<QGraphicsItem *> our_items;
        QList<QGraphicsItem *> selectedList;
        unsigned long oldID = m_data.id;
        our_items = m_scene->items();

        foreach(QGraphicsItem *SelItem, our_items)
        {
            if(SelItem->data(ITEM_TYPE).toString() == "LEVEL")
            {
                if(((ItemLevel *) SelItem)->m_data.id == oldID)
                    selectedList.push_back(SelItem);
            }
        }
        if(!selectedList.isEmpty())
        {
            m_scene->removeWldItems(selectedList);
            m_scene->Debugger_updateItemList();
        }
    }
    else if(selected == props)
        m_scene->openProps();
}


bool ItemLevel::itemTypeIsLocked()
{
    return m_scene->m_lockLevel;
}


///////////////////MainArray functions/////////////////////////////
//void ItemLevel::setLayer(QString layer)
//{
//    foreach(LevelLayers lr, scene->WldData->layers)
//    {
//        if(lr.name==layer)
//        {
//            levelData.layer = layer;
//            this->setVisible(!lr.hidden);
//            arrayApply();
//        break;
//        }
//    }
//}

void ItemLevel::transformTo(long target_id)
{
    if(target_id < 0) return;

    if(!m_scene->m_localConfigLevels.contains(target_id))
        return;

    obj_w_level &mergedSet = m_scene->m_localConfigLevels[target_id];
    long animator = mergedSet.animator_id;

    m_data.id = target_id;
    setLevelData(m_data, &mergedSet, &animator, &m_pathID, &m_bPathID);
    arrayApply();

    if(!m_scene->m_opts.animationEnabled)
        m_scene->update();
}

void ItemLevel::arrayApply()
{
    bool found = false;
    m_data.x = qRound(this->scenePos().x());
    m_data.y = qRound(this->scenePos().y());

    if(m_data.meta.index < (unsigned int)m_scene->m_data->levels.size())
    {
        //Check index
        if(m_data.meta.array_id == m_scene->m_data->levels[m_data.meta.index].meta.array_id)
            found = true;
    }

    //Apply current data in main array
    if(found)
    {
        //directlry
        m_scene->m_data->levels[m_data.meta.index] = m_data; //apply current levelData
    }
    else
        for(int i = 0; i < m_scene->m_data->levels.size(); i++)
        {
            //after find it into array
            if(m_scene->m_data->levels[i].meta.array_id == m_data.meta.array_id)
            {
                m_data.meta.index = i;
                m_scene->m_data->levels[i] = m_data;
                break;
            }
        }

    //Mark world map as modified
    m_scene->m_data->meta.modified = true;

    m_scene->unregisterElement(this);
    m_scene->registerElement(this);
}

void ItemLevel::removeFromArray()
{
    bool found = false;
    if(m_data.meta.index < (unsigned int)m_scene->m_data->levels.size())
    {
        //Check index
        if(m_data.meta.array_id == m_scene->m_data->levels[m_data.meta.index].meta.array_id)
            found = true;
    }

    if(found)
    {
        //directlry
        m_scene->m_data->levels.removeAt(m_data.meta.index);
    }
    else
        for(int i = 0; i < m_scene->m_data->levels.size(); i++)
        {
            if(m_scene->m_data->levels[i].meta.array_id == m_data.meta.array_id)
            {
                m_scene->m_data->levels.removeAt(i);
                break;
            }
        }

    //Mark world map as modified
    m_scene->m_data->meta.modified = true;
}

void ItemLevel::returnBack()
{
    setPos(m_data.x, m_data.y);
}

QPoint ItemLevel::sourcePos()
{
    return QPoint(m_data.x, m_data.y);
}


void ItemLevel::alwaysVisible(bool v)
{
    m_data.alwaysVisible = v;
    arrayApply();
}

void ItemLevel::setShowSmallPathBG(bool p)
{
    m_data.pathbg = p;
    arrayApply();
    this->update();
}

void ItemLevel::setShowBigPathBG(bool p)
{
    m_data.bigpathbg = p;
    arrayApply();

    this->update();

    m_imageSizeTarget = m_imageSize;

    if(p)
    {
        if(m_imageSizeTarget.left() > m_imageSizeBP.left())
            m_imageSizeTarget.setLeft(m_imageSizeBP.left());
        if(m_imageSizeTarget.right() < m_imageSizeBP.right())
            m_imageSizeTarget.setRight(m_imageSizeBP.right());
        if(m_imageSizeTarget.top() > m_imageSizeBP.top())
            m_imageSizeTarget.setTop(m_imageSizeBP.top());
        if(m_imageSizeTarget.bottom() < m_imageSizeBP.bottom())
            m_imageSizeTarget.setBottom(m_imageSizeBP.bottom());
    }
    m_scene->update();
}

void ItemLevel::setLevelData(WorldLevelTile inD, obj_w_level *mergedSet,
                             long *animator_id, long *path_id, long *bPath_id)
{
    m_data = inD;

    setPos(m_data.x, m_data.y);

    setData(ITEM_ID, QString::number(m_data.id));
    setData(ITEM_ARRAY_ID, QString::number(m_data.meta.array_id));

    if(mergedSet)
    {
        m_localProps = *mergedSet;
        m_gridSize = m_localProps.setup.grid;
        setData(ITEM_IS_META, m_localProps.setup.is_meta_object);
    }

    if(animator_id && path_id && bPath_id)
        setAnimator(*animator_id, *path_id, *bPath_id);

    m_scene->unregisterElement(this);
    m_scene->registerElement(this);
}


QRectF ItemLevel::boundingRect() const
{
    return m_imageSizeTarget;
}

void ItemLevel::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if(m_animatorID < 0)
    {
        painter->drawRect(QRect(m_imgOffsetX, m_imgOffsetY, 1, 1));
        return;
    }

    if(m_data.bigpathbg)
    {
        if(m_scene->m_animatorsLevels.size() > m_bPathID)
            painter->drawPixmap(m_imageSizeBP,
                                m_scene->m_animatorsLevels[m_bPathID]->wholeImage(),
                                m_scene->m_animatorsLevels[m_bPathID]->frameRect());
        else
            painter->drawRect(QRect(0, 0, 32, 32));
    }


    if(m_data.pathbg)
    {
        if(m_scene->m_animatorsLevels.size() > m_pathID)
            painter->drawPixmap(m_imageSizeP,
                                m_scene->m_animatorsLevels[m_pathID]->wholeImage(),
                                m_scene->m_animatorsLevels[m_pathID]->frameRect());
        else
            painter->drawRect(QRect(0, 0, 32, 32));
    }

    if(m_scene->m_animatorsLevels.size() > m_animatorID)
        painter->drawPixmap(m_imageSize,
                            m_scene->m_animatorsLevels[m_animatorID]->wholeImage(),
                            m_scene->m_animatorsLevels[m_animatorID]->frameRect());
    else
        painter->drawRect(QRect(0, 0, 32, 32));

    if(this->isSelected())
    {
        painter->setPen(QPen(QBrush(Qt::black), 2, Qt::SolidLine));
        painter->drawRect(m_imgOffsetX + 1, m_imgOffsetY + 1, m_imageSize.width() - 2, m_imageSize.height() - 2);
        painter->setPen(QPen(QBrush(Qt::magenta), 2, Qt::DotLine));
        painter->drawRect(m_imgOffsetX + 1, m_imgOffsetY + 1, m_imageSize.width() - 2, m_imageSize.height() - 2);
    }
}


////////////////Animation///////////////////

void ItemLevel::setAnimator(long aniID, long path, long bPath)
{
    if(aniID < m_scene->m_animatorsLevels.size())
    {
        QRect frameRect = m_scene->m_animatorsLevels[aniID]->frameRect();
        m_imgOffsetX = (int)qRound(-(qreal(frameRect.width() - m_gridSize)  / 2));
        m_imgOffsetY = (int)qRound(-qreal(frameRect.height()) + m_gridSize);
        m_imageSize = QRectF(m_imgOffsetX, m_imgOffsetY, frameRect.width(), frameRect.height());
    }

    if(path < m_scene->m_animatorsLevels.size())
    {
        QRect frameRect = m_scene->m_animatorsLevels[path]->frameRect();
        m_imgOffsetXp = (int)qRound(-(qreal(frameRect.width() - m_gridSize)  / 2));
        m_imgOffsetYp = (int)qRound(-qreal(frameRect.height()) + m_gridSize);
        m_imageSizeP = QRectF(m_imgOffsetXp, m_imgOffsetYp, frameRect.width(), frameRect.height());
    }

    if(bPath < m_scene->m_animatorsLevels.size())
    {
        QRect frameRect = m_scene->m_animatorsLevels[bPath]->frameRect();
        m_imgOffsetXbp = (int)qRound(-(qreal(frameRect.width() - m_gridSize)  / 2));
        m_imgOffsetYbp = (int)qRound(-qreal(frameRect.height()) + qreal(m_gridSize) * 1.25);
        m_imageSizeBP = QRectF(m_imgOffsetXbp, m_imgOffsetYbp, frameRect.width(), frameRect.height());
    }

    m_imageSizeTarget = m_imageSize;

    if(m_data.bigpathbg)
    {
        if(m_imageSizeTarget.left() > m_imageSizeBP.left())
            m_imageSizeTarget.setLeft(m_imageSizeBP.left());
        if(m_imageSizeTarget.right() < m_imageSizeBP.right())
            m_imageSizeTarget.setRight(m_imageSizeBP.right());
        if(m_imageSizeTarget.top() > m_imageSizeBP.top())
            m_imageSizeTarget.setTop(m_imageSizeBP.top());
        if(m_imageSizeTarget.bottom() < m_imageSizeBP.bottom())
            m_imageSizeTarget.setBottom(m_imageSizeBP.bottom());
    }

    this->setData(ITEM_WIDTH, QString::number(m_gridSize));    //width
    this->setData(ITEM_HEIGHT, QString::number(m_gridSize));    //height
    //WriteToLog(QtDebugMsg, QString("Tile Animator ID: %1").arg(aniID));

    m_pathID = path;
    m_bPathID = bPath;
    m_animatorID = aniID;
}

