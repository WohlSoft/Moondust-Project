/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2026 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "debugger_stats_model.h"
#include <editing/_scenes/level/lvl_scene.h>
#include <editing/_scenes/world/wld_scene.h>

DebuggerStatsModel::DebuggerStatsModel(QObject *parent)
: QAbstractTableModel(parent)
{}

void DebuggerStatsModel::re_translate()
{
    beginResetModel();

    switch(m_currentType)
    {
    case MoondustBaseScene::SCENE_LEVEL:
    {
        if(m_items.size() != 6)
            m_items.resize(6);

        m_items[0].title = tr("Player start points");
        m_items[1].title = tr("Blocks");
        m_items[2].title = tr("Background objects");
        m_items[3].title = tr("Non-playable characters/items");
        m_items[4].title = tr("Warp entries");
        m_items[5].title = tr("Physical env. zones");
        break;
    }

    case MoondustBaseScene::SCENE_WORLD:
    {
        if(m_items.size() != 5)
            m_items.resize(5);

        m_items[0].title = tr("Terrain tiles");
        m_items[1].title = tr("Scenery");
        m_items[2].title = tr("Path cells");
        m_items[3].title = tr("Level entrances");
        m_items[4].title = tr("Music change boxes");
        break;
    }
    }

    endResetModel();
}

void DebuggerStatsModel::refreshCounters(MoondustBaseScene *scene)
{
    switch(scene->sceneType())
    {
    case MoondustBaseScene::SCENE_LEVEL:
    {
        LvlScene *sc = dynamic_cast<LvlScene*>(scene);
        if(m_currentType != MoondustBaseScene::SCENE_LEVEL)
        {
            m_currentType = MoondustBaseScene::SCENE_LEVEL;
            re_translate();
        }

        m_items[0].count = sc->m_itemsPlayers.size();
        m_items[1].count = sc->m_itemsBlocks.size();
        m_items[2].count = sc->m_itemsBGO.size();
        m_items[3].count = sc->m_itemsNPC.size();
        m_items[4].count = sc->m_data->doors.size();
        m_items[5].count = sc->m_itemsPhysEnv.size();

        emit dataChanged(createIndex(0, 1), createIndex(5, 1), {Qt::DisplayRole});
        break;
    }

    case MoondustBaseScene::SCENE_WORLD:
    {
        WldScene *sc = dynamic_cast<WldScene*>(scene);
        if(m_currentType != MoondustBaseScene::SCENE_WORLD)
        {
            m_currentType = MoondustBaseScene::SCENE_WORLD;
            re_translate();
        }

        m_items[0].count = sc->m_itemsTiles.size();
        m_items[1].count = sc->m_itemsScenery.size();
        m_items[2].count = sc->m_itemsPaths.size();
        m_items[3].count = sc->m_itemsLevels.size();
        m_items[4].count = sc->m_itemsMusicBoxes.size();

        emit dataChanged(createIndex(0, 1), createIndex(4, 1), {Qt::DisplayRole});
        break;
    }

    default:
        break;
    }
}

QVariant DebuggerStatsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch(orientation)
    {
    case Qt::Horizontal:
        if(role == Qt::DisplayRole)
        {
            switch(section)
            {
            case 0:
                return tr("Item type");
            case 1:
                return tr("Count");
            }
        }
        break;

    case Qt::Vertical:
        break;
    }

    return QVariant();
}

int DebuggerStatsModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;

    return m_items.size();
}

int DebuggerStatsModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;

    return 2;
}

QVariant DebuggerStatsModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(index.column() >= 2 || index.row() >= m_items.size())
        return QVariant();

    switch(role)
    {
    case Qt::DisplayRole:
        switch(index.column())
        {
        case 0:
            return m_items[index.row()].title;
        case 1:
            return m_items[index.row()].count;
        }
        break;

    case Qt::TextAlignmentRole:
        switch(index.column())
        {
        case 0:
            return (int)Qt::AlignLeft | Qt::AlignVCenter;
        case 1:
            return (int)Qt::AlignLeft | Qt::AlignVCenter;
        default:
            return (int)Qt::AlignLeft | Qt::AlignVCenter;
        }
        break;
    }

    return QVariant();
}
