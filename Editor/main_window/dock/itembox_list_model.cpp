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

#include <QIcon>
#include <functional>

#include "itembox_list_model.h"


ItemBoxListModel::ItemBoxListModel(QObject *parent)
    : QAbstractListModel(parent)
{}


int ItemBoxListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    else
        return m_elementsVisibleMap.size();
}

QVariant ItemBoxListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(index.row() >= m_elementsVisibleMap.size())
        return QVariant(); // Gone out of range!

    int id = m_elementsVisibleMap[index.row()];
    const Element &e = m_elements[id];

    if (role == Qt::DecorationRole)
        return QIcon(e.pixmap.scaled(m_itemSize, m_itemSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    else if (role == Qt::DisplayRole)
        return e.name;
    else if (role == Qt::ToolTipRole)
        return e.description;
    else if (role == ItemBox_ItemPixmap)
        return e.pixmap;
    else if (role == ItemBox_ItemId)
        return static_cast<qulonglong>(e.elementId);

    return QVariant();
}

Qt::ItemFlags ItemBoxListModel::flags(const QModelIndex &index) const
{
    if (index.isValid())
        return (QAbstractListModel::flags(index)|Qt::ItemIsEnabled|Qt::ItemIsSelectable);
    return Qt::ItemIsEnabled|Qt::ItemIsSelectable;
}

void ItemBoxListModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, m_elements.size());
    m_groupCategories.clear();
    m_groupsMap.clear();
    m_categoriesMap.clear();
    m_filterCategory = -1;
    m_filterGroup = -1;
    m_elementsVisibleMap.clear();
    m_elements.clear();
    endRemoveRows();
}

void ItemBoxListModel::addElementsBegin()
{
    beginInsertRows(QModelIndex(), m_elements.size(), m_elements.size());
}

void ItemBoxListModel::addElementsEnd()
{
    endInsertRows();
    updateVisibilityMap();
    updateSort();
}

QStringList ItemBoxListModel::getCategoriesList(const QString &allField, const QString &customField)
{
    QStringList items;
    items.push_back(allField);
    items.push_back(customField);

    if(m_filterGroup == -1)
    {
        for(SIMap::iterator i = m_categoriesMap.begin(); i != m_categoriesMap.end(); i++)
            items.push_back(i.key());
    }
    else
    {
        QSet<QString> &l = m_groupCategories[m_groupsMap.key(m_filterGroup)];
        for(QSet<QString>::iterator i = l.begin(); i != l.end(); i++)
            items.push_back(*i);
    }
    return items;
}

QStringList ItemBoxListModel::getGroupsList(const QString &allField)
{
    QStringList items;
    items.push_back(allField);
    for(SIMap::iterator i = m_groupsMap.begin(); i != m_groupsMap.end(); i++)
        items.push_back(i.key());
    return items;
}

void ItemBoxListModel::setCategoryFilter(const QString &category)
{
    if(category.isEmpty())
        m_filterCategory = -1;
    else
        m_filterCategory = getCategory(category);
    updateFilter();
}

void ItemBoxListModel::setGroupFilter(const QString &group)
{
    if(group.isEmpty())
        m_filterGroup = -1;
    else
        m_filterGroup = getGroup(group);
    m_filterCategory = -1;
    updateFilter();
}

void ItemBoxListModel::setOriginsOnlyFilter(bool origs)
{
    m_filterOriginsOnly = origs;
    if(origs)
        m_filterCustomOnly = false;
    updateFilter();
}

void ItemBoxListModel::setCustomOnlyFilter(bool custom)
{
    m_filterCustomOnly = custom;
    if(custom)
        m_filterOriginsOnly  = false;
    updateFilter();
}

void ItemBoxListModel::addElement(const ItemBoxListModel::Element &element, const QString &group, const QString &category)
{
    Element e = element;
    if(element.isValid)
    {
        if(!group.isEmpty())
            e.groupId = getGroup(group);
        if(!category.isEmpty())
            e.categoryId = getCategory(category);
        if(!group.isEmpty() && !category.isEmpty())
        {
            QSet<QString> &l = m_groupCategories[group];
            if(!l.contains(category))
                l.insert(category);
        }

        m_elements.push_back(element);
    }
}

void ItemBoxListModel::setFilter(const QString &criteria, int searchType)
{
    if(m_filterCriteria != criteria)
        m_filterCriteria = criteria;
    if(m_filterSearchType != searchType)
        m_filterSearchType = searchType;
    updateFilter();
}

void ItemBoxListModel::setSort(int sortType, bool backward)
{
    m_sortType = sortType;
    m_sortBackward = backward;
    updateSort();
}

int ItemBoxListModel::getGroup(const QString &group)
{
    if(m_groupsMap.contains(group))
        return m_groupsMap[group];
    int ret = m_groupsMap.size();
    m_groupsMap.insert(group, ret);
    return ret;
}

int ItemBoxListModel::getCategory(const QString &category)
{
    if(m_categoriesMap.contains(category))
        return m_categoriesMap[category];
    int ret = m_categoriesMap.size();
    m_categoriesMap.insert(category, ret);
    return ret;
}

void ItemBoxListModel::updateFilter()
{
    if(m_filterCriteria.isEmpty() &&
       m_filterCategory < 0 &&
       m_filterGroup < 0 &&
       !m_filterCustomOnly &&
       !m_filterOriginsOnly)
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

        e.isVisible = true;
        if(!m_filterCriteria.isEmpty())
        {
            switch(m_filterSearchType)
            {
            default:
            case Search_ByName:
                e.isVisible = e.name.contains(m_filterCriteria, Qt::CaseInsensitive);
                break;
            case Search_ById:
                e.isVisible = e.elementId == m_filterCriteria.toULongLong();
                break;
            case Search_ByIdContained:
                e.isVisible = QString("%1").arg(e.elementId).contains(m_filterCriteria);
                break;
            }
        }

        if(e.isVisible)
        {
            if((m_filterCategory >= 0) && (m_filterGroup >= 0))
                e.isVisible = (e.categoryId == m_filterCategory) && (e.groupId == m_filterGroup);
            else if(m_filterCategory >= 0)
                e.isVisible = (e.categoryId == m_filterCategory);
            else if(m_filterGroup >= 0)
                e.isVisible = (e.groupId == m_filterGroup);
        }

        if(e.isVisible)
        {
            if(m_filterCustomOnly)
                e.isVisible = e.isCustom;
            if(m_filterOriginsOnly)
                e.isVisible = !e.isCustom;
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

void ItemBoxListModel::updateVisibilityMap()
{
    m_elementsVisibleMap.clear();
    m_elementsVisibleMap.reserve(m_elements.size());
    for(int i = 0; i < m_elements.size(); ++i)
    {
        if(m_elements[i].isVisible)
            m_elementsVisibleMap.push_back(i);
    }
}

void ItemBoxListModel::updateSort()
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
