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
    m_gridSize = m_scene->m_configs->defaultGrid.general / 2;
    setZValue(m_scene->Z_sys_PhysEnv);
    setLocked(m_scene->m_lockPhysenv);
}

void ItemPhysEnv::construct()
{
    m_waterSize = QSize(32, 32);
    m_penWidth = 2;

    m_color = QColor(Qt::darkBlue);
    m_pen = QPen(m_color, m_penWidth, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
    m_pen.setWidth(m_penWidth);
    m_pen.setCapStyle(Qt::FlatCap);
    m_pen.setJoinStyle(Qt::MiterJoin);

    m_gridSize = 16;

    m_data.w = 32;
    m_data.h = 32;
    m_data.x = qRound(this->pos().x());
    m_data.y = qRound(this->pos().y());
    m_data.env_type = LevelPhysEnv::ENV_WATER;

    setData(ITEM_TYPE, "Water");
    setData(ITEM_BLOCK_IS_SIZABLE, "sizable");
    setData(ITEM_WIDTH, static_cast<int>(m_data.w));
    setData(ITEM_HEIGHT, static_cast<int>(m_data.h));
}

ItemPhysEnv::~ItemPhysEnv()
{
    m_scene->unregisterElement(this);
}


void ItemPhysEnv::contextMenu(QGraphicsSceneMouseEvent *mouseEvent)
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
    QMenu *LayerName = ItemMenu.addMenu(tr("Layer: ") + QString("[%1]").arg(m_data.layer).replace("&", "&&"));
    QAction *setLayer;
    QList<QAction *> layerItems;
    QAction *newLayer = LayerName->addAction(tr("Add to new layer..."));
    LayerName->addSeparator();

    for(const LevelLayer &layer : m_scene->m_data->layers)
    {
        //Skip system layers
        if((layer.name == "Destroyed Blocks") || (layer.name == "Spawned NPCs"))
            continue;

        QString label = layer.name + ((layer.hidden) ? " " + tr("[hidden]") : "");
        setLayer = LayerName->addAction(label.replace("&", "&&"));
        setLayer->setData(layer.name);
        setLayer->setCheckable(true);
        setLayer->setEnabled(true);
        setLayer->setChecked(layer.name == m_data.layer);
        layerItems.push_back(setLayer);
    }
    ItemMenu.addSeparator();
    /*************Layers*end***************/

    QMenu *WaterType =     ItemMenu.addMenu(tr("Environment type"));
    WaterType->deleteLater();

#define CONTEXT_MENU_ITEM_CHK(name, enable, visible, label, checked_condition)\
    name = WaterType->addAction(label);\
    name->setCheckable(true);\
    name->setEnabled(enable);\
    name->setVisible(visible);\
    name->setChecked(checked_condition); typeID++;

    QAction *envTypes[16];
    int typeID = 0;
    bool enable_new_types = !m_scene->m_data->meta.smbx64strict
                            && (m_scene->m_configs->editor.supported_features.level_phys_ez_new_types == EditorSetup::FeaturesSupport::F_ENABLED);
    bool show_new_types = (m_scene->m_configs->editor.supported_features.level_phys_ez_new_types != EditorSetup::FeaturesSupport::F_HIDDEN);

    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], true, true,                       tr("Water"),        m_data.env_type == LevelPhysEnv::ENV_WATER)
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], true, true,                       tr("Quicksand"),    m_data.env_type == LevelPhysEnv::ENV_QUICKSAND)
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], enable_new_types, show_new_types, tr("Custom liquid"), m_data.env_type == LevelPhysEnv::ENV_CUSTOM_LIQUID)
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], enable_new_types, show_new_types, tr("Gravity Field"), m_data.env_type == LevelPhysEnv::ENV_GRAVITATIONAL_FIELD)
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], enable_new_types, show_new_types, tr("Touch Event (Once)"), m_data.env_type == LevelPhysEnv::ENV_TOUCH_EVENT_ONCE_PLAYER)
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], enable_new_types, show_new_types, tr("Touch Event (Every frame)"), m_data.env_type == LevelPhysEnv::ENV_TOUCH_EVENT_PLAYER)
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], enable_new_types, show_new_types, tr("NPC/Player Touch Event (Once)"), m_data.env_type == LevelPhysEnv::ENV_TOUCH_EVENT_ONCE_NPC)
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], enable_new_types, show_new_types, tr("NPC/Player Touch Event (Every frame)"), m_data.env_type == LevelPhysEnv::ENV_TOUCH_EVENT_NPC)
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], enable_new_types, show_new_types, tr("Mouse click Event"), m_data.env_type == LevelPhysEnv::ENV_CLICK_EVENT)
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], enable_new_types, show_new_types, tr("Collision script"), m_data.env_type == LevelPhysEnv::ENV_COLLISION_SCRIPT)
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], enable_new_types, show_new_types, tr("Mouse click Script"), m_data.env_type == LevelPhysEnv::ENV_CLICK_SCRIPT)
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], enable_new_types, show_new_types, tr("Collision Event"), m_data.env_type == LevelPhysEnv::ENV_COLLISION_EVENT)
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], enable_new_types, show_new_types, tr("Air chamber"), m_data.env_type == LevelPhysEnv::ENV_AIR)
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], enable_new_types, show_new_types, tr("NPC Touch Event (Once)"), m_data.env_type == LevelPhysEnv::ENV_TOUCH_EVENT_ONCE_NPC1)
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], enable_new_types, show_new_types, tr("NPC Touch Event (Every frame)"), m_data.env_type == LevelPhysEnv::ENV_TOUCH_EVENT_NPC1)
    CONTEXT_MENU_ITEM_CHK(envTypes[typeID], enable_new_types, show_new_types, tr("NPC Hurting Field"), m_data.env_type == LevelPhysEnv::ENV_NPC_HURTING_FIELD)

#undef CONTEXT_MENU_ITEM_CHK

    ItemMenu.addSeparator();

    QMenu *copyPreferences =    ItemMenu.addMenu(tr("Copy preferences"));
    QAction *copyArrayID =      copyPreferences->addAction(tr("Array-ID: %1").arg(m_data.meta.array_id));
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


    if(selected == cutWater)
        m_scene->m_mw->on_actionCut_triggered();
    else if(selected == copyWater)
        m_scene->m_mw->on_actionCopy_triggered();
    else if(selected == copyArrayID)
    {
        QApplication::clipboard()->setText(QString("%1").arg(m_data.meta.array_id));
        m_scene->m_mw->showStatusMsg(tr("Preferences have been copied: %1").arg(QApplication::clipboard()->text()));
    }
    else if(selected == copyPosXYWH)
    {
        QApplication::clipboard()->setText(
            QString("X=%1; Y=%2; W=%3; H=%4;")
            .arg(m_data.x)
            .arg(m_data.y)
            .arg(m_data.w)
            .arg(m_data.h)
        );
        m_scene->m_mw->showStatusMsg(tr("Preferences have been copied: %1").arg(QApplication::clipboard()->text()));
    }
    else if(selected == copyPosLTRB)
    {
        QApplication::clipboard()->setText(
            QString("Left=%1; Top=%2; Right=%3; Bottom=%4;")
            .arg(m_data.x)
            .arg(m_data.y)
            .arg(m_data.x + m_data.w)
            .arg(m_data.y + m_data.h)
        );
        m_scene->m_mw->showStatusMsg(tr("Preferences have been copied: %1").arg(QApplication::clipboard()->text()));
    }
    else if(selected == resize)
        m_scene->setPhysEnvResizer(this, true);
    else if(selected == remove)
        m_scene->removeSelectedLvlItems();
    else if(selected == newLayer)
        m_scene->setLayerToSelected();
    else
    {
        bool found = false;
        //Fetch layers menu
        foreach(QAction *lItem, layerItems)
        {
            if(selected == lItem)
            {
                //FOUND!!!
                m_scene->setLayerToSelected(lItem->data().toString());
                found = true;
                break;
            }//Find selected layer's item
        }

        if(!found)
        {
            for(int i = 0; i < typeID; i++)
            {
                if(selected == envTypes[i])
                {
                    LevelData modData;
                    for(QGraphicsItem *selItem : m_scene->selectedItems())
                    {
                        if(selItem->data(ITEM_TYPE).toString() == "Water")
                        {
                            ItemPhysEnv *pe = dynamic_cast<ItemPhysEnv *>(selItem);
                            modData.physez.push_back(pe->m_data);
                            pe->setType(i);
                            m_scene->invalidate(pe->boundingRect());
                        }
                    }
                    m_scene->m_history->addChangeSettings(modData, HistorySettings::SETTING_WATERTYPE, QVariant(true));
                    if(!m_scene->m_opts.animationEnabled)
                        m_scene->update();
                    found = true;
                    break;
                }
            }
        }
    }
}

QPainterPath ItemPhysEnv::shape() const
{
    QPainterPath path;
    QPolygonF lineBoarder;
    qreal points[] =
    {
        0.0,
        0.0,
        this->data(ITEM_WIDTH).toReal(),
        this->data(ITEM_HEIGHT).toReal()
    };

    const int BORDERWIDTH = 12;
    const int PLEFT = 0;
    const int PTOP = 1;
    const int PRIGHT = 2;
    const int PBOTTOM = 3;

    lineBoarder.push_back(QPointF(points[PLEFT],     points[PTOP]));
    lineBoarder.push_back(QPointF(points[PRIGHT],    points[PTOP]));
    lineBoarder.push_back(QPointF(points[PRIGHT],    points[PBOTTOM]));
    lineBoarder.push_back(QPointF(points[PLEFT],     points[PBOTTOM]));
    lineBoarder.push_back(QPointF(points[PLEFT],     points[PTOP]));

    lineBoarder.push_back(QPointF(points[PLEFT] + BORDERWIDTH,  points[PTOP] + BORDERWIDTH));
    lineBoarder.push_back(QPointF(points[PRIGHT] - BORDERWIDTH, points[PTOP] + BORDERWIDTH));
    lineBoarder.push_back(QPointF(points[PRIGHT] - BORDERWIDTH, points[PBOTTOM] - BORDERWIDTH));
    lineBoarder.push_back(QPointF(points[PLEFT] + BORDERWIDTH, points[PBOTTOM] - BORDERWIDTH));
    lineBoarder.push_back(QPointF(points[PLEFT] + BORDERWIDTH,  points[PTOP] + BORDERWIDTH));

    path.addPolygon(lineBoarder);

    return path;
}

///////////////////MainArray functions/////////////////////////////
void ItemPhysEnv::setLayer(QString layer)
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

QString ItemPhysEnv::getLayerName() {
    return m_data.layer;
}

void ItemPhysEnv::arrayApply()
{
    bool found = false;

    m_data.x = qRound(this->scenePos().x());
    m_data.y = qRound(this->scenePos().y());
    this->setData(ITEM_WIDTH, (int)m_data.w);
    this->setData(ITEM_HEIGHT, (int)m_data.h);

    if(m_data.meta.index < (unsigned int)m_scene->m_data->physez.size())
    {
        //Check index
        if(m_data.meta.array_id == m_scene->m_data->physez[(int)m_data.meta.index].meta.array_id)
            found = true;
    }

    //Apply current data in main array
    if(found)
    {
        //directlry
        m_scene->m_data->physez[(int)m_data.meta.index] = m_data; //apply current bgoData
    }
    else
        for(int i = 0; i < m_scene->m_data->physez.size(); i++)
        {
            //after find it into array
            if(m_scene->m_data->physez[i].meta.array_id == m_data.meta.array_id)
            {
                m_data.meta.index = (unsigned int)i;
                m_scene->m_data->physez[i] = m_data;
                break;
            }
        }

    //Mark level as modified
    m_scene->m_data->meta.modified = true;

    //Update R-tree innex
    m_scene->unregisterElement(this);
    m_scene->registerElement(this);
}

void ItemPhysEnv::removeFromArray()
{
    bool found = false;
    if(m_data.meta.index < (unsigned int)m_scene->m_data->physez.size())
    {
        //Check index
        if(m_data.meta.array_id == m_scene->m_data->physez[(int)m_data.meta.index].meta.array_id)
            found = true;
    }

    if(found)
    {
        //directlry
        m_scene->m_data->physez.removeAt((int)m_data.meta.index);
    }
    else
        for(int i = 0; i < m_scene->m_data->physez.size(); i++)
        {
            if(m_scene->m_data->physez[i].meta.array_id == m_data.meta.array_id)
            {
                m_scene->m_data->physez.removeAt(i);
                break;
            }
        }

    //Mark level as modified
    m_scene->m_data->meta.modified = true;
}


void ItemPhysEnv::returnBack()
{
    this->setPos(m_data.x, m_data.y);
}

QPoint ItemPhysEnv::sourcePos()
{
    return QPoint((int)m_data.x, (int)m_data.y);
}

void ItemPhysEnv::updateColor()
{
    switch(m_data.env_type)
    {
    case LevelPhysEnv::ENV_WATER:
    default:
        m_color = QColor(Qt::green);
        break;
    case LevelPhysEnv::ENV_QUICKSAND:
        m_color = QColor(Qt::yellow);
        break;
    case LevelPhysEnv::ENV_CUSTOM_LIQUID:
        m_color = QColor(Qt::darkGreen);
        break;
    case LevelPhysEnv::ENV_AIR:
        m_color = QColor(Qt::blue);
        break;
    case LevelPhysEnv::ENV_GRAVITATIONAL_FIELD:
        m_color = QColor(Qt::cyan);
        break;
    case LevelPhysEnv::ENV_TOUCH_EVENT_ONCE_PLAYER:
        m_color = QColor(Qt::darkRed);
        break;
    case LevelPhysEnv::ENV_TOUCH_EVENT_PLAYER:
        m_color = QColor(Qt::red);
        break;
    case LevelPhysEnv::ENV_TOUCH_EVENT_ONCE_NPC:
        m_color = QColor(Qt::darkMagenta);
        break;
    case LevelPhysEnv::ENV_TOUCH_EVENT_NPC:
        m_color = QColor(Qt::magenta);
        break;
    case LevelPhysEnv::ENV_CLICK_EVENT:
        m_color = QColor(Qt::darkCyan);
        break;
    case LevelPhysEnv::ENV_CLICK_SCRIPT:
        m_color = QColor(Qt::darkCyan);
        break;
    case LevelPhysEnv::ENV_COLLISION_EVENT:
        m_color = QColor(Qt::darkCyan);
        break;
    case LevelPhysEnv::ENV_COLLISION_SCRIPT:
        m_color = QColor(Qt::darkCyan);
        break;
    case LevelPhysEnv::ENV_TOUCH_EVENT_ONCE_NPC1:
        m_color = QColor(Qt::darkGray);
        break;
    case LevelPhysEnv::ENV_TOUCH_EVENT_NPC1:
        m_color = QColor(Qt::gray);
        break;
    case LevelPhysEnv::ENV_NPC_HURTING_FIELD:
        m_color = QColor(Qt::darkYellow);
        break;
    }
    m_pen.setColor(m_color);
    m_pen.setWidth(m_penWidth);
    m_pen.setStyle(Qt::SolidLine);
    m_pen.setCapStyle(Qt::FlatCap);
    m_pen.setJoinStyle(Qt::MiterJoin);

    QColor c(m_color);
    c.setAlpha(25);
    m_brush = QBrush(c);
    c.setAlpha(50);
    m_brushSubBorder = QBrush(c);
}

bool ItemPhysEnv::itemTypeIsLocked()
{
    if(!m_scene)
        return false;
    return m_scene->m_lockPhysenv;
}

void ItemPhysEnv::setType(int tp)
{
    m_data.env_type = tp;
    updateColor();
    arrayApply();
}

void ItemPhysEnv::setRectSize(QRect rect)
{
    QRectF br = this->boundingRect();
    br.setX((int)m_data.x);
    br.setY((int)m_data.y);
    m_data.x = rect.x();
    m_data.y = rect.y();
    m_data.w = rect.width();
    m_data.h = rect.height();
    m_waterSize = rect.size();
    setPos(m_data.x, m_data.y);
    refreshItemSize();
    arrayApply();
    m_scene->invalidate(br);
}

void ItemPhysEnv::setSize(QSize sz)
{
    m_waterSize = sz;
    m_data.w = sz.width();
    m_data.h = sz.height();
    refreshItemSize();
    arrayApply();
}


void ItemPhysEnv::setPhysEnvData(LevelPhysEnv inD)
{
    m_data = inD;
    m_waterSize = QSize((int)m_data.w, (int)m_data.h);
    setPos(m_data.x, m_data.y);
    setData(ITEM_ID, QString::number(0));
    setData(ITEM_ARRAY_ID, QString::number(m_data.meta.array_id));
    updateColor();
    refreshItemSize();
    m_scene->unregisterElement(this);
    m_scene->registerElement(this);
}

void ItemPhysEnv::refreshItemSize()
{
    setData(ITEM_WIDTH, (int)m_data.w);
    setData(ITEM_HEIGHT, (int)m_data.h);
}

QRectF ItemPhysEnv::boundingRect() const
{
    return QRectF(-1, -1,
                  m_waterSize.width() + m_penWidth,
                  m_waterSize.height() + m_penWidth);
}

void ItemPhysEnv::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    long h, w;
    w = m_data.w - m_penWidth;
    h = m_data.h - m_penWidth;

    painter->setBrush(m_brushSubBorder);
    painter->setPen(Qt::NoPen);
    painter->drawPath(shape());

    painter->setBrush(m_brush);
    painter->setPen(m_pen);
    painter->drawRect(1, 1, (int)w, (int)h);

    if(this->isSelected())
    {
        painter->setBrush(Qt::NoBrush);
        painter->setPen(QPen(QBrush(Qt::white), 2, Qt::DotLine));
        painter->drawRect(1, 1, (int)w, (int)h);
    }
}
