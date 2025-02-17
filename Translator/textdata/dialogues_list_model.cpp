/*
 * Moondust Translator, a free tool for internationalisation of levels and episodes
 * This is a part of the Moondust Project, a free platform for game making
 * Copyright (c) 2023-2025 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QColor>
#include <QPalette>
#include <QApplication>
#include <QtDebug>

#include "dialogues_list_model.h"


DialoguesListModel::DialoguesListModel(TranslateProject *project, QObject *parent)
    : QAbstractTableModel(parent)
    , m_project(project)
{}

void DialoguesListModel::setData(const QString &lang, const QString &level)
{
    beginResetModel();
    m_recentLang = lang;
    m_levelKey = level;
    m_level = &(*m_project)[TRANSLATE_METADATA].levels[level];
    m_dialogues = &m_level->dialogues;
    m_view.clear();

    qDebug() << "Dialogues view: adding" << m_dialogues->size() << "dialogue(s) to the list...";

    for(int i = 0; i < m_dialogues->size(); ++i)
    {
        auto &d = (*m_dialogues)[i];
        TrView e;
        e.i = i;
        e.count = d.messages.size() - 1;
        e.note = d.note;
        m_view.push_back(e);
    }

    endResetModel();
}

void DialoguesListModel::clear()
{
    beginResetModel();
    m_levelKey.clear();
    m_level = nullptr;
    m_dialogues = nullptr;
    m_view.clear();
    endResetModel();
}

TranslationData_Level &DialoguesListModel::level() const
{
    return *m_level;
}

QString DialoguesListModel::levelKey() const
{
    return m_levelKey;
}

QVariant DialoguesListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && role != Qt::DisplayRole)
        return QVariant();

    switch(section)
    {
    case C_INDEX:
        return tr("No");
    case C_NUMBER_ITEMS:
        return tr("Count");
    case C_TITLE:
        return tr("Title");
    }

    return QVariant();
}

int DialoguesListModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;

    return m_view.size();
}

int DialoguesListModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;

    return C_COUNT;
}

QVariant DialoguesListModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || index.row() >= m_view.size())
        return QVariant();

    auto &it = m_view[index.row()];

    switch(index.column())
    {
    case C_INDEX:
        if(role == Qt::DisplayRole)
            return it.i;
        break;
    case C_NUMBER_ITEMS:
        if(role == Qt::DisplayRole)
            return it.count;
        break;
    case C_TITLE:
        switch(role)
        {
        case Qt::DisplayRole:
            return it.note.isEmpty() ? tr("<Double-click to edit note>") : it.note;
        case Qt::ForegroundRole:
            return it.note.isEmpty() ? QColor(Qt::gray) : QApplication::palette().text().color();
        case Qt::EditRole:
            return it.note;
        }
        break;
    }

    return QVariant();
}

bool DialoguesListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(data(index, role) != value && index.column() == C_TITLE)
    {
        auto &d = m_view[index.row()];
        d.note = value.toString();
        (*m_dialogues)[d.i].note = d.note;
        emit dataChanged(index, index, {role});
        return true;
    }

    return false;
}

Qt::ItemFlags DialoguesListModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return Qt::NoItemFlags;

    switch(index.column())
    {
    case C_TITLE:
        return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
    default:
        return QAbstractItemModel::flags(index);
    }
}
