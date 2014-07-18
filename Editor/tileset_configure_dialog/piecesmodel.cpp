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


#include "piecesmodel.h"

#include <QIcon>
#include <QMimeData>

PiecesModel::PiecesModel(dataconfigs* conf, int pieceSize, QObject *parent)
    : QAbstractListModel(parent), m_PieceSize(pieceSize), m_conf(conf)
{
}

QVariant PiecesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DecorationRole)
        return QIcon(pixmaps.value(index.row()).scaled(m_PieceSize, m_PieceSize,
                         Qt::KeepAspectRatio, Qt::SmoothTransformation));
    else if (role == Qt::DisplayRole)
        return pixmapNames.value(index.row());
    else if (role == Qt::UserRole)
        return pixmaps.value(index.row());

    return QVariant();
}

void PiecesModel::addPiece(const int &index)
{
    beginInsertRows(QModelIndex(), pixmaps.size(), pixmaps.size());
    pixmapNames.insert(pixmaps.size(), m_conf->main_block[index].name);
    pixmaps.insert(pixmaps.size(), (m_conf->main_block[index].frames == 1 ?
                                        m_conf->main_block[index].image :
                                        m_conf->main_block[index].image.copy(0,0,32,32)));
    endInsertRows();
}

Qt::ItemFlags PiecesModel::flags(const QModelIndex &index) const
{
    if (index.isValid())
        return (QAbstractListModel::flags(index)|Qt::ItemIsDragEnabled);

    return Qt::ItemIsDropEnabled;
}

bool PiecesModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (parent.isValid())
        return false;

    if (row >= pixmaps.size() || row + count <= 0)
        return false;

//    int beginRow = qMax(0, row);
//    int endRow = qMin(row + count - 1, pixmaps.size() - 1);

//    beginRemoveRows(parent, beginRow, endRow);

//    while (beginRow <= endRow) {
//        pixmaps.removeAt(beginRow);
//        pixmapNames.removeAt(beginRow);
//        ++beginRow;
//    }

//    endRemoveRows();
    return false;
}

QStringList PiecesModel::mimeTypes() const
{
    QStringList types;
    types << "image/x-pge-piece";
    return types;
}

QMimeData *PiecesModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    foreach (QModelIndex index, indexes) {
        if (index.isValid()) {
            QPixmap pixmap = qvariant_cast<QPixmap>(data(index, Qt::UserRole));
            //QPoint location = data(index, Qt::UserRole+1).toPoint();
            stream << pixmap/* << location*/;
        }
    }

    mimeData->setData("image/x-pge-piece", encodedData);
    return mimeData;
}

bool PiecesModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
                               int /*row*/, int column, const QModelIndex &parent)
{
    if (!data->hasFormat("image/x-pge-piece"))
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

int PiecesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    else
        return pixmaps.size();
}

Qt::DropActions PiecesModel::supportedDropActions() const
{
    return Qt::CopyAction | Qt::MoveAction;
}


