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

#include <QLineEdit>
#include <QListView>
#include <QMenu>

#include "itembox_list_model.h"

void makeFilterSetupMenu(QMenu &menu, ItemBoxListModel *model, QLineEdit *search, QListView *view, bool hasUniformView)
{
    QAction *searchByName = menu.addAction(ItemBoxListModel::tr("Search by Name", "Element search criteria"));
    searchByName->setCheckable(true);
    searchByName->setChecked(true);

    QAction *searchById = menu.addAction(ItemBoxListModel::tr("Search by ID", "Element search criteria"));
    searchById->setCheckable(true);

    QAction *searchByIdContained = menu.addAction(ItemBoxListModel::tr("Search by ID (Contained)", "Element search criteria"));
    searchByIdContained->setCheckable(true);

    menu.addSeparator();
    QMenu   *sortBy = menu.addMenu(ItemBoxListModel::tr("Sort by", "Search settings pop-up menu, sort submenu"));

    QAction *sortByName = sortBy->addAction(ItemBoxListModel::tr("Name", "Sort by name"));
    sortByName->setCheckable(true);
    sortByName->setChecked(true);

    QAction *sortById   = sortBy->addAction(ItemBoxListModel::tr("ID", "Sort by ID"));
    sortById->setCheckable(true);

    sortBy->addSeparator();
    QAction *sortDesc = sortBy->addAction(ItemBoxListModel::tr("Descending", "Descending sorting order"));
    sortDesc->setCheckable(true);

    QAction *uniformView = nullptr;
    if(hasUniformView && view)
    {
        menu.addSeparator();
        uniformView = menu.addAction(ItemBoxListModel::tr("Uniform item sizes view", "Align elements inside of Item Box list in uniform view"));
        uniformView->setCheckable(true);
    }

    QAction *showCustomOnly = menu.addAction(ItemBoxListModel::tr("Show custom elements", "Show custom elements only in Item Box List"));
    showCustomOnly->setCheckable(true);

    QAction *showOriginOnly = menu.addAction(ItemBoxListModel::tr("Show standard elements", "Show standard elements only in Item Box List"));
    showOriginOnly->setCheckable(true);

    menu.connect(searchByName, &QAction::triggered,
    [=](bool)
    {
        searchByName->setChecked(true);
        searchById->setChecked(false);
        searchByIdContained->setChecked(false);
        model->setFilterSearchType(ItemBoxListModel::Search_ByName);
        search->clear();
    });

    menu.connect(searchById, &QAction::triggered,
    [=](bool)
    {
        searchByName->setChecked(false);
        searchById->setChecked(true);
        searchByIdContained->setChecked(false);
        model->setFilterSearchType(ItemBoxListModel::Search_ById);
        search->clear();
    });

    menu.connect(searchByIdContained, &QAction::triggered,
    [=](bool)
    {
        searchByName->setChecked(false);
        searchById->setChecked(false);
        searchByIdContained->setChecked(true);
        model->setFilterSearchType(ItemBoxListModel::Search_ByIdContained);
        search->clear();
    });

    menu.connect(sortByName, &QAction::triggered,
    [=](bool)
    {
        sortByName->setChecked(true);
        sortById->setChecked(false);
        model->setSort(ItemBoxListModel::Sort_ByName, sortDesc->isChecked());
    });

    menu.connect(sortById, &QAction::triggered,
    [=](bool)
    {
        sortByName->setChecked(false);
        sortById->setChecked(true);
        model->setSort(ItemBoxListModel::Sort_ById, sortDesc->isChecked());
    });

    menu.connect(sortDesc, &QAction::triggered,
    [=](bool)
    {
        model->setSort(sortByName->isChecked() ?
                             ItemBoxListModel::Sort_ByName :
                             ItemBoxListModel::Sort_ById,
                         sortDesc->isChecked());
    });

    if(hasUniformView && view)
    {
        menu.connect(uniformView, &QAction::triggered,
        [=](bool uniformView)
        {
            view->setUniformItemSizes(uniformView);
            view->adjustSize();
        });
    }

    menu.connect(showCustomOnly, &QAction::triggered,
    [=](bool filter)
    {
        showOriginOnly->setChecked(false);
        model->setCustomOnlyFilter(filter);
    });

    menu.connect(showOriginOnly, &QAction::triggered,
    [=](bool filter)
    {
        showCustomOnly->setChecked(false);
        model->setOriginsOnlyFilter(filter);
    });

    if(search)
    {
        search->connect(search, &QLineEdit::textChanged,
        [=](const QString &arg)
        {
            model->setFilterCriteria(arg);
        });
    }
}


ItemBoxListModel::ItemBoxListModel(QObject *parent)
    : QAbstractListModel(parent)
{}


int ItemBoxListModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;
    if(m_isTable)
        return m_tableHeight;
    else
        return m_elementsVisibleMap.size();
}

int ItemBoxListModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid() || !m_isTable)
        return 1;
    return m_tableWidth;
}

QVariant ItemBoxListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    int idx = 0;
    if(m_isTable)
    {
        if(index.row() >= m_tableHeight)
            return QVariant();
        if(index.column() >= m_tableWidth)
            return QVariant();
        idx = tableCordToIdx(index.row(), index.column());
        if(idx >= m_elementsVisibleMap.size())
            return QVariant(); // Gone out of range!
    }
    else
    {
        idx = index.row();
        if(idx >= m_elementsVisibleMap.size())
            return QVariant(); // Gone out of range!
    }

    int id = m_elementsVisibleMap[idx];
    const Element &e = m_elements[id];

    if (role == Qt::DecorationRole)
        return QIcon(e.pixmap);
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

QModelIndex ItemBoxListModel::findVisibleItemById(qulonglong id) const
{
    for(int i = 0; i < m_elementsVisibleMap.size(); i++)
    {
        const Element &e = m_elements[m_elementsVisibleMap[i]];
        if(e.elementId == id)
        {
            if(m_isTable)
                return createIndex(i % m_tableWidth, (i / m_tableWidth), nullptr);
            return createIndex(i, 0, nullptr);
        }
    }
    return QModelIndex();
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

void ItemBoxListModel::resetFilters()
{
    m_filterCriteria.clear();
    m_filterCustomOnly = false;
    m_filterOriginsOnly = false;
    m_filterCategory = -1;
    m_filterGroup = -1;
    updateFilter();
}

void ItemBoxListModel::setTableMode(bool isTable, int w, int h)
{
    m_isTable = isTable;
    m_tableWidth = w;
    m_tableHeight = h;
}

void ItemBoxListModel::addElementsBegin(int allocate)
{
    beginInsertRows(QModelIndex(), m_elements.size(), m_elements.size() + allocate);
    if(allocate < 0)
        m_elements.reserve(allocate);
}

void ItemBoxListModel::addElementsEnd()
{
    endInsertRows();
    beginResetModel();
    updateVisibilityMap();
    endResetModel();
    updateSort();
}


QStringList ItemBoxListModel::getCategoriesList(const QString &allField)
{
    QStringList items;
    items.push_back(allField);

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
    if(category.isEmpty() || (m_filterCategoryAllKey == category))
        m_filterCategory = -1;
    else
        m_filterCategory = getCategory(category);
    updateFilter();
}

void ItemBoxListModel::setCategoryAllKey(const QString &category)
{
    m_filterCategoryAllKey = category;
}

void ItemBoxListModel::setGroupFilter(const QString &group)
{
    if(group.isEmpty() || (m_filterGroupAllKey == group))
        m_filterGroup = -1;
    else
        m_filterGroup = getGroup(group);
    m_filterCategory = -1;
    updateFilter();
}

void ItemBoxListModel::setGroupAllKey(const QString &group)
{
    m_filterGroupAllKey = group;
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
        e.isVisible = isElementVisible(e);

        m_elements.push_back(e);
    }
}

void ItemBoxListModel::addElementCell(int x, int y, const ItemBoxListModel::Element &element, const QString &group, const QString &category)
{
    Element e = element;
    if(element.isValid)
    {
        int idx = tableCordToIdx(x, y);
        if(idx >= m_elements.size())
            m_elements.resize(idx + 1);

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
        e.isVisible = isElementVisible(e);
        m_elements[idx] = e;
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

void ItemBoxListModel::setFilterCriteria(const QString &criteria)
{
    if(m_filterCriteria != criteria)
        m_filterCriteria = criteria;
    updateFilter();
}

void ItemBoxListModel::setFilterSearchType(int searchType)
{
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

void ItemBoxListModel::setSortSkipFirst(bool e)
{
    m_sortSkipFirst = e;
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

int ItemBoxListModel::tableCordToIdx(int x, int y) const
{
    return (y * m_tableWidth) + x;
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
        e.isVisible = isElementVisible(e);
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

bool ItemBoxListModel::isElementVisible(const ItemBoxListModel::Element &e)
{
    bool visible = true;

    if(m_isTable)
        return true; //In table mode is always visible!

    if(!m_filterCriteria.isEmpty())
    {
        switch(m_filterSearchType)
        {
        default:
        case Search_ByName:
            visible = e.name.contains(m_filterCriteria, Qt::CaseInsensitive);
            break;
        case Search_ById:
            visible = e.elementId == m_filterCriteria.toULongLong();
            break;
        case Search_ByIdContained:
            visible = QString("%1").arg(e.elementId).contains(m_filterCriteria);
            break;
        }
    }

    if(visible)
    {
        if((m_filterCategory >= 0) && (m_filterGroup >= 0))
            visible = (e.groupId == m_filterGroup) && (e.categoryId == m_filterCategory);
        else if(m_filterCategory >= 0)
            visible = (e.categoryId == m_filterCategory);
        else if(m_filterGroup >= 0)
            visible = (e.groupId == m_filterGroup);
    }

    if(visible)
    {
        if(m_filterCustomOnly)
            visible = e.isCustom;
        if(m_filterOriginsOnly)
            visible = !e.isCustom;
    }

    return visible;
}

void ItemBoxListModel::updateVisibilityMap()
{
    m_elementsVisibleMap.clear();
    m_elementsVisibleMap.reserve(m_elements.size());
    for(int i = 0; i < m_elements.size(); ++i)
    {
        if(m_elements[i].isVisible || m_isTable)
            m_elementsVisibleMap.push_back(i);
    }
}

void ItemBoxListModel::updateSort()
{
    if(m_isTable)
        return;//Avoid resorting in table mode

    beginResetModel();
    switch(m_sortType)
    {
    default:
    case Sort_ByName:
        if(m_sortBackward)
        {
            std::sort(m_elements.begin() + (m_sortSkipFirst ? 1 : 0), m_elements.end(),
            [](const Element &a, const Element &b)
            {
                return a.name.compare(b.name, Qt::CaseInsensitive) > 0;
            });
        }
        else
        {
            std::sort(m_elements.begin() + (m_sortSkipFirst ? 1 : 0), m_elements.end(),
            [](const Element &a, const Element &b)
            {
                return a.name.compare(b.name, Qt::CaseInsensitive) < 0;
            });
        }
        break;

    case Sort_ById:
        if(m_sortBackward)
        {
            std::sort(m_elements.begin() + (m_sortSkipFirst ? 1 : 0), m_elements.end(),
            [](const Element &a, const Element &b)
            {
                return a.elementId > b.elementId;
            });
        }
        else
        {
            std::sort(m_elements.begin() + (m_sortSkipFirst ? 1 : 0), m_elements.end(),
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
