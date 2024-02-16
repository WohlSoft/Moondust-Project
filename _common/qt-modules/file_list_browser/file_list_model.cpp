/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2024 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QFileIconProvider>
#include <QRegExp>

#include "file_list_model.h"



FileListModel::FileListModel(QObject *parent) :
    QAbstractListModel(parent),
    m_iconProv(new QFileIconProvider)
{}

FileListModel::~FileListModel()
{
    m_iconProv.reset();
}

int FileListModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return m_entriesMap.size();
}

QVariant FileListModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(index.row() < 0 || index.row() > m_entriesMap.size())
        return QVariant();

    switch(role)
    {
    case Qt::DisplayRole:
        return m_entries[m_entriesMap[index.row()]];

    case Qt::DecorationRole:
        return m_iconProv->icon(QFileInfo(m_rootDirectory + m_entries[m_entriesMap[index.row()]]));

    default:
        break;
    }

    return QVariant();
}

Qt::ItemFlags FileListModel::flags(const QModelIndex& index) const
{
    if(index.isValid())
        return (QAbstractListModel::flags(index) | Qt::ItemIsEnabled | Qt::ItemIsSelectable);

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void FileListModel::setFilter(const QString filter)
{
    m_filter = filter;
    updateVisibility();
}

void FileListModel::clearList()
{
    if(m_entries.empty())
        return; //Do nothing, already clean

    beginRemoveRows(QModelIndex(), 0, std::max(rowCount() - 1, 0));
    m_entries.clear();
    m_entriesMap.clear();
    endRemoveRows();
}

void FileListModel::addEntry(const QString& fileName)
{
    int last_i = m_entries.size();
    m_entries.push_back(fileName);

    if(filterMatch(fileName))
    {
        beginInsertRows(QModelIndex(), m_entriesMap.size(), m_entriesMap.size() + 1);
        m_entriesMap.push_back(last_i);
        endInsertRows();
    }
}

void FileListModel::sortData()
{
    beginResetModel();
    std::sort(m_entries.begin(), m_entries.end(),
    [](const QString &a, const QString &b)
    {
        return a.compare(b, Qt::CaseInsensitive) < 0;
    });
    endResetModel();
}

void FileListModel::setDirectory(const QString &dir)
{
    m_rootDirectory = dir;
    if(!m_rootDirectory.endsWith('/'))
        m_rootDirectory.append('/');
}

void FileListModel::updateVisibility()
{
    beginResetModel();
    m_entriesMap.clear();
    m_entriesMap.reserve(m_entries.size());

    for(int i = 0; i < m_entries.size(); ++i)
    {
        if(filterMatch(m_entries[i]))
            m_entriesMap.push_back(i);
    }
    endResetModel();
}

bool FileListModel::filterMatch(const QString &file) const
{
    if(m_filter.isEmpty())
        return true;

    if(m_filter.contains('*'))
    {
        QRegExp cond(m_filter, Qt::CaseInsensitive, QRegExp::Wildcard);
        return cond.exactMatch(file);
    }

    return file.contains(m_filter, Qt::CaseInsensitive);
}
