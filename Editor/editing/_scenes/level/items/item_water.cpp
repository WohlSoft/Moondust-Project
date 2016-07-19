/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <mainwindow.h>
#include <common_features/logger.h>

#include "../lvl_history_manager.h"
#include "../newlayerbox.h"

ItemPhysEnv::ItemPhysEnv(QGraphicsItem *parent)
    : LvlBaseItem(parent)
{
    construct();
}

ItemPhysEnv::ItemPhysEnv(LvlScene *parentScene, QGraphicsItem *parent)
    : LvlBaseItem(parentScene, parent)
{
    construct();
    if(!parentScene) return;
    setScenePoint(parentScene);
    m_scene->addItem(this);
    m_gridSize = m_scene->m_configs->default_grid/2;
    setZValue(m_scene->Z_sys_PhysEnv);
    setLocked(m_scene->m_lockPhysenv);
}

void ItemPhysEnv::construct()
{
    m_waterSize = QSize(32,32);
    m_penWidth=2;

    m_color=QColor(Qt::darkBlue);
    m_pen = QPen(m_color, m_penWidth, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
    m_pen.setWidth(m_penWidth);
    m_pen.setCapStyle(Qt::FlatCap);
    m_pen.setJoinStyle(Qt::MiterJoin);

    m_gridSize=16;

    m_data.w=32;
    m_data.h=32;
    m_data.x=this->pos().x();
    m_data.y=this->pos().y();
    m_data.env_type = LevelPhysEnv::ENV_WATER;

    setData(ITEM_TYPE, "Water");
    setData(ITEM_BLOCK_IS_SIZABLE, "sizable");
    setData(ITEM_WIDTH, (int)m_data.w);
    setData(ITEM_HEIGHT, (int)m_data.h);
}

ItemPhysEnv::~ItemPhysEnv()
{
    m_scene->unregisterElement(this);
}


void ItemPhysEnv::contextMenu( QGraphicsSceneMouseEvent * mouseEvent )
{
    m_scene->m_contextMenuIsOpened = true; //bug protector

    //Remove selection from non-bgo items
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
    LayerName->addSeparator();

    foreach(LevelLayer layer, m_scene->m_data->layers)
    {
        //Skip system layers
        if((layer.name=="Destroyed Blocks")||(layer.name=="Spawned NPCs")) continue;

        setLayer = LayerName->addAction( layer.name.replace("&", "&&&")+((layer.hidden)?""+tr("[hidden]"):"") );
        setLayer->setData(layer.name);
        setLayer->setCheckable(true);
        setLayer->setEnabled(true);
        setLayer->setChecked( layer.name==m_data.layer );
        layerItems.push_back(setLayer);
    }
    ItemMenu.addSeparator();
    /*************Layers*end***************/

    QMenu * WaterType =     ItemMenu.addMenu(tr("Environment type"));
        WaterType->deleteLater();

    #define CONTEXT_MENU_ITEM_CHK(name, enable, label, checked_condition)\
        name = WaterType->addAction(label);\
        name->setCheckable(true);\
        name->setEnabled(enable);\
        name->setChecked(checked_condition); typeID++;

    QAction *envTypes[13]; int typeID=0;

    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], true,                          tr("Water"),        m_data.env_type==LevelPhysEnv::ENV_WATER);
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], true,                          tr("Quicksand"),    m_data.env_type==LevelPhysEnv::ENV_QUICKSAND);
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], !m_scene->m_data->meta.smbx64strict, tr("Custom liquid"), m_data.env_type==LevelPhysEnv::ENV_CUSTOM_LIQUID);
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], !m_scene->m_data->meta.smbx64strict, tr("Gravitational Field"), m_data.env_type==LevelPhysEnv::ENV_GRAVITATIONAL_FIELD);
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], !m_scene->m_data->meta.smbx64strict, tr("Touch Event (Once)"), m_data.env_type==LevelPhysEnv::ENV_TOUCH_EVENT_ONCE_PLAYER);
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], !m_scene->m_data->meta.smbx64strict, tr("Touch Event (Every frame)"), m_data.env_type==LevelPhysEnv::ENV_TOUCH_EVENT_PLAYER);
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], !m_scene->m_data->meta.smbx64strict, tr("NPC Touch Event (Once)"), m_data.env_type==LevelPhysEnv::ENV_TOUCH_EVENT_ONCE_NPC);
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], !m_scene->m_data->meta.smbx64strict, tr("NPC Touch Event (Every frame)"), m_data.env_type==LevelPhysEnv::ENV_TOUCH_EVENT_NPC);
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], !m_scene->m_data->meta.smbx64strict, tr("Mouse click Event"), m_data.env_type==LevelPhysEnv::ENV_CLICK_EVENT);
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], !m_scene->m_data->meta.smbx64strict, tr("Collision script"), m_data.env_type==LevelPhysEnv::ENV_COLLISION_SCRIPT);
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], !m_scene->m_data->meta.smbx64strict, tr("Mouse click Script"), m_data.env_type==LevelPhysEnv::ENV_CLICK_SCRIPT);
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], !m_scene->m_data->meta.smbx64strict, tr("Collision Event"), m_data.env_type==LevelPhysEnv::ENV_COLLISION_EVENT);
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], !m_scene->m_data->meta.smbx64strict, tr("Air chamber"), m_data.env_type==LevelPhysEnv::ENV_AIR);

    ItemMenu.addSeparator();

    QMenu * copyPreferences =   ItemMenu.addMenu(tr("Copy preferences"));
    QAction *copyPosXYWH =      copyPreferences->addAction(tr("Position: X, Y, Width, Height"));
    QAction *copyPosLTRB =      copyPreferences->addAction(tr("Position: Left, Top, Right, Bottom"));
                                ItemMenu.addSeparator();

    QAction *resize =           ItemMenu.addAction(tr("Resize"));
        resize->deleteLater();

                                ItemMenu.addSeparator();
    QAction *copyWater =        ItemMenu.addAction(tr("Copy"));
    QAction *cutWater =         ItemMenu.addAction(tr("Cut"));
                                ItemMenu.addSeparator();
    QAction *remove =           ItemMenu.addAction(tr("Remove"));

    /*****************Waiting for answer************************/
    QAction *selected = ItemMenu.exec(mouseEvent->screenPos());
    /***********************************************************/

    if(!selected)
        return;


    if(selected==cutWater)
    {
        m_scene->m_mw->on_actionCut_triggered();
    }
    else
    if(selected==copyWater)
    {
        m_scene->m_mw->on_actionCopy_triggered();
    }
    else
    if(selected==copyPosXYWH)
    {
        QApplication::clipboard()->setText(
                            QString("X=%1; Y=%2; W=%3; H=%4;")
                               .arg(m_data.x)
                               .arg(m_data.y)
                               .arg(m_data.w)
                               .arg(m_data.h)
                               );
        m_scene->m_mw->showStatusMsg(tr("Preferences has been copied: %1").arg(QApplication::clipboard()->text()));
    }
    else
    if(selected==copyPosLTRB)
    {
        QApplication::clipboard()->setText(
                            QString("Left=%1; Top=%2; Right=%3; Bottom=%4;")
                               .arg(m_data.x)
                               .arg(m_data.y)
                               .arg(m_data.x+m_data.w)
                               .arg(m_data.y+m_data.h)
                               );
        m_scene->m_mw->showStatusMsg(tr("Preferences has been copied: %1").arg(QApplication::clipboard()->text()));
    }
    else
    if(selected==resize)
    {
        m_scene->setPhysEnvResizer(this, true);
    }
    else
    if(selected==remove)
    {
        m_scene->removeSelectedLvlItems();
    }
    else
    if(selected==newLayer)
    {
        m_scene->setLayerToSelected();
    }
    else
    {
        bool found=false;
        //Fetch layers menu
        foreach(QAction * lItem, layerItems)
        {
            if(selected==lItem)
            {
                //FOUND!!!
                m_scene->setLayerToSelected(lItem->data().toString());
                found=true;
                break;
            }//Find selected layer's item
        }

        if(!found)
        {
            for(int i=0; i<typeID; i++)
            {
                if(selected==envTypes[i])
                {
                    LevelData modData;
                    foreach(QGraphicsItem * SelItem, m_scene->selectedItems() )
                    {
                        if(SelItem->data(ITEM_TYPE).toString()=="Water")
                        {
                            modData.physez.push_back(((ItemPhysEnv *)SelItem)->m_data);
                            ((ItemPhysEnv *)SelItem)->setType(i);
                        }
                    }
                    m_scene->m_history->addChangeSettings(modData, HistorySettings::SETTING_WATERTYPE, QVariant(true));
                    found = true;
                    break;
                }
            }
        }
    }
}

///////////////////MainArray functions/////////////////////////////
void ItemPhysEnv::setLayer(QString layer)
{
    foreach(LevelLayer lr, m_scene->m_data->layers)
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

void ItemPhysEnv::arrayApply()
{
    bool found=false;

    m_data.x = qRound(this->scenePos().x());
    m_data.y = qRound(this->scenePos().y());
    this->setData(ITEM_WIDTH, (int)m_data.w);
    this->setData(ITEM_HEIGHT, (int)m_data.h);

    if(m_data.meta.index < (unsigned int)m_scene->m_data->physez.size())
    { //Check index
        if(m_data.meta.array_id == m_scene->m_data->physez[m_data.meta.index].meta.array_id)
        {
            found=true;
        }
    }

    //Apply current data in main array
    if(found)
    { //directlry
        m_scene->m_data->physez[m_data.meta.index] = m_data; //apply current bgoData
    }
    else
    for(int i=0; i<m_scene->m_data->physez.size(); i++)
    { //after find it into array
        if(m_scene->m_data->physez[i].meta.array_id == m_data.meta.array_id)
        {
            m_data.meta.index = i;
            m_scene->m_data->physez[i] = m_data;
            break;
        }
    }

    //Update R-tree innex
    m_scene->unregisterElement(this);
    m_scene->registerElement(this);
}

void ItemPhysEnv::removeFromArray()
{
    bool found=false;
    if(m_data.meta.index < (unsigned int)m_scene->m_data->physez.size())
    { //Check index
        if(m_data.meta.array_id == m_scene->m_data->physez[m_data.meta.index].meta.array_id)
        {
            found=true;
        }
    }

    if(found)
    { //directlry
        m_scene->m_data->physez.removeAt(m_data.meta.index);
    }
    else
    for(int i=0; i<m_scene->m_data->physez.size(); i++)
    {
        if(m_scene->m_data->physez[i].meta.array_id == m_data.meta.array_id)
        {
            m_scene->m_data->physez.removeAt(i); break;
        }
    }
}


void ItemPhysEnv::returnBack()
{
    this->setPos(m_data.x, m_data.y);
}

QPoint ItemPhysEnv::sourcePos()
{
    return QPoint(m_data.x, m_data.y);
}

void ItemPhysEnv::updateColor()
{
    switch(m_data.env_type)
    {
    case LevelPhysEnv::ENV_WATER:
    default:
        m_color=QColor(Qt::green);
        break;
    case LevelPhysEnv::ENV_QUICKSAND:
        m_color=QColor(Qt::yellow);
        break;
    case LevelPhysEnv::ENV_CUSTOM_LIQUID:
        m_color=QColor(Qt::darkGreen);
        break;
    case LevelPhysEnv::ENV_AIR:
        m_color=QColor(Qt::blue);
        break;
    case LevelPhysEnv::ENV_GRAVITATIONAL_FIELD:
        m_color=QColor(Qt::cyan);
        break;
    case LevelPhysEnv::ENV_TOUCH_EVENT_ONCE_PLAYER:
        m_color=QColor(Qt::darkRed);
        break;
    case LevelPhysEnv::ENV_TOUCH_EVENT_PLAYER:
        m_color=QColor(Qt::red);
        break;
    case LevelPhysEnv::ENV_TOUCH_EVENT_ONCE_NPC:
        m_color=QColor(Qt::darkMagenta);
        break;
    case LevelPhysEnv::ENV_TOUCH_EVENT_NPC:
        m_color=QColor(Qt::magenta);
        break;
    case LevelPhysEnv::ENV_CLICK_EVENT:
        m_color=QColor(Qt::darkCyan);
        break;
    case LevelPhysEnv::ENV_CLICK_SCRIPT:
        m_color=QColor(Qt::darkCyan);
        break;
    case LevelPhysEnv::ENV_COLLISION_EVENT:
        m_color=QColor(Qt::darkCyan);
        break;
    case LevelPhysEnv::ENV_COLLISION_SCRIPT:
        m_color=QColor(Qt::darkCyan);
        break;
    }
    m_pen.setColor(m_color);
}

bool ItemPhysEnv::itemTypeIsLocked()
{
    if(!m_scene)
        return false;
    return m_scene->m_lockPhysenv;
}

void ItemPhysEnv::setType(int tp)
{
    m_data.env_type=tp;
    updateColor();
    //this->setPen(_pen);
    arrayApply();
}

void ItemPhysEnv::setRectSize(QRect rect)
{
    m_data.x = rect.x();
    m_data.y = rect.y();
    m_data.w = rect.width();
    m_data.h = rect.height();
    m_waterSize = rect.size();
    setPos(m_data.x, m_data.y);
    drawWater();
    arrayApply();
}

void ItemPhysEnv::setSize(QSize sz)
{
    m_waterSize = sz;
    m_data.w = sz.width();
    m_data.h = sz.height();
    drawWater();
    arrayApply();
}


void ItemPhysEnv::setPhysEnvData(LevelPhysEnv inD)
{
    m_data = inD;
    m_waterSize = QSize(m_data.w, m_data.h);
    setPos(m_data.x, m_data.y);
    setData(ITEM_ID, QString::number(0) );
    setData(ITEM_ARRAY_ID, QString::number(m_data.meta.array_id) );
    updateColor();
    drawWater();
    m_scene->unregisterElement(this);
    m_scene->registerElement(this);
}

void ItemPhysEnv::drawWater()
{
//    long x, y, h, w;

//    x = 1;
//    y = 1;
//    w = waterData.w-penWidth;
//    h = waterData.h-penWidth;
    setData(ITEM_WIDTH,  (int)m_data.w);
    setData(ITEM_HEIGHT, (int)m_data.h);
    m_pen=QPen(m_color, m_penWidth, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
    //setPen(_pen);
//    QVector<QPointF > points;
//    points.clear();

//    points.push_back(QPointF(x+3, y));
//    points.push_back(QPointF(x+w, y));
//    points.push_back(QPointF(x+w,y+h));
//    points.push_back(QPointF(x, y+h));
//    points.push_back(QPointF(x, y+3));

//    points.push_back(QPointF(x, y+h));
//    points.push_back(QPointF(x+w,y+h));
//    points.push_back(QPointF(x+w, y));
//    points.push_back(QPointF(x+3, y));

//    this->setPolygon( QPolygonF(points) );
}

QRectF ItemPhysEnv::boundingRect() const
{
    return QRectF(-1,-1,
                  m_waterSize.width()+m_penWidth,
                  m_waterSize.height()+m_penWidth);
}

void ItemPhysEnv::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setPen(m_pen);
    painter->setBrush(Qt::NoBrush);

    long h, w;
    w = m_data.w-m_penWidth;
    h = m_data.h-m_penWidth;

    painter->drawRect(1, 1, w, h);
    if(this->isSelected())
    {
        painter->setPen(QPen(QBrush(Qt::black), 2, Qt::SolidLine));
        painter->drawRect(1,1,w,h);
        painter->setPen(QPen(QBrush(Qt::white), 2, Qt::DotLine));
        painter->drawRect(1,1,w,h);
    }
}


