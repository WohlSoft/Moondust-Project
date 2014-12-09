/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef PIECESLIST_H
#define PIECESLIST_H

#include <QAbstractListModel>
#include <QList>
#include <QPixmap>
#include <QPoint>
#include <QStringList>
#include "../../data_configs/data_configs.h"

QT_BEGIN_NAMESPACE
class QMimeData;
QT_END_NAMESPACE

class PiecesModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum PieceType{
        LEVELPIECE_BLOCK = 0,
        LEVELPIECE_BGO,
        LEVELPIECE_NPC,
        WORLDPIECE_TILE,
        WORLDPIECE_SCENERY,
        WORLDPIECE_PATH,
        WORLDPIECE_LEVEL
    };

    enum GFXMode
    {
        GFX_Staff=0,
        GFX_Level,
        GFX_World
    };


    explicit PiecesModel(dataconfigs* conf, PieceType pieceType, int pieceSize = 32, QGraphicsScene *scene=0, QObject *parent = 0);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    bool removeRows(int row, int count, const QModelIndex &parent);

    bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                      int row, int column, const QModelIndex &parent);
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    QStringList mimeTypes() const;
    int rowCount(const QModelIndex &parent) const;
    Qt::DropActions supportedDropActions() const;

    void addPiece(const int &id);


private:

    GFXMode mode;
    QGraphicsScene *scn;

    QString getMimeType() const;
    QList<QPixmap> pixmaps;
    QList<QString> pixmapNames;
    QList<int> pixmapId;
    int m_PieceSize;
    dataconfigs* m_conf;
    PieceType m_type;
};

#endif // PIECESLIST_H
