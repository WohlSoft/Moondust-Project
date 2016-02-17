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
#include <common_features/logger.h>
#include <common_features/mainwinconnect.h>

#include "item_level.h"

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
    setZValue(m_scene->levelZ);
}

void ItemLevel::construct()
{
    setData(ITEM_TYPE, "LEVEL");
}

ItemLevel::~ItemLevel()
{
    m_scene->unregisterElement(this);
}

void ItemLevel::contextMenu( QGraphicsSceneMouseEvent * mouseEvent )
{
    //Remove selection from non-bgo items
    if(!this->isSelected())
    {
        m_scene->clearSelection();
        this->setSelected(true);
    }

    this->setSelected(1);
    QMenu ItemMenu;

    QAction *LvlTitle =         ItemMenu.addAction(QString("[%1]").arg(m_data.title));
        LvlTitle->setEnabled(false);
        LvlTitle->setVisible(!m_data.title.isEmpty());

    QAction *openLvl =          ItemMenu.addAction(tr("Open target file: %1").arg(m_data.lvlfile));
        openLvl->setVisible( (!m_data.lvlfile.isEmpty()) && (QFile(m_scene->WldData->path + "/" + m_data.lvlfile).exists()) );
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

    QMenu * copyPreferences =   ItemMenu.addMenu(tr("Copy preferences"));

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
                                ItemMenu.addSeparator();
    QAction *props =            ItemMenu.addAction(tr("Properties..."));

    m_scene->contextMenuOpened =  true; //bug protector

    QAction *selected =         ItemMenu.exec(mouseEvent->screenPos());
    if(!selected) return;


    if(selected==openLvl)
    {
        MainWinConnect::pMainWin->OpenFile(m_scene->WldData->path + "/" + m_data.lvlfile);
        m_scene->contextMenuOpened = false;
    }
    else
    if(selected==setPathBG)
    {
        m_scene->contextMenuOpened = false;
        WorldData selData;
        foreach(QGraphicsItem * SelItem, m_scene->selectedItems() )
        {
            if(SelItem->data(0).toString()=="LEVEL")
            {
                selData.levels << ((ItemLevel *)SelItem)->m_data;
                ((ItemLevel *)SelItem)->setShowSmallPathBG( setPathBG->isChecked() );
            }
        }
        m_scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_PATHBACKGROUND, QVariant(setPathBG->isChecked()));
    }
    else
    if(selected==setBigPathBG)
    {
        m_scene->contextMenuOpened = false;
        WorldData selData;
        foreach(QGraphicsItem * SelItem, m_scene->selectedItems() )
        {
            if(SelItem->data(0).toString()=="LEVEL")
            {
                selData.levels << ((ItemLevel *)SelItem)->m_data;
                ((ItemLevel *)SelItem)->setShowBigPathBG( setBigPathBG->isChecked() );
            }
        }
        m_scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_BIGPATHBACKGROUND, QVariant(setPathBG->isChecked()));
    }
    else
    if(selected==setAlVis)
    {
        m_scene->contextMenuOpened = false;
        WorldData selData;
        foreach(QGraphicsItem * SelItem, m_scene->selectedItems() )
        {
            if(SelItem->data(0).toString()=="LEVEL")
            {
                selData.levels << ((ItemLevel *)SelItem)->m_data;
                ((ItemLevel *)SelItem)->alwaysVisible( setAlVis->isChecked() );
            }
        }
        m_scene->addChangeSettingsHistory(selData, HistorySettings::SETTING_ALWAYSVISIBLE, QVariant(setPathBG->isChecked()));
    }
    else
    if(selected==cutTile)
    {
        //scene->doCut = true ;
        MainWinConnect::pMainWin->on_actionCut_triggered();
        m_scene->contextMenuOpened = false;
    }
    else
    if(selected==copyTile)
    {
        //scene->doCopy = true ;
        MainWinConnect::pMainWin->on_actionCopy_triggered();
        m_scene->contextMenuOpened = false;
    }
    else
    if((selected==transform)||(selected==transform_all))
    {
        WorldData oldData;
        WorldData newData;
        int transformTO;

        ItemSelectDialog * itemList = new ItemSelectDialog(m_scene->pConfigs, ItemSelectDialog::TAB_LEVEL,0,0,0,0,0,0,0,0,0,MainWinConnect::pMainWin);
        itemList->removeEmptyEntry(ItemSelectDialog::TAB_LEVEL);
        util::DialogToCenter(itemList, true);

        if(itemList->exec()==QDialog::Accepted)
        {
            QList<QGraphicsItem *> our_items;
            bool sameID=false;
            transformTO = itemList->levelID;
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
                if(SelItem->data(ITEM_TYPE).toString()=="LEVEL")
                {
                    if((!sameID)||(((ItemLevel *) SelItem)->m_data.id==oldID))
                    {
                        oldData.levels.push_back( ((ItemLevel *) SelItem)->m_data );
                        ((ItemLevel *) SelItem)->transformTo(transformTO);
                        newData.levels.push_back( ((ItemLevel *) SelItem)->m_data );
                    }
                }
            }
        }
        delete itemList;
        if(!newData.levels.isEmpty())
            m_scene->addTransformHistory(newData, oldData);
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
    if(selected==remove)
    {
        m_scene->removeSelectedWldItems();
    }
    else
    if(selected==props)
    {
        m_scene->openProps();
    }
}


bool ItemLevel::itemTypeIsLocked()
{
    return m_scene->lock_level;
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
    if(target_id<1) return;

    bool noimage=true;
    long item_i=0;
    long animator=0;
    obj_w_level mergedSet;

    //Get Level settings
    m_scene->getConfig_Level(target_id, item_i, animator, mergedSet, &noimage);

    if(noimage)
        return;//Don't transform, target item is not found

    m_data.id = target_id;
    setLevelData(m_data, &mergedSet, &animator, &m_pathID, &m_bPathID);
    arrayApply();

    if(!m_scene->opts.animationEnabled)
        m_scene->update();
}

void ItemLevel::arrayApply()
{
    bool found=false;
    m_data.x = qRound(this->scenePos().x());
    m_data.y = qRound(this->scenePos().y());

    if(m_data.index < (unsigned int)m_scene->WldData->levels.size())
    { //Check index
        if(m_data.array_id == m_scene->WldData->levels[m_data.index].array_id)
        {
            found=true;
        }
    }

    //Apply current data in main array
    if(found)
    { //directlry
        m_scene->WldData->levels[m_data.index] = m_data; //apply current levelData
    }
    else
    for(int i=0; i<m_scene->WldData->levels.size(); i++)
    { //after find it into array
        if(m_scene->WldData->levels[i].array_id == m_data.array_id)
        {
            m_data.index = i;
            m_scene->WldData->levels[i] = m_data;
            break;
        }
    }

    m_scene->unregisterElement(this);
    m_scene->registerElement(this);
}

void ItemLevel::removeFromArray()
{
    bool found=false;
    if(m_data.index < (unsigned int)m_scene->WldData->levels.size())
    { //Check index
        if(m_data.array_id == m_scene->WldData->levels[m_data.index].array_id)
        {
            found=true;
        }
    }

    if(found)
    { //directlry
        m_scene->WldData->levels.removeAt(m_data.index);
    }
    else
    for(int i=0; i<m_scene->WldData->levels.size(); i++)
    {
        if(m_scene->WldData->levels[i].array_id == m_data.array_id)
        {
            m_scene->WldData->levels.removeAt(i); break;
        }
    }
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
    m_data.pathbg=p;
    arrayApply();
    this->update();
}

void ItemLevel::setShowBigPathBG(bool p)
{
    m_data.bigpathbg=p;
    arrayApply();

    this->update();

    m_imageSizeTarget=m_imageSize;

    if(p)
    {
        if(m_imageSizeTarget.left() > m_imageSizeBP.left())
            m_imageSizeTarget.setLeft( m_imageSizeBP.left() );
        if(m_imageSizeTarget.right() < m_imageSizeBP.right())
            m_imageSizeTarget.setRight( m_imageSizeBP.right() );
        if(m_imageSizeTarget.top() > m_imageSizeBP.top())
            m_imageSizeTarget.setTop( m_imageSizeBP.top() );
        if(m_imageSizeTarget.bottom() < m_imageSizeBP.bottom())
            m_imageSizeTarget.setBottom( m_imageSizeBP.bottom() );
    }
    m_scene->update();
}

void ItemLevel::setLevelData(WorldLevels inD, obj_w_level *mergedSet,
                             long *animator_id, long *path_id, long *bPath_id)
{
    m_data = inD;

    setPos(m_data.x, m_data.y);

    setData(ITEM_ID, QString::number(m_data.id) );
    setData(ITEM_ARRAY_ID, QString::number(m_data.array_id) );

    if(mergedSet)
    {
        m_localProps = *mergedSet;
        m_gridSize = m_localProps.grid;
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
    if(m_animatorID<0)
    {
        painter->drawRect(QRect(m_imgOffsetX,m_imgOffsetY,1,1));
        return;
    }

    if(m_data.bigpathbg)
    {
        if(m_scene->animates_Levels.size()>m_bPathID)
            painter->drawPixmap(m_imageSizeBP, m_scene->animates_Levels[m_bPathID]->image(), m_scene->animates_Levels[m_bPathID]->image().rect());
        else
            painter->drawRect(QRect(0,0,32,32));
    }


    if(m_data.pathbg)
    {
        if(m_scene->animates_Levels.size()>m_pathID)
            painter->drawPixmap(m_imageSizeP, m_scene->animates_Levels[m_pathID]->image(), m_scene->animates_Levels[m_pathID]->image().rect());
        else
            painter->drawRect(QRect(0,0,32,32));
    }

    if(m_scene->animates_Levels.size()>m_animatorID)
        painter->drawPixmap(m_imageSize, m_scene->animates_Levels[m_animatorID]->image(), m_scene->animates_Levels[m_animatorID]->image().rect());
    else
        painter->drawRect(QRect(0,0,32,32));

    if(this->isSelected())
    {
        painter->setPen(QPen(QBrush(Qt::black), 2, Qt::SolidLine));
        painter->drawRect(m_imgOffsetX+1,m_imgOffsetY+1,m_imageSize.width()-2,m_imageSize.height()-2);
        painter->setPen(QPen(QBrush(Qt::magenta), 2, Qt::DotLine));
        painter->drawRect(m_imgOffsetX+1,m_imgOffsetY+1,m_imageSize.width()-2,m_imageSize.height()-2);
    }
}


////////////////Animation///////////////////

void ItemLevel::setAnimator(long aniID, long path, long bPath)
{
    if(aniID < m_scene->animates_Levels.size())
    {
    m_imgOffsetX = (int)qRound( -( qreal(m_scene->animates_Levels[aniID]->image().width() - m_gridSize)  / 2 ) );
    m_imgOffsetY = (int)qRound( -qreal(m_scene->animates_Levels[aniID]->image().height()) + m_gridSize );
    m_imageSize = QRectF(m_imgOffsetX,m_imgOffsetY,
                m_scene->animates_Levels[aniID]->image().width(),
                m_scene->animates_Levels[aniID]->image().height()
                );
    }

    if(path < m_scene->animates_Levels.size())
    {
    m_imgOffsetXp = (int)qRound( -( qreal(m_scene->animates_Levels[path]->image().width() - m_gridSize)  / 2 ) );
    m_imgOffsetYp = (int)qRound( -qreal(m_scene->animates_Levels[path]->image().height()) + m_gridSize );
    m_imageSizeP = QRectF(m_imgOffsetXp,m_imgOffsetYp,
                m_scene->animates_Levels[path]->image().width(),
                m_scene->animates_Levels[path]->image().height()
                );
    }

    if(bPath < m_scene->animates_Levels.size())
    {
    m_imgOffsetXbp = (int)qRound( -( qreal(m_scene->animates_Levels[bPath]->image().width() - m_gridSize)  / 2 ) );
    m_imgOffsetYbp = (int)qRound( -qreal(m_scene->animates_Levels[bPath]->image().height()) + qreal(m_gridSize)*1.25);
    m_imageSizeBP = QRectF(m_imgOffsetXbp,m_imgOffsetYbp,
                m_scene->animates_Levels[bPath]->image().width(),
                m_scene->animates_Levels[bPath]->image().height()
                );
    }


    m_imageSizeTarget=m_imageSize;

    if(m_data.bigpathbg)
    {
        if(m_imageSizeTarget.left() > m_imageSizeBP.left())
            m_imageSizeTarget.setLeft( m_imageSizeBP.left() );
        if(m_imageSizeTarget.right() < m_imageSizeBP.right())
            m_imageSizeTarget.setRight( m_imageSizeBP.right() );
        if(m_imageSizeTarget.top() > m_imageSizeBP.top())
            m_imageSizeTarget.setTop( m_imageSizeBP.top() );
        if(m_imageSizeTarget.bottom() < m_imageSizeBP.bottom())
            m_imageSizeTarget.setBottom( m_imageSizeBP.bottom() );
    }

    this->setData(ITEM_WIDTH, QString::number( m_gridSize ) ); //width
    this->setData(ITEM_HEIGHT, QString::number( m_gridSize ) ); //height
    //WriteToLog(QtDebugMsg, QString("Tile Animator ID: %1").arg(aniID));

    m_pathID = path;
    m_bPathID = bPath;
    m_animatorID = aniID;
}

