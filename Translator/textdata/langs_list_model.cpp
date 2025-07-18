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
#include <QLocale>
#include <QSettings>
#include <QApplication>

#include "langs_list_model.h"

void LangsListModel::rebuildMap()
{
    m_viewByLang.clear();

    for(int i = 0; i < m_view.size(); ++i)
        m_viewByLang[m_view[i].code] = i;
}

LangsListModel::LangsListModel(TranslateProject *project, QSettings *setup, QObject *parent)
    : QAbstractTableModel(parent)
    , m_project(project)
    , m_setup(setup)
{
    Q_ASSERT(m_project);
    Q_ASSERT(m_setup);
}

bool LangsListModel::langIsVisible(const QString &langCode)
{
    if(m_viewByLang.contains(langCode))
        return m_view[m_viewByLang[langCode]].vis;

    return false;
}

static QString getStdLangName(const QString &fname)
{
    QString locale;
    locale = fname;                             // "TranslationExample_de.qm"
    locale = locale.replace('-', '_');
    return locale;
}

void LangsListModel::refreshData()
{
    beginResetModel();

    m_view.clear();
    m_viewByLang.clear();

    m_setup->beginGroup("LangVisibility");

    for(auto it = m_project->begin(); it != m_project->end(); ++it)
    {
        if(it.key() == "metadata")
            continue; // The "metadata" is a meta-entry, don't list it!

        auto &et = it.value();

        LangsView e;
        e.vis = m_setup->value(it.key(), false).toBool();
        e.strings = et.t_strings;
        e.translated = et.t_translated;
        e.code = it.key();

        QLocale loc(getStdLangName(it.key()));

        e.lang_name = QString("%1 (%2)")
                                .arg(loc.nativeLanguageName())
                                .arg(loc.nativeCountryName());
        if(!e.lang_name.isEmpty())
            e.lang_name[0] = e.lang_name[0].toUpper();

        m_viewByLang[e.code] = (int)m_view.size();
        m_view.push_back(e);
    }

    m_setup->endGroup();

    endResetModel();
}

void LangsListModel::clear()
{
    beginResetModel();
    m_view.clear();
    m_viewByLang.clear();
    endResetModel();
}

QVariant LangsListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && role != Qt::DisplayRole)
        return QVariant();

    switch(section)
    {
    case C_VISIBLE:
        return tr("Vis.");
    case C_TOTAL_STRINGS:
        return tr("Str.");
    case C_TOTAL_TRANSLATED:
        return tr("Tr.");
    case C_LANG_CODE:
        return tr("Code");
    case C_LANG_NAME:
        return tr("Language");
    }

    return QVariant();
}

void LangsListModel::sort(int column, Qt::SortOrder order)
{
    switch(column)
    {
    case C_VISIBLE:
        if(order == Qt::AscendingOrder)
        {
            std::sort(m_view.begin(), m_view.end(),
                      [](const LangsView&o1, const LangsView&o2)->bool
                      {
                          return o1.vis > o2.vis;
                      });
        }
        else
        {
            std::sort(m_view.begin(), m_view.end(),
                      [](const LangsView&o1, const LangsView&o2)->bool
                      {
                          return o1.vis < o2.vis;
                      });
        }

        emit dataChanged(QModelIndex(), QModelIndex());
        break;

    case C_TOTAL_STRINGS:
        if(order == Qt::AscendingOrder)
        {
            std::sort(m_view.begin(), m_view.end(),
                      [](const LangsView&o1, const LangsView&o2)->bool
                      {
                          return o1.strings > o2.strings;
                      });
        }
        else
        {
            std::sort(m_view.begin(), m_view.end(),
                      [](const LangsView&o1, const LangsView&o2)->bool
                      {
                          return o1.strings < o2.strings;
                      });
        }

        emit dataChanged(QModelIndex(), QModelIndex());
        break;

    case C_TOTAL_TRANSLATED:
        if(order == Qt::AscendingOrder)
        {
            std::sort(m_view.begin(), m_view.end(),
                      [](const LangsView&o1, const LangsView&o2)->bool
                      {
                          return o1.translated > o2.translated;
                      });
        }
        else
        {
            std::sort(m_view.begin(), m_view.end(),
                      [](const LangsView&o1, const LangsView&o2)->bool
                      {
                          return o1.translated < o2.translated;
                      });
        }

        emit dataChanged(QModelIndex(), QModelIndex());
        break;

    case C_LANG_CODE:
        if(order == Qt::AscendingOrder)
        {
            std::sort(m_view.begin(), m_view.end(),
                      [](const LangsView&o1, const LangsView&o2)->bool
                      {
                          return o1.code > o2.code;
                      });
        }
        else
        {
            std::sort(m_view.begin(), m_view.end(),
                      [](const LangsView&o1, const LangsView&o2)->bool
                      {
                          return o1.code < o2.code;
                      });
        }

        emit dataChanged(QModelIndex(), QModelIndex());
        break;

    case C_LANG_NAME:
        if(order == Qt::AscendingOrder)
        {
            std::sort(m_view.begin(), m_view.end(),
                      [](const LangsView&o1, const LangsView&o2)->bool
                      {
                          return o1.lang_name > o2.lang_name;
                      });
        }
        else
        {
            std::sort(m_view.begin(), m_view.end(),
                      [](const LangsView&o1, const LangsView&o2)->bool
                      {
                          return o1.lang_name < o2.lang_name;
                      });
        }

        emit dataChanged(QModelIndex(), QModelIndex());
        break;
    }

    rebuildMap();
}

int LangsListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_view.size();
}

int LangsListModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    // FIXME: Implement me!
    return (int)P_COLUMNS_COUNT;
}

QVariant LangsListModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid() || index.row() >= m_view.size())
        return QVariant();

    auto &it = m_view[index.row()];

    switch(role)
    {
    case Qt::DisplayRole:
        switch(index.column())
        {
        case C_VISIBLE:
            return it.vis ? Qt::Checked : Qt::Unchecked;
        case C_TOTAL_STRINGS:
            return it.strings;
        case C_TOTAL_TRANSLATED:
            return it.translated;
        case C_LANG_CODE:
            return it.code;
        case C_LANG_NAME:
            return it.lang_name;
        }

        break;

    case Qt::CheckStateRole:
        switch(index.column())
        {
        case C_VISIBLE:
            return it.vis ? Qt::Checked : Qt::Unchecked;
        }

        break;

    case Qt::ForegroundRole:
        switch(index.column())
        {
        case C_LANG_NAME:
            return it.strings == it.translated ? QColor(Qt::green) : QApplication::palette().text().color();
        }
        break;
    }

    return QVariant();
}

bool LangsListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid() || index.row() >= m_view.size())
        return false;

    auto &it = m_view[index.row()];

    if(role == Qt::CheckStateRole)
    {
        switch(index.column())
        {
        case C_VISIBLE:
            it.vis = (value.toInt() == Qt::Checked);
            emit dataChanged(index, index, {role});
            emit visibilityChanged();
            m_setup->beginGroup("LangVisibility");
            m_setup->setValue(it.code, it.vis);
            m_setup->endArray();
            m_setup->sync();
            break;

        default:
            break;
        }
    }

    return false;
}

Qt::ItemFlags LangsListModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return QAbstractItemModel::flags(index);

    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

    switch(index.column())
    {
    case C_VISIBLE:
        flags |= Qt::ItemIsUserCheckable;
        break;
    default:
        break;
    }

    return flags;
}
