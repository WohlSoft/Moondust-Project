/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QIcon>
#include <QMimeData>
#include <functional>

#include <common_features/graphics_funcs.h>
#include <common_features/items.h>

#include "piecesmodel.h"

ElementsListModel::ElementsListModel(DataConfig* conf, ElementType pieceType, int pieceSize, QGraphicsScene *scene, QObject *parent)
    : QAbstractListModel(parent), m_PieceSize(pieceSize), m_conf(conf), m_type(pieceType)
{
    mode = GFX_Staff;
    m_scene = scene;
    if(m_scene != nullptr)
    {
        if(QString(m_scene->metaObject()->className()) == "LvlScene")
            mode = GFX_Level;
        else if(QString(m_scene->metaObject()->className()) == "WldScene")
            mode = GFX_World;
    }
}

QVariant ElementsListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(index.row() >= m_elementsVisibleMap.size())
        return QVariant(); // Gone out of range!

    int id = m_elementsVisibleMap[index.row()];
    const Element &e = m_elements[id];

    if (role == Qt::DecorationRole)
        return QIcon(e.pixmap.scaled(m_PieceSize, m_PieceSize,
                        Qt::KeepAspectRatio, Qt::SmoothTransformation));
    else if (role == Qt::DisplayRole)
        return e.name;
    else if (role == Qt::ToolTipRole)
        return e.description;
    else if (role == Qt::UserRole)
        return e.pixmap;
    else if (role == Qt::UserRole + 1)
        return static_cast<qulonglong>(e.elementId);

    return QVariant();
}

void ElementsListModel::setScene(QGraphicsScene *scene)
{
    m_scene = scene;
}

void ElementsListModel::setElementsType(ElementsListModel::ElementType elementType)
{
    clear();
    m_type = elementType;
}

void ElementsListModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, m_elements.size());
    m_elementsVisibleMap.clear();
    m_elements.clear();
    endRemoveRows();
}

void ElementsListModel::addElementsBegin()
{
    beginInsertRows(QModelIndex(), m_elements.size(), m_elements.size());
}

void ElementsListModel::addElementsEnd()
{
    endInsertRows();
    updateVisibilityMap();
    updateSort();
}

void ElementsListModel::addElement(const int &index)
{
    LvlScene * scene_lvl = dynamic_cast<LvlScene*>(m_scene);
    WldScene * scene_wld = dynamic_cast<WldScene*>(m_scene);

    Element element;

    if(m_type == LEVELPIECE_BLOCK)
    {
        obj_block& block = scene_lvl ? scene_lvl->m_localConfigBlocks[index] : m_conf->main_block[index];
        element.elementId = static_cast<qulonglong>(block.setup.id);
        element.name      = block.setup.name.isEmpty() ? QString("%1").arg(index) : block.setup.name;
        element.description = Items::getTilesetToolTip(m_type, element.elementId, m_scene);
        Items::getItemGFX(&block, element.pixmap, false, QSize(32,32));
        element.isValid = true;
        element.isVisible = true;
    }
    else
    if(m_type == LEVELPIECE_BGO)
    {
        obj_bgo& bgo = scene_lvl ? scene_lvl->m_localConfigBGOs[index] : m_conf->main_bgo[index];
        element.elementId = static_cast<qulonglong>(bgo.setup.id);
        element.name      = bgo.setup.name.isEmpty() ? QString("%1").arg(index) : bgo.setup.name;
        element.description = Items::getTilesetToolTip(m_type, element.elementId, m_scene);
        Items::getItemGFX(&bgo, element.pixmap, false, QSize(32,32));
        element.isValid = true;
        element.isVisible = true;
    }
    else
    if(m_type == LEVELPIECE_NPC)
    {
        obj_npc& npc = scene_lvl ? scene_lvl->m_localConfigNPCs[index] : m_conf->main_npc[index];
        element.elementId = static_cast<qulonglong>(npc.setup.id);
        element.name      = npc.setup.name.isEmpty() ? QString("%1").arg(index) : npc.setup.name;
        element.description = Items::getTilesetToolTip(m_type, element.elementId, m_scene);
        Items::getItemGFX(&npc, element.pixmap, false, QSize(32,32));
        element.isValid = true;
        element.isVisible = true;
    }
    else
    if(m_type == WORLDPIECE_TILE)
    {
        obj_w_tile& tile = scene_wld ? scene_wld->m_localConfigTerrain[index] : m_conf->main_wtiles[index];
        element.elementId = static_cast<qulonglong>(tile.setup.id);
        element.name      = tile.setup.name.isEmpty() ? QString("%1").arg(index) : tile.setup.name;
        element.description = Items::getTilesetToolTip(m_type, element.elementId, m_scene);
        Items::getItemGFX(&tile, element.pixmap, false, QSize(32,32));
        element.isValid = true;
        element.isVisible = true;
    }
    else
    if(m_type == WORLDPIECE_SCENERY)
    {
        obj_w_scenery& scenery = scene_wld ? scene_wld->m_localConfigScenery[index] : m_conf->main_wscene[index];
        element.elementId = static_cast<qulonglong>(scenery.setup.id);
        element.name      = scenery.setup.name.isEmpty() ? QString("%1").arg(index) : scenery.setup.name;
        element.description = Items::getTilesetToolTip(m_type, element.elementId, m_scene);
        Items::getItemGFX(&scenery, element.pixmap, false, QSize(32,32));
        element.isValid = true;
        element.isVisible = true;
    }
    else
    if(m_type == WORLDPIECE_PATH)
    {
        obj_w_path& wpath = scene_wld ? scene_wld->m_localConfigPaths[index] : m_conf->main_wpaths[index];
        element.elementId = static_cast<qulonglong>(wpath.setup.id);
        element.name      = wpath.setup.name.isEmpty() ? QString("%1").arg(index) : wpath.setup.name;
        element.description = Items::getTilesetToolTip(m_type, element.elementId, m_scene);
        Items::getItemGFX(&wpath, element.pixmap, false, QSize(32,32));
        element.isValid = true;
        element.isVisible = true;
    }
    else
    if(m_type == WORLDPIECE_LEVEL)
    {
        obj_w_level& wlevel = scene_wld ? scene_wld->m_localConfigLevels[index] : m_conf->main_wlevels[index];
        element.elementId = static_cast<qulonglong>(wlevel.setup.id);
        element.name      = wlevel.setup.name.isEmpty() ? QString("%1").arg(index) : wlevel.setup.name;
        element.description = Items::getTilesetToolTip(m_type, element.elementId, m_scene);
        Items::getItemGFX(&wlevel, element.pixmap, false, QSize(32,32));
        element.isValid = true;
        element.isVisible = true;
    }

    if(element.isValid)
        m_elements.push_back(element);
}

void ElementsListModel::setFilter(const QString &criteria, int searchType)
{
    if(m_filterCriteria != criteria)
        m_filterCriteria = criteria;
    if(m_filterSearchType != searchType)
        m_filterSearchType = searchType;

    if(criteria.isEmpty())
    {
        for(Element &e : m_elements)
            e.isVisible = true;
        beginInsertRows(QModelIndex(), m_elementsVisibleMap.size() - 1, m_elements.size() - 1);
        updateVisibilityMap();
        endInsertRows();
        return;
    }

    int oldVisiblesCount = m_elementsVisibleMap.size();
    int newVisiblesCount = 0;

    for(int i = 0; i < m_elements.size(); ++i)
    {
        Element &e = m_elements[i];
        switch(searchType)
        {
        default:
        case Search_ByName:
            e.isVisible = e.name.contains(criteria, Qt::CaseInsensitive);
            break;
        case Search_ById:
            e.isVisible = e.elementId == criteria.toULongLong();
            break;
        case Search_ByIdContained:
            e.isVisible = QString("%1").arg(e.elementId).contains(criteria);
            break;
        }
        if(e.isVisible)
            ++newVisiblesCount;
    }

    if(oldVisiblesCount < newVisiblesCount)
    {
        beginRemoveRows(QModelIndex(), newVisiblesCount - 1, oldVisiblesCount - 1);
        updateVisibilityMap();
        endRemoveRows();
    }
    else if(oldVisiblesCount > newVisiblesCount)
    {
        beginInsertRows(QModelIndex(), newVisiblesCount - 1, oldVisiblesCount - 1);
        updateVisibilityMap();
        endInsertRows();
    }
    else
    {
        beginResetModel();
        updateVisibilityMap();
        endResetModel();
    }
}

void ElementsListModel::setSort(int sortType, bool backward)
{
    m_sortType = sortType;
    m_sortBackward = backward;
    updateSort();
}

QString ElementsListModel::getMimeType() const
{
    switch (m_type)
    {
        case LEVELPIECE_BLOCK:      return QString("text/x-pge-piece-block");
        case LEVELPIECE_BGO:        return QString("text/x-pge-piece-bgo");
        case LEVELPIECE_NPC:        return QString("text/x-pge-piece-npc");
        case WORLDPIECE_TILE:       return QString("text/x-pge-piece-tile");
        case WORLDPIECE_SCENERY:    return QString("text/x-pge-piece-scenery");
        case WORLDPIECE_PATH:       return QString("text/x-pge-piece-path");
        case WORLDPIECE_LEVEL:      return QString("text/x-pge-piece-level");
        default: break;
    }
    return QString("image/x-pge-piece");
}

void ElementsListModel::updateVisibilityMap()
{
    m_elementsVisibleMap.clear();
    m_elementsVisibleMap.reserve(m_elements.size());
    for(int i = 0; i < m_elements.size(); ++i)
    {
        if(m_elements[i].isVisible)
            m_elementsVisibleMap.push_back(i);
    }
}

void ElementsListModel::updateSort()
{
    beginResetModel();
    switch(m_sortType)
    {
    default:
    case Sort_ByName:
        if(m_sortBackward)
        {
            std::sort(m_elements.begin(), m_elements.end(),
            [](const Element &a, const Element &b)
            {
                return a.name.compare(b.name, Qt::CaseInsensitive) > 0;
            });
        }
        else
        {
            std::sort(m_elements.begin(), m_elements.end(),
            [](const Element &a, const Element &b)
            {
                return a.name.compare(b.name, Qt::CaseInsensitive) < 0;
            });
        }
        break;

    case Sort_ById:
        if(m_sortBackward)
        {
            std::sort(m_elements.begin(), m_elements.end(),
            [](const Element &a, const Element &b)
            {
                return a.elementId > b.elementId;
            });
        }
        else
        {
            std::sort(m_elements.begin(), m_elements.end(),
            [](const Element &a, const Element &b)
            {
                return a.elementId < b.elementId;
            });
        }
        break;
    }

    // Also update visibility map to keep matching
    updateVisibilityMap();
    endResetModel();
}

Qt::ItemFlags ElementsListModel::flags(const QModelIndex &index) const
{
    if (index.isValid())
        return (QAbstractListModel::flags(index)|Qt::ItemIsDragEnabled);
    return Qt::ItemIsDropEnabled;
}

QStringList ElementsListModel::mimeTypes() const
{
    QStringList types;
    types << getMimeType().toStdString().c_str();
    return types;
}

QMimeData *ElementsListModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    foreach (QModelIndex index, indexes) {
        if (index.isValid()) {
            int id = data(index, Qt::UserRole+1).toInt();
            stream << id;
        }
    }

    mimeData->setData(getMimeType(), encodedData);
    return mimeData;
}

bool ElementsListModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                               int /*row*/, int column, const QModelIndex &parent)
{
    Q_UNUSED(parent);

    if (!data->hasFormat(getMimeType()))
        return false;

    if (action == Qt::IgnoreAction)
        return true;

    if (column > 0)
        return false;

//    int endRow;

//    if (!parent.isValid()) {
//        if (row < 0)
//            endRow = pixmaps.size();
//        else
//            endRow = qMin(row, pixmaps.size());
//    } else {
//        endRow = parent.row();
//    }

//    QByteArray encodedData = data->data("image/x-pge-piece");
//    QDataStream stream(&encodedData, QIODevice::ReadOnly);

//    while (!stream.atEnd()) {
//        QPixmap pixmap;
//        QPoint location;
//        stream >> pixmap >> location;

//        beginInsertRows(QModelIndex(), endRow, endRow);
//        pixmaps.insert(endRow, pixmap);
//        endInsertRows();

//        ++endRow;
//    }

    return false;
}

int ElementsListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    else
        return m_elementsVisibleMap.size();
}

Qt::DropActions ElementsListModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}
