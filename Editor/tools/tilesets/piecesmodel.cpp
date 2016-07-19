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
#include <QMimeData>

#include <common_features/graphics_funcs.h>
#include <common_features/items.h>

#include "piecesmodel.h"

PiecesModel::PiecesModel(dataconfigs* conf, PieceType pieceType, int pieceSize, QGraphicsScene *scene, QObject *parent)
    : QAbstractListModel(parent), m_PieceSize(pieceSize), m_conf(conf), m_type(pieceType)
{
    mode = GFX_Staff;
    scn = scene;
    if(scn != nullptr)
    {
        if(QString(scn->metaObject()->className())=="LvlScene") mode = GFX_Level;
        else
        if(QString(scn->metaObject()->className())=="WldScene") mode = GFX_World;
    }
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
    else if (role == Qt::UserRole + 1)
        return pixmapId.value(index.row());

    return QVariant();
}

void PiecesModel::addPiece(const int &index)
{
    beginInsertRows(QModelIndex(), pixmaps.size(), pixmaps.size());
    LvlScene * scene_lvl = dynamic_cast<LvlScene*>(scn);
    WldScene * scene_wld = dynamic_cast<WldScene*>(scn);

    if(m_type==LEVELPIECE_BLOCK)
    {
        obj_block& block = scene_lvl ? scene_lvl->m_localConfigBlocks[index] : m_conf->main_block[index];
        pixmapNames.insert(pixmaps.size(), block.name);
        QPixmap img;
        Items::getItemGFX(&block, img, false, QSize(32,32));
        pixmaps.insert(pixmaps.size(), img);
        pixmapId.insert(pixmaps.size(), block.id);
    }
    else
    if(m_type==LEVELPIECE_BGO)
    {
        obj_bgo& bgo = scene_lvl ? scene_lvl->m_localConfigBGOs[index] : m_conf->main_bgo[index];
        pixmapNames.insert(pixmaps.size(), bgo.name);
        QPixmap img;
        Items::getItemGFX(&bgo, img, false, QSize(32,32));
        pixmaps.insert(pixmaps.size(), img);
        pixmapId.insert(pixmaps.size(), bgo.id);
    }
    else
    if(m_type==LEVELPIECE_NPC)
    {
        obj_npc& npc = scene_lvl ? scene_lvl->m_localConfigNPCs[index] : m_conf->main_npc[index];
        pixmapNames.insert(pixmaps.size(), npc.name);
        QPixmap img;
        Items::getItemGFX(&npc, img, false, QSize(32,32));
        pixmaps.insert(pixmaps.size(), img);
        pixmapId.insert(pixmaps.size(), npc.id);
    }
    else
    if(m_type==WORLDPIECE_TILE)
    {
        obj_w_tile& tile = scene_wld ? scene_wld->uTiles[index] : m_conf->main_wtiles[index];
        pixmapNames.insert(pixmaps.size(), QString("%1").arg(index));
        QPixmap img;
        Items::getItemGFX(&tile, img, false, QSize(32,32));
        pixmaps.insert(pixmaps.size(), img);
        pixmapId.insert(pixmaps.size(), tile.id);
    }
    else
    if(m_type==WORLDPIECE_SCENERY)
    {
        obj_w_scenery& scenery = scene_wld ? scene_wld->uScenes[index] : m_conf->main_wscene[index];
        pixmapNames.insert(pixmaps.size(), QString("%1").arg(index));
        QPixmap img;
        Items::getItemGFX(&scenery, img, false, QSize(32,32));
        pixmaps.insert(pixmaps.size(), img);
        pixmapId.insert(pixmaps.size(), scenery.id);
    }
    else
    if(m_type==WORLDPIECE_PATH)
    {
        obj_w_path& wpath = scene_wld ? scene_wld->uPaths[index] : m_conf->main_wpaths[index];
        pixmapNames.insert(pixmaps.size(), QString("%1").arg(index));
        QPixmap img;
        Items::getItemGFX(&wpath, img, false, QSize(32,32));
        pixmaps.insert(pixmaps.size(), img);
        pixmapId.insert(pixmaps.size(), wpath.id);
    }
    else
    if(m_type==WORLDPIECE_LEVEL)
    {
        obj_w_level& wlevel = scene_wld ? scene_wld->uLevels[index] : m_conf->main_wlevels[index];
        pixmapNames.insert(pixmaps.size(), QString("%1").arg(index));
        QPixmap img;
        Items::getItemGFX(&wlevel, img, false, QSize(32,32));
        pixmaps.insert(pixmaps.size(), img);
        pixmapId.insert(pixmaps.size(), wlevel.id);
    }
    endInsertRows();
}

QString PiecesModel::getMimeType() const
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
    types << getMimeType().toStdString().c_str();
    return types;
}

QMimeData *PiecesModel::mimeData(const QModelIndexList &indexes) const
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

bool PiecesModel::dropMimeData(const QMimeData *data, Qt::DropAction action,
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


