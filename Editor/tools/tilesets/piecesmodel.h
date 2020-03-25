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

#pragma once
#ifndef PIECESLIST_H
#define PIECESLIST_H

#include <QAbstractListModel>
#include <QList>
#include <QPixmap>
#include <QPoint>
#include <QStringList>
#include <QGraphicsScene>

#include <data_configs/data_configs.h>

QT_BEGIN_NAMESPACE
class QMimeData;
QT_END_NAMESPACE

class ElementsListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum ElementType{
        LEVELPIECE_BLOCK = ItemTypes::LVL_Block,
        LEVELPIECE_BGO = ItemTypes::LVL_BGO,
        LEVELPIECE_NPC = ItemTypes::LVL_NPC,
        WORLDPIECE_TILE = ItemTypes::WLD_Tile,
        WORLDPIECE_SCENERY = ItemTypes::WLD_Scenery,
        WORLDPIECE_PATH = ItemTypes::WLD_Path,
        WORLDPIECE_LEVEL = ItemTypes::WLD_Level
    };

    enum GFXMode
    {
        GFX_Staff = 0,
        GFX_Level,
        GFX_World
    };

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

    explicit ElementsListModel(DataConfig* conf, ElementType pieceType, int pieceSize = 32, QGraphicsScene *scene=0, QObject *parent = 0);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                      int row, int column, const QModelIndex &parent);
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    QStringList mimeTypes() const;
    int rowCount(const QModelIndex &parent) const;
    Qt::DropActions supportedDropActions() const;

    void setScene(QGraphicsScene *scene);
    void setElementsType(ElementType elementType);

    void clear();

    void addElementsBegin();
    void addElement(const int &id);
    void addElementsEnd();

    void setFilter(const QString &criteria, int searchType = 0);
    void setSort(int sortType = 0, bool backward = false);

private:
    GFXMode mode;
    QGraphicsScene *m_scene;

    QString getMimeType() const;
    struct Element
    {
        bool isValid = false;
        qulonglong elementId = 0;
        QString name;
        QString description;
        QPixmap pixmap;
        bool isVisible = false;
    };

    QList<Element>  m_elements;
    QList<int>      m_elementsVisibleMap;

    QString m_filterCriteria;
    int     m_filterSearchType = Search_ByName;
    void    updateVisibilityMap();

    int     m_sortType = Sort_ByName;
    bool    m_sortBackward = false;
    void    updateSort();

    int m_PieceSize;
    DataConfig* m_conf;
    ElementType m_type;
};

#endif // PIECESLIST_H
