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

#ifndef DEBUGGER_STATS_MODEL_H
#define DEBUGGER_STATS_MODEL_H

#include <QAbstractTableModel>
#include <QString>
#include <QVector>

class MoondustBaseScene;

class DebuggerStatsModel : public QAbstractTableModel
{
    Q_OBJECT

    struct Entry
    {
        QString title;
        unsigned int count = 0;
    };

    QVector<Entry> m_items;
    int m_currentType = 0;

public:
    explicit DebuggerStatsModel(QObject *parent = nullptr);

    void re_translate();

    void refreshCounters(MoondustBaseScene *scene);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
};

#endif // DEBUGGER_STATS_MODEL_H
