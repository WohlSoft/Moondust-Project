/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef ITEMBOXLISTMODEL_H
#define ITEMBOXLISTMODEL_H

#include <QAbstractListModel>
#include <QMap>
#include <QSet>
#include <QStringList>

#include <QMenu>

class QLineEdit;
class QListView;
class ItemBoxListModel;
/**
 * @brief Create the filter setup menu
 * @param menu Destinition menu
 * @param model Elements container model
 * @param search Filter search field
 * @param view Elements view box
 * @param hasUniformView Also add the "Uniform grid view" menu item
 */
extern void makeFilterSetupMenu(QMenu &menu,
                                ItemBoxListModel *model,
                                QLineEdit *search,
                                QListView *view = nullptr,
                                bool hasUniformView = false);

class ItemBoxListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum SearchType
    {
        Search_ByName = 0,
        Search_ById,
        Search_ByIdContained
    };

    enum SortType
    {
        Sort_ByName,
        Sort_ById
    };

    enum
    {
        ItemBox_ItemPixmap = Qt::UserRole,
        ItemBox_ItemId     = Qt::UserRole + 1,
        ItemBox_ItemIsValid = Qt::UserRole + 2,
    };

    struct Element
    {
        bool isValid = false;
        qulonglong elementId = 0;
        QString name;
        QString description;
        QPixmap pixmap;
        bool isVisible = false;
        bool isCustom = false;
        int categoryId = -1;
        int groupId = -1;
    };

    explicit ItemBoxListModel(QObject *parent = nullptr);
    virtual ~ItemBoxListModel() override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QModelIndex findVisibleItemById(qulonglong id) const;

    void clear();
    void resetFilters();

    void setTableMode(bool isTable, int w = 0, int h = 0);

    void addElementsBegin(int allocate = 0);
    void addElement(const Element &element, const QString &group = "", const QString &category = "");
    void addElementsEnd();

    void addElementCell(int x, int y, const Element &element, const QString &group = "", const QString &category = "");

    QStringList getCategoriesList(const QString &allField);
    QStringList getGroupsList(const QString &allField);

    void setCategoryFilter(const QString &category);
    void setCategoryAllKey(const QString &category);
    void setGroupFilter(const QString &group);
    void setGroupAllKey(const QString &group);
    void setOriginsOnlyFilter(bool origs);
    void setCustomOnlyFilter(bool custom);

    void setFilter(const QString &criteria, int searchType = 0);
    void setFilterCriteria(const QString &criteria);
    void setFilterSearchType(int searchType);

    void setSort(int sortType = 0, bool backward = false);
    void setSortSkipFirst(bool e);

    int  getGroup(const QString &group);
    int  getCategory(const QString &category);

    void setShowLabels(bool show);

private:
    bool m_isTable = false;
    int  m_tableWidth  = 0;
    int  m_tableHeight = 0;
    bool m_showLabels = true;

    int  tableCordToIdx(int x, int y) const;

    QVector<Element>  m_elements;
    QVector<int>      m_elementsVisibleMap;

    typedef QMap<QString, int> SIMap;
    SIMap m_categoriesMap;
    SIMap m_groupsMap;
    QMap<QString, QSet<QString > > m_groupCategories;

    QString m_filterCriteria;
    int     m_filterSearchType = Search_ByName;
    int     m_filterCategory = -1;
    QString m_filterCategoryAllKey;
    int     m_filterGroup = -1;
    QString m_filterGroupAllKey;
    bool    m_filterCustomOnly = false;
    bool    m_filterOriginsOnly = false;
    void    updateFilter();
    bool    isElementVisible(const Element &e);

    void    updateVisibilityMap();

    int     m_sortType = Sort_ByName;
    bool    m_sortBackward = false;
    bool    m_sortSkipFirst = false;
    void    updateSort();
};

#endif // ITEMBOXLISTMODEL_H
